#pragma once
#include "element.h"
#include "DrawEngine.h"
class CCircleElement:public CElement
{
public:
	CCircleElement(DrawEngine *den = NULL);
	~CCircleElement(void);

	double m_CenX;//圆心x,地理经纬度
	double m_CenY;//圆心y,地理经纬度
	double m_Rinner;//内半径
	double m_Router;//外半径
	int m_r;//颜色r
	int m_g;//颜色g
	int m_b;//颜色b
	int m_a;//透明度a

    unsigned int m_vboID;//顶点id
    unsigned int m_iboID;//索引
	void RenderUseOgles();

	//test:点缓冲开视频功能中,通过点先显示一个圆,然后显示圆内的热点图标
	//该值为false时,类对象是普通的圆element
	bool m_bBufferVedio;
	//void RenderUseOglesForVideo();
	DrawEngine *den;
};

