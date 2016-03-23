#include "ArcgisMapLayer.h"
#include "DataStruct.h"

//extern int g_CtrlW;
//extern int g_CtrlH;
//extern double g_scale;

CArcgisMapLayer::CArcgisMapLayer(void)
{
}


CArcgisMapLayer::~CArcgisMapLayer(void)
{
	if (m_LayerDataList.size() != 0)
	{

		map<long, MapLayerData*>::iterator it = m_LayerDataList.begin();
		for (; it != m_LayerDataList.end(); it++)
		{
			if ((*it).second)
			{
				delete (*it).second;
				(*it).second = NULL;
			}
		}

		m_LayerDataList.clear();
	}
	
}

/*
 str = '{"CmdName":"OPEN","MapName":"ningbo_vector","CenX":120.1,"CenY":30.2,"Level":9,
	 "URL":"d:/map2d/map/ningbo_vector","MapType":"ArcGisMap","Format":"jpg","Cmdid":19}';
*/
//在这里 可以设置 创建缓存远程 图片的 目录
CArcgisMapLayer::CArcgisMapLayer(double cenX,double cenY,int js,CString mapname,int picW,int picH,CString path,CString pictype,DrawEngine *den)
{
	m_x = cenX;//中心点
	m_y = cenY;
	m_js = js; //当前地图显示级别
	m_PicW = picW;//切片宽高是一致的
	m_PicH = picH;
	m_Lon = 0.0;
	m_Lat = 0.0;
	m_MapName = mapname;//地图名
	m_Scale2ImageW = -1;
	m_Scale2ImageW = -1;
	m_resolution = 0.0; // 不同Level的级别的地图应该
	m_MapType = MapType::ArcgisMap; //地图类型
	m_layerType=VLayerType::vBaseLayer; //图层类型
	m_TitleType = pictype;  //瓦片类型

	g_CtrlW = den->g_CtrlW;  //保存组件创建窗口的大小
	g_CtrlH = den->g_CtrlH;
	//重新设计这个m_sPath
	//m_sPath = path;//地图路径，如果是服务发布的数据，则为URL
	CString serverurl = path;
	USES_CONVERSION; 
	//m_mapServerUrl = W2A(serverurl.GetBuffer(0));
	serverurl.ReleaseBuffer();
	int pos = path.Find(L"http:");
	if ( pos >= 0)
	{
		m_localornet = false;
	}
	else   // 这里的代码逻辑表示：如果地图本来是local就不需要 使用构造的本地路径，直接用path
	{
		m_localornet = true;   //本地图片
		m_sPath = path;     // 该路径始终指向 磁盘上路径，如果path是URL，那么它将指向我们[构造出来的本地路径]
	}
	m_bUpdateData = true; //控制重新的拿地图数据 ,在拿完数据后GetData该bool变量 会被置为false
	
	if(!m_localornet)  //不是本地路径，先创建 地图在磁盘的root目录，并将conf.xml文件下载到本地
	{
		CString szExePath = L"";
		//得到本模块的工作路径，浏览器的位置
		::GetModuleFileName(NULL,szExePath.GetBufferSetLength(MAX_PATH+1),MAX_PATH); 
		//反向查找
		szExePath = szExePath.Left(szExePath.ReverseFind('\\'));
		szExePath = szExePath.Left(szExePath.ReverseFind('\\'));

		CString szMap = L"";
		CString url(m_MapName);
		//szMap.Format(L"%s\\MAP\\%s\\Layers\\_allayers",szExePath,url);//m_LayerName
		szMap.Format(L"%s\\MAP",szExePath);// 构造本地存放图片的路径  C:\SmartCity\MAP
		bool bc = false;
		int err;
		if(!PathIsDirectory(szMap))
		{
			bc = CreateDirectory(szMap,NULL);   //如果不存在则创建该目录
			err = GetLastError();
		}
		szExePath.Format(L"%s\\%s", szMap,url);  //  MAP/ningbo_vector
		if(!PathIsDirectory(szExePath))
		{
			bc = CreateDirectory(szExePath,NULL);
			err = GetLastError();
		}
		szMap = szExePath;
		CString szMapPath = L"";
		szMapPath.Format(L"%s\\Layers",szMap);
		if(!PathIsDirectory(szMapPath))
		{
			bc = CreateDirectory(szMapPath,NULL);
			err = GetLastError();
		}

		szExePath = szMapPath;
		szMapPath.Format(L"%s\\_allayers",szExePath);
		if(!PathIsDirectory(szMapPath))
		{
			bc = CreateDirectory(szMapPath,NULL);
			err = GetLastError();
		}
		m_sPath = szMapPath;  //磁盘路径 D:\Chrome\MAP\ningbo_vector\Layers\_allayers
	}

}

/*
*param[in] strXmlPath:xml路径
*param[in] djs:地图级数
*param[in，out] lfTileOriginValueX:x
*param[in，out] lfTileOriginValueY:y
*param[in，out] dTileCols:切片宽
*param[in，out] dTileRows:切片高
*param[in，out] dDPI:每英寸像素
*param[in，out] lfResolution:分辨率
*param[in，out] lScale:比例尺
*/

