#include "Tile.h"

extern int g_CtrlW;
extern int g_CtrlH;

extern byte *gdatar;

extern GLuint  g_positionLoc;
extern GLuint  g_texCoordLoc;
extern GLuint  g_colorLoc;
extern GLuint  g_MVPLoc;


//本组件 现在使用的构造函数， 
//参数的意义：1.图片路径  2.几何图元的顶点数据 3.w,h表示图片的像素宽高256
Tile::Tile(CString sTileName,Vertex *verts,int w,int h,CString pictype,int js,int rowid,int colid)
{
	m_pdata = NULL;
	m_sourcedata = NULL;
	m_len = 0;

	m_pictype = pictype;
	memset(&m_Indices,NULL,sizeof(m_Indices));
	
	m_TileW = w;
	m_TileH = h;
	m_X = 0.0;
	m_Y = 0.0;
	m_Z = 0.0;
	m_js = js;
	m_rowid = rowid;
	m_colid = colid;

	m_vboID = 0;
	m_ibID = 0;
	m_textureID = 0;

	v_n = 4;
	i_n = 6;

	memcpy(&m_verts[0],&verts[0],sizeof(m_verts));

	m_Indices[0] = 0;
	m_Indices[1] = 2 ;
	m_Indices[2] = 1;
	m_Indices[3] = 3;
	m_Indices[4] = 2 ;
	m_Indices[5] = 0;

	//地图tile数据
	m_tileName = sTileName;
}

//包含了解码和邦定纹理,该方式适合于显示普通图片.如果用多线程解码的话就使用另一个构造函数,等解码完成以后就ini.
Tile::Tile(CString sTileName,long x,long y,int w,int h)
{
	m_sourcedata = NULL;
	m_len = 0;

	if (sTileName.Compare(L"") == 0)
	{
		return;
	}

	m_tileName = sTileName;
	m_pdata = NULL;

	memset(&m_Indices,NULL,sizeof(m_Indices));

	v_n = 4;
	i_n = 6;

	//memcpy(&m_verts[0],&verts[0],sizeof(m_verts));

	// Read the texture bits
	HBITMAP hBitMap; //GDI对象 位图句柄
	//GDI+ 对象
	Bitmap* bmp = Bitmap::FromFile(m_tileName);//L"c:\\0_0.jpg" D:\map2d\Chrome\Application\VideoTip
	m_TileW=bmp->GetWidth();
	m_TileH=bmp->GetHeight();

	int m_rendW = m_TileW;
	int m_rendH = m_TileH;

	if (m_pdata)
	{
		delete[] m_pdata;
		m_pdata = NULL;
	}
	m_pdata = new BYTE[m_TileW*m_TileH*4];

	bmp->GetHBITMAP(Color::White,&hBitMap);
	CBitmap* cbmp = CBitmap::FromHandle(hBitMap);
	DWORD size = cbmp->GetBitmapBits(m_TileW*m_TileH*4,m_pdata);
	for (int i=0;i<m_TileW*m_TileH;i++)
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

	memset(&m_Indices,NULL,sizeof(m_Indices));
	m_vboID = 0;
	m_ibID = 0;
	m_textureID = 0;

	m_Indices[0] = 0;
	m_Indices[1] = 2 ;
	m_Indices[2] = 1;

	m_Indices[3] = 3;
	m_Indices[4] = 2 ;
	m_Indices[5] = 0;

	m_positionLoc = g_positionLoc;
	m_texCoordLoc = g_texCoordLoc;
	m_colorLoc = g_colorLoc;
	m_MVPLoc = g_MVPLoc;

	double cenx = x;
	double ceny = y;

	//坐标转换
	long pix = x;
	long piy = y;
	//Projection_to_Pixel(cenx,ceny,pix,piy);
	float pix0;
	float piy0;
	float pix1;
	float piy1;
	float pix2;
	float piy2;
	float pix3;
	float piy3;
	//以图片中心点对应
	/*pix0 = pix - m_rendW/2;
	piy0 = piy + m_rendH/2;
	pix1 = pix+m_rendW/2;
	piy1 = piy+m_rendH/2;
	pix2 = pix+m_rendW/2;
	piy2 = piy-m_rendH/2;
	pix3 = pix-m_rendW/2;
	piy3 = piy-m_rendH/2;*/
	pix0 = pix - m_rendW/2;
	piy0 = piy + m_rendH/2;
	pix1 = pix+m_rendW/2;
	piy1 = piy+m_rendH/2;
	pix2 = pix+m_rendW/2;
	piy2 = piy-m_rendH/2;
	pix3 = pix-m_rendW/2;
	piy3 = piy-m_rendH/2;

	Vertex verts[4] = {};
	verts[0].tu = 0;
	verts[0].tv = 1;
	verts[0].loc[0] = pix0;
	verts[0].loc[1] = piy0;
	verts[0].loc[2] = 1;
	verts[0].color[0] = 255;
	verts[0].color[1] = 0;
	verts[0].color[2] = 0;

	verts[1].tu = 1;
	verts[1].tv = 1;
	verts[1].loc[0] = pix1;
	verts[1].loc[1] = piy1;
	verts[1].loc[2] = 1;
	verts[1].color[0] = 255;
	verts[1].color[1] = 0;
	verts[1].color[2] = 0;

	verts[2].tu = 1;
	verts[2].tv = 0;
	verts[2].loc[0] = pix2;
	verts[2].loc[1] = piy2;
	verts[2].loc[2] = 1;
	verts[2].color[0] = 255;
	verts[2].color[1] = 0;
	verts[2].color[2] = 0;

	verts[3].tu = 0;
	verts[3].tv = 0;
	verts[3].loc[0] = pix3;
	verts[3].loc[1] = piy3;
	verts[3].loc[2] = 1;
	verts[3].color[0] = 255;
	verts[3].color[1] = 0;
	verts[3].color[2] = 0;

	glGenTextures(1, &m_textureID);
	glBindTexture(GL_TEXTURE_2D, m_textureID);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA,  m_TileW, m_TileH , 0,GL_RGBA,GL_UNSIGNED_BYTE ,m_pdata );

	glGenBuffers(1, &m_vboID);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
	glBufferData(GL_ARRAY_BUFFER, v_n* sizeof(Vertex), (void*)&verts,
		GL_STATIC_DRAW);

	glGenBuffers(1, &m_ibID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, i_n * sizeof(char), (void*)&m_Indices[0],GL_STATIC_DRAW);
}

