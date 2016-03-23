#pragma once
#include "stdafxppapi.h"

class ShaderData
{
public:
	GLuint m_VerShader;						//着色器
	GLuint m_FramShader;
	GLuint m_Program;
public:
	ShaderData() 
		: m_VerShader(0)
		, m_FramShader(0)
		, m_Program(0)
	{
	}
	~ShaderData()
	{
		if(m_VerShader > 0)
		{
			glDeleteShader(m_VerShader);
			m_VerShader = 0;
		}
		if (m_FramShader > 0)
		{
			glDeleteShader(m_FramShader);
			m_FramShader = 0;
		}
		if (m_Program > 0)
		{
			glDeleteProgram(m_Program);
			m_Program = 0;
		}
	}
};

class ColorShader
{
public:
	ShaderData m_ShaderData;
	GLint m_PositioinLoc;
	GLint m_ColorLoc;
	GLint m_MvpLoc;
	ColorShader()
		: m_PositioinLoc(-1)
		, m_ColorLoc(-1)
		, m_MvpLoc(-1)
	{}
	~ColorShader(){}

};

class TextureShader
{
public:
	ShaderData m_ShaderData;
	GLint m_PositioinLoc;
	GLint m_MvpLoc;
	GLint m_TexCoord;
	TextureShader()
		: m_PositioinLoc(-1)
		, m_MvpLoc(-1)
		, m_TexCoord(-1)
	{}
	~TextureShader(){}
};



//绑定2D纹理
//纹理号码
//图片的宽高
//图片的数据内容
void Bind2DTextureFromBuffer(GLuint& theTexttureID, int theImageWidth, int theImageHeight, GLubyte* theBuffer)
{
	glBindTexture(GL_TEXTURE_2D, theTexttureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, theImageWidth, theImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, theBuffer);
	//过滤模式
	//放大过滤
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
	//超过TS坐标时候的采样
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}



