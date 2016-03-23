#pragma once
#include "stdafxcpp.h"
#include "stdafxwin32.h"
#include <stdio.h>
#include"LogFile.h"
//��������
enum VGeometryType
{	
	vUnknown	= 0,
	vUPoint		= 1,
	vULine		= 2,
	vUPolygon	= 3,
	vUMultiPoint	= 4,
	vUMultiLine		= 5,
	vUMultiPolygon	= 6,
	vUGeometryCollection	= 7,
	vUPointMark	=	8
};

//style type
enum enumStyleType
{	
	vPointstyle	= 0,
	vLinestyle	= 1,
	vPolygonstyle	= 2,
	vTextstyle	= 3
};

//type of layer
enum VLayerType
{
	vBaseLayer = 0,
	vHotLayer = 1,
	vDynamicLayer = 2,
	vTempLayer = 3,
	vElementLayer = 4,
	vBufferLayer = 5,
	vGifLayer = 6,
	vLuKuangLayer = 7,
	vWFSPointLayer = 8,
	vEditLayer = 9,
	vWFSLineLayer = 10,
	vWFSPolyLayer = 11,     //add by xuyan
	vHeatMapLayer,
	vClusterLayer
}VLayerType;

//��ͼ������ʽ
enum enumMapOperator
{
	vPanOperator = 0,		//���������ƽ�ƣ�����
	vChooseOperator = 1,	//ѡ����� ����[��ѡ] ��Ƭ����
	vDrawOperator = 2,		//���ʲ���
	vPolygonChoose = 3,		//��ѡ
	vMark,
	vCircleChoose,
	vLineChoose
};

enum enumChooseType
{
	vSingleChoose = 0,
	vRectChoose = 1,
	vPolyChoose,
	vMarkChoose,
	vNULL
};

//WFS�е� �ȵ����ݽṹ
struct MapEngineES_Point
{	
	double x;
	double y;
	int fid;
	int GisID;
};


struct Vertex
{
	float tu, tv;
	float loc[3];
	float color[4];
	Vertex()
	{
		tu = 0.0f;
		tv = 0.0f;
		memset(loc,0,sizeof(loc));
		memset(color,0,sizeof(color));
	}
};


struct ShPoint{
	float x_;
	float y_;
	ShPoint() :x_(.0), y_(.0){}
};

struct PixPoint{
	long x_;
	long y_;
	PixPoint() :x_(0), y_(0){}
	void setPos(long x, long y){ x_ = x, y_ = y; }
};


typedef struct 
{
	double x_;
	double y_;
	int Id_;
	void setPos(double x,double y,int Id=-1){ x_ = x,y_  = y,Id_ = Id ;}
	//void setPos(double x, double y){ x_ = x, y_ = y; }
}GeoPos;

typedef struct  
{
	GeoPos begin;
	GeoPos end;
	bool bVisiable;
}GeoRect;

typedef struct
{
	std::list<MapEngineES_Point> PolyBorder;
	bool bVisiable;
	bool bLoop;//��ʼΪfalse ��ʾδ�պ�
}GeoPoly;


//default radius:30  intensity:.2
struct HeatPoint
{
	HeatPoint():size(30),intensity(.2),x(.0),y(.0){}
	void setSize(int rhs){ size = rhs;}
	void setIntensity(double intensity){ this->intensity = intensity;}
	void setGeoPos(double x,double y){
		this->x = x;
		this->y = y;
	}
	double x;
	double y;
	int size;
	double intensity;
};

const long MAP_JS_SCALE = 1000000;
const long MAP_ROW_SCALE = 1000;

//�༭ʹ�õ����ݽṹ
class PointInfoStore
{
public:
	struct PointRecorder_
	{
		long	iFid_;
		bool	bChanged_;
		bool	bDelete_;
		bool	bAdd_;
		bool	bEdit_;
		double	pointx_;
		double	pointy_;	
	};
	struct currentEditValue_
	{
		long valueid_;					//�༭������Ժ�
		std::string value_;				//�༭�������ֵ
	};
	struct EditValue_
	{
		long lPointId_;
		std::vector<currentEditValue_> currentEditStore_;				//��ǰ�༭�������Դ�ŵ�
	};
	struct ValueField_							//���ӵĵ������ֵ
	{
		long lPointId_;							//��ı��
		std::vector<std::string> sFieldValue_;	//�õ��Ӧ������ֵ
	};

public:
	std::vector<PointRecorder_> m_PointRecorderVector_;		//��Ƶ��������Ϣ�洢�����ʱ�༭״̬�洢��

