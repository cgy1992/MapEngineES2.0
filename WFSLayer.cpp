#include "WFSLayer.h"

FILE *ver_log = NULL;  //可以更换为 日志库

bool CWFSLayer::GetData()
{
	if(m_UrlPath_ != "")
	{
		// get xml
		std::string theUrl = m_UrlPath_;
		std::string::size_type pos = theUrl.find("deegree");
		(pos == theUrl.npos)? (this->m_serverType = GEO):(this->m_serverType = DEEGREE);
		std::string theText = "";
		//运行这代code后，已render的Line会消失
		m_CurlConnect_->GetDataFromByHttp(theUrl, theText); //从http拿数据，得到就是点的wfs XML文件的信息
		if (m_layerType == vWFSPointLayer) 
		{
			// remove all points
			m_PointElementSet_.RemoveAllFeature();
			return (_dataLoadComplete = LoadDataByString(theText, m_PointElementSet_));  //该方法 依据参数的不同  overload
		}
		else if(m_layerType == vElementLayer) //提取Line的信息 ，这个函数里面会出现内存泄露
		{
			return (_dataLoadComplete = LoadDataByString(theText, m_LineElementSet_)); //函数里面出现 HEAP CORRUPTION DETECTED
		}
		else   // 提取面的信息
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
	//运行这代code后，已render的Line会消失
	m_CurlConnect_->GetDataFromByHttp(theUrl, theText);
	/*if(!threadRemove)*/
		m_LineElementSet_.RemoveAllFeature(); 
	return LoadDataByString(theText, m_LineElementSet_); //函数里面出现 HEAP CORRUPTION DETECTED
}

//绘线的 宽度及颜色属性
void CWFSLayer::put_Style(const int& width, const int& rcolor, const int& gcolor, const int& bcolor, const int& acolor)
{
	m_LineWidth_ = width;
	m_ColorR_ = rcolor;
	m_ColorG_ = gcolor;
	m_ColorB_ = bcolor;
	m_ColorA_ = acolor;
}
/*******************************************提取WFS中的 点、线、面文件信息*********************************************/
//添加提取 Poly的数据信息 author by xuyan 2014/8/26 ，找一个访问标准GeoServer接口
bool CWFSLayer::LoadDataByString(const std::string& theText, CWFSPolyFeatureSet& theSet)
{
	//load the file
	TiXmlDocument pDoc;
	pDoc.Parse(theText.c_str());
	TiXmlElement* pRootElet = pDoc.RootElement();  //指向 wfs:Feature
	if(NULL == pRootElet)
		return false;
	CWFSPolyFeature* theCurrentFeature = NULL;  //保存一条Poly 位置及纹理的相关属性
	std::string thePointListValue = ""; //保存从XML中提取的 Poly上的离散点的坐标信息
	//part1:兼容GEO
	TiXmlElement * i = NULL;
	if(this->m_serverType == DEEGREE){
		i = pRootElet->FirstChildElement();
	}else if(this->m_serverType == GEO){
		i = pRootElet->FirstChildElement()->NextSiblingElement(); //过滤掉第一个 gml:boundedBy
	}

	for(; i != NULL; i = i->NextSiblingElement())
	{
		TiXmlElement* pFeature = i->FirstChildElement();  // 指向 app:poly  
		std::string theLayerName = pFeature->Value(); //获取 app：poly的字符串
		if(theLayerName.compare("app:" + m_LayerName_) == 0)
		{
			theCurrentFeature = new CWFSPolyFeature();
			std::string featureid = pFeature->Attribute("gml:id");
			//part2: 兼容GEO
			int ifeapos;
			if(this->m_serverType == DEEGREE)
				ifeapos = featureid.find_last_of('_');  //modify by xuyan
			else
				ifeapos = featureid.find_last_of('.');
			featureid = featureid.substr(ifeapos + 1, featureid.length());
			long theFid = atoi(featureid.c_str());


			TiXmlElement *pField = pFeature->FirstChildElement(); //指向 app:geometry	
			//part3:兼容GEO
			if(this->m_serverType == GEO)
				pField = pField->NextSiblingElement(); //过滤掉<gml:boundedBy>
			//app:poly下的5个子标签 geometry,id,name,description,remark
			for(;pField != NULL; pField = pField->NextSiblingElement())
			{
				std::string theFieldName = pField->Value();
				if(0 == theFieldName.compare("app:geometry"))  //对这个子节点 单独处理
				{
					TiXmlElement *pGeo = pField->FirstChildElement(); // 指向 gml:Polygon
					if(0 == std::string(pGeo->Value()).compare("gml:Polygon"))
					{
					  TiXmlElement *pGeo1 = pGeo->FirstChildElement();
					  if(0 == std::string(pGeo1->Value()).compare("gml:exterior"))
					  {
					     TiXmlElement *pGeo2 = pGeo1->FirstChildElement();
						 if(0 == std::string(pGeo2->Value()).compare("gml:LinearRing"))
						 {
							 TiXmlNode *theNode = pGeo2->FirstChildElement(); //指向 gml:posList
							 if(theNode)
							 {
								TiXmlNode* theCurNode = theNode->FirstChild(); //指向 Poly 的离散点的坐标值
								if(theCurNode)
								{
									try
									{
										thePointListValue = theCurNode->Value();
									/*****************接下来就将stirng 按照空格来split 提取里面的坐标值*************/
										std::string::size_type ipos = thePointListValue.find(" ");
										MapEngineES_Point theCurrentPointLocation; //这里从XML直接取出的 经纬度信息
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
											theCurrentFeature->m_PointList_.push_back(theCurrentPointLocation); //提取出一对 坐标信息
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
					TiXmlNode* theNode = pField->FirstChild();  //得到标签 下的 值  xy
					if(theNode)
					{
						theValueString = theNode->Value();
					}
					//在WFSPointFeatureElement中 保存 ["app:id",1000033_1_0_0]
					theCurrentFeature->m_PolyFeature[theFieldName] = theValueString;
				}
			}//内层for循环
			m_PolyElementSet_.m_DataSet_[theFid] = theCurrentFeature;  //将Poly的位置数据保存在map容器中
		}//判别是否是 app:poly
	}//外层for循环，处理完所有的 app:geometry 即所有的Poly
	
	return true;
}
//modify by xuyan 2014/7/24 WFSLine信息
bool CWFSLayer::LoadDataByString(const std::string& theText, CWFSLineFeatureSet& theSet)
{
	TiXmlDocument pDoc;
	pDoc.Parse(theText.c_str());

	TiXmlElement* pRootElet = pDoc.RootElement();  //指向 wfs:FeatureCollection
	if(NULL == pRootElet)
	 return false;
	CWFSLineFeature* theCurrentFeature = NULL;  //保存一条Line 位置及纹理的相关属性
	std::string thePointListValue = ""; //保存从XML中提取的 line上的离散点的坐标信息
    
	TiXmlElement * i = NULL;
	if(this->m_serverType == DEEGREE){
		i = pRootElet->FirstChildElement();
	}else if(this->m_serverType == GEO){
		i = pRootElet->FirstChildElement()->NextSiblingElement(); //过滤掉第一个 gml:boundedBy
	}
	
   //外层for就是迭代所有的gml::featureMember  i 是 指向 gml：featureMember
   for( ; i != NULL; i = i->NextSiblingElement())
	{
		TiXmlElement* pFeature = i->FirstChildElement();  // 指向 app:road  
		std::string theLayerName = pFeature->Value(); //获取 app：road的字符串
	    //这里的m_LayerName_是在addWFSLine时，由这个初始化theWFSLayer->put_LayerName(root["LayerName"].asString());
		if(theLayerName.compare("app:" + m_LayerName_) == 0) //如果是app:road证明这是一条路
	    {
				theCurrentFeature = new CWFSLineFeature();  //创建一个用于存放 Line 的位置信息 的对象 xuyan
				std::string featureid = pFeature->Attribute("gml:id");
				//int ifeapos = featureid.find('_');
				int ifeapos;
				if(this->m_serverType == DEEGREE)
					ifeapos = featureid.find_last_of('_');  //modify by xuyan
				else
					ifeapos = featureid.find_last_of('.');

				featureid = featureid.substr(ifeapos + 1, featureid.length());
				long theFid = atoi(featureid.c_str()); //获取到Line的ID号，后面将会使用这个ID作为key放入管理Line的容器中

				TiXmlElement *pField = pFeature->FirstChildElement(); //指向 app:geometry
				if(this->m_serverType == GEO)
					pField = pField->NextSiblingElement(); //过滤掉<gml:boundedBy>
				//road 下的五个 子节点 才需要迭代
				for(;pField != NULL; pField = pField->NextSiblingElement())
				{

                   std::string theFieldName = pField->Value();
                   if(0 == theFieldName.compare("app:geometry"))  //对这个子节点 单独处理
                   {
                   	 TiXmlElement *pGeo = pField->FirstChildElement(); //指向 app:LineString
                   	 if(0 == std::string(pGeo->Value()).compare("gml:LineString"))
                   	 {
                   	 	TiXmlNode *theNode = pGeo->FirstChildElement(); //指向 app:posList
                   	 	if(theNode)
                   	 	{
                   	 		TiXmlNode* theCurNode = theNode->FirstChild(); //指向 Line 的离散点的坐标值
                   	 		if(theCurNode)
                   	 		{
								try
								{
									thePointListValue = theCurNode->Value();
								/*****************接下来就将stirng 按照空格来split 提取里面的坐标值*************/
									std::string::size_type ipos = thePointListValue.find(" ");
									MapEngineES_Point theCurrentPointLocation;
									while(ipos != std::string::npos)
									{
										std::string y = thePointListValue.substr(0, ipos); //39.839400
										thePointListValue = thePointListValue.substr(ipos + 1, thePointListValue.length()); // +1 是为了跳过空格
										ipos = thePointListValue.find(" ");
										if(ipos == std::string::npos)
											ipos = thePointListValue.length();
										std::string x = thePointListValue.substr(0, ipos); //119.402940
										theCurrentPointLocation.x = atof(x.c_str());
										theCurrentPointLocation.y = atof(y.c_str());
										theCurrentPointLocation.fid = theFid;
										theCurrentFeature->m_PointList_.push_back(theCurrentPointLocation); //将组成Line的离散的Point 保存起来

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
                   else  //其他的 四个子节点app:id app:name app:description app:remark 处理方式相同 都是[标签名,值]这种保存方式
                   {
						std::string theValueString = "";
						TiXmlNode* theNode = pField->FirstChild();  //得到标签 下的 值  xy
						if(theNode)
						{
							theValueString = theNode->Value();
						}
						//在WFSPointFeatureElement中 保存 ["app:id",1000033_1_0_0]
						theCurrentFeature->m_LineFeature[theFieldName] = theValueString;
                   }
 
				}
		      //将这条 Line保存在容器中
			 if(m_LineElementSet_.m_thread) //如果是thread操作
              m_LineElementSet_.m_Thread_DataSet_[theFid] = theCurrentFeature;
			 else
			  m_LineElementSet_.m_DataSet_[theFid] = theCurrentFeature;
	    }//这里是加了一层 if保护 确定这个是 <app:road>
	}//外层for循环 遍历所有的gml:featureMember(这个标签下面含有一条Line)
	return true;
}

//开始解析 point的 XML文件  xy
bool CWFSLayer::LoadDataByString(const std::string& theText, CWFSPointFeatureElementSet& theSet)
{
	
	TiXmlDocument pDoc;
	pDoc.Parse(theText.c_str()); //生成DOM对象

	TiXmlElement* pRootElet = pDoc.RootElement(); //wfs:FeatureCollection
	if(pRootElet == NULL)
		return false;

	if(this->m_serverType == DEEGREE){
		ParsePointXML(pRootElet->FirstChildElement(),theSet);
	}else if(this->m_serverType == GEO){
		//过滤掉 第一个 <gml:boundedBy>
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
		//app:video 没有与它同级的子标签 不需要for循环 xy在line中做了修改
		for (; pFeature != NULL; pFeature = pFeature->NextSiblingElement())
		{
			//app:hzvideo
			std::string theLayerName = pFeature->Value();  //app:video这个子标签  xy
			if(theLayerName.compare("app:" + m_LayerName_) == 0)  //在解析js发来的cmd时，已经完成赋值
			{
				if("app:bus" == theLayerName)
					m_PointElementSet_.m_IsBus = true;
				theCurrentFeature = new WFSPointFeatureElement();  //创建了一个热点元素
				std::string featureid = pFeature->Attribute("gml:id"); //APP_VIDEO_418 获取标签中的属性值
				int ifeapos,theFid;
				if(this->m_serverType == DEEGREE){
					ifeapos = featureid.find_last_of('_');
				}else if(this->m_serverType == GEO){
					ifeapos = featureid.find_last_of('.');
				}
				featureid = featureid.substr(ifeapos + 1); //提取出数字
				theFid = atoi(featureid.c_str());//提取数字ID 418  这个ID是作为map的key来放置WFSPointFeatureElement
				
				//field
				TiXmlElement* pfield = pFeature->FirstChildElement();
				if(this->m_serverType == GEO){
						pfield = pfield->NextSiblingElement(); //过滤掉 <gml:boundedBy>
				}
				//app:video 是有四个同级的子标签，所以这里的while是必须的 xy
				while(pfield)
				{  
					std::string theFieldName = pfield->Value();  //得到 app:geometry 这个标签
					if(theFieldName.compare("app:geometry") == 0)
					{
						TiXmlElement* pGeo = pfield->FirstChildElement(); // 得到该标签下 第一个子标签 gml:Point
						if(pGeo)
						{
							std::string geoStr = pGeo->Value();
							if (geoStr.compare("gml:Point") == 0)
							{
								TiXmlElement* theElement = pGeo->FirstChildElement(); //位置信息 gml:pos
								if(theElement)
								{
									TiXmlNode* theNode = theElement->FirstChild();
									if(theNode)
									{
										std::string thePos = theNode->Value();
										int ipos = thePos.find(" "); //因为 点的坐标值之间 是使用空格来分割的
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
                                        //设置点的 位置信息
											theCurrentFeature->x_ = atof(x.c_str()); //转换为double
											theCurrentFeature->y_ = atof(y.c_str());
										//设置点的 显示信息
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
					else  //其他的三个同级子标签相同处理,XML改变后，code无需变化，可处理多个子标签
					{
						std::string theValueString = "";
						TiXmlNode* theNode = pfield->FirstChild();  //得到标签 下的 值  xy
						if(theNode)
						{
							theValueString = theNode->Value();
						}
						//在WFSPointFeatureElement中 保存 ["app:id",1000033_1_0_0]
						theCurrentFeature->m_PontFeature[theFieldName] = theValueString;
					}
					pfield = pfield->NextSiblingElement();
				}
				//把从XML解析得到的 WFSPointFeatureElement 添加到结果容器当中去，按照gml:id作为key进行保存
				theSet.m_DataSet_[theFid] = theCurrentFeature;
			}
		}
	}
}
/****************************************************************************************************************/
void CWFSLayer::Draw()
{
	if(m_bVisible && _dataLoadComplete) //前端指令发送有问题，有发送了hideLayer  
	{
		if(m_layerType == vWFSPointLayer)  //绘制 热点
		{
			int currLevel = ((CBaseLayer *)(den->g_LayerManager.m_BaseLayer))->GetCurrLevel();
			den->m_offset = currLevel - den->m_level;
			m_PointElementSet_.RenderManyUseOgles(m_Style_,den);  //CWFSLayer有属性字段 保存样式
		}
		else if(m_layerType == vElementLayer)			//开始绘制路况信息
		{
			if(!m_LineWidth.empty()) //按照给定的好的pixel宽度来画线
			{
				int currLevel = ((CBaseLayer *)den->g_LayerManager.m_BaseLayer)->GetCurrLevel(); //当前的地图显示 Level
				int offset = currLevel - (den->m_level); //与基准地图级别 偏移差
				int widthSize = m_LineWidth.size();
				if( offset >= widthSize)  //越界后，不再放大
					offset = widthSize - 1;
				else if(offset <= 0)
					offset = 0;
				m_LineWidth_ = m_LineWidth[offset];  //修改画线的宽度
			}
		   if(m_LineElementSet_.m_thread_complete)
		   {
			   //清空原有的数据,只在使用数据时，才去删除旧的数据
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
			   // copy 线程下载的数据到 m_DataSet_
			   auto iter1 = m_LineElementSet_.m_Thread_DataSet_.begin();
			   auto iter2 = m_LineElementSet_.m_Thread_DataSet_.end();
			   m_LineElementSet_.m_DataSet_.insert(iter1,iter2);

			   m_LineElementSet_.m_update = true; //需要进行优化计算
			  
			   //使用数据
			   m_LineElementSet_.RenderManyUseOgles(m_LineWidth_, m_ColorR_, m_ColorG_, m_ColorB_, m_ColorA_,den);
			   m_LineElementSet_.m_thread_complete = false;
			   SetEvent(g_hThreadLineEvent); //可进行下一次更新，未触发前，更新thread一直处于等待阻塞状态
		   }
		   else
		   //绘制原有的数据
		    m_LineElementSet_.RenderManyUseOgles(m_LineWidth_, m_ColorR_, m_ColorG_, m_ColorB_, m_ColorA_,den);
		}
		else  //绘面
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
//利用WFS的服务在做计算
void CWFSLayer::SearchByFid(const long& fid, CWFSPointFeatureElementSet& theSet)
{
	char bufferRect[512] = {0};
	char* layerName = new char[m_LayerName_.length() + 1]; 
	//sprintf(layerName, "%s", m_LayerName_.c_str());
	strcpy(layerName,m_LayerName_.c_str());
	//如果使用string改写这个，会麻烦些，首先使用itoa或者sprint将fid cast to char * ,再使用string::operator+()
	sprintf(bufferRect, "&featureid=APP_%s_%d", strupr(layerName), fid);
	delete[] layerName;
	//std::string layerName = 
	std::string theUrl = m_UrlPath_ + bufferRect;
	std::string theText = "";
	m_CurlConnect_->GetDataFromByHttp(theUrl, theText);
	theSet.RemoveAllFeature();
	LoadDataByString(theText, theSet);  //解析XML 得到结果集
}
//利用WFS的服务在做计算
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
		// 小小 大大
		sprintf(bufferRect, "&BBOX=%f,%f,%f,%f,EPSG:4326", xbegin, ybegin, xend, yend);
		std::string theUrl = m_UrlPath_ + bufferRect;
		std::string theText = "";
		m_CurlConnect_->GetDataFromByHttp(theUrl, theText); //这里得到的数据是XML
		theSet.RemoveAllFeature();
		LoadDataByString(theText, theSet);
	}
}

//框选时，返回热点FID  利用CRgn 做空间几何运算
void CWFSLayer::SearchByRect(RECT& theRect, CWFSPointFeatureElementSet& theSet)
{
  //创建框选的区域
	CRgn rgn;
	rgn.CreateRectRgnIndirect(&theRect);
	std::map<long, WFSPointFeatureElement*>::iterator theBegin = m_PointElementSet_.m_DataSet_.begin();
	std::map<long, WFSPointFeatureElement*>::iterator theEnd = m_PointElementSet_.m_DataSet_.end();
	RECT rect; //表示各个热点FID的矩形区域

    while (theBegin != theEnd)
	{
		WFSPointFeatureElement* theCurrentElement = theBegin->second;
		long pix = 0;
		long piy = 0;
		den->Projection_to_Pixel(theCurrentElement->x_, theCurrentElement->y_, pix, piy);
		//g_CtrlW是窗口的 宽  g_CtrlH是窗口的高
		//如果 这个点 在我们的窗口的可视范围内 才绘制这个 点
		if(pix > 0 && pix < den->g_CtrlW && piy < den->g_CtrlH && piy > 0 && theCurrentElement->m_visible)
		{
			//FID 的RECT
			SetRect(&rect,pix - theCurrentElement->m_rendW / 2,piy - theCurrentElement->m_rendH,pix + theCurrentElement->m_rendW / 2,piy + theCurrentElement->m_rendH);
			if(rgn.RectInRegion(&rect)) //交集非空
			{
				WFSPointFeatureElement* newCurrentElement = new WFSPointFeatureElement();
				newCurrentElement->x_ = theCurrentElement->x_;
				newCurrentElement->y_ = theCurrentElement->y_;
				newCurrentElement->m_PontFeature.clear();
				newCurrentElement->m_PontFeature.insert(theCurrentElement->m_PontFeature.begin(), theCurrentElement->m_PontFeature.end());
				theSet.m_DataSet_[theBegin->first] = newCurrentElement; // theSet就是该函数 返回的值，还是按照热点原来的 key 
			}
		}
		theBegin++;
	}//while结束
}
// 针对线选操作
void CWFSLayer::SearchByLine(vector<POINT>&theRect, CWFSPointFeatureElementSet& theSet)
{
	int number = theRect.size() / 4; //四边形的总个数
	int* numberPoloygon = new int[number];
	for(int i = 0; i < number; i++)
		numberPoloygon[i] = 4;
	CRgn thePologyn;  //MFC里的类 ，完成了对 区域的 封装
	// 构建线条
	bool bResult = thePologyn.CreatePolyPolygonRgn(&(theRect[0]), numberPoloygon, number, WINDING); //环绕的填充模式,一次创建多个四边形区域
	
	POINT theCurrentPoint;  //取消不使用点判断
	RECT rect;//使用[热点] 纹理贴图的矩形 来判断是否相交
	
	std::map<long, WFSPointFeatureElement*>::iterator theBegin = m_PointElementSet_.m_DataSet_.begin();
	std::map<long, WFSPointFeatureElement*>::iterator theEnd = m_PointElementSet_.m_DataSet_.end();

	while (theBegin != theEnd)  //遍历 所有的 [热点]
	{
		WFSPointFeatureElement* theCurrentElement = theBegin->second;
		den->Projection_to_Pixel(theCurrentElement->x_, theCurrentElement->y_, theCurrentPoint.x, theCurrentPoint.y); // 把热点的 [经纬度坐标-->像素坐标]
		//只对在窗口显示范围之内的 [热点] ，对他们构造区域 并判断 区域之间是否相交
		if(theCurrentPoint.x > 0 && theCurrentPoint.x < den->g_CtrlW && theCurrentPoint.y < den->g_CtrlH && theCurrentPoint.y > 0)
		{   
			rect.left = theCurrentPoint.x - theCurrentElement->m_rendW/2;
			rect.top = theCurrentPoint.y - theCurrentElement->m_rendH/2;
			rect.right = theCurrentPoint.x + theCurrentElement->m_rendW/2;
			rect.bottom = theCurrentPoint.y + theCurrentElement->m_rendH/2;

			//if(NULL == ver_log)  测试写的日志文件
			//ver_log=fopen("D:\\MapEngine_log_ver.txt","w+b");  //查看下 文件打开的模式
			//assert(ver_log);
			//char tx_log[100]={};
			//sprintf(tx_log,"left:%d ,top:%d ,right:%d ,bottom:%d\n",rect.left,rect.top,rect.right,rect.bottom);
			//fwrite(tx_log,strlen(tx_log),1,ver_log);
			//fflush(ver_log); //将 内容立刻写到磁盘上

			if(thePologyn.RectInRegion(&rect))
			//if(thePologyn.PtInRegion(theCurrentPoint)) //这个[点] 是否在矩形区域
			{
				/*由外面CWFSPointFeatureElementSet析构释放*/
				WFSPointFeatureElement* newCurrentElement = new WFSPointFeatureElement();
				//memcpy(newCurrentElement, theCurrentElement, sizeof(WFSPointFeatureElement));
				newCurrentElement->x_ = theCurrentElement->x_;
				newCurrentElement->y_ = theCurrentElement->y_;
				newCurrentElement->m_PontFeature.clear();
				newCurrentElement->m_PontFeature.insert(theCurrentElement->m_PontFeature.begin(), theCurrentElement->m_PontFeature.end());

				theSet.m_DataSet_[theBegin->first] = newCurrentElement; // theSet就是该函数 返回的值，还是按照热点原来的 key 来保存
			}
		}
		theBegin++;
	}
	delete[] numberPoloygon;
}
// 面选操作
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
			/*外面CWFSPointFeatureElementSet析构释放*/
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
	return;   //查看容器theSet中的热点数量 面选是否是正确的
}
//针对圈选
void CWFSLayer::SearchByCircle(const double& xmid, const double& ymid, const double& theRout, CWFSPointFeatureElementSet& theSet)
{
	// 获取矩形服务数据资源
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

	// 筛选得到 圆的数据资源
	// 计算经纬度距离
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
			/*外面CWFSPointFeatureElementSet析构释放*/
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
//用于vSingleChoose  mouse的position Point 与热点FID组成的RECT
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
		//g_CtrlW是窗口的 宽  g_CtrlH是窗口的高
		//如果 这个点 在我们的窗口的可视范围内 才绘制这个 点
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
				theSet.m_DataSet_[theBegin->first] = newCurrentElement; // theSet就是该函数 返回的值，还是按照热点原来的 key 
			}
		}
		theBegin++;
	}//while结束

}
