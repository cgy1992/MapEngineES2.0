#include "DrawEngine.h"
#include "LineElement.h"
#include "CircleElement.h"
#include "ArcgisMapLayer.h"
#include "WFSLayer.h"
#include "ElementLayer.h"
#include "GifLayer.h"
#include "GifElement.h"
#include "MapServerLayer.h"
#include "license.h"
#include <algorithm>
#include <sstream>
#include <time.h>
#include <iterator>
#include <Windows.h>
#include "resource.h"
#include <boost/thread/thread.hpp>
#include "HeatMapLayer.h"
#include "ClusterLayer.h"

CCircleElement* g_ChooseCircleElement = NULL; //ÓÃÓÚ»æÖÆÈ¦Ñ¡
DrawEngine::DrawEngine()
	: m_bRefreshMap_(true)
	, m_xrotate_(-45)
	, m_yrotate_(0)
	, m_zrotate_(0)
	, m_xtranslate_(0)
	, m_ytranslate_(0)
	, m_ztranslate_(0)
	, styleid(0)
	, g_bDrawBegin(false)
	, g_bMoveCatch(false)
	, MouseMove(false)
	, g_bOnTime(false)
	, g_bexistGifTimer(false)
	, g_picScale(1.0)
	, m_init(true)
	, callback_factory_(this)
	, m_ChooseType(vNULL)
	, m_init_(true)
	, b_Lable(false)
	, m_rendW(32)
	, m_rendH(32)
	, ID(1)
	, RectChooseID(1)
	, Status(0)
	, m_bRectShow(false)
	, m_bPolyShow(false)
	, CircleChooseID(1)
	, PolyChooseID(1)
	, g_DrawRectVboid(0)
	, g_DrawPolyVboid(0)
	, m_brightUp(false)
	, poly_vboID(0)
	,m_bCreateQiePian(false)
	,g_theLicenseStyle(NULL)
	,bRegister(false)
	,bCreateFont(true)
	,m_textureid(0)
	,m_vbo(0)
	,m_IsPoint(false)
	//,_heatMap(NULL)
{
	gLog.on();
	//gdi+³õÊ¼»¯
	Gdiplus::GdiplusStartupInput m_gdiplusStartupInput;	// GDI+
	GdiplusStartup(&m_gdiplusToken, &m_gdiplusStartupInput, NULL);
	//m_Grapics3d_ = NULL;
	m_PicScale_ = 1.0;
	m_Function.clear();//Çå¿ÕÔ­ÓÐµÄ´¦Àíº¯Êý
	g_ChooseOperator = enumMapOperator::vPanOperator;  //Ñ¡Ôñ²Ù×÷ Õâ¸ö±äÁ¿Ó¦¸Ã·ÅÖÃ»æÍ¼ÒýÇæµ±ÖÐ£¬Ä¬ÈÏÊÇÆ½ÒÆ²Ù×÷
	m_Function["OPEN"] = &DrawEngine::OpenMapByJS;
	//WFSPoly
	m_Function["ADDWFSPOLYLAYER"] = &DrawEngine::LoadWFSPolyLayerByJS;
	//WFSLine
	m_Function["ADDWFSLINELAYER"] = &DrawEngine::LoadWFSLineLayerByJS;
	m_Function["UPDATEWFSLINELAYER"] = &DrawEngine::LoadWFSLineLayerByJS;  //µÀÂ·¸üÐÂ
	//queryLine
	m_Function["SHOWWFSLINELAYER"] = &DrawEngine::ShowWFSLineLayerByJS;
	m_Function["HIDEWFSLINELAYER"] = &DrawEngine::HideWFSLineByJS;
	//WFSPoint
	m_Function["REGMARKERSTYLE"] = &DrawEngine::RegistStyleByJS;
	m_Function["ADDWFSPOINTLAYER"] = &DrawEngine::LoadTheWFSLayerByJS;
	m_Function["UPDATEWFSPOINTLAYER"] = &DrawEngine::LoadTheWFSLayerByJS;
	m_Function["MODIFYWFSPOINTLAYER"] = &DrawEngine::LoadTheWFSLayerByJS; //ÐÞ¸ÄÈÈµãÑùÊ½
	m_Function["MOVEWFSPOINTLAYER"] = &DrawEngine::MoveTheWFSLayerByJS; //GPSÒÆ¶¯
	//query WFSPoint
	m_Function["HIDEWFSPOINTLAYER"] = &DrawEngine::HideWFSPointByJS;
	m_Function["SHOWWFSPOINTLAYER"] = &DrawEngine::ShowWFSPointByJS;
	//ÔÚ WFSPointÉÏµþ ÑùÊ½
	m_Function["ADDSTYLETOWFSPOINT"] = &DrawEngine::AddStyleToWFSPointByJS;
	m_Function["REMOVESTYLEFROMWFSPOINT"] = &DrawEngine::RemoveStyleFromWFSPointByJS;
	m_Function["MODIFYSTYLEWFSPOINT"] = &DrawEngine::ModifyStyleWFSPointByJS;
	//¹ØÓÚµØÍ¼µÄ²Ù×÷
	m_Function["OPERATORCHANGE"] = &DrawEngine::ChangeOperatorByJS; //ÇÐÆ¬
	m_Function["MOVEVIEW"] = &DrawEngine::MoveMapViewByJS; //chong
	//ÏßÂ·»Ø·Å render GIF
	m_Function["BUFFER"] = &DrawEngine::BufferByJS; 
	m_Function["GIFCMD"] = &DrawEngine::GifCmdByJS;
	//Õû¸öÍ¼²ã²Ù×÷
	m_Function["HIDELAYER"] = &DrawEngine::HideLayerByJS;
	m_Function["SHOWLAYER"] = &DrawEngine::ShowLayerByJS;
	m_Function["DELETELAYER"] = &DrawEngine::DeleteLayerByJS;
	//¸Ä±äÊó±êÑùÊ½
	m_Function["CHANGECURSORSTYLE"] =&DrawEngine::ChangeCurrsorStyleByJS;
	m_Function["CRTLREFRESH"] = &DrawEngine::CtrlRefresh;
	m_Function["ACTIVATE"] = &DrawEngine::Activate;  
	//Í¼Æ¬±ê×¢
	m_Function["ClearMark"] = &DrawEngine::ClearMark;
	m_Function["DeleteMark"] = &DrawEngine::DeleteMark;
	m_Function["AddMarkByLabel"] = &DrawEngine::SynMark;
	//¿òÑ¡¾ØÐÎ¿ò²Ù×÷
	m_Function["DeleteChooseRect"] = &DrawEngine::DeleteChooseRect;
	m_Function["ClearChooseRect"] = &DrawEngine::ClearChooseRect;
	m_Function["ShowChooseRect"] = &DrawEngine::ShowChooseRect;
	//ÃæÑ¡²Ù×÷
	m_Function["DeleteChoosePoly"] = &DrawEngine::DeleteChoosePoly;
	//Í¬²½×ÔÓÉ»æ±Ê
	m_Function["DRAWAGAIN"] = &DrawEngine::SynPenDraw;
	
	m_Function["HEATLAYER"] = &DrawEngine::AddHeatMapLayer;
	m_Function["CLUSTERLAYER"] = &DrawEngine::AddClusterLayer;
	
	//Êó±êÑùÊ½
	m_cursorStyleID["POINT"] = PP_MOUSECURSOR_TYPE_HAND; //40  ÊÖÖ¸
	m_cursorStyleID["MOVE"] =  PP_MOUSECURSOR_TYPE_GRABBING;// 41  °´ÏÂÈ¥ÊÖÕÂ
	m_cursorStyleID["HAND"] = PP_MOUSECURSOR_TYPE_GRAB;   //42  ÊÖÕÆ  PP_MOUSECURSOR_TYPE_GRAB;




	ppb_MouseCursor_interface = (PPB_MouseCursor*) ((pp::Module::Get()->get_browser_interface())(PPB_MOUSECURSOR_INTERFACE));
    console_ = reinterpret_cast<const PPB_Console*>(pp::Module::Get()->GetBrowserInterface(PPB_CONSOLE_INTERFACE));
	//Êó±êÌùÍ¼µÄÏìÓ¦µã
	theStart.x = 0;
	theStart.y = 0;	
	::g_hThreadLineEvent = CreateEvent(NULL,false,true,NULL);  //³õÊ¼»¯Ò»¸ö ÒÑ´¥·¢×Ô¶¯¸´Î»ÊÂ¼þ
	now = time(NULL); //Ê±¼ä´Á
	pre = now;
}

//ÓÐÇÐÆ¬µÄ»·¾³£¬¼´×é¼þ±»ÊµÀý»¯¶à¸ö£¬ÓÃÕâ¸öº¯ÊýÏÔÊ¾µÄ¸æËßopengl ÔÚ²Ù×÷ÄÇ¸öinstance
//Ô¤·ÀµØÍ¼ºÚ¿é
//µ±Ç°×ö·¨ÊÇÔÚMoveTheWFSLayerByJS GPSÖÐ¼ÓÈëÕâÒ»¾ä

//Ò»´ÎÐÔ Ìí¼Ó Ò»ÌõLine, step1:Ê×ÏÈÕÒµ½×ÔÓÉ»æ±Ê²ã step2:
void DrawEngine::SynPenDraw(const Json::Value& root, Json::Value& returnInfo)
{
	int layerid = -1;
	CElementLayer *penLineLayer = NULL; //ÓÉÍ¼²ãÈÝÆ÷£¬¸ºÔð»ØÊÕ¸Ã×ÊÔ´£¬ÔÚC versionÖÐ£¬¸ÃÍ¼²ãÊÇÓÉÈ«¾Ö±äÁ¿³ÖÓÐ
	CLayer *tmp = NULL;
	tmp = g_LayerManager.SearchByLayerType(VLayerType::vTempLayer,&layerid);
	if(tmp)
	{
		penLineLayer = static_cast<CElementLayer *>(tmp);
	}
	else
	{
		penLineLayer = new CElementLayer(this); //¶àÌí¼ÓÒ»¸ö this²ÎÊý
		penLineLayer->m_layerType = VLayerType::vTempLayer;  //ÁÙÊ±»æÏß²ã£¬
	    g_LayerManager.AddLayer(penLineLayer);  //½ÓÏÂÀ´ ¶ÔÕâ¸ö²ãµÄ²Ù×÷ÊÇ Í¨¹ýÏìÓ¦Êó±êÊÂ¼þÀ´Íê³ÉµÄ£¬·µ»ØÖµÊÇÍ¼²ãIDºÅ
	}
		//»ñÈ¡µ½ÏßÊôÐÔ£¬×¢ÏßelementµÄ´´½¨ÊÇÔÚÊó±êµã»÷Ê±´´½¨£¬ÕâÀïµÄÏßÊôÐÔ ÊÇÈ«¾Ö±äÁ¿
	lineW = root["Width"].asInt();
	lineR = root["ColorR"].asInt();
	lineG = root["ColorG"].asInt();
	lineB = root["ColorB"].asInt();
	lineA = root["ColorA"].asInt();
	
	//Éú³ÉÐÂµÄLine
	CLineElement *line = new CLineElement(lineR,lineG,lineB,lineA,lineW,this);
	//³õÊ¼»¯LineµÄ×ø±ê,Ôö¼ÓÒ»Ìõline
	Json::Value posX = root["X"];
	Json::Value poxY = root["Y"];
	if(!posX.empty())
	{
		int len = posX.size();
		for(int i = 0; i < len;++i)
		{
			double x,y;
			x = posX[i].asDouble();
			y = poxY[i].asDouble();
			line->AddPoint(x,y);
		}
	}
	line->m_type = 1; //»áinvoke CLineElement::DrawLines()
	penLineLayer->AddElement(line);
	Draw(); //ÖØ»æ
}

void DrawEngine::SynMark(const Json::Value& root, Json::Value& returnInfo)
{
	if(!b_Lable)  //»æÖÆ±ê×¢
		b_Lable = true;
	markstyle = root["StyleID"].asInt();
	m_rendW = root["Width"].asInt();
	m_rendH = root["Height"].asInt();
	if(root["MarkSet"].empty())
		return;
	int mark_num = root["MarkSet"].size();
	for(int i = 0;i < mark_num;++i)
	{
		int LabelID = root["MarkSet"][i]["LabelID"].asInt();
		if(0 == GeoLable.count(LabelID)) //Èç¹ûÔÚ±ê¼ÇÈÝÆ÷ÖÐ ²»´æÔÚ
		{
		  if(2 != root["MarkSet"][i]["GeoPos"].size()) //×ø±êÖµ
			  continue;
		  GeoPos pos; //²åÈëÒ»¸öÐÂ±ê×¢
		  pos.x_ = root["MarkSet"][i]["GeoPos"][UINT(0)].asDouble();
		  pos.y_ = root["MarkSet"][i]["GeoPos"][1].asDouble();
		  GeoLable.insert(make_pair(LabelID,pos));
		}
	}
	Draw(); //ÖØ»æ
}

void DrawEngine::ClearMark(const Json::Value& root, Json::Value& returnInfo)
{
	g_ChooseOperator = enumMapOperator::vPanOperator;
	ID = 1;
	b_Lable = false;
	GeoLable.clear();
	mark_triangles.clear();
	Draw();
}

void DrawEngine::DeleteMark(const Json::Value& root, Json::Value& returnInfo)
{
	int ID = root["LabelID"].asInt();
	auto iter = GeoLable.find(ID);
	if(iter != GeoLable.end())
	{
		GeoLable.erase(iter);
	}
	if(GeoLable.empty())
	{
		b_Lable = false;
	}
	Draw();
}
void DrawEngine::Activate(const Json::Value& root, Json::Value& returnInfo)
{
	glSetCurrentContextPPAPI(g_context);
}

int DrawEngine::SetBaseLayer(CString name,double cenX,double cenY,int js,CString path,CString pictype,CString maptype,double minlon,double minlat,double resolution0)
{	
	//Æµ·±µÄÊ¹ÓÃÖ¸Õë£¬ÕâÐ©×ÊÔ´ÊÇÊ²Ã´Ê±ºòÊÍ·ÅµÄ£¿£¿£¿ ÔÚPPAPI¿ò¼Ü½áÊøµÄÊ±ºòÊÍ·Å×ÊÔ´
	CBaseLayer *baselayer = NULL;
	// Ã¿Ò»¸öÍ¼²ã layer ¶¼ÓÐÒ»¸öÀàÐÍµÄÊôÐÔ
	if (maptype==L"VtronMap")
	{
		//baselayer = new CVtronMapLayer(x,y,js,name,256,256,path,pictype);
	}
	else if (maptype == L"ArcGisMap")
	{
		baselayer = new CArcgisMapLayer(cenX,cenY,js,name,256,256,path,pictype,this);   // ArcGis  Äþ²¨¹«½»
	}
	else if (maptype == L"ArcGisCompactMap")
	{
		//baselayer = new CArcgisCompactMapLayer(x,y,js,name,256,256,path,pictype);
	}
	else if (maptype == L"ArcServerMap")   
	{
		//baselayer = new CMapServerLayer(x,y,js,name,256,256,path,pictype,L"ArcServer");
	}
	else if (maptype == L"DeegreeMap")
	{
		//baselayer = new CMapServerLayer(x,y,js,name,256,256,path,pictype,L"DegreeServer");
	}
	else if (maptype == L"GeoBeansMap")   // ÇØ»Êµº ±¾×é¼þÖ÷ÒªµÄµ×Í¼
	{
		baselayer = new CMapServerLayer(cenX,cenY,js,name,256,256,path,pictype,L"GeoBeansServer",minlon,minlat,resolution0,this);  //´´½¨µØÍ¼Í¼²ã ¶ÔÏó
	}
	else if (maptype == L"PGISServerMap") //Ö§³ÖpgisµØÍ¼
	{
		baselayer = new CMapServerLayer(cenX,cenY,js,name,256,256,path,pictype,L"PGISServer",minlon,minlat,resolution0,this);
	}
	else
	{
		//baselayer = new CVtronMapLayer(x,y,js,name,256,256,path,pictype);//Ä¬ÈÏ
	}
	//±£´æµØÍ¼µÄ ÏÔÊ¾·¶Î§²ã¼¶ ÏÞ¶¨ÓÃ»§Ëõ·Å·¶Î§
	baselayer->m_MinLevel = MinLevel;
	baselayer->m_MaxLevel = MaxLevel;
	this->m_level = MinLevel;

	g_LayerManager.AddLayer(baselayer);
	baselayer->m_js = js;  //µØÍ¼µ±Ç°ÏÔÊ¾¼¶±ð
	Draw(); //Ö÷¶¯Òý·¢ ÖØ»æ²Ù×÷

	return 0;
}

void DrawEngine::CtrlRefresh(const Json::Value& root, Json::Value& returnInfo)
{
  Draw();
}

void DrawEngine::OpenMapByJS(const Json::Value& root, Json::Value& returnInfo)
{
	Activate(root,returnInfo);
	g_qiepian = true;
	bool bclearup = root["ClearUp"].asBool();
	if (g_LayerManager.GetLayerCount() != 0 && bclearup)
	{
		g_LayerManager.release();
		g_ChooseOperator = enumMapOperator::vPanOperator; 
	}
	string map = root["MapName"].asString();
	//CString cmap = UTF8ToGB2312(map.c_str());  //½« const char* --> CString,²»ÐèÒª
	CString cmap(map.c_str());
//µØÍ¼ÖÐÐÄµãÎ»ÖÃ
	double x = root["CenX"].asDouble();
	double y = root["CenY"].asDouble();

	int js = root["Level"].asInt(); //µ±Ç°
	MinLevel = root["MinLevel"].asInt();
	MaxLevel = root["MaxLevel"].asInt();

	string path = root["URL"].asString(); 
	CString cspath = UTF8ToGB2312(path.c_str());

	string type = root["Format"].asString();  
	CString cstype = UTF8ToGB2312(type.c_str());

	string maptype = root["MapType"].asString();  
	CString csmaptype = UTF8ToGB2312(maptype.c_str());

	//»ñÈ¡×óÉÏ½Ç×ø±ê
	if (csmaptype == "GeoBeansMap" || csmaptype == "PGISServerMap")   
	{

		double OriginY = root["MinLat"].asDouble();
		double OriginX = root["MinLon"].asDouble();
		double maxX = root["MaxLon"].asDouble();
		double maxY = root["MaxLat"].asDouble();
		double resolution0 = root["Resolution0"].asDouble();
		SetBaseLayer(cmap,x,y,js,cspath,cstype,csmaptype,OriginX,OriginY,resolution0);
	}
	else
	{
		SetBaseLayer(cmap,x,y,js,cspath,cstype,csmaptype,0,0,0);//Äþ²¨¹«½» ArcGis
	}
	
	returnInfo = ConstructJsonValue(root["CmdName"].asString(), root["Cmdid"].asInt(), -1, 0, 0, -1, -1);
}

