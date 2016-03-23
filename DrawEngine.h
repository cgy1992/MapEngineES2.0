#pragma once
#include "ShaderData.h"   
#include "DataStruct.h"
#include "matrix.h"
#include "VtronMap.h"
#include "Style.h"
#include "Stdafx.h"
#include "MarkerStyle.h"
#include "LogFile.h"
#include "HeatMap.h"

 // 绘图引擎，组件的一个instance，这里它采用了 "单例模式"
class DrawEngine
{
private:
	ULONG_PTR m_gdiplusToken;				// GDI+
	double m_PicScale_;						//图片缩放比	

	double m_xrotate_;
	double m_yrotate_;
	double m_zrotate_;
	double m_xtranslate_;
	double m_ytranslate_;
	double m_ztranslate_;

	bool m_bRefreshMap_;		// need refresh map or not;
	typedef void (DrawEngine::*HandleFun)(const Json::Value& root, Json::Value& returnInfo);//函数指针 类中类型别名
	std::map<std::string, HandleFun> m_Function;  //消息处理函数
public:
	DrawEngine();				//初始化了g_Function
	~DrawEngine(){}
public: //暴露给外部 使用的 接口函数
	void GetCurrentShowInfo(double &cenlon,double &cenlat,double & width,double & height,double & leftlon,double & Toplat,double& lon,double &lat,double & scale,CString & Path,double &resolution);
	void Draw(int32_t result =1);				//主绘制函数
	void CommandParsing(const std::string& CmdName, const Json::Value& root, Json::Value& returnInfo); // 解析前端js发送的command
	void Projection_to_Pixel(double prX,double prY,long &pix,long &piy);  //组件之间的 地图分辨率不同,所以这两个函数必须是成员函数
	void Pixel_to_Projection(long pix,long piy,double &prX,double &prY);
	void MoveView(double x,double y,double z,int& js);     //这些接口函数，会在PPAPI的框架中invoke，所以设置为 public的公有接口
	void Activate(const Json::Value& root, Json::Value& returnInfo);
private:
	void SetRect(const long& width, const long& height);  // 设定glViewport 与裁剪区 计算得到应用到 vertex之上的 mvp
	 int SetBaseLayer(CString name,double cenX,double cenY,int js,CString path,CString pictype,CString maptype,double minlon,double minlat,double resolution0);
	void RenderRect(PP_Point pStart,PP_Point pEnd);  //给出 [起点与终点 pixel坐标]
	void RenderPolygon(std::list<MapEngineES_Point>& thePointList, const PP_Point& point_mouseMove);  //用于 绘制[面选]	
//放置对各个消息的处理函数
	void OpenMapByJS(const Json::Value& root, Json::Value& returnInfo);	 //打开地图 显示底图
	void RegistStyleByJS(const Json::Value& root, Json::Value& returnInfo);
	
	void LoadWFSPolyLayerByJS(const Json::Value& root, Json::Value& returnInfo); //加载面层
	void LoadWFSLineLayerByJS(const Json::Value& root, Json::Value& returnInfo); //加载线层
	void LoadTheWFSLayerByJS(const Json::Value& root, Json::Value& returnInfo);  //加载点层
	void MoveTheWFSLayerByJS(const Json::Value& root, Json::Value& returnInfo);  //移动热点
//在WFSPoint上add与remove style 
	void AddStyleToWFSPointByJS(const Json::Value& root, Json::Value& returnInfo);
	void RemoveStyleFromWFSPointByJS(const Json::Value& root, Json::Value& returnInfo);
	void ModifyStyleWFSPointByJS(const Json::Value& root, Json::Value& returnInfo);