//overload这个函数的 ，使它从JSON中获取信息
//解析地图的配置文件 conf.xml,前面两个参数是作为函数的输入，后面作为读取的 [配置信息] 作为输出
void CArcgisMapLayer::GetMapConfigInfo(std::string strPath,int djs,double &lfTileOriginX,double &lfTileOriginY,int &dTileCols,int &dTileRows,int &dDPI,double &lfResolution,double &dscale)
{
	std::string strxml = strPath + "\\Layers\\conf.xml";
	std::string strjson = strPath + "\\Layers\\conf.json";
	if (PathFileExistsA(strxml.c_str()))
	{
		TiXmlDocument *pDoc = new TiXmlDocument();
		if (NULL == pDoc)
		{
			return;
		}

		if (!pDoc->LoadFile(strxml))//图层xml路径，从本地文件中 加载地图配置文件
		{
			return;
		}

		TiXmlElement* pRootElet = pDoc->RootElement();

		//获取地图的基本信息
		//TileOrigin : X Y
		TiXmlElement* TileOriginNode = NULL;
		std::string strTileOrigin = "TileOrigin";
		m_xmlwork.GetNodePointerByName(pRootElet, strTileOrigin, TileOriginNode);
		const char *TileOriginValueX = TileOriginNode->FirstChildElement()->FirstChild()->Value();
		const char *TileOriginValueY = TileOriginNode->FirstChildElement()->NextSiblingElement()->FirstChild()->Value();
		lfTileOriginX = atof(TileOriginValueX);
		lfTileOriginY = atof(TileOriginValueY);
		strTileOrigin.~basic_string();

		//TileCols
		TiXmlElement *TileColsNode = NULL;
		std::string strTileCols = "TileCols";
		m_xmlwork.GetNodePointerByName(pRootElet, strTileCols, TileColsNode);
		const char *cTileCols = TileColsNode->FirstChild()->Value();
		dTileCols = atoi(cTileCols);
		strTileCols.~basic_string();

		//TileRows
		TiXmlElement *TileRowsNode = NULL;
		std::string strTileRows = "TileRows";
		m_xmlwork.GetNodePointerByName(pRootElet, strTileRows, TileRowsNode);
		const char *cTileRows = TileRowsNode->FirstChild()->Value();
		dTileRows = atoi(cTileRows);
		strTileRows.~basic_string();

		//DPI
		TiXmlElement *DPINode = NULL;
		std::string strDPI = "DPI";
		m_xmlwork.GetNodePointerByName(pRootElet, strDPI, DPINode);
		const char *cDPI = DPINode->FirstChild()->Value();
		dDPI = atoi(cDPI);
		strDPI.~basic_string();


		//获取对应级别的对应信息：scale Resolution
		TiXmlElement* node;
		string nodeName = "LODInfos";
		m_xmlwork.GetNodePointerByName(pRootElet, nodeName, node);
		TiXmlAttribute* nodeAttribute = node->FirstAttribute();
		const char *value = nodeAttribute->Value();//测试
		nodeName.~basic_string();

		string LayerNode = "";
		lfResolution = 0.0;
		int lScale = 0;
		for (TiXmlElement * i = node->FirstChildElement(); i != NULL; i = i->NextSiblingElement())
		{
			TiXmlElement *node = NULL;
			std::string strlevelid = "LevelID";
			m_xmlwork.GetNodePointerByName(i, strlevelid, node);
			assert(node != NULL);
			const char * value = node->FirstChild()->Value();
			int dlevelid = atoi(value);
			if (dlevelid == djs)
			{
				TiXmlElement *dscNode = NULL;
				//定位到xml中相应级数
				std::string strResolution = "Resolution";
				std::string strScale = "Scale";

				m_xmlwork.GetNodePointerByName(i, strResolution, dscNode);
				const char *cResolution = dscNode->FirstChild()->Value();

				m_xmlwork.GetNodePointerByName(i, strScale, dscNode);
				const char *cScale = dscNode->FirstChild()->Value();

				lfResolution = atof(cResolution);
				dscale = atof(cScale);

				break;
			}
		}
		pDoc->~TiXmlDocument(); // 显示的invoke析构函数
		delete pDoc;
	}
	else if (PathFileExistsA(strjson.c_str()))
	{
		//map<long, MapLayerData*>::iterator itend 
		/*if (m_LayerDataList.find(djs)!=m_LayerDataList.end())
		{
		
		}*/
		//else
		{
			Json::Reader reader;
			Json::Value root;
			ifstream fjson;
			fjson.open(strjson.c_str(), ios::binary);

			if (reader.parse(fjson, root))  // reader将Json字符串解析到root，root将包含Json里所有子元素   
			{
				//获取切片大小
				dTileRows = root["tileInfo"]["rows"].asInt();
				dTileCols = root["tileInfo"]["cols"].asInt();

				lfTileOriginX = root["tileInfo"]["origin"]["x"].asInt();;
				lfTileOriginY = root["tileInfo"]["origin"]["y"].asInt();

				Json::Value plods = root["tileInfo"]["lods"];
				int pSize = plods.size(); //地图共有多少层 Level
				dDPI = root["tileInfo"]["dpi"].asInt();
				string format = root["tileInfo"]["format"].asString();
				if (format.compare("PNG32") == 0)
				{
					m_TitleType = L"png";
				}

				// 遍历数组  
				for (int i = 0; i < pSize; ++i)
				{
					int  plevel = plods[i]["level"].asInt();
					if (plevel==djs)
					{
						lfResolution = plods[i]["resolution"].asDouble();
						dscale = plods[i]["scale"].asDouble();
						break;
					}

					//double  presolution = plods[i]["resolution"].asDouble();
					//int  pscale = plods[i]["scale"].asInt();
					////不同Level的地图信息，最关键的就是 分辨率
					//MapLayerData* pLayerData = new MapLayerData();
					//pLayerData->Level = plevel;
					//pLayerData->Resolution = presolution;
					//pLayerData->Scale = pscale;
					//m_LayerDataList[plevel] = pLayerData;
				}
			}
			fjson.close();
		}
		
	}
}

