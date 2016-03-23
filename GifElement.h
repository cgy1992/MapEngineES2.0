#pragma once
#include "element.h"
#include "DrawEngine.h"

class CGifElement :
	public CElement
{
public:
	CGifElement(CString path,double x,double y,int w,int h,DrawEngine *den = NULL);
	~CGifElement(void);

	CString m_gifpath;//gif·��
	double m_x;
	double m_y;
	int m_width;//gif��
	int m_height;//gif��
	int m_delay;//���
	int m_frameCount;//֡��
	int m_rendW;
	int m_rendH;
	bool m_existfile;

	Vertex m_gifverts[4];//����
	unsigned char m_Indices[6];//����
	
	list<char*> m_giflistdata;//ÿ֡����������
	GLuint* m_gifTexture;//ÿ֡�������
	GLuint m_gifvboID;//��������
	GLuint m_gifibID;//��������
	int m_Currentid;//��ǰ֡id
	double m_tmp;//�������㵱ǰӦ����ʾ�ڼ�֡

	DrawEngine *den;
	void RenderUseOgles(double delaytime);
};

