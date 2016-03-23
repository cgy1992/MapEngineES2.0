#include "WFSLayer.h"

FILE *ver_log = NULL;  //���Ը���Ϊ ��־��

bool CWFSLayer::GetData()
{
	if(m_UrlPath_ != "")
	{
		// get xml
		std::string theUrl = m_UrlPath_;
		std::string::size_type pos = theUrl.find("deegree");
		(pos == theUrl.npos)? (this->m_serverType = GEO):(this->m_serverType = DEEGREE);
		std::string theText = "";
		//�������code����render��Line����ʧ
		m_CurlConnect_->GetDataFromByHttp(theUrl, theText); //��http�����ݣ��õ����ǵ��wfs XML�ļ�����Ϣ
		if (m_layerType == vWFSPointLayer) 
		{
			// remove all points
			m_PointElementSet_.RemoveAllFeature();
			return (_dataLoadComplete = LoadDataByString(theText, m_PointElementSet_));  //�÷��� ���ݲ����Ĳ�ͬ  overload
		}
		else if(m_layerType == vElementLayer) //��ȡLine����Ϣ ������������������ڴ�й¶
		{
			return (_dataLoadComplete = LoadDataByString(theText, m_LineElementSet_)); //����������� HEAP CORRUPTION DETECTED
		}
		else   // ��ȡ�����Ϣ
		{
		  m_PolyElementSet_.RemoveAllFeature();
		  return (_dataLoadComplete = LoadDataByString(theText, m_PolyElementSet_));
		}		
	}
}

bool CWFSLayer::TGetData()
{
	std::string theUrl = m_UrlPath_;
	std::string theText = "";
	//�������code����render��Line����ʧ
	m_CurlConnect_->GetDataFromByHttp(theUrl, theText);
	/*if(!threadRemove)*/
		m_LineElementSet_.RemoveAllFeature(); 
	return LoadDataByString(theText, m_LineElementSet_); //����������� HEAP CORRUPTION DETECTED
}

