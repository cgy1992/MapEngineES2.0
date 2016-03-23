//��ͼ��Ƭ
#pragma once

#include "stdafx.h"


//#include <GLES2/gl2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ppapi/c/pp_stdint.h"
#include "ppapi/c/pp_completion_callback.h"
#include "ppapi/c/pp_errors.h"
//#include "ppapi/c/pp_graphics_3d.h"
#include "ppapi/cpp/graphics_3d.h"
#include "ppapi/c/pp_module.h"
#include "ppapi/c/pp_var.h"
#include "ppapi/c/ppb.h"
#include "ppapi/c/ppb_core.h"
#include "ppapi/c/ppb_graphics_3d.h"
#include "ppapi/c/ppb_instance.h"
#include "ppapi/c/ppb_messaging.h"
#include "ppapi/c/ppb_opengles2.h"
#include "ppapi/c/ppb_var.h"
#include "ppapi/c/ppp.h"
#include "ppapi/c/ppp_instance.h"
#include "ppapi/c/ppp_messaging.h"
#include "ppapi/c/ppb_url_loader.h"
#include "ppapi/c/ppb_url_request_info.h"
#include "ppapi/cpp/fullscreen.h"

#include "ppapi/c/ppp_graphics_3d.h"
#include "ppapi/lib/gl/gles2/gl2ext_ppapi.h"
#include "ppapi/c/ppb_graphics_2d.h"
#include "matrix.h"
#include "DataStruct.h"



class Tile
{
public:
	Tile(CString sTileName,Vertex *verts,int w,int h,CString pictype,int js,int rowid,int colid);
	Tile(CString sTileName,const char* picdata,Vertex *verts,int w,int h,CString pictype,int js,int rowid,int colid);
	Tile(CString sTileName,long x,long y,int w,int h);
	~Tile(void);

	unsigned int m_TileW;
	unsigned int m_TileH;
	double m_X;//δʹ�ã�����
	double m_Y;//δʹ�ã�����
	double m_Z;//δʹ�ã�����
	int m_js;
	int m_rowid;
	int m_colid;

	unsigned int  m_positionLoc;
	unsigned int  m_texCoordLoc;
	unsigned int  m_colorLoc;
	unsigned int  m_MVPLoc;
	//VBO��IBO����
	unsigned int  m_vboID;
	unsigned int  m_ibID;
	unsigned int m_textureLoc;
	unsigned int m_textureID;
	Vertex m_verts[4];    //��������� ��С�ǲ���Ӧ���� 4��������

	int   v_n; //������� ���ڹ������г�ʼ��Ϊ 4
	int   i_n ; //index����
	unsigned char m_Indices[6]; //��������

	CString m_tileName;
	//byte m_data[256*256*4];//Ŀǰֻ֧��256*256�ĵ�ͼ,��Ҫ�Ľ�
	BYTE* m_pdata;//������������tile����,�ڲ����decodeʱ�������ݽ���:�����������ⲿ����ռ�,������(�ڲ�)�ͷ�
	CString m_pictype;//ͼƬ��ʽ:jpg png ...
	GLint m_sformat;//rbg,rbga
	char* m_sourcedata;//�ļ�����
	int m_len;

	int g_context; //���ڱ�ʶ��tile���ڸ�DrawEngine
	void init(int g_context = 0); //����������Ҫ��glTexImage2D�����ĵ���
	
	//���ü��� ��ͼ�ļ��ζ�������
	void SetVertex(Vertex *verts)
	{
		//copy����ڴ�
		memcpy(&m_verts[0],&verts[0],sizeof(m_verts));
		glBindBuffer(GL_ARRAY_BUFFER, m_vboID); //��һ��invoke���Է����ڴ�ռ�
		glBufferData(GL_ARRAY_BUFFER, 4* sizeof(Vertex), (void*)&m_verts,GL_STATIC_DRAW);
	}

	void RenderUseOgles();
};

