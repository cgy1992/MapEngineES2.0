#pragma once
#include "Element.h"
#include <map>
#include <string>
#include "stdafx.h"  //����ʹ��MFC�е���

//������Ľ����
class CNode:public CObject
{
public:
	int index;//��������
	int x,y;//x��y����ֵ
	bool out;//���⻷ Ĭ�����⻷
	int NO_in;//�ڻ���ţ��⻷Ϊ0���ڻ���1��ʼ
	bool head;//�Ƿ�ͷ
	bool tail;//�Ƿ�β
public:
	CNode()
	{
		index=0;
		x=0;
		y=0;
		out=TRUE;
		NO_in=0;
		head=FALSE;
		tail=FALSE;
	}//���ʼ��
	CNode(CNode *temp)
	{
		index=temp->index;
		x=temp->x;
		y=temp->y;
		out=temp->out;
		NO_in=temp->NO_in;
		head=temp->head;
		tail=temp->tail;
	}
	CNode(CNode &n)
	{
		index=n.index;
		x=n.x;
		y=n.y;
		out=n.out;
		NO_in=n.NO_in;
		head=n.head;
		tail=n.tail;
	}
	CNode& operator =(CNode n)
	{
		index=n.index;
		x=n.x;
		y=n.y;
		out=n.out;
		NO_in=n.NO_in;
		head=n.head;
		tail=n.tail;
		return *this;
	}
};

//����������Ľ����
class CTriange:public CObject
{
public:
	int index;//��������
	int L1,L2,L3;//�����ε��������������
public:
	CTriange()
	{
		index=0;
		L1=0;
		L2=0;
		L3=0;
	}//���ʼ��
	CTriange(CTriange *temp)
	{
		index=temp->index;
		L1=temp->L1;
		L2=temp->L2;
		L3=temp->L3;
	}
	CTriange(CTriange &t)
	{
		index=t.index;
		L1=t.L1;
		L2=t.L2;
		L3=t.L3;
	}
	CTriange& operator =(CTriange t)
	{
		index=t.index;
		L1=t.L1;
		L2=t.L1;
		L3=t.L3;
		return *this;
	}
};
//������ı���
class CEdge:public CObject
{
public:
	int index;//��������
	int L1,L2;//�ߵ��������������
public:
	CEdge()
	{
		index=0;
		L1=0;
		L2=0;
	}//���ʼ��
	CEdge(CEdge *temp)
	{
		index=temp->index;
		L1=temp->L1;
		L2=temp->L2;
	}
	CEdge(CEdge &t)
	{
		index=t.index;
		L1=t.L1;
		L2=t.L2;
	}
	CEdge& operator =(CEdge t)
	{
		index=t.index;
		L1=t.L1;
		L2=t.L2;
		return *this;
	}
};
//������ͼ�ı�����ı���
class CEdge2:public CObject
{
public:
	int color;//��������
	int x1,y1,x2,y2;//�ߵ������������
public:
	CEdge2()
	{
		color=1;
		x1=0;
		y1=0;
		x2=0;
		y2=0;
	}//���ʼ��
	CEdge2(CEdge2 *temp)
	{
		color=temp->color;
		x1=temp->x1;
		y1=temp->y1;
		x2=temp->x2;
		y2=temp->y2;
	}
	CEdge2(CEdge2 &t)
	{
		color=t.color;
		x1=t.x1;
		y1=t.y1;
		x2=t.x2;
		y2=t.y2;
	}
	CEdge2& operator =(CEdge2 t)  //�����˸�ֵ�����
	{
		color=t.color;
		x1=t.x1;
		y1=t.y1;
		x2=t.x2;
		y2=t.y2;
		return *this;
	}
};
//ֱ�߷��̽ṹ
class CLine
{
public:
    double k;    //б��
    double b;    //�ؾ�
};

