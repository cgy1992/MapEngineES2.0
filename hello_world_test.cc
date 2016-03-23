#include "hello_world_test.h"
#include "GetUrl.h"
#include "DataStruct.h"
#include "stdafxppapi.h"
#include "GlobalRender.h"
#include "HeatMap.h"
#include "ClusterLayer.h"
#include <iterator>

void HelloTutorialInstance::DidChangeView (const pp::Rect& position, const pp::Rect& clip)
{
	if(m_Grapics3d_ != NULL)
		if(!m_Grapics3d_->is_null())
			return;
	{
		//µ±Ç°3d»·¾³µÄÊÊÅä
		m_DrawEngine_->m_CtrlWidth = position.size().width();
		m_DrawEngine_->m_CtrlHeight = position.size().height();
		//3dÉè±¸µÄÊôÐÔ,ÀàËÆÓÚVCÖÐµÄÏñËØ¸ñÊ½
		int32_t attribs[] = 
		{
			PP_GRAPHICS3DATTRIB_ALPHA_SIZE, 8,
			PP_GRAPHICS3DATTRIB_DEPTH_SIZE, 24,
			PP_GRAPHICS3DATTRIB_STENCIL_SIZE, 8, //Ö§³ÖÄ£°å
			PP_GRAPHICS3DATTRIB_SAMPLES, 0,
			PP_GRAPHICS3DATTRIB_SAMPLE_BUFFERS, 0,
			PP_GRAPHICS3DATTRIB_WIDTH, m_DrawEngine_->m_CtrlWidth,
			PP_GRAPHICS3DATTRIB_HEIGHT, m_DrawEngine_->m_CtrlHeight,
			PP_GRAPHICS3DATTRIB_NONE
		};
		//´´½¨3dÉè±¸
		m_Grapics3d_ = new pp::Graphics3D(this, attribs);
		if(!m_Grapics3d_ || m_Grapics3d_->is_null())
			return;
		if(BindGraphics(*m_Grapics3d_) == false)
		{
			delete m_Grapics3d_;
			m_Grapics3d_ = NULL;
			return ;
		}
		//³õÊ¼»¯ OpenGlESµÄ¶«Î÷£¬//ÀàËÆÓÚ HGLRC äÖÈ¾ÃèÊö¾ä±ú
		PP_Resource g_context = m_Grapics3d_->pp_resource();  //²»Í¬µÄÊµÀý£¬Õâ¸öÖµ²»Í¬
		//ÕâÀï¾ÍÊÇ g_context
		m_DrawEngine_->SetContext(m_Grapics3d_->pp_resource()); //±£´æopenGLµÄ»æÍ¼ÉÏÏÂÎÄ
		
		// ²Â²â Õâ¸öº¯Êý ÊÇ²»ÊÇ²»Ö§³Ö invokeÁ½´Î£¿£¿ invokeµÚ¶þ´ÎÊ±£¬µÚÒ»´ÎµÄ»·¾³±ãÊ§Ð§ÁË,[²Â²â¶ÔÁË]
		//glSetCurrentContextPPAPI(m_Grapics3d_->pp_resource()); //±íÊ¾Óëµ±Ç°thread°ó¶¨
		m_DrawEngine_->m_Grapics3d_ = m_Grapics3d_; //Ò»¶¨Òª±£´æ swapbuff²Å»á³öÏÖÍ¼ÐÎ
		m_DrawEngine_->SetWindow(position.size().width(), position.size().height());  //±£´æÕâ¸ö´´½¨´°¿ÚµÄ´óÐ¡
		m_DrawEngine_->Draw(); //add by xuyan
		PostMessage("{\"CmdName\":\"LoadComplete\"}"); //Å×»ØÇ°¶Ë JS¶ÔÏó,
	}
}

//ÊÍ·ÅÄ£¿é×ÊÔ´
HelloTutorialInstance::~HelloTutorialInstance()
{

	if(NULL != m_DrawEngine_)
	{
		glSetCurrentContextPPAPI(m_DrawEngine_->g_context);
		
		//CString InstanceInfo;
		//InstanceInfo.Format(_T("DrawEngine instance[%d] and CurrentContext[%d] Destoryed"),m_DrawEngine_->g_instance,m_DrawEngine_->g_context);
		//gLog.Log(InstanceInfo); //×·¼Óµ½ÎÄ¼þÎ²²¿
		
		delete m_DrawEngine_;
		m_DrawEngine_ = NULL;
	}
	if(m_Grapics3d_ != NULL)
	{
		delete m_Grapics3d_;
		m_Grapics3d_ = NULL;
	}
}

//×é¼þÊ§È¥½¹µã
void HelloTutorialInstance::DidChangeFocus (bool has_focus)
{
	if(has_focus)
	{
		Json::Value test1,test2;
		m_DrawEngine_->Activate(test1,test2); //ÊÂ¼þ´¦Àí´¦£¬°ó¶¨ÏÂ
	}
}

//add by wangyuejiao
bool bChangeMapJS = false;