	void ChangeOperatorByJS(const Json::Value& root, Json::Value& returnInfo);
	void HideLayerByJS(const Json::Value& root, Json::Value& returnInfo);
	void ShowLayerByJS(const Json::Value& root, Json::Value& returnInfo);
	void DeleteLayerByJS(const Json::Value& root, Json::Value& returnInfo);	
	void MoveMapViewByJS(const Json::Value& root, Json::Value& returnInfo); //移动
	void BufferByJS(const Json::Value& root, Json::Value& returnInfo); //线缓冲，道路回放
    void GifCmdByJS(const Json::Value& root, Json::Value& returnInfo);
	void CtrlRefresh(const Json::Value& root, Json::Value& returnInfo);
	void GIFTimerProc(int);  //这个注册的 回调函数必须是 只带一个参数
	void Render(int32_t result); //辅助刷新
	void DrawPolygonbystencil(); //测试模板函数绘制多边形
	void DrawRegisterPic(const long& xleft, const long& xright, const long& yleft, const long& yrigth); //版权图标
	bool LoadImageFromResource(CImage* pImage,UINT nResID, LPCWSTR lpTyp);
//用于查询接口  WFSPoint
	void HideWFSPointByJS(const Json::Value& root, Json::Value& returnInfo);
	void ShowWFSPointByJS(const Json::Value& root, Json::Value& returnInfo);
//查询 WFSLine
	void HideWFSLineByJS(const Json::Value& root, Json::Value& returnInfo);
	void ShowWFSLineLayerByJS(const Json::Value& root, Json::Value& returnInfo);

//存在切片时地图黑块问题
	
//清除标注
	void ClearMark(const Json::Value& root, Json::Value& returnInfo); //清除全部
	void DeleteMark(const Json::Value& root, Json::Value& returnInfo);// 清除某一个
	void SynMark(const Json::Value& root, Json::Value& returnInfo); //同步大墙上标注
//删除框选框
	void DeleteChooseRect(const Json::Value& root, Json::Value& returnInfo);
	void ClearChooseRect(const Json::Value& root, Json::Value& returnInfo);
	void ShowChooseRect(const Json::Value& root, Json::Value& returnInfo);
	//void AddMarkByJS(const Json::Value& root, Json::Value& returnInfo);
//删除面选框
	void DeleteChoosePoly(const Json::Value& root, Json::Value& returnInfo);
//同步自由绘笔
	void SynPenDraw(const Json::Value& root, Json::Value& returnInfo);
public:  //因为这些函数需要在Instance::HandleInputEvent中invoke
	void ChangeCurrsorStyleByJS(const Json::Value& root, Json::Value& returnInfo);
	void ChangeCurrsorStyle(const std::string& theContext, PP_Instance& instance); //辅助更改鼠标样式的函数
	void ChangeCurrsorStyle(const CString& ptheCursorPath, PP_Instance& instance);
	void ChangeCurrsorStyle(const std::string& theCursor); //供事件处理时使用

	void AddHeatMapLayer(const Json::Value& root, Json::Value& returnInfo);
	void AddClusterLayer(const Json::Value& root, Json::Value& returnInfo);
	//构建返回的字符串
	Json::Value ConstructJsonValue(const std::string& CmdName, const int& CmdID, const int& elementID, 
						           const int& layerid, const int& status, const int& styleid, const int& mapLevel);
	Json::Value ConstructJsonValueforWFS(const std::string& CmdName, const int& CmdID, const int& elementID, 
	const int& layerid, const std::string& layerName,const int& status, const int& styleid, const int& mapLevel);

	 int RegistStyle(const std::string &image_path); //辅助注册函数,返回imge的注册ID
    
	 void Log(PP_LogLevel level, const char* format, ...)
	 {
	 	va_list args;
		va_start(args, format);
		char buf[512];
		vsnprintf(buf, sizeof(buf) - 1, format, args);
		buf[sizeof(buf) - 1] = '\0';
		va_end(args);

		pp::Var value(buf);
		console_->Log(g_instance, level, value.pp_var());
	 }
//填充多边形
	 void FillPoly(const vector<MapEngineES_Point> &PolyBorder,int r=255,int g=0,int b=0,int a=100);
//渲染文字
	 void RenderFont(const string &font_content);
//类中的成员函数 设置为thread routine 必须是static,去掉隐含参数this
     static unsigned __stdcall UpdateWFSLineMethod(void *pM); //辅助WFSLine刷新的thread 例程
public:
	long m_CtrlWidth;				//控件的宽高
	long m_CtrlHeight;
	CVtronMap g_LayerManager;			//地图  图层容器

	int styleid;  //初始化为0，表示样式style在map中的key
	map<int,CStyle*> g_vecStyle;        //管理 [样式]的容器


