#include "GifElement.h"
#include "libnsgif.h"
#include <afxinet.h>

extern GLuint  g_positionLoc;
extern GLuint  g_texCoordLoc;
extern GLuint  g_colorLoc;
extern GLuint  g_MVPLoc;


CString ConnethttpServer(CString serverurl)
{
	int nret = 0;
	//图层路径
	CString szExePath = L"";
	::GetModuleFileName(NULL,szExePath.GetBufferSetLength(MAX_PATH+1),MAX_PATH);
	szExePath = szExePath.Left(szExePath.ReverseFind('\\'));
	szExePath = szExePath.Left(szExePath.ReverseFind('\\'));

	CString gifname = serverurl.Right(serverurl.GetLength() - serverurl.ReverseFind('/') -1);;

	CString  gifpath = szExePath + L"\\ICON\\GIF\\";
	CString  strSavePath = gifpath + gifname;

	CFile file;
	UINT nCount = -1;
	char aChrs[102400];   //100KB

	CInternetSession sess(L"GIF");
	//CFtpConnection* fileGet;
	CHttpFile* pHttpFile = NULL;//CHttpConnection
	try
	{
		CString strFileName = serverurl;
		pHttpFile=(CHttpFile*)sess.OpenURL(strFileName);
		//fileGet = sess.GetFtpConnection(L"10.1.50.50",NULL,NULL,8080);
	}
	catch(CException* e)
	{
		//MessageBox(NULL,L"gife",L"",0);
		e->Delete();
		sess.Close();
		nret = 1;
		return NULL;
	}    

	if(pHttpFile)
	{

		//bool b = fileGet->GetFile(L"dw_anjian.gif",L"c://1.gif");//下载文件到C盘 ftp
		CString szContent = _T("");
		DWORD   dwFlags   = HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER;
		DWORD   dwStatus;
		DWORD   dwBuffLen = sizeof(dwStatus);
		BOOL    bResult   = FALSE;

		bResult = pHttpFile->QueryInfo(dwFlags,&dwStatus,&dwBuffLen);
		if (bResult && dwStatus >= 200 && dwStatus < 300)
		{
			if (! file.Open(strSavePath,CFile::modeCreate|CFile::modeWrite|CFile::typeBinary))
			{
				return FALSE;
			}

			while ((nCount = pHttpFile->Read(aChrs,1023)) != 0)
			{
				file.Write(aChrs,nCount);
			}
		}

		pHttpFile->Close();
		delete pHttpFile;
		pHttpFile = NULL;

		sess.Close();

		return strSavePath;
	}

	sess.Close();
	return strSavePath;
}

CGifElement::CGifElement(CString path,double x,double y,int w,int h,DrawEngine *den):
 m_gifvboID(0),
 m_gifibID(0)
{
	this->den = den;
	//m_existfile = false;
	m_delay = -1;//负值表示无效.
	m_gifpath = path;
	m_x = x;
	m_y = y;
	m_rendW = w;
	m_rendH = h;

	USES_CONVERSION; 
	string spath = W2A(m_gifpath.GetBuffer(0));

	//判断gif是本地文件还是需要从服务获取
	if (!PathFileExists(m_gifpath))
	{
		CString gifname = m_gifpath.Right(m_gifpath.GetLength() - m_gifpath.ReverseFind('/') -1);  //bus.gif
		if (m_gifpath.Find(L"http:") == 0)
		{
			CString theFilePath = ConnethttpServer(m_gifpath); //从网络上下载图片,并返回保存在本地后的 [磁盘路径]
			spath = W2A(theFilePath);
		}
	}	

	FILE *fp = fopen(spath.c_str(), "rb");//"d:\\gif\\1.gif"
	if( fp == NULL)
		return;
	fseek(fp,0,SEEK_END);
	int len = ftell(fp);
	rewind(fp);
	char *pdata = new char[len];
	memset(pdata,0,len);
	fread(pdata, 1, len, fp);
	fclose(fp);

	//m_existfile = true;//说明已经从网络获取到数据,或者是本地已经有数据.

	double delay = 0.0;
	GIFDecoder::DecodeGIF(pdata,len,&m_width,&m_height,&delay,m_giflistdata); //将GIF的每一帧解码放到list中
	delete [] pdata;

	m_delay = (int)(delay*1000);
	m_frameCount = m_giflistdata.size();

	m_gifTexture = new GLuint[m_frameCount];

	glGenTextures(m_frameCount,m_gifTexture);
	glGenBuffers(1, &m_gifvboID);
	glGenBuffers(1, &m_gifibID);

	m_Indices[0] = 0;
	m_Indices[1] = 2 ;
	m_Indices[2] = 1;

	m_Indices[3] = 0;
	m_Indices[4] = 2 ;
	m_Indices[5] = 3;

	m_tmp = 0.0;

	m_Currentid = 0;
}