//°Ñµ¥»÷²Ù×÷³éÏóÎªITools
bool HelloTutorialInstance::HandleInputEvent (const pp::InputEvent &event)
{

	switch (event.GetType())
	{
		case PP_INPUTEVENT_TYPE_UNDEFINED:
			break;
		case PP_INPUTEVENT_TYPE_MOUSEDOWN:   //Êó±ê°´ÏÂÊÂ¼þ
		{
			//¾Û½¹µØÍ¼
			m_bFocusMap_ = true;
			pp::MouseInputEvent theInputEvent(event);
			int32_t theClickCount = theInputEvent.GetClickCount();
			point = theInputEvent.GetPosition();  //Êó±ê °´ÏÂÊ±²¶»ñµÄ [Î»ÖÃÏñËØ×ø±ê]
			m_DrawEngine_->point_mouseDown = point;
		    Json::Value returnInfo;
			returnInfo["CmdName"] = "MOUSEDOWN";
			returnInfo["PosPix"].append(point.x);
			returnInfo["PosPix"].append(point.y);
			double geoX,geoY;
			m_DrawEngine_->Pixel_to_Projection(point.x,point.y,geoX,geoY);
			returnInfo["PosGeo"].append(geoX);
			returnInfo["PosGeo"].append(geoY);
			PostMessage(returnInfo.toStyledString().c_str());
			//×ó»÷ÊÂ¼þÏìÓ¦
			if (theInputEvent.GetButton() == PP_INPUTEVENT_MOUSEBUTTON_LEFT)   //[×ó¼ü] °´ÏÂ
			{
				switch(m_DrawEngine_->g_ChooseOperator)
				{
				case  enumMapOperator::vMark:
					{
						GeoPos pos = {geoX,geoY};
						//²åÈëÐÂµÄÖµ
						m_DrawEngine_->GeoLable.insert(make_pair(m_DrawEngine_->ID,pos));//²åÈë±ê×¢¾ØÐÎ¿ò
						//ÏòJS»ØÅ×ÐÅÏ¢£¬½«Õâ¸öIDÖµÅ×»ØÈ¥
						Json::Value returnInfo;
						returnInfo["CmdName"] = "AddMark";
						returnInfo["LabelID"] = m_DrawEngine_->ID;
						PostMessage(returnInfo.toStyledString().c_str());
						m_DrawEngine_->Draw();
						++m_DrawEngine_->ID;
					}break;
					case enumMapOperator::vPanOperator:  
					{
						if(m_DrawEngine_->m_ChooseType == vNULL)
						{
							m_bMapMove_ = true;
							m_DrawEngine_->ChangeCurrsorStyle("MOVE");  
							point1.x = point.x;
							point1.y = point.y;
							if (1 == theClickCount){  //针对聚合图层
								long layercount = m_DrawEngine_->g_LayerManager.GetLayerCount();
								for (; layercount > 0;--layercount){
									CLayer* theLayer = m_DrawEngine_->g_LayerManager.GetLayerByNumber(layercount);
									if (NULL == theLayer)
									{
										--layercount;
										continue;
									}
									if (theLayer->m_bVisible) 
									{
										if (theLayer->m_layerType == VLayerType::vClusterLayer)
										{
											Json::Value returnPointFid;
											ClusterLayer* curLayer = (ClusterLayer*)theLayer;
											vector<int> pointIdSet;
											pointIdSet = curLayer->searchByPoint(point);
											if (0 != pointIdSet.size())
												ConstructClusterInfo(pointIdSet);
										}
									}
								}
							}
						}
						else if(m_DrawEngine_->m_ChooseType == vSingleChoose && 2 == theClickCount)
						{
						 long layercount = m_DrawEngine_->g_LayerManager.GetLayerCount();
						 while(layercount > 0)
						 {
							CLayer* theLayer = m_DrawEngine_->g_LayerManager.GetLayerByNumber(layercount);
							if(NULL == theLayer) 
							{
								 --layercount;
								 continue;
							}
							if(theLayer->m_bVisible) //Ê×ÏÈ ÊÇ¸ÃÍ¼²ãÒª¿É¼û
							{
								if(theLayer->m_layerType == VLayerType::vWFSPointLayer)
								{
								   Json::Value returnPointFid;
								   CWFSLayer* curLayer = (CWFSLayer*)theLayer;
								   //Õâ¸ösearchÀïÃæ£¬»áÒªÇóµã¿É¼û
								   curLayer->SearchByPoint(point,ChoosePointSet); //ËÑË÷½á¹ûµÄ
								   //¹¹Ôì²¢ÏòÍâÅ×ÐÅÏ¢
								   ConstructChooseInfo(m_DrawEngine_->g_LayerManager.GetLayerHandle(layercount), vSingleChoose, curLayer->get_LayerName(), ChoosePointSet, returnPointFid);
								}
							}
						   --layercount;
						 }
						 //±ê×¢²ãÊ±¿É¼ûµÄ

					  } //¿ÉÒÔË«»÷
					  else if(m_DrawEngine_->m_ChooseType == vSingleChoose && 1 == theClickCount)
					  {
						  if(m_DrawEngine_->b_Lable)
						  {
							  Json::Value returnMark;
							  int LabelID;
							  m_DrawEngine_->SearchByPoint(point,ChooseMark,LabelID);
							  ConstructMarkInfo(vMarkChoose,ChooseMark,LabelID,returnMark);
						  }
					  }

					}break;
					case enumMapOperator::vChooseOperator://Êó±ê[×ó¼ü]µã»÷ºóµÄ[ÇÐÆ¬²Ù×÷]
					{
						m_DrawEngine_->g_bDrawBegin = true;  
						m_DrawEngine_->g_bMoveCatch = true; //Êó±êÒÆ¶¯²¶»ñ ×ø±êµã
						point1.x = point.x;
						point1.y = point.y;
					}break;
					case enumMapOperator::vPolygonChoose: //Êó±ê[×ó¼ü]µã»÷ºóµÄ[ÃæÑ¡²Ù×÷]
					{
						m_DrawEngine_->g_bDrawBegin = true;
						m_DrawEngine_->g_bMoveCatch = true;
						double x = 0;
						double y = 0;
						m_DrawEngine_->Pixel_to_Projection(point.x, point.y, x, y);
						MapEngineES_Point thePoint;
						thePoint.x = x;
						thePoint.y = y;
						m_DrawEngine_->g_PolygonChoosePointList.push_back(thePoint);//´æ´¢µã µØÀí×ø±ê
						POINT curPoint;
						curPoint.x = point.x;
						curPoint.y = point.y;
						m_DrawEngine_->g_PologynPointSet.push_back(curPoint); //±£´æµãµÄ ÏñËØ×ø±ê
					}break;
					case enumMapOperator::vCircleChoose: //×ó¼ü°´ÏÂµÄÈ¦Ñ¡
						{
							if(g_ChooseCircleElement)
							{
								g_ChooseCircleElement->m_visible = true;
								m_DrawEngine_->Pixel_to_Projection(point.x, point.y, g_ChooseCircleElement->m_CenX, g_ChooseCircleElement->m_CenY);
								m_DrawEngine_->g_bDrawBegin = true;
							}
						}break;
					case  enumMapOperator::vLineChoose:
						{
							m_DrawEngine_->g_bDrawBegin = true;
							double x = 0;
							double y = 0;
							m_DrawEngine_->Pixel_to_Projection(point.x, point.y, x, y);
							MapEngineES_Point thePoint;
							thePoint.x = x;
							thePoint.y = y;
							m_DrawEngine_->g_PolygonChoosePointList.push_back(thePoint);//´æ´¢µã µØÀí×ø±ê
							POINT curPoint;
							curPoint.x = point.x;
							curPoint.y = point.y;
							m_DrawEngine_->g_PologynPointSet.push_back(curPoint); //±£´æµãµÄ ÏñËØ×ø±ê
						}break;
					case enumMapOperator::vDrawOperator:  //[Êó±ê×ó¼ü°´ÏÂ ×ÔÓÉ»æ±Ê]
					{
							
						m_DrawEngine_->g_bDrawBegin = true; //¿ØÖÆ[×ÔÓÉ»æ±Ê]»æÖÆµÄÏßÌõÊÇ·ñÏÔÊ¾
						m_DrawEngine_->MouseMove = true;
						isLeftComplete = true;
						if(NULL == penLineLayer)//Ê×ÏÈÔÚÍ¼²ãÈÝÆ÷ÖÐ£¬ÕÒµ½×ÔÓÉ»æ±Ê²ã£¬ÒòÎªÔÚµã»÷¹¤¾ßÀ¸Ê±ÒÑ¾­´´½¨ÁË
						{
							CLayer *tmp = NULL;
							tmp = m_DrawEngine_->g_LayerManager.SearchByLayerType(VLayerType::vTempLayer);
							penLineLayer = static_cast<CElementLayer *>(tmp);
						}
						if(! m_DrawEngine_->m_IsPoint) //·ÇÀëÉ¢Ê±£¬Ã¿´ÎÓÒ»÷¶¼ new line
						{
							CLineElement *line = new CLineElement(m_DrawEngine_->lineR,m_DrawEngine_->lineG,m_DrawEngine_->lineB,m_DrawEngine_->lineA,m_DrawEngine_->lineW,m_DrawEngine_);
							line->m_type = 1;
							penLineLayer->AddElement(line);
							//½«chromeÆË»ñµÄÊó±êµã»÷×ø±ê ×ª»»Îª»æÍ¼µÄÍ¶Ó°×ø±ê
							double x = 0.0;
							double y = 0.0;
							m_DrawEngine_->Pixel_to_Projection(point.x,point.y,x,y);
							line->AddPoint(x,y);
						}
						else // ÀëÉ¢×´Ì¬ÏÂµÄ Á¬µã³ÉÏßµÄ¹¦ÄÜ
						{
						   if(isRightComplete) //ÓÒ¼üÍê³É,´´½¨ÐÂµÄline
						   {
							   isRightComplete = false;
							   CLineElement *line = new CLineElement(m_DrawEngine_->lineR,m_DrawEngine_->lineG,m_DrawEngine_->lineB,m_DrawEngine_->lineA,m_DrawEngine_->lineW,m_DrawEngine_);
							   line->m_type = 1;
							   penLineLayer->AddElement(line);
						   }
						   //¼Óµã
						   int Count = penLineLayer->GetCount(); //µÃµ½¸ÃÍ¼²ãÖÐ ÒªËØElement(»ù´¡Í¼ÔªËØ)µÄ¸öÊý
						   CLineElement *line = NULL;
						   if (Count!=0)
						   {
							   line = (CLineElement*)(penLineLayer->GetElement(Count-1)); //µÃµ½×îºóµÄÒ»¸öElement£¬ÒòÎªÏÂ±ê ´Ó0¿ªÊ¼µÄ
							   if (line != NULL)
							   {
								   double x,y;
								   m_DrawEngine_->Pixel_to_Projection(point.x,point.y,x,y);
								   line->AddPoint(x, y);//line ÖÐ¼ÇÂ¼µÄÊÇ¾­Î³¶È×ø±ê£¬ÔÚ´ËÊÇ²»ÊÇÖ±½ÓÊ¹ÓÃÏñËØ×ø±ê£¿
							   }
						   }
						}

					}break;
					default:
						break;
				}
			}//if ½áÊø
		    else if(theInputEvent.GetButton() == PP_INPUTEVENT_MOUSEBUTTON_RIGHT) //Êó±êÓÒ¼ü °´ÏÂ
			{
				switch(m_DrawEngine_->g_ChooseOperator)			
				{
					case enumMapOperator::vPolygonChoose:  //Êó±ê ÓÒ¼ü°´ÏÂºóµÄ [ÃæÑ¡]²Ù×÷
					{
						////ÃæÑ¡  point
						long x = 0;
						long y = 0;
						/******************modify by xuyan***************************/
						MapEngineES_Point temp;
						POINT thePoint;
						m_DrawEngine_->Pixel_to_Projection(point.x,point.y,temp.x,temp.y);
						m_DrawEngine_->g_PolygonChoosePointList.push_back(temp);  //±£´æÓÃ»§ÓÒ»÷ºóµÄµã(µØÀí×ø±ê)ÓÃÓÚ»æÖÆ
						thePoint.x = point.x;
						thePoint.y = point.y;
						m_DrawEngine_->g_PologynPointSet.push_back(thePoint); //ÓÃÓÚÇóÔÚÇøÓò·¶Î§Ö®ÄÚµÄÈÈµãÐÅÏ¢
						/******************modify by xuyan**********************************************************/
						//ÅÐ¶Ïg_PolygonChoosePointListÊÇ·ñÎª¿Õ
						if (m_DrawEngine_->g_PolygonChoosePointList.size()!=0)
						{
							//µ±µãÊý´óÓÚµÈÓÚ3Ê±£¬»æÖÆºó£¬·ÖÎö£¬ÔÚ·µ»Ø¸øjs
							if (m_DrawEngine_->g_PolygonChoosePointList.size()>=3)
							{
								m_DrawEngine_->Projection_to_Pixel(m_DrawEngine_->g_PolygonChoosePointList.front().x, m_DrawEngine_->g_PolygonChoosePointList.front().y, x, y);
								//point_mouseMove ÊÇÒ»¸öÈ«¾Ö±äÁ¿,½«ËüÉèÖÃÎª¹ì¼£µÄ µÚÒ»¸ö¶¥µã£¬ÕâÑù±ã¿ÉÒÔ»æÖÆÊ×Î²ÏàÁ¬µÄ[¿òÑ¡]								
								//½«×îºóÒ»¸öµãÖÃÎª Ê×¸öÎ»ÖÃµÄµã£¬¹¹³É·â±ÕµÄpoly
								m_DrawEngine_->point_mouseMove.x = x;
								m_DrawEngine_->point_mouseMove.y = y;
								Json::Value theInfo;
								Json::Value returnInfo; //json  Öµ¶ÔÏó
								double xlefttop;
								double ylefttop;
								double xrightbottom;
								double yrightbottom;
								m_DrawEngine_->Pixel_to_Projection(0, 0, xlefttop, ylefttop);
								m_DrawEngine_->Pixel_to_Projection(m_DrawEngine_->g_CtrlW, m_DrawEngine_->g_CtrlH, xrightbottom, yrightbottom);
								theInfo["MapExtern"].append(xlefttop);
								theInfo["MapExtern"].append(ylefttop);
								theInfo["MapExtern"].append(xrightbottom);
								theInfo["MapExtern"].append(yrightbottom);
								for(auto it = m_DrawEngine_->g_PologynPointSet.begin();it != m_DrawEngine_->g_PologynPointSet.end();++it )
								{
									theInfo["PixelExtern"].append(it->x);
									theInfo["PixelExtern"].append(it->y);
								}
								for(std::list<MapEngineES_Point>::iterator it = m_DrawEngine_->g_PolygonChoosePointList.begin();it !=m_DrawEngine_->g_PolygonChoosePointList.end();++it)
								{
									theInfo["GeoExtern"].append(it->x);
									theInfo["GeoExtern"].append(it->y);
								}
								int js = (m_DrawEngine_->g_LayerManager).m_BaseLayer->m_js;

								returnInfo["Level"] = js;
								returnInfo["CmdName"] = "EXTERNCHOOSE";  //ÃæÑ¡
								/**********************************************************************************/
								//¼ÓÈëËÑË÷FIDµÄ¼ÆËã´úÂë µ÷ÓÃCWFSLayerÖÐµÄ·½·¨
								
								//ÕÒµ½ ÈÈµã²ã
								long layercount = m_DrawEngine_->g_LayerManager.GetLayerCount();
								while(layercount > 0)
								{
									
									CLayer* theLayer = m_DrawEngine_->g_LayerManager.GetLayerByNumber(layercount);
									if(NULL == theLayer) continue;
									if(theLayer->m_bVisible) //Õâ¸öÍ¼²ã¿É¼û
									{
										if(theLayer->m_layerType == VLayerType::vWFSPointLayer) // ÊÇµã²ã
										{
											Json::Value returnPointFid;
											CWFSLayer* curLayer = (CWFSLayer*)theLayer;
											curLayer->SearchByPolgyn(m_DrawEngine_->g_PologynPointSet,ChoosePointSet);
											//¹¹Ôì²¢ÏòÍâÅ×ÐÅÏ¢£¬²¢½«½á¹û¼¯ºÏChoosePointSet Çå¿Õ
											//ËäÈ»ÕâÀïÊÇvPolyChoose µ«ÎÒÃÇ¹¹ÔìÊ±¶¼ÊÇ°´ÕÕvRectChoose À´¹¹ÔìÐÅÏ¢
											ConstructChooseInfo(m_DrawEngine_->g_LayerManager.GetLayerHandle(layercount), vRectChoose, curLayer->get_LayerName(), ChoosePointSet, returnPointFid);
											if(!returnPointFid.empty())
												returnInfo["SearchResult"]["FIDInfo"].append(returnPointFid);
										    ChoosePointSet.RemoveAllFeature();
										}
									}
									--layercount;
								}// while½áÊø
								/****************************************************************************************************************/
								GeoPoly tmp;
								tmp.bVisiable = false;
								tmp.bLoop = false;
								if(m_DrawEngine_->m_bPolyShow)
								{
									tmp.bVisiable = true;
								}
								//copy listÈÝÆ÷ÖÐµÄÖµ µ½ vectorÖÐ
								std::copy(m_DrawEngine_->g_PolygonChoosePointList.begin(),m_DrawEngine_->g_PolygonChoosePointList.end(),back_inserter(tmp.PolyBorder));
								//tmp.PolyBorder.push_back(tmp.PolyBorder.front()); //¹¹Ôì±ÕºÏµÄ
								theInfo["RectID"] = m_DrawEngine_->PolyChooseID; //»Øµ÷
								m_DrawEngine_->PolyChooseSet.insert(make_pair(m_DrawEngine_->PolyChooseID++,tmp));
								returnInfo["SearchResult"]["RectPos"] = theInfo;
								PostMessage(returnInfo.toStyledString().c_str());
								m_DrawEngine_->m_brightUp = TRUE; //±íÊ¾ÓÒÒÑ¾­°´ÏÂ£¬Ò»¶¨»á·Å¿ª,´ËÊ±Ë¢ÐÂ»áÌî³äÊµÌå
							}
							else//µ±µãÊýÐ¡ÓÚ3Ê±£¬Çå¿Õµã£¬²¢ÖØ»æ
							{
								m_DrawEngine_->g_PolygonChoosePointList.clear();
								m_DrawEngine_->g_PologynPointSet.clear();
								m_DrawEngine_->g_bDrawBegin = false;
								CtrlRefresh(pp::Point(0, 0), pp::Point(0, 0));
							}
						}
					}break;
					case enumMapOperator::vDrawOperator: //ÓÒ¼ü °´ÏÂºóµÄ ×ÔÓÉ»æ±Ê ÀëÉ¢×´Ì¬ÏÂ
						{
							if(m_DrawEngine_->m_IsPoint && isLeftComplete) //ÀëÉ¢×´Ì¬ÏÂµÄ×ÔÓÉ»æ±Ê
							{
								isLeftComplete = false; //ÊµÏÖ ÏÈ°´×ó¼üºó£¬²Å¿ÉÒÔ°´ÓÒ¼ü
								m_DrawEngine_->g_bDrawBegin = false;
								m_DrawEngine_->MouseMove = false;
								isRightComplete = true; //ÏÂ´Î×ó»÷£¬´´½¨ÐÂµÄline
								int Count = penLineLayer->GetCount(); //µÃµ½¸ÃÍ¼²ãÖÐ ÒªËØElement(»ù´¡Í¼ÔªËØ)µÄ¸öÊý
								CLineElement *line = NULL;
								if (Count!=0)
								{
									line = (CLineElement*)(penLineLayer->GetElement(Count-1)); //µÃµ½×îºóµÄÒ»¸öElement£¬ÒòÎªÏÂ±ê ´Ó0¿ªÊ¼µÄ
									if (line != NULL)
									{
										double x,y;
										m_DrawEngine_->Pixel_to_Projection(point.x,point.y,x,y);
										line->AddPoint(x, y);
									}
								}
								CtrlRefresh(pp::Point(0, 0), pp::Point(0, 0));
								//»Øµ÷ÐÅÏ¢
								if (penLineLayer)
								{
									CStringA theReturPoint;
									//ÏßÊýÄ¿
									int lineCount = penLineLayer->GetCount();
									//Ö»ÏòÇ°¶Ë·µ»Ø×îºóÒ»ÌõÏß
									//for(int ilineCount = 0; ilineCount < lineCount; ilineCount++)
									//{
									CLineElement* theLine = (CLineElement*)penLineLayer->GetElement(lineCount - 1); //×îºóÒ»Ìõline
									//µãÊýÄ¿
									int pointSize = theLine->m_vctPoint.size();
									for(int iPoint = 0; iPoint < pointSize; iPoint++)
									{
										CStringA theCurPoint;
										theCurPoint.Format("%f,%f", theLine->m_vctPoint[iPoint].x, theLine->m_vctPoint[iPoint].y);
										theReturPoint += theCurPoint;
										theReturPoint += ",";
									}
									//}
									theReturPoint = theReturPoint.Left(theReturPoint.GetLength() - 1); //È¥µô×îºóÒ»¸ö¶ººÅ
									CStringA ret;
									ret.Format("{\"CmdName\":\"DRAWPOINT\",\"ColorR\":%d,\"ColorG\":%d,\"ColorB\":%d,\"ColorA\":%d,\"Width\":%d,\"POINTS\":[%s]}",
										m_DrawEngine_->lineR, m_DrawEngine_->lineG, m_DrawEngine_->lineB, m_DrawEngine_->lineA,m_DrawEngine_-> lineW, theReturPoint);
									PostMessage(ret.GetBuffer());
								}
							}
						}break;
					case enumMapOperator::vLineChoose:   // ÓÒ¼ü °´ÏÂºóµÄ ÏßÑ¡
						{
							POINT thePoint;
							thePoint.x = point.x;
							thePoint.y = point.y;
							m_DrawEngine_->g_PologynPointSet.push_back(thePoint); //Ìí¼Ó[ÓÒ»÷]Ê± ×ø±êµã ÓÃÓÚ¼ÆËã
							Json::Value returnInfo;
							Json::Value theInfo;
							
                          //×éµã ½«line±äÎªËÄ±ßÐÎ
							std::vector<POINT> theLinePointSet; // ¹¹ÔìÓÐwidthµÄLine ËùÎ§³ÉµÄÇøÓò[ÀëÉ¢µÄµãÕó]

							for (int i = 0; i < m_DrawEngine_->g_PologynPointSet.size() - 1; i++)   //Ñ­»·ÄÚ²¿ Ò»´ÎÐÔ´¦Àí Á½¸öÏàÁÚµÄµã
							{
								int pWidth = m_DrawEngine_->g_LineChooseRect; //Õâ¸öÖµÐ´ËÀÁË ÊÇ5£¬Ô­À´ÔÚÕâÀï¹¹ÔìÇøÓòÊ±¾Í³ö´íÁË

								long pix1 = m_DrawEngine_->g_PologynPointSet[i].x;
								long piy1 = m_DrawEngine_->g_PologynPointSet[i].y;

								long pix2 = m_DrawEngine_->g_PologynPointSet[i + 1].x;
								long piy2 = m_DrawEngine_->g_PologynPointSet[i + 1].y;

								double x1,y1,x2,y2,x3,y3,x4,y4;

								if(pix1==pix2)
								{
									x1=pix1+pWidth/2.0;x2=pix1-pWidth/2.0;
									x3=pix2+pWidth/2.0;x4=pix2-pWidth/2.0;
									y1=y2=piy1;
									y3=y4=piy2;
								}
								else if(piy1==piy2)
								{
									y1=piy1+pWidth/2.0;y2=piy1-pWidth/2.0;
									y3=piy2+pWidth/2.0;y4=piy2-pWidth/2.0;
									x1=x2=pix1;
									x3=x4=pix2;
								}
								else
								{
									double d=(piy2-piy1)/(pix1-pix2);
									y1=pWidth/2.0/sqrt(d*d+1.0)+piy1;x1=d*(y1-piy1) +pix1;
									y2=-pWidth/2.0/sqrt(d*d+1.0)+piy1;x2=d*(y2-piy1)+pix1;
									y3=pWidth/2.0/sqrt(d*d+1.0)+piy2;x3=d*(y3-piy2)+pix2;
									y4=-pWidth/2.0/sqrt(d*d+1.0)+piy2;x4=d*(y4-piy2)+pix2;
								}

								POINT theCurPoint;
								theCurPoint.x = x1;
								theCurPoint.y = y1;
								theLinePointSet.push_back(theCurPoint);
								theCurPoint.x = x2;
								theCurPoint.y = y2;
								theLinePointSet.push_back(theCurPoint);
								theCurPoint.x = x4;
								theCurPoint.y = y4;
								theLinePointSet.push_back(theCurPoint);
								theCurPoint.x = x3;
								theCurPoint.y = y3;
								theLinePointSet.push_back(theCurPoint);
							}
							long layercount = m_DrawEngine_->g_LayerManager.GetLayerCount();
							while(layercount > 0)
							{
								
								CLayer* theLayer = m_DrawEngine_->g_LayerManager.GetLayerByNumber(layercount);
								if(NULL == theLayer) continue;
								if(theLayer->m_bVisible) //Õâ¸öÍ¼²ã¿É¼û
								{
									if(theLayer->m_layerType == VLayerType::vWFSPointLayer) // ÊÇµã²ã
									{
										Json::Value returnPointFid;
										CWFSLayer* curLayer = (CWFSLayer*)theLayer;
										curLayer->SearchByLine(theLinePointSet,ChoosePointSet);
										//¹¹Ôì²¢ÏòÍâÅ×ÐÅÏ¢£¬²¢½«½á¹û¼¯ºÏChoosePointSet Çå¿Õ
										ConstructChooseInfo(m_DrawEngine_->g_LayerManager.GetLayerHandle(layercount), vRectChoose, curLayer->get_LayerName(), ChoosePointSet, returnPointFid);
										if(! returnPointFid.empty())
											returnInfo["SearchResult"]["FIDInfo"].append(returnPointFid);
										ChoosePointSet.RemoveAllFeature();
									}
								}
								--layercount;
							}// while½áÊø
							returnInfo["CmdName"] = "EXTERNCHOOSE"; //ÏßÑ¡
							PostMessage(returnInfo.toStyledString().c_str());
						}break;
					default:
						break;
				}	
			}
		
		}break; //Êó±ê°´ÏÂ
		case PP_INPUTEVENT_TYPE_MOUSEUP:  // Êó±êËÉ¿ªºó£¬ÏòÇ°¶ËÍ¶µÝÏûÏ¢
		{
			pp::MouseInputEvent theInputEvent(event);
			int32_t theClickCount = theInputEvent.GetClickCount(); 
		    PP_Point theMouseUpPoint = theInputEvent.GetPosition();  //Êó±ê·Å¿ªºó£¬²¶»ñµÄÎ»ÖÃÊó±êÎ»ÖÃÐÅÏ¢
			m_DrawEngine_->point_mouseUp = theMouseUpPoint;
			Json::Value returnInfo;
			returnInfo["CmdName"] = "MOUSEUP";
			returnInfo["PosPix"].append(theMouseUpPoint.x);
			returnInfo["PosPix"].append(theMouseUpPoint.y);
			double geoX,geoY;
			m_DrawEngine_->Pixel_to_Projection(theMouseUpPoint.x,theMouseUpPoint.y,geoX,geoY);
			returnInfo["PosGeo"].append(geoX);
			returnInfo["PosGeo"].append(geoY);
			PostMessage(returnInfo.toStyledString().c_str());

			if (theInputEvent.GetButton() == PP_INPUTEVENT_MOUSEBUTTON_LEFT)
			{
				switch(m_DrawEngine_->g_ChooseOperator)
				{
					case enumMapOperator::vPanOperator:  //×ó¼ü·Å¿ªºóµÄ Æ½ÒÆ²Ù×÷
					{
						if(vNULL == m_DrawEngine_->m_ChooseType)
						{
							m_DrawEngine_->ChangeCurrsorStyle("HAND"); //»Ö¸´ÎªÊÖÕÆÐÎ
							m_bMapMove_ = false; 
							//²éÕÒ³öÊ¸Á¿²ã£¬ÉèÖÃ¸Ã±äÁ¿
							int LayerCount = (m_DrawEngine_->g_LayerManager).GetLayerCount();
							for(int i = 1;i <= LayerCount;++i)
							{
								CLayer *layer = (m_DrawEngine_->g_LayerManager).GetLayerByNumber(i);
								//¿¼ÂÇµ½ÓÐµÄÍ¼²ãÒÑ¾­É¾³ý£¬¶ø²éÕÒÊÇ forµÄÏßÐÔÑ­»· £¬ËùÒÔ²ÅÓÐÁË ÕâÀï ifÅÐ¶Ï
								if (layer == NULL)
								{
									continue;
								}
								//»æÖÆWFS²ãµÄ µã¡¢Ïß¡¢Ãæ   ²»ÖªÎªºÎ·ÅÔÚÒ»Æðºó£¬ÈÈµã Óë Line ²»¿ÉÒÔÍ¬Ê±ÏÔÊ¾,ËûÃÇÊ¹ÓÃµÄ×ÅÉ«Æ÷²»Í¬                                                                                         
								if ( (layer->m_layerType == VLayerType::vElementLayer) || (layer->m_layerType == VLayerType::vBufferLayer) || (layer->m_layerType == VLayerType::vWFSPolyLayer))
								{  
				
									CWFSLayer *eleLayer = (CWFSLayer *)layer;
									(eleLayer->m_LineElementSet_).m_MapMoveComplete = true;
								}
							}
						}
					}break;
					case enumMapOperator::vDrawOperator:  //×ó¼ü·Å¿ªºóµÄ ×ÔÓÉ»æ±Ê
					{
						if(! m_DrawEngine_->m_IsPoint) //·ÇÀëÉ¢×´Ì¬ÏÂµÄ×ÔÓÉ»æÖÆ
						{
							m_DrawEngine_->g_bDrawBegin = false;
							m_DrawEngine_->MouseMove = false;

							 if (penLineLayer)
							 {
								 CStringA theReturPoint;
								 //ÏßÊýÄ¿
								 int lineCount = penLineLayer->GetCount();
								 //Ö»ÏòÇ°¶Ë·µ»Ø×îºóÒ»ÌõÏß
								 //for(int ilineCount = 0; ilineCount < lineCount; ilineCount++)
								 //{
								 CLineElement* theLine = (CLineElement*)penLineLayer->GetElement(lineCount - 1); //×îºóÒ»Ìõline
								 //µãÊýÄ¿
								 int pointSize = theLine->m_vctPoint.size();
								 for(int iPoint = 0; iPoint < pointSize; iPoint++)
								 {
									 CStringA theCurPoint;
									 theCurPoint.Format("%f,%f", theLine->m_vctPoint[iPoint].x, theLine->m_vctPoint[iPoint].y);
									 theReturPoint += theCurPoint;
									 theReturPoint += ",";
								 }
								 //}
								 theReturPoint = theReturPoint.Left(theReturPoint.GetLength() - 1); //È¥µô×îºóÒ»¸ö¶ººÅ
								 CStringA ret;
								 ret.Format("{\"CmdName\":\"DRAWPOINT\",\"ColorR\":%d,\"ColorG\":%d,\"ColorB\":%d,\"ColorA\":%d,\"Width\":%d,\"POINTS\":[%s]}",
									 m_DrawEngine_->lineR, m_DrawEngine_->lineG, m_DrawEngine_->lineB, m_DrawEngine_->lineA,m_DrawEngine_-> lineW, theReturPoint);
								 PostMessage(ret.GetBuffer());
							 }
						 }
					}break;
					case enumMapOperator::vCircleChoose: // [×ó¼ü·Å¿ªºóµÄ È¦Ñ¡]
						{
							if(g_ChooseCircleElement != NULL)
							{
								m_DrawEngine_->g_bDrawBegin = false; //¿ØÖÆÊó±êÒÆ¶¯ÖÐµÄË¢ÐÂ
								Json::Value theInfo;
								//ÕÒµ½ ÈÈµã²ã
								long layercount = m_DrawEngine_->g_LayerManager.GetLayerCount();
								while(layercount > 0)
								{
									
									CLayer* theLayer = m_DrawEngine_->g_LayerManager.GetLayerByNumber(layercount);
									if(NULL == theLayer) continue;
									if(theLayer->m_bVisible) //Õâ¸öÍ¼²ã¿É¼û
									{
										if(theLayer->m_layerType == VLayerType::vWFSPointLayer) // ÊÇµã²ã
										{
											Json::Value returnPointFid;
											CWFSLayer* curLayer = (CWFSLayer*)theLayer;
											curLayer->SearchByCircle(g_ChooseCircleElement->m_CenX, g_ChooseCircleElement->m_CenY, g_ChooseCircleElement->m_Router, ChoosePointSet);// shous
											//¹¹Ôì²¢ÏòÍâÅ×ÐÅÏ¢£¬²¢½«½á¹û¼¯ºÏChoosePointSet Çå¿Õ
											ConstructChooseInfo(m_DrawEngine_->g_LayerManager.GetLayerHandle(layercount), vRectChoose, curLayer->get_LayerName(), ChoosePointSet, returnPointFid);
											if(! returnPointFid.empty())
												theInfo["SearchResult"]["FIDInfo"].append(returnPointFid);
											ChoosePointSet.RemoveAllFeature(); //Çå¿Õ½á¹û¼¯ºÏ
										}
									}
									--layercount;
								}// while½áÊø
								double xlefttop;
								double ylefttop;
								double xrightbottom;
								double yrightbottom;
								m_DrawEngine_->Pixel_to_Projection(0, 0, xlefttop, ylefttop);
								m_DrawEngine_->Pixel_to_Projection(m_DrawEngine_->g_CtrlW, m_DrawEngine_->g_CtrlH, xrightbottom, yrightbottom);
								theInfo["CmdName"] = "EXTERNCHOOSE"; //È¦Ñ¡
								returnInfo["MapExtern"].append(xlefttop);
								returnInfo["MapExtern"].append(ylefttop);
								returnInfo["MapExtern"].append(xrightbottom);
								returnInfo["MapExtern"].append(yrightbottom);
								
								//ÖÐÐÄµã
								returnInfo["GeoExtern"].append(g_ChooseCircleElement->m_CenX);
								returnInfo["GeoExtern"].append(g_ChooseCircleElement->m_CenY);
								long xmidpix;
								long ymidpiy;
								m_DrawEngine_->Projection_to_Pixel(g_ChooseCircleElement->m_CenX, g_ChooseCircleElement->m_CenY, xmidpix, ymidpiy);
								returnInfo["PixelExtern"].append(xmidpix);
								returnInfo["PixelExtern"].append(ymidpiy);
								returnInfo["radius"].append(g_ChooseCircleElement->m_Router);
								theInfo["SearchResult"]["RectPos"] = returnInfo;

								PostMessage(theInfo.toStyledString().c_str());
							}
						}break;
					case enumMapOperator::vChooseOperator: //[×ó¼ü·Å¿ªºó ÇÐÆ¬]
					{
						Json::Value returnInfo;
						int xBe,yBe,xEd,yEd;
						double pxBe,pyBe,pxEd,pyEd; //µØÀí×ø±ê
						int js = (m_DrawEngine_->g_LayerManager).m_BaseLayer->m_js;
						xBe = m_DrawEngine_->point_mouseDown.x;
						yBe = m_DrawEngine_->point_mouseDown.y;
						xEd = m_DrawEngine_->point_mouseUp.x;
						yEd = m_DrawEngine_->point_mouseUp.y;
						m_DrawEngine_->Pixel_to_Projection(xBe,yBe,pxBe,pyBe);
						m_DrawEngine_->Pixel_to_Projection(xEd,yEd,pxEd,pyEd);
					  if(vRectChoose != m_DrawEngine_->m_ChooseType) //
					  {
						returnInfo["PixelExtern"].append(xBe);
						returnInfo["PixelExtern"].append(yBe);  //×óÉÏ½Ç
						returnInfo["PixelExtern"].append(xEd);
						returnInfo["PixelExtern"].append(yBe);

						returnInfo["PixelExtern"].append(xEd);
						returnInfo["PixelExtern"].append(yEd); //ÓÒÏÂ½Ç
						returnInfo["PixelExtern"].append(xBe);
						returnInfo["PixelExtern"].append(yEd);

						returnInfo["ProjectionExtern"].append(pxBe);
						returnInfo["ProjectionExtern"].append(pyBe);  //×óÉÏ½Ç
						returnInfo["ProjectionExtern"].append(pxEd);
						returnInfo["ProjectionExtern"].append(pyBe);

						returnInfo["ProjectionExtern"].append(pxEd);
						returnInfo["ProjectionExtern"].append(pyEd); //ÓÒÏÂ½Ç
						returnInfo["ProjectionExtern"].append(pxBe);
						returnInfo["ProjectionExtern"].append(pyEd);
						returnInfo["Level"] = js;
						returnInfo["CmdName"] = "RANGECHOOSE";  //ÇÐÆ¬·¶Î§
						g_qiepian = true;
						if(m_DrawEngine_->m_bCreateQiePian) //ÊÇ·ñ´´½¨ÐÂµÄÇÐÆ¬ÊµÀý
						{
							returnInfo["Create"] = 1;
							m_DrawEngine_->m_bCreateQiePian = false;
						}
						else
							returnInfo["Create"] = 0;
						PostMessage(returnInfo.toStyledString().c_str());
					  }
					  else  //( m_DrawEngine_->m_ChooseType == vRectChoose)  //[×ó¼ü·Å¿ªºóµÄ ¿òÑ¡]
					  {
						 if(m_DrawEngine_->point_mouseDown == m_DrawEngine_->point_mouseUp) //µ¥Ò»µÄµã»÷,Ö±½ÓÌø³ö
					     {
							 m_DrawEngine_->g_bDrawBegin = false; // Ê¹ÍÏ¶¯µÄÍ¼°¸ÏûÊ§
							 m_DrawEngine_->g_bMoveCatch = false;
							 break;
						 }
						  RECT rect;
						  SetRect(&rect,xBe,yBe,xEd,yEd);  //½«¿ò×éÒ»¸ö¿ò,¿Õ¼äËÑË÷
						  Json::Value theInfo; //ËùÓÐÍ¼²ãµÄÐÅÏ¢
						  //ÕÒµ½ ÈÈµã²ã
						  long layercount = m_DrawEngine_->g_LayerManager.GetLayerCount();
						  while(layercount > 0)
						  {
							CLayer* theLayer = m_DrawEngine_->g_LayerManager.GetLayerByNumber(layercount);
							if(NULL == theLayer) continue;
							if(theLayer->m_bVisible) //Õâ¸öÍ¼²ã¿É¼û
							{
								if(theLayer->m_layerType == VLayerType::vWFSPointLayer) // ÊÇµã²ã
								{
								   Json::Value returnPointFid; //Ã¿Ò»¸öÍ¼²ãµÄÐÅÏ¢
								   CWFSLayer* curLayer = (CWFSLayer*)theLayer;
								   curLayer->SearchByRect(rect,ChoosePointSet);// ËÑË÷±¾Í¼²ãµÄÐÅÏ¢
								   //¹¹Ôì²¢ÏòÍâÅ×ÐÅÏ¢£¬²¢½«½á¹û¼¯ºÏChoosePointSetÇå¿Õ£¬ÏÂÃæµÄÇå¿Õ²Ù×÷ÓÐµã¶àÓà
								   ConstructChooseInfo(m_DrawEngine_->g_LayerManager.GetLayerHandle(layercount), vRectChoose, curLayer->get_LayerName(), ChoosePointSet, returnPointFid);
								   if(!returnPointFid.empty())
									theInfo["SearchResult"]["FIDInfo"].append(returnPointFid);
								  ChoosePointSet.RemoveAllFeature(); //Çå³ýÕâ´ÎÍ¼²ãµÄ½á¹û¼¯ºÏ£¬±ãÓÚÏÂ´ÎÍ³¼Æ
								}
							}
						   --layercount;
						   }// while½áÊø
						  	double xlefttop;
							double ylefttop;
							double xrightbottom;
							double yrightbottom;
							m_DrawEngine_->Pixel_to_Projection(0, 0, xlefttop, ylefttop);
							m_DrawEngine_->Pixel_to_Projection(m_DrawEngine_->g_CtrlW, m_DrawEngine_->g_CtrlH, xrightbottom, yrightbottom);
							theInfo["CmdName"] = "EXTERNCHOOSE"; //¿òÑ¡
							returnInfo["MapExtern"].append(xlefttop);
						    returnInfo["MapExtern"].append(ylefttop);
							returnInfo["MapExtern"].append(xrightbottom);
							returnInfo["MapExtern"].append(yrightbottom);
	
							returnInfo["PixelExtern"].append(xBe);
							returnInfo["PixelExtern"].append(yBe);

							returnInfo["PixelExtern"].append(xEd);
							returnInfo["PixelExtern"].append(yBe);

							returnInfo["PixelExtern"].append(xEd);
							returnInfo["PixelExtern"].append(yEd);

							returnInfo["PixelExtern"].append(xBe);
							returnInfo["PixelExtern"].append(yEd);
//µØÀí×ø±ê
							returnInfo["GeoExtern"].append(pxBe);
							returnInfo["GeoExtern"].append(pyBe);

							returnInfo["GeoExtern"].append(pxEd);
							returnInfo["GeoExtern"].append(pyBe);

							returnInfo["GeoExtern"].append(pxEd);
							returnInfo["GeoExtern"].append(pyEd);

							returnInfo["GeoExtern"].append(pxBe);
							returnInfo["GeoExtern"].append(pyEd);
							//¶Ô±ÈµÄ

						  //½«¿òÑ¡µÄ¾ØÐÎ¿òÖÃÈëmap±£´æ,³õÊ¼»¯²»¿É¼û
							GeoRect chooseRect;
							chooseRect.bVisiable = false; 
							if(m_DrawEngine_->m_bRectShow)
							{
								chooseRect.bVisiable = true;
							}
							chooseRect.begin.x_  = pxBe;
							chooseRect.begin.y_ = pyBe;
							chooseRect.end.x_ = pxEd;
							chooseRect.end.y_ = pyEd;
							//½«Í¼ÐÎ±£´æÏÂÀ´
							m_DrawEngine_->RectChooseSet.insert(make_pair(m_DrawEngine_->RectChooseID,chooseRect));
		
							returnInfo["RectID"] = m_DrawEngine_->RectChooseID++;
							returnInfo["Status"] = m_DrawEngine_->Status;
							theInfo["SearchResult"]["RectPos"] = returnInfo;

							PostMessage(theInfo.toStyledString().c_str());
					   }				
					   m_DrawEngine_->g_bDrawBegin = false; // Ê¹ÍÏ¶¯µÄÍ¼°¸ÏûÊ§
					   m_DrawEngine_->g_bMoveCatch = false;
						//ÏòÇ°¶ËÍ¶µÝÏûÏ¢ wait for complete
					  CtrlRefresh(pp::Point(0, 0), pp::Point(0, 0));  //´ËÊ±¿òÑ¡µÄÍ¼Ïñ»áÏûÊ§
					}break;
				default:
					break;
				}//ÄÚ²¿µÄswitch
			} //if ×ó¼ü ½áÊø
			else if(theInputEvent.GetButton() == PP_INPUTEVENT_MOUSEBUTTON_RIGHT)
			{
				switch(m_DrawEngine_->g_ChooseOperator)
				{
					case enumMapOperator::vPolygonChoose:   //Êó±êÓÒ¼ü ·Å¿ªºóµÄ[ÃæÑ¡]
						{
							m_DrawEngine_->g_bDrawBegin = false; //ÈÃÍ¼°¸ÏûÊ§
							m_DrawEngine_->g_bMoveCatch = false;
							//Ë¢ÐÂÖ®ºó m_brightUp = false
							m_DrawEngine_->g_PolygonChoosePointList.clear(); //¾­Î³¶È
							m_DrawEngine_->g_PologynPointSet.clear();
							CtrlRefresh(pp::Point(0, 0), pp::Point(0, 0));  // ÕâÊ±Ë¢ÐÂºó [ÃæÑ¡]»áÏûÊ§
						}break;
					case enumMapOperator::vLineChoose:
						{
							m_DrawEngine_->g_bDrawBegin = false;
							m_DrawEngine_->g_PolygonChoosePointList.clear(); //¾­Î³¶È
							m_DrawEngine_->g_PologynPointSet.clear();
							CtrlRefresh(pp::Point(0, 0), pp::Point(0, 0));  // ÕâÊ±Ë¢ÐÂºó [ÃæÑ¡]»áÏûÊ§
						}break;
				default:
					break;
				}
			}

		}break;  //Êó±ê¼ü ·Å¿ª µÄcaseÓï¾ä
		case PP_INPUTEVENT_TYPE_MOUSEMOVE:   //Êó±êÒÆ¶¯
		{
			pp::MouseInputEvent theInputEvent(event);
		    point = theInputEvent.GetPosition();  //»ñÈ¡ÒÆ¶¯¹ý³ÌÖÐµÄ µã
			//paintAtCoord(point.x, point.y);
			switch(m_DrawEngine_->g_ChooseOperator)
			{
				case enumMapOperator::vPanOperator:  //Êó±êÒÆ¶¯¹ý³ÌÖÐµÄ Æ½ÒÆ
				{
					if(m_bMapMove_)
					{
						int pixX,pixY;
						pixX = point.x - point1.x;		//ÒÆ¶¯µÄÏñËØÎ»ÖÃ
						pixY = point.y - point1.y;
						point1.x = point.x;
						point1.y = point.y;
					    //add by wangyuejiao
						//bChangeMapJS = false;
						SetWFSLineUseMatrix(false);
						translateMatrix(pixX, pixY, 0, g_mapTranslationMatrix);

						double prX = 0.0;
						double prY=0.0;
						double prZ=0.0;
						int cjs=0;
						long x=0;
						long y = 0;
						CBaseLayer *baselayer = static_cast<CBaseLayer*>(m_DrawEngine_->g_LayerManager.GetLayer(0));

						if (NULL != baselayer)
						{
							baselayer->GetMapCentre(prX,prY,prZ,cjs);//»ñÈ¡µ±Ç°ÖÐÐÄµãÐÅÏ¢
							m_DrawEngine_->Projection_to_Pixel(prX,prY,x,y);//ÖÐÐÄµã¾­Î³¶È×ªÏñËØ:ÊÇ²»ÊÇ¿ÉÒÔÖ±½ÓÓÃg_w/2 g_h/2??
							m_DrawEngine_->Pixel_to_Projection(x-pixX,y-pixY,prX,prY);
							m_DrawEngine_->MoveView(prX,prY,0,cjs); //ÕâÀïÎÒÒªÉ¾³ýµôÔ­ÏÈµÄCtrlRfresh²Ù×÷£¬invoke [instanceÖÐµÄË¢ÐÂ²Ù×÷]
							//CtrlRefresh(pp::Point(0, 0), pp::Point(0, 0));
							//Æ½ÒÆ²Ù×÷£¬ÏòÇ°¶Ë Í¶µÝÐÂµÄµØÍ¼ÖÐÐÄµãÐÅÏ¢
							double x0=0.0;
							double y0=0.0;
							double x1=0.0;
							double y1=0.0;
							m_DrawEngine_->Pixel_to_Projection(0,0,x0,y0);
							m_DrawEngine_->Pixel_to_Projection(m_DrawEngine_->g_CtrlW,m_DrawEngine_->g_CtrlH,x1,y1);
							PostToJs(0,"MOVEVIEW",1,1,0,0,x0,y0,x1,y1,cjs); //·µ»Ø×é¼þ´°¿ÚµÄ [µØÀí×ø±ê]
						}
					}
					//break; 
					if( !m_bMapMove_) 
					{
						int LayerCount = m_DrawEngine_->g_LayerManager.GetLayerCount();
						for (int i=0;i<LayerCount;i++)
						{	
							CLayer *layer = m_DrawEngine_->g_LayerManager.GetLayerByNumber(i+1);
							if (layer == NULL)
							{
								continue;
							}
							if(layer->m_layerType == VLayerType::vWFSPointLayer)
							{
								CWFSLayer* wfsLayer = (CWFSLayer*)layer;
								if(! wfsLayer->m_bVisible) 
								{
									continue; 
								} //Í¼²ã²»¿É¼û£¬¾Í¼ÌÐø
								//wfsLayer->m_PointElementSet_ ÈÈµã·¶Î§
								std::map<long, WFSPointFeatureElement*>::iterator theBegin = wfsLayer->m_PointElementSet_.m_DataSet_.begin();
								std::map<long, WFSPointFeatureElement*>::iterator theEnd = wfsLayer->m_PointElementSet_.m_DataSet_.end();
								POINT temp;
								std::vector<POINT> theVertexVector;
								//±éÀúÃ¿Ò»¸öÈÈµã
								while (theBegin != theEnd)
								{
									WFSPointFeatureElement* theCurrentFeature = theBegin->second;
									long pix = 0;
									long piy = 0;
									m_DrawEngine_->Projection_to_Pixel(theCurrentFeature->x_, theCurrentFeature->y_, pix, piy);
									//g_CtrlWÊÇ´°¿ÚµÄ ¿í  g_CtrlHÊÇ´°¿ÚµÄ¸ß
									//Èç¹û Õâ¸öµã ÔÚÎÒÃÇµÄ´°¿ÚµÄ¿ÉÊÓ·¶Î§ÄÚ ²Å»æÖÆÕâ¸ö µã
									if(theCurrentFeature->m_visible &&pix > 0 && pix < m_DrawEngine_->g_CtrlW && piy < m_DrawEngine_->g_CtrlH && piy > 0)
									{
										temp.x = pix - theCurrentFeature->m_rendW / 2;
										temp.y = piy - theCurrentFeature->m_rendH / 2;
										theVertexVector.push_back(temp);
								
										temp.x = pix + theCurrentFeature->m_rendW / 2;
										temp.y = piy - theCurrentFeature->m_rendH / 2;
										theVertexVector.push_back(temp);

										temp.x = pix + theCurrentFeature->m_rendW / 2;
										temp.y = piy + theCurrentFeature->m_rendH / 2;
										theVertexVector.push_back(temp);

										temp.x = pix - theCurrentFeature->m_rendW / 2;
										temp.y = piy + theCurrentFeature->m_rendH / 2;
										theVertexVector.push_back(temp);
									}
									theBegin++;
								}//while½áÊø
							  //´´½¨¶à±ßÐÎµÄÇøÓò CRgn
								if(theVertexVector.empty())  //¼ÓÈëÕâ¸ö±£»¤£¬±ÜÃâÈ«²¿WFSPoint¶¼²»ÔÚ´°¿ÚÏÔÊ¾·¶Î§ÄÚÊ±£¬
									 break;                        //ÏÂÃæµÄ&(theVertexVector[0])»áÔì³É¿ÕÖ¸Õë·ÃÎÊ£¬µ¼ÖÂcrash	
								int number = theVertexVector.size() / 4;
								int* numberPoloygon = new int[number];
								for(int i = 0; i < number; i++)
									numberPoloygon[i] = 4;
								//memset(numberPoloygon,'',×Ö½ÚÊý); //memsetÊÇ°´×Ö½ÚÀ´¸³Öµ
								CRgn thePologyn;
								bool bResult = thePologyn.CreatePolyPolygonRgn(&(theVertexVector[0]), numberPoloygon, number, WINDING);
								POINT theCurrentPoint;  //Êó±êÒÆ¶¯ ´ËÊ±µÄÎ»ÖÃ
								theCurrentPoint.x = point.x;
								theCurrentPoint.y = point.y;

								if(thePologyn.PtInRegion(theCurrentPoint))
								{
									m_DrawEngine_->ChangeCurrsorStyle("POINT"); // ¸Ä±äÊó±êÑùÊ½ ±äÎªÊÖÖ¸ÐÎ
									m_DrawEngine_->m_ChooseType = vSingleChoose;
									break;
								}
								else 
								{
								  m_DrawEngine_->m_ChooseType = vNULL;
								}
							
							}//if WFSPoint 图层

							//if (layer->m_layerType == VLayerType::vClusterLayer) //如果是聚合图
							//{
							//	ClusterLayer* clusterLayer = (ClusterLayer*)layer;
							//	if (!clusterLayer->m_bVisible)
							//	{
							//		continue;
							//	} 
							//	CRgn pointArea;
							//	clusterLayer->getRgn(pointArea);
							//	POINT currentPos;
							//	currentPos.x = point.x;
							//	currentPos.y = point.y;
							//	if (pointArea.PtInRegion(currentPos)){
							//		m_DrawEngine_->ChangeCurrsorStyle("POINT");
							//		break;
							//	}
							//}

						} //遍历所有图层
			
						if(m_DrawEngine_->b_Lable)
						{
							if(!(m_DrawEngine_->GeoLable.empty()) ) //±ê×¢·Ç¿Õ
							{
								std::vector<POINT> theVertexVector; //¹¹½¨ÇøÓòµÄµã¼¯ºÏ

								auto iter1 = m_DrawEngine_->GeoLable.begin();
								auto iter2 = m_DrawEngine_->GeoLable.end();
								for(;iter1 != iter2; ++iter1)
								{
									const GeoPos &currNode = iter1->second; //µ±Ç°±ê×¢µã
									long pix = 0;
									long piy = 0;
									m_DrawEngine_->Projection_to_Pixel(currNode.x_, currNode.y_, pix, piy);
									if(pix > 0 && pix < m_DrawEngine_->g_CtrlW && piy < m_DrawEngine_->g_CtrlH && piy > 0)
									{
										POINT temp;
										temp.x = pix - m_DrawEngine_->m_rendW / 2;
										temp.y = piy - m_DrawEngine_->m_rendH / 2;
										theVertexVector.push_back(temp);

										temp.x = pix + m_DrawEngine_->m_rendW / 2;
										temp.y = piy - m_DrawEngine_->m_rendH / 2;
										theVertexVector.push_back(temp);

										temp.x = pix + m_DrawEngine_->m_rendW / 2;
										temp.y = piy + m_DrawEngine_->m_rendH / 2;
										theVertexVector.push_back(temp);

										temp.x = pix - m_DrawEngine_->m_rendW / 2;
										temp.y = piy + m_DrawEngine_->m_rendH / 2;
										theVertexVector.push_back(temp);
									}
								} //×éµã½áÊø
								if(theVertexVector.empty())  //¼ÓÈëÕâ¸ö±£»¤£¬±ÜÃâÈ«²¿WFSPoint¶¼²»ÔÚ´°¿ÚÏÔÊ¾·¶Î§ÄÚÊ±£¬
									break;                        //ÏÂÃæµÄ&(theVertexVector[0])»áÔì³É¿ÕÖ¸Õë·ÃÎÊ£¬µ¼ÖÂcrash	
								int number = theVertexVector.size() / 4;
								int* numberPoloygon = new int[number];
								for(int i = 0; i < number; i++)
									numberPoloygon[i] = 4;
								//memset(numberPoloygon,'',×Ö½ÚÊý); //memsetÊÇ°´×Ö½ÚÀ´¸³Öµ
								CRgn thePologyn;
								bool bResult = thePologyn.CreatePolyPolygonRgn(&(theVertexVector[0]), numberPoloygon, number, WINDING);
								POINT theCurrentPoint;  //Êó±êÒÆ¶¯ ´ËÊ±µÄÎ»ÖÃ
								theCurrentPoint.x = point.x;
								theCurrentPoint.y = point.y;
								if(thePologyn.PtInRegion(theCurrentPoint))
								{
									m_DrawEngine_->ChangeCurrsorStyle("POINT"); // ¸Ä±äÊó±êÑùÊ½ ±äÎªÊÖÖ¸ÐÎ
									m_DrawEngine_->m_ChooseType = vSingleChoose;
								}
								else 
								{
									m_DrawEngine_->m_ChooseType = vNULL;
								}

							}//±ê×¢ÈÝÆ÷·Ç¿Õ
						} //ÁÙÊ±±ê×¢²ã ¿É¼û
					} //Íâ²ã Æ½ÒÆ²Ù×÷ÏÂ£¬·ÇÍÏ¶¯µØÍ¼Êó±êÒÆ¶¯
					if(m_DrawEngine_->m_ChooseType != vSingleChoose && m_bMapMove_ == false)
						m_DrawEngine_->ChangeCurrsorStyle("HAND");
				}
				break;
				case enumMapOperator::vChooseOperator: //Êó±êÒÆ¶¯¹ý³ÌÖÐµÄ[ÇÐÆ¬²Ù×÷Óë¿òÑ¡Í¬ÓÃ]
					{
						if(m_DrawEngine_->g_bDrawBegin && m_DrawEngine_->g_bMoveCatch)
						{
							m_DrawEngine_->point_mouseMove = point;
							CtrlRefresh(pp::Point(0, 0), pp::Point(0, 0));
						}
					}break;
				case enumMapOperator::vCircleChoose: //Êó±êÒÆ¶¯ÖÐµÄÈ¦Ñ¡
					{
						if(g_ChooseCircleElement && m_DrawEngine_->g_bDrawBegin)
						{
							long pix = 0;
							long piy = 0;
							m_DrawEngine_->Projection_to_Pixel(g_ChooseCircleElement->m_CenX, g_ChooseCircleElement->m_CenY, pix, piy);
							g_ChooseCircleElement->m_Router = sqrt(1.0 * (pix - point.x) * (pix - point.x) + (piy - point.y) * (piy - point.y)); //ÍâÔ²°ë¾¶
							CtrlRefresh(pp::Point(0, 0), pp::Point(0, 0));
						}
					}break;
				case enumMapOperator::vLineChoose:
					{
						if(m_DrawEngine_->g_bDrawBegin)
						{
							m_DrawEngine_->point_mouseMove = point;  //Êó±êÒÆ¶¯¹ý³ÌÖÐ²úÉúµÄ [µã]
							CtrlRefresh(pp::Point(0, 0), pp::Point(0, 0));
						}
					}break;
				case enumMapOperator::vPolygonChoose: //Êó±êÒÆ¶¯¹ý³ÌÖÐµÄ[ÃæÑ¡]
					{
					 	if(m_DrawEngine_->g_bDrawBegin == true && m_DrawEngine_->g_bMoveCatch)
						{
							m_DrawEngine_->point_mouseMove = point;  //Êó±êÒÆ¶¯¹ý³ÌÖÐ²úÉúµÄ [µã]
							CtrlRefresh(pp::Point(0, 0), pp::Point(0, 0));
						}
					}break;
			   case enumMapOperator::vDrawOperator:  //Êó±êÒÆ¶¯¹ý³ÌÖÐµÄ×ÔÓÉ»æ±Ê
				   {
					   if(! m_DrawEngine_->m_IsPoint) //²»ÊÇÀëÉ¢µÄÁ¬µã
					   {
						   if (penLineLayer && m_DrawEngine_->g_bDrawBegin &&m_DrawEngine_->MouseMove)  //ÔÚµã»÷Êó±ê×ó¼üÊ±£¬Õâ¸ög_bDrawBegin ±»ÉèÖÃÎªtrue
						   {
							   double x = 0.0;
							   double y = 0.0;
							   if(penLineLayer)
							   {
								   int Count = penLineLayer->GetCount(); //µÃµ½¸ÃÍ¼²ãÖÐ ÒªËØElement(»ù´¡Í¼ÔªËØ)µÄ¸öÊý
								   CLineElement *line = NULL;
								   if (Count!=0)
								   {
									   line = (CLineElement*)(penLineLayer->GetElement(Count-1)); //µÃµ½×îºóµÄÒ»¸öElement£¬ÒòÎªÏÂ±ê ´Ó0¿ªÊ¼µÄ
									   if (line != NULL)
									   {
										   m_DrawEngine_->Pixel_to_Projection(point.x,point.y,x,y);
										   line->AddPoint(x, y);//line ÖÐ¼ÇÂ¼µÄÊÇ¾­Î³¶È×ø±ê£¬ÔÚ´ËÊÇ²»ÊÇÖ±½ÓÊ¹ÓÃÏñËØ×ø±ê£¿
										   CtrlRefresh(pp::Point(0, 0), pp::Point(0, 0));  //Ö´ÐÐË¢ÐÂ²Ù×÷
									   }
								   }
							   }//±£»¤»úÖÆ
						   }
					   }
					   else //ÀëÉ¢×´Ì¬ÏÂµÄ Á¬µã³ÉÏß
					   {
						    if (penLineLayer && m_DrawEngine_->g_bDrawBegin &&m_DrawEngine_->MouseMove)
							{
								m_DrawEngine_->point_mouseMove = point; //Ìí¼ÓÒ»¸öµã ÏñËØ×ø±ê
								CtrlRefresh(pp::Point(0, 0), pp::Point(0, 0));
							}

					   }
				   }break;
		    } //ÄÚ²ãswitch½áÊø
		} //ÕâÒ»²ãcase½áÊø
		break;
		case PP_INPUTEVENT_TYPE_MOUSEENTER:
			break;
		case PP_INPUTEVENT_TYPE_MOUSELEAVE:
			m_bFocusMap_ = false;
			break;
		case PP_INPUTEVENT_TYPE_WHEEL:
		{
			SetWFSLineUseMatrix(true);
			SetClusterLayerReload(true); //重新进行 聚合分析
			pp::WheelInputEvent theInputEvent(event);
			PP_FloatPoint thePointClick = theInputEvent.GetTicks(); //»¬ÂÖ ²úÉúµÄ Õý¡¢¸ºÖµ

			double prX = 0.0;
			double prY=0.0;
			double prZ=0.0;
			int cjs=0;
			long x=0;
			long y = 0;
			CBaseLayer *baselayer = static_cast<CBaseLayer*>(m_DrawEngine_->g_LayerManager.GetLayer(0)); //µÃµ½µØÍ¼
			if(!m_bFocusMap_)  //µã»÷Êó±ê×ó¼üÊ±£¬±íÊ¾¾Û½¹µ½´°ÖÐµØÍ¼
			{
				break;
			}
				
			if(NULL != baselayer)
			{
				baselayer->GetMapCentre(prX,prY,prZ,cjs);
				cjs += thePointClick.y; //¹ö¶¯»¬ÂÖ£¬Ôö¼ÓµØÍ¼µÄÏÔÊ¾¼¶±ð
				m_DrawEngine_ -> MoveView(prX,prY,0,cjs);
				int LayerCount = (m_DrawEngine_->g_LayerManager).GetLayerCount();
				for(int i = 1;i <= LayerCount;++i)
				{
					CLayer *layer = (m_DrawEngine_->g_LayerManager).GetLayerByNumber(i);
					//¿¼ÂÇµ½ÓÐµÄÍ¼²ãÒÑ¾­É¾³ý£¬¶ø²éÕÒÊÇ forµÄÏßÐÔÑ­»· £¬ËùÒÔ²ÅÓÐÁË ÕâÀï ifÅÐ¶Ï
					if (layer == NULL)
					{
						continue;
					}
					//»æÖÆWFS²ãµÄ µã¡¢Ïß¡¢Ãæ   ²»ÖªÎªºÎ·ÅÔÚÒ»Æðºó£¬ÈÈµã Óë Line ²»¿ÉÒÔÍ¬Ê±ÏÔÊ¾,ËûÃÇÊ¹ÓÃµÄ×ÅÉ«Æ÷²»Í¬                                                                                         
					if ( (layer->m_layerType == VLayerType::vElementLayer) || (layer->m_layerType == VLayerType::vBufferLayer) || (layer->m_layerType == VLayerType::vWFSPolyLayer))
					{  

						CWFSLayer *eleLayer = (CWFSLayer *)layer;
						(eleLayer->m_LineElementSet_).m_MapMoveComplete = true; //»æÖÆÏßµÄ Ê±ºòÐèÒªÖØÐÂ´«µÝ Êý¾Ý
					}
				}
				double x0=0.0;
				double y0=0.0;
				double x1=0.0;
				double y1=0.0;
				m_DrawEngine_ ->Pixel_to_Projection(0,0,x0,y0);
				m_DrawEngine_ ->Pixel_to_Projection(m_DrawEngine_ ->g_CtrlW,m_DrawEngine_ ->g_CtrlH,x1,y1);
				PostToJs(0,"MOVEVIEW",1,1,0,0,x0,y0,x1,y1,cjs); //·µ»ØµØÍ¼ ·¶Î§
				CtrlRefresh(pp::Point(0, 0), pp::Point(0, 0));
			}
			
		}break;
		case PP_INPUTEVENT_TYPE_RAWKEYDOWN:
			break;
		case PP_INPUTEVENT_TYPE_KEYDOWN:
			break;
		case PP_INPUTEVENT_TYPE_KEYUP:
			break;
		case PP_INPUTEVENT_TYPE_CHAR:
			break;
		default:
			return false;
			break;
	} //×îÍâ²ãµÄ switch(EventType)
	return true;
}

