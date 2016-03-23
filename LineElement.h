#pragma once
#include "DataStruct.h"
#include "element.h"
#include "DrawEngine.h"
#include <vector>
using namespace std;
class CLineElement :public CElement
{
public:
	CLineElement(DrawEngine *den = NULL); //常用构造器
	CLineElement(int r,int g,int b,int a,int w=1,DrawEngine *den = NULL,bool bfillPoly = false,const vector<MapEngineES_Point> &line=vector<MapEngineES_Point>()); //添加了w的默认参数，默认是不填充
	~CLineElement(void);

	vector<MapEngineES_Point> m_vctPoint; //注：point不要new出来,由捕获的鼠标 坐标来初始
	int m_r;
	int m_g;
	int m_b;
	int m_a;
	int m_w;
	bool g_binitPoint;
	bool m_PassData; //标识是否向显卡传送数据
	bool m_bFill;
	//0,普通绘制,不需要填拐点；
	//1：自由绘笔，拐点补圆点；
	//2：线缓冲区，拐点补圆；
	//3其它
	int m_type;
	bool m_cast; //坐标需要转换
	std::vector<int> m_LineWidth;
	int widthSize;
	int AddPoint(double x,double y);
	unsigned int m_vboID;//顶点id
	unsigned int m_iboID;//索引

	void RenderUseOgles();
	void RenderManyOgles(std::vector<Vertex>& theTriangleVector,std::vector<Vertex> &pointVec = std::vector<Vertex>());

	//两点一条线
	void DrawLine(double xs,double ys,double xe,double ye,int m_r, int m_g,int m_b,int m_a, int m_w);
	//批量render线
	void DrawLines(int m_r, int m_g,int m_b,int m_a, int m_w);

	void DrawCircle(double CenX,double CenY,double w,double r,double g,double b,double a);//画拐点

	void DrawCirclepat(Vertex* vects_circle,int count,double w,double r,double g,double b,double a);//批量绘制拐点

	//清空m_vctPoint
	void ClearData()
	{
		if(m_vctPoint.size()!=0)
		{
			m_vctPoint.clear();
		}
	}

	//test 创建圆点着色器
	void CreateCirlePointShader();
};

