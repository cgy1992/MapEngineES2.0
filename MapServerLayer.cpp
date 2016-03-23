#include "MapServerLayer.h"
#include "json/json.h"
#include <afxinet.h>

//extern int g_CtrlW;
//extern int g_CtrlH;
//extern double g_scale;

//const int	MAX_MAP_TEXTURE_TEMP = 2000;

CMapServerLayer::CMapServerLayer(void)
{
	m_bUpdateData = true;
}


CMapServerLayer::~CMapServerLayer(void)
{
	if (m_LayerDataList.size() != 0)
	{
		
		map<long, MapLayerData*>::iterator it = m_LayerDataList.begin();
		for (; it!=m_LayerDataList.end();it++)
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

//从远程服务上取 构造器 使用的参数
CMapServerLayer::CMapServerLayer(double cenX,double cenY,int js,CString mapname,int picW,int picH,CString path,CString pictype,CString ServerType)
{
	m_x = cenX;//中心点 120.14894, 30.26827, 0
	m_y = cenY;
	m_js = js;
	m_PicW = picW;//切片宽高是一致的
	m_PicH = picH;
	m_Lon = 0.0;
	m_Lat = 0.0;
	m_MapName = mapname;//地图名
	m_TitleType = pictype;
	m_Scale2ImageW = -1;
	m_Scale2ImageW = -1;
	m_resolution = 0.0;	
	m_layerType=VLayerType::vBaseLayer;
	m_LayerName = mapname;

	//图层路径
	CString szExePath = L"";
	::GetModuleFileName(NULL,szExePath.GetBufferSetLength(MAX_PATH+1),MAX_PATH);
	szExePath = szExePath.Left(szExePath.ReverseFind('\\'));
	szExePath = szExePath.Left(szExePath.ReverseFind('\\'));

	//创建GIS_MAP文件夹
	CString szMap = L"";
	CString url(m_LayerName);
	//szMap.Format(L"%s\\MAP\\Map_Temp\\ArcServer_MAP_%s",szExePath,url);//m_LayerName
	szMap.Format(L"%s\\MAP",szExePath);//m_LayerName

	bool bc = false;
	int err;
	if(!PathIsDirectory(szMap))
	{
		bc = CreateDirectory(szMap,NULL);
		err = GetLastError();
	}
	szExePath.Format(L"%s\\Map_TEMP", szMap);
	if(!PathIsDirectory(szExePath))
	{
		bc = CreateDirectory(szExePath,NULL);
		err = GetLastError();
	}
	szMap = szExePath;
	CString szMapPath = L"";
	szMapPath.Format(L"%s\\ArcServer_MAP_%s",szMap,url);
	if(!PathIsDirectory(szMapPath))
	{
		bc = CreateDirectory(szMapPath,NULL);
		err = GetLastError();
	}
	m_sPath = szMapPath;

	CString LayerPath = L"";
	LayerPath.Format(L"%s\\L%0.2d",szMapPath,js);
	if(!PathIsDirectory(LayerPath))
	{
		bc = CreateDirectory(LayerPath,NULL);
	}

	m_bUpdateData = true;

	CString serverurl	= path;
	USES_CONVERSION; 
	m_mapServerUrl = W2A(serverurl.GetBuffer(0));
	serverurl.ReleaseBuffer();

	m_bconnect = false;//0表示连接不上
	if (ServerType == "ArcServer")
	{
		m_MapType = MapType::ArcgisServerMap;
		m_bconnect = ConnetArcServer(serverurl); //从远程服务上，读取地图 config.json
	}
	else if (ServerType == "DegreeServer")
	{
		m_MapType = MapType::DegreeServermap;
		m_bconnect =  CoonnetDegreeServer(serverurl);
	}
}

//从本地磁盘上 取地图数据,[本组件 GeoBeansMap 使用该方法]  我将修改为不管是从远程服务还是本地 都使用这个构造器
CMapServerLayer::CMapServerLayer(double cenX,double cenY,int js,CString mapname,int picW,int picH,CString path,CString pictype,CString ServerType,double minlon,double minlat,double resolution0,DrawEngine *den)
{
	m_x = cenX;//中心点 120.14894, 30.26827, 0
	m_y = cenY;
	m_js = js;
	m_PicW = picW;//切片宽高是一致的
	m_PicH = picH;
	m_Lon = 0.0;
	m_Lat = 0.0;
	m_MapName = mapname;//地图名
	m_TitleType = pictype;
	m_Scale2ImageW = -1;
	m_Scale2ImageW = -1;
	m_resolution = 0.0;	
	m_layerType=VLayerType::vBaseLayer;
	m_LayerName = mapname;
	m_OriginX = minlon;
	m_OriginY = minlat;

	g_CtrlW = den->g_CtrlW;  //保存组件创建窗口的大小
	g_CtrlH = den->g_CtrlH;
	//add by xuyan
	CString serverurl	= path;
	USES_CONVERSION; 
	m_mapServerUrl = W2A(serverurl.GetBuffer(0));
	serverurl.ReleaseBuffer();
	

	//图片路径 是否在 本地
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
	//pos >=0 ? m_localornet = false : m_localornet = true;
	//m_sPath = path;

	m_bUpdateData = true;  //表示 需要重新的 取地图数据
	m_bconnect = false;//0表示连接不上

    if (ServerType == "GeoBeansServer")
	{
		m_MapType = MapType::GeoBeansServerMap;
		//m_bconnect =  true;  modify by xuyan
		if (m_localornet)
		{
		    m_bconnect = true;
			
			//依据[前端发来的Resolution0":1.406250078533376]，计算出了L0~L19不同地图图层的分辨率，
			//并设置了 当前地图级别的分辨率 m_resolution = m_LayerDataList[m_js]->Resolution;
			//CoonnetGeoBeansServer(serverurl,resolution0);  
			CoonnetGeoBeansServer(m_sPath,resolution0);   //modify by xuyan 不在使用 serverurl,并且这个m_sPath字段本来就没有用到
		}
		else   //modify by xuyan 将
		{
		   //如果不是本地，从网络上做，该如何？？,这部分的逻辑，在Draw中的取地图数据时 会考虑到，所以我们只是在这里创建 用缓存图片的 [路径]

			//图层路径
			CString szExePath = L"";
			::GetModuleFileName(NULL,szExePath.GetBufferSetLength(MAX_PATH+1),MAX_PATH); //得到本模块的工作路径
			szExePath = szExePath.Left(szExePath.ReverseFind('\\'));
			szExePath = szExePath.Left(szExePath.ReverseFind('\\'));

			//创建GIS_MAP文件夹
			CString szMap = L"";
			CString url(m_LayerName);
			//szMap.Format(L"%s\\MAP\\Map_Temp\\ArcServer_MAP_%s",szExePath,url);//m_LayerName
			szMap.Format(L"%s\\MAP",szExePath);// 构造本地存放图片的路径  C:\SmartCity\MAP

			bool bc = false;
			int err;
			if(!PathIsDirectory(szMap))
			{
				bc = CreateDirectory(szMap,NULL);   //如果存在则创建该目录
				err = GetLastError();
			}

			szExePath.Format(L"%s\\Map_TEMP", szMap);
	
			if(!PathIsDirectory(szExePath))
			{
				bc = CreateDirectory(szExePath,NULL);
				err = GetLastError();
			}

			szMap = szExePath;
			CString szMapPath = L"";
			szMapPath.Format(L"%s\\GeoBeansServer_MAP_%s",szMap,url);
			if(!PathIsDirectory(szMapPath))
			{
				bc = CreateDirectory(szMapPath,NULL);
				err = GetLastError();
			}
			m_sPath = szMapPath; //保存 buff下来的地图tile而构造的[本地路径]

			CString LayerPath = L"";
			LayerPath.Format(L"%s\\L%0.2d",szMapPath,js);  //保存 buff下来的地图tile而构造的[本地路径]
			if(!PathIsDirectory(LayerPath))
			{
				bc = CreateDirectory(LayerPath,NULL);
			}
		 // 是否缺少对 CoonnetGeoBeansServer(m_sPath,resolution0);的调用，计算不同js的分辨率 
		}
		
	}//秦皇岛的逻辑处理完毕
	else  if (ServerType == "PGISServer")
	{
		m_MapType = MapType::PGISServerMap;
		//m_bconnect =  true; //把该字段放置在内部
		if (m_localornet)
		{
			m_bconnect = true;
			//CoonnetGeoBeansServer(serverurl,resolution0);
			CoonnetGeoBeansServer(m_sPath,resolution0);
		}
		else
		{
		   // 如果不是本地，从网络上，有待完成！
		}
	}
}

//下载工具
bool CMapServerLayer::ConnetArcServer(CString serverurl)
{
	int nret = 0;

	CString strSentence;
	/*"http://10.1.50.213/ArcGIS/rest/services/HangZhou/MapServer?f=json";*/  // 构建一个JSON字符串
	CString strFileName = serverurl + L"?f=json"; //构造下载字符串  获取地图的配置文件

	CInternetSession sess(L"ARCSVR");   //MFC中的类
	CHttpFile* fileGet = NULL;
	try
	{
		fileGet=(CHttpFile*)sess.OpenURL(strFileName);
	}
	catch(CException* e)
	{
		nret = 1;
		sess.Close();
		return false;
	}    

	if(fileGet)
	{
		DWORD dwStatus;
		DWORD dwBuffLen = sizeof(dwStatus);
		BOOL bSuccess = fileGet->QueryInfo(HTTP_QUERY_STATUS_CODE|HTTP_QUERY_FLAG_NUMBER, &dwStatus, &dwBuffLen);

		if( bSuccess && dwStatus>= 200&& dwStatus<300 ) 
		{ 
			CString data("");//获取HTML
			char* szBuff = new char[10241];
			memset(szBuff, 0, 10241);
			int nReadCount = 0;
			while((nReadCount = fileGet->Read(szBuff, 10240)) > 0) // 该循环只会执行一遍，一次性读完 所有的内容
			{
				//int nReadLength = fileGet->Read(szBuff, 1024);
				if(nReadCount==0) break;  //内容读取完毕
				szBuff[nReadCount]='\0';
				data += szBuff;

				const char* str = szBuff;  

				Json::Reader reader;  
				Json::Value root;  
				if (reader.parse(str, root))  // reader将Json字符串解析到root，root将包含Json里所有子元素   
				{  
					//获取切片大小
					m_PicSizeW = root["tileInfo"]["rows"].asInt();
					m_PicSizeH = root["tileInfo"]["cols"].asInt();
					Json::Value plods = root["tileInfo"]["lods"];
					int pSize  = plods.size(); //地图共有多少层 Level
					m_dpi = root["tileInfo"]["dpi"].asInt();
					string format= root["tileInfo"]["format"].asString();
					if (format.compare("PNG32") == 0)
					{
						m_TitleType = L"png";
					}

					// 遍历数组  
					for(int i = 0; i < pSize; ++i)  
					{  
						int  plevel = plods[i]["level"].asInt();
						double  presolution = plods[i]["resolution"].asDouble();
						int  pscale = plods[i]["scale"].asInt();
						MapLayerData* pLayerData = new MapLayerData();  //不同Level的地图信息，最关键的就是 分辨率
						pLayerData->Level = plevel;
						pLayerData->Resolution = presolution;
						pLayerData->Scale = pscale;
						m_LayerDataList[plevel] = pLayerData;
					}
					//获取extent的
					Json::Value pExtent = root["fullExtent"];
					m_Xmin = pExtent["xmin"].asDouble();
					m_Xmax = pExtent["xmax"].asDouble();
					m_Ymin = pExtent["ymin"].asDouble();
					m_Ymax = pExtent["ymax"].asDouble();

					//获取左上角坐标
					Json::Value porigin = root["tileInfo"]["origin"];
					m_OriginX = porigin["x"].asDouble();
					m_OriginY = porigin["y"].asDouble();
				}  
				memset(szBuff, 0, 10241);
			}
			delete szBuff;
			szBuff = NULL;
		}
		else 
		{

		}
	}
	fileGet->Close();
	delete fileGet;
	fileGet = NULL;

	sess.Close();

	curl_global_init(CURL_GLOBAL_ALL);     
	m_multi_handle=curl_multi_init();  

	return true;
}

//int CMapServerLayer::CoonnetDegreeServer(CString serverurl)
//{
//	int nret = 0;
//
//	CString strSentence;
//	//http://10.1.50.213:8088/deegree-webservices-3.3.1/services
//	//组成：http://10.1.50.213:8088/deegree-webservices-3.3.1/services/wmts?service=WMTS&request=GetCapabilities
//	CString strFileName = serverurl + L"/wmts?service=WMTS&request=GetCapabilities";
//
//	CInternetSession sess(L"ARCSVR");
//	CHttpFile* fileGet;
//	try
//	{
//		fileGet=(CHttpFile*)sess.OpenURL(strFileName);
//	}
//	catch(CException* e)
//	{
//		nret = 1;
//	}    
//
//	if(fileGet)
//	{
//		DWORD dwStatus;
//		DWORD dwBuffLen = sizeof(dwStatus);
//		BOOL bSuccess = fileGet->QueryInfo(HTTP_QUERY_STATUS_CODE|HTTP_QUERY_FLAG_NUMBER, &dwStatus, &dwBuffLen);
//
//		if( bSuccess && dwStatus>= 200&& dwStatus<300 ) 
//		{ 
//			CString data("");//获取HTML
//			char* szBuff = new char[10241];
//			memset(szBuff, 0, 10241);
//			int nReadCount = 0;
//			while((nReadCount = fileGet->Read(szBuff, 10240)) > 0)
//			{
//				//int nReadLength = fileGet->Read(szBuff, 1024);
//				if(nReadCount==0) break;
//				szBuff[nReadCount]='\0';
//				data += szBuff;
//
//				const char* str = szBuff;  
//
//				Json::Reader reader;  
//				Json::Value root;  
//				if (reader.parse(str, root))  // reader将Json字符串解析到root，root将包含Json里所有子元素   
//				{  
//					//获取切片大小
//					m_PicSizeW = root["tileInfo"]["rows"].asInt();
//					m_PicSizeH = root["tileInfo"]["cols"].asInt();
//					Json::Value plods = root["tileInfo"]["lods"];
//					int pSize  = plods.size(); //
//					m_dpi = root["tileInfo"]["dpi"].asInt();
//					string format= root["tileInfo"]["format"].asString();
//					if (format.compare("PNG32") == 0)
//					{
//						m_TitleType = L"png";
//					}
//
//					// 遍历数组  
//					for(int i = 0; i < pSize; ++i)  
//					{  
//						int  plevel = plods[i]["level"].asInt();
//						double  presolution = plods[i]["resolution"].asDouble();
//						int  pscale = plods[i]["scale"].asInt();
//						MapLayerData* pLayerData = new MapLayerData();
//						pLayerData->Level = plevel;
//						pLayerData->Resolution = presolution;
//						pLayerData->Scale = pscale;
//						m_LayerDataList[plevel] = pLayerData;
//					}
//					//获取extent的
//					Json::Value pExtent = root["fullExtent"];
//					m_Xmin = pExtent["xmin"].asDouble();
//					m_Xmax = pExtent["xmax"].asDouble();
//					m_Ymin = pExtent["ymin"].asDouble();
//					m_Ymax = pExtent["ymax"].asDouble();
//
//					//获取左上角坐标
//					Json::Value porigin = root["tileInfo"]["origin"];
//					m_OriginX = porigin["x"].asDouble();
//					m_OriginY = porigin["y"].asDouble();
//				}  
//				memset(szBuff, 0, 10241);
//			}
//			delete szBuff;
//			szBuff = NULL;
//		}
//		else 
//		{
//			//strSentence.Format(L"打开网页文件出错，错误码：%d", dwStatus);
//			//AfxMessageBox(strSentence);
//		}
//	}
//	fileGet->Close();
//	delete fileGet;
//	fileGet = NULL;
//
//	sess.Close();
//
//	curl_global_init(CURL_GLOBAL_ALL);     
//	m_multi_handle=curl_multi_init();  
//
//	return 1;
//}

void CMapServerLayer::GetMapConfigInfo(int djs,double &lfTileOriginX,double &lfTileOriginY,int &dTileCols,int &dTileRows,
	int &dDPI,double &lfResolution,double &dscale)
{
	if (djs >=0&&m_LayerDataList.size()!=0&&(djs<=m_LayerDataList.size()-1))
	{
		dscale = m_LayerDataList[djs]->Scale;
		lfResolution = m_LayerDataList[djs]->Resolution;//地图分别率 
		lfTileOriginX = m_OriginX;
		lfTileOriginY = m_OriginY;
		dTileCols = m_PicSizeH;
		dTileRows = m_PicSizeW;
		dDPI = m_dpi;
	}
}

int CMapServerLayer::MergeRequestUrl(int row,int col,int level,char* urlStr)
{
	//L"http://10.1.10.47/ArcGIS/rest/services/World/MapServer/tile/4/40/40";

	sprintf(urlStr,"%s/%d/%d/%d",m_mapServerUrl,level,col,row);  //构造动态的字符串
	//sprintf(urlStr,"http://%s:%d%sCol=%d&Row=%d&Zoom=%d&V=0.3",m_cIP,m_iPort,m_mapServerUrl,col,row,level-4);
	//http://10.1.254.87:10000/EzServer/Maps/ChinaMap12/EzMap?Service=getImage&Type=RGB&ZoomOffset=4&Col=15&Row=3&Zoom=2&V=0.3
	//sprintf(urlStr,"http://10.1.254.87:10000/EzServer/Maps/ChinaMap12/EzMap?Service=getImage&Type=RGB&ZoomOffset=4&Col=3&Row=0&Zoom=0&V=0.3");
	//OutputDebugStringA(urlStr);

	return 1;
}

struct buf_t 
{
	size_t sz;
	char buffer[204800];//200K
};

//vector<buf_t*> bitmapdata;
size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)  //这个函数是为了符合CURLOPT_WRITEFUNCTION, 而构造的
{
	buf_t *bf = (buf_t *)stream;
	memcpy(bf->buffer + bf->sz, ptr, size * nmemb);
	bf->sz += size * nmemb;
	return size * nmemb;
}

buf_t bufs[1000];   //2M的内存空间，每一个数组元素代表了 一张地图瓦片

map<long, buf_t*> ServerMapData; //存放从远程服务上 取的图片

vector<Tile*> CMapServerLayer::GetData(int js,double cenlon,double cenlat,double WScreen,double HScreen,long& ditX,long& ditY,long& Wcount,long & Hcount)
{
	ServerMapData.clear();  //开辟内存空间，保存从远程服务上读取的 图片

	CURL *curl[1000];//最多1000个url
	char* url[1000];
	int xx = 0;  //统计从远程服务上取的图片数

	vector<Tile*> LayerTileVector;
	double lon ;
	double lat ;
	double width ;
	double height;
	double letflon;
	double Toplat;
	double scale;
	int ndpi;
	double dresolution;

	GetMapConfigInfo(js,letflon,Toplat,m_PicH,m_PicW,ndpi,dresolution,scale);
	//g_scale = scale;

	//中心点对应的行列:行=1表示R00000001,即第二行，列类似
	double tileWidth = m_PicW * dresolution;
	double tColCenter = (int)floor((cenlon - letflon) / tileWidth);
	lon = letflon + tColCenter*tileWidth;

	double tileHeight = m_PicH * dresolution;
	double tRowCenter = (int)floor((Toplat-cenlat  ) / tileHeight);
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

	//ditY = (int)((m_OriginY - cenlat)/dresolution) - tRowCenter * 256;
	//ditX = (int)((cenlon - m_OriginX)/dresolution) - tColCenter * 256;

	m_tmpData.clear();

	int TitleNum = 0;

	USES_CONVERSION;
	string mapname = W2A(m_MapName.GetBuffer(0));
	m_MapName.ReleaseBuffer();
	string TitleType = W2A(m_TitleType.GetBuffer(0));
	m_TitleType.ReleaseBuffer();

	//i表示列数，j表示行数
	CString strJs;
	strJs.Format(L"%s\\L%02d\\",m_sPath,js);
	if(!PathIsDirectory(strJs))
	{
		CreateDirectory(strJs,NULL);//如果本地没有该层图片,则先创建文件夹
	}

	//组url:把所要请求的tile的url 都存在一个数组内
	for (int j = m_areashow[0]; j<=m_areashow[1] ;j++)
	{
		CString strRow;
		strRow.Format(L"%sR%08x",strJs,j);
		if(!PathIsDirectory(strRow))
		{
			CreateDirectory(strRow,NULL);//创建行文件夹
		}

		for(int i = m_areashow[2]; i<=m_areashow[3] ;i++)
		{	
			CString strCol = L"";
			strCol.Format(L"\\C%08x",i);

			CString pStrFullPath =  strRow + strCol + "." +  m_TitleType;//图片格式png,jpg等

			if ( PathFileExists(pStrFullPath))//如果本地已经有该文件则不需从服务获取数据
			{	// 地图重新下载
				Bitmap* theBitmap = Bitmap::FromFile(pStrFullPath);
				if (theBitmap->GetLastStatus() == S_OK)
				{
					DeleteObject(theBitmap);
					delete theBitmap;
					continue;
				}
				DeleteObject(theBitmap);
				delete theBitmap;
			}

			TitleNum++;

			long key =js * 10000 + (i ) * 100 + j;
			curl[xx] = curl_easy_init();

			char* urlStr = new char[512];
			if (m_MapType == MapType::ArcgisServerMap)
			{
				//js传进来path http://10.1.50.213/ArcGIS/rest/services/HangZhou/MapServer
				//需要组成http://10.1.50.213/ArcGIS/rest/services/HangZhou/MapServer/tile/js/rowid/colid
				sprintf(urlStr,"%s/tile/%d/%d/%d",m_mapServerUrl.c_str(),js,j,i);
			}
			else if (m_MapType == MapType::DegreeServermap)
			{
				//js传进来path http://10.1.50.213:8088/deegree-webservices-3.3.1/services   ;///wmts?service=WMTS
				               http://10.1.50.213:8088/deegree-webservices-3.3.1/services/wmts?service?SERVICE=WMTS&REQUEST=GetTile&VERSION=1.0.0&LAYER=h&STYLE=default&TILEMATRIXSET=tilenatrixset_hzyx&TILEMATRIX=2&TILEROW=4&TILECOL=4&FORMAT=image%2Fpng
				//需要组成http:http://10.1.50.213:8088/deegree-webservices-3.3.1/services?SERVICE=WMTS&REQUEST=GetTile&VERSION=1.0.0&LAYER=hzyxTile&STYLE=default&TILEMATRIXSET=tilematrixset_hzyx&TILEMATRIX=2&TILEROW=4&TILECOL=4&FORMAT=image%2Fpng
				sprintf(urlStr,"%s?SERVICE=WMTS&REQUEST=GetTile&VERSION=1.0.0&LAYER=%s&STYLE=default&TILEMATRIXSET=%s&TILEMATRIX=%d&TILEROW=%d&TILECOL=%d&FORMAT=image\%%2F%s",m_mapServerUrl.c_str(),mapname.c_str(),m_TileMatrixSetname,js,j,i,TitleType.c_str());//tilematrixset_hzyx

			}

			url[xx] = urlStr; 
			bufs[xx].sz = 0;
			curl_easy_setopt(curl[xx], CURLOPT_URL, url[xx]);
			curl_easy_setopt(curl[xx], CURLOPT_WRITEFUNCTION, &write_data);
			curl_easy_setopt(curl[xx], CURLOPT_WRITEDATA, &bufs[xx]);  //下载图片，到[开辟的内存空间]
			set_share_handle(curl[xx]);
			curl_easy_setopt(curl[xx], CURLOPT_FORBID_REUSE, 1); 
			curl_multi_add_handle(m_multi_handle, curl[xx]);
			ServerMapData[key] = &bufs[xx]; //
			xx++;
		}
	}

	int running_handle_count;
	while (CURLM_CALL_MULTI_PERFORM == curl_multi_perform(m_multi_handle, &running_handle_count))
	{
		//		cout << running_handle_count << endl;
	}

	while (running_handle_count)
	{
		timeval tv;
		tv.tv_sec = 1;
		tv.tv_usec = 0;

		int max_fd;
		fd_set fd_read;
		fd_set fd_write;
		fd_set fd_except;

		FD_ZERO(&fd_read);
		FD_ZERO(&fd_write);
		FD_ZERO(&fd_except);

		curl_multi_fdset(m_multi_handle, &fd_read, &fd_write, &fd_except, &max_fd);
		int return_code = select(max_fd + 1, &fd_read, &fd_write, &fd_except, &tv);
		if (-1 == return_code)
		{
			//cerr << "select error." << endl;
			break;
		}
		else
		{
			while (CURLM_CALL_MULTI_PERFORM == curl_multi_perform(m_multi_handle, &running_handle_count))
			{
				//	cout << running_handle_count << endl;
			}
		}
	}

	//从服务获取到数据以后:
	for (int j = m_areashow[0]; j<=m_areashow[1] ;j++)
	{
		CString strRow;
		strRow.Format(L"%sR%08x",strJs,j);
	
		for(int i = m_areashow[2]; i<=m_areashow[3] ;i++)
		{	
			bool bexistsTile = true;
			CString strCol = L"";
			strCol.Format(L"\\C%08x",i);

			CString pStrFullPath =  strRow + strCol + "." + m_TitleType;//图片格式jpg

			//获取数据
			long key =js * 10000 + (i ) * 100 + j;
	
			if (!PathFileExists(pStrFullPath) && ServerMapData[key] != NULL)//本地不存在文件并且已经从server拿到数据
			{		
				//缓存图片	测试了PNG 图片，其他格式的还需要测试。
				buf_t* pbuf_t = ServerMapData[key];
				//判断内存数据是否是图片
				if (pbuf_t->sz!=0 || pbuf_t->buffer[0]!='<')
				{				
					FILE* f = fopen(CStringA(pStrFullPath),"wb");//"D:\\L02\\vtron.png"
					fwrite(&pbuf_t->buffer,1,pbuf_t->sz,f); //保存到本地文件夹
					fflush(f);
					fclose(f);
				}
				else
				{
					//服务返回的数据不是图片：arcserver返回html
					bexistsTile = false;
				}
			}	

				Vertex verts[4] = {}; //设置贴图几何图片的 [顶点信息]
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
				map<long,Tile*>::iterator It;
				It = m_Cachemap.find(key);

				if(It != m_Cachemap.end())
				{
					tile = It->second;
					tile->SetVertex(verts);
				}
				else
				{
					if (bexistsTile)
					{
						tile = new Tile(pStrFullPath,verts,m_PicW,m_PicH,m_TitleType,js,j,i); //
						tile->m_sformat = GL_RGBA;
						m_tmpData[key] = tile;	
					}					
				}
				LayerTileVector.push_back(tile);
			}
	}

	for (int i = 0;i < TitleNum;i++) //消耗用于请求的链接
	{
		delete[] url[i];
		curl_easy_cleanup(curl[i]);
	}


	int datasize = m_tmpData.size(); //遍历容器中
	if (datasize>0)
	{
		DWORD start = GetTickCount();
		//多线程解码jpg		
		if (datasize >= 6)
		{
			DecodeTile();
		}
		else
		{
			decodetileSingleT();
		}
		DWORD end = GetTickCount() - start;
		map<long,Tile*>::iterator it = m_tmpData.begin();
		for(;it!=m_tmpData.end();it++)
		{
			Tile*tile = (*it).second;
			int key = (*it).first;
			tile->init();
			m_Cachemap[key] = tile;
		}
	}

	//控制缓存大小
	if (m_Cachemap.size()>100)//要删除哪张图片,需要
	{
		map<long, Tile*>::iterator it = m_Cachemap.begin();
		for (;it!=m_Cachemap.end();it++)
		{
			long tmpkey = (*it).first;
			Tile* tile = (*it).second;
			if (tile!=NULL)//如果该对象不是当前要显示的则可以删除
			{
				if (tile->m_js != js || (tile->m_rowid<m_areashow[0])||(tile->m_rowid>m_areashow[1])   //修改为正常
					|| (tile->m_colid<m_areashow[2])||(tile->m_colid>m_areashow[3]))
				{
					delete tile;
					tile = NULL;
					m_Cachemap.erase(it);
					if (m_Cachemap.size()<100)
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

	m_CenLon = cenlon;//当前中心点
	m_CenLat = cenlat;
	m_LeftTop_Lon = letflon;//整个图层的左上角坐标
	m_LeftTop_Lat = Toplat;
	m_Lon = m_Scale2ImageW*dresolution;
	m_Lat = m_Scale2ImageH*dresolution;

	m_Scale = scale;
	m_resolution = dresolution;

	return LayerTileVector;
}

//geo地图 ，获取地图 图片数据  [秦皇岛使用该函数读取图片]
vector<Tile*> CMapServerLayer::GetGeobeansData(int js,double cenlon,double cenlat,double WScreen,double HScreen,long& ditX,long& ditY,long& Wcount,long & Hcount)
{
	//每次拿数据，只是关注，当前窗口中正在显示的 地图Tile
	ServerMapData.clear();  //用于保存 不在本地 的地图 图片[只是在当前窗口中显示的图片]
		m_tmpData.clear(); //清空临时的 装图片的 容器[CBaseLayer中定义]，在当前窗口中出现的图片，这个容器中的保存的 key:value 主要用于buff在m_cachemap容器中
	vector<Tile*> LayerTileVector;//保存在当前窗口显示范围之内的地图图片，这个容器是作为结果返回。
	
	CURL *curl[1000];//最多1000个url
	char* url[1000];
	int xx = 0;  //记录用于从远程服务上下载图片 的URL数量

	
	double lat ;
	double width ;
	double height;
	double letflon;
	double Toplat;
	double scale;
	int ndpi;
	double dresolution;
	
	//得到分辨率是js级的 地图信息
	GetMapConfigInfo(js,letflon,Toplat,m_PicH,m_PicW,ndpi,dresolution,scale);  //这里为什么不直接使用，CMapServerLayer中已经设置好的成员变量，而采用[局部变量]

	//中心点对应的行列
	double tileWidth = m_PicW * dresolution; //一张图片的宽度占用的[经纬度数]
	double tColCenter = (int)floor((cenlon - letflon) / tileWidth);  //中心点

	double tileHeight = m_PicH * dresolution;
	double tRowCenter = (int)floor((cenlat - Toplat) / tileHeight);

	Wcount = WScreen / m_PicW ;
	Hcount = HScreen / m_PicH; 

	double picsizeW = (double)m_PicW;
	double picsizeH = (double)m_PicH;
	double pixX = (cenlon - letflon)/dresolution;
	double pixY = ((cenlat - Toplat)/ dresolution);

	int startx =(pixX-WScreen/2)/picsizeW;//lie
	int endx = (pixX+WScreen/2)/picsizeW;
	int starty = (pixY+HScreen/2)/picsizeH; //hang
	int endy = (pixY-HScreen/2)/picsizeH;

	m_areashow[0] = starty>0?starty:0;//hang
	m_areashow[1] = endy>0?endy:0;
	m_areashow[2] = startx>0?startx:0;//lie
	m_areashow[3] = endx>0?endx:0;

	int xmin = m_areashow[2];
	int ymin = m_areashow[0];


	//通过获取的第一张图片的左上角坐标与控件左上角坐标进行差值获取偏移
	ditX = (pixX - WScreen / 2.0) - xmin * picsizeW;
	ditY = picsizeH - ((pixY + HScreen / 2.0) - ymin * picsizeH);

	int TitleNum = 0; //统计从 远程服务中 拿的图片的 数目

	USES_CONVERSION;
	string mapname = W2A(m_MapName.GetBuffer(0));/*"raster"*/
	m_MapName.ReleaseBuffer();
	string TitleType = W2A(m_TitleType.GetBuffer(0));/*"jpg";*/
	m_TitleType.ReleaseBuffer();

	//i表示列数，j表示行数
	CString strJs;
	strJs.Format(L"%s\\L%02d\\",m_sPath,js);  //  f:/raster\L14\  //

	if(!PathIsDirectory(strJs))    //因为这里的m_sPath可能是真实的[地图本地路径] 也可能是 [我们构造的缓存路径]
	{                            
		CreateDirectory(strJs,NULL);
	}

	for (int j = m_areashow[0]; j>=m_areashow[1] ;j--)
	{
		CString strRow;
		strRow.Format(L"%sR%d",strJs,j);  // f:/raster\L14\R14107 
	
		if(!PathIsDirectory(strRow))
		{
			CreateDirectory(strRow,NULL);//创建行文件夹
		}

		for(int i = m_areashow[2]; i<=m_areashow[3] ;i++)
		{	
			CString strCol = L"";
			strCol.Format(L"\\C%d",i);

			CString pStrFullPath =  strRow + strCol + "." +  m_TitleType;//图片格式png,jpg等  // f:/raster\L14\R14107\C13632.jpg

			//因为pStrFullPath 真实的[地图本地路径] 也可能是 [我们构造的缓存路径]，所以才有下面判断文件是否存在的操作
			if ( PathFileExists(pStrFullPath))//如果本地已经有该文件则不需从服务获取数据
			{	
				Bitmap* theBitmap = Bitmap::FromFile(pStrFullPath);
				if (theBitmap->GetLastStatus() == S_OK)
				{
					DeleteObject(theBitmap);
					delete theBitmap;
					continue;
				}
				DeleteObject(theBitmap);
				delete theBitmap;
			}

			TitleNum++;//统计从 远程服务中 拿的图片的 数目

			//这个bool变量在 构造器中已经设置好
			if (!m_localornet)   //这里明确了一个语义，如果传递的path是本地路径，那么地图肯定是全的不会取下载，如果是URL才会取下载
			{
				long key =js * 10000 + (i ) * 100 + j;

				char* urlStr = new char[512];
				if (m_MapType == MapType::GeoBeansServerMap)
				{
					//js传进来path http://111.63.38.38:9100/QuadServer/maprequest
					//需要组成http://111.63.38.38:9100/QuadServer/maprequest?services=raster&row=14129&col=13630&level=14
					sprintf(urlStr,"%s?services=%s&row=%d&col=%d&level=%d&rowModel=a",m_mapServerUrl.c_str(),mapname.c_str(),j,i,js);
				}

				curl[xx] = curl_easy_init();
				url[xx] = urlStr; 
				bufs[xx].sz = 0;  //bufs 是定义的全局数组
				curl_easy_setopt(curl[xx], CURLOPT_URL, url[xx]);
				curl_easy_setopt(curl[xx], CURLOPT_WRITEFUNCTION, &write_data);
				curl_easy_setopt(curl[xx], CURLOPT_WRITEDATA, &bufs[xx]);     //将图片写入到
				set_share_handle(curl[xx]);
				curl_easy_setopt(curl[xx], CURLOPT_FORBID_REUSE, 1); 
				curl_multi_add_handle(m_multi_handle, curl[xx]);
				ServerMapData[key] = &bufs[xx];
			}
			xx++; //统计从 远程服务中 拿的图片的 URL数目
		}
	}// 所有的 [在当前窗口中显示的地图图片] 在本地的不做任何操作，不在本地，读取数据并保存到 ServerMapData

	int running_handle_count;
	while (CURLM_CALL_MULTI_PERFORM == curl_multi_perform(m_multi_handle, &running_handle_count)&& !m_localornet)
	{
		//		cout << running_handle_count << endl;
	}

	while (running_handle_count && !m_localornet)
	{
		timeval tv;
		tv.tv_sec = 1;
		tv.tv_usec = 0;

		int max_fd;
		fd_set fd_read;
		fd_set fd_write;
		fd_set fd_except;

		FD_ZERO(&fd_read);
		FD_ZERO(&fd_write);
		FD_ZERO(&fd_except);

		curl_multi_fdset(m_multi_handle, &fd_read, &fd_write, &fd_except, &max_fd);
		int return_code = select(max_fd + 1, &fd_read, &fd_write, &fd_except, &tv);
		if (-1 == return_code)
		{
			//cerr << "select error." << endl;
			break;
		}
		else
		{
			while (CURLM_CALL_MULTI_PERFORM == curl_multi_perform(m_multi_handle, &running_handle_count))
			{
				//	cout << running_handle_count << endl;
			}
		}
	}
	
	//从服务获取到数据以后: 处理所有在窗口显示范围内的 图片
	for (int j = m_areashow[0]; j>=m_areashow[1] ;j--)
	{
		CString strRow;
		strRow.Format(L"%sR%d",strJs,j);

		for(int i = m_areashow[2]; i<=m_areashow[3] ;i++)
		{	
			CString strCol = L"";
			strCol.Format(L"\\C%d",i);

			CString pStrFullPath =  strRow + strCol + "." + m_TitleType;//图片格式jpg
			
			bool bexistsTile = PathFileExists(pStrFullPath); //验证该图片 是否存在本地磁盘

			//获取数据，由图片的层级js、行、列共同来组成 key
			long key =js * 10000 + (i ) * 100 + j;

			//现有组件中是，地图超出了显示级别， 此时的图片[既不在本地 也不在 ServerMapData中]
			//这样会导致，有NULL的Tile被 push_back进 [LayerTileVector]
			if (!bexistsTile && ServerMapData[key] != NULL)//本地不存在文件并且已经从server拿到数据
			{		
				//缓存图片	测试了PNG 图片，其他格式的还需要测试。
				buf_t* pbuf_t = ServerMapData[key];
				//判断内存数据是否是图片
				if (pbuf_t->sz!=0 && pbuf_t->buffer[0]!='<')
				{				
					FILE* f = fopen(CStringA(pStrFullPath),"wb");
					fwrite(&pbuf_t->buffer,1,pbuf_t->sz,f);  //将保存在 ServerMapData中的远程图片数据 ，保存在本地磁盘中 *.jpg
					fflush(f);
					fclose(f);
				}
				else
				{
					//服务返回的数据不是图片：arcserver返回html
					bexistsTile = false;
				}
			}	

			//指定一张jpg图片的，图元大小256*256，并设置好 纹理坐标，及图元的 顶点坐标，这里是以pixel为单位来 计算坐标
			Vertex verts[4] = {};
			verts[0].tu = 0;
			verts[0].tv = 0;
			verts[0].loc[0] = (i-xmin)*m_PicW-ditX;
			verts[0].loc[1] = (ymin-j)*m_PicH-ditY;
			verts[0].loc[2] = 0;
			verts[0].color[0] = 255;
			verts[0].color[1] = 0;
			verts[0].color[2] = 0;
			verts[3].color[3] = 200;

			verts[1].tu = 1;
			verts[1].tv = 0;
			verts[1].loc[0] = (i-xmin)*m_PicW-ditX + m_PicW;
			verts[1].loc[1] = (ymin-j)*m_PicH-ditY;
			verts[1].loc[2] = 0;
			verts[1].color[0] = 100;
			verts[1].color[1] = 0;
			verts[1].color[2] = 0;
			verts[3].color[3] = 0;

			verts[2].tu = 1;
			verts[2].tv = 1;
			verts[2].loc[0] = (i-xmin)*m_PicW-ditX + m_PicW;
			verts[2].loc[1] = (ymin-j)*m_PicH-ditY + m_PicH;
			verts[2].loc[2] = 0;
			verts[2].color[0] = 100;
			verts[2].color[1] = 0;
			verts[2].color[2] = 0;
			verts[3].color[3] = 0;

			verts[3].tu = 0;
			verts[3].tv = 1;
			verts[3].loc[0] = (i-xmin)*m_PicW-ditX;
			verts[3].loc[1] = (ymin-j)*m_PicH-ditY + m_PicH;
			verts[3].loc[2] = 0;
			verts[3].color[0] =255;
			verts[3].color[1] = 0;
			verts[3].color[2] = 0;
			verts[3].color[3] = 200;

			//先判断内存缓冲池中是否有该图片
			Tile* tile = NULL;
			map<long,Tile*>::iterator It;
			It = m_Cachemap.find(key);

			if(It != m_Cachemap.end())
			{
				tile = It->second;
				tile->SetVertex(verts);
			}
			else
			{
				if (bexistsTile) // 需要显示的图片在本地磁盘上
				{
					tile = new Tile(pStrFullPath,verts,m_PicW,m_PicH,m_TitleType,js,j,i);
					tile->m_sformat = GL_RGBA;
					m_tmpData[key] = tile;	 //这里的图片还在磁盘上，需要将jpg解码成 bitmap格式
				}					
			}
			//可以在这里添加一个  if(NULL != tile)
			LayerTileVector.push_back(tile);
		}//内层for循环，处理完毕单张图片
	}//处理完毕 所有的 地图图片

	for (int i = 0;i < TitleNum && !m_localornet;i++)
	{
		delete[] url[i];
		curl_easy_cleanup(curl[i]);
	}


	int datasize = m_tmpData.size(); 
	if (datasize>0)
	{
		//DWORD start = GetTickCount();
		//多线程解码jpg		  将jpg从磁盘读入内存，并转化为可以用于纹理贴图的[位图]
		if (datasize >= 6)
		{
			DecodeTile();
		}
		else
		{
			decodetileSingleT();
		}
		//DWORD end = GetTickCount() - start;

		map<long,Tile*>::iterator it = m_tmpData.begin();
		for(;it!=m_tmpData.end();it++)
		{
			Tile*tile = (*it).second;
			int key = (*it).first;
			tile->init();//opengl里面的操作，产生纹理对象，绑定纹理状态
			m_Cachemap[key] = tile;
		}
	}

	//控制缓存大小
	if (m_Cachemap.size()>2000)
	{
		map<long, Tile*>::iterator it = m_Cachemap.begin();
		for (;it!=m_Cachemap.end();it++)
		{
			long tmpkey = (*it).first;
			Tile* tile = (*it).second;
			if (tile!=NULL)//如果该对象不是当前要显示的则可以删除
			{
				//原来的code是：tile->m_rowid<m_areashow[0])||(tile->m_rowid>m_areashow[1] 这个范围写反了，导致图片删错
				//造成的后果就是 当前正在显示的图片，被delete了，这样就造成了地图的显示出现了缺图现象
				if (tile->m_js != js || (tile->m_rowid<m_areashow[1])||(tile->m_rowid>m_areashow[0])   
					|| (tile->m_colid<m_areashow[2])||(tile->m_colid>m_areashow[3]))
				{
					delete tile;
					tile = NULL;
					m_Cachemap.erase(it); //map的erase操作 是返回void的，不是 iterator
					if (m_Cachemap.size()<2000)
					{
						break;
					}
					else
					{
						it = m_Cachemap.begin();
					}
				}

			}
		} // 遍历处理完，容器中所有的[图片]
	}

	m_CenLon = cenlon;//当前中心点
	m_CenLat = cenlat;
	m_LeftTop_Lon = letflon;//整个图层的左上角坐标
	m_LeftTop_Lat = Toplat;
	m_Lon = 360;//m_Scale2ImageW*dresolution;
	m_Lat = 360;//m_Scale2ImageH*dresolution;

	m_Scale = scale;
	m_resolution = dresolution;

	return LayerTileVector; //返回装有 已经解码的Tile的vector，下面就是遍历这个容器，并使用oges绘制
}

//从下载工具
bool CMapServerLayer::DownLoadTheTile(std::vector<MapTileLocation> theReturnLocation,CString path)
{
	int g_downloadNum = 0;
	bool m_bSuceess;

	int maxConnnect = 100;

	std::string MapUrlStringFormat = "%s/Maps/%s/EzMap?Service=getImage&Type=RGB&Col=%d&Row=%d&Zoom=%d&V=0.3";
	//sprintf(urlStr,"%s?services=%s&row=%d&col=%d&level=%d&rowModel=a",m_mapServerUrl.c_str(),mapname.c_str(),j,i,js);

	//std::string MapUrlStringFormat = "%s/maprequest?services=%s&level=%d&row=%d&col=%d&errorType=icon";

	int currentConnect = 0;
	char theUrlChar[1024] = {0};

	CurlConnect* theCurlConnect = CurlConnect::GetInstance();
	CURL* CurlArray[MAX_MAP_TEXTURE_TEMP];
	std::string	theContext[MAX_MAP_TEXTURE_TEMP];
	CURLM* m_mulit_curl_ = NULL;
	std::string theConnectString = "";
	unsigned int theTimeOut = 3000;
	

	USES_CONVERSION;
	string mapname = W2A(m_MapName.GetBuffer(0));/*"raster"*/
	m_MapName.ReleaseBuffer();
	string TitleType = W2A(m_TitleType.GetBuffer(0));/*"jpg";*/
	m_TitleType.ReleaseBuffer();
	string dataPath = W2A(path.GetBuffer(0));
	path.ReleaseBuffer();

	for(int i = 0; i < theReturnLocation.size();)
	{
		if(currentConnect == 0)
			m_mulit_curl_ = curl_multi_init();

		if(currentConnect < maxConnnect)
		{
			int zoom = m_js;
			sprintf(theUrlChar, MapUrlStringFormat.c_str(), m_mapServerUrl.c_str(), mapname.c_str(),
				theReturnLocation[i].Col_, theReturnLocation[i].Row_,zoom);

			theConnectString = theUrlChar;

			CurlArray[currentConnect] = theCurlConnect->curl_easy_handler(theConnectString, theContext[currentConnect], theTimeOut);
			curl_multi_add_handle(m_mulit_curl_, CurlArray[currentConnect]);

			i++;
			currentConnect++;
		}


		if(currentConnect >= maxConnnect || i >= theReturnLocation.size())
		{
			theCurlConnect->WateForMulitCurl(m_mulit_curl_);//从网络获取到数据

			for (int idx = 0; idx < currentConnect; ++idx)
				curl_multi_remove_handle(theCurlConnect, CurlArray[idx]);

			for (int idx = 0; idx < currentConnect; ++idx)
				curl_easy_cleanup(CurlArray[idx]);

			curl_multi_cleanup(m_mulit_curl_);//theCurlConnect


			int lostNumber = 0;
			// save to file
			{
				std::string fileRowName = dataPath;
				std::string fullFileName = "";
				char fileName[1024] = {0};
				std::string stringFileName = "";
				int idx = 0;
				int currentRow = -1;
				for(int j = i - currentConnect; j < i; j++)
				{
					if(currentRow != theReturnLocation[j].Row_)
					{
						currentRow = theReturnLocation[j].Row_;
						sprintf(fileName, "%sR%d\\", dataPath.c_str(), currentRow);
						if(!PathIsDirectoryA(fileName))
							CreateDirectoryA(fileName, NULL);
						fileRowName = fileName;
					}
					sprintf(fileName, "%sC%d.%s", fileRowName.c_str(), theReturnLocation[j].Col_,TitleType.c_str());
					fullFileName = fileName;

					//保存数据
					if(theContext[idx].length() > 0)
					{
						if (theContext[idx][0]!='<')
						{
							FILE* theFile;
							theFile = fopen(fullFileName.c_str(), "wb+");
							int SaveSize = fwrite(theContext[idx].c_str(), theContext[idx].length(), 1, theFile);
							if (SaveSize != 1)
							{
								int error = GetLastError();
								if (error==112)
								{
									::MessageBoxA(NULL,"保存地图的磁盘空间不足！","Error",0);
								}
								else
								{
									CString errorinf;
									errorinf.Format(L"图片保存错误，Error=%d !",error);
									::MessageBox(NULL,errorinf,L"Error",0);
								}

								fclose(theFile);

								return 0;
							}
							fflush(theFile);
							fclose(theFile);
							g_downloadNum++;						
						}
						else
						{
							m_bSuceess = false;
						}

					}
					else
					{
						lostNumber++;
						theReturnLocation.push_back(theReturnLocation[j]);			// need redownload
					}
					idx++;
				}
			}
			currentConnect = 0;
		}
	}

	return 1;
}

vector<Tile*> CMapServerLayer::GetGeobeansDataEx(int js,double cenlon,double cenlat,double WScreen,double HScreen,long& ditX,long& ditY,long& Wcount,long & Hcount)
{
	ServerMapData.clear();

	CURL *curl[1000];//最多1000个url
	char* url[1000];
	int xx = 0;

	vector<Tile*> LayerTileVector;
	double lon ;
	double lat ;
	double width ;
	double height;
	double letflon;
	double Toplat;
	double scale;
	int ndpi;
	double dresolution;

	GetMapConfigInfo(js,letflon,Toplat,m_PicH,m_PicW,ndpi,dresolution,scale);
	//g_scale = scale;

	//中心点对应的行列
	double tileWidth = m_PicW * dresolution;
	double tColCenter = (int)floor((cenlon - letflon) / tileWidth);
	//lon = letflon + tColCenter*tileWidth;

	double tileHeight = m_PicH * dresolution;
	double tRowCenter = (int)floor((cenlat - Toplat) / tileHeight);
	//lat = Toplat - tRowCenter*tileHeight;

	Wcount = WScreen / m_PicW ;
	Hcount = HScreen / m_PicH; 

	double picsizeW = (double)m_PicW;
	double picsizeH = (double)m_PicH;
	double pixX = (cenlon - letflon)/dresolution;
	double pixY = ((cenlat - Toplat)/ dresolution);

	int startx =(pixX-WScreen/2)/picsizeW;//lie
	int endx = (pixX+WScreen/2)/picsizeW;
	int starty = (pixY+HScreen/2)/picsizeH; //hang
	int endy = (pixY-HScreen/2)/picsizeH;

	m_areashow[0] = starty>0?starty:0;//hang
	m_areashow[1] = endy>0?endy:0;
	m_areashow[2] = startx>0?startx:0;//lie
	m_areashow[3] = endx>0?endx:0;

	int xmin = m_areashow[2];
	int ymin = m_areashow[0];
	double  xC= xmin*picsizeW;//通过获取的第一张图片的左上角坐标与控件左上角坐标进行差值获取偏移
	ditX = (pixX-WScreen/2) - xC;
	double  yc= ymin*picsizeH;
	ditY = (pixY+HScreen/2) - yc;

	ditX = (pixX - WScreen / 2.0) - xmin * picsizeW;
	ditY = picsizeH - ((pixY + HScreen / 2.0) - ymin * picsizeH);

	m_tmpData.clear();

	int TitleNum = 0;

	USES_CONVERSION;
	string mapname = W2A(m_MapName.GetBuffer(0));/*"raster"*/
	m_MapName.ReleaseBuffer();
	string TitleType = W2A(m_TitleType.GetBuffer(0));/*"jpg";*/
	m_TitleType.ReleaseBuffer();

	//i表示列数，j表示行数
	CString strJs;
	strJs.Format(L"%s\\L%02d\\",m_sPath,js);
	if(!PathIsDirectory(strJs))
	{
		CreateDirectory(strJs,NULL);//如果本地没有该层图片,则先创建文件夹
	}

	std::vector<MapTileLocation> theRequestTile;

	//组url:把所要请求的tile的url 都存在一个数组内
	for (int j = m_areashow[0]; j>=m_areashow[1] ;j--)
	{
		CString strRow;
		strRow.Format(L"%sR%d",strJs,j);
		if(!PathIsDirectory(strRow))
		{
			CreateDirectory(strRow,NULL);//创建行文件夹
		}

		for(int i = m_areashow[2]; i<=m_areashow[3] ;i++)
		{	
			CString strCol = L"";
			strCol.Format(L"\\C%d",i);

			CString pStrFullPath =  strRow + strCol + "." +  m_TitleType;//图片格式png,jpg等
			bool bexistsTile = PathFileExists(pStrFullPath);

			if ( PathFileExists(pStrFullPath))//如果本地已经有该文件则不需从服务获取数据
			{	// 地图重新下载
				Bitmap* theBitmap = Bitmap::FromFile(pStrFullPath);
				if (theBitmap->GetLastStatus() == S_OK)
				{
					DeleteObject(theBitmap);
					delete theBitmap;
					continue;
				}
				DeleteObject(theBitmap);
				delete theBitmap;
			}

			TitleNum++;

			//long key =js * 10000 + (i ) * 100 + j;
			
			MapTileLocation theCurrentLocation;
			theCurrentLocation.js_ = js;
			theCurrentLocation.Row_ = j;
			theCurrentLocation.Col_ = i;
			theRequestTile.push_back(theCurrentLocation);
		}
	}

	//请求数据
	if (theRequestTile.size() != 0 && !m_localornet)
	{
		DownLoadTheTile(theRequestTile,strJs);
	}
	

	//从服务获取到数据以后:
	for (int j = m_areashow[0]; j>=m_areashow[1] ;j--)
	{
		CString strRow;
		strRow.Format(L"%sR%d",strJs,j);

		for(int i = m_areashow[2]; i<=m_areashow[3] ;i++)
		{	
			bool bexistsTile = true;
			CString strCol = L"";
			strCol.Format(L"\\C%d",i);

			CString pStrFullPath =  strRow + strCol + "." + m_TitleType;//图片格式jpg

			//获取数据  由hash的方式来取数据
			long key =js * 10000 + (i ) * 100 + j;

			Vertex verts[4] = {};
			verts[0].tu = 0;
			verts[0].tv = 0;
			verts[0].loc[0] = (i-xmin)*m_PicW-ditX;
			verts[0].loc[1] = (ymin-j)*m_PicH-ditY;
			verts[0].loc[2] = 0;
			verts[0].color[0] = 255;
			verts[0].color[1] = 0;
			verts[0].color[2] = 0;
			verts[3].color[3] = 200;

			verts[1].tu = 1;
			verts[1].tv = 0;
			verts[1].loc[0] = (i-xmin)*m_PicW-ditX + m_PicW;
			verts[1].loc[1] = (ymin-j)*m_PicH-ditY;
			verts[1].loc[2] = 0;
			verts[1].color[0] = 100;
			verts[1].color[1] = 0;
			verts[1].color[2] = 0;
			verts[3].color[3] = 0;

			verts[2].tu = 1;
			verts[2].tv = 1;
			verts[2].loc[0] = (i-xmin)*m_PicW-ditX + m_PicW;
			verts[2].loc[1] = (ymin-j)*m_PicH-ditY + m_PicH;
			verts[2].loc[2] = 0;
			verts[2].color[0] = 100;
			verts[2].color[1] = 0;
			verts[2].color[2] = 0;
			verts[3].color[3] = 0;

			verts[3].tu = 0;
			verts[3].tv = 1;
			verts[3].loc[0] = (i-xmin)*m_PicW-ditX;
			verts[3].loc[1] = (ymin-j)*m_PicH-ditY + m_PicH;
			verts[3].loc[2] = 0;
			verts[3].color[0] =255;
			verts[3].color[1] = 0;
			verts[3].color[2] = 0;
			verts[3].color[3] = 200;

			//判断内存缓冲池中是否有该图片
			Tile* tile = NULL; //表示
			map<long,Tile*>::iterator It;
			It = m_Cachemap.find(key);

			if(It != m_Cachemap.end())
			{
				tile = It->second;
				tile->SetVertex(verts);// 设置[贴图图元的坐标] 和 [纹理坐标]
			}
			else
			{
				if (bexistsTile)
				{
					tile = new Tile(pStrFullPath,verts,m_PicW,m_PicH,m_TitleType,js,j,i);
					tile->m_sformat = GL_RGBA;
					m_tmpData[key] = tile;	 //这个里面的只是保存了 还在磁盘上 jpg的路径，需要读入内存并解码
				}					
			}
			LayerTileVector.push_back(tile);
		}
	}


	int datasize = m_tmpData.size();
	if (datasize>0)
	{
		DWORD start = GetTickCount();
		//多线程解码jpg		
		if (datasize >= 6)
		{
			DecodeTile();
		}
		else
		{
			decodetileSingleT();
		}
		DWORD end = GetTickCount() - start;
		map<long,Tile*>::iterator it = m_tmpData.begin();
		for(;it!=m_tmpData.end();it++)
		{
			Tile*tile = (*it).second;
			int key = (*it).first;
			tile->init();
			//将 解码后的 图片，缓存起来
			m_Cachemap[key] = tile;
		}
	}

	//控制缓存大小
	if (m_Cachemap.size()>1000)//要删除哪张图片,需要
	{
		map<long, Tile*>::iterator it = m_Cachemap.begin();
		for (;it!=m_Cachemap.end();it++) // 枚举检查，缓存中的每一张图片
		{
			long tmpkey = (*it).first;
			Tile* tile = (*it).second;
			if (tile!=NULL)//如果该对象不是当前要显示的则可以删除
			{
				if (tile->m_js != js || (tile->m_rowid>m_areashow[1])||(tile->m_rowid<m_areashow[0])
					|| (tile->m_colid<m_areashow[2])||(tile->m_colid>m_areashow[3]))
				{
					delete tile;
					tile = NULL;
					m_Cachemap.erase(it);
					if (m_Cachemap.size()<1000)
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

	m_CenLon = cenlon;//当前中心点
	m_CenLat = cenlat;
	m_LeftTop_Lon = letflon;//整个图层的左上角坐标
	m_LeftTop_Lat = Toplat;
	m_Lon = 256;//m_Scale2ImageW*dresolution;
	m_Lat = 256;//m_Scale2ImageH*dresolution;

	m_Scale = scale;
	m_resolution = dresolution;

	return LayerTileVector;
}

void CMapServerLayer::DrawLayer(vector<Tile*> vctTile,int ditx,int dity,bool m_realtime)
{
	int index = 0;
	if (m_MapType==GeoBeansServerMap || m_MapType==PGISServerMap)
	{
		for (int i = m_areashow[0];i>= m_areashow[1] ;i--)
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
			}
		}
	}
	else
	{
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
			}
		}
	}


	return;
}

// 地图的绘制 主函数
void CMapServerLayer::Draw()
{
	if (!m_bconnect)  //远程服务连接失败，不执行下面取地图数据，已经绘制地图
	{
		return;
	}

	long ditx = 0;
	long dity = 0;
	long wCount = 0;
	long hCount = 0;

	//DWORD s = GetTickCount();
	if (m_bUpdateData)
	{
		if (m_MapType==GeoBeansServerMap)  //绘制秦皇岛
		{
			//vector<Tile*> m_pTile 装载图片的 容器
			//计算出在当前窗口中需要显示的图片文件，读入内存并解码成bitmap,计算出纹理贴图时几何图元的坐标值
			// 其中将图片读入内存，有需要从远程服务上取图片，并且设立了 图片内存的缓存机制
			m_pTile = GetGeobeansData(m_js,m_x,m_y,g_CtrlW,g_CtrlH,ditx,dity,wCount,hCount);  
			
		}
		else if(m_MapType==PGISServerMap)
		{
			m_pTile = GetGeobeansDataEx(m_js,m_x,m_y,g_CtrlW,g_CtrlH,ditx,dity,wCount,hCount);
		}
		else
		{
			m_pTile = GetData(m_js,m_x,m_y,g_CtrlW,g_CtrlH,ditx,dity,wCount,hCount);
		}
		
		m_bUpdateData = false; //置为false，表示此时的地图数据是最新的。当地图数据需要重新获取时，将该变量重置为TRUE
	}
	//DWORD e = GetTickCount()-s;

	DrawLayer(m_pTile,ditx,dity,true); //利用ES来纹理贴图 显示二维图片

	//DWORD e2 = GetTickCount() -s;
	return;
}

bool CMapServerLayer::CoonnetDegreeServer(CString serverurl)
{
	CString strSentence;
	string strjsonURL = "";
	CString strFileName = serverurl + L"/wmts?service=WMTS&request=GetCapabilities";//wmts?service=WMTS&request=GetCapabilities

	CInternetSession sess(L"VTRONSV");
	CHttpFile* fileGet;
	try
	{
		fileGet=(CHttpFile*)sess.OpenURL(strFileName);
	}
	catch(CException* e)
	{
		sess.Close();
		return false;
	}    

	if(fileGet)
	{
		DWORD dwStatus;
		DWORD dwBuffLen = sizeof(dwStatus);
		BOOL bSuccess = fileGet->QueryInfo(HTTP_QUERY_STATUS_CODE|HTTP_QUERY_FLAG_NUMBER, &dwStatus, &dwBuffLen);

		if( bSuccess && dwStatus>= 200&& dwStatus<300 ) 
		{ 
			CString data("");//获取HTML
			char* szBuff = new char[10241];
			memset(szBuff, 0, 10241);
			int nReadCount = 0;
			while((nReadCount = fileGet->Read(szBuff, 10240)) > 0)
			{
				if(nReadCount==0) break;
				szBuff[nReadCount]='\0';
				data += szBuff;

				//const char* str = szBuff;  
				memset(szBuff, 0, 10241);
			}
			TiXmlDocument *pDoc = new TiXmlDocument();    
			if (NULL==pDoc)    
			{    
				return S_FALSE;    
			}    
			int len = WideCharToMultiByte(CP_UTF8,0,data,data.GetLength(),NULL,0,NULL,NULL);
			char* pCxml = new char[len + 1];
			len = WideCharToMultiByte(CP_UTF8,0,data,data.GetLength(),pCxml,len + 1,NULL,NULL);
			pCxml[len]  = 0;
			const char* pCCxml = (const char*)pCxml;
			pDoc->Parse(pCCxml);
			TiXmlElement* pRootElet = pDoc->RootElement();

			TiXmlElement* node;
			string nodeName = "Contents";
			m_xmlwork.GetNodePointerByName(pRootElet,nodeName,node);

			//找到layer对应的matrixset
			USES_CONVERSION;
			string layername = W2A(m_MapName.GetBuffer(0));
			m_MapName.ReleaseBuffer();
			for(TiXmlElement * i = node->FirstChildElement(); i != NULL; i = i->NextSiblingElement())
			{
				const char* valuename = i->Value();
				if(strcmp(valuename,"Layer") == 0)
				{
					const char* servicename = i->FirstChildElement()->FirstChild()->Value();
					if (strcmp(servicename,layername.c_str()) == 0)
					{
						for (TiXmlElement * TileMatrixElement = i->FirstChildElement();TileMatrixElement != NULL; TileMatrixElement = TileMatrixElement->NextSiblingElement())
						{
							const char* nodevalue = TileMatrixElement->Value();
							if(strcmp(nodevalue,"TileMatrixSetLink") == 0)
							{
								 m_TileMatrixSetname = TileMatrixElement->FirstChildElement("TileMatrixSet")->FirstChild()->Value();
							}

							else if(strcmp(nodevalue,"Format") == 0)
							{
								const char* tiletype = TileMatrixElement->FirstChild()->Value();
								CString stiletp(tiletype);
								m_TitleType = stiletp.Right(stiletp.ReverseFind('/')-2);
								
							}
						}
						break;
					}


				}
			}
			

			//找到layer
			for(TiXmlElement * i = node->FirstChildElement(); i != NULL; i = i->NextSiblingElement())
			{
				const char* valuename = i->Value();
				if(strcmp(valuename,"TileMatrixSet") == 0)
				{
					const char* servicename = i->FirstChildElement()->FirstChild()->Value();
					if (strcmp(servicename,m_TileMatrixSetname) == 0)
					{
						for (TiXmlElement * TileMatrixElement = i->FirstChildElement();TileMatrixElement != NULL; TileMatrixElement = TileMatrixElement->NextSiblingElement())
						{
							const char* nodevalue = TileMatrixElement->Value();
							if(strcmp(nodevalue,"TileMatrix") == 0)
							{
								const char* cLevel = TileMatrixElement->FirstChildElement()->FirstChild()->Value();
								const char* cScaleDenominator = TileMatrixElement->FirstChildElement("ScaleDenominator")->FirstChild()->Value();
								const char* cTopLeftCorner = TileMatrixElement->FirstChildElement("TopLeftCorner")->FirstChild()->Value();
								const char* cTileWidth = TileMatrixElement->FirstChildElement("TileWidth")->FirstChild()->Value();
								const char* cTileHeight = TileMatrixElement->FirstChildElement("TileHeight")->FirstChild()->Value();
								const char* cMatrixWidth = TileMatrixElement->FirstChildElement("MatrixWidth")->FirstChild()->Value();
								const char* cMatrixHeight = TileMatrixElement->FirstChildElement("MatrixHeight")->FirstChild()->Value();
								const char* cMatrixResolution = TileMatrixElement->FirstChildElement("Resolution")->FirstChild()->Value();
								int ScaleDenominator = atoi(cScaleDenominator);

								int Level = atoi(cLevel);

								if (Level == m_js)
								{
									m_PicSizeW = atoi(cTileWidth);
									m_PicSizeH = atoi(cTileHeight);
									m_Scale2ImageW = atoi(cMatrixWidth) * m_PicSizeW;
									m_Scale2ImageH = atoi(cMatrixHeight) * m_PicSizeH;
									m_resolution = atof(cMatrixResolution);

									char *p;  
									p = strtok((char*)cTopLeftCorner, " ");
									bool IsReal = false;
									while(p)  
									{   
										if (!IsReal)
										{
											m_OriginX = atof(p);
											IsReal = true;
										}
										else 
										{
											m_OriginY = atof(p);
										}
										p = strtok(NULL, ","); 
									}    

								}

								MapLayerData* pLayerData = new MapLayerData();
								pLayerData->Level = Level;
								double presolution = 0;		
								pLayerData->Resolution = m_resolution;
								pLayerData->Scale = ScaleDenominator;
								m_LayerDataList[Level] = pLayerData; 

							}
						}
						break;
					}
				}
			}


			delete[] pCxml;
			pCxml = NULL;
			delete[] szBuff;
			szBuff = NULL;

		}
		else 
		{
		}
		fileGet->Close();
		delete fileGet;
		fileGet = NULL;
	}

	sess.Close();

	curl_global_init(CURL_GLOBAL_ALL);     
	m_multi_handle=curl_multi_init();   

	return true;
}

bool CMapServerLayer::CoonnetGeoBeansServer(CString serverurl,double r0)
{

	curl_global_init(CURL_GLOBAL_ALL);     
	m_multi_handle=curl_multi_init();   

	//计算分辨率 后续需要从服务获取
	double pResolution = r0*2;   // 156543.04/2/3.1415926/6378137*360;
	for(int i = 0; i < 20; i++)
	{
		pResolution /=2;

		MapLayerData* pLayerData = new MapLayerData();
		pLayerData->Level = i;
		double presolution = 0;		
		pLayerData->Resolution = pResolution;
		pLayerData->Scale =(pResolution*(2*3.1415926*6378137/360)*96/0.0254) ; //比例尺
		m_LayerDataList[i] = pLayerData; 
	}
	m_PicSizeW = 256;
	m_PicSizeH = 256;
	//m_Scale2ImageW = atoi(cMatrixWidth) * m_PicSizeW;
	//m_Scale2ImageH = atoi(cMatrixHeight) * m_PicSizeH;
	m_resolution = m_LayerDataList[m_js]->Resolution;
	return true;
}
