#include "WFSLineFeature.h"
#include "MarkerElement.h"

//#define DEBUGGPS
extern std::map<int,CStyle*> g_vecStyle; //¿ÉÒÔÓÃÓÚ ±íÊ¾Â·¶Î·½Ïò£¬¶øÌùÍ¼

void CWFSLineFeatureSet::RenderManyUseOgles(int width, const int& rcolor, const int& gcolor, const int& bcolor, const int& acolor,DrawEngine *den)
{
	// scane all the line
	std::map<long, CWFSLineFeature*>::iterator theLineBegin = m_DataSet_.begin();
	std::map<long, CWFSLineFeature*>::iterator theLineEnd = m_DataSet_.end();
	//ËÄ¸ö¶¥µã ¹¹ÔìÁ½¸öÈý½ÇÐÎ
	//Èç¹ûÊ¹ÓÃlong À´±íÊ¾×ø±ê£¬»áÔì³ÉÑÏÖØµÄ¾«¶È¶ªÊ§£¬µ¼ÖÂºóÃæ»æÖÆµÄÏßÌõÆ«ÒÆÒÔ¼°²¹µãÓëÏßÌõµÄÎ»ÖÃ²»×¼
	float x1 = 0.0f;
	float y1 = 0.0f;
	float x2 = 0.0f;
	float y2 = 0.0f;
	float x3 = 0.0f;
	float y3 = 0.0f;
	float x4 = 0.0f;
	float y4 = 0.0f;
	//Àï±ßµÄ×ø±ê¶¼ÒÑ¾­ ÊÇÏñËØ×ø±ê
	//modify by wangyuejiao
	//std::vector<Vertex> theLineData; //¹¹ÔìÈý½ÇÐÎ vertexµÄ¸öÊý
	std::vector<Vertex> pointVec; //ÓÃÓÚ²¹¹Õµã
	std::vector<Vertex> directVec; //ÓÃÓÚ±íÊ¾ Â·¶ÎµÄ·½Ïò
	Vertex theCurrentData;
	theCurrentData.tu = 0;
	theCurrentData.tv = 0;
	theCurrentData.color[0] = rcolor / 255.0;
	theCurrentData.color[1] = gcolor / 255.0;
	theCurrentData.color[2] = bcolor / 255.0;
	theCurrentData.color[3] = acolor / 255.0;

	//CLineElement theLineElement(den);  //»æLineµÄ»ù´¡Àà£¬¾Ö²¿±äÁ¿,

	int pWidth = width;  //ÓÉjsµÄcmd·¢À´ ÕâÀïµÄÖµÊÇ100  ±íÊ¾ LineµÄ¿í¶È
	double d;
	if (m_cast)  //ÕâÀïµÄÄ¬ÈÏÖµ ÊÇ"" Ö»ÓÐÂ·¿ö ²ÅÊÇmetre  lineLuKuang->m_unit = L"metre"
	{
		double X,Y,pletflon,plon,pwidth,pheight,pToplat,plat,resolution;
		double scale = 0.0;
		CString name;
		den->GetCurrentShowInfo(X,Y ,pwidth,pheight,pletflon,pToplat,plon,plat,scale,name,resolution);

		if (scale<0.00000001f)
		{
			return;
		}
		//pWidth = pWidth*(96*39.7)/scale;//ÐèÒªÑéÖ¤????
		double an = pWidth / (2 * 3.1415926 * 6378137.0) * 360;
		//d = w / (2 * 3.1415926 * 6378137.0) * 360;
	     pWidth = an/resolution;
	}
	theLineElement.m_w = pWidth; //ÉèÖÃ ÏÔÊ¾Â·¿öµÄ Ïß¿í ÕâÀïÒÑ¾­×ª»»Îª ÒÔpixelÎªµ¥Î»

	//¼ÌÐøµü´ú±éÀú´¦ÀíÃ¿Ò»ÌõÏß ,¸ù¾ÝÊÇ·ñ²éÑ¯

	theLineBegin = m_query_show ? m_QueryDataSet_.begin():m_DataSet_.begin();
	theLineEnd = m_query_show ? m_QueryDataSet_.end():m_DataSet_.end();

	//add by wangyejiao  first or scale lead computer vertex data
	//if(theLineData.size() == 0 || bChangeMapJS || m_update)
	if(theLineData.size() == 0 || m_ChangeMapJS_ || m_update)
	{
		theLineData.clear();
		identity_matrix(g_mapTranslationMatrix);
				/*********************************************************************************************************************/
		while(theLineBegin != theLineEnd) //遍历所有的Line，(一条Line又是由多个离散的PoinAt组成)
		{
			//如果线条在 HideLineSet中 则不显示 及
			if(m_query_hide && m_HideLineSet_.count(theLineBegin->first) == 1)
			{
				theLineBegin ++;
				continue;
			}
			//不在可视范围内的不处理
			long xi,yi;
			long xe,ye;
			den->Projection_to_Pixel( theLineBegin->second->xmin,  theLineBegin->second->ymax, xi, yi);
			den->Projection_to_Pixel( theLineBegin->second->xmax,  theLineBegin->second->ymin, xe, ye);

			int xxi = xi>0?xi:0;
			int yyi = yi>0?yi:0;
			int xxe = xe>den->g_CtrlW?den->g_CtrlW:xe;
			int yye = ye>den->g_CtrlH?den->g_CtrlH:ye;
			//放开对Line筛选的代码
			//if(!(xxi <xxe&&yyi<yye))
			//{
			//	theLineBegin ++;
			//	continue;
			//}
			//设置该条线的颜色
			if(1 == theLineBegin->second->m_LineFeature.count("app:description"))
			{
				std::string theRGB = theLineBegin->second->m_LineFeature["app:description"];
				Json::Value root;
				Json::Reader reader;
				reader.parse(theRGB.c_str(), root);
				theCurrentData.color[0] = root["r"].asInt() / 255.0;
				theCurrentData.color[1] = root["g"].asInt() / 255.0;
				theCurrentData.color[2] = root["b"].asInt() / 255.0;
				theCurrentData.color[3] = root["a"].asInt() / 255.0;
			}
			// scane all the point
			std::vector<MapEngineES_Point>::iterator thePointBegin = theLineBegin->second->m_PointList_.begin();
			std::vector<MapEngineES_Point>::iterator thePointEnd = theLineBegin->second->m_PointList_.end();
			std::vector<MapEngineES_Point>::iterator thePointPre;
			while(thePointBegin != thePointEnd)  //循环内部 一次性处理 两个相邻的点
			{
				thePointPre = thePointBegin;		// 下一个
				long prexpix = 0;
				long preypiy = 0;
				den->Projection_to_Pixel(thePointPre->x, thePointPre->y, prexpix, preypiy);
				/**************add by xuyan  用于补拐点*******************************/
				theCurrentData.loc[0] = prexpix;
				theCurrentData.loc[1] = preypiy;
				theCurrentData.loc[2] = 1;
				pointVec.push_back(theCurrentData);   //放置在这里后，路况的尾端也会补点
				thePointBegin++;
				/****************一次循环处理两个点****************************************/  
				//保存 每一条路的 终点,将来可用于保存路段的方向
				if(thePointEnd == thePointBegin || thePointPre->fid != thePointBegin->fid)
				{
					theCurrentData.loc[0] = prexpix;
					theCurrentData.loc[1] = preypiy;
					theCurrentData.loc[2] = 1;
					directVec.push_back(theCurrentData);
				}
				if(thePointBegin != thePointEnd)
				{
					// construct the triangle
					long curxpix = 0;
					long curypiy = 0;
					den->Projection_to_Pixel(thePointBegin->x, thePointBegin->y, curxpix, curypiy);

					if (prexpix == curxpix)
					{
						x1 = prexpix-pWidth/2;
						x2 = prexpix+pWidth/2;
						x3 = curxpix-pWidth/2;
						x4 = curxpix+pWidth/2;
						y1 = preypiy;
						y2 = preypiy;
						y3 = curypiy;
						y4 = curypiy;
					}
					else if (preypiy == curypiy)
					{
						x1 = prexpix;
						x2 = prexpix;
						x3 = curxpix;
						x4 = curxpix;
						y1 = preypiy+pWidth/2;
						y2 = preypiy-pWidth/2;
						y3 = curypiy+pWidth/2;
						y4 = curypiy-pWidth/2;
					}
					else  //相邻的两个 点 之间的位置 不是特殊位置
					{
						double d=(curypiy-preypiy)*1.0/(prexpix-curxpix);
						y1=pWidth/2.0/sqrt(d*d+1.0)+preypiy;
						x1=d*(y1-preypiy) +prexpix;
						y2=-pWidth/2.0/sqrt(d*d+1.0)+preypiy;
						x2=d*(y2-preypiy)+prexpix;
						y3=pWidth/2.0/sqrt(d*d+1.0)+curypiy;
						x3=d*(y3-curypiy)+curxpix;
						y4=-pWidth/2.0/sqrt(d*d+1.0)+curypiy;
						x4=d*(y4-curypiy)+curxpix;
					}
					//相邻的两个顶点 之间的宽线段 构成了一个实体矩形，我们使用两个 三角形来表示这个矩形
					//1 2 3
					theCurrentData.loc[0] = x1;
					theCurrentData.loc[1] = y1;
					theCurrentData.loc[2] = 1;
					theLineData.push_back(theCurrentData);

					theCurrentData.loc[0] = x2;
					theCurrentData.loc[1] = y2;
					theCurrentData.loc[2] = 1;
					theLineData.push_back(theCurrentData);

					theCurrentData.loc[0] = x3;
					theCurrentData.loc[1] = y3;
					theCurrentData.loc[2] = 1;
					theLineData.push_back(theCurrentData);

					//2 3 4
					theCurrentData.loc[0] = x3;
					theCurrentData.loc[1] = y3;
					theCurrentData.loc[2] = 1;
					theLineData.push_back(theCurrentData);

					theCurrentData.loc[0] = x2;
					theCurrentData.loc[1] = y2;
					theCurrentData.loc[2] = 1;
					theLineData.push_back(theCurrentData);

					theCurrentData.loc[0] = x4;
					theCurrentData.loc[1] = y4;
					theCurrentData.loc[2] = 1;
					theLineData.push_back(theCurrentData);
				}
			} //循环 处理完毕 一条Line内的Point
			theLineBegin++;  //外层的 循环记数变量，表示处理完一条线了
		}
	   /*
		//ÔÚÔ­ÓÐµÄ½Ó¿ÚÉÏÌí¼Ó²ÎÊý ¾ù²ÉÓÃÄ¬ÈÏ²ÎÊýµÄ×ö·¨
		if(m_MapMoveComplete)  //Æ½ÒÆ»òÕß»¬¶¯¹öÂÖwheel
		{
			theLineElement.m_PassData = true;
			m_MapMoveComplete = false;  //ÔÚÊÂ¼þ´¦Àíº¯ÊýÖÐÉèÖÃÎª true
		}
		else //·ÇÍÏ¶¯µØÍ¼µÄÆ½ÒÆ
		{
			if(m_b || m_update || g_qiepian)  //ÏßÂ·µÚÒ»´ÎÏÔÊ¾,g_qiepianÊÇÈ«¾Ö±äÁ¿
			{
				theLineElement.m_PassData = true;
				m_b = false;
				m_update = false;
				g_qiepian = false;
			}
			else
				theLineElement.m_PassData = false;
		}
		*/
		glBindBuffer(GL_ARRAY_BUFFER,theLineElement.m_vboID);
		glBufferData(GL_ARRAY_BUFFER, theLineData.size() * sizeof(Vertex), (GLvoid*)&(theLineData[0]),GL_STATIC_DRAW);
		
		if(m_update) m_update = false;
		//bChangeMapJS = false; //下一次不需要 重新计算
		m_ChangeMapJS_ = false;
	}
	//theLineData and pointVec not need 
	theLineElement.RenderManyOgles(theLineData,pointVec);//ÅúÁ¿ ÏÔÊ¾Èý½ÇÐÎ,µÚ¶þ¸ö²ÎÊýÓÃÓÚ²¹¹Õµã£¬µÚÈý¸ö²ÎÊýÖ»ÊÇ²âÊÔÊ¹ÓÃ²¹´óµã±íÊ¾Â·¶Î·½Ïò
#ifdef DEBUGGPS
	unsigned lineSize = theLineData.size();
	char lineInfo[100] = {};
	sprintf(lineInfo,"compose line point size : %u\r\n",lineSize);
	fwrite(lineInfo,strlen(lineInfo),1,tex_log);
	fflush(tex_log);
#endif
}

//ÏÖÔÚÊÇ ½ö½öÇå¿ÕthreadÊ¹ÓÃµÄÈÝÆ÷£¬Êý¾ÝÉ¾³ý·ÅÖÃÔÚCWFSLayer::Draw()ÖÐÈ¥´¦Àí
void CWFSLineFeatureSet::RemoveAllFeature()
{

	m_Thread_DataSet_.clear(); //Ö»ÊÇÇå¿ÕÏß³ÌµÄË½ÓÐÊý¾Ý
}
