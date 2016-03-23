#pragma once
#include <iostream>
#include <string>
#include <map>
#include "MarkerElement.h"
#include <set>

 
class StylePointID;  //������

class WFSPointFeatureElement: public CMarkerElement
{
 //�����˸����е� CMarkerStyle *m_markerstyle ���ڸ����б���ʼ��ΪNULL
public:
	WFSPointFeatureElement(){}
	~WFSPointFeatureElement(){}
	double	x_;				//����ʵ�ĵ�������
	double	y_; 
	std::map<std::string, std::string> m_PontFeature;	//����
	//std::vector<int> m_show_flagArr;
	//������WFSPoint��style�ڵ�ͼ����Level�ϵ���ʾ��Ϣ
	std::map<StylePointID,vector<int> > m_show_flagArr;
	struct AppendStylePos; //������������ʽ�������Point���ĵ��λ��offset

	//�������ʹ��multimap 
	std::map<StylePointID,AppendStylePos> m_styleID_to_pos;
//��¼����ʽ��CWFSPointFeatureElementSet::multimap�е�λ�ã�����ɾ������
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
		//���ifҲ��֤�ˣ������ǿ�ʱ�����ᷢ��ɾ������
		if(m_styleID_to_pos.count(styleId)) //ɾ�� ��ʽλ����Ϣ
			m_styleID_to_pos.erase(m_styleID_to_pos.find(styleId));
		if(m_styleID_to_iter.count(styleId)) //ɾ�� ��ʽ�� �ȵ㼯::multimapռ�ݵ�λ��
			m_styleID_to_iter.erase(m_styleID_to_iter.find(styleId));
		if(m_show_flagArr.count(styleId)) //ɾ�� ��ʽ��ĳ������Ƿ���ʾ
			m_show_flagArr.erase(m_show_flagArr.find(styleId));
	}
private:
	struct AppendStylePos
	{
		//ǰ���������������WFSPiont���ĵ�(x,y)��ƫ��������λ��pixel
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
	//���ȵ��Fid �ϳ�����ʽ������Ƭ ��㺯��
	void ComposeVertex(const StylePointID &Fid,std::vector<Vertex> &theVertexVector,DrawEngine *den);
	void ComposeVertex(const WFSPointFeatureElement *theCurrentFeature,std::vector<Vertex> &theVertexVector,DrawEngine *den,int &discardPoint);
private:
	//add one feature
	long AddOneFeature(WFSPointFeatureElement* pFeature);
public:
	std::map<long, WFSPointFeatureElement*> m_DataSet_;
	std::map<long,WFSPointFeatureElement*> m_QueryDataSet_; //query ��Ҫ��ʾ��Line
	std::set<int> m_HidePointSet_; // ��Ҫ��ʾ�� point��FID
	std::set<int> m_Style_; //����ͼ���޸���ʽ��ʹ�õ��µ� styleID
	std::multimap<int,long> m_ModifySet_; //key����ʽID��long��FID����
	void RenderManyUseOgles(CMarkerStyle* theStyle,DrawEngine *den = NULL);

	//��Ҫ�޸ĵ���ʽ�ĵ㣬��ʼ��Ϊfalse
	bool m_ModifyStyle_;
	bool m_query_show;
	bool m_query_hide;

	bool m_IsBus; //��ʾ���ȵ�ͼ�� �ǲ��ǳ�����Ĭ����false��������bus.xmlʱ��Ϊtrue
	bool m_addStyle; //�Ƿ���ɾ����ʽ
	std::set<int> m_append_style;  //���ʹ�õ���ʽ����
	std::multimap<int,StylePointID> m_styleID_to_Fid;
	CMarkerElement *theElement; //������ͼ�Ļ�����
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