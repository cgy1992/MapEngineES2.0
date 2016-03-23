//PPAPI����Ŀؼ���ͼ��
//�������ֵ�ͼ��ʱ����Ҫ�Ӹøļ̳У���ʵ�ֻ���ķ���
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

//��ͼ��ͼ��
class CBaseLayer: public CLayer
{
public:
	CBaseLayer(void);
	virtual ~CBaseLayer(void);

	double m_CenLon;//�ؼ����ĵ��Ӧ�ĵ�������
	double m_CenLat;
	double m_Scale;
	double m_Scale2ImageW;//��������ͼƬ�Ŀ��
	double m_Scale2ImageH;
	double m_LeftTop_Lon;//�������ϽǾ�γ��
	double m_LeftTop_Lat;
	double m_Lon;//��������ͼƬ�ľ�γ�ȵĿ��
	double m_Lat;
	//int m_CurrLevel; //��ͼ��ǰ����ʾ ���� �������и� m_js��Ա
	int m_MinLevel,m_MaxLevel;			//���ļ���

	//string m_LayerName;//��ͼ��
	MapType m_MapType;
	CString m_TitleType;
	bool m_bUpdateData;//��ͼ�Ƿ�Ҫ���£�����ͼƽ��ʱΪtrue,���¾ͱ�ʾ
	
	int GetCurrLevel(){ return m_js;} //�õ�map��ǰ����ʾLevel
	virtual void GetBaseLayerConf(double &cenlon,double &cenlat,double & width,double & height,double & leftlon,double & Toplat,double& lon,double &lat,double & scale,CString & Path,double &resolution);
	virtual void Draw();

	//�޸ĵ�ͼ�����ĵ�
	virtual void SetMapCentre(double CenX,double CenY,double CenZ,int js);
	virtual void GetMapCentre(double &CenX,double &CenY,double &CenZ,int &js);

	vector<Tile*> m_pTile;//��ʱ�����������ͷ��ڴ�
	map<long, Tile*> m_Cachemap;
	map<long,Tile*> m_tmpData;
	int m_areashow[4];//���кŷ�Χ

	void DecodeTile();
	static DWORD WINAPI  Datathread1(LPVOID);
	static DWORD WINAPI  Datathread2(LPVOID);
	static DWORD WINAPI  Datathread3(LPVOID);
	static DWORD WINAPI  Datathread4(LPVOID);
	static DWORD WINAPI  Datathread5(LPVOID);
	static DWORD WINAPI  Datathread6(LPVOID);
	void  decodetileSingleT();
};