vector<Tile*> CArcgisMapLayer::GetData(int js,double cenlon,double cenlat,double WScreen,double HScreen,long& ditX,long& ditY,long& Wcount,long & Hcount,int g_context)
{	
	vector<Tile*> LayerTileVector; //用于结果返回，表示当前窗口中，正在显示的图片
	m_tmpData.clear();  //用于存放，地图刷新后，重新拿数据，处于当前窗口中需要显示的图片

	double lon = 0.0;
	double lat = 0.0 ;
	double width = 0.0;
	double height = 0.0;
	double letflon = 0.0 ;
	double Toplat = 0.0 ;
	double scale = 0.0;

	int ndpi;
	double dresolution = 0.0;
	USES_CONVERSION; 
	string MapPath = W2A(m_sPath.GetBuffer(0));


	//前两个参数，是作为输入，后面的参数作为函数的输出，
	/*
	  函数的作用是从地图的配置文件 conf.xml中 读取地图层级为js的  最左边顶点的[经纬度] 及 [该层别的分辨率]
	*/
	//GetMapConfigInfo(MapPath + "\\Layers\\conf.xml",js,letflon,Toplat,m_PicH,m_PicW,ndpi,dresolution,scale);  //前两个参数 是用户传入的 输入值
	GetMapConfigInfo(MapPath, js, letflon, Toplat, m_PicH, m_PicW, ndpi, dresolution, scale);  //前两个参数 是用户传入的 输入值

	m_sPath.ReleaseBuffer();
	
	//g_scale = scale;
	
	//中心点对应的行列:行=1表示R00000001,即第二行，列类似
	double tileWidth = m_PicW * dresolution;
	double tColCenter = (int)floor((cenlon - letflon) / tileWidth);
	lon = letflon + tColCenter*tileWidth;
	double tileHeight = m_PicH * dresolution;
	double tRowCenter = (int)floor((Toplat-cenlat) / tileHeight);
	lat = Toplat - tRowCenter*tileHeight;

	Wcount = WScreen / m_PicW ;
	Hcount = HScreen / m_PicH; 

	double picsizeW = (double)m_PicW;
	double picsizeH = (double)m_PicH;
	double pixX = (cenlon - letflon)/dresolution;
	double pixY = ((Toplat - cenlat)/ dresolution);

	int startx =(pixX-WScreen/2)/picsizeW;//lie
	int endx = (pixX+WScreen/2)/picsizeW;
	int starty = (pixY-HScreen/2)/picsizeH; //hang
	int endy = (pixY+HScreen/2)/picsizeH;

	m_areashow[0] = starty>0?starty:0;//hang
	m_areashow[1] = endy>0?endy:0;
	m_areashow[2] = startx>0?startx:0;//lie
	m_areashow[3] = endx>0?endx:0;

	int xmin = m_areashow[2];
	int ymin = m_areashow[0];
	double  xC= xmin*picsizeW;//通过获取的第一张图片的左上角坐标与控件左上角坐标进行差值获取偏移
	ditX = (pixX-WScreen/2) - xC;
	double  yc= ymin*picsizeH;
	ditY = (pixY-HScreen/2) - yc;
	//i表示列数，j表示行数
	CString strJs;
	string Path = m_sPath + "\\Layers\\_alllayers\\";
	CString strdir;
	strdir	=LPCSTR(Path.c_str());//目录
	strJs.Format(L"%sL%02d\\",strdir,js);

	for (int j = m_areashow[0]; j<=m_areashow[1] ;j++)
	{
		for(int i = m_areashow[2]; i<=m_areashow[3] ;i++)
		{	
			
			CString strRow = L"";
			strRow.Format(L"R%08x\\",j);
			CString strCol = L"";
			strCol.Format(L"C%08x",i);
			
			CString pStrFullPath = strJs + strRow + strCol + "." +  m_TitleType;//图片格式jpg

			Vertex verts[4] = {}; //将数组直接初始化为空
			verts[0].tu = 0;
			verts[0].tv = 0;
			verts[0].loc[0] = (i-xmin)*m_PicW-ditX;
			verts[0].loc[1] = (j-ymin)*m_PicH-ditY;
			verts[0].loc[2] = 0;
			verts[0].color[0] = 255;
			verts[0].color[1] = 0;
			verts[0].color[2] = 0;
			verts[3].color[3] = 200;

			verts[1].tu = 1;
			verts[1].tv = 0;
			verts[1].loc[0] = (i-xmin)*m_PicW-ditX + m_PicW;
			verts[1].loc[1] = (j-ymin)*m_PicH-ditY;
			verts[1].loc[2] = 0;
			verts[1].color[0] = 100;
			verts[1].color[1] = 0;
			verts[1].color[2] = 0;
			verts[3].color[3] = 0;

			verts[2].tu = 1;
			verts[2].tv = 1;
			verts[2].loc[0] = (i-xmin)*m_PicW-ditX + m_PicW;
			verts[2].loc[1] = (j-ymin)*m_PicH-ditY + m_PicH;
			verts[2].loc[2] = 0;
			verts[2].color[0] = 100;
			verts[2].color[1] = 0;
			verts[2].color[2] = 0;
			verts[3].color[3] = 0;

			verts[3].tu = 0;
			verts[3].tv = 1;
			verts[3].loc[0] = (i-xmin)*m_PicW-ditX;
			verts[3].loc[1] = (j-ymin)*m_PicH-ditY + m_PicH;
			verts[3].loc[2] = 0;
			verts[3].color[0] =255;
			verts[3].color[1] = 0;
			verts[3].color[2] = 0;
			verts[3].color[3] = 200;


			//判断内存缓冲池中是否有该图片
			Tile* tile = NULL;
			long key =js * 10000 + (i ) * 100 + j;
			map<long,Tile*>::iterator It;
			It = m_Cachemap.find(key);

			if(It != m_Cachemap.end())
			{
				tile = It->second;
				tile->SetVertex(verts);
			}
			else
			{
				if (PathFileExists(pStrFullPath)) //验证图片是否存在
				{
					tile = new Tile(pStrFullPath,verts,m_PicW,m_PicH,m_TitleType,js,j,i);  //invoke 下面的decode程序时，才会将图片读入内存
					tile->m_sformat = GL_RGB;
					m_tmpData[key] = tile;
				}				
			}
			LayerTileVector.push_back(tile);
			
		}
	}

	int datasize = m_tmpData.size();
	
	if (datasize>0)  //解码的过程包括将 图片读入内存，并解析为bitmap
	{
		//多线程解码jpg		
		if (datasize>=6)
		{
			DecodeTile();
		}
		else
		{
			//主线程解码
			decodetileSingleT();
		}

		//解码以后，ini中进行纹理贴图，最后保存到m_Cachemap中
		map<long,Tile*>::iterator it = m_tmpData.begin();
		for(;it!=m_tmpData.end();it++)
		{
			Tile*tile = (*it).second;
			int key = (*it).first;
			tile->init(g_context); //设置好纹理状态，并输入标识DrawEngine的g_context
			delete m_Cachemap[key];
			m_Cachemap[key] = tile;
		}
	}
	//int map_buff_size = 
	//控制缓存大小，删除tile的机制，一张张的删除效率不高
	if (m_Cachemap.size()>1000)//要删除哪张图片,需要
	{
		map<long, Tile*>::iterator it = m_Cachemap.begin();
		for (;it!=m_Cachemap.end();it++)
		{
			long tmpkey = (*it).first;
			Tile* tile = (*it).second;
			if (tile!=NULL)//如果该对象不是当前要显示的则可以删除
			{
				//if (tile->m_js != js || (tile->m_rowid<m_areashow[0])||(tile->m_rowid>m_areashow[1])
				//	|| (tile->m_colid<m_areashow[2])||(tile->m_colid>m_areashow[3]))
				if (tile->m_js != js || (tile->m_rowid<m_areashow[0])||(tile->m_rowid>m_areashow[1])   
				    || (tile->m_colid<m_areashow[2])||(tile->m_colid>m_areashow[3]))
				{
					delete tile;
					tile = NULL;
					m_Cachemap.erase(it);
					if (m_Cachemap.size() < 600)
					{
						break;
					}
					else
					{
						it = m_Cachemap.begin();
					}
				}
								
			}
		}
	}
	m_CenLon = cenlon;
	m_CenLat = cenlat;
	m_LeftTop_Lon = letflon;
	m_LeftTop_Lat = Toplat;
	m_Lon = m_x + (g_CtrlW/2)*dresolution - m_LeftTop_Lon;
	m_Lat = m_y - (g_CtrlH/2)*dresolution - m_LeftTop_Lat;
	m_Scale = scale;
	m_resolution = dresolution;

	return LayerTileVector;
}