bool HelloTutorialInstance::HandleDocumentLoad (const pp::URLLoader &url_loader)
{
	return true;
}

//ÓÉä¯ÀÀÆ÷ »ñÈ¡Ç°¶Ëjs·¢ËÍµÄÐÅÏ¢£¬×÷Îªº¯ÊýµÄÊäÈë£¬´«µÝ½øÀ´
void HelloTutorialInstance::HandleMessage (const pp::Var& message)
{
	/* 测试当前路径 全局查找NINGBO.log文件
	LogFile log;
	log.on();
	log.Log(TEXT("你好"));
	*/
	
	/* ÌáÊ¾¿ò ÖÐÎÄÏÔÊ¾Õý³£
	CString cs(TEXT("²âÊÔÏÔÊ¾ÖÐÎÄ£¡£¡"));
	MessageBox(NULL,cs,TEXT("±êÌâÀ¸"),MB_OK);
	*/

	std::string theReceiveMessage = message.AsString();
	Json::Value root;
	Json::Reader reader;//Àà·½·¨
 	bool parsingSuccessful = reader.parse(theReceiveMessage.c_str(), root);  //½âÎö Ç°¶Ë·¢À´µÄ JSON×Ö·û´®
	if (parsingSuccessful || 1)  //·Å¿ª£¬ÒòÎªÔÚ½âÎöShowWFSLineLayer·¢ËÍµÄÖ¸ÁîÊ±»á³ö´í
	{
		std::string theCmdName = root["CmdName"].asString();
		string url = root["PicURL"].asString();
		//¹ýÂËGPS
#ifdef FILTERGPS
		if(theCmdName == "MOVEWFSPOINTLAYER" || theCmdName == "REMOVESTYLEFROMWFSPOINT" || theCmdName == "ADDSTYLETOWFSPOINT") return; 
#endif
		
#ifdef DEBUGGPS
		 if(theCmdName == "MOVEWFSPOINTLAYER") //¼ÇÂ¼ÏÂ
		 {
			 if(isFisrtPointMove) //µÚÒ»´ÎÊÕµ½MOVEPointÏûÏ¢
			 {
				 dwStart = GetTickCount();
				 isFisrtPointMove = false;
			 }
			 else
			 {
				 //DWORD dwEnd = GetTickCount();
				 //Ð´ÈëÈÕÖ¾ÎÄ¼þ
				 //time_t now;
				 time(&m_DrawEngine_->now);
				 time(&dwEnd);
				 double interval  = difftime(m_DrawEngine_->now,m_DrawEngine_->pre);
				if(interval >= 5)
				 {
					 char tex_info[100] = {0};
					 strftime(tex_info,20,"%Y-%m-%d %H:%M:%S",localtime(&m_DrawEngine_->now));
					 fwrite(tex_info,strlen(tex_info),1,tex_log);
				 }

				//Í³¼Æ½ÓÊÕµ½GPSÖ¸ÁîµÄÆµÂÊ
				char gps_rate[100];
				memset(gps_rate,0,strlen(gps_rate));
				sprintf(gps_rate,"\nDrawEngine ID[%d]  gps_rate-->%d s\n",m_DrawEngine_->g_context,(dwEnd - dwStart));
				fwrite(gps_rate,strlen(gps_rate),1,tex_log);
				dwStart = dwEnd;
			 }
		 }
#endif

		 Json::Value theReturnInfo; //jsonÖµ¶ÔÏó
		 m_DrawEngine_->CommandParsing(theCmdName, root, theReturnInfo);	//²éÕÒg_FunctionµÄ´¦Àíº¯Êý
	
		 pp::Var theReturnMessage(theReturnInfo.toStyledString());		//·µ»ØÏûÏ¢
		 //Ö±½ÓÔÚÕâÀï Õâ¸öÊÇPPAPIÌá¹©µÄ¿ò¼Ü
		 PostMessage(theReturnMessage);  // ÏòÇ°¶ËjsÍ¶µÝÏûÏ¢
	}
}