/******************************************************************************************************************/
class CWFSPolyFeature
{
public:
	CWFSPolyFeature():currentnode(0),currentedge(0),waihuan(0),neihuan(0),N(0),k(0),wai(true),tri_index(0){}
	~CWFSPolyFeature() {}
	std::vector<MapEngineES_Point> m_PointList_;		// ���б�
	std::map<std::string, std::string> m_PolyFeature;	// ����
public:
	void InitPolyVertex(DrawEngine *den = NULL); 
	void OnDelaunay();
	void fun1();
	void fun2(int &k);
	void fun3(int &k);
	void fun4(int &k);
	void fun5(int &k);
public:    //˽������
	double const static pai ;  //�ೣ��
	CObList *mpNodeList,*mpTriList,*mpCandidateNode,*mpCandidateNode2,*mpEdgeList;
	int currentnode,currentedge,waihuan,neihuan,tri_index;
	int N;//��ǰ����α���
	int k;//��ǰ�������
	bool wai; //�õ��Ƿ����⻷��
	static bool in_start; //��bool�����ڻ����� �Ƿ�ʼ��
	POINT apt2[3];//���һ�������εĶ���
	CEdge2 last[3]; //����������

	CNode *pL11,*pL12,*pLk1,*pLk2,*pLo1,*pLo2,*pLm1,*pLm2,*pLn1,*pLn2;//���ߵ�ʼ�յ�
	CEdge *pLk,*pL1,*pLo,*pLm,*pLn;//����

	CNode p1,p2,p3;
	CNode m1,m2,m3;
	CLine l1,l2,l3,lm1,lm2,lm3;
	CNode circlep;
	double circler,dis2,dis3;

private:  //˽�з���
	bool isEdge(CNode*,CNode*,int&);//�ж�ĳ�����Ƿ����εıߣ�����ǣ���������
	int WhichSide(CNode*,CNode*,CNode*);//����ֱ���Ĳ�
	double waixin(CNode*,CNode*,CNode*);//�����Բ�뾶
	int	Area2(CNode*,CNode*,CNode*);//���������������������
	bool ToLeft(CNode*,CNode*,CNode*);//�ж�ĳ���Ƿ���һ��ֱ�ߵ����
	void findnode(CEdge*,CNode*&,CNode*&);//�ҵ�ĳ�ߵ�ʼ�յ�
	void findedge(int,CEdge*&);//�������ҵ�ĳ����
	bool samep(CNode*,CNode*);//�ж������Ƿ���ͬ
	CNode midpoint(CNode*, CNode*);//��������е�
	CLine line2p(CNode*,CNode*);//������ȷ����ֱ��
	bool isatline(CLine,CNode*);//�ж�ĳ���Ƿ���ֱ����
	CLine linepoint(CNode,double,double); //��ĳ��ֱ�߷���
	CNode crosspoint(CLine,CLine);//����ֱ�߽���
	double dist(CNode&,CNode&);//���������

	bool IsCanExistInCircle(CNode* pL11,CNode* pL12,CNode* pLo2);
	void findLo2(CNode* pL11,CNode* pL12,CNode*& pLo2,CObList *pCandidateList);
	bool intersect(CNode* pP1,CNode* pP2,CNode* pP3,CNode* pP4);
	bool iscross(CLine l1,CLine l2);
};
const double CWFSPolyFeature::pai  = 3.1415926535897932384626;
bool CWFSPolyFeature::in_start = false;

class CWFSPolyFeatureSet
{
public:
	CWFSPolyFeatureSet()
		: m_PolyHandle_(-1)
	{
	}
	~CWFSPolyFeatureSet()
	{
		RemoveAllFeature();
	}
	std::map<long, CWFSPolyFeature*> m_DataSet_; //����Poly��FID����ţ�һ��pair����һ�������

	void RenderManyUseOgles(int width, const int& rcolor, const int& gcolor, const int& bcolor, const int& acolor,DrawEngine *den = NULL);

	void RemoveAllFeature();
private:
	long m_PolyHandle_;
};
/******************************************************************************************************************/