CGifElement::~CGifElement(void)
{
	if (m_gifibID != 0)
	{
		glDeleteBuffers(1,&m_gifibID);
		m_gifibID = 0;
	}

	if (m_gifvboID != 0)
	{
		glDeleteBuffers(1,&m_gifvboID);
		m_gifvboID = 0;
	}

	for (int i=0;i<m_frameCount;i++)
	{
	  glDeleteTextures(1,&m_gifTexture[i]);	
	  m_frameCount = 0;
	}

	//delete m_gifTexture;
	m_gifTexture = NULL;

}

void CGifElement::RenderUseOgles(double delaytime)
{
	if (m_delay == -1 || m_gifTexture == NULL)
	{
		return;
	}

	glUseProgram(g_programObj);
	g_MVPLoc = glGetUniformLocation(g_programObj, "a_MVP");
	glUniformMatrix4fv(g_MVPLoc, 1, GL_FALSE, (GLfloat*) den->g_mpv);
	//gif层
	if (m_giflistdata.size() != NULL)
	{  
		if (m_Currentid == m_frameCount-1)
		{
			m_Currentid = 0;
			m_tmp = 0.0;
		}

		if (delaytime!=0)
		{
			m_tmp += delaytime;
			m_Currentid = m_tmp/m_delay - 1;
		}
		
		int id = 0;
		for (list<char*>::iterator iter = m_giflistdata.begin()++ ;iter != m_giflistdata.end();iter++)
		{
			if(id != m_Currentid)
			{
				id++;
				continue;
			}

			unsigned char* ptdata = (unsigned char*)(*iter);

			glBindTexture(GL_TEXTURE_2D,m_gifTexture[m_Currentid]);

			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glTexImage2D(GL_TEXTURE_2D, 0,GL_BGRA_EXT , m_width, m_height, 0,GL_BGRA_EXT , GL_UNSIGNED_BYTE,ptdata);//GL_BGRA_EXT

			long pix = 0;
			long piy = 0;
			den->Projection_to_Pixel(m_x,m_y,pix,piy);

			m_gifverts[0].tu = 0;
			m_gifverts[0].tv = 0;
			m_gifverts[0].loc[0] = pix-m_rendW/2;
			m_gifverts[0].loc[1] = piy+m_rendH/2;
			m_gifverts[0].loc[2] = 1;
			m_gifverts[0].color[0] = 255;
			m_gifverts[0].color[1] = 0;
			m_gifverts[0].color[2] = 0;

			m_gifverts[1].tu = 1;
			m_gifverts[1].tv = 0;
			m_gifverts[1].loc[0] = pix+m_rendW/2;
			m_gifverts[1].loc[1] = piy+m_rendH/2;
			m_gifverts[1].loc[2] = 1;
			m_gifverts[1].color[0] = 255;
			m_gifverts[1].color[1] = 0;
			m_gifverts[1].color[2] = 0;

			m_gifverts[2].tu = 1;
			m_gifverts[2].tv = 1;
			m_gifverts[2].loc[0] = pix+m_rendW/2;
			m_gifverts[2].loc[1] = piy-m_rendH/2;
			m_gifverts[2].loc[2] = 1;
			m_gifverts[2].color[0] = 255;
			m_gifverts[2].color[1] = 0;
			m_gifverts[2].color[2] = 0;

			m_gifverts[3].tu = 0;
			m_gifverts[3].tv = 1;
			m_gifverts[3].loc[0] = pix-m_rendW/2;
			m_gifverts[3].loc[1] = piy-m_rendH/2;
			m_gifverts[3].loc[2] = 1;
			m_gifverts[3].color[0] = 255;
			m_gifverts[3].color[1] = 0;
			m_gifverts[3].color[2] = 0;


			glBindBuffer(GL_ARRAY_BUFFER, m_gifvboID);
			glBufferData(GL_ARRAY_BUFFER, 4* sizeof(Vertex), (void*)&m_gifverts,GL_STATIC_DRAW); //数据 由内存传送至显卡


			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_gifibID);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(char), (void*)&m_Indices[0],GL_STATIC_DRAW);

			glVertexAttribPointer(g_positionLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,loc));
			glEnableVertexAttribArray(g_positionLoc);

			glVertexAttribPointer(g_texCoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,tu));
			glEnableVertexAttribArray(g_texCoordLoc);

		/*	glVertexAttribPointer(g_colorLoc, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,color));
			glEnableVertexAttribArray(g_colorLoc);*/

			glDrawElements (GL_TRIANGLE_FAN, 6, GL_UNSIGNED_BYTE ,0 );//GL_TRIANGLE_FAN
            break;
		}

	}

}