//���ߵ� ��ȼ���ɫ����
void CWFSLayer::put_Style(const int& width, const int& rcolor, const int& gcolor, const int& bcolor, const int& acolor)
{
	m_LineWidth_ = width;
	m_ColorR_ = rcolor;
	m_ColorG_ = gcolor;
	m_ColorB_ = bcolor;
	m_ColorA_ = acolor;
}
/*******************************************��ȡWFS�е� �㡢�ߡ����ļ���Ϣ*********************************************/
//�����ȡ Poly��������Ϣ author by xuyan 2014/8/26 ����һ�����ʱ�׼GeoServer�ӿ�
bool CWFSLayer::LoadDataByString(const std::string& theText, CWFSPolyFeatureSet& theSet)
{
	//load the file
	TiXmlDocument pDoc;
	pDoc.Parse(theText.c_str());
	TiXmlElement* pRootElet = pDoc.RootElement();  //ָ�� wfs:Feature
	if(NULL == pRootElet)
		return false;
	CWFSPolyFeature* theCurrentFeature = NULL;  //����һ��Poly λ�ü�������������
	std::string thePointListValue = ""; //�����XML����ȡ�� Poly�ϵ���ɢ���������Ϣ
	//part1:����GEO
	TiXmlElement * i = NULL;
	if(this->m_serverType == DEEGREE){
		i = pRootElet->FirstChildElement();
	}else if(this->m_serverType == GEO){
		i = pRootElet->FirstChildElement()->NextSiblingElement(); //���˵���һ�� gml:boundedBy
	}

	for(; i != NULL; i = i->NextSiblingElement())
	{
		TiXmlElement* pFeature = i->FirstChildElement();  // ָ�� app:poly  
		std::string theLayerName = pFeature->Value(); //��ȡ app��poly���ַ���
		if(theLayerName.compare("app:" + m_LayerName_) == 0)
		{
			theCurrentFeature = new CWFSPolyFeature();
			std::string featureid = pFeature->Attribute("gml:id");
			//part2: ����GEO
			int ifeapos;
			if(this->m_serverType == DEEGREE)
				ifeapos = featureid.find_last_of('_');  //modify by xuyan
			else
				ifeapos = featureid.find_last_of('.');
			featureid = featureid.substr(ifeapos + 1, featureid.length());
			long theFid = atoi(featureid.c_str());


			TiXmlElement *pField = pFeature->FirstChildElement(); //ָ�� app:geometry	
			//part3:����GEO
			if(this->m_serverType == GEO)
				pField = pField->NextSiblingElement(); //���˵�<gml:boundedBy>
			//app:poly�µ�5���ӱ�ǩ geometry,id,name,description,remark
			for(;pField != NULL; pField = pField->NextSiblingElement())
			{
				std::string theFieldName = pField->Value();
				if(0 == theFieldName.compare("app:geometry"))  //������ӽڵ� ��������
				{
					TiXmlElement *pGeo = pField->FirstChildElement(); // ָ�� gml:Polygon
					if(0 == std::string(pGeo->Value()).compare("gml:Polygon"))
					{
					  TiXmlElement *pGeo1 = pGeo->FirstChildElement();
					  if(0 == std::string(pGeo1->Value()).compare("gml:exterior"))
					  {
					     TiXmlElement *pGeo2 = pGeo1->FirstChildElement();
						 if(0 == std::string(pGeo2->Value()).compare("gml:LinearRing"))
						 {
							 TiXmlNode *theNode = pGeo2->FirstChildElement(); //ָ�� gml:posList
							 if(theNode)
							 {
								TiXmlNode* theCurNode = theNode->FirstChild(); //ָ�� Poly ����ɢ�������ֵ
								if(theCurNode)
								{
									try
									{
										thePointListValue = theCurNode->Value();
									/*****************�������ͽ�stirng ���տո���split ��ȡ���������ֵ*************/
										std::string::size_type ipos = thePointListValue.find(" ");
										MapEngineES_Point theCurrentPointLocation; //�����XMLֱ��ȡ���� ��γ����Ϣ
										while(ipos != std::string::npos)
										{
											std::string y = thePointListValue.substr(0, ipos); //39.839400
											thePointListValue = thePointListValue.substr(ipos + 1, thePointListValue.length());
											ipos = thePointListValue.find(" ");
											if(ipos == std::string::npos)
												ipos = thePointListValue.length();
											std::string x = thePointListValue.substr(0, ipos); //119.402940
											theCurrentPointLocation.x = atof(x.c_str());
											theCurrentPointLocation.y = atof(y.c_str());
											theCurrentFeature->m_PointList_.push_back(theCurrentPointLocation); //��ȡ��һ�� ������Ϣ
											if(ipos + 1 < thePointListValue.length())
												thePointListValue = thePointListValue.substr(ipos + 1, thePointListValue.length());
											ipos = thePointListValue.find(" ");
										}
										//m_LineElementSet_.m_DataSet_[theFid] = theCurrentFeature; 
									}
									catch (CMemoryException* e)
									{				
									}
									catch (CFileException* e)
									{
									}
									catch (CException* e)
									{
									}
								}
							 }
						 }
					  }
					}
				}
				else
				{
					std::string theValueString = "";
					TiXmlNode* theNode = pField->FirstChild();  //�õ���ǩ �µ� ֵ  xy
					if(theNode)
					{
						theValueString = theNode->Value();
					}
					//��WFSPointFeatureElement�� ���� ["app:id",1000033_1_0_0]
					theCurrentFeature->m_PolyFeature[theFieldName] = theValueString;
				}
			}//�ڲ�forѭ��
			m_PolyElementSet_.m_DataSet_[theFid] = theCurrentFeature;  //��Poly��λ�����ݱ�����map������
		}//�б��Ƿ��� app:poly
	}//���forѭ�������������е� app:geometry �����е�Poly
	
	return true;
}
//modify by xuyan 2014/7/24 WFSLine��Ϣ
bool CWFSLayer::LoadDataByString(const std::string& theText, CWFSLineFeatureSet& theSet)
{
	TiXmlDocument pDoc;
	pDoc.Parse(theText.c_str());

	TiXmlElement* pRootElet = pDoc.RootElement();  //ָ�� wfs:FeatureCollection
	if(NULL == pRootElet)
	 return false;
	CWFSLineFeature* theCurrentFeature = NULL;  //����һ��Line λ�ü�������������
	std::string thePointListValue = ""; //�����XML����ȡ�� line�ϵ���ɢ���������Ϣ
    
	TiXmlElement * i = NULL;
	if(this->m_serverType == DEEGREE){
		i = pRootElet->FirstChildElement();
	}else if(this->m_serverType == GEO){
		i = pRootElet->FirstChildElement()->NextSiblingElement(); //���˵���һ�� gml:boundedBy
	}
	
   //���for���ǵ������е�gml::featureMember  i �� ָ�� gml��featureMember
   for( ; i != NULL; i = i->NextSiblingElement())
	{
		TiXmlElement* pFeature = i->FirstChildElement();  // ָ�� app:road  
		std::string theLayerName = pFeature->Value(); //��ȡ app��road���ַ���
	    //�����m_LayerName_����addWFSLineʱ���������ʼ��theWFSLayer->put_LayerName(root["LayerName"].asString());
		if(theLayerName.compare("app:" + m_LayerName_) == 0) //�����app:road֤������һ��·
	    {
				theCurrentFeature = new CWFSLineFeature();  //����һ�����ڴ�� Line ��λ����Ϣ �Ķ��� xuyan
				std::string featureid = pFeature->Attribute("gml:id");
				//int ifeapos = featureid.find('_');
				int ifeapos;
				if(this->m_serverType == DEEGREE)
					ifeapos = featureid.find_last_of('_');  //modify by xuyan
				else
					ifeapos = featureid.find_last_of('.');

				featureid = featureid.substr(ifeapos + 1, featureid.length());
				long theFid = atoi(featureid.c_str()); //��ȡ��Line��ID�ţ����潫��ʹ�����ID��Ϊkey�������Line��������

				TiXmlElement *pField = pFeature->FirstChildElement(); //ָ�� app:geometry
				if(this->m_serverType == GEO)
					pField = pField->NextSiblingElement(); //���˵�<gml:boundedBy>
				//road �µ���� �ӽڵ� ����Ҫ����
				for(;pField != NULL; pField = pField->NextSiblingElement())
				{

                   std::string theFieldName = pField->Value();
                   if(0 == theFieldName.compare("app:geometry"))  //������ӽڵ� ��������
                   {
                   	 TiXmlElement *pGeo = pField->FirstChildElement(); //ָ�� app:LineString
                   	 if(0 == std::string(pGeo->Value()).compare("gml:LineString"))
                   	 {
                   	 	TiXmlNode *theNode = pGeo->FirstChildElement(); //ָ�� app:posList
                   	 	if(theNode)
                   	 	{
                   	 		TiXmlNode* theCurNode = theNode->FirstChild(); //ָ�� Line ����ɢ�������ֵ
                   	 		if(theCurNode)
                   	 		{
								try
								{
									thePointListValue = theCurNode->Value();
								/*****************�������ͽ�stirng ���տո���split ��ȡ���������ֵ*************/
									std::string::size_type ipos = thePointListValue.find(" ");
									MapEngineES_Point theCurrentPointLocation;
									while(ipos != std::string::npos)
									{
										std::string y = thePointListValue.substr(0, ipos); //39.839400
										thePointListValue = thePointListValue.substr(ipos + 1, thePointListValue.length()); // +1 ��Ϊ�������ո�
										ipos = thePointListValue.find(" ");
										if(ipos == std::string::npos)
											ipos = thePointListValue.length();
										std::string x = thePointListValue.substr(0, ipos); //119.402940
										theCurrentPointLocation.x = atof(x.c_str());
										theCurrentPointLocation.y = atof(y.c_str());
										theCurrentPointLocation.fid = theFid;
										theCurrentFeature->m_PointList_.push_back(theCurrentPointLocation); //�����Line����ɢ��Point ��������

										if(ipos + 1 < thePointListValue.length())
											thePointListValue = thePointListValue.substr(ipos + 1, thePointListValue.length());
										ipos = thePointListValue.find(" ");
									}
								}
								catch (CMemoryException* e)
								{				
								}
							    catch (CFileException* e)
								{
								}
								catch (CException* e)
								{
								}
                   	 		}
                   	 	}
                   	 }
                   } 
                   else  //������ �ĸ��ӽڵ�app:id app:name app:description app:remark ����ʽ��ͬ ����[��ǩ��,ֵ]���ֱ��淽ʽ
                   {
						std::string theValueString = "";
						TiXmlNode* theNode = pField->FirstChild();  //�õ���ǩ �µ� ֵ  xy
						if(theNode)
						{
							theValueString = theNode->Value();
						}
						//��WFSPointFeatureElement�� ���� ["app:id",1000033_1_0_0]
						theCurrentFeature->m_LineFeature[theFieldName] = theValueString;
                   }
 
				}
		      //������ Line������������
			 if(m_LineElementSet_.m_thread) //�����thread����
              m_LineElementSet_.m_Thread_DataSet_[theFid] = theCurrentFeature;
			 else
			  m_LineElementSet_.m_DataSet_[theFid] = theCurrentFeature;
	    }//�����Ǽ���һ�� if���� ȷ������� <app:road>
	}//���forѭ�� �������е�gml:featureMember(�����ǩ���溬��һ��Line)
	return true;
}

