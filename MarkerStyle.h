// MarkerStyle.h : Declaration of the CMarkerStyle

#pragma once
#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GLES2/gl2.h>  
#include "Tile.h"
#include "Style.h"

class CMarkerStyle:public CStyle
{
public:
	CMarkerStyle()
	{
		m_ImageFileName = L"";
		m_ImageWidth = 0;
		m_ImageHeight = 0;
		m_EnableTransColor = S_OK;
		m_ImageX = 0;
		m_ImageY = 0;

		m_pdata = NULL;
		m_textureID = 0;
		m_styletype = vPointstyle;
	}

   ~CMarkerStyle()
	{
		if (m_pdata)
		{
			delete[] m_pdata;
			m_pdata = NULL;
		}

		if (m_textureID != 0)
		{
			glDeleteTextures(1,&m_textureID);
			m_textureID = 0;
		}
	}


	CString m_ImageFileName;
	long m_ImageWidth;
	long m_ImageHeight;
	bool m_EnableTransColor;
	long m_ImageX;
	long m_ImageY;

	void Init();
	bool Init(const std::string& theStringData); //本组件 使用的是这个方法
	void Init(CBitmap* thebmp, const int& height, const int& width, bool bRGBA);
	BYTE *m_pdata;
	//byte m_data[256*256*4]; //每个pixel含有4B RGBA
	GLuint m_textureID;  
private:
	bool CreateTheTextFromGdiBitmap(Gdiplus::Bitmap* bitmap);
};