void HelloTutorialInstance::CtrlRefresh(pp::Point theBeg, pp::Point theEnd)
{
	MapEngineES_Point beg;
	MapEngineES_Point end;
	beg.x = theBeg.x();
	beg.y = theBeg.y();
	end.x = theEnd.x();
	end.y = theEnd.y();

	m_DrawEngine_->Draw();  // Ö÷»æÖÆº¯Êý

}

void HelloTutorialInstance::Render(int32_t result)
{
}

void HelloTutorialInstance::PostToJs(int cmdid,CStringA cmdname,int status,int layerid,int elementid,int styleid,double x0,double y0,double x1,double y1,int js)
{
	if (m_DrawEngine_->g_LayerManager.GetLayer(0) == NULL)
	{
		//return;
		x0 = 0;
		y0 = 0;
		x1 = 0;
		y1 = 0;
	}
	CStringA ret;  //Õë¶ÔcharµÄ°æ±¾
	ret.Format("{\"Cmdid\":%d,\"CmdName\":\"%s\",\"Status\":%d,\"Layerid\":%d,\"Elementid\":%d,\"Styleid\":%d,\"MapExtern\":[%lf,%lf,%lf,%lf],\"MapLevel\":%d}",cmdid,cmdname,status,layerid,elementid,styleid,x0,y0,x1,y1,js);
	PostMessage(ret.GetBuffer()); //²ÎÊýÀàÐÍ£¬ÒªÇóÊÇconst char*
}


