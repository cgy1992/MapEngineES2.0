#pragma once

#include "CurlConnect.h"
#include "WFSPointFeature.h"
#include "WFSLineFeature.h"
#include "WFSPolyFeature.h"
#include "DrawEngine.h"
#include "tinyxml\tinyxml.h"//chkun
#include "XmlWork.h"

class CWFSLayer: public CLayer
{                   // ÃæµÄ²Ù×÷¼¯ºÏ     author by xuyan 
public:
	explicit CWFSLayer(enum VLayerType theLayerType,DrawEngine *den = NULL)
		: CLayer()
		, m_Style_(NULL)
		, m_UrlPath_("")
		, m_bGetData_(false)
		, m_Height_(0)
		, m_Width_(0)
		, m_CurlConnect_(NULL)
		, m_LayerName_("")
		, m_LineWidth_(5)
		, m_ColorR_(255)
		, m_ColorG_(0)
		, m_ColorB_(0)
		, m_ColorA_(100)
		, den(den)
		, m_PointElementSet_(den)
		, m_LineElementSet_(den)
		, m_serverType(DEEGREE)
		, _dataLoadComplete(false)
	{
		m_CurlConnect_ = CurlConnect::GetInstance();
		m_layerType = theLayerType;
	}
	enum{DEEGREE,GEO}; //Àà³£Á¿
	int m_serverType;
	~CWFSLayer(){}
	// layer name
	void put_LayerName(const std::string& theName) 
	{ 
		m_LayerName_ = theName; 
		USES_CONVERSION; 
		m_MapName = A2W(theName.c_str());
	}
	std::string get_LayerName() { return m_LayerName_; }
	// layer url
	void put_UrlPath(const std::string& theUrl) { m_UrlPath_ = theUrl; }
	// style
	void put_Style(CMarkerStyle* theStyle) { m_Style_ = theStyle; }
	CMarkerStyle* get_Style() { return m_Style_; }
	void put_Style(const int& width, const int& rcolor, const int& gcolor, const int& bcolor, const int& acolor);
	// width height
	void put_Rect(const int& width, const int& height) { m_Width_ = width; m_Height_ = height; }
	void get_Rect(int& width, int& height) { width = m_Width_; height = m_Height_; }

	// get data by http
	bool GetData();
	bool TGetData(); //thread Ê¹ÓÃµ¥¶ÀµÄº¯Êý
	// load file µã¡¢Ïß¡¢Ãæ
	bool LoadDataByString(const std::string& theText, CWFSPointFeatureElementSet& theSet);
	bool LoadDataByString(const std::string& theText, CWFSLineFeatureSet& theSet);
	bool LoadDataByString(const std::string& theText, CWFSPolyFeatureSet& theSet);
	// draw
	void Draw();
	
	//search
	void SearchByRect(vector<double>&theRect, CWFSPointFeatureElementSet& theSet);
	
	void SearchByRect(RECT& theRect, CWFSPointFeatureElementSet& theSet);
	
	void SearchByPoint(PP_Point& thePoint, CWFSPointFeatureElementSet& theSet);
	//
	void SearchByFid(const long& fid, CWFSPointFeatureElementSet& theSet);

	// ÃæÑ¡
	void SearchByPolgyn(vector<POINT>&theRect, CWFSPointFeatureElementSet& theSet);

	void SearchByLine(vector<POINT>&theRect, CWFSPointFeatureElementSet& theSet);

	// È¦Ñ¡¼¯ºÏ
	void SearchByCircle(const double& xmid, const double& ymid, const double& theRout, CWFSPointFeatureElementSet& theSet);

	//µÚÒ»´ÎÁ¬½Ó·þÎñ»ñÈ¡µ½wfslayerÊý¾ÝÒÔºó£¬Í¼²ãÊý¾Ý±ã±£´æÔÚ±¾µØ£¬ÅúÁ¿fidÐÅÏ¢µÄ»ñÈ¡Ö±½Ó´Ó¸ÃÊý¾ÝÖÐ²éÕÒ 
	void SearchByFidsFromLocal(vector<long> vecfid, CWFSPointFeatureElementSet& theSet);

private:
	CMarkerStyle* m_Style_;
	std::string m_UrlPath_;
	bool m_bGetData_;

	int m_Height_;
	int m_Width_;

	CurlConnect* m_CurlConnect_;
	std::string m_LayerName_;  //ÓÃÓÚÇø±ðÕâ²ãÊÇÏÔÊ¾ µã »¹ÊÇÏß

	//ÏÔÊ¾lineµÄ Ïà¹Ø»æÖÆÊôÐÔ
	int m_LineWidth_;
	int m_ColorR_;
	int m_ColorG_;
	int m_ColorB_;
	int m_ColorA_;
	DrawEngine *den;
	void ParsePointXML(TiXmlElement * first,CWFSPointFeatureElementSet& theSet);
public:
	static bool threadRemove;
public:
	bool _dataLoadComplete;  // init false
	CWFSPointFeatureElementSet m_PointElementSet_;				// µãµÄ²Ù×÷¼¯ºÏ
	CWFSLineFeatureSet m_LineElementSet_;// ÏßµÄ²Ù×÷¼¯ºÏ     author by xuyan
	CWFSPolyFeatureSet m_PolyElementSet_;   
	std::vector<int> m_LineWidth; //²»Í¬µØÍ¼¼¶±ð£¬LineWidthµÄÏñËØ ¿í¶È£¬µÀÂ·»Ø·Å»áÊ¹ÓÃ
	
};

bool CWFSLayer::threadRemove = false;
