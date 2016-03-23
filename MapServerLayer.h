//各种网络地图的数据获取和显示
/*
  arcserver\degreeserver 
  PGISServer切图是以经纬度00为起点，向东北方向递增，0-22级，每级分辨率固定，0级为2，依次为上级别分辨率的1/2
  GeoBeansServer切图可以设置起点经纬度，可以向东北方向或者东南方向递增，默认是东北方向；分辨率不固定，但同样为上级别分辨率的1/2
  在MapEngineES中把PGIS作为一种特殊的GeoBeans图层。
*/
#pragma once
#include "BaseLayer.h"
#include "Tile.h"
#include "curl.h"  
#include "types.h" 
#include "easy.h" 
#include "tinyxml\tinyxml.h"//chkun
#include "XmlWork.h"
#pragma comment(lib,"libcurl_imp.lib")   //指定链接特定的静态库
#include "CurlConnect.h"
#include "MapUrl.h"
#include "DrawEngine.h"



class CMapServerLayer :public CBaseLayer    //秦皇岛的图层
{
public:
	CMapServerLayer(void);
	CMapServerLayer(double cenX,double cenY,int js,CString mapname,int picW,int picH,CString path,CString pice,CString servertype);  //从服务上去取，要读服务中地图的配置文件 conf.xml
	CMapServerLayer(double cenX,double cenY,int js,CString mapname,int picW,int picH,CString path,CString pictype,CString servertype,double minlon,double minlat,double resolution0,DrawEngine *den);  //从本地磁盘上取地图图片，
	~CMapServerLayer(void);

private:
	char m_cServerIP[128]; //IP地址
	int	m_iPort;
	string m_mapServerUrl; //连接的URL:
	bool m_bconnect;//是否连接上服务	
	long m_Minlevel;
	CString m_LayerName;//图层名
	const char* m_TileMatrixSetname;//地图对应的矩阵名，在构造url时需要
	 

	double m_Xmax;//连接arcserver时获取的地图属性
	double m_Ymax;
	double m_Xmin;
	double m_Ymin;
	double m_OriginX;//经度的起点
	double m_OriginY;//纬度的起点
	int m_PicSizeW;//每张瓦片的宽 象素单位
	int m_PicSizeH;//每张瓦片的高 象素单位
	int m_dpi;//底图的dpi 不是必须的
	//add by xuyan
	int g_CtrlW;  //插件窗口的大小
	int g_CtrlH;
	bool m_localornet; //本地 or 网络

	map<long, MapLayerData*> m_LayerDataList; //分辨率 不同的地图层，可以根据js发来的Resolution0的值来计算

	//服务的连接函数：从服务获取记录地图的描述文件，并解析
	//PGIS地图作为一种特殊的GeoBeans地图
    bool ConnetArcServer(CString serverurl);//rest wmts
	bool CoonnetDegreeServer(CString serverurl);
	bool CoonnetGeoBeansServer(CString serverurl,double r0);

	int MergeRequestUrl(int row,int col,int level,char* urlStr);
	void GetMapConfigInfo(int djs,double &lfTileOriginX,double &lfTileOriginY,int &dTileCols,int &dTileRows,int &dDPI,double &lfResolution,double &dscale);
	void DrawLayer(vector<Tile*> pTile,int ditx,int dity,bool m_realtime);  //容器这里 应该是传 [引用]
	void Draw();

	vector<Tile*> GetData(int js,double cenlon,double cenlat,double WScreen,double HScreen,long& ditX,long& ditY,long& Wcount,long & Hcount);

	vector<Tile*> GetGeobeansData(int js,double cenlon,double cenlat,double WScreen,double HScreen,long& ditX,long& ditY,long& Wcount,long & Hcount);
	
	//使用封装了的多任务下载
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

