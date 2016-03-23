#pragma once
#include "MarkerStyle.h"
#include "Element.h"
class CMarkerElement:public CElement
{
public:
	CMarkerElement(DrawEngine *den=NULL);
	~CMarkerElement(void);

	CMarkerStyle *m_markerstyle;//��ʽ
	double m_X;
	double m_Y;
	double m_Z;
	unsigned char m_Indices[6];//�ı��ε�����
	unsigned int m_ibID;//����id
	bool m_gen_vboID; //�Ƿ����vboID��ʼ����true
	unsigned int m_vboID;//����id
	Vertex m_verts[6];//�ı��εĶ�������
	bool m_Visiable;//�����Ƿ���ʾ
	double m_Angle;
	float m_AngleP[16];

	int m_rendW;//������Ⱦ�Ŀ����ص�λ
	int m_rendH;//������Ⱦ�ĸߣ����ص�λ

	void SetVertex(Vertex *verts);//���ö�������
	void SetVertex(double *verts);//���ö�������
	void SetVertex(double x,double y,double z);//���ö�������
	void SetVertex();
	void RenderUseOgles();//��ȾҪ�أ�δʹ�õ�
	void RenderManyUseOgles(std::vector<Vertex>& theVertexVector); //ʹ����ʽ m_markerstyle ȥ���� [�ȵ�]
};

