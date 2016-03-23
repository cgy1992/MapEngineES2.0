#pragma once
#include "element.h"
#include "DrawEngine.h"

class CGifElement :
	public CElement
{
public:
	CGifElement(CString path,double x,double y,int w,int h,DrawEngine *den = NULL);
	~CGifElement(void);

	CString m_gifpath;//gif路径
	double m_x;
	double m_y;
	int m_width;//gif宽
	int m_height;//gif高
	int m_delay;//间隔
	int m_frameCount;//帧数
	int m_rendW;
	int m_rendH;
	bool m_existfile;

	Vertex m_gifverts[4];//顶点
	unsigned char m_Indices[6];//索引
	
	list<char*> m_giflistdata;//每帧解码后的数据
	GLuint* m_gifTexture;//每帧的纹理号
	GLuint m_gifvboID;//顶点数组
	GLuint m_gifibID;//索引数据
	int m_Currentid;//当前帧id
	double m_tmp;//辅助计算当前应该显示第几帧

	DrawEngine *den;
	void RenderUseOgles(double delaytime);
};

