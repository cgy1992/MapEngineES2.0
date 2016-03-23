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
//	//��ɫ����ر���
//	//դ����ʾ
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
//	//ʸ����ʾ
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
//	//Բ����ʾ
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
//		//դ�񶥵���ɫ��
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
//		//դ��Ƭ����ɫ��
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
//		//ʸ����ƬԪ��ɫ
//		//ʸ���Ķ�����ɫ
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
//		m_FShaderShpData = "precision mediump float;"   //������ʾģʽ
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
//		//Բ����ɫ��:vertex �� Ƭ��
//		m_VShaderCirclePointData = "uniform mat4 a_MVP;"
//			//"attribute vec2 a_texCoord;"
//			"attribute vec4 a_color;"
//			"attribute vec4 a_position;"
//			"attribute float pointsize;"    //attribute ��ʾ��ɫ����input
//			"varying  vec4 v_color;"
//			//"varying  vec2 v_texCoord;"
//			"void main()" 
//			"{"
//			//"gl_PointSize = 10.0;"
//			"gl_PointSize = pointsize;"  // ԭ���������д���ˣ����ڸ��ñ�������������ɻ�� ���� �����ȵ�����
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
//		m_vertexShader = compileShader(GL_VERTEX_SHADER, m_VShaderData);//դ��Ķ�����ɫ��
//		m_fragmentShader = compileShader(GL_FRAGMENT_SHADER, m_FShaderData);
//		//��ʼ����ɫ��������
//		m_programObj = glCreateProgram();
//		glAttachShader(m_programObj, m_vertexShader);
//		glAttachShader(m_programObj, m_fragmentShader);
//		glLinkProgram(m_programObj);
//
//		//�ж��Ƿ�������
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
//		//��ʼ����ɫ����ʸ��
//		m_vertexShader_Shp = compileShader(GL_VERTEX_SHADER, m_VShaderShpData);//ʸ���Ķ�����ɫ��
//		m_fragmentShader_Shp = compileShader(GL_FRAGMENT_SHADER,m_FShaderShpData);
//		m_positionLoc_Shp = glCreateProgram();
//		glAttachShader(m_positionLoc_Shp, m_vertexShader_Shp);
//		glAttachShader(m_positionLoc_Shp, m_fragmentShader_Shp);
//		glLinkProgram(m_positionLoc_Shp);
//
//		//�������ǲ���Ӧ�ü����Ƿ������ϵ��ж�  GLint link
//
//		m_positionLoc_Shp = glGetAttribLocation(m_positionLoc_Shp, "a_position");
//		//m_texCoordLoc_Shp = glGetAttribLocation(den->m_positionLoc_Shp, "a_texCoord");
//		m_colorLoc_Shp = glGetAttribLocation(m_positionLoc_Shp, "a_color");
//		m_MVPLoc_Shp = glGetUniformLocation(m_positionLoc_Shp, "a_MVP");
//	}
//};

