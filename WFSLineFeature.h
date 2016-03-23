#pragma once
#include "Element.h"
#include <map>
#include <string>
#include <set>
#include "LineElement.h"
class CWFSLineFeature
{
public:
	CWFSLineFeature() {}
	~CWFSLineFeature() {}
	std::vector<MapEngineES_Point> m_PointList_;		// µãÁÐ±í
	std::map<std::string, std::string> m_LineFeature;	// ÊôÐÔ
	//Ìí¼ÓÒ»¶Ô×ø±êÖµ ¼ÇÂ¼Ò»ÌõÏßµÄ ÆðµãºÍÖÕµã
	double xmin;
	double ymin;
	double xmax;
	double ymax;

};

class CWFSLineFeatureSet
{
public:
	CWFSLineFeatureSet(DrawEngine *den = NULL) : m_LineHandle_(-1),den(den),theLineElement(den)
	{
		m_b = true; //³õÊ¼»¯Ê±ÐèÒª ¶ÔLineµÄ¼¯ºÏ½øÐÐÔ¤´¦Àí
		m_update = false;
		m_cast = true; //Ä¬ÈÏ ´«ÈëµÄÏß¿íÊÇÐèÒª×ª»¯µÄ
		m_query_show = false;
		m_query_hide = false;
		m_thread = false; //·Çthread²Ù×÷
		m_thread_complete = false;
		m_MapMoveComplete = false;
		m_ChangeMapJS_ = false;
	}
	~CWFSLineFeatureSet()
	{
		RemoveAllFeature();
	}
	std::map<long, CWFSLineFeature*> m_DataSet_; // all Line data
	std::map<long, CWFSLineFeature*> m_Thread_DataSet_; //threadÊ¹ÓÃµÄÊý¾Ý¼¯ºÏ
	std::map<long,CWFSLineFeature*> m_QueryDataSet_; //query ÐèÒªÏÔÊ¾µÄLine
	std::set<int> m_HideLineSet_;  // ²»ÒªÏÔÊ¾µÄ LineµÄFID

	//add by wangyuejiao
	std::vector<Vertex> theLineData; 
	//end by wangyuejiao

	void RenderManyUseOgles(/*const int& width,*/int width, const int& rcolor, const int& gcolor, const int& bcolor, const int& acolor,DrawEngine *den = NULL);
	void RemoveAllFeature();
//ÉèÖÃÏß³Ì²ÎÊý
	void SetThreadPara()
	{
	
	}
	unsigned ThreadRenderFun(PVOID pM);
private:
	long m_LineHandle_;
	
	struct ThreadPara //¹¹Ôì´«µÝ¸öThreadµÄ²ÎÊý
	{
	
	};
public:
	bool m_b;  //¼ÆËãLineµÄMinÓëMax£¬±ãÓÚÏÔÊ¾µÄÓÅ»¯´¦Àí
	bool m_update; //update Í¬ÑùÐèÒª¶ÔLine½øÐÐÔ¤´¦Àí¼ÆËã£¬¸Ã±äÁ¿ÐèÒªÔÚDrawEngine.cppÖÐaccess so set public
	bool m_cast; // Ä¬ÈÏÏß¿í widthÊÇ´«ÈëµÄ µØÀí×ø±ê£¬ÐèÒª×ª»»ÎªpixelÖµ
	bool m_query_show; //±êÊ¶ Line ÓÐ²éÑ¯ÏÔÊ¾
	bool m_query_hide; //±êÊ¶ Line ÓÐ²éÑ¯Òþ²Ø
	bool m_thread; // Èç¹ûÊÇthread£¬ÏÈ½«¸üÐÂÏÂÔØµÄÊý¾Ý ·ÅÖÃµ½Ò»¸öµ¥¶ÀµÄÈÝÆ÷m_Thread_DataSet_ÖÐ
	bool m_thread_complete; //±íÊ¾threadÍê³É ³õÊ¼»¯Îªfalse
	bool m_MapMoveComplete; //ÍÏ¶¯µØÍ¼£¬»òÕß»¬¶¯»¬ÂÖ
	DrawEngine *den;
	CLineElement theLineElement; //»æÏßµÄ»ù´¡Àà
	bool m_ChangeMapJS_; //针对矢量
};