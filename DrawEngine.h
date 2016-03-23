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

 // ��ͼ���棬�����һ��instance�������������� "����ģʽ"
class DrawEngine
{
private:
	ULONG_PTR m_gdiplusToken;				// GDI+
	double m_PicScale_;						//ͼƬ���ű�	

	double m_xrotate_;
	double m_yrotate_;
	double m_zrotate_;
	double m_xtranslate_;
	double m_ytranslate_;
	double m_ztranslate_;

	bool m_bRefreshMap_;		// need refresh map or not;
	typedef void (DrawEngine::*HandleFun)(const Json::Value& root, Json::Value& returnInfo);//����ָ�� �������ͱ���
	std::map<std::string, HandleFun> m_Function;  //��Ϣ������
public:
	DrawEngine();				//��ʼ����g_Function
	~DrawEngine(){}
public: //��¶���ⲿ ʹ�õ� �ӿں���
	void GetCurrentShowInfo(double &cenlon,double &cenlat,double & width,double & height,double & leftlon,double & Toplat,double& lon,double &lat,double & scale,CString & Path,double &resolution);
	void Draw(int32_t result =1);				//�����ƺ���
	void CommandParsing(const std::string& CmdName, const Json::Value& root, Json::Value& returnInfo); // ����ǰ��js���͵�command
	void Projection_to_Pixel(double prX,double prY,long &pix,long &piy);  //���֮��� ��ͼ�ֱ��ʲ�ͬ,�������������������ǳ�Ա����
	void Pixel_to_Projection(long pix,long piy,double &prX,double &prY);
	void MoveView(double x,double y,double z,int& js);     //��Щ�ӿں���������PPAPI�Ŀ����invoke����������Ϊ public�Ĺ��нӿ�
	void Activate(const Json::Value& root, Json::Value& returnInfo);
private:
	void SetRect(const long& width, const long& height);  // �趨glViewport ��ü��� ����õ�Ӧ�õ� vertex֮�ϵ� mvp
	 int SetBaseLayer(CString name,double cenX,double cenY,int js,CString path,CString pictype,CString maptype,double minlon,double minlat,double resolution0);
	void RenderRect(PP_Point pStart,PP_Point pEnd);  //���� [������յ� pixel����]
	void RenderPolygon(std::list<MapEngineES_Point>& thePointList, const PP_Point& point_mouseMove);  //���� ����[��ѡ]	
//���öԸ�����Ϣ�Ĵ�����
	void OpenMapByJS(const Json::Value& root, Json::Value& returnInfo);	 //�򿪵�ͼ ��ʾ��ͼ
	void RegistStyleByJS(const Json::Value& root, Json::Value& returnInfo);
	
	void LoadWFSPolyLayerByJS(const Json::Value& root, Json::Value& returnInfo); //�������
	void LoadWFSLineLayerByJS(const Json::Value& root, Json::Value& returnInfo); //�����߲�
	void LoadTheWFSLayerByJS(const Json::Value& root, Json::Value& returnInfo);  //���ص��
	void MoveTheWFSLayerByJS(const Json::Value& root, Json::Value& returnInfo);  //�ƶ��ȵ�
//��WFSPoint��add��remove style 
	void AddStyleToWFSPointByJS(const Json::Value& root, Json::Value& returnInfo);
	void RemoveStyleFromWFSPointByJS(const Json::Value& root, Json::Value& returnInfo);
	void ModifyStyleWFSPointByJS(const Json::Value& root, Json::Value& returnInfo);