	std::list<ValueField_> PointValueField_;			//�����ӵĵ����Ե���ʱ�洢λ�ã�����㣩
	ValueField_ myValue_;								//�������������ʱ��ţ������㣩


	long m_CurrentPointId;								//��ǰ�༭�ĵ�ı��
	EditValue_ currVal_;								//��ǰ�༭�������ֵ��ʱ�洢�㣨�����㣩
	std::list<EditValue_> EditPointStore_;				//�༭���ĵ��δ����ǰ�Ĵ�ŵ�ַ������㣩

	int ChoseLayerNum_;									//��Ƶ��༭��ʱ��ѡ�е�ͼ���
	std::vector<long> ChosePointNum_;					//��Ƶ��༭��ʱ��ѡ�еĵ���
public:
	PointInfoStore() 
	{
		ResetInfo();
	}
	~PointInfoStore() 
	{
		ResetInfo();
	}
private:
	void ResetInfo()
	{
		m_PointRecorderVector_.clear();
		PointValueField_.clear();
		myValue_.sFieldValue_.clear();
		m_CurrentPointId = -1;
		currVal_.currentEditStore_.clear();
		EditPointStore_.clear();
		ChoseLayerNum_ = -1;
		ChosePointNum_.clear();
	}

};

//��־�ļ�
extern FILE *tex_log;
//����ʱ�ɽ��ܷ�Χ
const long MAX_TIME_COST = 50;
//����cs
//CRITICAL_SECTION g_EnterDrawSection;

const double g_Zlocation = 1.0;
//��ͼ�������Ļ���
const int MAX_MAP_TEXTURE_TEMP = 2000;

//void Projection_to_Pixel(double prX,double prY,long &pix,long &piy);  Ӧ����DrawEngine�ĳ�Ա����
//void Pixel_to_Projection(long pix,long piy,double &prX,double &prY);

//ʸ���ߺ������ɫ������
const char *g_FShaderShpData = NULL;
const char *g_VShaderShpData = NULL;

//��ͨ�Ķ���Ķ�����ɫ�Ͷ�����ɫ������:
const char *g_FShaderData = NULL;
const char *g_VShaderData = NULL;

//Բ����ɫ�Ͷ�����ɫ������
//bool g_binitPoint = true; ���õ�LineElement.cpp�ĳ�Ա��
const char *g_VShaderCirclePointData = NULL;
const char *g_FShaderCirclePointData = NULL;


extern GLuint g_programObj;
extern GLuint g_vertexShader;
extern GLuint g_fragmentShader;

extern GLuint g_programObj_Shp;
extern GLuint g_fragmentShader_Shp;
extern GLuint g_vertexShader_Shp;
extern GLuint g_positionLoc_Shp;
extern GLuint g_texCoordLoc_Shp;
extern GLuint g_colorLoc_Shp;
extern GLuint g_MVPLoc_Shp;

extern GLuint g_textureLoc;
extern GLuint g_textureID;

extern GLuint  g_positionLoc;
extern GLuint  g_texCoordLoc;
extern GLuint  g_colorLoc;
extern GLuint  g_MVPLoc;
extern GLuint  g_vboID;
extern GLuint  g_ibID;

extern GLuint g_CPvertexShader ;
extern GLuint g_CPfragmentShader;
extern GLuint g_programObj_CP;
extern GLuint g_positionLoc_CP;
extern GLuint g_colorLoc_CP;
extern GLuint g_MVPLoc_Cp;
extern GLuint g_texCoordLoc_CP;

//add by wangyuejiao
extern GLfloat g_mapTranslationMatrix[16]; 
extern bool bChangeMapJS;
//end by wangyuejiao
extern HANDLE g_hRenderLineEvent;
extern HANDLE g_hThreadLineEvent;
extern HANDLE g_hAddLineEvent;
extern bool   g_UpdateLine;
extern bool g_qiepian;
#define pi 3.141593
/*******************************ȫ�ֺ���********************************/
CString UTF8ToGB2312(const char *str);
std::wstring s2ws(const std::string& s) ;
void initcode();
void InitProgram( void );
GLuint compileShader(GLenum type, const char *data);

extern LogFile gLog;


wchar_t* CharToWchar(char* c);
char* WcharToChar(wchar_t* wc);