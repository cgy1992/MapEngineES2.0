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

	CXmlWork m_xmlwork; //����XML�ļ�

	//���ĸ���Ա �ڻ��� CBaseLayer�д���
	//vector<Tile*> m_pTile;//��ʱ�����������ͷ��ڴ�   
	//map<long, Tile*> m_Cachemap;  // �����ڴ�������ʾ����ͼƬ
	//map<long,Tile*> m_tmpData;
	//int m_areashow[4];//���кŷ�Χ


	void CArcgisMapLayer::GetMapConfigInfo(std::string strXmlPath,int djs,double &lfTileOriginX,double &lfTileOriginY,int &dTileCols,int &dTileRows,
		int &dDPI,double &lfResolution,double &dscale);  //�ӵ�ͼ�������ļ� conf.xml�ж�ȡ ��ͼ��Ϣ

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
	int g_CtrlW;  //���ڵĴ�С
	int g_CtrlH;
	bool m_localornet; //���� or ����
	//struct MapLayerData* m_LayerData;
	map<long, MapLayerData*> m_LayerDataList; //�ֱ��� ��ͬ�ĵ�ͼ�㣬���Ը���js������Resolution0��ֵ������
};

