// MarkerStyle.cpp : Implementation of CMarkerStyle

#include "stdafx.h"
#include "MarkerStyle.h"
#include "Tile.h"

extern GLuint  g_positionLoc;
extern GLuint  g_texCoordLoc;
extern GLuint  g_colorLoc;
extern GLuint  g_MVPLoc;

//// CMarkerStyle 从本地 读取样式图片
void CMarkerStyle::Init()
{
	if (m_ImageFileName.Compare(L"") == 0)
	{
		return;
	}
	// Read the texture bits
	HBITMAP hBitMap; //SDK 位图句柄
	Gdiplus::Bitmap* bmp = Bitmap::FromFile(m_ImageFileName);  //没有考虑图片不存在的情况
	m_ImageWidth=bmp->GetWidth();
	m_ImageHeight=bmp->GetHeight();
	
	if (m_pdata)
	{
		delete[] m_pdata;
		m_pdata = NULL;
	}
	m_pdata = new BYTE[m_ImageWidth*m_ImageHeight*4];//RGBA 1个pixel 32bit

	bmp->GetHBITMAP(Color::White,&hBitMap); //从GDI+对象中 生成位图句柄
	CBitmap* cbmp = CBitmap::FromHandle(hBitMap); //从句柄中 生成mfc中的类
	DWORD size = cbmp->GetBitmapBits(m_ImageWidth*m_ImageHeight*4,m_pdata);
	for (int i=0;i<m_ImageWidth*m_ImageHeight;i++)
	{
		BYTE tmp = (m_pdata[i*4]);
		m_pdata[i*4+0] = (m_pdata[i*4+2]); //r
		m_pdata[i*4+2] = tmp;//b
	}

	cbmp->DeleteObject();
	DeleteObject(hBitMap);
	DeleteObject(bmp);
	delete bmp;
	bmp = NULL;

	glGenTextures(1, &m_textureID);  //就是这里出错了
	glBindTexture(GL_TEXTURE_2D, m_textureID);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA,  m_ImageWidth, m_ImageHeight , 0, GL_RGBA,GL_UNSIGNED_BYTE ,m_pdata );

}

//从网络上获取的图片，会使用这个函数来读图片
bool CMarkerStyle::Init(const std::string& theStringData)
{
	if(theStringData.length() == 0)
		return false;
	//内存分配，该api不提倡使用，该使用新的heap manage function
	HGLOBAL global = ::GlobalAlloc(GMEM_MOVEABLE, theStringData.length());
	if(global == NULL)
		return false;
	bool theResult = false;
	void* theBuffer = GlobalLock(global);
	if(theBuffer != NULL)
	{
		memcpy(theBuffer, theStringData.c_str(), theStringData.length());
		::GlobalUnlock(global);
		CComPtr<IStream> pstm = NULL;  //ATL 中定义的智能指针
		//从指定内存创建流对象。
		CreateStreamOnHGlobal(global, TRUE, &pstm);
		//GDI+
		Gdiplus::Bitmap* theImage = Gdiplus::Bitmap::FromStream(pstm); //方式二：从stream中生成位图对象
		//标注图片PNG出现问题了
		theResult = CreateTheTextFromGdiBitmap(theImage); //这个函数会为 m_pdata分配内存空间
		DeleteObject(theImage);

		glGenTextures(1, &m_textureID);//管理多个纹理对象
		glBindTexture(GL_TEXTURE_2D, m_textureID);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		//绑定纹理数据
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,  m_ImageWidth, m_ImageHeight , 0, GL_RGBA,GL_UNSIGNED_BYTE ,m_pdata);
	}
	GlobalFree(global);
	return theResult;
}

void CMarkerStyle::Init(CBitmap* thebmp, const int& height, const int& width, bool bRGBA)
{
	int pixsize = 0;
	if(bRGBA)
		pixsize = 4;
	else
		pixsize = 3;
	// Read the texture bits
	m_ImageWidth=width;
	m_ImageHeight=height;
	if (m_pdata)
	{
		delete[] m_pdata;
		m_pdata = NULL;
	}
	m_pdata = new BYTE[m_ImageWidth*m_ImageHeight*pixsize];
	DWORD size = thebmp->GetBitmapBits(m_ImageWidth*m_ImageHeight*pixsize,m_pdata);
	for (int i=0;i<m_ImageWidth*m_ImageHeight;i++)
	{
		BYTE tmp = (m_pdata[i*pixsize]);
		m_pdata[i*pixsize+0] = (m_pdata[i*pixsize+2]); //r
		m_pdata[i*pixsize+2] = tmp;//b
	}

	glGenTextures(1, &m_textureID);
	glBindTexture(GL_TEXTURE_2D, m_textureID);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	if(!bRGBA)
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB,  m_ImageWidth, m_ImageHeight , 0, GL_RGB,GL_UNSIGNED_BYTE ,m_pdata );
	else
		glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA,  m_ImageWidth, m_ImageHeight , 0, GL_RGBA,GL_UNSIGNED_BYTE ,m_pdata );
}

bool CMarkerStyle::CreateTheTextFromGdiBitmap(Gdiplus::Bitmap* bitmap)
{
	try
	{
		HBITMAP gdibitmap; //SDK 位图句柄
		bitmap->GetHBITMAP(Gdiplus::Color::White, &gdibitmap);					//GDIBITMAP
		if(m_pdata)
			delete[] m_pdata;
		m_ImageWidth = bitmap->GetWidth();
		m_ImageHeight = bitmap->GetHeight();
		DWORD bitmapSize = bitmap->GetHeight() * bitmap->GetWidth();
		if(bitmapSize == 0)
			return false;
		m_pdata = new GLubyte[bitmapSize * 4];									//Create Memery
		CBitmap* cbitmap = CBitmap::FromHandle(gdibitmap);						//Get the handle;
		cbitmap->GetBitmapBits(bitmapSize * 4, m_pdata);		//get the picture data
		//from bgr to rgb
		for (int i = 0; i < bitmapSize; i++)
		{
			int tempBit = m_pdata[i * 4];
			m_pdata[i * 4] = m_pdata[i * 4 + 2];
			m_pdata[i * 4 + 2] = tempBit;
		}
		cbitmap->DeleteObject();
		DeleteObject(gdibitmap);
		return true;
	}
	catch (CMemoryException* e)
	{

	}
	catch (CFileException* e)
	{
	}
	catch (CException* e)
	{
	}
	return false;
}