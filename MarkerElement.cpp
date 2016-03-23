#include "stdafx.h"
#include "MarkerStyle.h"
#include "Tile.h"
#include "MarkerElement.h"

extern GLuint  g_positionLoc;
extern GLuint  g_texCoordLoc;
extern GLuint  g_colorLoc;
extern GLuint  g_MVPLoc;

// CMarkerElement
CMarkerElement::CMarkerElement(DrawEngine *den):m_markerstyle(NULL),m_gen_vboID(true)  //modify by xuyan 将成员样式初始化为NULL
{
	//identity_matrix(m_AngleP);
	this->den = den;
	m_Visiable = true;
	m_Geometry.m_type = vUPoint;

	m_rendW = 64;
	m_rendH = 64;
	m_Angle = 180.0;		//从0度 转为 180度，这样是正常的。 yanjianlong 2013.11.18
//注释by xuyan ，没有使用到索引数组
	//memset(&m_Indices,NULL,sizeof(m_Indices));
	//m_Indices[0] = 0;
	//m_Indices[1] = 2 ;
	//m_Indices[2] = 1;

	//m_Indices[3] = 3;
	//m_Indices[4] = 2 ;
	//m_Indices[5] = 0;

	m_ibID = 0;
	m_vboID = 0;

	//glGenBuffers(1, &m_ibID);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibID);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(char), (void*)&m_Indices[0],GL_STATIC_DRAW);
	//
	//glGenBuffers(1, &m_vboID);
}


CMarkerElement::~CMarkerElement(void)
{
	//IBO对象未使用
	if (m_ibID!=0)
	{
		glDeleteBuffers(1,&m_ibID);
		m_ibID = 0;
	}

	if (m_vboID != 0)
	{
		glDeleteBuffers(1,&m_vboID);
	    //den->Log(PP_LOGLEVEL_LOG,"Delete VBO ID : the ID is %d\n",m_vboID);
		m_vboID = 0;
	}
}



void CMarkerElement::SetVertex(Vertex *verts)
{
	memcpy(&m_verts[0],&verts[0],sizeof(m_verts));
	
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
	glBufferData(GL_ARRAY_BUFFER, 4* sizeof(Vertex), (void*)&m_verts,
		GL_STATIC_DRAW);
}

void CMarkerElement::SetVertex(double *verts1)
{
	Vertex verts[4] = {};
	verts[0].tu = 0;
	verts[0].tv = 0;
	verts[0].loc[0] = verts1[0];
	verts[0].loc[1] = verts1[1];
	verts[0].loc[2] = 1;
	verts[0].color[0] = 255;
	verts[0].color[1] = 0;
	verts[0].color[2] = 0;

	verts[1].tu = 1;
	verts[1].tv = 0;
	verts[1].loc[0] = verts1[2];
	verts[1].loc[1] = verts1[3];
	verts[1].loc[2] = 1;
	verts[1].color[0] = 255;
	verts[1].color[1] = 0;
	verts[1].color[2] = 0;


	verts[2].tu = 1;
	verts[2].tv = 1;
	verts[2].loc[0] = verts1[4];
	verts[2].loc[1] = verts1[5];
	verts[2].loc[2] = 1;
	verts[2].color[0] = 255;
	verts[2].color[1] = 0;
	verts[2].color[2] = 0;

	verts[3].tu = 0;
	verts[3].tv = 1;
	verts[3].loc[0] = verts1[6];
	verts[3].loc[1] = verts1[7];
	verts[3].loc[2] = 1;
	verts[3].color[0] = 255;
	verts[3].color[1] = 0;
	verts[3].color[2] = 0;

	SetVertex(verts);
}

void CMarkerElement::SetVertex(double x,double y,double z)
{
	Vertex verts[4] = {};
	verts[0].tu = 0;
	verts[0].tv = 0;
	verts[0].loc[0] = x;
	verts[0].loc[1] = y+m_rendH;
	verts[0].loc[2] = z;
	verts[0].color[0] = 255;
	verts[0].color[1] = 0;
	verts[0].color[2] = 0;

	verts[1].tu = 1;
	verts[1].tv = 0;
	verts[1].loc[0] = x+m_rendW;
	verts[1].loc[1] = y+m_rendH;
	verts[1].loc[2] = z;
	verts[1].color[0] = 255;
	verts[1].color[1] = 0;
	verts[1].color[2] = 0;


	verts[2].tu = 1;
	verts[2].tv = 1;
	verts[2].loc[0] = x+m_rendW;
	verts[2].loc[1] = y;
	verts[2].loc[2] = z;
	verts[2].color[0] = 255;
	verts[2].color[1] = 0;
	verts[2].color[2] = 0;

	verts[3].tu = 0;
	verts[3].tv = 1;
	verts[3].loc[0] = x;
	verts[3].loc[1] = y;
	verts[3].loc[2] = z;
	verts[3].color[0] = 255;
	verts[3].color[1] = 0;
	verts[3].color[2] = 0;

	SetVertex(verts);
}

