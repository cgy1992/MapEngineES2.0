#pragma once
#include "element.h"
#include "DrawEngine.h"
class CCircleElement:public CElement
{
public:
	CCircleElement(DrawEngine *den = NULL);
	~CCircleElement(void);

	double m_CenX;//Բ��x,����γ��
	double m_CenY;//Բ��y,����γ��
	double m_Rinner;//�ڰ뾶
	double m_Router;//��뾶
	int m_r;//��ɫr
	int m_g;//��ɫg
	int m_b;//��ɫb
	int m_a;//͸����a

    unsigned int m_vboID;//����id
    unsigned int m_iboID;//����
	void RenderUseOgles();

	//test:�㻺�忪��Ƶ������,ͨ��������ʾһ��Բ,Ȼ����ʾԲ�ڵ��ȵ�ͼ��
	//��ֵΪfalseʱ,���������ͨ��Բelement
	bool m_bBufferVedio;
	//void RenderUseOglesForVideo();
	DrawEngine *den;
};