	void ChangeOperatorByJS(const Json::Value& root, Json::Value& returnInfo);
	void HideLayerByJS(const Json::Value& root, Json::Value& returnInfo);
	void ShowLayerByJS(const Json::Value& root, Json::Value& returnInfo);
	void DeleteLayerByJS(const Json::Value& root, Json::Value& returnInfo);	
	void MoveMapViewByJS(const Json::Value& root, Json::Value& returnInfo); //�ƶ�
	void BufferByJS(const Json::Value& root, Json::Value& returnInfo); //�߻��壬��·�ط�
    void GifCmdByJS(const Json::Value& root, Json::Value& returnInfo);
	void CtrlRefresh(const Json::Value& root, Json::Value& returnInfo);
	void GIFTimerProc(int);  //���ע��� �ص����������� ֻ��һ������
	void Render(int32_t result); //����ˢ��
	void DrawPolygonbystencil(); //����ģ�庯�����ƶ����
	void DrawRegisterPic(const long& xleft, const long& xright, const long& yleft, const long& yrigth); //��Ȩͼ��
	bool LoadImageFromResource(CImage* pImage,UINT nResID, LPCWSTR lpTyp);
//���ڲ�ѯ�ӿ�  WFSPoint
	void HideWFSPointByJS(const Json::Value& root, Json::Value& returnInfo);
	void ShowWFSPointByJS(const Json::Value& root, Json::Value& returnInfo);
//��ѯ WFSLine
	void HideWFSLineByJS(const Json::Value& root, Json::Value& returnInfo);
	void ShowWFSLineLayerByJS(const Json::Value& root, Json::Value& returnInfo);

//������Ƭʱ��ͼ�ڿ�����
	
//�����ע
	void ClearMark(const Json::Value& root, Json::Value& returnInfo); //���ȫ��
	void DeleteMark(const Json::Value& root, Json::Value& returnInfo);// ���ĳһ��
	void SynMark(const Json::Value& root, Json::Value& returnInfo); //ͬ����ǽ�ϱ�ע
//ɾ����ѡ��
	void DeleteChooseRect(const Json::Value& root, Json::Value& returnInfo);
	void ClearChooseRect(const Json::Value& root, Json::Value& returnInfo);
	void ShowChooseRect(const Json::Value& root, Json::Value& returnInfo);
	//void AddMarkByJS(const Json::Value& root, Json::Value& returnInfo);
//ɾ����ѡ��
	void DeleteChoosePoly(const Json::Value& root, Json::Value& returnInfo);
//ͬ�����ɻ��
	void SynPenDraw(const Json::Value& root, Json::Value& returnInfo);
public:  //��Ϊ��Щ������Ҫ��Instance::HandleInputEvent��invoke
	void ChangeCurrsorStyleByJS(const Json::Value& root, Json::Value& returnInfo);
	void ChangeCurrsorStyle(const std::string& theContext, PP_Instance& instance); //�������������ʽ�ĺ���
	void ChangeCurrsorStyle(const CString& ptheCursorPath, PP_Instance& instance);
	void ChangeCurrsorStyle(const std::string& theCursor); //���¼�����ʱʹ��

	void AddHeatMapLayer(const Json::Value& root, Json::Value& returnInfo);
	void AddClusterLayer(const Json::Value& root, Json::Value& returnInfo);
	//�������ص��ַ���
	Json::Value ConstructJsonValue(const std::string& CmdName, const int& CmdID, const int& elementID, 
						           const int& layerid, const int& status, const int& styleid, const int& mapLevel);
	Json::Value ConstructJsonValueforWFS(const std::string& CmdName, const int& CmdID, const int& elementID, 
	const int& layerid, const std::string& layerName,const int& status, const int& styleid, const int& mapLevel);

	 int RegistStyle(const std::string &image_path); //����ע�ắ��,����imge��ע��ID
    
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
//�������
	 void FillPoly(const vector<MapEngineES_Point> &PolyBorder,int r=255,int g=0,int b=0,int a=100);
//��Ⱦ����
	 void RenderFont(const string &font_content);
//���еĳ�Ա���� ����Ϊthread routine ������static,ȥ����������this
     static unsigned __stdcall UpdateWFSLineMethod(void *pM); //����WFSLineˢ�µ�thread ����
public:
	long m_CtrlWidth;				//�ؼ��Ŀ��
	long m_CtrlHeight;
	CVtronMap g_LayerManager;			//��ͼ  ͼ������

	int styleid;  //��ʼ��Ϊ0����ʾ��ʽstyle��map�е�key
	map<int,CStyle*> g_vecStyle;        //���� [��ʽ]������


	enumMapOperator g_ChooseOperator;	//Ĭ����ƽ�Ʋ���
	pp::Graphics3D* m_Grapics3d_;  //���Եõ� g_context���ó�Ա������HelloTutorialInstance::DidChangeView�б���ʼ��,swapbuff������ʹ��
	bool g_bDrawBegin; // ��ʼ��Ϊfalse�����ڻ����Ƿ���� �û�������ͼʱ�Ĺ켣[��Ƭ����ѡ����ѡ]
	bool g_bMoveCatch; //�����moveʱ�����������
	bool MouseMove; //���� ���ɻ��
	GLuint g_DrawRectVboid; //��ѡʱ����ͼ��ر���
	GLuint g_DrawPolyVboid;//��ѡʱ��VBO����
	// ���ƾ��� [���] �� [�յ�]
	PP_Point point_mouseDown;
	PP_Point point_mouseMove;
	PP_Point point_mouseUp;