void HelloTutorialInstance::ConstructChooseInfo(const long& layerHandle, const enumChooseType& chooseType, const std::string& layerName, CWFSPointFeatureElementSet& theSet, Json::Value& returnInfo)
{
	if(theSet.m_DataSet_.empty())
		return;
	returnInfo["LayerHandle"] = layerHandle;
	switch(m_DrawEngine_->m_ChooseType)
	{
	case vSingleChoose:
		{
			double xlefttop;
			double ylefttop;
			double xrightbottom;
			double yrightbottom;
			m_DrawEngine_->Pixel_to_Projection(0, 0, xlefttop, ylefttop);
			m_DrawEngine_->Pixel_to_Projection(m_DrawEngine_->g_CtrlW, m_DrawEngine_->g_CtrlH, xrightbottom, yrightbottom);
			returnInfo["MapExtern"].append(xlefttop);
			returnInfo["MapExtern"].append(ylefttop);
			returnInfo["MapExtern"].append(xrightbottom);
			returnInfo["MapExtern"].append(yrightbottom);
			//¹¹ÔìÊý¾ÝÐÅÏ¢
			WFSPointFeatureElement* theFind = theSet.m_DataSet_.begin()->second;
			returnInfo["CmdName"] = "SINGLECHOOSE";
			returnInfo["Location"].append(theFind->x_);
			returnInfo["Location"].append(theFind->y_);
			returnInfo["NAME"] = layerName;
			Json::Value theInfoContext;
			{
				theInfoContext["FID"] = theSet.m_DataSet_.begin()->first;
				theInfoContext["X"] = theFind->x_;
				theInfoContext["Y"] = theFind->y_;
				std::map<std::string, std::string>::iterator theBegin = theFind->m_PontFeature.begin();
				std::map<std::string, std::string>::iterator theEnd = theFind->m_PontFeature.end();
				while(theBegin != theEnd)
				{
					theInfoContext[theBegin->first] = theBegin->second;
					theBegin++;
				}
			}
			returnInfo["Information"] = theInfoContext;
			PostMessage(returnInfo.toStyledString().c_str());  //ÓÉÁíÒ»¸öÊÂ¼þ À´´¥·¢ Å×ÏûÏ¢
		}break;
	case vRectChoose://·µ»ØlayerHandle£¬layername£¬ÒÔ¼°fidµÄ¼¯ºÏ
		{
			if(theSet.m_DataSet_.empty())  //¿òÑ¡µÄÊý¾Ý¼¯ÊÇ¶ÔµÄ
				return;
			returnInfo["LayerHandle"] = layerHandle;
			returnInfo["NAME"] = layerName;
			Json::Value theInfoContext;
			std::map<long, WFSPointFeatureElement*>::iterator theBegin = theSet.m_DataSet_.begin();
			std::map<long, WFSPointFeatureElement*>::iterator theEnd = theSet.m_DataSet_.end();
			while (theBegin != theEnd)
			{
				theInfoContext["FID"] = theBegin->first;  //WFSPointSetÖÐ ÒÔÈÈµãµÄ  FID×÷ÎªmapµÄkey
				theInfoContext["X"] = theBegin->second->x_;
				theInfoContext["Y"] = theBegin->second->y_;
				theBegin++;
				returnInfo["Information"].append(theInfoContext);
			}
		}break;
	default:
		break;
	}
	theSet.RemoveAllFeature();// ½«½á¹û¼¯ºÏÇå¿Õ£¬¹©ÏÂÒ»´ÎÊ¹ÓÃ
}

