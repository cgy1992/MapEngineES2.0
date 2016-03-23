#pragma once
#include "baselayer.h"
#include <vector>
#include "Tile.h"
#include "tinyxml\tinyxml.h"//chkun
#include "XmlWork.h"
#include "DrawEngine.h"

using namespace std;

class CArcgisMapLayer :public CBaseLayer
{
public:
	CArcgisMapLayer(void);
	CArcgisMapLayer(double cenX,double cenY,int js,CString mapname,int picW,int picH,CString path,CString pictype,DrawEngine *den = NULL);
	~CArcgisMapLayer(void);

	CXmlWork m_xmlwork; //生成XML文件

	//这四个成员 在基类 CBaseLayer中存在
	//vector<Tile*> m_pTile;//临时变量，不用释放内存   
	//map<long, Tile*> m_Cachemap;  // 缓存在窗口中显示过的图片
	//map<long,Tile*> m_tmpData;
	//int m_areashow[4];//行列号范围


	void CArcgisMapLayer::GetMapConfigInfo(std::string strXmlPath,int djs,double &lfTileOriginX,double &lfTileOriginY,int &dTileCols,int &dTileRows,
		int &dDPI,double &lfResolution,double &dscale);  //从地图的配置文件 conf.xml中读取 地图信息

	void CArcgisMapLayer::GetBaseLayerConf(double &Cenlon,double &Cenlat,double & width,double & height
		,double & leftlon,double & Toplat,double& lon,double &lat,double & scale,CString & Path,double &resolution);


	vector<Tile*> GetData(int js,double cenlon,double cenlat,double WScreen,double HScreen,long& ditX,long& ditY,long& Wcount,long & Hcount,int g_context = 0);
	void DrawLayer(vector<Tile*> &pTile,int ditx,int dity,bool m_realtime);
	void Draw();
	void Draw(int g_context);
	//test
	void DecodeTile();
	static DWORD WINAPI  Datathread1(LPVOID);
	static DWORD WINAPI  Datathread2(LPVOID);
	static DWORD WINAPI  Datathread3(LPVOID);
	static DWORD WINAPI  Datathread4(LPVOID);
	static DWORD WINAPI  Datathread5(LPVOID);
	static DWORD WINAPI  Datathread6(LPVOID);
	void  decodetileSingleT();
private:
	int g_CtrlW;  //窗口的大小
	int g_CtrlH;
	bool m_localornet; //本地 or 网络
	//struct MapLayerData* m_LayerData;
	map<long, MapLayerData*> m_LayerDataList; //分辨率 不同的地图层，可以根据js发来的Resolution0的值来计算
};