	std::list<MapEngineES_Point> g_PolygonChoosePointList;  //װ�� [��ѡ]ʱ�� �������� �����ڻ���
	std::vector<POINT> g_PologynPointSet; // װ�� [��ѡ]ʱ�� pixel�������ڼ����ڿ��ڵ�FID
	int g_LineChooseRect;  //��ѡʱ����ʾ�ڵ�ͼ�ϵ� LineWidth

	//��GIF��ص� ���ƿ���
	bool g_bOnTime;
	bool g_bexistGifTimer;
	//ͼƬ����
	double g_picScale;
	//��ͼ����ʾ��Level��Χ
	int MinLevel,MaxLevel;
public: //������CLineElement���ܻ�ʹ�õ�
	GLfloat			g_mpv[16];  //ͶӰ �� ת�� �Ļ��۽��matrix
	PP_Resource g_context;   //Ӧ���Ƿ��뵽private�У� ���Ӧ�����൱��RC
	void SetContext(PP_Resource g_context) { this->g_context = g_context;}  
	//��instance�Ĺ��캯���У���invoke���

	PP_Instance g_instance;
	void SetInstance(PP_Resource instance) { this->g_instance = instance;}
	
	const PPB_Console *console_; //��־�ļ�

	int g_CtrlW;    //�����Ӧ�Ĵ��ڵĿ��
	int g_CtrlH;
	void SetWindow(int w,int h){ 
		g_CtrlW = w;
		g_CtrlH = h;
	}
	pp::CompletionCallbackFactory<DrawEngine> callback_factory_; //���ڹ���ص�����
	bool m_init; //��ʼ��Ϊtrue�����Ƴ�ʼ��ES�Ļ�ͼ����

	int m_level; //��¼��ͼ��С�ļ���
	int m_offset; //��ǰ��ͼ��ʾ��������Сʱ��ƫ����
	std::map<std::string,PP_MouseCursor_Type> m_cursorStyleID;
	PPB_MouseCursor* ppb_MouseCursor_interface;
	PP_Point theStart; //�ı������ʽ����ͼ���������
	enumChooseType m_ChooseType; //��ͼƽ�Ʋ����£����Ƶ�ѡvSingleChoose�� vRectChoose��������Ƭ���ǿ�ѡ
public:
	bool m_init_;
	time_t pre,now; //����ͳ��GPS��Ƶ��
	int lineR,lineG,lineB,lineA,lineW; //���ɻ�ʵ�line����
	
	bool b_Lable; //��ʼΪfalse
	int markstyle; //��עʹ�õ���ʽID
	int ID; //������ע��IDֵ��postmessage��ǰ��
	std::map<int,GeoPos> GeoLable;//��������
	std::vector<Vertex> mark_triangles; //��ͼ����Ƭ 

	typedef std::map<int,GeoPos>::value_type pair;
	void Handle(const pair &element);
	int m_rendW,m_rendH;

	int Status; // �����û� ����˹����� 1 ����DIV�в�ѯ��ť 0

	int RectChooseID; //���ο�ID
	bool m_bRectShow; //���ο�Ŀɼ���
	std::map<int,GeoRect> RectChooseSet; //��ž��ο�
	typedef map<int,GeoRect>::value_type ElementType;
    void HandleRender(const ElementType &element);
	
	unsigned poly_vboID; //����poly��vbo ESʹ��
	//int r_poly,g_poly,b_poly,a_poly;
	int PolyChooseID; //��ѡID
	bool m_bPolyShow; //�����Ķ���� �Ƿ���ʾ,���ֹ������е���������ڲ�ѯ�е��
	std::map<int,GeoPoly> PolyChooseSet;
	//void HandleRender(const map<int,GeoPoly>::value_type &element);
	int CircleChooseID;
	bool m_bCreateQiePian; //��Ƭʱ�Ƿ񴴽��µ���Ƭʵ��
	bool m_brightUp; // �Ҽ��ſ�
	CMarkerStyle *g_theLicenseStyle;  //����ע���ͼƬ
	bool bRegister; //����Ƿ�ע�� ��ʼ��false
	
	//render����
	CImage m_TextImage;
	BYTE *m_pdata;
	HDC m_canvas;
	GLuint m_textureid;
	GLuint m_vbo;
	bool bCreateFont;
	bool m_IsPoint; //���ɻ��������������������ߣ���ʼ��Ϊfalse
	void SearchByPoint(PP_Point& thePoint, GeoPos& theMark,int &LabelID);
	//HeatMap *_heatMap;
};
