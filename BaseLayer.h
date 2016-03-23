//PPAPI组件的控件底图类
//创建各种底图类时，需要从该改继承，并实现基类的方法
#pragma once
#include "stdafxcpp.h"
#include "Layer.h"
#include "Tile.h"
using namespace std;

#define  g_countT 6

enum MapType
{
  VtronMapTile = 0,
  VtronMapCompact = 1,
  ArcgisMap = 2,
  ArcgisServerMap = 3,
  DegreeServermap = 4,
  ArcgisCompactMap = 5,
  GeoBeansServerMap = 6,
  PGISServerMap = 7
}enumMapType;

struct MapLayerData
{
	int Level;
	double Resolution;
	double Scale;
};

//地图底图类
class CBaseLayer: public CLayer
{
public:
	CBaseLayer(void);
	virtual ~CBaseLayer(void);

	double m_CenLon;//控件中心点对应的地理坐标
	double m_CenLat;
	double m_Scale;
	double m_Scale2ImageW;//整个级别图片的宽高
	double m_Scale2ImageH;
	double m_LeftTop_Lon;//整个左上角经纬度
	double m_LeftTop_Lat;
	double m_Lon;//整个级别图片的经纬度的跨度
	double m_Lat;
	//int m_CurrLevel; //地图当前的显示 级别 父类中有个 m_js成员
	int m_MinLevel,m_MaxLevel;			//最大的级数

	//string m_LayerName;//地图名
	MapType m_MapType;
	CString m_TitleType;
	bool m_bUpdateData;//底图是否要更新：当地图平移时为true,更新就表示
	
	int GetCurrLevel(){ return m_js;} //得到map当前的显示Level
	virtual void GetBaseLayerConf(double &cenlon,double &cenlat,double & width,double & height,double & leftlon,double & Toplat,double& lon,double &lat,double & scale,CString & Path,double &resolution);
	virtual void Draw();

	//修改地图的中心点
	virtual void SetMapCentre(double CenX,double CenY,double CenZ,int js);
	virtual void GetMapCentre(double &CenX,double &CenY,double &CenZ,int &js);

	vector<Tile*> m_pTile;//临时变量，不用释放内存
	map<long, Tile*> m_Cachemap;
	map<long,Tile*> m_tmpData;
	int m_areashow[4];//行列号范围

	void DecodeTile();
	static DWORD WINAPI  Datathread1(LPVOID);
	static DWORD WINAPI  Datathread2(LPVOID);
	static DWORD WINAPI  Datathread3(LPVOID);
	static DWORD WINAPI  Datathread4(LPVOID);
	static DWORD WINAPI  Datathread5(LPVOID);
	static DWORD WINAPI  Datathread6(LPVOID);
	void  decodetileSingleT();
};