void DrawEngine::RegistStyleByJS(const Json::Value& root, Json::Value& returnInfo)
{
	//if(m_init_)  //Ô¤ÏÈ×¢²áºÃ Êý×ÖÑùÊ½  ²âÊÔ´úÂë
	//{
	//	RegistStyle("C:\\Users\\xuyan\\Desktop\\GPSIcons\\0.png");
	//	RegistStyle("C:\\Users\\xuyan\\Desktop\\GPSIcons\\1.png");
	//	RegistStyle("C:\\Users\\xuyan\\Desktop\\GPSIcons\\2.png");
	//	RegistStyle("C:\\Users\\xuyan\\Desktop\\GPSIcons\\3.png");
	//	RegistStyle("C:\\Users\\xuyan\\Desktop\\GPSIcons\\4.png");
	//	RegistStyle("C:\\Users\\xuyan\\Desktop\\GPSIcons\\5.png");
	//	RegistStyle("C:\\Users\\xuyan\\Desktop\\GPSIcons\\6.png");
	//	RegistStyle("C:\\Users\\xuyan\\Desktop\\GPSIcons\\7.png");
	//	RegistStyle("C:\\Users\\xuyan\\Desktop\\GPSIcons\\8.png");
	//	RegistStyle("C:\\Users\\xuyan\\Desktop\\GPSIcons\\9.png");
	//	m_init_ = false;
	//}

	int style = 0;
	string style_path = root["PicURL"].asString();

	CMarkerStyle* markerstyle = new CMarkerStyle();
	if(style_path.find("http") == 0)  
	{
			//ÍøÂçÍ¼Æ¬URL
			CurlConnect* theInstance = CurlConnect::GetInstance();
			std::string theText = "";
			theInstance->GetDataFromByHttp(style_path, theText); //´ÓHTTPÖÐÄÃÊý¾Ý
			//±ê×¢Í¼Æ¬¾ÍÊÇÕâÀï³öÁËÎÊÌâ
			markerstyle->Init(theText);  //¼ÓÔØÍ¼Æ¬µ½ÄÚ´æ£¬²¢½âÂëÎªbitmap
	}
	else
	{
			//±¾µØÍ¼Æ¬URL
			CString cspath = UTF8ToGB2312(style_path.c_str()); //½«C·ç¸ñ×Ö·û´®
			markerstyle->m_ImageFileName = cspath;
			markerstyle->Init(); //½«´ÅÅÌÉÏµÄ Í¼Æ¬¶ÁÈëÄÚ´æ£¬²¢×ª»¯ÎªBYTEÊý×é
	}

	g_vecStyle.insert(map<int,CMarkerStyle*>::value_type(styleid,markerstyle)); //³õÊ¼ÖµÊÇ 0
	style = styleid;  //´ú±í¸ÃÑùÊ½µÄIDÖµ
	styleid++;
	//ÏÖÔÚ·µ»ØµÄÊÇÒÑ×¢²áµÄÑùÊ½ÖÐ IDÖµ×î´óµÄ
	returnInfo = ConstructJsonValue(root["CmdName"].asString(), root["Cmdid"].asInt(), -1, -1, 0, style, -1);  //ÖØÒªµÄ×Ö¶ÎÊÇ style Õâ¸öÑùÊ½ID value
	//ÔÚ·µ»ØÐÅÏ¢ÖÐ Ìí¼Óstyle_path
    returnInfo["PicURL"] = style_path;
}
//ÓÃÓÚ²âÊÔÊ¹ÓÃ
int DrawEngine::RegistStyle(const std::string &image_path )
{
	if(image_path.empty())
		return -1;
	int style = 0;
	CMarkerStyle* markerstyle = new CMarkerStyle();
	if(image_path.find("http") == 0)  
	{
			//ÍøÂçÍ¼Æ¬URL
			CurlConnect* theInstance = CurlConnect::GetInstance();
			std::string theText = "";
			theInstance->GetDataFromByHttp(image_path, theText); //´ÓHTTPÖÐÄÃÊý¾Ý
			markerstyle->Init(theText);  //¼ÓÔØÍ¼Æ¬µ½ÄÚ´æ£¬²¢½âÂëÎªbitmap
	}
	else
	{
			//±¾µØÍ¼Æ¬URL
			CString cspath = UTF8ToGB2312(image_path.c_str()); //½«C·ç¸ñ×Ö·û´®
			markerstyle->m_ImageFileName = cspath;
			markerstyle->Init(); //½«´ÅÅÌÉÏµÄ Í¼Æ¬¶ÁÈëÄÚ´æ£¬²¢×ª»¯ÎªBYTEÊý×é
	}

	g_vecStyle.insert(map<int,CMarkerStyle*>::value_type(styleid,markerstyle)); //³õÊ¼ÖµÊÇ 0
	style = styleid++;  //´ú±í¸ÃÑùÊ½µÄIDÖµ
	return style;
}

void DrawEngine::LoadWFSPolyLayerByJS(const Json::Value& root, Json::Value& returnInfo)
{
	string cmdName = root["CmdName"].asString();
	long layerHandle = -1; //Õâ¸öÍ¼²ãÔÚg_LayerManagerÖÐµÄIDºÅ£¬³õÊ¼ÎªÎÞÐ§Öµ
	if(0 == cmdName.compare("ADDWFSPOLYLAYER"))
	{
		CWFSLayer* theWFSLayer = new CWFSLayer(vWFSPolyLayer,this);		
		theWFSLayer->put_UrlPath(root["PolyFileURL"].asString());
		theWFSLayer->put_LayerName(root["LayerName"].asString());
		//if(theWFSLayer->GetData())
		boost::thread thrd(boost::bind(&CWFSLayer::GetData,theWFSLayer));
		{
			layerHandle = g_LayerManager.AddLayer(theWFSLayer);
			theWFSLayer->put_Style(root["Width"].asInt(), root["ColorR"].asInt(), root["ColorG"].asInt(), root["ColorB"].asInt(), root["ColorA"].asInt());
			returnInfo = ConstructJsonValue(root["CmdName"].asString(), root["Cmdid"].asInt(), -1, layerHandle, 0, -1, -1);
		}
		//else
			//delete theWFSLayer;
	}
}

void DrawEngine::AddHeatMapLayer(const Json::Value& root, Json::Value& returnInfo)
{
	HeatMapLayer *heatMapLayer = new HeatMapLayer(vHeatMapLayer,this);
	heatMapLayer->loadDataFromJSON(root);
	long layerID = -1;
	layerID = g_LayerManager.AddLayer(heatMapLayer);
	returnInfo = ConstructJsonValue(root["CmdName"].asString(), root["Cmdid"].asInt(), -1, layerID, 0, -1, -1);
}

void DrawEngine::AddClusterLayer(const Json::Value& root, Json::Value& returnInfo)
{
	ClusterLayer *clusterLayer = new ClusterLayer(vClusterLayer, this);
	clusterLayer->loadDateFromJson(root);
	long layerID = -1;
	layerID = g_LayerManager.AddLayer(clusterLayer);
	returnInfo = ConstructJsonValue(root["CmdName"].asString(), root["Cmdid"].asInt(), -1, layerID, 0, -1, -1);
}

void DrawEngine::LoadWFSLineLayerByJS(const Json::Value& root, Json::Value& returnInfo)
{
	string cmdName = root["CmdName"].asString();
	long layerHandle = -1; //Õâ¸öÍ¼²ãÔÚg_LayerManagerÖÐµÄIDºÅ£¬³õÊ¼ÎªÎÞÐ§Öµ
	if(0 == cmdName.compare("ADDWFSLINELAYER"))
	{
		CWFSLayer* theWFSLayer = new CWFSLayer(vElementLayer,this);	//ÀàÐÍÔÝÊ±ÏÈÅªÎªÕâ¸ö£¬Èç¹û¿¼ÂÇÊôÐÔ ¿ÉÒÔÌæ»»¸öÊôÐÔÀàÐÍ vWFSLineLayer
		theWFSLayer->put_UrlPath(root["LineFileURL"].asString());
		theWFSLayer->put_LayerName(root["LayerName"].asString());
		//if(theWFSLayer->GetData())//µ½WFS·þÎñÉÏÈ¥È¡ µã»òÏßµÄ Î»ÖÃÐÅÏ¢, Õâ¸ö¹ý³Ì±È½Ï[ºÄÊ±]
		boost::thread thrd(boost::bind(&CWFSLayer::GetData,theWFSLayer));
		{
			layerHandle = g_LayerManager.AddLayer(theWFSLayer); //Ìí¼Óµ½Í¼²ãÈÝÆ÷
			int width = root["Width"].asInt();  //ÕâÊÇwidthÊÇ µØÀí¿í¶È
			if(root["levelWidth"] != Json::nullValue)
			{
				theWFSLayer->m_LineElementSet_.m_cast = false; //´«ÈëµÄÊÇpixel¿í¶È£¬²»ÐèÒª×ª»»
				int widthSize = root["levelWidth"].size();
				Json::Value width_arry = root["levelWidth"];
				for(int i = 0;i< widthSize;i++)  //±£´æ¸÷¸öµØÍ¼ÏÔÊ¾LevelÊ±£¬»­ÏßÊ±µÄ¿í¶È
					theWFSLayer->m_LineWidth.push_back(width_arry[i].asInt());
				
				int currLevel = ((CBaseLayer *)g_LayerManager.m_BaseLayer)->GetCurrLevel();
				int offset = currLevel - (this->m_level);
				if( offset >= widthSize)  //Ô½½çºó£¬²»ÔÙ·Å´ó
					offset = widthSize - 1;
				else if(offset <= 0)
					offset = 0;
				width = theWFSLayer->m_LineWidth[offset];
			}

			theWFSLayer->put_Style(width, root["ColorR"].asInt(), root["ColorG"].asInt(), root["ColorB"].asInt(), root["ColorA"].asInt());
			theWFSLayer->m_bVisible = true; 
			returnInfo = ConstructJsonValue(root["CmdName"].asString(), root["Cmdid"].asInt(), -1, layerHandle, 0, -1, -1);  //¸ÃÔÚMessaging_HandleÖÐ·µ»Ø
		}
		//else
			//delete theWFSLayer;
	}
	else if(0 == cmdName.compare("UPDATEWFSLINELAYER")) //¸üÐÂ µã»òÏßµÄ ÐÅÏ¢,Éæ¼°ÖØÐÂÈ¥·þÎñÈ¡Êý¾Ý
	{
		//while(1); ÄÚ´æ·ÅÖÃÔÚ
		CLayer *layerObject = NULL;
		//g_LayerManager.GetLayerByName(root["LayerName"].asString(),&layerObject,layerHandle);
		//ÐÞ¸ÄÁË ÔÚÕâÀïÎÒÃÇÍ¨¹ýLayerIdÀ´È¡Í¼²ã
		layerHandle = root["LayerId"].asInt();
		layerObject = g_LayerManager.GetLayerByNumber(layerHandle); 
		if (layerObject)
		{
			HANDLE hUpdateLineThread=(HANDLE)_beginthreadex(NULL,NULL,&UpdateWFSLineMethod,(void *)layerObject,NULL,NULL);
			CloseHandle(hUpdateLineThread); //²¢Î´ÖÕÖ¹thread£¬Ö»ÊÇµÝ¼õ[kernel object]µÄÒýÓÃ
		}
	}
	
}

unsigned __stdcall DrawEngine:: UpdateWFSLineMethod(void *pM)
{

	::WaitForSingleObject(::g_hThreadLineEvent,INFINITE); //µÚÒ»´Î²»¹ÒÆð,µÈ´ýrend LineÊ¹ÓÃÁËÊý¾Ý
	CWFSLayer* wfslayer = (CWFSLayer*)pM;
	CWFSLayer::threadRemove = true;
	
	(wfslayer->m_LineElementSet_).m_thread = true; //±íÊ¾thread²Ù×÷
	//::WaitForSingleObject(::g_hAddLineEvent,INFINITE);
	wfslayer->TGetData(); //ÖØÐÂÈ¥ WFS·þÎñÖÐÈ¥È¡Êý¾Ý£¬¸Ãº¯ÊýÒ»¿ªÊ¼¾Í»áÇå¿ÕÈÝÆ÷ÖÐÔ­ÓÐµÄÊý¾Ý£¬
	//::WaitForSingleObject(::g_hRenderLineEvent,INFINITE); //µÈ´ý»æÖÆÍê³É

	wfslayer->m_LineElementSet_.m_update = true;  //ÓÃÓÚ»­ÏßÊ±ºò£¬ÊÇ·ñÔÚ´°¿Ú·¶Î§ÄÚµÄÓÅ»¯¼ÆËã
	
	//ÖØ½¨ query WFSLine ¼¯ºÏ£¬ÐÂµÄÊý¾ÝÖÐLineµÄFIDÖµÊÇ²»»á±ä»¯µÄ,ÖØÐÂµ½ÈÝÆ÷ÖÐÈ¡
	if(wfslayer->m_LineElementSet_.m_query_show)
	{
		auto iter = wfslayer->m_LineElementSet_.m_QueryDataSet_.begin();
	    auto end = wfslayer->m_LineElementSet_.m_QueryDataSet_.end();
	   for(;iter != end; ++iter)
		iter->second = wfslayer->m_LineElementSet_.m_DataSet_[iter->first]; //threadµÄÁíÒ»¸öÈÝÆ÷  	  
	}
	(wfslayer->m_LineElementSet_).m_thread_complete = true;
	
   return 0;
}

void DrawEngine::ShowWFSLineLayerByJS(const Json::Value& root, Json::Value& returnInfo)
{
	string cmdName = root["CmdName"].asString();
	long layerHandle = root["LayerHandle"].asInt();
	if(0 == cmdName.compare("SHOWWFSLINELAYER"))
	{
		CLayer *layerObject = NULL;
		layerObject = g_LayerManager.GetLayerByNumber(root["LayerHandle"].asInt()); 
		if (layerObject)
		{
			CWFSLayer* wfslayer = static_cast<CWFSLayer*>(layerObject);
/*			if(root["FID"].empty())
				wfslayer->m_bVisible = true; *///¿ØÖÆÕû¸öLineLayer
			wfslayer->m_LineElementSet_.m_query_show = false;
			wfslayer->m_LineElementSet_.m_query_hide = false;
			if(root["FID"] != Json::nullValue)
			{
			  wfslayer->m_LineElementSet_.m_query_show = true;
			  wfslayer->m_LineElementSet_.m_QueryDataSet_.clear(); //Çå¿ÕÉÏ´ÎÈÝÆ÷ÖÐµÄÖµ
			  int lineSize = root["FID"].size();
			  Json::Value line_array = root["FID"];
			  for(int i=0;i < lineSize;++i)  //×é×°ÒªÏÔÊ¾µÄLine
			  {
				  if(1 == wfslayer->m_LineElementSet_.m_DataSet_.count(line_array[i].asInt())) // ±£Ö¤Ç°¶Ë´«ÈëµÄfidÓÐÐ§,Ìî³äÐÂµÄÐèÒªÏÔÊ¾µÄLine
				  {
					  wfslayer->m_LineElementSet_.m_QueryDataSet_[line_array[i].asInt()] = wfslayer->m_LineElementSet_.m_DataSet_[line_array[i].asInt()];
				  }
			  }
			}
			else  //Èç¹ûLineÊÇ¿Õ
			{
				//WFSLine²»×öÈÎºÎ´¦Àí£¬WFSPointÊÇÎªÏìÓ¦Êó±êË«»÷ÊÂ¼þ²ÅÓÐÉèÖÃ¿É¼ûÐÔµÄ²Ù×÷
				//auto iter1 = wfslayer->m_LineElementSet_.m_DataSet_.begin();
				//auto iter2 = wfslayer->m_LineElementSet_.m_DataSet_.end();
				//while(iter1 != iter2)
				//{
				//	//iter1->second->m_visible = false;
				//	++iter1;
				//}
			}
			//CtrlRefresh(true);
			Draw();
		}
	}
	returnInfo = ConstructJsonValue(root["CmdName"].asString(), root["Cmdid"].asInt(), -1, layerHandle, 0, -1, -1);
}

void DrawEngine::HideWFSLineByJS(const Json::Value& root, Json::Value& returnInfo)
{
	long layerHandle = root["LayerHandle"].asInt();
	CLayer *layerObject = NULL;
	layerObject = g_LayerManager.GetLayerByNumber(root["LayerHandle"].asInt()); 
	if(layerObject)
	{
		CWFSLayer* wfslayer = static_cast<CWFSLayer*>(layerObject);
		//if(root["FID"].empty())
		//	wfslayer->m_bVisible = false; //¿ØÖÆÕû¸öLineLayer ²»ÏÔÊ¾
		if(root["FID"] != Json::nullValue)
		{
			wfslayer->m_bVisible = true;  //Ö»ÊÇÒþ²Ø²¿·ÖµÄLine
			wfslayer->m_LineElementSet_.m_query_hide = true;
			wfslayer->m_LineElementSet_.m_HideLineSet_.clear();
		    int lineSize = root["FID"].size();
			Json::Value line_array = root["FID"];
			for(int i=0;i < lineSize;++i)  //×é×°ÒªÏÔÊ¾µÄLine
			{
				wfslayer->m_LineElementSet_.m_HideLineSet_.insert(line_array[i].asInt());
			}
		}
		else
		{
			//auto iter1 = wfslayer->m_LineElementSet_.m_DataSet_.begin();
			//auto iter2 = wfslayer->m_LineElementSet_.m_DataSet_.end();
			//while(iter1 != iter2)
			//{
			//	//iter1->second->m_visible = true;
			//	++iter1;
			//}
		}
	}
}

void DrawEngine::ShowWFSPointByJS(const Json::Value& root, Json::Value& returnInfo)
{
	long layerHandle = root["LayerHandle"].asInt();
	CLayer *layerObject = NULL;
	layerObject = g_LayerManager.GetLayerByNumber(root["LayerHandle"].asInt()); 
	if(layerObject)
	{
		CWFSLayer* wfslayer = static_cast<CWFSLayer*>(layerObject);
		//if(root["FID"].empty())
		//	wfslayer->m_bVisible = true; //Õâ¸ö¹¦ÄÜ½Ó¿Ú²»ÔÚ ¿ØÖÆÍ¼²ãÏÔÊ¾
		wfslayer->m_PointElementSet_.m_query_show = false;
		wfslayer->m_PointElementSet_.m_query_hide = false;
		if( !root["FID"].empty())
		{
		  wfslayer->m_PointElementSet_.m_query_show = true;
		  wfslayer->m_PointElementSet_.m_QueryDataSet_.clear();
		  int pointSize = root["FID"].size();
		  Json::Value point_array = root["FID"];
		  for(int i = 0;i < pointSize; ++i)
		  {
			  if(1 == wfslayer->m_PointElementSet_.m_DataSet_.count(point_array[i].asInt()))
			  {
				  wfslayer->m_PointElementSet_.m_QueryDataSet_[point_array[i].asInt()] = wfslayer->m_PointElementSet_.m_DataSet_[point_array[i].asInt()];
				  wfslayer->m_PointElementSet_.m_QueryDataSet_[point_array[i].asInt()]->m_visible = true;
			  }
		  }
		  //ÉèÖÃÈÈµãµÄ¿É¼ûÐÔ ËùÓÐ²»ÔÚ ²éÑ¯·¶Î§ÄÚµÄFID ÉèÖÃÎªfalse
		    auto iter1 = wfslayer->m_PointElementSet_.m_DataSet_.begin();
			auto iter2 = wfslayer->m_PointElementSet_.m_DataSet_.end();
			while(iter1 != iter2)
			{
				if(0 == wfslayer->m_PointElementSet_.m_QueryDataSet_.count(iter1->first))
					iter1->second->m_visible = false;
			  ++iter1;
			}
		}
		else //Èç¹ûFID×Ö¶ÎÎª¿Õ£¬½«ËùÓÐµÄÈÈµãµÄ ¿É¼ûÐÔÖÃÎªfalse
		{
			//Èç¹û¿ÕÕû¸ö²ã¶¼ÏÔÊ¾ ²¢ÇÒ±éÀúËùÓÐµÄÈÈµã£¬½«m_visibleÖØÐÂÉèÖÃÎªTRUE
			//wfslayer->m_bVisible = true;
			auto iter1 = wfslayer->m_PointElementSet_.m_DataSet_.begin();
			auto iter2 = wfslayer->m_PointElementSet_.m_DataSet_.end();
			while(iter1 != iter2)
			{
				iter1->second->m_visible = false;
				++iter1;
			}
		}
	  Draw();
	}
   returnInfo = ConstructJsonValue(root["CmdName"].asString(), root["Cmdid"].asInt(), -1, layerHandle, 0, -1, -1);
}