	enumMapOperator g_ChooseOperator;	//默认是平移操作
	pp::Graphics3D* m_Grapics3d_;  //可以得到 g_context，该成员变量在HelloTutorialInstance::DidChangeView中被初始化,swapbuff函数会使用
	bool g_bDrawBegin; // 初始化为false，用于绘制是否绘制 用户操作地图时的轨迹[切片、线选、面选]
	bool g_bMoveCatch; //鼠标移move时，捕获坐标点
	bool MouseMove; //控制 自由绘笔
	GLuint g_DrawRectVboid; //框选时，绘图相关变量
	GLuint g_DrawPolyVboid;//面选时，VBO对象
	// 绘制矩形 [起点] 与 [终点]
	PP_Point point_mouseDown;
	PP_Point point_mouseMove;
	PP_Point point_mouseUp;

	std::list<MapEngineES_Point> g_PolygonChoosePointList;  //装入 [面选]时的 地理坐标 ，用于绘制
	std::vector<POINT> g_PologynPointSet; // 装入 [面选]时的 pixel坐标用于计算在框内的FID
	int g_LineChooseRect;  //面选时，显示在地图上的 LineWidth

	//与GIF相关的 控制开关
	bool g_bOnTime;
	bool g_bexistGifTimer;
	//图片比例
	double g_picScale;
	//地图的显示的Level范围
	int MinLevel,MaxLevel;
public: //其他的CLineElement可能会使用到
	GLfloat			g_mpv[16];  //投影 及 转换 的积累结果matrix
	PP_Resource g_context;   //应该是放入到private中， 这个应该是相当于RC
	void SetContext(PP_Resource g_context) { this->g_context = g_context;}  
	//在instance的构造函数中，会invoke这个

	PP_Instance g_instance;
	void SetInstance(PP_Resource instance) { this->g_instance = instance;}
	
	const PPB_Console *console_; //日志文件

	int g_CtrlW;    //组件对应的窗口的宽高
	int g_CtrlH;
	void SetWindow(int w,int h){ 
		g_CtrlW = w;
		g_CtrlH = h;
	}
	pp::CompletionCallbackFactory<DrawEngine> callback_factory_; //用于构造回调函数
	bool m_init; //初始化为true，控制初始化ES的绘图环境

	int m_level; //记录地图最小的级别
	int m_offset; //当前地图显示级别与最小时的偏移量
	std::map<std::string,PP_MouseCursor_Type> m_cursorStyleID;
	PPB_MouseCursor* ppb_MouseCursor_interface;
	PP_Point theStart; //改变鼠标样式，贴图的起点坐标
	enumChooseType m_ChooseType; //地图平移操作下，控制点选vSingleChoose、 vRectChoose区分是切片还是框选
public:
	bool m_init_;
	time_t pre,now; //用于统计GPS的频率
	int lineR,lineG,lineB,lineA,lineW; //自由绘笔的line属性
	
	bool b_Lable; //初始为false
	int markstyle; //标注使用的样式ID
	int ID; //各个标注的ID值，postmessage给前端
	std::map<int,GeoPos> GeoLable;//地理坐标
	std::vector<Vertex> mark_triangles; //绘图三角片 

	typedef std::map<int,GeoPos>::value_type pair;
	void Handle(const pair &element);
	int m_rendW,m_rendH;

	int Status; // 区分用户 点击了工具栏 1 还是DIV中查询按钮 0

	int RectChooseID; //矩形框ID
	bool m_bRectShow; //矩形框的可见性
	std::map<int,GeoRect> RectChooseSet; //存放矩形框
	typedef map<int,GeoRect>::value_type ElementType;
    void HandleRender(const ElementType &element);
	
	unsigned poly_vboID; //绘制poly的vbo ES使用
	//int r_poly,g_poly,b_poly,a_poly;
	int PolyChooseID; //面选ID
	bool m_bPolyShow; //创建的多边形 是否显示,区分工具条中点击，还是在查询中点击
	std::map<int,GeoPoly> PolyChooseSet;
	//void HandleRender(const map<int,GeoPoly>::value_type &element);
	int CircleChooseID;
	bool m_bCreateQiePian; //切片时是否创建新的切片实例
	bool m_brightUp; // 右键放开
	CMarkerStyle *g_theLicenseStyle;  //绘制注册的图片
	bool bRegister; //软件是否注册 初始化false
	
	//render文字
	CImage m_TextImage;
	BYTE *m_pdata;
	HDC m_canvas;
	GLuint m_textureid;
	GLuint m_vbo;
	bool bCreateFont;
	bool m_IsPoint; //自由绘笔是连续，还是连点成线，初始化为false
	void SearchByPoint(PP_Point& thePoint, GeoPos& theMark,int &LabelID);
	//HeatMap *_heatMap;
};