void CArcgisMapLayer::DrawLayer(vector<Tile*> &vctTile,int ditx,int dity,bool m_realtime)
{
	int index = 0;
	//关闭日志输出
	//gLog.off();
	//CString curTileInfo;
	//CString info;
	for (int i = m_areashow[0];i<= m_areashow[1] ;i++)
	{
		for (int j = m_areashow[2];j<= m_areashow[3];j++)
		{
			Tile* p = vctTile.at(index);
			if (p==NULL)
			{
				index++;
				continue;
			}
			p->RenderUseOgles();
			index++;
			//info.Format(_T("%d  "),p->m_textureID);
			//curTileInfo += info;
			//info.Empty();
		}
		//gLog.append(curTileInfo);
		//curTileInfo.Empty(); //清空
	}
	//gLog.append(_T("############################################################"));
	return;
}

//底图的主绘制函数逻辑：先取数据---再绘图
void CArcgisMapLayer::Draw()
{
	long ditx = 0;  //无意义 应该是256
    long dity = 0;
	long wCount = 0;
	long hCount = 0;
	
	if (m_bUpdateData)
	{
		m_pTile = GetData(m_js,m_x,m_y,g_CtrlW,g_CtrlH,ditx,dity,wCount,hCount);  //先去 取数据
		m_bUpdateData = false;
	}
	DrawLayer(m_pTile,ditx,dity,true);

	return;
}
//测试使用
void CArcgisMapLayer::Draw(int g_context)
{
	long ditx = 0;  //无意义 应该是256
	long dity = 0;
	long wCount = 0;
	long hCount = 0;

	if (m_bUpdateData)
	{
		m_pTile = GetData(m_js,m_x,m_y,g_CtrlW,g_CtrlH,ditx,dity,wCount,hCount,g_context);  //先去 取数据
		m_bUpdateData = false;
	}
	DrawLayer(m_pTile,ditx,dity,true);

	return;
}
//读取地图的 成员变量
void CArcgisMapLayer::GetBaseLayerConf(double &Cenlon,double &Cenlat,double & width,double & height
	,double & leftlon,double & Toplat,double& lon,double &lat,double & scale,CString & Path,double &resolution)
{
	Cenlon = m_CenLon;
	Cenlat = m_CenLat;
	width = m_Scale2ImageW;
	height = m_Scale2ImageH;
	leftlon = m_LeftTop_Lon;
	Toplat = m_LeftTop_Lat;
	lon = m_Lon;
	lat  = m_Lat;
	scale = m_Scale;
	Path = m_MapName;
	resolution = m_resolution; //当前分辨率
};