//��ʼ���� point�� XML�ļ�  xy
bool CWFSLayer::LoadDataByString(const std::string& theText, CWFSPointFeatureElementSet& theSet)
{
	
	TiXmlDocument pDoc;
	pDoc.Parse(theText.c_str()); //����DOM����

	TiXmlElement* pRootElet = pDoc.RootElement(); //wfs:FeatureCollection
	if(pRootElet == NULL)
		return false;

	if(this->m_serverType == DEEGREE){
		ParsePointXML(pRootElet->FirstChildElement(),theSet);
	}else if(this->m_serverType == GEO){
		//���˵� ��һ�� <gml:boundedBy>
		ParsePointXML(pRootElet->FirstChildElement()->NextSiblingElement(),theSet);
	}

	return true;
}
void CWFSLayer::ParsePointXML(TiXmlElement * first,CWFSPointFeatureElementSet& theSet){

	WFSPointFeatureElement* theCurrentFeature = NULL;
	for(TiXmlElement * i = first; i != NULL; i = i->NextSiblingElement())
	{
		//gml::featureMember
		TiXmlElement* pFeature = i->FirstChildElement(); //app:bus
		//app:video û������ͬ�����ӱ�ǩ ����Ҫforѭ�� xy��line�������޸�
		for (; pFeature != NULL; pFeature = pFeature->NextSiblingElement())
		{
			//app:hzvideo
			std::string theLayerName = pFeature->Value();  //app:video����ӱ�ǩ  xy
			if(theLayerName.compare("app:" + m_LayerName_) == 0)  //�ڽ���js������cmdʱ���Ѿ���ɸ�ֵ
			{
				if("app:bus" == theLayerName)
					m_PointElementSet_.m_IsBus = true;
				theCurrentFeature = new WFSPointFeatureElement();  //������һ���ȵ�Ԫ��
				std::string featureid = pFeature->Attribute("gml:id"); //APP_VIDEO_418 ��ȡ��ǩ�е�����ֵ
				int ifeapos,theFid;
				if(this->m_serverType == DEEGREE){
					ifeapos = featureid.find_last_of('_');
				}else if(this->m_serverType == GEO){
					ifeapos = featureid.find_last_of('.');
				}
				featureid = featureid.substr(ifeapos + 1); //��ȡ������
				theFid = atoi(featureid.c_str());//��ȡ����ID 418  ���ID����Ϊmap��key������WFSPointFeatureElement
				
				//field
				TiXmlElement* pfield = pFeature->FirstChildElement();
				if(this->m_serverType == GEO){
						pfield = pfield->NextSiblingElement(); //���˵� <gml:boundedBy>
				}
				//app:video �����ĸ�ͬ�����ӱ�ǩ�����������while�Ǳ���� xy
				while(pfield)
				{  
					std::string theFieldName = pfield->Value();  //�õ� app:geometry �����ǩ
					if(theFieldName.compare("app:geometry") == 0)
					{
						TiXmlElement* pGeo = pfield->FirstChildElement(); // �õ��ñ�ǩ�� ��һ���ӱ�ǩ gml:Point
						if(pGeo)
						{
							std::string geoStr = pGeo->Value();
							if (geoStr.compare("gml:Point") == 0)
							{
								TiXmlElement* theElement = pGeo->FirstChildElement(); //λ����Ϣ gml:pos
								if(theElement)
								{
									TiXmlNode* theNode = theElement->FirstChild();
									if(theNode)
									{
										std::string thePos = theNode->Value();
										int ipos = thePos.find(" "); //��Ϊ �������ֵ֮�� ��ʹ�ÿո����ָ��
										std::string y = thePos.substr(0, ipos);
										std::string x = thePos.substr(ipos + 1, thePos.length());
										try
										{
											if(m_Width_ <= 0)
												m_Width_ = m_Style_->m_ImageWidth;
											if(m_Height_ <= 0)
												m_Height_ = m_Style_->m_ImageHeight;

											int showWidth = m_Width_;
											int showHeight = m_Height_;
                                        //���õ�� λ����Ϣ
											theCurrentFeature->x_ = atof(x.c_str()); //ת��Ϊdouble
											theCurrentFeature->y_ = atof(y.c_str());
										//���õ�� ��ʾ��Ϣ
											theCurrentFeature->m_rendW = showWidth;
											theCurrentFeature->m_rendH = showHeight;
										}
										catch (CMemoryException* e)
										{

										}
										catch (CFileException* e)
										{
										}
										catch (CException* e)
										{
										}
									}
								}
							}
						}
					}
					else  //����������ͬ���ӱ�ǩ��ͬ����,XML�ı��code����仯���ɴ������ӱ�ǩ
					{
						std::string theValueString = "";
						TiXmlNode* theNode = pfield->FirstChild();  //�õ���ǩ �µ� ֵ  xy
						if(theNode)
						{
							theValueString = theNode->Value();
						}
						//��WFSPointFeatureElement�� ���� ["app:id",1000033_1_0_0]
						theCurrentFeature->m_PontFeature[theFieldName] = theValueString;
					}
					pfield = pfield->NextSiblingElement();
				}
				//�Ѵ�XML�����õ��� WFSPointFeatureElement ��ӵ������������ȥ������gml:id��Ϊkey���б���
				theSet.m_DataSet_[theFid] = theCurrentFeature;
			}
		}
	}
}
/****************************************************************************************************************/
void CWFSLayer::Draw()
{
	if(m_bVisible && _dataLoadComplete) //ǰ��ָ��������⣬�з�����hideLayer  
	{
		if(m_layerType == vWFSPointLayer)  //���� �ȵ�
		{
			int currLevel = ((CBaseLayer *)(den->g_LayerManager.m_BaseLayer))->GetCurrLevel();
			den->m_offset = currLevel - den->m_level;
			m_PointElementSet_.RenderManyUseOgles(m_Style_,den);  //CWFSLayer�������ֶ� ������ʽ
		}
		else if(m_layerType == vElementLayer)			//��ʼ����·����Ϣ
		{
			if(!m_LineWidth.empty()) //���ո����ĺõ�pixel���������
			{
				int currLevel = ((CBaseLayer *)den->g_LayerManager.m_BaseLayer)->GetCurrLevel(); //��ǰ�ĵ�ͼ��ʾ Level
				int offset = currLevel - (den->m_level); //���׼��ͼ���� ƫ�Ʋ�
				int widthSize = m_LineWidth.size();
				if( offset >= widthSize)  //Խ��󣬲��ٷŴ�
					offset = widthSize - 1;
				else if(offset <= 0)
					offset = 0;
				m_LineWidth_ = m_LineWidth[offset];  //�޸Ļ��ߵĿ��
			}
		   if(m_LineElementSet_.m_thread_complete)
		   {
			   //���ԭ�е�����,ֻ��ʹ������ʱ����ȥɾ���ɵ�����
			   if(!m_LineElementSet_.m_DataSet_.empty())
			   {
				   auto iter1 = m_LineElementSet_.m_DataSet_.begin();
				   auto iter2 = m_LineElementSet_.m_DataSet_.end();
				   while(iter1 != iter2)
				   {
					   if(iter1->second)
					   {
						   delete iter1->second;
						   iter1->second = NULL;
					   }
					   ++iter1;
				   }
			   }
			   m_LineElementSet_.m_DataSet_.clear();
			   // copy �߳����ص����ݵ� m_DataSet_
			   auto iter1 = m_LineElementSet_.m_Thread_DataSet_.begin();
			   auto iter2 = m_LineElementSet_.m_Thread_DataSet_.end();
			   m_LineElementSet_.m_DataSet_.insert(iter1,iter2);

			   m_LineElementSet_.m_update = true; //��Ҫ�����Ż�����
			  
			   //ʹ������
			   m_LineElementSet_.RenderManyUseOgles(m_LineWidth_, m_ColorR_, m_ColorG_, m_ColorB_, m_ColorA_,den);
			   m_LineElementSet_.m_thread_complete = false;
			   SetEvent(g_hThreadLineEvent); //�ɽ�����һ�θ��£�δ����ǰ������threadһֱ���ڵȴ�����״̬
		   }
		   else
		   //����ԭ�е�����
		    m_LineElementSet_.RenderManyUseOgles(m_LineWidth_, m_ColorR_, m_ColorG_, m_ColorB_, m_ColorA_,den);
		}
		else  //����
		{
			auto iter1 = m_PolyElementSet_.m_DataSet_.begin();
			auto iter2 = m_PolyElementSet_.m_DataSet_.end();
			for(;iter1 != iter2 ;++iter1)
			{
				den->FillPoly(iter1->second->m_PointList_, m_ColorR_, m_ColorG_, m_ColorB_, m_ColorA_);
			}
		}
	}
}
//����WFS�ķ�����������
void CWFSLayer::SearchByFid(const long& fid, CWFSPointFeatureElementSet& theSet)
{
	char bufferRect[512] = {0};
	char* layerName = new char[m_LayerName_.length() + 1]; 
	//sprintf(layerName, "%s", m_LayerName_.c_str());
	strcpy(layerName,m_LayerName_.c_str());
	//���ʹ��string��д��������鷳Щ������ʹ��itoa����sprint��fid cast to char * ,��ʹ��string::operator+()
	sprintf(bufferRect, "&featureid=APP_%s_%d", strupr(layerName), fid);
	delete[] layerName;
	//std::string layerName = 
	std::string theUrl = m_UrlPath_ + bufferRect;
	std::string theText = "";
	m_CurlConnect_->GetDataFromByHttp(theUrl, theText);
	theSet.RemoveAllFeature();
	LoadDataByString(theText, theSet);  //����XML �õ������
}
//����WFS�ķ�����������
void CWFSLayer::SearchByRect(vector<double>&theRect, CWFSPointFeatureElementSet& theSet)
{
	if(theRect.size() == 4)
	{
		double xbegin = theRect[0];
		double ybegin = theRect[1];
		double xend = theRect[2];
		double yend = theRect[3];
		char bufferRect[512] = {0};
		if (xbegin>xend)
		{
			double tmp = xend;
			xend = xbegin;
			xbegin = tmp;
		}

		if (ybegin>yend)
		{
			double tmp = yend;
			yend = ybegin;
			ybegin = tmp;
		}
		// СС ���
		sprintf(bufferRect, "&BBOX=%f,%f,%f,%f,EPSG:4326", xbegin, ybegin, xend, yend);
		std::string theUrl = m_UrlPath_ + bufferRect;
		std::string theText = "";
		m_CurlConnect_->GetDataFromByHttp(theUrl, theText); //����õ���������XML
		theSet.RemoveAllFeature();
		LoadDataByString(theText, theSet);
	}
}