void HelloTutorialInstance::ConstructMarkInfo(const enumChooseType& chooseType,const GeoPos& ChooseMark,const int& LabelID,Json::Value& returnInfo)
{
	double xlefttop;
	double ylefttop;
	double xrightbottom;
	double yrightbottom;
	m_DrawEngine_->Pixel_to_Projection(0, 0, xlefttop, ylefttop);
	m_DrawEngine_->Pixel_to_Projection(m_DrawEngine_->g_CtrlW, m_DrawEngine_->g_CtrlH, xrightbottom, yrightbottom);
	returnInfo["MapExtern"].append(xlefttop);
	returnInfo["MapExtern"].append(ylefttop);
	returnInfo["MapExtern"].append(xrightbottom);
	returnInfo["MapExtern"].append(yrightbottom);

	returnInfo["CmdName"] = "SINGLECHOOSE";
	returnInfo["Location"].append(ChooseMark.x_);
	returnInfo["Location"].append(ChooseMark.y_);
	returnInfo["LabelID"] = LabelID;
	returnInfo["NAME"] = "vMarkChoose";
	PostMessage(returnInfo.toStyledString().c_str());
}

void HelloTutorialInstance::ConstructClusterInfo(std::vector<int> pointSet)
{
	Json::Value returnInfo;

	double xlefttop;
	double ylefttop;
	double xrightbottom;
	double yrightbottom;
	m_DrawEngine_->Pixel_to_Projection(0, 0, xlefttop, ylefttop);
	m_DrawEngine_->Pixel_to_Projection(m_DrawEngine_->g_CtrlW, m_DrawEngine_->g_CtrlH, xrightbottom, yrightbottom);
	returnInfo["MapExtern"].append(xlefttop);
	returnInfo["MapExtern"].append(ylefttop);
	returnInfo["MapExtern"].append(xrightbottom);
	returnInfo["MapExtern"].append(yrightbottom);
	returnInfo["CmdName"] = "CLUSTERPOINT";
	returnInfo["PointSet"].resize(0);
	if (pointSet.size() != 0){
		for (const int& item : pointSet){
			returnInfo["PointSet"].append(item);
		}
	}
	PostMessage(returnInfo.toStyledString().c_str());
}

void HelloTutorialInstance::SetWFSLineUseMatrix(bool flag){

	int LayerCount = m_DrawEngine_->g_LayerManager.GetLayerCount();
	for (int i=0;i<LayerCount;i++){
		CLayer *layer = m_DrawEngine_->g_LayerManager.GetLayerByNumber(i+1);
		if (layer == NULL)
		{
			continue;
		}
		if(layer->m_layerType == VLayerType::vElementLayer){
			CWFSLayer* wfsLayer = (CWFSLayer*)layer;
			(wfsLayer->m_LineElementSet_).m_ChangeMapJS_ = flag;
		}
	}
}

void HelloTutorialInstance::SetClusterLayerReload(bool flag){
	int LayerCount = m_DrawEngine_->g_LayerManager.GetLayerCount();
	for (int i = 0; i<LayerCount; i++){
		CLayer *layer = m_DrawEngine_->g_LayerManager.GetLayerByNumber(i + 1);
		if (layer == NULL)
		{
			continue;
		}
		if (layer->m_layerType == VLayerType::vClusterLayer){
			ClusterLayer* clusterLayer = (ClusterLayer*)layer;
			clusterLayer->setReloadData(true);
		}
	}
}