void DrawEngine::HideWFSPointByJS(const Json::Value& root, Json::Value& returnInfo)
{
	long layerHandle = root["LayerHandle"].asInt();
	CLayer *layerObject = NULL;
	layerObject = g_LayerManager.GetLayerByNumber(root["LayerHandle"].asInt()); 
	if(layerObject)
	{
		CWFSLayer* wfslayer = static_cast<CWFSLayer*>(layerObject);
		//Èç¹û¿Õ£¬Òþ²ØÕû¸ö²ã
		//if(root["FID"].empty())
		//	wfslayer->m_bVisible = false; //¿ØÖÆÕû¸öLineLayer ²»ÏÔÊ¾
		if(!root["FID"].empty())
		{
			wfslayer->m_PointElementSet_.m_query_hide = true;
			wfslayer->m_PointElementSet_.m_HidePointSet_.clear();
		    int pointSize = root["FID"].size();
			Json::Value point_array = root["FID"];
			for(int i=0;i < pointSize;++i)  //×é×°ÒªÏÔÊ¾µÄLine
			{
				if(wfslayer->m_PointElementSet_.m_DataSet_.count(point_array[i].asInt()))
					wfslayer->m_PointElementSet_.m_DataSet_[point_array[i].asInt()]->m_visible = false;
				wfslayer->m_PointElementSet_.m_HidePointSet_.insert(point_array[i].asInt());
			}
		}
		else  //Èç¹ûÊý×éÎª¿Õ£¬½«Õû¸öÈÈµãµÄ ¿É¼ûÐÔÉèÖÃÎª true
		{
			auto iter1 = wfslayer->m_PointElementSet_.m_DataSet_.begin();
			auto iter2 = wfslayer->m_PointElementSet_.m_DataSet_.end();
			while(iter1 != iter2)
			{
				iter1->second->m_visible = true;
				++iter1;
			}
		}
	}
}
// ¼ÓÔØÈÈµã²ã
void DrawEngine::LoadTheWFSLayerByJS(const Json::Value& root, Json::Value& returnInfo)
{
	//ADDLINELAYER,ÏßÊ¸Á¿ÎÄ¼þµÄÂ·¾¶,ÏßÑùÊ½µÄ¾ä±ú,Ö¸ÁîµÄÐòºÅ
	if (g_vecStyle.size()!=0) // ÔÚÒ³ÃæÉÏµã»÷¡°×¢²áµãÑùÊ½¡±ºó£¬¸ÃÈÝÆ÷µÄÖµ¾Í²»ÊÇ 0 ÁË
	{
		//"CmdName":"DeegreeWFS","MapName":"hzxq","URL":
		string URL = root["PointFileURL"].asString();
		//CString filename = UTF8ToGB2312(URL.c_str());
        //ÒÀ¾Ýjs·¢À´µÄÑùÊ½ID£¬À´È¡style£¬ÕâÀïÎÒÃÇÎªÁË²âÊÔ£¬ËùÒÔÐ´ËÀÁËÎª0£¬Êµ¼ÊÖÐÕâ¸öÖµ£¬ÊÇ×¢²áÑùÊ½ÁËºóÓÉJSÈ¡µÃ
		int style = root["Markerid"].asInt(); 
		string mapname = root["LayerName"].asString(); //LayerµÄÃû×Ö
		int theWidth = root["Width"].asInt();
		int theHeight = root["Height"].asInt();
		string cmdName = root["CmdName"].asString();
		string layername = root["LayerName"].asString();

		//Json::Value point_arry = root["FID"];  È¡Î´»ñµÃµÄÖµ ÊÇNULL

		//AddWFSpointLayer
		LONG layerhandle = -1;
		if (cmdName.compare("ADDWFSPOINTLAYER") == 0)
		{
			CWFSLayer* wfsLayer = new CWFSLayer(vWFSPointLayer,this);  //ÒÀ¾ÝWFSÖÐ²»Í¬²ãµÄ ÀàÐÍÀ´new CWFSLayerµÄÊµÀý
			wfsLayer->put_LayerName(mapname);
			wfsLayer->put_Rect(theWidth, theHeight); //ÈÈµãÏÔÊ¾µÄ¼¸ºÎ[¿í¶ÈÓë¸ß¶È]
			wfsLayer->put_UrlPath(URL);
			//style ÓÉjs ·¢À´µÄcmd¸ø³ö£¬ÕâÑùÑùÊ½ID¾ÍÊÇÐ´ËÀÁË£¬ÄÇÃ´ÎÒÃÇ²Ù×÷×¢²áÑùÊ½Ê±£¬[×¢²áµãÑùÊ½] ±ØÐëµÚÒ»¸ö×¢²á
			CMarkerStyle* theStyle = (CMarkerStyle*)(g_vecStyle.find(style)->second); //ÏòÏÂµÄ Ç¿ÖÆÀàÐÍ×ª»»  xy
			wfsLayer->put_Style(theStyle); //³õÊ¼»¯£¬ÈÈµã»æÖÆµÄ Ä¬ÈÏstyle
			//ÔÚ½âÎöµãµÄÎ»ÖÃÐÅÏ¢Ê±£¬»áÉèÖÃµãÏÔÊ¾µÄ¿í¶ÈÓë¸ß¶ÈÓÃÉÏÃæµÄtheWidth,theHeight
			//wfsLayer->GetData();  //µ÷ÓÃ²»Í¬µÄ ·½·¨ À´½âÎöÍøÂç·þÎñ´«À´µÄÎ»ÖÃÐÅÏ¢µÄ XMLÎÄ¼þ
			boost::thread thrd(boost::bind(&CWFSLayer::GetData,wfsLayer));
			layerhandle = g_LayerManager.AddLayer(wfsLayer);
		  
		}
		else if (cmdName.compare("UPDATEWFSPOINTLAYER") == 0)
		{
			//int layerid= root["LayerID"].asInt();  js·¢À´µÄcmd×Ö·û´®ÖÐÃ»ÓÐ LayerIDÕâ¸ö×Ö¶Î£¬¶ørootÓÖÊÇÊ¹ÓÃµÄmap ¹ÊÕâÀïÊÇ0
			//int layerid = 1;  Õâ²ÅÊÇÕýÈ·µÄ
			CLayer *layerObject = NULL;
			//long layerid = -1; //±íÊ¾ÎÞÐ§Öµ
			//g_LayerManager.GetLayerByName(mapname,&layerObject,layerid); ÏÖÔÚÍ³Ò»¸ü¸ÄÊ¹ÓÃLayerIDÀ´È¡Öµ
			long layerid = root["LayerId"].asInt();
			layerObject = g_LayerManager.GetLayerByNumber(layerid); 
		   // CLayer* layerObject = g_LayerManager.GetLayer(layerid); ²»ÄÜ¿¿IDÀ´È¡Í¼²ã
			if (layerObject)
			{
				CWFSLayer* wfslayer = (CWFSLayer*)layerObject;
				wfslayer->GetData();  //»á½« Ö®Ç°ÈÝÆ÷ÖÐµÄÖµ Çå³ý
				layerhandle = layerid;
				//ÖØ½¨ query WFSPoint ¼¯ºÏ
				if(wfslayer->m_PointElementSet_.m_query_show)
				{
					auto iter = wfslayer->m_PointElementSet_.m_QueryDataSet_.begin();
					auto end = wfslayer->m_PointElementSet_.m_QueryDataSet_.end();
					//for_each(iter,end,[](pair<const long,CWFSLineFeature*> &currLine ){ currLine.second = wfslayer->m_LineElementSet_.m_DataSet_[currLine.first];});
				   for(;iter != end; ++iter)
					   iter->second = wfslayer->m_PointElementSet_.m_DataSet_[iter->first];
				   //²¢½«ÆäËûµãµÄ m_visibleÉèÖÃÎªfalse
				    auto iter1 = wfslayer->m_PointElementSet_.m_DataSet_.begin();
					auto iter2 = wfslayer->m_PointElementSet_.m_DataSet_.end();
					while(iter1 != iter2)
					{
						if(0 == wfslayer->m_PointElementSet_.m_QueryDataSet_.count(iter1->first))
							iter1->second->m_visible = false;
					  ++iter1;
					}
				}
			}
		}
		else if(cmdName.compare("MODIFYWFSPOINTLAYER") == 0)  // ÐÞ¸ÄÈÈµãµÄÑùÊ½Í¼±ê
		{
		  CLayer *layerObject = NULL;
		  //´ÓrootÖÐµÃµ½ ²ãID
		  int layerID = root["LayerHandle"].asInt();
		  //g_LayerManager.GetLayerByID
		  layerObject = g_LayerManager.GetLayer(layerID);
		  assert(layerObject);
		  CWFSLayer *temp = static_cast<CWFSLayer *>(layerObject);
		
		  //½«CWFSLayer::m_PointElementSet_.m_ModifyStyle_ = true;
		  temp->m_PointElementSet_.m_ModifyStyle_ = true;
		  //´ÓrootÖÐ µÃµ½ ModifyStyleµÄID
		  int styleID = root["ModifyStyleID"].asInt();
		  CMarkerStyle* theStyle = (CMarkerStyle*)(g_vecStyle.find(styleID)->second);
		  assert(theStyle);
		  temp->m_PointElementSet_.m_Style_.insert(styleID); //½«Ê¹ÓÃ¹ýµÄstyleID·ÅÖÃÔÚ setÖÐ

		  //±éÀú FID[] ,ÐÞ¸Ä m_PointElementSet_[i]µÄm_markerstyle
		  int pointSize = root["FID"].size();
		  Json::Value point_arry = root["FID"];  //µÃµ½ÈÈµãÊý×é
		  for(int i=0;i<pointSize;++i)
		  {
			  int fid = point_arry[i].asInt();
			  if(1 == (temp->m_PointElementSet_).m_DataSet_.count(fid)) //±£Ö¤Ç°¶Ë·¢À´ÊÇÓÐÐ§µÄÈÈµã fid
			  {
			  	  WFSPointFeatureElement *curPoint = ((temp->m_PointElementSet_).m_DataSet_)[fid];
				  curPoint->m_markerstyle = theStyle; //Ê¹ÓÃÐÞ¸Äºó ÐÂµÄÑùÊ½
				  typedef std::multimap<int,long>::iterator fids_it;
				  std::pair<fids_it,fids_it> range= temp->m_PointElementSet_.m_ModifySet_.equal_range(styleID);
				 //±éÀúÊ¹ÓÃÍ¬ÖÖstyleµÄFID,²é¿´´ËFIDÊÇ·ñÒÑ¾­²åÈë¹ý
				  bool binsert = true;
				  while(range.first != range.second) 
				  {
					  if (range.first->second == fid)
					  {
						  binsert = false;
						  break;
					  }

					  range.first++;
				  }
				  if(binsert)
				  {
					  temp->m_PointElementSet_.m_ModifySet_.insert(make_pair(styleID,fid));
				  }

			  }
		  }
		} //ÐÞ¸ÄÈÈµã´¦ÀíÍê±Ï
		returnInfo = ConstructJsonValueforWFS(root["CmdName"].asString(), root["Cmdid"].asInt(), -1, layerhandle,root["LayerName"].asString(), 0, style, -1);
	}
}

void DrawEngine::MoveTheWFSLayerByJS(const Json::Value& root, Json::Value& returnInfo)
{
	glSetCurrentContextPPAPI(g_context); //ÒÆ¶¯µØÍ¼£¬¿ÉÄÜ·¢ÉúÉ¾³ýÎÆÀíµÄ²Ù×÷£¬¼´»áÈ±¿é£¬¼ÓÉÏÕâ¾ä¾Í²»»áÁË
	CLayer *layerObject = NULL;
	//´ÓrootÖÐµÃµ½ ²ãID
	int layerID = root["LayerHandle"].asInt();
	layerObject = g_LayerManager.GetLayer(layerID);
	//assert(layerObject);
	if(layerObject == NULL)
		return;
	CWFSLayer *temp = static_cast<CWFSLayer *>(layerObject);
	if( root["FID"].empty())
		return;
	int pointSize = root["FID"].size();
	Json::Value point_array = root["FID"];  //µÃµ½ÈÈµãÊý×é

	Json::Value position_X = root["X"]; 
	Json::Value position_Y = root["Y"];

	if( point_array.empty() || position_X.empty() || position_Y.empty())
		return;
	bool is_change_position = false;
	for(int i=0;i<pointSize;++i)
	{
		int fid = point_array[i].asInt();  //Ç°¶ËÖ¸Áî"FID":["1233"] ÕâÀï²»ÄÜÓÐË«ÒýºÅ,ÒòÎªÊÇ°´asIntÀ´½âÎö£¬·¢ËÍintÊÇ²»´øË«ÒýºÅ
		if(1 == (temp->m_PointElementSet_).m_DataSet_.count(fid)) //±£Ö¤Ç°¶Ë·¢À´ÊÇÓÐÐ§µÄÈÈµã fid
		{
			WFSPointFeatureElement *curPoint = ((temp->m_PointElementSet_).m_DataSet_)[fid];
			
			if( i < position_X.size())
			{
				curPoint->x_ = position_X[i].asDouble();
				curPoint->y_ = position_Y[i].asDouble();
				is_change_position = true;
			}
		
		}
	}

	if(temp->m_bVisible &&is_change_position) //¸Ä±ä¹ýµãµÄ Î»ÖÃ²ÅÖØ»æ,Í¼²ã¿É¼û
		Draw();
}

//add Óë remove WFSPoint style
void DrawEngine::AddStyleToWFSPointByJS(const Json::Value& root, Json::Value& returnInfo)
{
	CLayer *layerObject = NULL;
	//´ÓrootÖÐµÃµ½ ²ãID
	int layerID = root["LayerHandle"].asInt();
	//g_LayerManager.GetLayerByID
	layerObject = g_LayerManager.GetLayer(layerID);
	//assert(layerObject);
	if(layerObject == NULL)
		return;
	CWFSLayer *temp = static_cast<CWFSLayer *>(layerObject);
	temp->m_PointElementSet_.m_addStyle = true; //¿ØÖÆWFSPointµÄ»æÖÆÂß¼­
	Json::Value arr = root["AddStyleArray"];
	int pointSize = arr.size();
	for(int i=0;i<pointSize;++i)
	{
		int fid = arr[i]["FID"].asInt();
		int posId = arr[i]["PosID"].asInt();
		StylePointID fpId = {fid,posId};  //×Ô¶¨ÒåÀàÐÍ£¬Ã¿Ò»¸öÑùÊ½¶¼ÊÇ ²»Í¬µÄ
		int styleId = arr[i]["StyleID"].asInt();
		Json::Value style_pos = arr[i]["StylePos"];
//ÌáÈ¡ÑùÊ½µÄÎ»ÖÃ×ø±ê ÐÅÏ¢
		int stylepos[4];
		int size = style_pos.size();
		for(int j=0;j<size;++j)
			stylepos[j] = style_pos[j].asInt();

		if((temp->m_PointElementSet_).m_DataSet_.count(fid))
		{
			WFSPointFeatureElement *curPoint = ((temp->m_PointElementSet_).m_DataSet_)[fid];
//ÒÔµ¥¸öPonitÎªµ¥Î»£¬¿ØÖÆÆäÔÚ¸÷¸ö²ã´ÎµÄ ÊÇ·ñÏÔÊ¾
			Json::Value levelShow_flagArr = arr[i]["level"];
			if(!levelShow_flagArr.empty())
			{
				curPoint->m_show_flagArr[fpId].clear(); //ÏÈÇå¿ÕÔ­ÓÐµÄ
				for(int i = 0;i<levelShow_flagArr.size();++i)
				{
					curPoint->m_show_flagArr[fpId].push_back(levelShow_flagArr[i].asInt());
				}
			}
		 //ÏÈÔÚCWFSPointFeatureElementSetÉÏ²Ù×÷
			temp->m_PointElementSet_.m_append_style.insert(styleId); //±£´æWFSPointÊ¹ÓÃ¹ýµÄstyle,±£Ö¤ÁËÎ¨Ò»ÐÔ
		 //±ÜÃâ½«[styleId,fid]¶à¸öÍêÈ«ÏàÍ¬µÄ²åÈëµ½multimap	
			auto range = temp->m_PointElementSet_.m_styleID_to_Fid.equal_range(styleId);
			bool b_exist = false; //false ±íÊ¾²»´æÔÚ
			while(range.first != range.second)
			{
				if(range.first->second == fpId) //ÖØÔØµÄ²Ù×÷·û
				{
				  b_exist = true;
				  break;
				}
				++range.first;
			}
			if(range.first == range.second || !b_exist)  //²»´æÔÚ ²Å²åÈë
			{
				//C++11 ÀàÐÍÍÆ¶Ï
				//typedef std::multimap<int,StylePointID>::iterator iter;
				//multimap insert¾ÍÊÇ·µ»Ø iteratorÕâÊÇÓë map²åÈë ·µ»Øpair²»Í¬µÄµØ·½
				auto pos = (temp->m_PointElementSet_).m_styleID_to_Fid.insert(make_pair(styleId,fpId));
				//Ô­À´ÕâÀïÐ´´íÁË Ö±½Ó²åÈëµÄÊÇpos
				curPoint->m_styleID_to_iter.insert(make_pair(fpId,pos));
			}

		 //È»ºóÔÚWFSPointÖÐµ¥¶ÀÉèÖÃ styleµÄÏñËØ×ø±ê Î»ÖÃ
			curPoint->SetAppendStylePos(fpId,stylepos[0],stylepos[1],stylepos[2],stylepos[3]);
		}
			
	}
}

void DrawEngine::RemoveStyleFromWFSPointByJS(const Json::Value& root, Json::Value& returnInfo)
{
	CLayer *layerObject = NULL;
	//´ÓrootÖÐµÃµ½ ²ãID
	int layerID = root["LayerHandle"].asInt();
	//g_LayerManager.GetLayerByID
	layerObject = g_LayerManager.GetLayer(layerID);
	//assert(layerObject);
	if(layerObject == NULL)
		return;
	CWFSLayer *temp = static_cast<CWFSLayer *>(layerObject);
	Json::Value arr = root["DeleteStyleArray"];
	int pointSize = arr.size();
	for(int i=0;i<pointSize;++i)
	{
		int fid = arr[i]["FID"].asInt();
		int posId = arr[i]["PosID"].asInt();
		StylePointID fpId = {fid,posId};  //¹¹½¨×Ô¶¨ÒåÀàÐÍ

		int styleId = arr[i]["StyleID"].asInt(); //ÑùÊ½ID
		if((temp->m_PointElementSet_).m_DataSet_.count(fid)) //±£Ö¤WFSPointÓÐÐ§
		{
		  //ÏÈÔÚCWFSPointFeatureElementSetÉÏ²Ù×÷ delete
		     //1.ÕÒµ½ÔÚpointÔÚÊý¾Ý¼¯ÖÐµÄÎ»ÖÃ
			WFSPointFeatureElement *curPoint = ((temp->m_PointElementSet_).m_DataSet_)[fid];
			if(curPoint->m_styleID_to_iter.count(fpId))
			{
			   auto pos = curPoint->m_styleID_to_iter[fpId]; //×¢ÒâÕâ¸öposµÄÀàÐÍÊÇ multimap<int,StylePointID>::iterator
		       //2.ÔÚsetÉÏÉ¾³ý  
			   temp->m_PointElementSet_.m_styleID_to_Fid.erase(pos);
			}
			if(temp->m_PointElementSet_.m_styleID_to_Fid.empty())
			{
				 temp->m_PointElementSet_.m_addStyle = false;
				 temp->m_PointElementSet_.m_append_style.clear(); //Çå¿ÕÊ¹ÓÃ¹ýµÄstyleID setÈÝÆ÷
			}
		 //ÔÚµãÉÏÉ¾³ý ÕâÐ©ÐÅÏ¢
			curPoint->RemoveStyle(fpId);
		}
	}
}