//��ѡʱ�������ȵ�FID  ����CRgn ���ռ伸������
void CWFSLayer::SearchByRect(RECT& theRect, CWFSPointFeatureElementSet& theSet)
{
  //������ѡ������
	CRgn rgn;
	rgn.CreateRectRgnIndirect(&theRect);
	std::map<long, WFSPointFeatureElement*>::iterator theBegin = m_PointElementSet_.m_DataSet_.begin();
	std::map<long, WFSPointFeatureElement*>::iterator theEnd = m_PointElementSet_.m_DataSet_.end();
	RECT rect; //��ʾ�����ȵ�FID�ľ�������

    while (theBegin != theEnd)
	{
		WFSPointFeatureElement* theCurrentElement = theBegin->second;
		long pix = 0;
		long piy = 0;
		den->Projection_to_Pixel(theCurrentElement->x_, theCurrentElement->y_, pix, piy);
		//g_CtrlW�Ǵ��ڵ� ��  g_CtrlH�Ǵ��ڵĸ�
		//��� ����� �����ǵĴ��ڵĿ��ӷ�Χ�� �Ż������ ��
		if(pix > 0 && pix < den->g_CtrlW && piy < den->g_CtrlH && piy > 0 && theCurrentElement->m_visible)
		{
			//FID ��RECT
			SetRect(&rect,pix - theCurrentElement->m_rendW / 2,piy - theCurrentElement->m_rendH,pix + theCurrentElement->m_rendW / 2,piy + theCurrentElement->m_rendH);
			if(rgn.RectInRegion(&rect)) //�����ǿ�
			{
				WFSPointFeatureElement* newCurrentElement = new WFSPointFeatureElement();
				newCurrentElement->x_ = theCurrentElement->x_;
				newCurrentElement->y_ = theCurrentElement->y_;
				newCurrentElement->m_PontFeature.clear();
				newCurrentElement->m_PontFeature.insert(theCurrentElement->m_PontFeature.begin(), theCurrentElement->m_PontFeature.end());
				theSet.m_DataSet_[theBegin->first] = newCurrentElement; // theSet���Ǹú��� ���ص�ֵ�����ǰ����ȵ�ԭ���� key 
			}
		}
		theBegin++;
	}//while����
}
// �����ѡ����
void CWFSLayer::SearchByLine(vector<POINT>&theRect, CWFSPointFeatureElementSet& theSet)
{
	int number = theRect.size() / 4; //�ı��ε��ܸ���
	int* numberPoloygon = new int[number];
	for(int i = 0; i < number; i++)
		numberPoloygon[i] = 4;
	CRgn thePologyn;  //MFC����� ������˶� ����� ��װ
	// ��������
	bool bResult = thePologyn.CreatePolyPolygonRgn(&(theRect[0]), numberPoloygon, number, WINDING); //���Ƶ����ģʽ,һ�δ�������ı�������
	
	POINT theCurrentPoint;  //ȡ����ʹ�õ��ж�
	RECT rect;//ʹ��[�ȵ�] ������ͼ�ľ��� ���ж��Ƿ��ཻ
	
	std::map<long, WFSPointFeatureElement*>::iterator theBegin = m_PointElementSet_.m_DataSet_.begin();
	std::map<long, WFSPointFeatureElement*>::iterator theEnd = m_PointElementSet_.m_DataSet_.end();

	while (theBegin != theEnd)  //���� ���е� [�ȵ�]
	{
		WFSPointFeatureElement* theCurrentElement = theBegin->second;
		den->Projection_to_Pixel(theCurrentElement->x_, theCurrentElement->y_, theCurrentPoint.x, theCurrentPoint.y); // ���ȵ�� [��γ������-->��������]
		//ֻ���ڴ�����ʾ��Χ֮�ڵ� [�ȵ�] �������ǹ������� ���ж� ����֮���Ƿ��ཻ
		if(theCurrentPoint.x > 0 && theCurrentPoint.x < den->g_CtrlW && theCurrentPoint.y < den->g_CtrlH && theCurrentPoint.y > 0)
		{   
			rect.left = theCurrentPoint.x - theCurrentElement->m_rendW/2;
			rect.top = theCurrentPoint.y - theCurrentElement->m_rendH/2;
			rect.right = theCurrentPoint.x + theCurrentElement->m_rendW/2;
			rect.bottom = theCurrentPoint.y + theCurrentElement->m_rendH/2;

			//if(NULL == ver_log)  ����д����־�ļ�
			//ver_log=fopen("D:\\MapEngine_log_ver.txt","w+b");  //�鿴�� �ļ��򿪵�ģʽ
			//assert(ver_log);
			//char tx_log[100]={};
			//sprintf(tx_log,"left:%d ,top:%d ,right:%d ,bottom:%d\n",rect.left,rect.top,rect.right,rect.bottom);
			//fwrite(tx_log,strlen(tx_log),1,ver_log);
			//fflush(ver_log); //�� ��������д��������

			if(thePologyn.RectInRegion(&rect))
			//if(thePologyn.PtInRegion(theCurrentPoint)) //���[��] �Ƿ��ھ�������
			{
				/*������CWFSPointFeatureElementSet�����ͷ�*/
				WFSPointFeatureElement* newCurrentElement = new WFSPointFeatureElement();
				//memcpy(newCurrentElement, theCurrentElement, sizeof(WFSPointFeatureElement));
				newCurrentElement->x_ = theCurrentElement->x_;
				newCurrentElement->y_ = theCurrentElement->y_;
				newCurrentElement->m_PontFeature.clear();
				newCurrentElement->m_PontFeature.insert(theCurrentElement->m_PontFeature.begin(), theCurrentElement->m_PontFeature.end());

				theSet.m_DataSet_[theBegin->first] = newCurrentElement; // theSet���Ǹú��� ���ص�ֵ�����ǰ����ȵ�ԭ���� key ������
			}
		}
		theBegin++;
	}
	delete[] numberPoloygon;
}
// ��ѡ����
void CWFSLayer::SearchByPolgyn(vector<POINT>&theRect, CWFSPointFeatureElementSet& theSet)
{
	CRgn thePolgynArea;
	thePolgynArea.CreatePolygonRgn(&(theRect[0]), theRect.size(), WINDING);
	//POINT theCurrentPoint;
	long pix = 0;
	long piy = 0;
	std::map<long, WFSPointFeatureElement*>::iterator theBegin = m_PointElementSet_.m_DataSet_.begin();
	std::map<long, WFSPointFeatureElement*>::iterator theEnd = m_PointElementSet_.m_DataSet_.end();
	while (theBegin != theEnd)
	{
		WFSPointFeatureElement* theCurrentElement = theBegin->second;
		den->Projection_to_Pixel(theCurrentElement->x_, theCurrentElement->y_, pix, piy);
		CRect rect;
		rect.SetRect(pix - theCurrentElement->m_rendW / 2,piy - theCurrentElement->m_rendH,pix + theCurrentElement->m_rendW / 2,piy + theCurrentElement->m_rendH);
		if(thePolgynArea.RectInRegion(rect))
		{
			/*����CWFSPointFeatureElementSet�����ͷ�*/
			WFSPointFeatureElement* newCurrentElement = new WFSPointFeatureElement();
			//memcpy(newCurrentElement, theCurrentElement, sizeof(WFSPointFeatureElement));
			newCurrentElement->x_ = theCurrentElement->x_;
			newCurrentElement->y_ = theCurrentElement->y_;
			newCurrentElement->m_PontFeature.clear();
			newCurrentElement->m_PontFeature.insert(theCurrentElement->m_PontFeature.begin(), theCurrentElement->m_PontFeature.end());
			theSet.m_DataSet_[theBegin->first] = newCurrentElement;
		}
		theBegin++;
	}
	return;   //�鿴����theSet�е��ȵ����� ��ѡ�Ƿ�����ȷ��
}
//���Ȧѡ
void CWFSLayer::SearchByCircle(const double& xmid, const double& ymid, const double& theRout, CWFSPointFeatureElementSet& theSet)
{
	// ��ȡ���η���������Դ
	long xmidpix;
	long ymidpiy;
	den->Projection_to_Pixel(xmid, ymid, xmidpix, ymidpiy);
	double leftx = 0;
	double lefty = 0;
	double rightx = 0;
	double righty = 0;
	den->Pixel_to_Projection(xmidpix - theRout, ymidpiy - theRout, leftx, lefty);
	den->Pixel_to_Projection(xmidpix + theRout, ymidpiy + theRout, rightx, righty);
	std::vector<double> theLocationSet;
	theLocationSet.clear();
	theLocationSet.push_back(leftx);
	theLocationSet.push_back(righty);
	theLocationSet.push_back(rightx);
	theLocationSet.push_back(lefty);
	CWFSPointFeatureElementSet theRectSet;
	SearchByRect(theLocationSet, theRectSet);

	// ɸѡ�õ� Բ��������Դ
	// ���㾭γ�Ⱦ���
	double projectdistance = sqrt((xmid - leftx) * (xmid - leftx) + (ymid - (righty + lefty) / 2) * (ymid - (righty + lefty) / 2));
	std::map<long, WFSPointFeatureElement*>::iterator theBegin = theRectSet.m_DataSet_.begin();
	std::map<long, WFSPointFeatureElement*>::iterator theEnd = theRectSet.m_DataSet_.end();
	while (theBegin != theEnd)
	{
		WFSPointFeatureElement* theCurrentElement = theBegin->second;
		double theCurDistance = .0;
		theCurDistance = sqrt((theCurrentElement->x_ - xmid) * (theCurrentElement->x_ - xmid) + (theCurrentElement->y_ - ymid) * (theCurrentElement->y_ - ymid));
		if(theCurDistance < projectdistance)
		{
			/*����CWFSPointFeatureElementSet�����ͷ�*/
			WFSPointFeatureElement* newCurrentElement = new WFSPointFeatureElement();
			//memcpy(newCurrentElement, theCurrentElement, sizeof(WFSPointFeatureElement));
			newCurrentElement->x_ = theCurrentElement->x_;
			newCurrentElement->y_ = theCurrentElement->y_;
			newCurrentElement->m_PontFeature.clear();
			newCurrentElement->m_PontFeature.insert(theCurrentElement->m_PontFeature.begin(), theCurrentElement->m_PontFeature.end());
			theSet.m_DataSet_[theBegin->first] = newCurrentElement;
		}
		theBegin++;
	}
}

