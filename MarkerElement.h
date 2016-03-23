#pragma once
#include "MarkerStyle.h"
#include "Element.h"
class CMarkerElement:public CElement
{
public:
	CMarkerElement(DrawEngine *den=NULL);
	~CMarkerElement(void);

	CMarkerStyle *m_markerstyle;//样式
	double m_X;
	double m_Y;
	double m_Z;
	unsigned char m_Indices[6];//四边形的索引
	unsigned int m_ibID;//索引id
	bool m_gen_vboID; //是否分配vboID初始化是true
	unsigned int m_vboID;//顶点id
	Vertex m_verts[6];//四边形的顶点数组
	bool m_Visiable;//对象是否显示
	double m_Angle;
	float m_AngleP[16];

	int m_rendW;//对象渲染的宽，像素单位
	int m_rendH;//对象渲染的高，像素单位

	void SetVertex(Vertex *verts);//设置顶点数组
	void SetVertex(double *verts);//设置顶点数组
	void SetVertex(double x,double y,double z);//设置顶点数组
	void SetVertex();
	void RenderUseOgles();//渲染要素，未使用到
	void RenderManyUseOgles(std::vector<Vertex>& theVertexVector); //使用样式 m_markerstyle 去绘制 [热点]
};