void DrawEngine::ModifyStyleWFSPointByJS(const Json::Value& root, Json::Value& returnInfo)
{
	CLayer *layerObject = NULL;
	//´ÓrootÖÐµÃµ½ ²ãID
	int layerID = root["LayerHandle"].asInt();
	//g_LayerManager.GetLayerByID
	layerObject = g_LayerManager.GetLayer(layerID);
	//assert(layerObject);
	if(layerObject == NULL)
		return;
	CWFSLayer *temp = static_cast<CWFSLayer *>(layerObject);
	Json::Value arr = root["ModifyStyleArray"];
	int pointSize = arr.size();
	for(int i=0;i<pointSize;++i)
	{
		int fid = arr[i]["FID"].asInt();
		int posId = arr[i]["PosID"].asInt();
		StylePointID fpId = {fid,posId};  //¹¹½¨×Ô¶¨ÒåÀàÐÍ
		int styleId = arr[i]["StyleID"].asInt(); //ÑùÊ½ID
		if((temp->m_PointElementSet_).m_DataSet_.count(fid)) //±£Ö¤WFSPointÓÐÐ§
		{
			//1.ÕÒµ½pointÔÚÊý¾ÝsetÖÐµÄÎ»ÖÃ
			WFSPointFeatureElement *curPoint = ((temp->m_PointElementSet_).m_DataSet_)[fid];
			if(curPoint->m_styleID_to_iter.count(fpId))
			{
				//ÏÈÔÚ m_styleID_to_FidÖÐÉ¾³ý
				(temp->m_PointElementSet_).m_styleID_to_Fid.erase((curPoint->m_styleID_to_iter)[fpId]);
			    //ÔÙ²åÈëÐÂµÄ ÔÚmultimapÉÏinsertÊÇÖ±½Ó·µ»Ø iterator
			    auto pos = (temp->m_PointElementSet_).m_styleID_to_Fid.insert(make_pair(styleId,fpId));
				//ÔÚ ÈÈµãÉÏ ±£´æÐÂµÄ Î»ÖÃ
				curPoint->m_styleID_to_iter[fpId] = pos;
			}
		}
	}

}

void DrawEngine::ChangeOperatorByJS(const Json::Value& root, Json::Value& returnInfo)
{
	int thelayerhandle = -1;
	std::string theOpeator = root["OPERATOR"].asString();
	m_bRectShow = false;
	m_bPolyShow = false;
	m_IsPoint = false; //½«ÀëÉ¢ÖØÖÃ,×ÔÓÉ»æ±ÊµÄÁíÒ»ÖÖÄ£Ê½
	if(root["Status"] !=Json::nullValue) //Çø·ÖÊÇ´Ó¹¤¾ßÀ¸ÉÏ£¬»¹ÊÇDIVµÄÖÐ
	{
		Status = root["Status"].asInt();
		if(1 == Status)
		{
			m_bRectShow = true;
			m_bPolyShow = true;
		}
	}
	else
		Status = 0;
	if(theOpeator.compare("CHOOSE") == 0)   //¿òÑ¡ ÕâÀïÎÒ½«ÐÞ¸ÄÎª[ÇÐÆ¬]
	{
		m_ChooseType = vNULL;
		g_ChooseOperator = enumMapOperator::vChooseOperator;
		m_bCreateQiePian = true; //±êÊ¶ÊÇ·ñ´´½¨ÐÂµÄ ÇÐÆ¬ÊµÀý
	}
	else if(theOpeator.compare("PAN") == 0)  //µØÍ¼Æ½ÒÆ²Ù×÷
	{
		g_ChooseOperator = enumMapOperator::vPanOperator;
	}
	else if(theOpeator == "AddMark")
	{
		g_ChooseOperator = enumMapOperator::vMark;
		b_Lable = TRUE;
		markstyle = root["StyleID"].asInt();
		m_rendW = root["Width"].asInt();
		m_rendH = root["Height"].asInt();
	}
	else if(theOpeator.compare("DRAW") == 0)
	{
		g_ChooseOperator = enumMapOperator::vDrawOperator; //Ö÷´¦Àíº¯ÊýÖÐµÄ[×ÔÓÉ»æ±Ê]
		
		int layerid = -1;
		CElementLayer *penLineLayer = NULL; //ÓÉÍ¼²ãÈÝÆ÷£¬¸ºÔð»ØÊÕ¸Ã×ÊÔ´£¬ÔÚC versionÖÐ£¬¸ÃÍ¼²ãÊÇÓÉÈ«¾Ö±äÁ¿³ÖÓÐ
		CLayer *tmp = NULL;
		tmp = g_LayerManager.SearchByLayerType(VLayerType::vTempLayer,&layerid);

		if(tmp)
		{
			penLineLayer = static_cast<CElementLayer *>(tmp);
		}
		else //ÖØÐÂ´´½¨ÐÂµÄÍ¼²ã
		{
			penLineLayer = new CElementLayer(this); //¶àÌí¼ÓÒ»¸ö this²ÎÊý
			penLineLayer->m_layerType = VLayerType::vTempLayer;  //ÁÙÊ±»æÏß²ã£¬
			thelayerhandle = g_LayerManager.AddLayer(penLineLayer);  //½ÓÏÂÀ´ ¶ÔÕâ¸ö²ãµÄ²Ù×÷ÊÇ Í¨¹ýÏìÓ¦Êó±êÊÂ¼þÀ´Íê³ÉµÄ£¬·µ»ØÖµÊÇÍ¼²ãIDºÅ
		}
		//»ñÈ¡µ½ÏßÊôÐÔ£¬×¢ÏßelementµÄ´´½¨ÊÇÔÚÊó±êµã»÷Ê±´´½¨£¬ÕâÀïµÄÏßÊôÐÔ ÊÇÈ«¾Ö±äÁ¿
		lineW = root["Width"].asInt();
		lineR = root["ColorR"].asInt();
		lineG = root["ColorG"].asInt();
		lineB = root["ColorB"].asInt();
		lineA = root["ColorA"].asInt();
		Json::Value lineModel = root["ISPOINT"];
		if(1 == lineModel.asInt())
			m_IsPoint = true;
		
	}
	else if (theOpeator.compare("CANCLE") == 0) //È¡Ïû×ÔÓÉ»æ±Ê removeÕâ¸öÍ¼²ã
	{
		CElementLayer *penLineLayer = NULL;
		int layerid = -1;
		int layerCount = g_LayerManager.GetLayerCount(); //×ÜÍ¼²ãÊý
		for(int i = 0 ;i < layerCount; ++i)
		{
			CLayer *layer = g_LayerManager.GetLayerByNumber(i+1);
			//¿¼ÂÇµ½ÓÐµÄÍ¼²ãÒÑ¾­É¾³ý£¬¶ø²éÕÒÊÇ forµÄÏßÐÔÑ­»· £¬ËùÒÔ²ÅÓÐÁË ÕâÀï ifÅÐ¶Ï
			if (layer == NULL)
			{
				continue;
			}
			if(VLayerType::vTempLayer == layer->m_layerType)
			{
				penLineLayer = static_cast<CElementLayer *>(layer);
				layerid = i + 1;
			}
		}
		if (penLineLayer!=NULL)  //Èç¹ûÕâ¸ö ×ÔÓÉ»æ±Ê²ã ´æÔÚ
		{
			g_LayerManager.RemoveLayer(layerid);
			Draw();
		}
	}
	else if (theOpeator.compare("CLEAR") == 0) //Çå¿Õ×ÔÓÉ»æ±Ê ½«¸Ã²ãÖÐµÄÔªËØ
	{
		CElementLayer *penLineLayer = NULL;
		int layerCount = g_LayerManager.GetLayerCount(); //×ÜÍ¼²ãÊý
		for(int i = 0 ;i < layerCount; ++i)
		{
			CLayer *layer = g_LayerManager.GetLayerByNumber(i+1);
			//¿¼ÂÇµ½ÓÐµÄÍ¼²ãÒÑ¾­É¾³ý£¬¶ø²éÕÒÊÇ forµÄÏßÐÔÑ­»· £¬ËùÒÔ²ÅÓÐÁË ÕâÀï ifÅÐ¶Ï
			if (layer == NULL)
			{
				continue;
			}
			if(VLayerType::vTempLayer == layer->m_layerType)
			{
				penLineLayer = static_cast<CElementLayer *>(layer);
			}
		}
		if (penLineLayer!=NULL)
		{		
			penLineLayer->DeleteAllElement();
			Draw();
		}
	}
	else if (theOpeator.compare("CLEARCHOOSEMARKER") == 0)
	{

	}
	//¾ØÐÎÑ¡ÔñÇøÓò
	else if(theOpeator == "CHOOSERECT")
	{
		m_ChooseType = vRectChoose;
		g_ChooseOperator = enumMapOperator::vChooseOperator;
	}
	else if(theOpeator.compare("POLYGONCHOOSE") == 0)  //¶à±ßÐÎÑ¡²Ù×÷
	{
		m_ChooseType = vRectChoose; //±ãÓÚ¹¹Ôì»Øµ÷ÐÅÏ¢
		g_ChooseOperator = enumMapOperator::vPolygonChoose;		//ÃæÑ¡²Ù×÷£¬´¦Àíº¯ÊýÖÐµÄ
		g_PolygonChoosePointList.clear();						//Çå³ý²Ù×÷
		g_PologynPointSet.clear();
		g_LineChooseRect = root["LINEWIDTH"].asInt();
	}
	else if(theOpeator.compare("LINECHOOSE") == 0)  // ÏßÑ¡²Ù×÷  £¬ÔÚÊó±êÓÒ»÷ºó »áÓÐPostMessage ÏòÇ°¶ËÍ¶µÝÏûÏ¢
	{
		m_ChooseType = vRectChoose;
		g_ChooseOperator = enumMapOperator::vLineChoose;
		g_PolygonChoosePointList.clear(); //±£´æµÄµã(¾­Î³¶È) ÓÃÓÚ»æÖÆ
		g_PologynPointSet.clear(); //±£´æµã pixel×ø±ê£¬ÓÃÓÚ¹¹ÔìÇøÓò£¬Ñ¡ÔñÈÈµã
		g_LineChooseRect = root["LINEWIDTH"].asInt(); //ÏßÑ¡Ê±£¬ÏßµÄwidth
	}
	else if(theOpeator.compare("CIRCLECHOOSE") == 0)   // Ô²È¦Ñ¡Ôñ
	{
		m_ChooseType = vRectChoose;
		g_ChooseOperator = enumMapOperator::vCircleChoose;		
		if(g_ChooseCircleElement != NULL)
			delete g_ChooseCircleElement; 
		g_ChooseCircleElement = new CCircleElement(this); 
		g_ChooseCircleElement->m_Rinner = 0; //³õÊ¼ÄÚ°ë¾¶Îª0
		g_ChooseCircleElement->m_a = root["ColorA"].asInt();
		g_ChooseCircleElement->m_g = root["ColorG"].asInt();
		g_ChooseCircleElement->m_r = root["ColorR"].asInt();
		g_ChooseCircleElement->m_b = root["ColorB"].asInt();
	}
	returnInfo = ConstructJsonValue(root["CmdName"].asString(), root["Cmdid"].asInt(), -1, thelayerhandle, Status, -1, -1); //¹¹Ôì»Øµ÷ÐÅÏ¢
}


void DrawEngine::CommandParsing(const std::string& CmdName, const Json::Value& root, Json::Value& returnInfo) //´¦Àí
{
	Activate(root,returnInfo); //¹ØÁªµ±Ç° ÃèÊö¾ä±ú
	std::map<std::string, HandleFun>::iterator theFunction = m_Function.find(CmdName);
	if(theFunction != m_Function.end())
		(this->*(theFunction->second))(root, returnInfo);
}

void DrawEngine::HideLayerByJS(const Json::Value& root, Json::Value& returnInfo)
{
	int layerid = root["Layerid"].asInt();

	CLayer* layer = g_LayerManager.GetLayer(layerid);
	if(layer)
	{
		layer->m_bVisible = false;
	}
	Draw(); // Ë¢ÐÂÒ»´Î£¬²úÉúÒþ²ØµÄÐ§¹û
	returnInfo = ConstructJsonValue(root["CmdName"].asString(), root["Cmdid"].asInt(), -1, layerid, 0, -1, -1);
}

void DrawEngine::ShowLayerByJS(const Json::Value& root, Json::Value& returnInfo)
{
	int layerid = root["Layerid"].asInt();;

	CLayer* layer = g_LayerManager.GetLayer(layerid);
	if(layer)
	{
		layer->m_bVisible = true;
	}
	//CtrlRefresh(true);
	Draw();
	returnInfo = ConstructJsonValue(root["CmdName"].asString(), root["Cmdid"].asInt(), -1, layerid, 0, -1, -1);
}

void DrawEngine::DeleteLayerByJS(const Json::Value& root, Json::Value& returnInfo)
{
	int layerid = root["Layerid"].asInt();
	//int eventid = _ttoi(pStr[2]);

	bool bret = g_LayerManager.RemoveLayer(layerid);
	Draw();
	returnInfo = ConstructJsonValue(root["CmdName"].asString(), root["Cmdid"].asInt(), -1, layerid, 0, -1, -1);
}

//¹¹Ôì»ØÏÔÖµ
Json::Value DrawEngine::ConstructJsonValue(const std::string& CmdName, const int& CmdID, const int& elementID, 
	const int& layerid, const int& status, const int& styleid, const int& mapLevel)
{
	//ret.Format("{\"Cmdid\":%d,\"CmdName\":\"%s\",\"Status\":%d,\"Layerid\":%d,\"Elementid\":%d,\"Styleid\":%d,\"MapExtern\":[%lf,%lf,%lf,%lf],\"MapLevel\":%d}",cmdid,cmdname,status,layerid,elementid,styleid,x0,y0,x1,y1,js);
	Json::Value theReturnInfo;
	theReturnInfo["CmdName"] = Json::Value(CmdName);
	theReturnInfo["Cmdid"] = Json::Value(CmdID);
	theReturnInfo["Status"] = Json::Value(status);
	theReturnInfo["Layerid"] = Json::Value(layerid);
	theReturnInfo["Elementid"] = Json::Value(elementID);
	//µØÍ¼¼¶±ð
	{
		if(mapLevel > -1)
			theReturnInfo["mapLevel"] = Json::Value(mapLevel);
		else
		{
			int theMapJs = -1;
			CBaseLayer* theLayer = (CBaseLayer*)g_LayerManager.GetLayerByNumber(0);
			if(theLayer)
				theMapJs = theLayer->m_js;
			theReturnInfo["mapLevel"] = Json::Value(theMapJs);
		}	
	}
	theReturnInfo["Styleid"] = Json::Value(styleid);
	double xlefttop;
	double ylefttop;
	double xrightbottom;
	double yrightbottom;
	Pixel_to_Projection(0, 0, xlefttop, ylefttop);
	Pixel_to_Projection(g_CtrlW, g_CtrlH, xrightbottom, yrightbottom);
	theReturnInfo["MapExtern"].append(xlefttop);
	theReturnInfo["MapExtern"].append(ylefttop);
	theReturnInfo["MapExtern"].append(xrightbottom);
	theReturnInfo["MapExtern"].append(yrightbottom);
	return theReturnInfo;
}

Json::Value DrawEngine::ConstructJsonValueforWFS(const std::string& CmdName, const int& CmdID, const int& elementID, 
	const int& layerid, const std::string& layerName,const int& status, const int& styleid, const int& mapLevel)
{
	//ret.Format("{\"Cmdid\":%d,\"CmdName\":\"%s\",\"Status\":%d,\"Layerid\":%d,\"Elementid\":%d,\"Styleid\":%d,\"MapExtern\":[%lf,%lf,%lf,%lf],\"MapLevel\":%d}",cmdid,cmdname,status,layerid,elementid,styleid,x0,y0,x1,y1,js);
	Json::Value theReturnInfo;
	theReturnInfo["CmdName"] = Json::Value(CmdName);
	theReturnInfo["Cmdid"] = Json::Value(CmdID);
	theReturnInfo["Status"] = Json::Value(status);
	theReturnInfo["Layerid"] = Json::Value(layerid);
	theReturnInfo["LayerName"] = Json::Value(layerName);
	theReturnInfo["Elementid"] = Json::Value(elementID);

	{
		if(mapLevel > -1)
			theReturnInfo["mapLevel"] = Json::Value(mapLevel);
		else
		{
			int theMapJs = -1;
			CBaseLayer* theLayer = (CBaseLayer*)g_LayerManager.GetLayerByNumber(0);
			if(theLayer)
				theMapJs = theLayer->m_js;
			theReturnInfo["mapLevel"] = Json::Value(theMapJs);
		}	
	}
	theReturnInfo["Styleid"] = Json::Value(styleid);
	double xlefttop;
	double ylefttop;
	double xrightbottom;
	double yrightbottom;
	Pixel_to_Projection(0, 0, xlefttop, ylefttop);
	Pixel_to_Projection(g_CtrlW, g_CtrlH, xrightbottom, yrightbottom);
	theReturnInfo["MapExtern"].append(xlefttop);
	theReturnInfo["MapExtern"].append(ylefttop);
	theReturnInfo["MapExtern"].append(xrightbottom);
	theReturnInfo["MapExtern"].append(yrightbottom);
	return theReturnInfo;
}

//#define GLHPERSPECTIVEF2
void DrawEngine::SetRect(const long& width, const long& height)
{

	m_CtrlWidth = width;
	m_CtrlHeight = height;
	
	float mpv[16];
	float trs[16];
	float rot[16];
	float objectrot[16];
	

	identity_matrix(trs);
	identity_matrix(rot);
	identity_matrix(g_mpv);
	identity_matrix(objectrot);

	float rot2[16];
	identity_matrix(rot2);
#ifdef GLHPERSPECTIVEF2
	GLfloat rotTemp1[16];
	GLfloat rotTemp2[16];
	glhPerspectivef2(&mpv[0], 45.0f, width * 1.0 / height, 2, 100000); //×Ô¼ºÊµÏÖµÄÍ¸ÊÓ
	m_ztranslate_ = -(m_CtrlWidth + m_CtrlHeight) / 2;
	m_xtranslate_ = -width / 2;
	m_ytranslate_ = -height / 2 - 256 / 2;				// the picture size is 256

	rotate_matrix(m_xrotate_, 0, 0, rot);			// z * y * x
	translate_matrix(m_xtranslate_, m_ytranslate_, m_ztranslate_, trs);
	rotate_matrix(0, 0, m_zrotate_, rotTemp1);
	multiply_matrix(rot, rotTemp1, rot);
	multiply_matrix(trs, rot, trs);							// trs * rot
	multiply_matrix(&mpv[0], trs, &g_mpv[0]);				// mpv * trs
#else
	//×Ô¼ºÊµÏÖµÄ Ö¸¶¨clip volume,´ËÊ±µÄ[ÊÓµã]ÔÚ´°¿ÚµÄ[×óÏÂ½Ç]  ÕýÍ¶Ó°¾ØÕó
	glhOrtho2(&g_mpv[0], 0, width, 0, height, -1, 1);  //´´½¨clip,yanjianlongÕâ¸öº¯Êý±àÐ´´íÎóÁË

	//glhPerspectivef2(&g_mpv[0],45,1, 1, 1000);

	translate_matrix(0.0f, height, 0.0f, trs); //½«ÊÓµã [ÏòÉÏ] Æ½ÒÆ g_CtrlH¸öµ¥Î»
	scale_matrix(1.0f, -1.0f, 1.0f, rot);  //°ÑyÖáµÄ·½ÏòÄæÖÃ
	
	multiply_matrix(trs,objectrot,trs);
	multiply_matrix(trs, rot, trs); // ÏòÉÏÆ½ÒÆg_CtrlH¸öµ¥Î»
	multiply_matrix(trs, rot2, trs);// ½«YÖáµÄÖµ·´×ª
	multiply_matrix(g_mpv, trs, g_mpv); // µÃµ½×îÖÕ×ª»»¾ØÕó
#endif
	return;

}


