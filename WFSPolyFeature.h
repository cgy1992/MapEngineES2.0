#pragma once
#include "Element.h"
#include <map>
#include <string>
#include "stdafx.h"  //可以使用MFC中的类

//点链表的结点类
class CNode:public CObject
{
public:
	int index;//点索引号
	int x,y;//x，y坐标值
	bool out;//内外环 默认是外环
	int NO_in;//内环编号，外环为0，内环从1开始
	bool head;//是否环头
	bool tail;//是否环尾
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
	}//零初始化
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

//三角形链表的结点类
class CTriange:public CObject
{
public:
	int index;//点索引号
	int L1,L2,L3;//三角形的三个点的索引号
public:
	CTriange()
	{
		index=0;
		L1=0;
		L2=0;
		L3=0;
	}//零初始化
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
//边链表的边类
class CEdge:public CObject
{
public:
	int index;//边索引号
	int L1,L2;//边的两个点的索引号
public:
	CEdge()
	{
		index=0;
		L1=0;
		L2=0;
	}//零初始化
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
//用来画图的边链表的边类
class CEdge2:public CObject
{
public:
	int color;//边索引号
	int x1,y1,x2,y2;//边的两个点的坐标
public:
	CEdge2()
	{
		color=1;
		x1=0;
		y1=0;
		x2=0;
		y2=0;
	}//零初始化
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
	CEdge2& operator =(CEdge2 t)  //重载了赋值运算符
	{
		color=t.color;
		x1=t.x1;
		y1=t.y1;
		x2=t.x2;
		y2=t.y2;
		return *this;
	}
};
//直线方程结构
class CLine
{
public:
    double k;    //斜率
    double b;    //截距
};

/******************************************************************************************************************/
class CWFSPolyFeature
{
public:
	CWFSPolyFeature():currentnode(0),currentedge(0),waihuan(0),neihuan(0),N(0),k(0),wai(true),tri_index(0){}
	~CWFSPolyFeature() {}
	std::vector<MapEngineES_Point> m_PointList_;		// 点列表
	std::map<std::string, std::string> m_PolyFeature;	// 属性
public:
	void InitPolyVertex(DrawEngine *den = NULL); 
	void OnDelaunay();
	void fun1();
	void fun2(int &k);
	void fun3(int &k);
	void fun4(int &k);
	void fun5(int &k);
public:    //私有属性
	double const static pai ;  //类常量
	CObList *mpNodeList,*mpTriList,*mpCandidateNode,*mpCandidateNode2,*mpEdgeList;
	int currentnode,currentedge,waihuan,neihuan,tri_index;
	int N;//当前多边形边数
	int k;//当前处理边数
	bool wai; //该点是否是外环的
	static bool in_start; //该bool用于内环输入 是否开始？
	POINT apt2[3];//最后一个三角形的顶点
	CEdge2 last[3]; //最后的三条边

	CNode *pL11,*pL12,*pLk1,*pLk2,*pLo1,*pLo2,*pLm1,*pLm2,*pLn1,*pLn2;//各边的始终点
	CEdge *pLk,*pL1,*pLo,*pLm,*pLn;//各边

	CNode p1,p2,p3;
	CNode m1,m2,m3;
	CLine l1,l2,l3,lm1,lm2,lm3;
	CNode circlep;
	double circler,dis2,dis3;

private:  //私有方法
	bool isEdge(CNode*,CNode*,int&);//判断某两点是否多边形的边，如果是，是那条边
	int WhichSide(CNode*,CNode*,CNode*);//点在直线哪侧
	double waixin(CNode*,CNode*,CNode*);//求外接圆半径
	int	Area2(CNode*,CNode*,CNode*);//由三点坐标求三角形面积
	bool ToLeft(CNode*,CNode*,CNode*);//判断某点是否在一条直线的左侧
	void findnode(CEdge*,CNode*&,CNode*&);//找到某边的始终点
	void findedge(int,CEdge*&);//由索引找到某条边
	bool samep(CNode*,CNode*);//判断两点是否相同
	CNode midpoint(CNode*, CNode*);//求两点的中点
	CLine line2p(CNode*,CNode*);//求两点确定的直线
	bool isatline(CLine,CNode*);//判断某点是否在直线上
	CLine linepoint(CNode,double,double); //过某点直线方程
	CNode crosspoint(CLine,CLine);//求两直线交点
	double dist(CNode&,CNode&);//求两点距离

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
	std::map<long, CWFSPolyFeature*> m_DataSet_; //按照Poly的FID来存放，一个pair就是一个多边形

	void RenderManyUseOgles(int width, const int& rcolor, const int& gcolor, const int& bcolor, const int& acolor,DrawEngine *den = NULL);

	void RemoveAllFeature();
private:
	long m_PolyHandle_;
};
/******************************************************************************************************************/