//紧凑型缓存文件中读取切片时使用
Tile::Tile(CString sTileName,const char* picdata,Vertex *verts,int w,int h,CString pictype,int js,int rowid,int colid)
{
	m_pdata = NULL;
	m_sourcedata = NULL;
	m_len = 0;

	m_pictype = pictype;
	memset(&m_Indices,NULL,sizeof(m_Indices));

	m_TileW = w;
	m_TileH = h;
	m_X = 0.0;
	m_Y = 0.0;
	m_Z = 0.0;
	m_js = js;
	m_rowid = rowid;
	m_colid = colid;

	m_vboID = 0;
	m_ibID = 0;
	m_textureID = 0;

	v_n = 4;
	i_n = 6;

	memcpy(&m_verts[0],&verts[0],sizeof(m_verts));

	m_Indices[0] = 0;
	m_Indices[1] = 2 ;
	m_Indices[2] = 1;
	m_Indices[3] = 3;
	m_Indices[4] = 2 ;
	m_Indices[5] = 0;

	//地图切片数据
	m_tileName = sTileName;
}

void Tile::init(int g_context)
{
	this->g_context = g_context;
	m_positionLoc = g_positionLoc;
	m_texCoordLoc = g_texCoordLoc;
	m_colorLoc = g_colorLoc;
	m_MVPLoc = g_MVPLoc;
	glGenTextures(1, &m_textureID); //生成一个 纹理对象(能够实现多重纹理)
	glBindTexture(GL_TEXTURE_2D, m_textureID); //为[纹理对象] 绑定一个 [纹理状态]
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexImage2D(GL_TEXTURE_2D,0,m_sformat,m_TileW, m_TileH , 0, m_sformat,GL_UNSIGNED_BYTE ,m_pdata); //从内存区域 载入 纹理数据
	if (m_pdata==NULL || m_textureID==0)
	{
		
	}
	
	glGenBuffers(1, &m_vboID);
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
	glBufferData(GL_ARRAY_BUFFER, v_n* sizeof(Vertex), (void*)&m_verts,GL_STREAM_DRAW);

	glGenBuffers(1, &m_ibID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibID);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, i_n * sizeof(char), (void*)&m_Indices[0],GL_STREAM_DRAW);

	//char tx_log[100] = {};
	//sprintf(tx_log,"DrawEngine's context is [%d] : tile's glGenTextures's is [%d] : glGenBuffers is [%d]\r\n",this->g_context,m_textureID,m_vboID);
	//fwrite(tx_log,strlen(tx_log),1,tex_log);
	//fflush(tex_log);
}

Tile::~Tile(void)
{
	if (m_pdata!=NULL)
	{
		delete[] m_pdata; //解码后的图片数据
		m_pdata = nullptr;
	}

	glDeleteTextures(1,&m_textureID);
	glDeleteBuffers(1,&m_ibID);
	glDeleteBuffers(1,&m_vboID);

	//char tx_log[100] = {};
	//sprintf(tx_log,"DELETE context is [%d] : glDeleteTextures's is [%d] : glDeleteBuffers is [%d]\r\n",this->g_context,m_textureID,m_vboID);
	//fwrite(tx_log,strlen(tx_log),1,tex_log);
	//fflush(tex_log);
}

void Tile::RenderUseOgles()
{
	
	glBindTexture(GL_TEXTURE_2D,m_textureID); //传数据到显卡
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibID); //索引数组

	glVertexAttribPointer(m_positionLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,loc));
	glEnableVertexAttribArray(m_positionLoc);

	glVertexAttribPointer(m_texCoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,tu));
	glEnableVertexAttribArray(m_texCoordLoc);

	/*glVertexAttribPointer(m_colorLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,color));
	glEnableVertexAttribArray(m_colorLoc);*/
//当前绑定的 VAO是 索引缓存
	glDrawElements (/*GL_TRIANGLE_FAN*/GL_TRIANGLES, i_n, GL_UNSIGNED_BYTE ,0);//GL_TRIANGLE_FAN  三角形扇
	//glDrawArrays(GL_POINTS,0,v_n);
	
	//恢复默认的纹理绑定设置，并停止VAO
	glBindTexture(GL_TEXTURE_2D,0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}