//多线程解码
void CArcgisMapLayer::DecodeTile()
{
	// TODO: 在此添加控件通知处理程序代码
	DWORD start = GetTickCount();
	HANDLE hdl[g_countT];
	hdl[0]=CreateThread(NULL,NULL,&Datathread1,  (LPVOID)this,NULL,NULL);
	hdl[1]=CreateThread(NULL,NULL,&Datathread2,  (LPVOID)this,NULL,NULL);
	hdl[2]=CreateThread(NULL,NULL,&Datathread3,  (LPVOID)this,NULL,NULL);
	hdl[3]=CreateThread(NULL,NULL,&Datathread4,  (LPVOID)this,NULL,NULL);
	hdl[4]=CreateThread(NULL,NULL,&Datathread5,  (LPVOID)this,NULL,NULL);
	hdl[5]=CreateThread(NULL,NULL,&Datathread6,  (LPVOID)this,NULL,NULL);
	Sleep(0);
	WaitForMultipleObjects(g_countT,hdl,TRUE,INFINITE); //等待所有的 线程完成
	DWORD end = GetTickCount() - start;
}

//这6个thread_routine 可以简化为两个，给每个thread_routine传递一个ID值，标识它们处理的范围
DWORD WINAPI  CArcgisMapLayer::Datathread1(LPVOID t)
{
	CArcgisMapLayer *object = (CArcgisMapLayer*)t;
	CString m_tileName;

	map<long,Tile*>::iterator it = (object->m_tmpData).begin();
	int id = 0;

	for(;it!=(object->m_tmpData).end();it++)
	{	
		id++;
		if (id > (object->m_tmpData).size()/g_countT )
		{
			break;
		}
		Tile *tile = (*it).second;
		int w = tile->m_TileW;
		int h = tile->m_TileH;
		CImage image;
		HBITMAP hBitMap;  //位图句柄
		HRESULT hr = image.Load(tile->m_tileName);
		if (hr == E_FAIL)
		{
			tile->m_pdata = NULL;
			image.Destroy();
			continue;
		}
		int dep = image.GetBPP()/8;
		BYTE *rgbdata = NULL;

		if (dep==1||dep==3)
		{
			tile->m_sformat=GL_RGB;
			rgbdata = new BYTE[w*h*3];
		}
		else if (dep==4)
		{
			tile->m_sformat = GL_RGBA;
			rgbdata = new BYTE[w*h*4];
		}

	    if (image.IsIndexed())
	    {
			RGBQUAD prgbColors[256];
			image.GetColorTable(
				0,
				256,
				prgbColors 
				);

			hBitMap=image.Detach();

			BYTE *pdata = new BYTE[w*h*dep];
			CBitmap cbmp;
			cbmp.Attach(hBitMap);
			DWORD size = cbmp.GetBitmapBits(w*h*dep,pdata);

			for (int i=0;i<w*h;i++)//颜色索引转rgb
			{	
				int id = pdata[i];
				rgbdata[i*3+0] = prgbColors[id].rgbRed; //r
				rgbdata[i*3+1] = prgbColors[id].rgbGreen; //g
				rgbdata[i*3+2] = prgbColors[id].rgbBlue; //b
				//rgbdata[i*4+3] = 255; 
			}

			cbmp.Detach();
			cbmp.DeleteObject();
			delete pdata;
			pdata = NULL;
	    }
		else
		{
			hBitMap=image.Detach();

			CBitmap cbmp;
			cbmp.Attach(hBitMap);
			DWORD size = cbmp.GetBitmapBits(w*h*dep,rgbdata);

			for (int i=0;i<w*h;i++)
			{
				BYTE tmp = (rgbdata[i*dep]);
				rgbdata[i*dep+0] = (rgbdata[i*dep+2]); //r
				rgbdata[i*dep+2] = tmp;//b
			}

			cbmp.Detach();
			cbmp.DeleteObject();
		}

		DeleteObject(hBitMap);
		image.Destroy();
		tile->m_pdata = rgbdata;
	}
	return 1;
}
DWORD WINAPI  CArcgisMapLayer::Datathread2(LPVOID t)
{
	CArcgisMapLayer *object = (CArcgisMapLayer*)t;
	CString m_tileName;

	map<long,Tile*>::iterator it = (object->m_tmpData).begin();
	int id = 0;
	for(;it!=(object->m_tmpData).end();it++)
	{	
		id++;
		if (id<=(object->m_tmpData).size()*1/g_countT)
		{
			continue;
		}
		if (id>(object->m_tmpData).size()*2/g_countT)
		{
			break;
		}
		Tile *tile = (*it).second;
		int w = tile->m_TileW;
		int h = tile->m_TileH;
		CImage image;
		HBITMAP hBitMap;
		HRESULT hr = image.Load(tile->m_tileName);
		if (hr == E_FAIL)
		{
			tile->m_pdata = NULL;
			image.Destroy();
			continue;
		}
		int dep = image.GetBPP()/8;
		BYTE *rgbdata = NULL;
		if (dep==1||dep==3)
		{
			tile->m_sformat=GL_RGB;
			rgbdata = new BYTE[w*h*3];
		}
		else if (dep==4)
		{
			tile->m_sformat = GL_RGBA;
			rgbdata = new BYTE[w*h*4];
		}

		if (image.IsIndexed())
		{
			RGBQUAD prgbColors[256];
			image.GetColorTable(
				0,
				256,
				prgbColors 
				);

			hBitMap=image.Detach();

			BYTE *pdata = new BYTE[w*h*dep];
			CBitmap cbmp;
			cbmp.Attach(hBitMap);
			DWORD size = cbmp.GetBitmapBits(w*h*dep,pdata);

			for (int i=0;i<w*h;i++)//颜色索引转rgb
			{	
				int id = pdata[i];
				rgbdata[i*3+0] = prgbColors[id].rgbRed; //r
				rgbdata[i*3+1] = prgbColors[id].rgbGreen; //g
				rgbdata[i*3+2] = prgbColors[id].rgbBlue; //b
				//rgbdata[i*4+3] = 255; 
			}

			cbmp.Detach();
			cbmp.DeleteObject();
			delete pdata;
			pdata = NULL;
		}
		else
		{
			hBitMap=image.Detach();

			CBitmap cbmp;
			cbmp.Attach(hBitMap);
			DWORD size = cbmp.GetBitmapBits(w*h*dep,rgbdata);

			for (int i=0;i<w*h;i++)
			{
				BYTE tmp = (rgbdata[i*dep]);
				rgbdata[i*dep+0] = (rgbdata[i*dep+2]); //r
				rgbdata[i*dep+2] = tmp;//b
			}

			cbmp.Detach();
			cbmp.DeleteObject();
			cbmp.Detach();
		}

		DeleteObject(hBitMap);
		image.Destroy();
		tile->m_pdata = rgbdata;
	}
	return 1;
}
DWORD WINAPI  CArcgisMapLayer::Datathread3(LPVOID t)
{
	CArcgisMapLayer *object = (CArcgisMapLayer*)t;
	CString m_tileName;

	map<long,Tile*>::iterator it = (object->m_tmpData).begin();
	int id = 0;
	for(;it!=(object->m_tmpData).end();it++)
	{	
		id++;
		if (id<=(object->m_tmpData).size()*2/g_countT)
		{
			continue;
		}
		if (id>(object->m_tmpData).size()*3/g_countT)
		{
			break;
		}
		Tile *tile = (*it).second;
		int w = tile->m_TileW;
		int h = tile->m_TileH;
		CImage image;
		HBITMAP hBitMap;
		HRESULT hr = image.Load(tile->m_tileName);
		if (hr == E_FAIL)
		{
			tile->m_pdata = NULL;
			image.Destroy();
			continue;
		}
		int dep = image.GetBPP()/8;
		BYTE *rgbdata = NULL;
		
		if (dep==1||dep==3)
		{
			tile->m_sformat=GL_RGB;
			rgbdata = new BYTE[w*h*3];
		}
		else if (dep==4)
		{
			tile->m_sformat = GL_RGBA;
			rgbdata = new BYTE[w*h*4];
		}

		if (image.IsIndexed())
		{
			RGBQUAD prgbColors[256];
			image.GetColorTable(
				0,
				256,
				prgbColors 
				);

			hBitMap=image.Detach();

			BYTE *pdata = new BYTE[w*h*dep];
			CBitmap cbmp;
			cbmp.Attach(hBitMap);
			DWORD size = cbmp.GetBitmapBits(w*h*dep,pdata);

			for (int i=0;i<w*h;i++)//颜色索引转rgb
			{	
				int id = pdata[i];
				rgbdata[i*3+0] = prgbColors[id].rgbRed; //r
				rgbdata[i*3+1] = prgbColors[id].rgbGreen; //g
				rgbdata[i*3+2] = prgbColors[id].rgbBlue; //b
				//rgbdata[i*4+3] = 255; 
			}

			cbmp.Detach();
			cbmp.DeleteObject();
			delete pdata;
			pdata = NULL;
		}
		else
		{
			hBitMap=image.Detach();

			CBitmap cbmp;
			cbmp.Attach(hBitMap);
			DWORD size = cbmp.GetBitmapBits(w*h*dep,rgbdata);

			for (int i=0;i<w*h;i++)
			{
				BYTE tmp = (rgbdata[i*dep]);
				rgbdata[i*dep+0] = (rgbdata[i*dep+2]); //r
				rgbdata[i*dep+2] = tmp;//b
			}

			cbmp.Detach();
			cbmp.DeleteObject();
		}

		DeleteObject(hBitMap);
		image.Destroy();
		tile->m_pdata = rgbdata;
	}
	return 1;
}
DWORD WINAPI  CArcgisMapLayer::Datathread4(LPVOID t)
{
	CArcgisMapLayer *object = (CArcgisMapLayer*)t;
	CString m_tileName;

	map<long,Tile*>::iterator it = (object->m_tmpData).begin();
	int id = 0;
	for(;it!=(object->m_tmpData).end();it++)
	{	
		id++;
		if (id<=(object->m_tmpData).size()*3/g_countT)
		{
			continue;
		}
		if (id>(object->m_tmpData).size()*4/g_countT)
		{
			break;
		}
		Tile *tile = (*it).second;
		int w = tile->m_TileW;
		int h = tile->m_TileH;
		CImage image;
		HBITMAP hBitMap;
		HRESULT hr = image.Load(tile->m_tileName);
		if (hr == E_FAIL)
		{
			tile->m_pdata = NULL;
			image.Destroy();
			continue;
		}
		int dep = image.GetBPP()/8;
		BYTE *rgbdata = NULL;

		if (dep==1||dep==3)
		{
			tile->m_sformat=GL_RGB;
			rgbdata = new BYTE[w*h*3];
		}
		else if (dep==4)
		{
			tile->m_sformat = GL_RGBA;
			rgbdata = new BYTE[w*h*4];
		}

		if (image.IsIndexed())
		{
			RGBQUAD prgbColors[256];
			image.GetColorTable(
				0,
				256,
				prgbColors 
				);

			hBitMap=image.Detach();

			BYTE *pdata = new BYTE[w*h*dep];
			CBitmap cbmp;
			cbmp.Attach(hBitMap);
			DWORD size = cbmp.GetBitmapBits(w*h*dep,pdata);

			for (int i=0;i<w*h;i++)//颜色索引转rgb
			{	
				int id = pdata[i];
				rgbdata[i*3+0] = prgbColors[id].rgbRed; //r
				rgbdata[i*3+1] = prgbColors[id].rgbGreen; //g
				rgbdata[i*3+2] = prgbColors[id].rgbBlue; //b
				//rgbdata[i*dep+3] = 255; 
			}

			cbmp.Detach();
			cbmp.DeleteObject();
			delete pdata;
			pdata = NULL;
		}
		else
		{
			hBitMap=image.Detach();

			CBitmap cbmp;
			cbmp.Attach(hBitMap);
			DWORD size = cbmp.GetBitmapBits(w*h*dep,rgbdata);

			for (int i=0;i<w*h;i++)
			{
				BYTE tmp = (rgbdata[i*dep]);
				rgbdata[i*dep+0] = (rgbdata[i*dep+2]); //r
				rgbdata[i*dep+2] = tmp;//b
			}

			cbmp.DeleteObject();
			cbmp.Detach();
		}

		DeleteObject(hBitMap);
		image.Destroy();
		tile->m_pdata = rgbdata;
	}
	return 1;
}
DWORD WINAPI  CArcgisMapLayer::Datathread5(LPVOID t)
{
	CArcgisMapLayer *object = (CArcgisMapLayer*)t;
	CString m_tileName;

	map<long,Tile*>::iterator it = (object->m_tmpData).begin();
	int id = 0;
	for(;it!=(object->m_tmpData).end();it++)
	{	
		id++;
		if (id<=(object->m_tmpData).size()*4/g_countT)
		{
			continue;
		}
		if (id>(object->m_tmpData).size()*5/g_countT)
		{
			break;
		}
		Tile *tile = (*it).second;
		int w = tile->m_TileW;
		int h = tile->m_TileH;
		CImage image;
		HBITMAP hBitMap;
		HRESULT hr = image.Load(tile->m_tileName);
		if (hr == E_FAIL)
		{
			tile->m_pdata = NULL;
			image.Destroy();
			continue;
		}
		int dep = image.GetBPP()/8;
		BYTE *rgbdata = NULL;

		if (dep==1||dep==3)
		{
			tile->m_sformat=GL_RGB;
			rgbdata = new BYTE[w*h*3];
		}
		else if (dep==4)
		{
			tile->m_sformat = GL_RGBA;
			rgbdata = new BYTE[w*h*4];
		}

		if (image.IsIndexed())
		{
			RGBQUAD prgbColors[256];
			image.GetColorTable(
				0,
				256,
				prgbColors 
				);

			hBitMap=image.Detach();

			BYTE *pdata = new BYTE[w*h*dep];
			CBitmap cbmp;
			cbmp.Attach(hBitMap);
			DWORD size = cbmp.GetBitmapBits(w*h*dep,pdata);

			for (int i=0;i<w*h;i++)//颜色索引转rgb
			{	
				int id = pdata[i];
				rgbdata[i*3+0] = prgbColors[id].rgbRed; //r
				rgbdata[i*3+1] = prgbColors[id].rgbGreen; //g
				rgbdata[i*3+2] = prgbColors[id].rgbBlue; //b
				//rgbdata[i*dep+3] = 255; 
			}

			cbmp.Detach();
			cbmp.DeleteObject();
			delete pdata;
			pdata = NULL;
		}
		else
		{
			hBitMap=image.Detach();

			CBitmap cbmp;
			cbmp.Attach(hBitMap);
			DWORD size = cbmp.GetBitmapBits(w*h*dep,rgbdata);

			for (int i=0;i<w*h;i++)
			{
				BYTE tmp = (rgbdata[i*dep]);
				rgbdata[i*dep+0] = (rgbdata[i*dep+2]); //r
				rgbdata[i*dep+2] = tmp;//b
			}

			cbmp.DeleteObject();
			cbmp.Detach();
		}

		DeleteObject(hBitMap);
		image.Destroy();
		tile->m_pdata = rgbdata;
	}
	return 1;
}
DWORD WINAPI  CArcgisMapLayer::Datathread6(LPVOID t)
{
	CArcgisMapLayer *object = (CArcgisMapLayer*)t;
	CString m_tileName;

	map<long,Tile*>::iterator it = (object->m_tmpData).begin();
	int id = 0;
	for(;it!=(object->m_tmpData).end();it++)
	{	
		id++;
		if (id<=(object->m_tmpData).size()*5/g_countT)
		{
			continue;
		}
		Tile *tile = (*it).second;
		int w = tile->m_TileW;
		int h = tile->m_TileH;
		CImage image;
		HBITMAP hBitMap;
		HRESULT hr = image.Load(tile->m_tileName);
		if (hr == E_FAIL)
		{
			tile->m_pdata = NULL;
			image.Destroy();
			continue;
		}
		int dep = image.GetBPP()/8;
		BYTE *rgbdata = NULL;

		if (dep==1||dep==3)
		{
			tile->m_sformat=GL_RGB;
			rgbdata = new BYTE[w*h*3];
		}
		else if (dep==4)
		{
			tile->m_sformat = GL_RGBA;
			rgbdata = new BYTE[w*h*4];
		}

		if (image.IsIndexed())
		{
			RGBQUAD prgbColors[256];
			image.GetColorTable(
				0,
				256,
				prgbColors 
				);

			hBitMap=image.Detach();

			BYTE *pdata = new BYTE[w*h*dep];
			CBitmap cbmp;
			cbmp.Attach(hBitMap);
			DWORD size = cbmp.GetBitmapBits(w*h*dep,pdata);

			for (int i=0;i<w*h;i++)//颜色索引转rgb
			{	
				int id = pdata[i];
				rgbdata[i*3+0] = prgbColors[id].rgbRed; //r
				rgbdata[i*3+1] = prgbColors[id].rgbGreen; //g
				rgbdata[i*3+2] = prgbColors[id].rgbBlue; //b
				//rgbdata[i*dep+3] = 255; 
			}

			cbmp.Detach();
			cbmp.DeleteObject();
			delete pdata;
			pdata = NULL;
		}
		else
		{
			hBitMap=image.Detach();

			CBitmap cbmp;
			cbmp.Attach(hBitMap);
			DWORD size = cbmp.GetBitmapBits(w*h*dep,rgbdata);

			for (int i=0;i<w*h;i++)
			{
				BYTE tmp = (rgbdata[i*dep]);
				rgbdata[i*dep+0] = (rgbdata[i*dep+2]); //r
				rgbdata[i*dep+2] = tmp;//b
			}

			cbmp.Detach();
			cbmp.DeleteObject();
		}

		DeleteObject(hBitMap);
		image.Destroy();
		tile->m_pdata = rgbdata;
	}
	return 1;
}

