//���������ͼ�����ݻ�ȡ����ʾ
/*
  arcserver\degreeserver 
  PGISServer��ͼ���Ծ�γ��00Ϊ��㣬�򶫱����������0-22����ÿ���ֱ��ʹ̶���0��Ϊ2������Ϊ�ϼ���ֱ��ʵ�1/2
  GeoBeansServer��ͼ����������㾭γ�ȣ������򶫱�������߶��Ϸ��������Ĭ���Ƕ������򣻷ֱ��ʲ��̶�����ͬ��Ϊ�ϼ���ֱ��ʵ�1/2
  ��MapEngineES�а�PGIS��Ϊһ�������GeoBeansͼ�㡣
*/
#pragma once
#include "BaseLayer.h"
#include "Tile.h"
#include "curl.h"  
#include "types.h" 
#include "easy.h" 
#include "tinyxml\tinyxml.h"//chkun
#include "XmlWork.h"
#pragma comment(lib,"libcurl_imp.lib")   //ָ�������ض��ľ�̬��
#include "CurlConnect.h"
#include "MapUrl.h"
#include "DrawEngine.h"



class CMapServerLayer :public CBaseLayer    //�ػʵ���ͼ��
{
public:
	CMapServerLayer(void);
	CMapServerLayer(double cenX,double cenY,int js,CString mapname,int picW,int picH,CString path,CString pice,CString servertype);  //�ӷ�����ȥȡ��Ҫ�������е�ͼ�������ļ� conf.xml
	CMapServerLayer(double cenX,double cenY,int js,CString mapname,int picW,int picH,CString path,CString pictype,CString servertype,double minlon,double minlat,double resolution0,DrawEngine *den);  //�ӱ��ش�����ȡ��ͼͼƬ��
	~CMapServerLayer(void);

private:
	char m_cServerIP[128]; //IP��ַ
	int	m_iPort;
	string m_mapServerUrl; //���ӵ�URL:
	bool m_bconnect;//�Ƿ������Ϸ���	
	long m_Minlevel;
	CString m_LayerName;//ͼ����
	const char* m_TileMatrixSetname;//��ͼ��Ӧ�ľ��������ڹ���urlʱ��Ҫ
	 

	double m_Xmax;//����arcserverʱ��ȡ�ĵ�ͼ����
	double m_Ymax;
	double m_Xmin;
	double m_Ymin;
	double m_OriginX;//���ȵ����
	double m_OriginY;//γ�ȵ����
	int m_PicSizeW;//ÿ����Ƭ�Ŀ� ���ص�λ
	int m_PicSizeH;//ÿ����Ƭ�ĸ� ���ص�λ
	int m_dpi;//��ͼ��dpi ���Ǳ����
	//add by xuyan
	int g_CtrlW;  //������ڵĴ�С
	int g_CtrlH;
	bool m_localornet; //���� or ����

	map<long, MapLayerData*> m_LayerDataList; //�ֱ��� ��ͬ�ĵ�ͼ�㣬���Ը���js������Resolution0��ֵ������

	//��������Ӻ������ӷ����ȡ��¼��ͼ�������ļ���������
	//PGIS��ͼ��Ϊһ�������GeoBeans��ͼ
    bool ConnetArcServer(CString serverurl);//rest wmts
	bool CoonnetDegreeServer(CString serverurl);
	bool CoonnetGeoBeansServer(CString serverurl,double r0);

	int MergeRequestUrl(int row,int col,int level,char* urlStr);
	void GetMapConfigInfo(int djs,double &lfTileOriginX,double &lfTileOriginY,int &dTileCols,int &dTileRows,int &dDPI,double &lfResolution,double &dscale);
	void DrawLayer(vector<Tile*> pTile,int ditx,int dity,bool m_realtime);  //�������� Ӧ���Ǵ� [����]
	void Draw();

	vector<Tile*> GetData(int js,double cenlon,double cenlat,double WScreen,double HScreen,long& ditX,long& ditY,long& Wcount,long & Hcount);

	vector<Tile*> GetGeobeansData(int js,double cenlon,double cenlat,double WScreen,double HScreen,long& ditX,long& ditY,long& Wcount,long & Hcount);
	
	//ʹ�÷�װ�˵Ķ���������
	vector<Tile*> GetGeobeansDataEx(int js,double cenlon,double cenlat,double WScreen,double HScreen,long& ditX,long& ditY,long& Wcount,long & Hcount);

	CXmlWork m_xmlwork;
	CURLM *   m_multi_handle;

	void set_share_handle(CURL* curl_handle)
	{
		static CURLSH* share_handle = NULL;
		if (!share_handle)
		{
			share_handle = curl_share_init();
			curl_share_setopt(share_handle, CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);
		}
		curl_easy_setopt(curl_handle, CURLOPT_SHARE, share_handle);
		curl_easy_setopt(curl_handle, CURLOPT_DNS_CACHE_TIMEOUT, 60 * 5);
	}

	bool DownLoadTheTile(std::vector<MapTileLocation> theReturnLocation,CString path);
};

