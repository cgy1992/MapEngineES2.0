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

//��Զ�̷�����ȡ ������ ʹ�õĲ���
CMapServerLayer::CMapServerLayer(double cenX,double cenY,int js,CString mapname,int picW,int picH,CString path,CString pictype,CString ServerType)
{
	m_x = cenX;//���ĵ� 120.14894, 30.26827, 0
	m_y = cenY;
	m_js = js;
	m_PicW = picW;//��Ƭ�����һ�µ�
	m_PicH = picH;
	m_Lon = 0.0;
	m_Lat = 0.0;
	m_MapName = mapname;//��ͼ��
	m_TitleType = pictype;
	m_Scale2ImageW = -1;
	m_Scale2ImageW = -1;
	m_resolution = 0.0;	
	m_layerType=VLayerType::vBaseLayer;
	m_LayerName = mapname;

	//ͼ��·��
	CString szExePath = L"";
	::GetModuleFileName(NULL,szExePath.GetBufferSetLength(MAX_PATH+1),MAX_PATH);
	szExePath = szExePath.Left(szExePath.ReverseFind('\\'));
	szExePath = szExePath.Left(szExePath.ReverseFind('\\'));

	//����GIS_MAP�ļ���
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

	m_bconnect = false;//0��ʾ���Ӳ���
	if (ServerType == "ArcServer")
	{
		m_MapType = MapType::ArcgisServerMap;
		m_bconnect = ConnetArcServer(serverurl); //��Զ�̷����ϣ���ȡ��ͼ config.json
	}
	else if (ServerType == "DegreeServer")
	{
		m_MapType = MapType::DegreeServermap;
		m_bconnect =  CoonnetDegreeServer(serverurl);
	}
}

