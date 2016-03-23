#pragma once
#include "stdafxcpp.h"
#include "stdafxwin32.h"
#include <stdio.h>
#include"LogFile.h"
//几何类型
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

//地图操作方式
enum enumMapOperator
{
	vPanOperator = 0,		//浏览操作，平移，缩放
	vChooseOperator = 1,	//选择操作 就是[框选] 切片操作
	vDrawOperator = 2,		//画笔操作
	vPolygonChoose = 3,		//面选
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

//WFS中的 热点数据结构
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
	bool bLoop;//初始为false 表示未闭合
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

//编辑使用的数据结构
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
		long valueid_;					//编辑点的属性号
		std::string value_;				//编辑点的属性值
	};
	struct EditValue_
	{
		long lPointId_;
		std::vector<currentEditValue_> currentEditStore_;				//当前编辑过的属性存放点
	};
	struct ValueField_							//增加的点的属性值
	{
		long lPointId_;							//点的编号
		std::vector<std::string> sFieldValue_;	//该点对应的属性值
	};

public:
	std::vector<PointRecorder_> m_PointRecorderVector_;		//视频点坐标信息存储点和临时编辑状态存储点

	std::list<ValueField_> PointValueField_;			//新增加的点属性的临时存储位置（多个点）
	ValueField_ myValue_;								//新增点的属性临时存放（单个点）


	long m_CurrentPointId;								//当前编辑的点的编号
	EditValue_ currVal_;								//当前编辑点的属性值临时存储点（单个点）
	std::list<EditValue_> EditPointStore_;				//编辑过的点的未保存前的存放地址（多个点）

	int ChoseLayerNum_;									//视频点编辑的时候选中的图层号
	std::vector<long> ChosePointNum_;					//视频点编辑的时候选中的点编号
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

//日志文件
extern FILE *tex_log;
//最大耗时可接受范围
const long MAX_TIME_COST = 50;
//绘制cs
//CRITICAL_SECTION g_EnterDrawSection;

const double g_Zlocation = 1.0;
//地图纹理最大的缓存
const int MAX_MAP_TEXTURE_TEMP = 2000;

//void Projection_to_Pixel(double prX,double prY,long &pix,long &piy);  应该是DrawEngine的成员函数
//void Pixel_to_Projection(long pix,long piy,double &prX,double &prY);

//矢量线和面的着色器代码
const char *g_FShaderShpData = NULL;
const char *g_VShaderShpData = NULL;

//普通的对象的顶点着色和顶点作色器代码:
const char *g_FShaderData = NULL;
const char *g_VShaderData = NULL;

//圆点着色和顶点作色器代码
//bool g_binitPoint = true; 放置到LineElement.cpp的成员中
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
/*******************************全局函数********************************/
CString UTF8ToGB2312(const char *str);
std::wstring s2ws(const std::string& s) ;
void initcode();
void InitProgram( void );
GLuint compileShader(GLenum type, const char *data);

extern LogFile gLog;


wchar_t* CharToWchar(char* c);
char* WcharToChar(wchar_t* wc);