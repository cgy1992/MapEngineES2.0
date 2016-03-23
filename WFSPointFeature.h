#pragma once
#include <iostream>
#include <string>
#include <map>
#include "MarkerElement.h"
#include <set>

 
class StylePointID;  //类声明

class WFSPointFeatureElement: public CMarkerElement
{
 //包含了父类中的 CMarkerStyle *m_markerstyle 并在父类中被初始化为NULL
public:
	WFSPointFeatureElement(){}
	~WFSPointFeatureElement(){}
	double	x_;				//点真实的地理坐标
	double	y_; 
	std::map<std::string, std::string> m_PontFeature;	//属性
	//std::vector<int> m_show_flagArr;
	//保存了WFSPoint的style在地图各个Level上的显示信息
	std::map<StylePointID,vector<int> > m_show_flagArr;
	struct AppendStylePos; //类型声明，样式的相对于Point中心点的位置offset

	//这里如果使用multimap 
	std::map<StylePointID,AppendStylePos> m_styleID_to_pos;
//记录该样式在CWFSPointFeatureElementSet::multimap中的位置，用于删除纹理
	typedef std::multimap<int,StylePointID>::iterator iter;
	std::map<StylePointID,iter> m_styleID_to_iter; 
	
	void SetAppendStylePos(const StylePointID &styleID,int left,int top,int width,int height)
	{
		m_styleID_to_pos[styleID].left = left;
		m_styleID_to_pos[styleID].top = top;
		m_styleID_to_pos[styleID].width = width;
		m_styleID_to_pos[styleID].height = height;
	}

	void RemoveStyle(const StylePointID & styleId)
	{
		//这个if也保证了，容器是空时，不会发生删除操作
		if(m_styleID_to_pos.count(styleId)) //删除 样式位置信息
			m_styleID_to_pos.erase(m_styleID_to_pos.find(styleId));
		if(m_styleID_to_iter.count(styleId)) //删除 样式在 热点集::multimap占据的位置
			m_styleID_to_iter.erase(m_styleID_to_iter.find(styleId));
		if(m_show_flagArr.count(styleId)) //删除 样式在某个层次是否显示
			m_show_flagArr.erase(m_show_flagArr.find(styleId));
	}
private:
	struct AppendStylePos
	{
		//前两个坐标是相对于WFSPiont中心点(x,y)的偏移量，单位是pixel
		int left,top,width,height;
	};
};

class CWFSPointFeatureElementSet
{
private:
	long m_FeatureHandle_;

public:
	CWFSPointFeatureElementSet(DrawEngine *pden = NULL);
	~CWFSPointFeatureElementSet();

	//delete all feature
	void RemoveAllFeature();
	//由热点的Fid 合成其样式的三角片 组点函数
	void ComposeVertex(const StylePointID &Fid,std::vector<Vertex> &theVertexVector,DrawEngine *den);
	void ComposeVertex(const WFSPointFeatureElement *theCurrentFeature,std::vector<Vertex> &theVertexVector,DrawEngine *den,int &discardPoint);
private:
	//add one feature
	long AddOneFeature(WFSPointFeatureElement* pFeature);
public:
	std::map<long, WFSPointFeatureElement*> m_DataSet_;
	std::map<long,WFSPointFeatureElement*> m_QueryDataSet_; //query 需要显示的Line
	std::set<int> m_HidePointSet_; // 不要显示的 point的FID
	std::set<int> m_Style_; //车辆图标修改样式后使用的新的 styleID
	std::multimap<int,long> m_ModifySet_; //key是样式ID，long是FID集合
	void RenderManyUseOgles(CMarkerStyle* theStyle,DrawEngine *den = NULL);

	//需要修改的样式的点，初始化为false
	bool m_ModifyStyle_;
	bool m_query_show;
	bool m_query_hide;

	bool m_IsBus; //表示该热点图层 是不是车辆，默认是false，当解析bus.xml时置为true
	bool m_addStyle; //是否增删过样式
	std::set<int> m_append_style;  //添加使用的样式种类
	std::multimap<int,StylePointID> m_styleID_to_Fid;
	CMarkerElement *theElement; //纹理贴图的基础类
	DrawEngine *den;
};

class StylePointID
{
	public:
		int fid;
		int PosId;
};

inline bool operator<(const StylePointID &lhs,const StylePointID &rhs)
{
	return lhs.fid == rhs.fid? (lhs.PosId <rhs.PosId):(lhs.fid<rhs.fid);
}

inline bool operator==(const StylePointID &lhs,const StylePointID &rhs)
{
	if(lhs.fid == rhs.fid && lhs.PosId == rhs.PosId)
		return true;
   else
		return false;
}