void DrawEngine::RenderRect(PP_Point pStart,PP_Point pEnd)    //ÓÉÖÕÆðµã×ø±ê »æÖÆ¾ØÐÎ
{
	if(pStart.x < 0 || pStart.y < 0|| pEnd.x < 0 || pEnd.y < 0)
		return;
	if(pStart.x == pEnd.x || pStart.y == pEnd.y)
		return;
	if(g_DrawRectVboid == 0)
		glGenBuffers(1, &g_DrawRectVboid);

	glUseProgram(g_programObj_Shp);
	//g_MVPLoc = glGetUniformLocation(g_programObj_Shp, "a_MVP");
	glUniformMatrix4fv(g_MVPLoc, 1, GL_FALSE, (GLfloat*) g_mpv);

	long pix1 = pStart.x;
	long piy1 = pStart.y;
	long pix2 = pEnd.x;
	long piy2 = pEnd.y;

	Vertex* verts = new Vertex[4];

	verts[0].tu = 0;
	verts[0].tv = 0;
	verts[0].loc[0] = pix1;
	verts[0].loc[1] = piy1;
	verts[0].loc[2] = 1;
	verts[0].color[0] = 200/255.0;
	verts[0].color[1] = 0/255.0;
	verts[0].color[2] = 0/255.0;
	verts[0].color[3] = 100/255.0;

	verts[1].tu = 0;
	verts[1].tv = 0;
	verts[1].loc[0] = pix1;
	verts[1].loc[1] = piy2;
	verts[1].loc[2] = 1;
	verts[1].color[0] = 200/255.0;
	verts[1].color[1] = 0/255.0;
	verts[1].color[2] = 0/255.0;
	verts[1].color[3] = 100/255.0;

	verts[2].tu = 0;
	verts[2].tv = 0;
	verts[2].loc[0] = pix2;
	verts[2].loc[1] = piy1;
	verts[2].loc[2] = 1;
	verts[2].color[0] = 200/255.0;
	verts[2].color[1] = 0/255.0;
	verts[2].color[2] = 0/255.0;
	verts[2].color[3] = 100/255.0;

	verts[3].tu = 0;
	verts[3].tv = 0;
	verts[3].loc[0] = pix2;
	verts[3].loc[1] = piy2;
	verts[3].loc[2] = 1;
	verts[3].color[0] = 200/255.0;
	verts[3].color[1] = 0/255.0;
	verts[3].color[2] = 0/255.0;
	verts[3].color[3] = 100/255.0;

	glBindBuffer(GL_ARRAY_BUFFER, g_DrawRectVboid);
	glBufferData(GL_ARRAY_BUFFER, 4* sizeof(Vertex), verts,GL_STATIC_DRAW);
//·Ö¸îÊý×éÖÐ Êý¾Ý¸ñÊ½
	glVertexAttribPointer(g_positionLoc_Shp, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,loc));
	glEnableVertexAttribArray(g_positionLoc_Shp);

	glVertexAttribPointer(g_colorLoc_Shp, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,color));
	glEnableVertexAttribArray(g_colorLoc_Shp);

	/*glVertexAttribPointer(g_texCoordLoc_Shp, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,tu));
	glEnableVertexAttribArray(g_texCoordLoc_Shp);*/

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);//GL_TRIANGLE_STRIP
	//glDeleteBuffers(1, &vboid);		// ´æÔÚÎÊÌâ
	delete[] verts;
	verts = NULL;
}   
//#define FILLPOLY
//Íâ¿òÂ·¾¶£¬¼ÓÉÏ×îºóÒ»¸öµã£¬ÊµÖÊÉÏ¾ÍÊÇ Â·¾¶ÖÐµÄµÚÒ»¸öµã
void DrawEngine::RenderPolygon(std::list<MapEngineES_Point>& thePointList, const PP_Point& point_mouseMove)
{
	if(thePointList.empty())  //Êó±ê [×ó¼ü]µã»÷´¦ ËùÓÐµÄ µã
		return;
	std::list<MapEngineES_Point>::iterator thebeg = thePointList.begin();
	std::list<MapEngineES_Point>::iterator theend = thePointList.end();
//ÔÚÕâÀï¼ÓÈëÎ²µã ÊÇÁ¬ÐøµÄÐ§¹û
	//double x = 0;
	//double y = 0;
	//Pixel_to_Projection(point_mouseMove.x, point_mouseMove.y, x, y);
	//MapEngineES_Point tmp = {x,y};
	//thePointList.push_back(tmp);

//ÕâÀïÐ´ËÀÁË linewidth
	CLineElement *g_LineElement;
	if(g_ChooseOperator == enumMapOperator::vLineChoose)
		g_LineElement = new CLineElement(255, 0, 0, 100, g_LineChooseRect,this,FALSE);
	else
		g_LineElement = new CLineElement(255, 0, 0, 100, 1,this,TRUE); //R,G,B,A,width  ´´½¨Ò»¸ö»æÏßµÄ»ù´¡Àà,Ê¹ÓÃÊµÌåÌî³ä
	
	g_LineElement->m_type = 1; //Ò»´ÎÐÔ»æÖÆ¶àÌõLine DrawLines(),²¢ÇÒÐèÒª²¹Ô²

	while(thebeg != theend)
	{
		g_LineElement->AddPoint(thebeg->x, thebeg->y);
		thebeg++;
	}
//ÔÚÕâÀï¼ÓÈëÎ²µã ÊÇÀëÉ¢µÄÏß¶ÎÐ§¹û
	double x = 0;
	double y = 0;
	Pixel_to_Projection(point_mouseMove.x, point_mouseMove.y, x, y);
	g_LineElement->AddPoint(x, y); //½«Êó±êÒÆ¶¯¹ý³ÌÖÐµÄ²úÉúµÄ [µã µØÀí×ø±ê] ±£´æÏÂÀ´

	g_LineElement->RenderUseOgles();
	delete g_LineElement;  //ÊÍ·Å¸ÃÓÃÓÚ»æÖÆµÄ ÁÙÊ±±äÁ¿£¬ÔÚÔ­À´µÄC versionÖÐÕâ¸ö»æÖÆ»ù´¡ÀàÊÇÈ«¾Ö±äÁ¿
}
//²ÎÊýÊÇpolyµÄÍâ¿ò(Ê×Î²±ÕºÏµÄ) µØÀí×ø±ê
 void DrawEngine::FillPoly(const vector<MapEngineES_Point> &PolyBorder,int r,int g,int b,int a)
 {
     glUseProgram(g_programObj_Shp);  //ÕâÀïÃæµÄ×ÅÉ«Æ÷ ÊÇÔÚMainWnd.cppÖÐµÄvoid InitProgram( void )ÖÐÍê³É³õÊ¼»¯
	 glUniformMatrix4fv(g_MVPLoc_Shp, 1, GL_FALSE, (GLfloat*) g_mpv);
	 //if (poly_vboID==0)   //¾ÍÊÇÕâ¶Î´úÂëÓÐÎó
		glGenBuffers(1, &poly_vboID);
	 
	 glEnable(GL_STENCIL_TEST);
	 glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);    // ½ûÓÃÑÕÉ«»º´æÐ´Èë ÖØÒª£¡£¡ 
	 glClear(GL_STENCIL_BUFFER_BIT); //¼ÓÈëÕâ¾ä±ÜÃâÉÁË¸
	 // ÉèÖÃÄ£°å»º´æ²Ù×÷º¯ÊýÎªGL_INVERT
	 glStencilFunc(GL_ALWAYS, 0x1, 0x1);
	 glStencilOp(GL_KEEP, GL_KEEP, GL_INVERT); 
	 glDisable(GL_DEPTH_TEST); //½ûÓÃÉî¶È²âÊÔ

	 int count = PolyBorder.size();
	 Vertex *verts = new Vertex[count+1];
	 long x1;
	 long y1;
	 for (int i=1 ;i<(count+1);i++) //Ô¤ÁôÒ»¸öÎ»ÖÃ·ÅÖÃÖÐÐÄµã
	 {
	    Projection_to_Pixel(PolyBorder[i-1].x ,PolyBorder[i-1].y,x1,y1);  //¾­Î³¶È µ½OpenGL»æÖÆÊ±Ê¹ÓÃµÄÂß¼­×ø±êÖµ
	 	verts[i].tu = 0;
	 	verts[i].tv = 0;
	 	verts[i].loc[0] = x1;
	 	verts[i].loc[1] = y1;
	 	verts[i].loc[2] = 1;
	 	verts[i].color[0] = r/255.0;
	 	verts[i].color[1] = g/255.0;
	 	verts[i].color[2] = b/255.0;
	 	verts[i].color[3] = a/255.0;
	 }
	 //ÉèÖÃÖÐÐÄµã,ÕâÀï¾ÍÊÇÉèÖÃµÄÊÇµÚÒ»¸öµã
	 verts[0].tu = 0;
	 verts[0].tv = 0;
	 verts[0].loc[0] = x1;
	 verts[0].loc[1] = y1;
	 verts[0].loc[2] = 1;
	 verts[0].color[0] = r/255.0;
	 verts[0].color[1] = g/255.0;
	 verts[0].color[2] = b/255.0;
	 verts[0].color[3] = a/255.0;

	 glBindBuffer(GL_ARRAY_BUFFER,poly_vboID);
	 glBufferData(GL_ARRAY_BUFFER, (count+1)* sizeof(Vertex), verts,GL_STATIC_DRAW);
	 glVertexAttribPointer(g_positionLoc_Shp, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,loc));
	 glEnableVertexAttribArray(g_positionLoc_Shp);
	 glVertexAttribPointer(g_colorLoc_Shp, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,color));
	 glEnableVertexAttribArray(g_colorLoc_Shp);
	 glDrawArrays(GL_TRIANGLE_FAN,0,count+1);//GL_TRIANGLE_STRIP

	 glStencilFunc(GL_NOTEQUAL,0,0x1);
	 glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	 glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);  
	 glBindBuffer(GL_ARRAY_BUFFER,poly_vboID);
	 glVertexAttribPointer(g_positionLoc_Shp, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,loc));
	 glEnableVertexAttribArray(g_positionLoc_Shp);
	 glVertexAttribPointer(g_colorLoc_Shp, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,color));
	 glEnableVertexAttribArray(g_colorLoc_Shp);
	 glDrawArrays(GL_TRIANGLE_FAN,0,count+1);//GL_TRIANGLE_STRIP
	 glDisable(GL_STENCIL_TEST);
 }

 //»ñÈ¡µ±Ç°µØÍ¼µÄ ÏÔÊ¾·Ö±æÂÊ
void DrawEngine::GetCurrentShowInfo(double &cenlon,double &cenlat,double & width,double & height,double & leftlon,double & Toplat,double& lon,double &lat,double & scale,CString & Path,double &resolution)
{
	CBaseLayer* baselayer = static_cast<CBaseLayer* >(g_LayerManager.GetLayer(0));
	if (baselayer)
	{
		baselayer->GetBaseLayerConf(cenlon,cenlat,width,height,leftlon,Toplat,lon,lat,scale,Path,resolution);
	}
}

void DrawEngine::Projection_to_Pixel(double prX,double prY,long &pix,long &piy)
{
	double m_X,m_Y,pletflon,plon,pwidth,pheight,pToplat,plat,scale,resolution;
	CString name;
	GetCurrentShowInfo(m_X,m_Y ,pwidth,pheight,pletflon,pToplat,plon,plat,scale,name,resolution);

	double resolutionX = 0.0;//µØÍ¼ºáÏò·Ö±æÂÊºÍ×ÝÏò·Ö±æÂÊ¿ÉÄÜ²»Ò»Ñù£¡
	double resolutionY = 0.0;
	resolutionX = plon/pwidth;
	resolutionY = plat/pheight;

	if (pwidth==-1)  //ÇØ»Êµº Õâ¸öÖµÄ¬ÈÏ ¾Í±»ÉèÖÃÎªÁË-1
	{
		resolutionX = resolution;
		resolutionY = resolution;

		//×óÉÏ½ÇµÄ×ø±ê
		int tLeftpixX = (int)((m_X - pletflon)/resolutionX) - g_CtrlW/2;
		int tLeftpixY = (int)((m_Y - pToplat )/resolutionY) + g_CtrlH/2;

		//´«½øÀ´µÄµØÀí×ø±ê
		int tpixX = (int)((prX - pletflon)/resolutionX);
		int tpixY = (int)((prY - pToplat)/resolutionY);

		piy =  tLeftpixY - tpixY  ;
		pix =  tpixX - tLeftpixX;
	}
	else
	{
		//×óÉÏ½ÇµÄ×ø±ê
		int tLeftpixX = (int)((m_X - pletflon)/resolutionX) - g_CtrlW/2;
		int tLeftpixY = (int)((pToplat - m_Y)/resolutionY) - g_CtrlH/2;

		//´«½øÀ´µÄµØÀí×ø±ê
		int tpixX = (int)((prX - pletflon)/resolutionX);
		int tpixY = (int)((pToplat - prY)/resolutionY);

		piy =  tpixY - tLeftpixY;
		pix =  tpixX - tLeftpixX;
	}
}

void DrawEngine::Pixel_to_Projection(long pix,long piy,double &prX,double &prY)
{
	double m_X,m_Y,pletflon,plon,pwidth,pheight,pToplat,plat,scale,resolution;
	CString name;
	//ÖÐÐÄµã¾­Î³¶È£¬Í¼²ãÖÐÍ¼Æ¬µÄ×Ü¿í¸ß£¬Í¼²ã×óÉÏ½Ç¾­Î³¶È£¬Í¼²ãµÄ¾­Î³¶È¿ç¶È£¬±ÈÀý£¬Í¼²ãÃû,¶ÁÈ¡µ×Í¼
	GetCurrentShowInfo(m_X,m_Y ,pwidth,pheight,pletflon,pToplat,plon,plat,scale,name,resolution);

	CBaseLayer* baselayer = static_cast<CBaseLayer* >(g_LayerManager.GetLayer(0));
	if (baselayer)
	{
		if (baselayer->m_MapType==GeoBeansServerMap || baselayer->m_MapType==PGISServerMap || baselayer->m_MapType == ArcgisMap)  
		{
			prX = m_X - resolution * (g_CtrlW/2 - pix);
			prY = m_Y + resolution * (g_CtrlH/2 - piy);
		}
		else   //Õâ¸öelseÊÇ¶àÓàµÄ
		{
			prX = m_X - (plon/pwidth) * (g_CtrlW/2 - pix);
			prY = m_Y + (plat/pheight)* (g_CtrlH/2 - piy);
		}

	}
}

void DrawEngine::MoveView(double x,double y,double z,int& js)
{
	CLayer* layer = g_LayerManager.GetLayer(0);
	if (layer)
	{
		CBaseLayer *baseLy = static_cast<CBaseLayer*>(layer); //ÏòÉÏcast Ó¦¸ÃÊÇ×Ô¶¯Íê³ÉµÄ	
		//¼ì²éµ±Ç°µØÍ¼µÄ ·Ö±æÂÊ¼¶±ð,ÓÐÎÊÌâ£¬ÕâÀïµÄm_MaxLevel ÖµÊ¼ÖÕÊÇÔÚ¹¹ÔìÆ÷ÖÐ³õÊ¼µÄ -1
		if(js < baseLy->m_MinLevel || (baseLy->m_MaxLevel > 0 && js > baseLy->m_MaxLevel))
		{
			//µ±Ç°µÄµØÍ¼±ÈÀý³ß²»ÔÚ·¶Î§ÄÚ£¬ÒªÏë»Ø¹éµ½·¶Î§ÄÚ
			if(js < baseLy->m_MinLevel)
				js = baseLy->m_MinLevel;  
			else 
				js = baseLy->m_MaxLevel;
		}
		baseLy->SetMapCentre(x,y,z,js); //½«m_bUpdateDataÉèÖÃÎªtrue£¬ÔÙ´ÎÖØ»æÊ±£¬»áÖØÐÂµÄÄÃµØÍ¼
		
	}
	Draw(); //µØÍ¼ÖÐµÄÖÐÐÄµãÎ»ÖÃÒÑ¾­¸üÐÂ£¬ÖØ»æ»áµ¼ÖÂÕæÊµµÄµØÍ¼ÕâÀïÊÇ CArcgisMapLayer±£´æÕâ¸öÐÂµÄµØÍ¼ÖÐÐÄµã£¬ÕâÑù½ÓÏÂÀ´£¬Pixel_to_Projection
	return;
}

void DrawEngine::MoveMapViewByJS(const Json::Value& root, Json::Value& returnInfo)
{
	int LayerCount = g_LayerManager.GetLayerCount();
	for(int i = 1;i <= LayerCount;++i)
	{
		CLayer *layer = g_LayerManager.GetLayerByNumber(i);
		if (layer == NULL)
		{
			continue;
		}                                                                                       
		if ( (layer->m_layerType == VLayerType::vElementLayer) || (layer->m_layerType == VLayerType::vBufferLayer) || (layer->m_layerType == VLayerType::vWFSPolyLayer))
		{  

			CWFSLayer *eleLayer = (CWFSLayer *)layer;
			(eleLayer->m_LineElementSet_).m_MapMoveComplete = true;
		}
	}

    //MOVEVIEW PIX x,y,js,ÐòºÅ
	string coortype = root["Coortype"].asString();  //×ø±êÀàÐÍ
	if (coortype.compare("PIX") == 0)
	{
		int pixX = root["OffsetX"].asInt();
		int pixY = root["OffsetY"].asInt();
		int js = root["Level"].asInt();  //ÕâÀïÖµ ½âÎöµÄ Îª 0
		double prX = 0.0;
		double prY=0.0;
		double prZ=0.0;
		int cjs=0;
		long x=0;
		long y = 0;
		CBaseLayer *baselayer = static_cast<CBaseLayer*>(g_LayerManager.GetLayer(0));
		if (NULL != baselayer)
		{
			if(js <= 0)  // modify by xuyan  Ìí¼ÓÒ»¸öµÈºÅ£¬Ê¹µÃÆ½ÒÆ²Ù×÷»Ö¸´Õý³£
				js = baselayer->m_js;
			baselayer->GetMapCentre(prX,prY,prZ,cjs); //»ñÈ¡µØÍ¼ÖÐÐÄµã(¾­Î³¶È)
			Projection_to_Pixel(prX,prY,x,y);
			Pixel_to_Projection(x-pixX,y-pixY,prX,prY);
			MoveView(prX,prY,0,js);
		}
	}	
	else if (coortype.compare("GEO") == 0)  //·¢ËÍÀ´µÄÊÇµØÀí×ø±ê
	{
		double prxX = root["CenX"].asDouble();
		double prxY = root["CenY"].asDouble();   //ÖÐÐÄµãµÄ ¾­Î³¶È ÖØÖÃÊÇ¶ÔµÄ
		int js = root["Level"].asInt();  //ÖµÎª0

		CBaseLayer *baselayer = static_cast<CBaseLayer*>(g_LayerManager.GetLayer(0));
		if (NULL != baselayer)
		{
			MoveView(prxX,prxY,0,js); //¸Ä±äÁËµØÍ¼µÄÖÐÐÄµã£¬»áÓ°ÏìPixel_to_ProjectionºÍProjection_to_PixelµÄ¼ÆËã½á¹ûÖµ
		}
	}
	returnInfo = ConstructJsonValue(root["CmdName"].asString(), root["Cmdid"].asInt(), -1, -1, 0, -1, -1);
}

