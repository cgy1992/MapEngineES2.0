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
//������ �������� ��������Զ�� ͼƬ�� Ŀ¼
CArcgisMapLayer::CArcgisMapLayer(double cenX,double cenY,int js,CString mapname,int picW,int picH,CString path,CString pictype,DrawEngine *den)
{
	m_x = cenX;//���ĵ�
	m_y = cenY;
	m_js = js; //��ǰ��ͼ��ʾ����
	m_PicW = picW;//��Ƭ�����һ�µ�
	m_PicH = picH;
	m_Lon = 0.0;
	m_Lat = 0.0;
	m_MapName = mapname;//��ͼ��
	m_Scale2ImageW = -1;
	m_Scale2ImageW = -1;
	m_resolution = 0.0; // ��ͬLevel�ļ���ĵ�ͼӦ��
	m_MapType = MapType::ArcgisMap; //��ͼ����
	m_layerType=VLayerType::vBaseLayer; //ͼ������
	m_TitleType = pictype;  //��Ƭ����

	g_CtrlW = den->g_CtrlW;  //��������������ڵĴ�С
	g_CtrlH = den->g_CtrlH;
	//����������m_sPath
	//m_sPath = path;//��ͼ·��������Ƿ��񷢲������ݣ���ΪURL
	CString serverurl = path;
	USES_CONVERSION; 
	//m_mapServerUrl = W2A(serverurl.GetBuffer(0));
	serverurl.ReleaseBuffer();
	int pos = path.Find(L"http:");
	if ( pos >= 0)
	{
		m_localornet = false;
	}
	else   // ����Ĵ����߼���ʾ�������ͼ������local�Ͳ���Ҫ ʹ�ù���ı���·����ֱ����path
	{
		m_localornet = true;   //����ͼƬ
		m_sPath = path;     // ��·��ʼ��ָ�� ������·�������path��URL����ô����ָ������[��������ı���·��]
	}
	m_bUpdateData = true; //�������µ��õ�ͼ���� ,���������ݺ�GetData��bool���� �ᱻ��Ϊfalse
	
	if(!m_localornet)  //���Ǳ���·�����ȴ��� ��ͼ�ڴ��̵�rootĿ¼������conf.xml�ļ����ص�����
	{
		CString szExePath = L"";
		//�õ���ģ��Ĺ���·�����������λ��
		::GetModuleFileName(NULL,szExePath.GetBufferSetLength(MAX_PATH+1),MAX_PATH); 
		//�������
		szExePath = szExePath.Left(szExePath.ReverseFind('\\'));
		szExePath = szExePath.Left(szExePath.ReverseFind('\\'));

		CString szMap = L"";
		CString url(m_MapName);
		//szMap.Format(L"%s\\MAP\\%s\\Layers\\_allayers",szExePath,url);//m_LayerName
		szMap.Format(L"%s\\MAP",szExePath);// ���챾�ش��ͼƬ��·��  C:\SmartCity\MAP
		bool bc = false;
		int err;
		if(!PathIsDirectory(szMap))
		{
			bc = CreateDirectory(szMap,NULL);   //����������򴴽���Ŀ¼
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
		m_sPath = szMapPath;  //����·�� D:\Chrome\MAP\ningbo_vector\Layers\_allayers
	}

}

/*
*param[in] strXmlPath:xml·��
*param[in] djs:��ͼ����
*param[in��out] lfTileOriginValueX:x
*param[in��out] lfTileOriginValueY:y
*param[in��out] dTileCols:��Ƭ��
*param[in��out] dTileRows:��Ƭ��
*param[in��out] dDPI:ÿӢ������
*param[in��out] lfResolution:�ֱ���
*param[in��out] lScale:������
*/

//overload��������� ��ʹ����JSON�л�ȡ��Ϣ
//������ͼ�������ļ� conf.xml,ǰ��������������Ϊ���������룬������Ϊ��ȡ�� [������Ϣ] ��Ϊ���
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

		if (!pDoc->LoadFile(strxml))//ͼ��xml·�����ӱ����ļ��� ���ص�ͼ�����ļ�
		{
			return;
		}

		TiXmlElement* pRootElet = pDoc->RootElement();

		//��ȡ��ͼ�Ļ�����Ϣ
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


		//��ȡ��Ӧ����Ķ�Ӧ��Ϣ��scale Resolution
		TiXmlElement* node;
		string nodeName = "LODInfos";
		m_xmlwork.GetNodePointerByName(pRootElet, nodeName, node);
		TiXmlAttribute* nodeAttribute = node->FirstAttribute();
		const char *value = nodeAttribute->Value();//����
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
				//��λ��xml����Ӧ����
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
		pDoc->~TiXmlDocument(); // ��ʾ��invoke��������
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

			if (reader.parse(fjson, root))  // reader��Json�ַ���������root��root������Json��������Ԫ��   
			{
				//��ȡ��Ƭ��С
				dTileRows = root["tileInfo"]["rows"].asInt();
				dTileCols = root["tileInfo"]["cols"].asInt();

				lfTileOriginX = root["tileInfo"]["origin"]["x"].asInt();;
				lfTileOriginY = root["tileInfo"]["origin"]["y"].asInt();

				Json::Value plods = root["tileInfo"]["lods"];
				int pSize = plods.size(); //��ͼ���ж��ٲ� Level
				dDPI = root["tileInfo"]["dpi"].asInt();
				string format = root["tileInfo"]["format"].asString();
				if (format.compare("PNG32") == 0)
				{
					m_TitleType = L"png";
				}

				// ��������  
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
					////��ͬLevel�ĵ�ͼ��Ϣ����ؼ��ľ��� �ֱ���
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
	vector<Tile*> LayerTileVector; //���ڽ�����أ���ʾ��ǰ�����У�������ʾ��ͼƬ
	m_tmpData.clear();  //���ڴ�ţ���ͼˢ�º����������ݣ����ڵ�ǰ��������Ҫ��ʾ��ͼƬ

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


	//ǰ��������������Ϊ���룬����Ĳ�����Ϊ�����������
	/*
	  �����������Ǵӵ�ͼ�������ļ� conf.xml�� ��ȡ��ͼ�㼶Ϊjs��  ����߶����[��γ��] �� [�ò��ķֱ���]
	*/
	//GetMapConfigInfo(MapPath + "\\Layers\\conf.xml",js,letflon,Toplat,m_PicH,m_PicW,ndpi,dresolution,scale);  //ǰ�������� ���û������ ����ֵ
	GetMapConfigInfo(MapPath, js, letflon, Toplat, m_PicH, m_PicW, ndpi, dresolution, scale);  //ǰ�������� ���û������ ����ֵ

	m_sPath.ReleaseBuffer();
	
	//g_scale = scale;
	
	//���ĵ��Ӧ������:��=1��ʾR00000001,���ڶ��У�������
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
	double  xC= xmin*picsizeW;//ͨ����ȡ�ĵ�һ��ͼƬ�����Ͻ�������ؼ����Ͻ�������в�ֵ��ȡƫ��
	ditX = (pixX-WScreen/2) - xC;
	double  yc= ymin*picsizeH;
	ditY = (pixY-HScreen/2) - yc;
	//i��ʾ������j��ʾ����
	CString strJs;
	string Path = m_sPath + "\\Layers\\_alllayers\\";
	CString strdir;
	strdir	=LPCSTR(Path.c_str());//Ŀ¼
	strJs.Format(L"%sL%02d\\",strdir,js);

	for (int j = m_areashow[0]; j<=m_areashow[1] ;j++)
	{
		for(int i = m_areashow[2]; i<=m_areashow[3] ;i++)
		{	
			
			CString strRow = L"";
			strRow.Format(L"R%08x\\",j);
			CString strCol = L"";
			strCol.Format(L"C%08x",i);
			
			CString pStrFullPath = strJs + strRow + strCol + "." +  m_TitleType;//ͼƬ��ʽjpg

			Vertex verts[4] = {}; //������ֱ�ӳ�ʼ��Ϊ��
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


			//�ж��ڴ滺������Ƿ��и�ͼƬ
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
				if (PathFileExists(pStrFullPath)) //��֤ͼƬ�Ƿ����
				{
					tile = new Tile(pStrFullPath,verts,m_PicW,m_PicH,m_TitleType,js,j,i);  //invoke �����decode����ʱ���ŻὫͼƬ�����ڴ�
					tile->m_sformat = GL_RGB;
					m_tmpData[key] = tile;
				}				
			}
			LayerTileVector.push_back(tile);
			
		}
	}

	int datasize = m_tmpData.size();
	
	if (datasize>0)  //����Ĺ��̰����� ͼƬ�����ڴ棬������Ϊbitmap
	{
		//���߳̽���jpg		
		if (datasize>=6)
		{
			DecodeTile();
		}
		else
		{
			//���߳̽���
			decodetileSingleT();
		}

		//�����Ժ�ini�н���������ͼ����󱣴浽m_Cachemap��
		map<long,Tile*>::iterator it = m_tmpData.begin();
		for(;it!=m_tmpData.end();it++)
		{
			Tile*tile = (*it).second;
			int key = (*it).first;
			tile->init(g_context); //���ú�����״̬���������ʶDrawEngine��g_context
			delete m_Cachemap[key];
			m_Cachemap[key] = tile;
		}
	}
	//int map_buff_size = 
	//���ƻ����С��ɾ��tile�Ļ��ƣ�һ���ŵ�ɾ��Ч�ʲ���
	if (m_Cachemap.size()>1000)//Ҫɾ������ͼƬ,��Ҫ
	{
		map<long, Tile*>::iterator it = m_Cachemap.begin();
		for (;it!=m_Cachemap.end();it++)
		{
			long tmpkey = (*it).first;
			Tile* tile = (*it).second;
			if (tile!=NULL)//����ö����ǵ�ǰҪ��ʾ�������ɾ��
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
	//�ر���־���
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
		//curTileInfo.Empty(); //���
	}
	//gLog.append(_T("############################################################"));
	return;
}

//��ͼ�������ƺ����߼�����ȡ����---�ٻ�ͼ
void CArcgisMapLayer::Draw()
{
	long ditx = 0;  //������ Ӧ����256
    long dity = 0;
	long wCount = 0;
	long hCount = 0;
	
	if (m_bUpdateData)
	{
		m_pTile = GetData(m_js,m_x,m_y,g_CtrlW,g_CtrlH,ditx,dity,wCount,hCount);  //��ȥ ȡ����
		m_bUpdateData = false;
	}
	DrawLayer(m_pTile,ditx,dity,true);

	return;
}
//����ʹ��
void CArcgisMapLayer::Draw(int g_context)
{
	long ditx = 0;  //������ Ӧ����256
	long dity = 0;
	long wCount = 0;
	long hCount = 0;

	if (m_bUpdateData)
	{
		m_pTile = GetData(m_js,m_x,m_y,g_CtrlW,g_CtrlH,ditx,dity,wCount,hCount,g_context);  //��ȥ ȡ����
		m_bUpdateData = false;
	}
	DrawLayer(m_pTile,ditx,dity,true);

	return;
}
//��ȡ��ͼ�� ��Ա����
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
	resolution = m_resolution; //��ǰ�ֱ���
};