void CMarkerElement::SetVertex()
{
	LONG pix =0;
	LONG piy = 0;

	//坐标转换
	den->Projection_to_Pixel(m_X,m_Y,pix,piy);
	float pix0;
	float piy0;
	float pix1;
	float piy1;
	float pix2;
	float piy2;
	float pix3;
	float piy3;

	//以图片左上角为起点
	/*pix0 = pix;
	piy0 = piy+m_rendH;
	pix1 = pix+m_rendW;
	piy1 = piy+m_rendH;
	pix2 = pix+m_rendW;
	piy2 = piy;
	pix3 = pix;
	piy3 = piy;*/

	//以图片中心点对应
	pix0 = pix - m_rendW/2;
	piy0 = piy - m_rendH/2;
	pix1 = pix+m_rendW/2;
	piy1 = piy-m_rendH/2;
	pix2 = pix+m_rendW/2;
	piy2 = piy+m_rendH/2;
	pix3 = pix-m_rendW/2;
	piy3 = piy+m_rendH/2;

	//计算旋转
	
	//float vx = cenx + m_R * cos(i);
	//float vy = ceny + m_R * sin(i);
	double m_R = sqrt(m_rendH*m_rendH*1.0+m_rendW*m_rendW)/2.0;
	if (m_Angle != 0)
	{	
		float tmp = atan(m_rendH/m_rendW*1.0)*180/M_PI;
		float angletmp3 = tmp + m_Angle;
		pix3 = pix - m_R * cos(angletmp3*M_PI/180);
		piy3 = piy - m_R * sin(angletmp3*M_PI/180);
	  
		//double a = cos(45.0*M_PI/180);
		//a= sin(45.0*M_PI/180);
		float angletmp2 =  tmp - m_Angle;
		pix2 = pix + m_R * cos(angletmp2*M_PI/180);
		piy2 = piy - m_R * sin(angletmp2*M_PI/180);

		float angletmp1 =  tmp + m_Angle;
		pix1 = pix + m_R * cos(angletmp1*M_PI/180);
		piy1 = piy + m_R * sin(angletmp1*M_PI/180);

		float angletmp0 = tmp - m_Angle;
		pix0 = pix - m_R * cos(angletmp0*M_PI/180);
		piy0 = piy + m_R * sin(angletmp0*M_PI/180);
	}
	
	//double a = atan(m_rendH/m_rendW)*180/M_PI;

	Vertex verts[4] = {};
	verts[0].tu = 0;
	verts[0].tv = 0;
	verts[0].loc[0] = pix0;
	verts[0].loc[1] = piy0;
	verts[0].loc[2] = m_Z;
	verts[0].color[0] = 255;
	verts[0].color[1] = 0;
	verts[0].color[2] = 0;

	verts[1].tu = 1;
	verts[1].tv = 0;
	verts[1].loc[0] = pix1;
	verts[1].loc[1] = piy1;
	verts[1].loc[2] = m_Z;
	verts[1].color[0] = 255;
	verts[1].color[1] = 0;
	verts[1].color[2] = 0;

	verts[2].tu = 1;
	verts[2].tv = 1;
	verts[2].loc[0] = pix2;
	verts[2].loc[1] = piy2;
	verts[2].loc[2] = m_Z;
	verts[2].color[0] = 255;
	verts[2].color[1] = 0;
	verts[2].color[2] = 0;

	verts[3].tu = 0;
	verts[3].tv = 1;
	verts[3].loc[0] = pix3;
	verts[3].loc[1] = piy3;
	verts[3].loc[2] = m_Z;
	verts[3].color[0] = 255;
	verts[3].color[1] = 0;
	verts[3].color[2] = 0;

	SetVertex(verts);
}

void CMarkerElement::RenderUseOgles()
{
	if(!m_Visiable)
		return;
		//add by xuyan
	glUseProgram( g_programObj );
	g_MVPLoc = glGetUniformLocation(g_programObj, "a_MVP");
	glUniformMatrix4fv(g_MVPLoc, 1, GL_FALSE, (GLfloat*) den->g_mpv);

	glBindTexture(GL_TEXTURE_2D,m_markerstyle->m_textureID);
	
	if(m_gen_vboID)
	{
		glGenBuffers(1, &m_vboID);
		m_gen_vboID = false;
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_vboID);  //render 热点这个功能没有使用到

	glVertexAttribPointer(g_positionLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,loc));
	glEnableVertexAttribArray(g_positionLoc); //几何位置 坐标值

	glVertexAttribPointer(g_texCoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,tu));
	glEnableVertexAttribArray(g_texCoordLoc);//纹理

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibID);
	glDrawElements (GL_TRIANGLE_FAN, 6, GL_UNSIGNED_BYTE ,0 );//GL_TRIANGLE_FAN

}


//依据 纹理来 绘制点
void CMarkerElement::RenderManyUseOgles(std::vector<Vertex>& theVertexVector)
{
	////绘制
	if(theVertexVector.empty())
		return;

	glUseProgram( g_programObj );
	g_MVPLoc = glGetUniformLocation(g_programObj, "a_MVP"); //获取着色器属性
	glUniformMatrix4fv(g_MVPLoc, 1, GL_FALSE, (GLfloat*) den->g_mpv);

	glBindTexture(GL_TEXTURE_2D,m_markerstyle->m_textureID);  //绑定纹理
	if(m_gen_vboID)
	{
		glGenBuffers(1, &m_vboID);
		m_gen_vboID = false;
	}
	glBindBuffer(GL_ARRAY_BUFFER, m_vboID);
	glBufferData(GL_ARRAY_BUFFER, theVertexVector.size() * sizeof(Vertex), (GLvoid*)&theVertexVector[0], GL_STATIC_DRAW);
	
	glVertexAttribPointer(g_positionLoc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,loc));
	glEnableVertexAttribArray(g_positionLoc); //启用这个功能

	glVertexAttribPointer(g_texCoordLoc, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,tu));
	glEnableVertexAttribArray(g_texCoordLoc);

	glDrawArrays(GL_TRIANGLES,0,theVertexVector.size());  //画三角形
}