//»æÖÆÁÙÊ±µÄ »º³åLineLayer ,µã»º³å  ÓïÒå£ºvBufferLayerÔÚÍ¼²ãÈÝÆ÷ÖÐÖ»´æÔÚÒ»¸ö£¬ËùÒÔ¿ÉÒÔ°´ÕÕÍ¼²ãÀàÐÍÔÚÈÝÆ÷ÖÐ²éÕÒ
void DrawEngine::BufferByJS(const Json::Value& root, Json::Value& returnInfo)
{
    int layerid = -1;
	CElementLayer *BufferLayer = NULL; //ÓÉÍ¼²ãÈÝÆ÷£¬¸ºÔð»ØÊÕ¸Ã×ÊÔ´£¬ÔÚC versionÖÐ£¬¸ÃÍ¼²ãÊÇÓÉÈ«¾Ö±äÁ¿³ÖÓÐ
	
	int layerCount = g_LayerManager.GetLayerCount(); //×ÜÍ¼²ãÊý
	for(int i = 0 ;i < layerCount; ++i)
	{
		CLayer *layer = g_LayerManager.GetLayerByNumber(i+1);
		//¿¼ÂÇµ½ÓÐµÄÍ¼²ãÒÑ¾­É¾³ý£¬¶ø²éÕÒÊÇ forµÄÏßÐÔÑ­»· £¬ËùÒÔ²ÅÓÐÁË ÕâÀï ifÅÐ¶Ï
		if (layer == NULL)
		{
			continue;
		}
		if(VLayerType::vBufferLayer == layer->m_layerType)
		{
			BufferLayer = static_cast<CElementLayer *>(layer);
			layerid = i + 1;
		}
	}
	//ÈÝÆ÷ÖÐ²»´æÔÚÕâ²ãÊ±£¬²ÅÌí¼ÓÐÂµÄ²ã
	if (BufferLayer==NULL)
	{
		BufferLayer = new CElementLayer(); //´ËÊ±Ä¬ÈÏµÄ m_layerType = vElementLayer
		BufferLayer->m_layerType = VLayerType::vBufferLayer;  //Í¼²ãÀàÐÍ
		BufferLayer->m_bVisible = true;
		layerid = g_LayerManager.AddLayer(BufferLayer);
	}

	int layerhandle = layerid;
	int elementhandle = -1;
	std::string theOpeator = root["Type"].asString();
	if(theOpeator.compare("POINT") == 0)  //µã»º³å²ã
	{
		//double x = root["X"].asDouble();
		//double y = root["Y"].asDouble();
		//int r = root["ColorR"].asInt();
		//int g = root["ColorG"].asInt();
		//int b = root["ColorB"].asInt();
		//int a = root["ColorA"].asInt();
		//string u = root["UNIT"].asString();
		//CString unit = UTF8ToGB2312(u.c_str());
		//int d = root["DISTANCE"].asInt();

		//elementhandle = AddCircleRing(layerid,x,y,0,d,r,g,b,a,unit);
		//CtrlRefresh(true);
	}
	else if(theOpeator.compare("LINE") == 0)  
	{		
		CLineElement *lineBuffer = new CLineElement(this); //´ËÊ±Ä¬ÈÏµÄ m_type = 0,ÒòÎªÔÚCLineElement::DrawLinesÖÐ»áµ÷ÓÃden->GetCurrentShowInfo(X,Y ,pwidth,pheight,pletflon,pToplat,plon,plat,scale,name,resolution);

		int pointcount = root["X"].size();
		Json::Value pointX_arry = root["X"];
		Json::Value pointY_arry = root["Y"];
		for (int i=0;i<pointcount;i++)  //×éµã
		{
			//MapEngineES_Point p;
			double x = pointX_arry[i].asDouble();
			double y = pointY_arry[i].asDouble();
			lineBuffer->AddPoint(x,y);
		}

		int r = root["ColorR"].asInt();
		int g = root["ColorG"].asInt();
		int b = root["ColorB"].asInt();
		int a = root["ColorA"].asInt();
		int w = root["DISTANCE"].asInt();
		string u = root["UNIT"].asString();
		CString unit = UTF8ToGB2312(u.c_str());  //ÕâÀï¾ÍÊÇ±ê×¢µÄ metre
		int d = root["DISTANCE"].asInt();

		if(!root["levelWidth"].empty())
		{
			lineBuffer->m_cast = false; //µÀÂ·»Ø·Å
			lineBuffer->widthSize = root["levelWidth"].size();
			Json::Value width_arry = root["levelWidth"];
			
			lineBuffer->m_LineWidth.clear(); //ÏÈÇå¿Õ Ïß¿í
			for(int i = 0;i< lineBuffer->widthSize;i++) 
				lineBuffer->m_LineWidth.push_back(width_arry[i].asInt());

		}

		lineBuffer->m_w = w;
		lineBuffer->m_r = r;
		lineBuffer->m_g = g;
		lineBuffer->m_b = b;
		lineBuffer->m_a = a;
		lineBuffer->m_unit = unit;
		lineBuffer->m_type = 2;  //Ê¹ÓÃDrawCirclepat»­Ô²È¦À´²¹£¬½«¸ÃÀàÐÍ ¸ü¸ÄÎª1£¬Ê¹ÓÃ×ÅÉ«Æ÷²¹µã

		elementhandle = BufferLayer->AddElement(lineBuffer);
		//CtrlRefresh(true);
	}
	else if (theOpeator.compare("RING") == 0)
	{
		//double x = root["X"].asDouble();
		//double y = root["Y"].asDouble();
		//int r = root["ColorR"].asInt();
		//int g = root["ColorG"].asInt();
		//int b = root["ColorB"].asInt();
		//int a = root["ColorA"].asInt();
		//string u = root["UNIT"].asString();
		//CString unit = UTF8ToGB2312(u.c_str());
		//int d1 = root["DISTANCE1"].asInt();
		//int d2 = root["DISTANCE2"].asInt();

		//elementhandle = AddCircleRing(layerid,x,y,d1,d2,r,g,b,a,unit);
		//CtrlRefresh(true);
	}
	else if (theOpeator.compare("POLYGON") == 0)
	{

	}
	else if (theOpeator.compare("CLEAR") == 0)
	{
		if (BufferLayer!=NULL)
		{		
			BufferLayer->DeleteAllElement();
		//	BufferLayer = NULL;
//			CtrlRefresh(true);
		}
	}
	if(theOpeator.compare("VEDIO") == 0)
	{
		//double x = root["X"].asDouble();
		//double y = root["Y"].asDouble();
		//string u = root["UNIT"].asString();
		//CString unit = UTF8ToGB2312(u.c_str());
		//int d = root["DISTANCE"].asInt();
		//int lid = root["LAYERID"].asInt();//ÐèÒª¼ÆËã»º³åµÄÍ¼²ãid

		//{
		//	//Ã»ÓÐÈÎºÎ»æÖÆ£¬Ö»ÊÇ·µ»ØÊý¾Ý
		//	//µ¥Î»×ª»¯:Ã××ªÏñËØ
		//	double radius2 = 0;
		//	if (u.compare("metre") == 0)
		//	{
		//		double X, Y, pletflon, plon, pwidth, pheight, pToplat, plat, scale, resolution;
		//		CString name;
		//		GetCurrentShowInfo(X, Y, pwidth,pheight,pletflon,pToplat,plon,plat,scale,name,resolution);

		//		radius2 = d * (96 * 39.7) / scale;
		//		//radius2 = 
		//	}
		//	long cenx = 0.0;
		//	long ceny = 0.0;
		//	Projection_to_Pixel(x, y, cenx, ceny);
		//	double xbegin = cenx - radius2;//*cos(45.0);
		//	double ybegin = ceny+radius2;//*sin(45.0);
		//	double xend = cenx+radius2;//*cos(45.0);
		//	double yend = ceny-radius2;//*sin(45.0);

		//	//¹¹½¨¾ØÐÎÇøÓò
		//	double x0,y0,x1,y1;

		//	Pixel_to_Projection(xbegin,ybegin,x0,y0);
		//	Pixel_to_Projection(xend,yend,x1,y1);
		//	std::string theState = root["VIDEO"].asString();
		//	//int layercount = g_LayerManager.GetLayerCount();		//²»°üÀ¨µ×Í¼
		//	int layernumber = g_LayerManager.GetLayerNumber(lid);
		//	if(layernumber > 0)
		//	{
		//		vector<MapEngineES_Point> theReturnValue;

		//		bool searchState = false;
		//		if(theState.compare("TRUE") == 0)					//Ö»Ñ¡ÊÓÆµµã			//xÐ¡ yÐ¡ x´ó y´ó
		//			searchState = isExistInShapefileForVideo(layernumber, x0, y0, x1, y1, theReturnValue);
		//		else
		//			searchState = isExistInShapefileForVideo(layernumber, x0, y0, x1, y1, theReturnValue, false);
		//		//if(isExistInShapefileForVideo(layercount, x0, y1, x1, y0, theReturnValue))

		//		//·µ»ØÐÅÏ¢µ½Ò³Ãæ
		//		if(searchState)
		//		{
		//			CStringA retValue;			//·µ»ØµÄ×Ö·û´®¸ñÊ½FID ;
		//			//Íùjs·¢ËÍÐÅÏ¢
		//			for(int i = 0;i<theReturnValue.size();)
		//			{
		//				//ÅÐ¶ÏÊÇ·ñÔÚÔ°ÄÚ
		//				long pix,piy;
		//				Projection_to_Pixel(theReturnValue[i].x, theReturnValue[i].y, pix, piy);
		//				double xx = (pix - cenx) * (pix - cenx);
		//				double yy = (piy - ceny) * (piy - ceny);
		//				double d = sqrt((xx + yy) * 1.0);
		//				if (d < radius2)
		//				{
		//					//ÔÚÔ°ÄÚ
		//					CStringA fidString;
		//					fidString.Format("{\"FID\":%d,", theReturnValue[i].fid);
		//					retValue += fidString;
		//					fidString.Format("\"GISID\":%d},", theReturnValue[i].GisID);
		//					retValue += fidString;
		//				} 
		//				i++;
		//			}
		//			CStringA ret;
		//			ret.Format("{\"CmdName\":\"VIDEOSEARCH\",\"LayerHandle\":%d,\"SearchResult\":[%s]}", g_LayerManager.GetLayerHandle(layernumber), retValue.Left(retValue.GetLength() - 1));
		//			PostMessage(ret.GetBuffer());
		//			ret.ReleaseBuffer();

		//			//if(theState.compare("TRUE") == 0)			//Ö»ÊÇÑ¡ÊÓÆµµã
		//				//break;
		//		}
		//		//layercount--;
		//	}
		//	return;
		//}
	}
	returnInfo = ConstructJsonValue(root["CmdName"].asString(), root["Cmdid"].asInt(), elementhandle, layerhandle, 0, -1, -1);
}