//���߳̽���
void CArcgisMapLayer::DecodeTile()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	DWORD start = GetTickCount();
	HANDLE hdl[g_countT];
	hdl[0]=CreateThread(NULL,NULL,&Datathread1,  (LPVOID)this,NULL,NULL);
	hdl[1]=CreateThread(NULL,NULL,&Datathread2,  (LPVOID)this,NULL,NULL);
	hdl[2]=CreateThread(NULL,NULL,&Datathread3,  (LPVOID)this,NULL,NULL);
	hdl[3]=CreateThread(NULL,NULL,&Datathread4,  (LPVOID)this,NULL,NULL);
	hdl[4]=CreateThread(NULL,NULL,&Datathread5,  (LPVOID)this,NULL,NULL);
	hdl[5]=CreateThread(NULL,NULL,&Datathread6,  (LPVOID)this,NULL,NULL);
	Sleep(0);
	WaitForMultipleObjects(g_countT,hdl,TRUE,INFINITE); //�ȴ����е� �߳����
	DWORD end = GetTickCount() - start;
}

//��6��thread_routine ���Լ�Ϊ��������ÿ��thread_routine����һ��IDֵ����ʶ���Ǵ���ķ�Χ
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
		HBITMAP hBitMap;  //λͼ���
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

			for (int i=0;i<w*h;i++)//��ɫ����תrgb
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

			for (int i=0;i<w*h;i++)//��ɫ����תrgb
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

			for (int i=0;i<w*h;i++)//��ɫ����תrgb
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

			for (int i=0;i<w*h;i++)//��ɫ����תrgb
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

			for (int i=0;i<w*h;i++)//��ɫ����תrgb
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

			for (int i=0;i<w*h;i++)//��ɫ����תrgb
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

//���߳̽���
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

			for (int i=0;i<w*h;i++)//��ɫ����תrgb
			{	
				int id = pdata[i];
				rgbdata[i*3+0] = prgbColors[id].rgbRed; //r
				rgbdata[i*3+1] = prgbColors[id].rgbGreen; //g
				rgbdata[i*3+2] = prgbColors[id].rgbBlue; //b
				//rgbdata[i*dep+3] = 255; //��ɫ������Aֵ�̶�
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