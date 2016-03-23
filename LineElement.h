#pragma once
#include "DataStruct.h"
#include "element.h"
#include "DrawEngine.h"
#include <vector>
using namespace std;
class CLineElement :public CElement
{
public:
	CLineElement(DrawEngine *den = NULL); //���ù�����
	CLineElement(int r,int g,int b,int a,int w=1,DrawEngine *den = NULL,bool bfillPoly = false,const vector<MapEngineES_Point> &line=vector<MapEngineES_Point>()); //�����w��Ĭ�ϲ�����Ĭ���ǲ����
	~CLineElement(void);

	vector<MapEngineES_Point> m_vctPoint; //ע��point��Ҫnew����,�ɲ������� ��������ʼ
	int m_r;
	int m_g;
	int m_b;
	int m_a;
	int m_w;
	bool g_binitPoint;
	bool m_PassData; //��ʶ�Ƿ����Կ���������
	bool m_bFill;
	//0,��ͨ����,����Ҫ��յ㣻
	//1�����ɻ�ʣ��յ㲹Բ�㣻
	//2���߻��������յ㲹Բ��
	//3����
	int m_type;
	bool m_cast; //������Ҫת��
	std::vector<int> m_LineWidth;
	int widthSize;
	int AddPoint(double x,double y);
	unsigned int m_vboID;//����id
	unsigned int m_iboID;//����

	void RenderUseOgles();
	void RenderManyOgles(std::vector<Vertex>& theTriangleVector,std::vector<Vertex> &pointVec = std::vector<Vertex>());

	//����һ����
	void DrawLine(double xs,double ys,double xe,double ye,int m_r, int m_g,int m_b,int m_a, int m_w);
	//����render��
	void DrawLines(int m_r, int m_g,int m_b,int m_a, int m_w);

	void DrawCircle(double CenX,double CenY,double w,double r,double g,double b,double a);//���յ�

	void DrawCirclepat(Vertex* vects_circle,int count,double w,double r,double g,double b,double a);//�������ƹյ�

	//���m_vctPoint
	void ClearData()
	{
		if(m_vctPoint.size()!=0)
		{
			m_vctPoint.clear();
		}
	}

	//test ����Բ����ɫ��
	void CreateCirlePointShader();
};