//²Ù×÷GIFµÄº¯Êý
void DrawEngine::GifCmdByJS(const Json::Value& root, Json::Value& returnInfo)
{
	//return;
	Activate(root,returnInfo); //ÏÈ¼¤»î²Ù×÷µÄµ±Ç°ÊµÀý£¬±ÜÃâÈ±Í¼ 
	int layerhandle = -1;
	int elementhandle = -1;
	int status = 0;
	std::string theOperaotr = root["OPERATOR"].asString();
	CGifLayer *GIFLayer = NULL;
	if(theOperaotr.compare("ADD") == 0)			//Ôö¼Ó
	{
		
		string URL = root["URL"].asString();
		CString filename = UTF8ToGB2312(URL.c_str());
		//ÅÐ¶ÏgifÊÇ±¾µØ»¹ÊÇ·þÎñ

		double x = root["X"].asDouble();
		double y = root["Y"].asDouble();
		int w = root["Width"].asInt() * g_picScale;
		int h = root["Height"].asInt() * g_picScale;

//ÔÚÍ¼²ãÈÝÆ÷ÖÐ£¬ÒÀ¾Ý[Í¼²ãÀàÐÍ]£¬ÕÒµ½¸ÃÍ¼²ã
		int LayerCount = g_LayerManager.GetLayerCount();
		for(int i=0;i<LayerCount;i++)
		{
			CLayer *layer = g_LayerManager.GetLayerByNumber(i+1);
			//¿¼ÂÇµ½ÓÐµÄÍ¼²ãÒÑ¾­É¾³ý£¬¶ø²éÕÒÊÇ forµÄÏßÐÔÑ­»· £¬ËùÒÔ²ÅÓÐÁË ÕâÀï ifÅÐ¶Ï
			if (layer == NULL)
			{
				continue;
			}
			if( layer->m_layerType == VLayerType::vGifLayer)
			{
			   GIFLayer = (CGifLayer *)layer;
			   layerhandle = i + 1;    //±£´æÍ¼²ãIDÓÃÓÚ·µ»Ø
			   break;
			}
		}

		if (GIFLayer == NULL) //¸Ã²Ù×÷·½Ê½£¬È·±£ÁË£¬CGifLayer Ö»»á´æÔÚÒ»¸öÍ¼²ã£¬ËùÒÔ£¬¿ÉÒÔ¸ù¾ÝÍ¼²ãÀàÐÍ£¬´Óg_LayerManagerÖÐ²éÑ¯³ö ¸ÃLayer
		{
			GIFLayer = new CGifLayer(); 
			layerhandle = g_LayerManager.AddLayer(GIFLayer);
		}

		CGifElement *gifobj = NULL;
		if(URL.find("http") != -1)  
		{
			//´ÓHttpÄÃÊý¾Ý
			CString szExePath = L"";
			::GetModuleFileName(NULL, szExePath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
			szExePath = szExePath.Left(szExePath.ReverseFind('\\'));
			szExePath = szExePath.Left(szExePath.ReverseFind('\\'));

			CString gifname = filename.Right(filename.GetLength() - filename.ReverseFind('/') -1);
			//CString gifpath = szExePath + L"\\ICON\\GIF\\";
			//ÔÚ´ËÖ®Ç°ÎÒÒª´´½¨Õâ¸ö±£´æ±£´æÍøÂçÍ¼Æ¬µÄ ±¾µØÄ¿Â¼
            CString pTemp = szExePath + L"\\ICON";
			bool bc = false;
			int err;
			if(!PathIsDirectory(pTemp))
			{
				bc = CreateDirectory(pTemp,NULL);   //Èç¹û²»´æÔÚÔò´´½¨¸ÃÄ¿Â¼
				err = GetLastError();
			}
			pTemp += L"\\GIF";
			if(!PathIsDirectory(pTemp))
			{
				bc = CreateDirectory(pTemp,NULL);   //Èç¹û´æÔÚÔò´´½¨¸ÃÄ¿Â¼
				err = GetLastError();
			}
			CString gifpath = pTemp + L"\\";
			CString strSavePath = gifpath + gifname;

			if (PathFileExists(strSavePath))  //¸Ã´ÅÅÌÎÄ¼þ´æÔÚ¡£Ê¹ÓÃÍ¬Ò»¸öconstructor,´«µÝÍ¼Æ¬Â·¾¶²»Í¬
				gifobj = new CGifElement(strSavePath, x, y, w, h,this);  //ÍøÂçÉÏµÄÍ¼Æ¬£¬ÔÚ±¾µØ´ÅÅÌÉÏ´æÔÚ
			else
				gifobj = new CGifElement(filename,x,y,w,h,this); //²»´æÔÚ£¬ÔòÒªÈ¥ÍøÂçÉÏÈ¥ÏÂÔØ
		}
		else
		{
			//´Ó±¾µØÄÃ
			fstream _file;
			_file.open(URL, ios::in);
			if (!_file) //Èç¹û´ò¿ªÎÄ¼þÊ§°Ü
				return;
			else
				_file.close();
			gifobj = new CGifElement(filename, x, y, w, h,this);  //ÔÚ¹¹Ôìº¯ÊýÖÐ£¬Íê³ÉÊý¾ÝµÄ»ñÈ¡£¬²¢½âÂë
		}

		if (gifobj==NULL)
		{
			return;
		}

		elementhandle = GIFLayer->AddElement(gifobj); //½« GIF Ìí¼Óµ½Í¼²ãÖÐ£¬±éÀúÍ¼²ã½øÐÐ»æÖÆ,·µ»ØElementIDÖµ

		std::string theState = root["STATE"].asString();
		if(theState.compare("TRUE") == 0)
		{
			//Èç¹ûÊÇTRUE Ôò±íÊ¾ÓÀ¾ÃÏÔÊ¾
		}
		else if(theState.compare("FALSE") == 0)
		{
			//¼ÇÂ¼ÏÔÊ¾µÄ´ÎÊý
			status = root["TIME"].asInt();
		}
		//¶ÁÈ¡Ê±¼ä
		double delay = GIFLayer->GetMinDelay();

		if (delay > 0 && !g_bexistGifTimer)  //¶¨Ê±Æ÷µÄ ³õÊ¼»¯ Îªfalse ¼´²»´æÔÚ
		{
			pp::CompletionCallback cc = callback_factory_.NewCallback(&DrawEngine::GIFTimerProc);
			pp::Module::Get()->core()->CallOnMainThread(delay,cc,g_context);
			g_bexistGifTimer = true;
		}	

		returnInfo = ConstructJsonValue(root["CmdName"].asString(), root["Cmdid"].asInt(), elementhandle, layerhandle, status, -1, -1);  //Ö»ÔÚaddÊ±£¬ÏòÇ°¶Ë·µ»ØÐÅÏ¢
	}
	else if(theOperaotr.compare("DELETE") == 0)	//É¾³ý
	{
		int theLayerHandle = root["Layerid"].asInt(); //ÐÞ¸ÄÎª´Ó Í¼²ãÈÝÆ÷ÖÐÈ¡³öLayer
		CGifLayer * GIFLayer = (CGifLayer *)g_LayerManager.GetLayerByNumber(theLayerHandle); 
		int theElementHandle = root["Elementid"].asInt();
		if(GIFLayer != NULL)
		{
			GIFLayer->DeleteElement(theElementHandle);  
		}
		//CtrlRefresh(true);
	}
	Draw();
	//returnInfo = ConstructJsonValue(root["CmdName"].asString(), root["Cmdid"].asInt(), elementhandle, layerhandle, status, -1, -1);
}

void DrawEngine::GIFTimerProc(int)
{
	return;
	CGifLayer *GIFLayer = NULL;
	int LayerCount = g_LayerManager.GetLayerCount();
	for(int i=0;i<LayerCount;i++)
	{
		CLayer *layer = g_LayerManager.GetLayerByNumber(i+1);
		//¿¼ÂÇµ½ÓÐµÄÍ¼²ãÒÑ¾­É¾³ý£¬¶ø²éÕÒÊÇ forµÄÏßÐÔÑ­»· £¬ËùÒÔ²ÅÓÐÁË ÕâÀï ifÅÐ¶Ï
		if (layer == NULL)
		{
			continue;
		}
		if( layer->m_layerType == VLayerType::vGifLayer)
		{
		   GIFLayer = (CGifLayer *)layer;
		   break;
		}
	}

	if(GIFLayer == NULL || GIFLayer->isEmpty())  //µÝ¹é»æÖÆGIFº¯ÊýµÄ ³ö¿Ú
	{
		//CtrlRefresh(true);
		Draw();
		g_bexistGifTimer = false;
		return;
	}

	g_bOnTime = true;
	Draw();
	
	g_bOnTime = false;
	double delay = GIFLayer->GetMinDelay();
	//ppb_core_interface->CallOnMainThread(delay, (PP_CompletionCallback)g_GIFTimer, 0);  // ÕâÀï»á³öÏÖ µÝ¹éµ÷ÓÃ
	pp::Module::Get()->core()->CallOnMainThread(delay,callback_factory_.NewCallback(&DrawEngine::GIFTimerProc),g_context);
}

void DrawEngine::Render(int32_t result)
{
}

void DrawEngine::ChangeCurrsorStyleByJS(const Json::Value& root, Json::Value& returnInfo)
{
	string cursorStyle = root["TYPE"].asString();
	if(cursorStyle.empty() || !m_cursorStyleID.count(cursorStyle))
		 ppb_MouseCursor_interface->SetCursor(g_instance, PP_MOUSECURSOR_TYPE_GRAB, 0, &theStart);  //ÕÒ²»µ½¾ÍÊ¹ÓÃÄ¬ÈÏµÄÊÖÕÆÐÎ×´
	else
		 ppb_MouseCursor_interface->SetCursor(g_instance, m_cursorStyleID[cursorStyle], 0, &theStart);
	returnInfo = ConstructJsonValue(root["CmdName"].asString(), root["Cmdid"].asInt(), -1, -1, 0, -1, -1);
}

void DrawEngine::ChangeCurrsorStyle(const std::string& theCursor)
{
	ppb_MouseCursor_interface->SetCursor(g_instance, m_cursorStyleID[theCursor], 0, &theStart);
}

void DrawEngine::ChangeCurrsorStyle(const std::string& theContext, PP_Instance& instance)
{
	HGLOBAL global = GlobalAlloc(GMEM_MOVEABLE, theContext.length());
	if(global == NULL)
		return ;
	bool theResult = false;
	void* theBuffer = GlobalLock(global);
	if (theBuffer != NULL)
	{
		memcpy(theBuffer, theContext.c_str(), theContext.length());
		GlobalUnlock(global);
		CComPtr<IStream> pstm = NULL;
		CreateStreamOnHGlobal(global, TRUE, &pstm);
		
		Gdiplus::Bitmap* theImage = Gdiplus::Bitmap::FromStream(pstm);
		HBITMAP hBitMap;
		int m_ImageWidth = theImage->GetWidth();
		int m_ImageHeight = theImage->GetHeight();
		BYTE* m_pdata = NULL;
		m_pdata = new BYTE[m_ImageWidth * m_ImageHeight * 4];
		if(m_pdata != NULL)
		{
			theImage->GetHBITMAP(Color::White, &hBitMap);
			CBitmap* cbmp = CBitmap::FromHandle(hBitMap);

			DWORD size = cbmp->GetBitmapBits(m_ImageWidth * m_ImageHeight * 4, m_pdata);//BGRA

			cbmp->DeleteObject();
			DeleteObject(hBitMap);
			DeleteObject(theImage);
			delete theImage;

			PP_Size theSize;					//Í¼Æ¬¿í¸ß
			theSize.height = m_ImageHeight;
			theSize.width = m_ImageWidth;				
			//add by xuyan
			PPB_ImageData* ppb_ImageData_interface = (PPB_ImageData*) ((pp::Module::Get()->get_browser_interface())(PPB_IMAGEDATA_INTERFACE));
			PPB_MouseCursor* ppb_MouseCursor_interface = (PPB_MouseCursor*) ((pp::Module::Get()->get_browser_interface())(PPB_MOUSECURSOR_INTERFACE));

			PP_ImageDataFormat theFormat = ppb_ImageData_interface->GetNativeImageDataFormat();				//ä¯ÀÀÆ÷Í¼Æ¬¸ñÊ½
			PP_Resource theRes = ppb_ImageData_interface->Create(instance, theFormat, &theSize, PP_TRUE);	//´´½¨¿Õ°×ÇøÓò
			void* theImageData = ppb_ImageData_interface->Map(theRes);										//¿Õ°×ÇøÓòµÄÖ¸Õë
			PP_ImageDataDesc theDesc;
			PP_Bool theDescri = ppb_ImageData_interface->Describe(theRes, &theDesc);						//¿Õ°×ÇøÓòµÄÃèÊö

			//memcpy(theImageData, (void*)m_pdata, m_ImageWidth * m_ImageHeight * 4);							//ÇøÓòÌî³äÍ¼Æ¬Êý¾Ý
			delete[] m_pdata;

			PP_Point theStart;
			theStart.x = 0;
			theStart.y = 0;																					//ÆðÊ¼µã
			PP_Bool theResult = ppb_MouseCursor_interface->SetCursor(instance, PP_MOUSECURSOR_TYPE_POINTER, 0, &theStart);	//Í¼Æ¬Êý¾ÝÌî³äµ½Êó±êÑùÊ½
			
			//delete[] theImageData;
			//ppb_ImageData_interface->Unmap(theRes);
		}
	}
	GlobalFree(global);
}
typedef std::map<int,GeoPos>::value_type pair;
void DrawEngine::Handle(const pair &element)
{
	Vertex verts[6];
	memset(&verts[0], 0, 6 * sizeof(Vertex));
	verts[0].color[3] = 1;
	verts[1].color[3] = 1;
	verts[2].color[3] = 1;
	verts[3].color[3] = 1;
	verts[4].color[3] = 1;
	verts[5].color[3] = 1;
	long pix = 0;
	long piy = 0;
	Projection_to_Pixel(element.second.x_, element.second.y_, pix, piy);

	if(pix > 0 && pix < g_CtrlW && piy < g_CtrlH && piy > 0)
	{
		verts[0].tu = 0;
		verts[0].tv = 1;
		verts[0].loc[0] = pix - m_rendW / 2;
		verts[0].loc[1] = piy + m_rendH / 2;
		verts[0].loc[2] = g_Zlocation;
		mark_triangles.push_back(verts[0]);
		//1
		verts[1].tu = 0;
		verts[1].tv = 0;
		verts[1].loc[0] = pix - m_rendW / 2;
		verts[1].loc[1] = piy - m_rendH / 2;
		verts[1].loc[2] = g_Zlocation;
		mark_triangles.push_back(verts[1]);
		//2
		verts[2].tu = 1;
		verts[2].tv = 1;
		verts[2].loc[0] = pix + m_rendW / 2;
		verts[2].loc[1] = piy + m_rendH / 2;
		verts[2].loc[2] = g_Zlocation;
		mark_triangles.push_back(verts[2]);

		//2
		verts[3].tu = 1;
		verts[3].tv = 1;
		verts[3].loc[0] = pix + m_rendW / 2;
		verts[3].loc[1] = piy + m_rendH / 2;
		verts[3].loc[2] = g_Zlocation;
		mark_triangles.push_back(verts[3]);
		//3
		verts[4].tu = 0;
		verts[4].tv = 0;
		verts[4].loc[0] = pix - m_rendW / 2;
		verts[4].loc[1] = piy - m_rendH / 2;
		verts[4].loc[2] = g_Zlocation;
		mark_triangles.push_back(verts[4]);
		//2
		verts[5].tu = 1;
		verts[5].tv = 0;
		verts[5].loc[0] = pix + m_rendW / 2;
		verts[5].loc[1] = piy - m_rendH / 2;
		verts[5].loc[2] = g_Zlocation;
		mark_triangles.push_back(verts[5]);
	}
}

void DrawEngine::ChangeCurrsorStyle(const CString& ptheCursorPath, PP_Instance& instance)
{
	BYTE* m_pdata;
	HBITMAP hBitMap;
	Bitmap* bmp = Bitmap::FromFile(ptheCursorPath);	
	int m_ImageWidth = bmp->GetWidth();
	int m_ImageHeight = bmp->GetHeight();
	m_pdata = new BYTE[m_ImageWidth * m_ImageHeight * 4];
	if(m_pdata != NULL)
	{
		bmp->GetHBITMAP(Color::White, &hBitMap);
		CBitmap* cbmp = CBitmap::FromHandle(hBitMap);

		DWORD size = cbmp->GetBitmapBits(m_ImageWidth * m_ImageHeight * 4, m_pdata);//BGRA

		cbmp->DeleteObject();
		DeleteObject(hBitMap);
		DeleteObject(bmp);
		delete bmp;
		bmp = NULL;

		PP_Size theSize;					//Í¼Æ¬¿í¸ß
		theSize.height = m_ImageHeight;
		theSize.width = m_ImageWidth;	
	
		PPB_ImageData* ppb_ImageData_interface = (PPB_ImageData*) ((pp::Module::Get()->get_browser_interface())(PPB_IMAGEDATA_INTERFACE));
		PPB_MouseCursor* ppb_MouseCursor_interface = (PPB_MouseCursor*) ((pp::Module::Get()->get_browser_interface())(PPB_MOUSECURSOR_INTERFACE));

		PP_ImageDataFormat theFormat = ppb_ImageData_interface->GetNativeImageDataFormat();				//ä¯ÀÀÆ÷Í¼Æ¬¸ñÊ½
		PP_Resource theRes = ppb_ImageData_interface->Create(instance, theFormat, &theSize, PP_TRUE);	//´´½¨¿Õ°×ÇøÓò
		void* theImageData = ppb_ImageData_interface->Map(theRes);										//¿Õ°×ÇøÓòµÄÖ¸Õë
		PP_ImageDataDesc theDesc;
		PP_Bool theDescri = ppb_ImageData_interface->Describe(theRes, &theDesc);						//¿Õ°×ÇøÓòµÄÃèÊö

		memcpy(theImageData, (void*)m_pdata, m_ImageWidth * m_ImageHeight * 4);							//ÇøÓòÌî³äÍ¼Æ¬Êý¾Ý
		delete[] m_pdata;

		PP_Point theStart;
		theStart.x = 0;
		theStart.y = 0;																					//ÆðÊ¼µã
		PP_Bool theResult = ppb_MouseCursor_interface->SetCursor(instance, PP_MOUSECURSOR_TYPE_CUSTOM, theRes, &theStart);	//Í¼Æ¬Êý¾ÝÌî³äµ½Êó±êÑùÊ½
	}
}

//Ð´Ò»¸ö¼ÆËãÖ¡ÂÊµÄ¹¤¾ßº¯Êý
void DrawEngine::Draw(int32_t result)
{
	glSetCurrentContextPPAPI(g_context); //ÉèÖÃÎªµ±Ç°threadµÄäÖÈ¾ÃèÊö±í
	
	if(! bRegister)
	{
		// ¶ÁÈ¡LICENESEÎÄ¼þ
		std::ifstream theLicenseFileIn;
		theLicenseFileIn.open( "license"); //¶ÁÈ¡±¾µØ´ÅÅÌÎÄ¼þµÄÄÚÈÝ
		const int theBufferSize = 1024;
		char theBuffer[theBufferSize] = {0};
		theLicenseFileIn.read(theBuffer, theBufferSize);
		//Ö¸ÏòÍ¬Ò»¿ébuff
		std::string  theDesc = theBuffer;
		std::string theSource = theBuffer;
		license thelicense;
		thelicense.Decryption(theSource, theDesc); //½âÂë³É¹¦
		theLicenseFileIn.close();
		// ÃÜÎÄÊÇ·ñÕýÈ·
		time_t t = time(0);
		char tmp[64] = {0};
		strftime(tmp, sizeof(tmp), "%Y.%m.%d" ,localtime(&t));
		theSource = tmp; //µ±Ç°Ê±¼ä
		bRegister = thelicense.isRegister(theDesc, theSource);
	}

	if(m_init)
	{      
		/****************³õÊ¼»¯È«¾ÖµÄ×ÅÉ«Æ÷´úÂë***********************/
		initcode();
		InitProgram();
		/************************************************************/
		SetRect(g_CtrlW,g_CtrlH); //ÉèÖÃ²Ã¼ôÇøÓòÒÔ¼°ÏàÓ¦µÄÍ¶Ó°¼°×ª»»matrix g_mvp[16]
		m_init = false;
	}

	glEnable(GL_BLEND);   //ÆôÓÃÉ«²Ê»ìºÏ
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearColor(100/255.0, 100/255.0, 100/255.0, 1);  //ºìÉ«µÄ±³¾°É«
	glClear(GL_COLOR_BUFFER_BIT|GL_STENCIL_BUFFER_BIT); //Çå³ý Ê¹´°¿Ú±ä³É Ò»Ö¡ÐÂµÄ »­²¼
	//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE); es²»Ö§³Ö
	glUseProgram( g_programObj ); //Ê¹ÓÃÕâ¸ö×ÅÉ«Æ÷
	g_MVPLoc = glGetUniformLocation(g_programObj, "a_MVP");
	glUniformMatrix4fv(g_MVPLoc, 1, GL_FALSE, (GLfloat*) g_mpv); //×ª»»¾ØÕóÊ¹ÓÃµÄDrawEngineÀïµÄ
	CLayer *baselayer = g_LayerManager.GetLayer(0);
	if (NULL != baselayer)
	{
		baselayer->Draw(); //»æÖÆµØÍ¼  ²»Í¬ÀàÐÍµÄµ×Í¼
		//baselayer->Draw(g_context); //ÎªÁË´òÓ¡ ÊÇÄÇ¸öÇÐÆ¬
	}
	else
	{
		return;  // µ×Ã»ÓÐ»æÖÆ£¬ÆäËûµÄÍ¼²ã Ò²¾Í²»ÓÃ»æÖÆÁË
	}    

	int LayerCount = g_LayerManager.GetLayerCount();//Ñ¡Ôñ¡°´ò¿ªµØÍ¼¡±ÕâÀïÊÇ0£¬ÒòÎªÃ»ÓÐÌí¼ÓÆäËüÈÎºÎµÄÍ¼²ã¡£
	//聚合图
	for (int i = 0; i<LayerCount; i++)
	{
		CLayer *layer = g_LayerManager.GetLayerByNumber(i + 1); //ÓÐ "Ä§Êý"£¬ÒòÎªµ×Í¼ÊÇµ¥¶ÀÒ»¸ö´æ·Å£¬ÆäËûµÄÍ¼²ã¶¼ÊÇ´Ó1¿ªÊ¼×÷Îªkey´æ·ÅÔÚmapÖÐ
		if (layer && (layer->m_layerType == VLayerType::vClusterLayer))
		{
			layer->Draw();
		}
	}
	//热力图
	for (int i = 0; i<LayerCount; i++)
	{
		CLayer *layer = g_LayerManager.GetLayerByNumber(i + 1); //ÓÐ "Ä§Êý"£¬ÒòÎªµ×Í¼ÊÇµ¥¶ÀÒ»¸ö´æ·Å£¬ÆäËûµÄÍ¼²ã¶¼ÊÇ´Ó1¿ªÊ¼×÷Îªkey´æ·ÅÔÚmapÖÐ
		if (layer && (layer->m_layerType == VLayerType::vHeatMapLayer))
		{
			layer->Draw();
		}
	}

//WFSLine图层
	glUseProgram(g_programObj_Shp);
	glUniformMatrix4fv(g_MVPLoc_Shp, 1, GL_FALSE, (GLfloat*) g_mpv);
	for (int i=0;i<LayerCount;i++)
	{      
		CLayer *layer = g_LayerManager.GetLayerByNumber(i+1);
		if (layer == NULL)
		{
			continue;
		}
	
		if ( (layer->m_layerType == VLayerType::vElementLayer) || (layer->m_layerType == VLayerType::vBufferLayer) || (layer->m_layerType == VLayerType::vWFSPolyLayer) ||(layer->m_layerType == VLayerType::vTempLayer))
		{  

			if(m_IsPoint &&layer->m_layerType == VLayerType::vTempLayer) 
			{
				CElementLayer *penLineLayer = static_cast<CElementLayer *>(layer);
				int Count = penLineLayer->GetCount(); 
				CLineElement *line = NULL;
				if (Count!=0)
				{
					for(int i=0;i<Count-1;++i)
					{
						line = (CLineElement*)(penLineLayer->GetElement(i));
						if (line != NULL)
						{
							line->RenderUseOgles();
						}
					}
					//µ¥¶À»æÖÆ×îºóÒ»ÌõÏß
					line = (CLineElement*)(penLineLayer->GetElement(Count-1));
					double x = 0;
					double y = 0;
					Pixel_to_Projection(point_mouseMove.x, point_mouseMove.y, x, y);
					line->AddPoint(x, y);
					line->RenderUseOgles();
					//É¾³ýÌí¼ÓµÄµã
					line->m_vctPoint.pop_back();
				}
			}
			else
				layer->Draw();  

		}
	}

//绘制WFSPoint	
	glUseProgram( g_programObj ); 
	glUniformMatrix4fv(g_MVPLoc, 1, GL_FALSE, (GLfloat*) g_mpv);
	
	for (int i=0;i<LayerCount;i++)
	{             
		CLayer *layer = g_LayerManager.GetLayerByNumber(i+1); 
		if (layer && (layer->m_layerType == VLayerType::vHotLayer || layer->m_layerType == VLayerType::vWFSPointLayer || layer->m_layerType == vEditLayer) /*&& layer->m_bVisible == true && !g_bmove*/ )
		{
			layer->Draw();
		}
	}
//绘制标绘图标	
	if(b_Lable)
	{
		CMarkerElement rendMark( this);
		rendMark.m_markerstyle = (CMarkerStyle*)(g_vecStyle.find(markstyle)->second);
		mark_triangles.clear(); //Çå¿ÕÖØÐÂ×éµã
		auto iter1 = GeoLable.begin();
		for(;iter1 != GeoLable.end();++iter1)
			Handle(*iter1);

		if( !mark_triangles.empty())
		{
			rendMark.RenderManyUseOgles(mark_triangles);
		}
	}
//GIF图层绘制
	for (int i=0;i<LayerCount;i++)
	{
		CLayer *layer = g_LayerManager.GetLayerByNumber(i+1);
		if (layer == NULL)
		{
			continue;
		}
		if( (layer->m_layerType == VLayerType::vGifLayer) && layer->m_bVisible )
		{
			CGifLayer *GIFLayer = (CGifLayer *)layer;
			if (g_bOnTime)  //µØÍ¼Æ½ÒÆµÄ¹ý³ÌÖÐ¸ö£¬GIF»æÖÆÓÐÎÊÌâ
			{
				GIFLayer->Draw();  //Ê¹ÓÃÕâ¸ö CGifElement *obj->RenderUseOgles(m_minDelay);
			}
			else   //Õý³£»æÖÆGIF£¬»á½øÈëÕâ¸ö·ÖÖ§
			{
				GIFLayer->Draw(1);  //Ê¹ÓÃÕâ¸ö CGifElement *obj->RenderUseOgles(0);
			}
		}
	}

	
	if(g_bDrawBegin && (g_ChooseOperator == enumMapOperator::vPolygonChoose || g_ChooseOperator == enumMapOperator::vLineChoose) && !g_PolygonChoosePointList.empty())
	{
		RenderPolygon(g_PolygonChoosePointList, point_mouseMove); 
	}

	if(g_ChooseOperator == enumMapOperator::vChooseOperator && g_bDrawBegin) 
	{
		
		RenderRect(point_mouseDown, point_mouseMove);
	}
//圈选	
	if(g_ChooseOperator == enumMapOperator::vCircleChoose && g_ChooseCircleElement && g_bDrawBegin)
	{
		g_ChooseCircleElement->RenderUseOgles();
	}
//框标绘
	if(!RectChooseSet.empty()) 
	{
		auto iter1 = RectChooseSet.begin();
		auto iter2 = RectChooseSet.end();
		for(;iter1 != iter2;++iter1)
		{
			if(iter1->second.bVisiable) 
				HandleRender(*iter1);
		}
	}
//画多边形
	if(! PolyChooseSet.empty())
	{
		auto iter1 = PolyChooseSet.begin();
		auto iter2 = PolyChooseSet.end();
		for(;iter1 != iter2;++iter1)
		{
			if(iter1 ->second.bVisiable)
			{
				PP_Point tmp; //¹¹³É±ÕºÏ
				long x,y;
				Projection_to_Pixel(iter1->second.PolyBorder.front().x,iter1->second.PolyBorder.front().y,x,y);
				tmp.x = x;
				tmp.y = y;
				//Ê¹ÓÃÏÂÃæÕâ¶Î´úÂë»æÖÆ²»³öÀ´
				if(!iter1->second.bLoop)
				{
					iter1->second.PolyBorder.push_back(iter1->second.PolyBorder.front());
					iter1->second.bLoop = true;
				}
				vector<MapEngineES_Point> tmpSet;
				copy(iter1->second.PolyBorder.begin(),iter1->second.PolyBorder.end(),std::back_inserter(tmpSet));
				FillPoly(tmpSet);
			}
		}
	}


	glEnable(GL_BLEND);  
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);



	if(!bRegister) //»æÖÆvtronµÄÍ¼±ê
	{
		DrawRegisterPic(g_CtrlW/2, g_CtrlW, g_CtrlH * 0.945, g_CtrlH);  //logoÔÚ´°¿ÚµÄ [ÓÒÏÂ½Ç]
	}

	if(m_Grapics3d_ != NULL)  //Í¼ÐÎ²Å»áÏÔÊ¾ÔÚ´°¿ÚÖÐ£¬½»»»
	{
		pp::CompletionCallback cb = callback_factory_.NewCallback(&DrawEngine::Render); //Õâ¸öRenderÊÇ¸ö¿Õº¯Êý
		m_Grapics3d_->SwapBuffers(cb); //通知 chrome main thread 完成更新,该函数立即返回
	}
}