void CWFSLayer::SearchByFidsFromLocal(vector<long> vecfid, CWFSPointFeatureElementSet& theSet)
{
	int count = vecfid.size();

	for (int i=0;i<count;i++)
	{
		long fid = vecfid[i];
		WFSPointFeatureElement* theCurrentElement = m_PointElementSet_.m_DataSet_[fid];
		if (theCurrentElement)
		{
			WFSPointFeatureElement* newCurrentElement = new WFSPointFeatureElement();
			newCurrentElement->x_ = theCurrentElement->x_;
			newCurrentElement->y_ = theCurrentElement->y_;
			newCurrentElement->m_PontFeature.clear();
			newCurrentElement->m_PontFeature.insert(theCurrentElement->m_PontFeature.begin(), theCurrentElement->m_PontFeature.end());
			theSet.m_DataSet_[fid] = newCurrentElement;
		}	
	}
}
//����vSingleChoose  mouse��position Point ���ȵ�FID��ɵ�RECT
void CWFSLayer::SearchByPoint(PP_Point& thePoint, CWFSPointFeatureElementSet& theSet)
{
	std::map<long, WFSPointFeatureElement*>::iterator theBegin = m_PointElementSet_.m_DataSet_.begin();
	std::map<long, WFSPointFeatureElement*>::iterator theEnd = m_PointElementSet_.m_DataSet_.end();
	POINT temp;
	temp.x = thePoint.x;
	temp.y = thePoint.y;
	RECT rect;
    while (theBegin != theEnd)
	{
		WFSPointFeatureElement* theCurrentElement = theBegin->second;
		long pix = 0;
		long piy = 0;
		den->Projection_to_Pixel(theCurrentElement->x_, theCurrentElement->y_, pix, piy);
		//g_CtrlW�Ǵ��ڵ� ��  g_CtrlH�Ǵ��ڵĸ�
		//��� ����� �����ǵĴ��ڵĿ��ӷ�Χ�� �Ż������ ��
		if(theCurrentElement->m_visible && pix > 0 && pix < den->g_CtrlW && piy < den->g_CtrlH && piy > 0)
		{
			SetRect(&rect,pix - theCurrentElement->m_rendW / 2,piy - theCurrentElement->m_rendH,pix + theCurrentElement->m_rendW / 2,piy + theCurrentElement->m_rendH);
			if(PtInRect(&rect,temp))
			{
				WFSPointFeatureElement* newCurrentElement = new WFSPointFeatureElement();
				newCurrentElement->x_ = theCurrentElement->x_;
				newCurrentElement->y_ = theCurrentElement->y_;
				newCurrentElement->m_PontFeature.clear();
				newCurrentElement->m_PontFeature.insert(theCurrentElement->m_PontFeature.begin(), theCurrentElement->m_PontFeature.end());
				theSet.m_DataSet_[theBegin->first] = newCurrentElement; // theSet���Ǹú��� ���ص�ֵ�����ǰ����ȵ�ԭ���� key 
			}
		}
		theBegin++;
	}//while����

}
