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

class HelloTutorialInstance  //��̳�                
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
			tex_log=fopen("D:\\MapEngine_log.txt","w+b");  //�鿴�� �ļ��򿪵�ģʽ
#endif
		this->RequestInputEvents(PP_INPUTEVENT_CLASS_MOUSE | PP_INPUTEVENT_CLASS_WHEEL);				//����¼�
		this->RequestFilteringInputEvents(PP_INPUTEVENT_CLASS_KEYBOARD);								//�����¼�
		m_DrawEngine_ = new DrawEngine(); 
		//������ע��10��������ʽ
		//���������invoke  glGenTextures(1, &m_textureID);�ͻ����,
		//ES������û�г�ʼ����ɡ���HelloTutorialInstance::DidChangeView����ɳ�ʼ������
/****************************************************************************/
		m_DrawEngine_->SetInstance(instance);
		
		//gLog.on(); //Ĭ�����������ǰ��·���� NINGBO.log
		//CString InstanceInfo;
		//InstanceInfo.Format(_T("DrawEngine instance[%d] created"),instance);
		//gLog.Log(InstanceInfo); 

		//Sleep(5000); //ע�͵����ߣ�
		m_Grapics3d_ = NULL;
		//����
		m_MouseDownPoint_.set_x(0);
		m_MouseDownPoint_.set_y(0);

	}

	virtual ~HelloTutorialInstance();

	virtual void Graphics3DContextLost()
	{
		PostMessage("Graphics3DContextLost");
	}
	// ϵͳ������ �ӿ�

	//��������������������Ϣ  �����ݸ�DrawEngine����ȥ����
	void HandleMessage(const pp::Var& var_message);

	//����ı�����������С����Ϣ
	void DidChangeView(const pp::Rect& position, const pp::Rect& clip);

	//����ı��Ƿ�ѡ�в������Ϣ
	void DidChangeFocus(bool has_focus);

	//�����¼�����Ϣ��Ӧ����¼�
	bool HandleInputEvent (const pp::InputEvent &event);

	//������init����
	bool HandleDocumentLoad (const pp::URLLoader &url_loader);

	//ˢ��
	void CtrlRefresh(pp::Point theBeg, pp::Point theEnd);
	//�������Ҫ����ȵ���Ϣ
	//bool m_bpostPointMessage;
	//Json::Value returnPointFid;
	CWFSPointFeatureElementSet ChoosePointSet;  //��ѡ�е��ȵ㼯��
	GeoPos ChooseMark;
	void ConstructChooseInfo(const long& layerHandle, const enumChooseType& chooseType, const std::string& layerName, CWFSPointFeatureElementSet& theSet, Json::Value& returnInfo);
	void ConstructMarkInfo(const enumChooseType& chooseType,const GeoPos& ChooseMark,const int& LabelID,Json::Value& returnInfo);
	void ConstructClusterInfo(std::vector<int> pointSet);
	void PostToJs(int cmdid,CStringA cmdname,int status,int layerid,int elementid,int styleid,double x0,double y0,double x1,double y1,int js); //��ͼƽ�ƺ���jsͶ���µĵ�ͼ��Ϣ
	void SetWFSLineUseMatrix(bool flag);
	void SetClusterLayerReload(bool flag);
private:
	DrawEngine* m_DrawEngine_;					//����
	pp::Graphics3D* m_Grapics3d_;				//������Ĺ��ܽӿ�

	bool m_bFocusMap_;				//�Ƿ�۽�����ͼ
	bool m_bMapMove_;				//��ͼ�Ƿ���Ҫ�ƶ�

	//����ѧϰʹ�õ�
	pp::Point m_MouseDownPoint_;
	PP_Point point;
	PP_Point point1;
	bool isFisrtPointMove;
	time_t dwStart,dwEnd;
	bool isRightComplete; //���ɻ��� �Ҽ��Ƿ���� ��ʼ��Ϊtrue ��һ����� create�µ�line
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
	CElementLayer *penLineLayer; // ���ɻ�ʲ�
};