void DrawEngine::RenderFont(const string &font_content)
{
	if(bCreateFont)
	{
		m_TextImage.Create(256,256,32, CImage::createAlphaChannel); //´´½¨Ò»ÕÅÍ¼Æ¬
		m_pdata = new BYTE[256*256*4];
		m_canvas = m_TextImage.GetDC();//ÄÚ´æÐ¹Â¶
		Graphics graphics(m_canvas); //´´½¨Ò»¸ö»­²¼

		FontFamily fontFamily(L"ËÎÌå");
		FontStyle pFontStyle=FontStyleRegular;
		StringFormat gpStringFormat;
		gpStringFormat.SetAlignment(StringAlignmentNear);
		SolidBrush FontBrush(Color(255, 255, 0, 255));
		Gdiplus::Font font(&fontFamily, 20, pFontStyle, UnitPoint); //×ÖÌå³ß´ç
		PointF pointF(10, 10);
		//½«sting-->WCHAR*
		std::wstring stemp = s2ws(font_content);
		LPCWSTR text = stemp.c_str(); 
		graphics.DrawString(text/*L"ÖÐ¹ú"*/,(INT)wcslen(text), &font, pointF,&gpStringFormat, &FontBrush);//»æÖÆµ½ÄÚ´æ
		//m_TextImage.Save(L"D:\\t.png");
		graphics.ReleaseHDC(m_canvas);

		int line = 256*4;
		for (int y = 0; y < 256; y++)
		{
			for (int x = 0; x < 256; x++)
			{
				Color color;
				unsigned char * pucColor = (unsigned char *)m_TextImage.GetPixelAddress(x, y);
				//rgba
				m_pdata[y*line+x*4+0]=pucColor[2];
				m_pdata[y*line+x*4+1]=pucColor[1];
				m_pdata[y*line+x*4+2]=pucColor[0];
				m_pdata[y*line+x*4+3]=pucColor[3];
			}
		}
		bCreateFont = false;
	}

	//°ó¶¨ÎÆÀí
	if(m_textureid == 0)
		glGenTextures(1, &m_textureid);
	glBindTexture(GL_TEXTURE_2D,m_textureid);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA,  256,256 , 0,GL_RGBA,GL_UNSIGNED_BYTE ,m_pdata);

	//»æÖÆ,
	glUseProgram(g_programObj);
	glUniformMatrix4fv(g_MVPLoc, 1, GL_FALSE, g_mpv);
	
	glBindTexture(GL_TEXTURE_2D,m_textureid); //Ê¹ÓÃÎÆÀí¶ÔÏó
	//ÎÆÀí×ø±ê
	vector<Vertex> theVertexVector;
	Vertex verts[6];
	memset(&verts[0], 0, 6 * sizeof(Vertex));
	//Í¸Ã÷¶È ¾ùÉèÖÃÎª1
	verts[0].color[3] = 1;
	verts[1].color[3] = 1;
	verts[2].color[3] = 1;
	verts[3].color[3] = 1;
	verts[4].color[3] = 1;
	verts[5].color[3] = 1;

	verts[0].tu = 0;
	verts[0].tv = 1;
	verts[0].loc[0] = 400;
	verts[0].loc[1] = 700;
	verts[0].loc[2] = 1.0;
	theVertexVector.push_back(verts[0]);

	verts[1].tu = 0;
	verts[1].tv = 0;
	verts[1].loc[0] = 400;
	verts[1].loc[1] = 400;
	verts[1].loc[2] = 1.0;
	theVertexVector.push_back(verts[1]);

	verts[2].tu = 1;
	verts[2].tv = 1;
	verts[2].loc[0] = 900;
	verts[2].loc[1] = 700;
	verts[2].loc[2] = 1.0;
	theVertexVector.push_back(verts[2]);

	verts[3].tu = 1;
	verts[3].tv = 1;
	verts[3].loc[0] =900;
	verts[3].loc[1] = 700;
	verts[3].loc[2] = 1.0;
	theVertexVector.push_back(verts[3]);

	verts[4].tu = 0;
	verts[4].tv = 0;
	verts[4].loc[0] = 400;
	verts[4].loc[1] = 400;
	verts[4].loc[2] = 1.0;
	theVertexVector.push_back(verts[4]);

	verts[5].tu = 1;
	verts[5].tv = 0;
	verts[5].loc[0] = 900;
	verts[5].loc[1] = 400;
	verts[5].loc[2] = 1.0;
	theVertexVector.push_back(verts[5]);

	if(0 == m_vbo)
		glGenBuffers(1, &m_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, theVertexVector.size() * sizeof(Vertex), (GLvoid*)&theVertexVector[0], GL_STATIC_DRAW);
	glVertexAttribPointer(g_positionLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,loc));
	glEnableVertexAttribArray(g_positionLoc); //ÆôÓÃÕâ¸ö¹¦ÄÜ

	glVertexAttribPointer(g_texCoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,tu));
	glEnableVertexAttribArray(g_texCoordLoc);
	glDrawArrays(GL_TRIANGLES,0,theVertexVector.size());
}

//»æÖÆÈ«¾Ö±äÁ¿
unsigned int m_vboID=0;

//Ê¹ÓÃÄ£°åÀ´ »æÖÆ¶à±ßÐÎ
void DrawEngine::DrawPolygonbystencil()
{
	if (m_vboID==0)
	{
		glGenBuffers(1, &m_vboID);
	}	
	glUseProgram(g_programObj_Shp);  //ÕâÀïÃæµÄ×ÅÉ«Æ÷ ÊÇÔÚMainWnd.cppÖÐµÄvoid InitProgram( void )ÖÐÍê³É³õÊ¼»¯
	glUniformMatrix4fv(g_MVPLoc_Shp, 1, GL_FALSE, (GLfloat*) g_mpv);

	glEnable(GL_STENCIL_TEST);
	glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);    // ½ûÓÃÑÕÉ«»º´æÐ´Èë ÖØÒª£¡£¡ 

	// ÉèÖÃÄ£°å»º´æ²Ù×÷º¯ÊýÎªGL_INVERT
	glStencilFunc(GL_ALWAYS, 0x1, 0x1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_INVERT); 

	glDisable(GL_DEPTH_TEST);    // ½ûÓÃÉî¶È»º´æ ÖØÒª£¡£¡

	Vertex* verts = new Vertex[6]; 
	{
		verts[0].tu = 0;
		verts[0].tv = 0;
		verts[0].loc[0] = 300;
		verts[0].loc[1] = 210;
		verts[0].loc[2] = 1;
		verts[0].color[0] = 255/255.0;
		verts[0].color[1] = 0/255.0;
		verts[0].color[2] = 0/255.0;
		verts[0].color[3] = 255/255.0;

		verts[1].tu = 0;
		verts[1].tv = 0;
		verts[1].loc[0] = 300;
		verts[1].loc[1] = 200;
		verts[1].loc[2] = 1;
		verts[1].color[0] = 255/255.0;
		verts[1].color[1] = 0/255.0;
		verts[1].color[2] = 0/255.0;
		verts[1].color[3] = 255/255.0;

		verts[2].tu = 0;
		verts[2].tv = 0;
		verts[2].loc[0] = 400;
		verts[2].loc[1] = 400;
		verts[2].loc[2] = 1;
		verts[2].color[0] = 255/255.0;
		verts[2].color[1] = 0/255.0;
		verts[2].color[2] = 0/255.0;
		verts[2].color[3] = 255/255.0;

		verts[3].tu = 0;
		verts[3].tv = 0;
		verts[3].loc[0] = 300;
		verts[3].loc[1] = 400;
		verts[3].loc[2] = 1;
		verts[3].color[0] = 255/255.0;
		verts[3].color[1] = 0/255.0;
		verts[3].color[2] = 0/255.0;
		verts[3].color[3] = 255/255.0;

		verts[4].tu = 0;
		verts[4].tv = 0;
		verts[4].loc[0] = 400;
		verts[4].loc[1] = 200;
		verts[4].loc[2] = 1;
		verts[4].color[0] = 255/255.0;
		verts[4].color[1] = 0/255.0;
		verts[4].color[2] = 0/255.0;
		verts[4].color[3] = 255/255.0;

		verts[5].tu = 0;
		verts[5].tv = 0;
		verts[5].loc[0] = 300;
		verts[5].loc[1] = 200;
		verts[5].loc[2] = 1;
		verts[5].color[0] = 255/255.0;
		verts[5].color[1] = 0/255.0;
		verts[5].color[2] = 0/255.0;
		verts[5].color[3] = 255/255.0;
	}
	glBindBuffer(GL_ARRAY_BUFFER,m_vboID);
	glBufferData(GL_ARRAY_BUFFER, 6* sizeof(Vertex), verts,GL_STATIC_DRAW);

	glVertexAttribPointer(g_positionLoc_Shp, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,loc));
	glEnableVertexAttribArray(g_positionLoc_Shp);

	glVertexAttribPointer(g_colorLoc_Shp, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,color));
	glEnableVertexAttribArray(g_colorLoc_Shp);

	glDrawArrays(GL_TRIANGLE_FAN,0,6);//GL_TRIANGLE_STRIP
	delete verts;
	verts = NULL;


	glStencilFunc(GL_NOTEQUAL,0,0x1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
	glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);        // ÖØÒª£¡£¡ 

	glBindBuffer(GL_ARRAY_BUFFER,m_vboID);
	glVertexAttribPointer(g_positionLoc_Shp, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,loc));
	glEnableVertexAttribArray(g_positionLoc_Shp);

	glVertexAttribPointer(g_colorLoc_Shp, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,color));
	glEnableVertexAttribArray(g_colorLoc_Shp);
	glDrawArrays(GL_TRIANGLE_FAN,0,6);

	glDisable(GL_STENCIL_TEST);

}

bool DrawEngine::LoadImageFromResource(CImage* pImage,UINT nResID, LPCWSTR lpTyp)
{
	if ( pImage == NULL) return false;

	pImage->Destroy();

	// ²éÕÒ×ÊÔ´
	//	HMODULE ghmodule = GetModuleHandle(NULL);

	HRSRC hRsrc = ::FindResource(_AtlBaseModule.GetResourceInstance(), MAKEINTRESOURCE(nResID), lpTyp);
	if (hRsrc == NULL) return false;

	// ¼ÓÔØ×ÊÔ´
	HGLOBAL hImgData = ::LoadResource(_AtlBaseModule.GetResourceInstance(), hRsrc);
	if (hImgData == NULL)
	{
		::FreeResource(hImgData);
		return false;
	}

	// Ëø¶¨ÄÚ´æÖÐµÄÖ¸¶¨×ÊÔ´
	LPVOID lpVoid    = ::LockResource(hImgData);

	LPSTREAM pStream = NULL;
	DWORD dwSize    = ::SizeofResource(_AtlBaseModule.GetResourceInstance(), hRsrc);
	HGLOBAL hNew    = ::GlobalAlloc(GHND, dwSize);
	LPBYTE lpByte    = (LPBYTE)::GlobalLock(hNew);
	::memcpy(lpByte, lpVoid, dwSize);

	// ½â³ýÄÚ´æÖÐµÄÖ¸¶¨×ÊÔ´
	::GlobalUnlock(hNew);

	// ´ÓÖ¸¶¨ÄÚ´æ´´½¨Á÷¶ÔÏó
	HRESULT ht = ::CreateStreamOnHGlobal(hNew, TRUE, &pStream);
	if ( ht != S_OK )
	{
		GlobalFree(hNew);
	}
	else
	{
		// ¼ÓÔØÍ¼Æ¬
		pImage->Load(pStream);
		GlobalFree(hNew);
	}

	// ÊÍ·Å×ÊÔ´
	::FreeResource(hImgData);
	pStream->Release();
	pStream = NULL;
	return true;
}

void DrawEngine::DrawRegisterPic(const long& xleft, const long& xright, const long& yleft, const long& yrigth)
{
	if(g_theLicenseStyle == NULL)
	{
		CImage theImage;
		LoadImageFromResource(&theImage, IDB_PNG1, L"PNG");  //ÐèÒª½«PNGÍ¼Æ¬½âÂëÎª Î»Í¼¸ñÊ½£¬²ÅÄÜÓÃÓÚopenGLµÄÎÆÀíÌùÍ¼
		//theImage.LoadFromResource(_AtlBaseModule.GetResourceInstance(), IDB_PNG1);
		//theImage.Load(L"D:\\developFloders\\SmartCity\\main\\HTML5\\Plugins\\Map2d\\nacl_sdk\\vs_addin\\examples\\MapEngineES\\MapEngineES\\VTRON logo.png");
		int height = theImage.GetHeight();
		int width = theImage.GetWidth(); 
		g_theLicenseStyle = new CMarkerStyle();
		CBitmap* theBitmap;
		theBitmap = CBitmap::FromHandle(theImage.Detach());
		g_theLicenseStyle->Init(theBitmap, height, width, true);
		theBitmap->DeleteObject();
		theImage.Destroy();
	}
	// »æÖÆ
	// ¹¹Ôì¶¥µã
	Vertex verts[6];
	memset(&verts[0], 0, 6 * sizeof(Vertex));
	verts[0].color[3] = 1;
	verts[1].color[3] = 1;
	verts[2].color[3] = 1;
	verts[3].color[3] = 1;
	verts[4].color[3] = 1;
	verts[5].color[3] = 1;
	std::vector<Vertex> theVertexVector;
	//0
	verts[0].tu = 0;
	verts[0].tv = 1;  //ÎÆÀí×ø±ê
	verts[0].loc[0] = xleft;
	verts[0].loc[1] = yrigth;
	verts[0].loc[2] = g_Zlocation;
	theVertexVector.push_back(verts[0]);
	//1
	verts[1].tu = 0;
	verts[1].tv = 0;
	verts[1].loc[0] = xleft;
	verts[1].loc[1] = yleft;
	verts[1].loc[2] = g_Zlocation;
	theVertexVector.push_back(verts[1]);
	//2
	verts[2].tu = 1;
	verts[2].tv = 1;
	verts[2].loc[0] = xright;
	verts[2].loc[1] = yrigth;
	verts[2].loc[2] = g_Zlocation;
	theVertexVector.push_back(verts[2]);

	//2
	verts[3].tu = 1;
	verts[3].tv = 1;
	verts[3].loc[0] = xright;
	verts[3].loc[1] = yrigth;
	verts[3].loc[2] = g_Zlocation;
	theVertexVector.push_back(verts[3]);
	//3
	verts[4].tu = 0;
	verts[4].tv = 0;
	verts[4].loc[0] = xleft;
	verts[4].loc[1] = yleft;
	verts[4].loc[2] = g_Zlocation;
	theVertexVector.push_back(verts[4]);
	//2
	verts[5].tu = 1;
	verts[5].tv = 0;
	verts[5].loc[0] = xright;
	verts[5].loc[1] = yleft;
	verts[5].loc[2] = g_Zlocation;
	theVertexVector.push_back(verts[5]);
	// 
	//set what program to use
	glUseProgram( g_programObj );
	glUniformMatrix4fv(g_MVPLoc, 1, GL_FALSE, (GLfloat*) g_mpv);
	CMarkerElement *theElememt=new CMarkerElement(this);
	theElememt->m_markerstyle = g_theLicenseStyle;
	theElememt->RenderManyUseOgles(theVertexVector);
	delete theElememt;
}

void DrawEngine::HandleRender(const ElementType &element)
{
	POINT begin,end;
	Projection_to_Pixel(element.second.begin.x_,element.second.begin.y_,begin.x,begin.y);
	Projection_to_Pixel(element.second.end.x_,element.second.end.y_,end.x,end.y);
	PP_Point be ={begin.x,begin.y};
	PP_Point ed = {end.x,end.y};
	RenderRect(be,ed);
}


void DrawEngine::DeleteChooseRect(const Json::Value& root, Json::Value& returnInfo)
{
	int RectID = root["RectID"].asInt();
	auto iter1 = RectChooseSet.find(RectID);
	auto iter2 = RectChooseSet.end();
	if(iter1 != iter2)
	{
		RectChooseSet.erase(iter1);
		Draw();
	}
}


void DrawEngine::DeleteChoosePoly(const Json::Value& root, Json::Value& returnInfo)
{
	int RectID = root["RectID"].asInt();
	auto iter1 = PolyChooseSet.find(RectID);
	auto iter2 = PolyChooseSet.end();
	if(iter1 != iter2)
	{
		PolyChooseSet.erase(iter1);
		Draw();
	}
}

void DrawEngine::ClearChooseRect(const Json::Value& root, Json::Value& returnInfo)
{
	RectChooseID = 1;
	RectChooseSet.clear();
	Draw();
}

void DrawEngine::ShowChooseRect(const Json::Value& root, Json::Value& returnInfo)
{
	int RectID = root["RectID"].asInt();
	if(RectChooseSet.count(RectID))
	{
		RectChooseSet[RectID].bVisiable = true;
		Draw();
	}
}

void DrawEngine::SearchByPoint(PP_Point& thePoint, GeoPos& theMark,int &LabelID)
{
	auto iter1 = GeoLable.begin();
	auto iter2 = GeoLable.end();
	POINT temp;
	temp.x = thePoint.x;
	temp.y = thePoint.y;
	RECT rect;
	while (iter1 != iter2)
	{
		const GeoPos &theCurrentElement = iter1->second;
		long pix = 0;
		long piy = 0;
		Projection_to_Pixel(theCurrentElement.x_, theCurrentElement.y_, pix, piy);
		//g_CtrlWÊÇ´°¿ÚµÄ ¿í  g_CtrlHÊÇ´°¿ÚµÄ¸ß
		//Èç¹û Õâ¸öµã ÔÚÎÒÃÇµÄ´°¿ÚµÄ¿ÉÊÓ·¶Î§ÄÚ ²Å»æÖÆÕâ¸ö µã
		if( pix > 0 && pix < g_CtrlW && piy <g_CtrlH && piy > 0)
		{
			::SetRect(&rect,pix -m_rendW / 2,piy - m_rendH,pix +m_rendW / 2,piy +m_rendH);
			if(PtInRect(&rect,temp))
			{
				LabelID = iter1->first;
				theMark.setPos(theCurrentElement.x_,theCurrentElement.y_);
			}
		}
		++iter1;
	}//while½áÊø

}