//�ӱ��ش����� ȡ��ͼ����,[����� GeoBeansMap ʹ�ø÷���]  �ҽ��޸�Ϊ�����Ǵ�Զ�̷����Ǳ��� ��ʹ�����������
CMapServerLayer::CMapServerLayer(double cenX,double cenY,int js,CString mapname,int picW,int picH,CString path,CString pictype,CString ServerType,double minlon,double minlat,double resolution0,DrawEngine *den)
{
	m_x = cenX;//���ĵ� 120.14894, 30.26827, 0
	m_y = cenY;
	m_js = js;
	m_PicW = picW;//��Ƭ�����һ�µ�
	m_PicH = picH;
	m_Lon = 0.0;
	m_Lat = 0.0;
	m_MapName = mapname;//��ͼ��
	m_TitleType = pictype;
	m_Scale2ImageW = -1;
	m_Scale2ImageW = -1;
	m_resolution = 0.0;	
	m_layerType=VLayerType::vBaseLayer;
	m_LayerName = mapname;
	m_OriginX = minlon;
	m_OriginY = minlat;

	g_CtrlW = den->g_CtrlW;  //��������������ڵĴ�С
	g_CtrlH = den->g_CtrlH;
	//add by xuyan
	CString serverurl	= path;
	USES_CONVERSION; 
	m_mapServerUrl = W2A(serverurl.GetBuffer(0));
	serverurl.ReleaseBuffer();
	

	//ͼƬ·�� �Ƿ��� ����
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
	//pos >=0 ? m_localornet = false : m_localornet = true;
	//m_sPath = path;

	m_bUpdateData = true;  //��ʾ ��Ҫ���µ� ȡ��ͼ����
	m_bconnect = false;//0��ʾ���Ӳ���

    if (ServerType == "GeoBeansServer")
	{
		m_MapType = MapType::GeoBeansServerMap;
		//m_bconnect =  true;  modify by xuyan
		if (m_localornet)
		{
		    m_bconnect = true;
			
			//����[ǰ�˷�����Resolution0":1.406250078533376]���������L0~L19��ͬ��ͼͼ��ķֱ��ʣ�
			//�������� ��ǰ��ͼ����ķֱ��� m_resolution = m_LayerDataList[m_js]->Resolution;
			//CoonnetGeoBeansServer(serverurl,resolution0);  
			CoonnetGeoBeansServer(m_sPath,resolution0);   //modify by xuyan ����ʹ�� serverurl,�������m_sPath�ֶα�����û���õ�
		}
		else   //modify by xuyan ��
		{
		   //������Ǳ��أ�����������������Σ���,�ⲿ�ֵ��߼�����Draw�е�ȡ��ͼ����ʱ �ῼ�ǵ�����������ֻ�������ﴴ�� �û���ͼƬ�� [·��]

			//ͼ��·��
			CString szExePath = L"";
			::GetModuleFileName(NULL,szExePath.GetBufferSetLength(MAX_PATH+1),MAX_PATH); //�õ���ģ��Ĺ���·��
			szExePath = szExePath.Left(szExePath.ReverseFind('\\'));
			szExePath = szExePath.Left(szExePath.ReverseFind('\\'));

			//����GIS_MAP�ļ���
			CString szMap = L"";
			CString url(m_LayerName);
			//szMap.Format(L"%s\\MAP\\Map_Temp\\ArcServer_MAP_%s",szExePath,url);//m_LayerName
			szMap.Format(L"%s\\MAP",szExePath);// ���챾�ش��ͼƬ��·��  C:\SmartCity\MAP

			bool bc = false;
			int err;
			if(!PathIsDirectory(szMap))
			{
				bc = CreateDirectory(szMap,NULL);   //��������򴴽���Ŀ¼
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
			m_sPath = szMapPath; //���� buff�����ĵ�ͼtile�������[����·��]

			CString LayerPath = L"";
			LayerPath.Format(L"%s\\L%0.2d",szMapPath,js);  //���� buff�����ĵ�ͼtile�������[����·��]
			if(!PathIsDirectory(LayerPath))
			{
				bc = CreateDirectory(LayerPath,NULL);
			}
		 // �Ƿ�ȱ�ٶ� CoonnetGeoBeansServer(m_sPath,resolution0);�ĵ��ã����㲻ͬjs�ķֱ��� 
		}
		
	}//�ػʵ����߼��������
	else  if (ServerType == "PGISServer")
	{
		m_MapType = MapType::PGISServerMap;
		//m_bconnect =  true; //�Ѹ��ֶη������ڲ�
		if (m_localornet)
		{
			m_bconnect = true;
			//CoonnetGeoBeansServer(serverurl,resolution0);
			CoonnetGeoBeansServer(m_sPath,resolution0);
		}
		else
		{
		   // ������Ǳ��أ��������ϣ��д���ɣ�
		}
	}
}

//���ع���
bool CMapServerLayer::ConnetArcServer(CString serverurl)
{
	int nret = 0;

	CString strSentence;
	/*"http://10.1.50.213/ArcGIS/rest/services/HangZhou/MapServer?f=json";*/  // ����һ��JSON�ַ���
	CString strFileName = serverurl + L"?f=json"; //���������ַ���  ��ȡ��ͼ�������ļ�

	CInternetSession sess(L"ARCSVR");   //MFC�е���
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
			CString data("");//��ȡHTML
			char* szBuff = new char[10241];
			memset(szBuff, 0, 10241);
			int nReadCount = 0;
			while((nReadCount = fileGet->Read(szBuff, 10240)) > 0) // ��ѭ��ֻ��ִ��һ�飬һ���Զ��� ���е�����
			{
				//int nReadLength = fileGet->Read(szBuff, 1024);
				if(nReadCount==0) break;  //���ݶ�ȡ���
				szBuff[nReadCount]='\0';
				data += szBuff;

				const char* str = szBuff;  

				Json::Reader reader;  
				Json::Value root;  
				if (reader.parse(str, root))  // reader��Json�ַ���������root��root������Json��������Ԫ��   
				{  
					//��ȡ��Ƭ��С
					m_PicSizeW = root["tileInfo"]["rows"].asInt();
					m_PicSizeH = root["tileInfo"]["cols"].asInt();
					Json::Value plods = root["tileInfo"]["lods"];
					int pSize  = plods.size(); //��ͼ���ж��ٲ� Level
					m_dpi = root["tileInfo"]["dpi"].asInt();
					string format= root["tileInfo"]["format"].asString();
					if (format.compare("PNG32") == 0)
					{
						m_TitleType = L"png";
					}

					// ��������  
					for(int i = 0; i < pSize; ++i)  
					{  
						int  plevel = plods[i]["level"].asInt();
						double  presolution = plods[i]["resolution"].asDouble();
						int  pscale = plods[i]["scale"].asInt();
						MapLayerData* pLayerData = new MapLayerData();  //��ͬLevel�ĵ�ͼ��Ϣ����ؼ��ľ��� �ֱ���
						pLayerData->Level = plevel;
						pLayerData->Resolution = presolution;
						pLayerData->Scale = pscale;
						m_LayerDataList[plevel] = pLayerData;
					}
					//��ȡextent��
					Json::Value pExtent = root["fullExtent"];
					m_Xmin = pExtent["xmin"].asDouble();
					m_Xmax = pExtent["xmax"].asDouble();
					m_Ymin = pExtent["ymin"].asDouble();
					m_Ymax = pExtent["ymax"].asDouble();

					//��ȡ���Ͻ�����
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
//	//��ɣ�http://10.1.50.213:8088/deegree-webservices-3.3.1/services/wmts?service=WMTS&request=GetCapabilities
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
//			CString data("");//��ȡHTML
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
//				if (reader.parse(str, root))  // reader��Json�ַ���������root��root������Json��������Ԫ��   
//				{  
//					//��ȡ��Ƭ��С
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
//					// ��������  
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
//					//��ȡextent��
//					Json::Value pExtent = root["fullExtent"];
//					m_Xmin = pExtent["xmin"].asDouble();
//					m_Xmax = pExtent["xmax"].asDouble();
//					m_Ymin = pExtent["ymin"].asDouble();
//					m_Ymax = pExtent["ymax"].asDouble();
//
//					//��ȡ���Ͻ�����
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
//			//strSentence.Format(L"����ҳ�ļ����������룺%d", dwStatus);
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
		lfResolution = m_LayerDataList[djs]->Resolution;//��ͼ�ֱ��� 
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

	sprintf(urlStr,"%s/%d/%d/%d",m_mapServerUrl,level,col,row);  //���춯̬���ַ���
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
size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)  //���������Ϊ�˷���CURLOPT_WRITEFUNCTION, �������
{
	buf_t *bf = (buf_t *)stream;
	memcpy(bf->buffer + bf->sz, ptr, size * nmemb);
	bf->sz += size * nmemb;
	return size * nmemb;
}

buf_t bufs[1000];   //2M���ڴ�ռ䣬ÿһ������Ԫ�ش����� һ�ŵ�ͼ��Ƭ

map<long, buf_t*> ServerMapData; //��Ŵ�Զ�̷����� ȡ��ͼƬ

vector<Tile*> CMapServerLayer::GetData(int js,double cenlon,double cenlat,double WScreen,double HScreen,long& ditX,long& ditY,long& Wcount,long & Hcount)
{
	ServerMapData.clear();  //�����ڴ�ռ䣬�����Զ�̷����϶�ȡ�� ͼƬ

	CURL *curl[1000];//���1000��url
	char* url[1000];
	int xx = 0;  //ͳ�ƴ�Զ�̷�����ȡ��ͼƬ��

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

	//���ĵ��Ӧ������:��=1��ʾR00000001,���ڶ��У�������
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
	double  xC= xmin*picsizeW;//ͨ����ȡ�ĵ�һ��ͼƬ�����Ͻ�������ؼ����Ͻ�������в�ֵ��ȡƫ��
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

	//i��ʾ������j��ʾ����
	CString strJs;
	strJs.Format(L"%s\\L%02d\\",m_sPath,js);
	if(!PathIsDirectory(strJs))
	{
		CreateDirectory(strJs,NULL);//�������û�иò�ͼƬ,���ȴ����ļ���
	}

	//��url:����Ҫ�����tile��url ������һ��������
	for (int j = m_areashow[0]; j<=m_areashow[1] ;j++)
	{
		CString strRow;
		strRow.Format(L"%sR%08x",strJs,j);
		if(!PathIsDirectory(strRow))
		{
			CreateDirectory(strRow,NULL);//�������ļ���
		}

		for(int i = m_areashow[2]; i<=m_areashow[3] ;i++)
		{	
			CString strCol = L"";
			strCol.Format(L"\\C%08x",i);

			CString pStrFullPath =  strRow + strCol + "." +  m_TitleType;//ͼƬ��ʽpng,jpg��

			if ( PathFileExists(pStrFullPath))//��������Ѿ��и��ļ�����ӷ����ȡ����
			{	// ��ͼ��������
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
				//js������path http://10.1.50.213/ArcGIS/rest/services/HangZhou/MapServer
				//��Ҫ���http://10.1.50.213/ArcGIS/rest/services/HangZhou/MapServer/tile/js/rowid/colid
				sprintf(urlStr,"%s/tile/%d/%d/%d",m_mapServerUrl.c_str(),js,j,i);
			}
			else if (m_MapType == MapType::DegreeServermap)
			{
				//js������path http://10.1.50.213:8088/deegree-webservices-3.3.1/services   ;///wmts?service=WMTS
				               http://10.1.50.213:8088/deegree-webservices-3.3.1/services/wmts?service?SERVICE=WMTS&REQUEST=GetTile&VERSION=1.0.0&LAYER=h&STYLE=default&TILEMATRIXSET=tilenatrixset_hzyx&TILEMATRIX=2&TILEROW=4&TILECOL=4&FORMAT=image%2Fpng
				//��Ҫ���http:http://10.1.50.213:8088/deegree-webservices-3.3.1/services?SERVICE=WMTS&REQUEST=GetTile&VERSION=1.0.0&LAYER=hzyxTile&STYLE=default&TILEMATRIXSET=tilematrixset_hzyx&TILEMATRIX=2&TILEROW=4&TILECOL=4&FORMAT=image%2Fpng
				sprintf(urlStr,"%s?SERVICE=WMTS&REQUEST=GetTile&VERSION=1.0.0&LAYER=%s&STYLE=default&TILEMATRIXSET=%s&TILEMATRIX=%d&TILEROW=%d&TILECOL=%d&FORMAT=image\%%2F%s",m_mapServerUrl.c_str(),mapname.c_str(),m_TileMatrixSetname,js,j,i,TitleType.c_str());//tilematrixset_hzyx

			}

			url[xx] = urlStr; 
			bufs[xx].sz = 0;
			curl_easy_setopt(curl[xx], CURLOPT_URL, url[xx]);
			curl_easy_setopt(curl[xx], CURLOPT_WRITEFUNCTION, &write_data);
			curl_easy_setopt(curl[xx], CURLOPT_WRITEDATA, &bufs[xx]);  //����ͼƬ����[���ٵ��ڴ�ռ�]
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

	//�ӷ����ȡ�������Ժ�:
	for (int j = m_areashow[0]; j<=m_areashow[1] ;j++)
	{
		CString strRow;
		strRow.Format(L"%sR%08x",strJs,j);
	
		for(int i = m_areashow[2]; i<=m_areashow[3] ;i++)
		{	
			bool bexistsTile = true;
			CString strCol = L"";
			strCol.Format(L"\\C%08x",i);

			CString pStrFullPath =  strRow + strCol + "." + m_TitleType;//ͼƬ��ʽjpg

			//��ȡ����
			long key =js * 10000 + (i ) * 100 + j;
	
			if (!PathFileExists(pStrFullPath) && ServerMapData[key] != NULL)//���ز������ļ������Ѿ���server�õ�����
			{		
				//����ͼƬ	������PNG ͼƬ��������ʽ�Ļ���Ҫ���ԡ�
				buf_t* pbuf_t = ServerMapData[key];
				//�ж��ڴ������Ƿ���ͼƬ
				if (pbuf_t->sz!=0 || pbuf_t->buffer[0]!='<')
				{				
					FILE* f = fopen(CStringA(pStrFullPath),"wb");//"D:\\L02\\vtron.png"
					fwrite(&pbuf_t->buffer,1,pbuf_t->sz,f); //���浽�����ļ���
					fflush(f);
					fclose(f);
				}
				else
				{
					//���񷵻ص����ݲ���ͼƬ��arcserver����html
					bexistsTile = false;
				}
			}	

				Vertex verts[4] = {}; //������ͼ����ͼƬ�� [������Ϣ]
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

	for (int i = 0;i < TitleNum;i++) //�����������������
	{
		delete[] url[i];
		curl_easy_cleanup(curl[i]);
	}


	int datasize = m_tmpData.size(); //����������
	if (datasize>0)
	{
		DWORD start = GetTickCount();
		//���߳̽���jpg		
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

	//���ƻ����С
	if (m_Cachemap.size()>100)//Ҫɾ������ͼƬ,��Ҫ
	{
		map<long, Tile*>::iterator it = m_Cachemap.begin();
		for (;it!=m_Cachemap.end();it++)
		{
			long tmpkey = (*it).first;
			Tile* tile = (*it).second;
			if (tile!=NULL)//����ö����ǵ�ǰҪ��ʾ�������ɾ��
			{
				if (tile->m_js != js || (tile->m_rowid<m_areashow[0])||(tile->m_rowid>m_areashow[1])   //�޸�Ϊ����
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

	m_CenLon = cenlon;//��ǰ���ĵ�
	m_CenLat = cenlat;
	m_LeftTop_Lon = letflon;//����ͼ������Ͻ�����
	m_LeftTop_Lat = Toplat;
	m_Lon = m_Scale2ImageW*dresolution;
	m_Lat = m_Scale2ImageH*dresolution;

	m_Scale = scale;
	m_resolution = dresolution;

	return LayerTileVector;
}

//geo��ͼ ����ȡ��ͼ ͼƬ����  [�ػʵ�ʹ�øú�����ȡͼƬ]
vector<Tile*> CMapServerLayer::GetGeobeansData(int js,double cenlon,double cenlat,double WScreen,double HScreen,long& ditX,long& ditY,long& Wcount,long & Hcount)
{
	//ÿ�������ݣ�ֻ�ǹ�ע����ǰ������������ʾ�� ��ͼTile
	ServerMapData.clear();  //���ڱ��� ���ڱ��� �ĵ�ͼ ͼƬ[ֻ���ڵ�ǰ��������ʾ��ͼƬ]
		m_tmpData.clear(); //�����ʱ�� װͼƬ�� ����[CBaseLayer�ж���]���ڵ�ǰ�����г��ֵ�ͼƬ����������еı���� key:value ��Ҫ����buff��m_cachemap������
	vector<Tile*> LayerTileVector;//�����ڵ�ǰ������ʾ��Χ֮�ڵĵ�ͼͼƬ�������������Ϊ������ء�
	
	CURL *curl[1000];//���1000��url
	char* url[1000];
	int xx = 0;  //��¼���ڴ�Զ�̷���������ͼƬ ��URL����

	
	double lat ;
	double width ;
	double height;
	double letflon;
	double Toplat;
	double scale;
	int ndpi;
	double dresolution;
	
	//�õ��ֱ�����js���� ��ͼ��Ϣ
	GetMapConfigInfo(js,letflon,Toplat,m_PicH,m_PicW,ndpi,dresolution,scale);  //����Ϊʲô��ֱ��ʹ�ã�CMapServerLayer���Ѿ����úõĳ�Ա������������[�ֲ�����]

	//���ĵ��Ӧ������
	double tileWidth = m_PicW * dresolution; //һ��ͼƬ�Ŀ��ռ�õ�[��γ����]
	double tColCenter = (int)floor((cenlon - letflon) / tileWidth);  //���ĵ�

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


	//ͨ����ȡ�ĵ�һ��ͼƬ�����Ͻ�������ؼ����Ͻ�������в�ֵ��ȡƫ��
	ditX = (pixX - WScreen / 2.0) - xmin * picsizeW;
	ditY = picsizeH - ((pixY + HScreen / 2.0) - ymin * picsizeH);

	int TitleNum = 0; //ͳ�ƴ� Զ�̷����� �õ�ͼƬ�� ��Ŀ

	USES_CONVERSION;
	string mapname = W2A(m_MapName.GetBuffer(0));/*"raster"*/
	m_MapName.ReleaseBuffer();
	string TitleType = W2A(m_TitleType.GetBuffer(0));/*"jpg";*/
	m_TitleType.ReleaseBuffer();

	//i��ʾ������j��ʾ����
	CString strJs;
	strJs.Format(L"%s\\L%02d\\",m_sPath,js);  //  f:/raster\L14\  //

	if(!PathIsDirectory(strJs))    //��Ϊ�����m_sPath��������ʵ��[��ͼ����·��] Ҳ������ [���ǹ���Ļ���·��]
	{                            
		CreateDirectory(strJs,NULL);
	}

	for (int j = m_areashow[0]; j>=m_areashow[1] ;j--)
	{
		CString strRow;
		strRow.Format(L"%sR%d",strJs,j);  // f:/raster\L14\R14107 
	
		if(!PathIsDirectory(strRow))
		{
			CreateDirectory(strRow,NULL);//�������ļ���
		}

		for(int i = m_areashow[2]; i<=m_areashow[3] ;i++)
		{	
			CString strCol = L"";
			strCol.Format(L"\\C%d",i);

			CString pStrFullPath =  strRow + strCol + "." +  m_TitleType;//ͼƬ��ʽpng,jpg��  // f:/raster\L14\R14107\C13632.jpg

			//��ΪpStrFullPath ��ʵ��[��ͼ����·��] Ҳ������ [���ǹ���Ļ���·��]�����Բ��������ж��ļ��Ƿ���ڵĲ���
			if ( PathFileExists(pStrFullPath))//��������Ѿ��и��ļ�����ӷ����ȡ����
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

			TitleNum++;//ͳ�ƴ� Զ�̷����� �õ�ͼƬ�� ��Ŀ

			//���bool������ ���������Ѿ����ú�
			if (!m_localornet)   //������ȷ��һ�����壬������ݵ�path�Ǳ���·������ô��ͼ�϶���ȫ�Ĳ���ȡ���أ������URL�Ż�ȡ����
			{
				long key =js * 10000 + (i ) * 100 + j;

				char* urlStr = new char[512];
				if (m_MapType == MapType::GeoBeansServerMap)
				{
					//js������path http://111.63.38.38:9100/QuadServer/maprequest
					//��Ҫ���http://111.63.38.38:9100/QuadServer/maprequest?services=raster&row=14129&col=13630&level=14
					sprintf(urlStr,"%s?services=%s&row=%d&col=%d&level=%d&rowModel=a",m_mapServerUrl.c_str(),mapname.c_str(),j,i,js);
				}

				curl[xx] = curl_easy_init();
				url[xx] = urlStr; 
				bufs[xx].sz = 0;  //bufs �Ƕ����ȫ������
				curl_easy_setopt(curl[xx], CURLOPT_URL, url[xx]);
				curl_easy_setopt(curl[xx], CURLOPT_WRITEFUNCTION, &write_data);
				curl_easy_setopt(curl[xx], CURLOPT_WRITEDATA, &bufs[xx]);     //��ͼƬд�뵽
				set_share_handle(curl[xx]);
				curl_easy_setopt(curl[xx], CURLOPT_FORBID_REUSE, 1); 
				curl_multi_add_handle(m_multi_handle, curl[xx]);
				ServerMapData[key] = &bufs[xx];
			}
			xx++; //ͳ�ƴ� Զ�̷����� �õ�ͼƬ�� URL��Ŀ
		}
	}// ���е� [�ڵ�ǰ��������ʾ�ĵ�ͼͼƬ] �ڱ��صĲ����κβ��������ڱ��أ���ȡ���ݲ����浽 ServerMapData

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
	
	//�ӷ����ȡ�������Ժ�: ���������ڴ�����ʾ��Χ�ڵ� ͼƬ
	for (int j = m_areashow[0]; j>=m_areashow[1] ;j--)
	{
		CString strRow;
		strRow.Format(L"%sR%d",strJs,j);

		for(int i = m_areashow[2]; i<=m_areashow[3] ;i++)
		{	
			CString strCol = L"";
			strCol.Format(L"\\C%d",i);

			CString pStrFullPath =  strRow + strCol + "." + m_TitleType;//ͼƬ��ʽjpg
			
			bool bexistsTile = PathFileExists(pStrFullPath); //��֤��ͼƬ �Ƿ���ڱ��ش���

			//��ȡ���ݣ���ͼƬ�Ĳ㼶js���С��й�ͬ����� key
			long key =js * 10000 + (i ) * 100 + j;

			//����������ǣ���ͼ��������ʾ���� ��ʱ��ͼƬ[�Ȳ��ڱ��� Ҳ���� ServerMapData��]
			//�����ᵼ�£���NULL��Tile�� push_back�� [LayerTileVector]
			if (!bexistsTile && ServerMapData[key] != NULL)//���ز������ļ������Ѿ���server�õ�����
			{		
				//����ͼƬ	������PNG ͼƬ��������ʽ�Ļ���Ҫ���ԡ�
				buf_t* pbuf_t = ServerMapData[key];
				//�ж��ڴ������Ƿ���ͼƬ
				if (pbuf_t->sz!=0 && pbuf_t->buffer[0]!='<')
				{				
					FILE* f = fopen(CStringA(pStrFullPath),"wb");
					fwrite(&pbuf_t->buffer,1,pbuf_t->sz,f);  //�������� ServerMapData�е�Զ��ͼƬ���� �������ڱ��ش����� *.jpg
					fflush(f);
					fclose(f);
				}
				else
				{
					//���񷵻ص����ݲ���ͼƬ��arcserver����html
					bexistsTile = false;
				}
			}	

			//ָ��һ��jpgͼƬ�ģ�ͼԪ��С256*256�������ú� �������꣬��ͼԪ�� �������꣬��������pixelΪ��λ�� ��������
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

			//���ж��ڴ滺������Ƿ��и�ͼƬ
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
				if (bexistsTile) // ��Ҫ��ʾ��ͼƬ�ڱ��ش�����
				{
					tile = new Tile(pStrFullPath,verts,m_PicW,m_PicH,m_TitleType,js,j,i);
					tile->m_sformat = GL_RGBA;
					m_tmpData[key] = tile;	 //�����ͼƬ���ڴ����ϣ���Ҫ��jpg����� bitmap��ʽ
				}					
			}
			//�������������һ��  if(NULL != tile)
			LayerTileVector.push_back(tile);
		}//�ڲ�forѭ����������ϵ���ͼƬ
	}//������� ���е� ��ͼͼƬ

	for (int i = 0;i < TitleNum && !m_localornet;i++)
	{
		delete[] url[i];
		curl_easy_cleanup(curl[i]);
	}


	int datasize = m_tmpData.size(); 
	if (datasize>0)
	{
		//DWORD start = GetTickCount();
		//���߳̽���jpg		  ��jpg�Ӵ��̶����ڴ棬��ת��Ϊ��������������ͼ��[λͼ]
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
			tile->init();//opengl����Ĳ���������������󣬰�����״̬
			m_Cachemap[key] = tile;
		}
	}

	//���ƻ����С
	if (m_Cachemap.size()>2000)
	{
		map<long, Tile*>::iterator it = m_Cachemap.begin();
		for (;it!=m_Cachemap.end();it++)
		{
			long tmpkey = (*it).first;
			Tile* tile = (*it).second;
			if (tile!=NULL)//����ö����ǵ�ǰҪ��ʾ�������ɾ��
			{
				//ԭ����code�ǣ�tile->m_rowid<m_areashow[0])||(tile->m_rowid>m_areashow[1] �����Χд���ˣ�����ͼƬɾ��
				//��ɵĺ������ ��ǰ������ʾ��ͼƬ����delete�ˣ�����������˵�ͼ����ʾ������ȱͼ����
				if (tile->m_js != js || (tile->m_rowid<m_areashow[1])||(tile->m_rowid>m_areashow[0])   
					|| (tile->m_colid<m_areashow[2])||(tile->m_colid>m_areashow[3]))
				{
					delete tile;
					tile = NULL;
					m_Cachemap.erase(it); //map��erase���� �Ƿ���void�ģ����� iterator
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
		} // ���������꣬���������е�[ͼƬ]
	}

	m_CenLon = cenlon;//��ǰ���ĵ�
	m_CenLat = cenlat;
	m_LeftTop_Lon = letflon;//����ͼ������Ͻ�����
	m_LeftTop_Lat = Toplat;
	m_Lon = 360;//m_Scale2ImageW*dresolution;
	m_Lat = 360;//m_Scale2ImageH*dresolution;

	m_Scale = scale;
	m_resolution = dresolution;

	return LayerTileVector; //����װ�� �Ѿ������Tile��vector��������Ǳ��������������ʹ��oges����
}

//�����ع���
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
			theCurlConnect->WateForMulitCurl(m_mulit_curl_);//�������ȡ������

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

					//��������
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
									::MessageBoxA(NULL,"�����ͼ�Ĵ��̿ռ䲻�㣡","Error",0);
								}
								else
								{
									CString errorinf;
									errorinf.Format(L"ͼƬ�������Error=%d !",error);
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

	CURL *curl[1000];//���1000��url
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

	//���ĵ��Ӧ������
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
	double  xC= xmin*picsizeW;//ͨ����ȡ�ĵ�һ��ͼƬ�����Ͻ�������ؼ����Ͻ�������в�ֵ��ȡƫ��
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

	//i��ʾ������j��ʾ����
	CString strJs;
	strJs.Format(L"%s\\L%02d\\",m_sPath,js);
	if(!PathIsDirectory(strJs))
	{
		CreateDirectory(strJs,NULL);//�������û�иò�ͼƬ,���ȴ����ļ���
	}

	std::vector<MapTileLocation> theRequestTile;

	//��url:����Ҫ�����tile��url ������һ��������
	for (int j = m_areashow[0]; j>=m_areashow[1] ;j--)
	{
		CString strRow;
		strRow.Format(L"%sR%d",strJs,j);
		if(!PathIsDirectory(strRow))
		{
			CreateDirectory(strRow,NULL);//�������ļ���
		}

		for(int i = m_areashow[2]; i<=m_areashow[3] ;i++)
		{	
			CString strCol = L"";
			strCol.Format(L"\\C%d",i);

			CString pStrFullPath =  strRow + strCol + "." +  m_TitleType;//ͼƬ��ʽpng,jpg��
			bool bexistsTile = PathFileExists(pStrFullPath);

			if ( PathFileExists(pStrFullPath))//��������Ѿ��и��ļ�����ӷ����ȡ����
			{	// ��ͼ��������
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

	//��������
	if (theRequestTile.size() != 0 && !m_localornet)
	{
		DownLoadTheTile(theRequestTile,strJs);
	}
	

	//�ӷ����ȡ�������Ժ�:
	for (int j = m_areashow[0]; j>=m_areashow[1] ;j--)
	{
		CString strRow;
		strRow.Format(L"%sR%d",strJs,j);

		for(int i = m_areashow[2]; i<=m_areashow[3] ;i++)
		{	
			bool bexistsTile = true;
			CString strCol = L"";
			strCol.Format(L"\\C%d",i);

			CString pStrFullPath =  strRow + strCol + "." + m_TitleType;//ͼƬ��ʽjpg

			//��ȡ����  ��hash�ķ�ʽ��ȡ����
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

			//�ж��ڴ滺������Ƿ��и�ͼƬ
			Tile* tile = NULL; //��ʾ
			map<long,Tile*>::iterator It;
			It = m_Cachemap.find(key);

			if(It != m_Cachemap.end())
			{
				tile = It->second;
				tile->SetVertex(verts);// ����[��ͼͼԪ������] �� [��������]
			}
			else
			{
				if (bexistsTile)
				{
					tile = new Tile(pStrFullPath,verts,m_PicW,m_PicH,m_TitleType,js,j,i);
					tile->m_sformat = GL_RGBA;
					m_tmpData[key] = tile;	 //��������ֻ�Ǳ����� ���ڴ����� jpg��·������Ҫ�����ڴ沢����
				}					
			}
			LayerTileVector.push_back(tile);
		}
	}


	int datasize = m_tmpData.size();
	if (datasize>0)
	{
		DWORD start = GetTickCount();
		//���߳̽���jpg		
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
			//�� ������ ͼƬ����������
			m_Cachemap[key] = tile;
		}
	}

	//���ƻ����С
	if (m_Cachemap.size()>1000)//Ҫɾ������ͼƬ,��Ҫ
	{
		map<long, Tile*>::iterator it = m_Cachemap.begin();
		for (;it!=m_Cachemap.end();it++) // ö�ټ�飬�����е�ÿһ��ͼƬ
		{
			long tmpkey = (*it).first;
			Tile* tile = (*it).second;
			if (tile!=NULL)//����ö����ǵ�ǰҪ��ʾ�������ɾ��
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

	m_CenLon = cenlon;//��ǰ���ĵ�
	m_CenLat = cenlat;
	m_LeftTop_Lon = letflon;//����ͼ������Ͻ�����
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

// ��ͼ�Ļ��� ������
void CMapServerLayer::Draw()
{
	if (!m_bconnect)  //Զ�̷�������ʧ�ܣ���ִ������ȡ��ͼ���ݣ��Ѿ����Ƶ�ͼ
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
		if (m_MapType==GeoBeansServerMap)  //�����ػʵ�
		{
			//vector<Tile*> m_pTile װ��ͼƬ�� ����
			//������ڵ�ǰ��������Ҫ��ʾ��ͼƬ�ļ��������ڴ沢�����bitmap,�����������ͼʱ����ͼԪ������ֵ
			// ���н�ͼƬ�����ڴ棬����Ҫ��Զ�̷�����ȡͼƬ������������ ͼƬ�ڴ�Ļ������
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
		
		m_bUpdateData = false; //��Ϊfalse����ʾ��ʱ�ĵ�ͼ���������µġ�����ͼ������Ҫ���»�ȡʱ�����ñ�������ΪTRUE
	}
	//DWORD e = GetTickCount()-s;

	DrawLayer(m_pTile,ditx,dity,true); //����ES��������ͼ ��ʾ��άͼƬ

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
			CString data("");//��ȡHTML
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

			//�ҵ�layer��Ӧ��matrixset
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
			

			//�ҵ�layer
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

	//����ֱ��� ������Ҫ�ӷ����ȡ
	double pResolution = r0*2;   // 156543.04/2/3.1415926/6378137*360;
	for(int i = 0; i < 20; i++)
	{
		pResolution /=2;

		MapLayerData* pLayerData = new MapLayerData();
		pLayerData->Level = i;
		double presolution = 0;		
		pLayerData->Resolution = pResolution;
		pLayerData->Scale =(pResolution*(2*3.1415926*6378137/360)*96/0.0254) ; //������
		m_LayerDataList[i] = pLayerData; 
	}
	m_PicSizeW = 256;
	m_PicSizeH = 256;
	//m_Scale2ImageW = atoi(cMatrixWidth) * m_PicSizeW;
	//m_Scale2ImageH = atoi(cMatrixHeight) * m_PicSizeH;
	m_resolution = m_LayerDataList[m_js]->Resolution;
	return true;
}