//主线程解码
void  CArcgisMapLayer::decodetileSingleT()
{
	CArcgisMapLayer *object = (CArcgisMapLayer*)this;
	CString m_tileName;

	map<long,Tile*>::iterator it = (object->m_tmpData).begin();

	for(;it!=(object->m_tmpData).end();it++)
	{	
		Tile *tile = (*it).second;
		int w = tile->m_TileW;
		int h = tile->m_TileH;
		CImage image;
		HBITMAP hBitMap;
		HRESULT hr = image.Load(tile->m_tileName);
		if (hr == E_FAIL)
		{
			tile->m_pdata = NULL;
			image.Destroy();
			continue;
		}

		int dep = image.GetBPP()/8;	
		BYTE *rgbdata = NULL;
		if (dep==1||dep==3)
		{
			tile->m_sformat=GL_RGB;
			rgbdata = new BYTE[w*h*3];
		}
		else if (dep==4)
		{
			tile->m_sformat = GL_RGBA;
			rgbdata = new BYTE[w*h*4];
		}
	    if (image.IsIndexed())
	    {
			RGBQUAD prgbColors[256];
			image.GetColorTable(
				0,
				256,
				prgbColors 
				);

			hBitMap=image.Detach();

			BYTE *pdata = new BYTE[w*h*dep];
			CBitmap cbmp;
			cbmp.Attach(hBitMap);
			DWORD size = cbmp.GetBitmapBits(w*h*dep,pdata);

			for (int i=0;i<w*h;i++)//颜色索引转rgb
			{	
				int id = pdata[i];
				rgbdata[i*3+0] = prgbColors[id].rgbRed; //r
				rgbdata[i*3+1] = prgbColors[id].rgbGreen; //g
				rgbdata[i*3+2] = prgbColors[id].rgbBlue; //b
				//rgbdata[i*dep+3] = 255; //颜色索引的A值固定
			}

			cbmp.Detach();
			cbmp.DeleteObject();
			delete pdata;
			pdata = NULL;
	    }
		else
		{
			hBitMap=image.Detach();

			CBitmap cbmp;
			cbmp.Attach(hBitMap);
			DWORD size = cbmp.GetBitmapBits(w*h*dep,rgbdata);

			for (int i=0;i<w*h;i++)
			{
				BYTE tmp = (rgbdata[i*dep]);
				rgbdata[i*dep+0] = (rgbdata[i*dep+2]); //r
				rgbdata[i*dep+2] = tmp;//b
			}
			cbmp.Detach();
			cbmp.DeleteObject();
		}

		
		DeleteObject(hBitMap);
		image.Destroy();
		tile->m_pdata = rgbdata;
	}
	return;
}