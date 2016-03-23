#pragma once
#include "Stdafx.h"
#include "DrawEngine.h"
#include "WFSPointFeature.h"
#include "WFSLayer.h"
#include "ElementLayer.h"
#include<crtdbg.h>
#define DEBUGGPS

inline void EnableMemLeakCheck()
{
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
}

#ifdef _DEBUG
#define new   new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

class HelloTutorialInstance  //多继承                
	: public pp::Instance
	, public pp::Graphics3DClient  
{
public:
	/// The constructor creates the plugin-side instance.
	/// @param[in] instance the handle to the browser-side plugin instance.
	explicit HelloTutorialInstance(PP_Instance instance)
		: pp::Instance(instance)
		, pp::Graphics3DClient(this)
		, callback_factory_(this)
		, m_bFocusMap_(false)
		, m_bMapMove_(false)
		//, m_bpostPointMessage(false)
		,isFisrtPointMove(true)
		,penLineLayer(NULL)
		,isRightComplete(true)
		,isLeftComplete(false)
		,m_DrawEngine_(NULL)
	{
		EnableMemLeakCheck();
#ifdef DEBUGGPS
		if(NULL == tex_log)
			tex_log=fopen("D:\\MapEngine_log.txt","w+b");  //查看下 文件打开的模式
#endif
		this->RequestInputEvents(PP_INPUTEVENT_CLASS_MOUSE | PP_INPUTEVENT_CLASS_WHEEL);				//鼠标事件
		this->RequestFilteringInputEvents(PP_INPUTEVENT_CLASS_KEYBOARD);								//键盘事件
		m_DrawEngine_ = new DrawEngine(); 
		//测试先注册10个数字样式
		//放置在这里，invoke  glGenTextures(1, &m_textureID);就会出错,
		//ES环境还没有初始化完成。在HelloTutorialInstance::DidChangeView中完成初始化操作
/****************************************************************************/
		m_DrawEngine_->SetInstance(instance);
		
		//gLog.on(); //默认在浏览器当前的路径下 NINGBO.log
		//CString InstanceInfo;
		//InstanceInfo.Format(_T("DrawEngine instance[%d] created"),instance);
		//gLog.Log(InstanceInfo); 

		//Sleep(5000); //注释掉休眠，
		m_Grapics3d_ = NULL;
		//测试
		m_MouseDownPoint_.set_x(0);
		m_MouseDownPoint_.set_y(0);

	}

	virtual ~HelloTutorialInstance();

	virtual void Graphics3DContextLost()
	{
		PostMessage("Graphics3DContextLost");
	}
	// 系统级别类 接口

	//处理浏览器发给插件的消息  ，传递给DrawEngine对象去处理
	void HandleMessage(const pp::Var& var_message);

	//处理改变浏览器插件大小的消息
	void DidChangeView(const pp::Rect& position, const pp::Rect& clip);

	//处理改变是否选中插件的消息
	void DidChangeFocus(bool has_focus);

	//输入事件的消息响应鼠标事件
	bool HandleInputEvent (const pp::InputEvent &event);

	//。。。init（）
	bool HandleDocumentLoad (const pp::URLLoader &url_loader);

	//刷新
	void CtrlRefresh(pp::Point theBeg, pp::Point theEnd);
	//构造符合要求的热点信息
	//bool m_bpostPointMessage;
	//Json::Value returnPointFid;
	CWFSPointFeatureElementSet ChoosePointSet;  //被选中的热点集合
	GeoPos ChooseMark;
	void ConstructChooseInfo(const long& layerHandle, const enumChooseType& chooseType, const std::string& layerName, CWFSPointFeatureElementSet& theSet, Json::Value& returnInfo);
	void ConstructMarkInfo(const enumChooseType& chooseType,const GeoPos& ChooseMark,const int& LabelID,Json::Value& returnInfo);
	void ConstructClusterInfo(std::vector<int> pointSet);
	void PostToJs(int cmdid,CStringA cmdname,int status,int layerid,int elementid,int styleid,double x0,double y0,double x1,double y1,int js); //地图平移后向js投递新的地图信息
	void SetWFSLineUseMatrix(bool flag);
	void SetClusterLayerReload(bool flag);
private:
	DrawEngine* m_DrawEngine_;					//绘制
	pp::Graphics3D* m_Grapics3d_;				//浏览器的功能接口

	bool m_bFocusMap_;				//是否聚焦到地图
	bool m_bMapMove_;				//地图是否需要移动

	//测试学习使用的
	pp::Point m_MouseDownPoint_;
	PP_Point point;
	PP_Point point1;
	bool isFisrtPointMove;
	time_t dwStart,dwEnd;
	bool isRightComplete; //自由绘制 右键是否结束 初始化为true 第一次左击 create新的line
	bool isLeftComplete;
private:
	pp::CompletionCallbackFactory<HelloTutorialInstance> callback_factory_;
	void Render(int32_t result);
	/*void paintAtCoord(int x,int y){
		int count = 0;
		while (count < 5){
			double xoff = rand() / double(RAND_MAX) * 2 - 1;
			double yoff = rand() / double(RAND_MAX) * 2 - 1;
			double sum = xoff * xoff + yoff * yoff;
			if (sum > 1)
				continue;
			double ls = sqrt(sum);
			xoff /= ls; yoff /= ls;
			xoff *= (1 - sum);
			yoff *= (1 - sum);
			count += 1;
			if(m_DrawEngine_->_heatMap != NULL)
				m_DrawEngine_->_heatMap->addPoint(x + xoff *2 , y + yoff*2 , 30, 20 / 300);
		}
	}*/
	CElementLayer *penLineLayer; // 自由绘笔层
};