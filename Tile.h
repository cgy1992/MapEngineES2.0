//地图切片
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
	double m_X;//未使用，保留
	double m_Y;//未使用，保留
	double m_Z;//未使用，保留
	int m_js;
	int m_rowid;
	int m_colid;

	unsigned int  m_positionLoc;
	unsigned int  m_texCoordLoc;
	unsigned int  m_colorLoc;
	unsigned int  m_MVPLoc;
	//VBO与IBO对象
	unsigned int  m_vboID;
	unsigned int  m_ibID;
	unsigned int m_textureLoc;
	unsigned int m_textureID;
	Vertex m_verts[4];    //这里的数组 大小是不是应该是 4，包括了

	int   v_n; //顶点个数 ，在构造器中初始化为 4
	int   i_n ; //index个数
	unsigned char m_Indices[6]; //索引数组

	CString m_tileName;
	//byte m_data[256*256*4];//目前只支持256*256的底图,需要改进
	BYTE* m_pdata;//层对象包含若干tile对象,在层进行decode时传递数据进来:即该数据在外部申请空间,在析构(内部)释放
	CString m_pictype;//图片格式:jpg png ...
	GLint m_sformat;//rbg,rbga
	char* m_sourcedata;//文件数据
	int m_len;

	int g_context; //用于辨识这tile属于个DrawEngine
	void init(int g_context = 0); //设置纹理主要是glTexImage2D函数的调用
	
	//设置集合 贴图的几何顶点坐标
	void SetVertex(Vertex *verts)
	{
		//copy这块内存
		memcpy(&m_verts[0],&verts[0],sizeof(m_verts));
		glBindBuffer(GL_ARRAY_BUFFER, m_vboID); //第一次invoke可以分配内存空间
		glBufferData(GL_ARRAY_BUFFER, 4* sizeof(Vertex), (void*)&m_verts,GL_STATIC_DRAW);
	}

	void RenderUseOgles();
};

