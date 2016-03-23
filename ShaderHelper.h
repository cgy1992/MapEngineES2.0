#pragma once
#include "Tile.h"
#include "Element.h"
#include "DrawEngine.h"
//class CShaderHelper
//{
//public:
//	CShaderHelper(void);
//	~CShaderHelper(void);
//
//	friend class CTile;
//	friend class CElement;
//	//friend class DrawEngine;
//
//	//着色器相关变量
//	//栅格显示
//	GLuint m_programObj;
//	GLuint m_vertexShader;
//	GLuint m_fragmentShader;
//	GLuint  m_positionLoc;
//	GLuint  m_texCoordLoc;
//	GLuint  m_colorLoc;
//	GLuint  m_MVPLoc;
//	GLuint  m_vboID;
//	GLuint  m_ibID;
//
//	//矢量显示
//	GLuint m_programObj_Shp;
//	GLuint m_fragmentShader_Shp;
//	GLuint m_vertexShader_Shp;
//	GLuint m_positionLoc_Shp;
//	GLuint m_texCoordLoc_Shp;
//	GLuint m_colorLoc_Shp;
//	GLuint m_MVPLoc_Shp;
//
//	GLuint m_textureLoc;
//	GLuint m_textureID;
//
//
//	//圆点显示
//	GLuint m_CPvertexShader ;
//	GLuint m_CPfragmentShader;
//	GLuint m_programObj_CP;
//	GLuint m_positionLoc_CP;
//	GLuint m_colorLoc_CP;
//	GLuint m_MVPLoc_Cp;
//	GLuint m_texCoordLoc_CP;
//
//	GLuint compileShader(GLenum type, const char *data) {
//		const char *shaderStrings[1];
//		shaderStrings[0] = data;
//
//		GLuint shader = glCreateShader(type);
//		glShaderSource(shader, 1, shaderStrings, NULL );
//		glCompileShader(shader);
//		GLint compiled;
//		glGetShaderiv(shader,GL_COMPILE_STATUS,&compiled);
//		if (!compiled)
//		{
//			GLint infoLen = 0;
//			glGetShaderiv(shader,GL_INFO_LOG_LENGTH,&infoLen);
//			if (infoLen > 1)
//			{
//				char* infolog = new char(sizeof(char)*infoLen);;
//				glGetShaderInfoLog(shader,infoLen,NULL,infolog);
//				delete[] infolog;
//				infolog = NULL;
//			}
//			glDeleteShader(shader);
//		}
//		return shader;
//	}
//
//	void initcode()
//	{
//		//栅格顶点着色器
//		m_VShaderData = "uniform mat4 a_MVP;"
//			"attribute vec2 a_texCoord;"
//			//"attribute vec3 a_color;"
//			"attribute vec4 a_position;"
//			//"varying  vec3 v_color;"
//			"varying  vec2 v_texCoord;"
//			"void main()" 
//			"{"
//			"gl_PointSize = 1.0;"
//			"gl_Position = a_MVP*a_position;"
//			//"v_color = a_color;"
//			"v_texCoord = a_texCoord;"
//			"}";
//		//栅格片段着色器
//		m_FShaderData = "precision mediump float;"
//			"varying vec3 v_color;"
//			"varying vec2 v_texCoord;"
//			"uniform sampler2D s_texture;"
//			//"uniform float uAlpha;"
//			"void main()"
//			"{"
//			//"gl_FragColor = texture2D( s_texture, vec2(v_texCoord.x, v_texCoord.y) )+ vec4(v_color.x*0.0,v_color.y*0.0,v_color.z*0.0,0);"
//			//"gl_FragColor = vec4(1.0,0.5,0.5,1.0);"
//			"gl_FragColor = texture2D( s_texture, vec2(v_texCoord.x, v_texCoord.y) );"
//			"}";
//
//		//矢量的片元着色
//		//矢量的定点着色
//		m_VShaderShpData = "uniform mat4 a_MVP;"
//			//"attribute vec2 a_texCoord;"
//			"attribute vec4 a_color;"
//			"attribute vec4 a_position;"
//			"varying  vec4 v_color;"
//			//"varying  vec2 v_texCoord;"
//			"void main()" 
//			"{"
//			"gl_PointSize = 1.0;"
//			"gl_Position = a_MVP*a_position;"
//			"v_color = a_color;"
//			//"v_texCoord = a_texCoord;"
//			"}";
//
//		m_FShaderShpData = "precision mediump float;"   //设置显示模式
//			"varying vec4 v_color;"
//			//"varying vec2 v_texCoord;"
//			//"uniform sampler2D s_texture;"
//			"void main()"
//			"{ "
//			//"gl_FragColor = texture2D( s_texture, vec2(v_texCoord.x, v_texCoord.y) );"
//			//"gl_FragColor = vec4(v_color.x,v_color.y,v_color.z,1.0);"
//			"gl_FragColor = v_color;"
//			"}";
//
//
//		//圆点着色器:vertex 与 片段
//		m_VShaderCirclePointData = "uniform mat4 a_MVP;"
//			//"attribute vec2 a_texCoord;"
//			"attribute vec4 a_color;"
//			"attribute vec4 a_position;"
//			"attribute float pointsize;"    //attribute 表示着色器的input
//			"varying  vec4 v_color;"
//			//"varying  vec2 v_texCoord;"
//			"void main()" 
//			"{"
//			//"gl_PointSize = 10.0;"
//			"gl_PointSize = pointsize;"  // 原来这里的是写死了，现在改用变量，解决了自由绘笔 画线 任意宽度的问题
//			"gl_Position = a_MVP*a_position;"
//			"v_color = a_color;"
//			//"v_texCoord = a_texCoord;"
//			"}";
//
//		m_FShaderCirclePointData = "precision mediump float;"
//			"varying vec4 v_color;"
//			//"varying vec2 v_texCoord;"
//			//"uniform sampler2D s_texture;"
//			//"uniform float uAlpha;"
//			//"vec4 c_color;"
//			"void main()"
//			"{ "
//			"gl_FragColor = v_color;"
//			"if(length(gl_PointCoord-vec2(0.5)) > 0.5)"
//			"discard;"
//			"}";
//	}
//
//	void InitProgram( void )
//	{
//		//glSetCurrentContextPPAPI(m_context);
//
//		/************************part 1:*********************************************************/
//		m_vertexShader = compileShader(GL_VERTEX_SHADER, m_VShaderData);//栅格的顶点着色器
//		m_fragmentShader = compileShader(GL_FRAGMENT_SHADER, m_FShaderData);
//		//初始化着色器：纹理
//		m_programObj = glCreateProgram();
//		glAttachShader(m_programObj, m_vertexShader);
//		glAttachShader(m_programObj, m_fragmentShader);
//		glLinkProgram(m_programObj);
//
//		//判断是否连接上
//		GLint link;
//		glGetProgramiv(m_programObj,GL_LINK_STATUS,&link);
//		if (!link)
//		{
//			glDeleteProgram(m_programObj);
//			m_programObj = 0;
//			return;
//		}
//		m_positionLoc = glGetAttribLocation(m_programObj, "a_position");
//		m_texCoordLoc = glGetAttribLocation(m_programObj, "a_texCoord");
//		//m_colorLoc = glGetAttribLocation(m_programObj, "a_color");
//		m_MVPLoc = glGetUniformLocation(m_programObj, "a_MVP");
//
//
//		/************************part 2:**********************************************/
//		//初始化着色器：矢量
//		m_vertexShader_Shp = compileShader(GL_VERTEX_SHADER, m_VShaderShpData);//矢量的顶点着色器
//		m_fragmentShader_Shp = compileShader(GL_FRAGMENT_SHADER,m_FShaderShpData);
//		m_positionLoc_Shp = glCreateProgram();
//		glAttachShader(m_positionLoc_Shp, m_vertexShader_Shp);
//		glAttachShader(m_positionLoc_Shp, m_fragmentShader_Shp);
//		glLinkProgram(m_positionLoc_Shp);
//
//		//在这里是不是应该加入是否连接上的判断  GLint link
//
//		m_positionLoc_Shp = glGetAttribLocation(m_positionLoc_Shp, "a_position");
//		//m_texCoordLoc_Shp = glGetAttribLocation(den->m_positionLoc_Shp, "a_texCoord");
//		m_colorLoc_Shp = glGetAttribLocation(m_positionLoc_Shp, "a_color");
//		m_MVPLoc_Shp = glGetUniformLocation(m_positionLoc_Shp, "a_MVP");
//	}
//};

