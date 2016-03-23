#include "ElementLayer.h"
#include "DataStruct.h"
#include "LineElement.h"


CElementLayer::CElementLayer(DrawEngine *den)
{
	m_layerType = vElementLayer;
	//m_ElementList;  // 对vector 容器 默认初始化为空

	m_vboID = 0;
	glGenBuffers(1,&m_vboID);
	this->den = den;
}


CElementLayer::~CElementLayer(void)
{
	DeleteAllElement();

	glDeleteBuffers(1,&m_vboID);
}


int CElementLayer::AddElement(CElement* elementObject)
{
	m_ElementList.push_back(elementObject);
	return m_ElementList.size()-1;
}

CElement* CElementLayer::GetElement(int index)
{
	if (m_ElementList.size() != 0)
	{
		return m_ElementList[index];
	}
	return NULL;
}

void CElementLayer::Draw()
{
	if (m_ElementList.size() == 0)
	{
		return;
	}

	if (m_layerType == VLayerType::vLuKuangLayer)
	{
		//路况层：采用独立的渲染函数
		DrawAllLineElement();
	}
	else
	{
		vector<CElement *>::iterator it = m_ElementList.begin();
		for (; it!=m_ElementList.end();it++)
		{
			CElement* obj = (CElement*)(*it);
			if (obj)
			{
				obj->RenderUseOgles();
			}
		}
	}
}

void CElementLayer::DrawAllLineElement()
{
	//每个要素都是线element,否则出错
	int LineCount = m_ElementList.size();
	vector<CElement *>::iterator it = m_ElementList.begin();
	int triCount = 0;//需要绘制的三角形个数
	for (int i=0; i<LineCount;i++)
	{
		CLineElement* lineobj = static_cast<CLineElement*>(m_ElementList[i]);
		if (lineobj)
		{
			int PointCount = lineobj->m_vctPoint.size();
			triCount += (PointCount-1)*2;
		}
	}

	//后面代码的目的就是把所有的线都用三角形保存起来，为了做批量绘制
	int vertexCount = triCount*3;
	Vertex* verts = new Vertex[vertexCount];
	int add = 0;
	for (int i=0; i<LineCount;i++)//层的线个数
	{
		CLineElement* lineobj = static_cast<CLineElement*>(m_ElementList[i]);

		//求线宽对应的像素值
		int pWidth = lineobj->m_w;
		double d;
		if (lineobj->m_unit.Compare(L"metre") == 0)
		{
			double X,Y,pletflon,plon,pwidth,pheight,pToplat,plat,scale,resolution;
			CString name;
			den->GetCurrentShowInfo(X,Y ,pwidth,pheight,pletflon,pToplat,plon,plat,scale,name,resolution);

			d = lineobj->m_w / (2 * 3.1415926 * 6378137.0) * 360;
			pWidth = d/resolution;
		}
		int r = lineobj->m_r;
		int g = lineobj->m_g;
		int b = lineobj->m_b;
		int a = lineobj->m_a;
		
		int PointCount = lineobj->m_vctPoint.size();
		//后面代码的目的是把一根线用三角形表示
		for (int j = 0; j<PointCount-1; j++)
		{
			double xs = lineobj->m_vctPoint[j].x;
			double ys = lineobj->m_vctPoint[j].y;
			long pix1 =0;
			long piy1 = 0;
			den->Projection_to_Pixel(xs ,ys,pix1,piy1);

			double xe = lineobj->m_vctPoint[j+1].x;
			double ye = lineobj->m_vctPoint[j+1].y;
			long pix2 =0;
			long piy2 = 0;
			den->Projection_to_Pixel(xe ,ye,pix2,piy2);

			double x1,y1,x2,y2,x3,y3,x4,y4;

			if (pix1 == pix2)
			{
				x1 = pix1-pWidth/2;
				x2 = pix1+pWidth/2;
				x3 = pix2-pWidth/2;
				x4 = pix2+pWidth/2;
				y1 = piy1;
				y2 = piy1;
				y3 = piy2;
				y4 = piy2;
			}
			else if (piy1 == piy2)
			{
				x1 = pix1;
				x2 = pix1;
				x3 = pix2;
				x4 = pix2;
				y1 = piy1+pWidth/2;
				y2 = piy1-pWidth/2;
				y3 = piy2+pWidth/2;
				y4 = piy2-pWidth/2;
			}
			else
			{
				double d=(piy2-piy1)*1.0/(pix1-pix2);
				y1=pWidth/2.0/sqrt(d*d+1.0)+piy1;
				x1=d*(y1-piy1) +pix1;
				y2=-pWidth/2.0/sqrt(d*d+1.0)+piy1;
				x2=d*(y2-piy1)+pix1;
				y3=pWidth/2.0/sqrt(d*d+1.0)+piy2;
				x3=d*(y3-piy2)+pix2;
				y4=-pWidth/2.0/sqrt(d*d+1.0)+piy2;
				x4=d*(y4-piy2)+pix2;
			}

			//顶点数组
			verts[add+j*6+0].tu = 0;
			verts[add+j*6+0].tv = 0;
			verts[add+j*6+0].loc[0] = x1;
			verts[add+j*6+0].loc[1] = y1;
			verts[add+j*6+0].loc[2] = 1;
			verts[add+j*6+0].color[0] = r/255.0;
			verts[add+j*6+0].color[1] = g/255.0;
			verts[add+j*6+0].color[2] = b/255.0;
			verts[add+j*6+0].color[3] = a/255.0;

			verts[add+j*6+1].tu = 0;
			verts[add+j*6+1].tv = 0;
			verts[add+j*6+1].loc[0] = x2;
			verts[add+j*6+1].loc[1] = y2;
			verts[add+j*6+1].loc[2] = 1;
			verts[add+j*6+1].color[0] = r/255.0;
			verts[add+j*6+1].color[1] = g/255.0;
			verts[add+j*6+1].color[2] = b/255.0;
			verts[add+j*6+1].color[3] = a/255.0;

			verts[add+j*6+2].tu = 0;
			verts[add+j*6+2].tv = 0;
			verts[add+j*6+2].loc[0] = x3;
			verts[add+j*6+2].loc[1] = y3;
			verts[add+j*6+2].loc[2] = 1;
			verts[add+j*6+2].color[0] = r/255.0;
			verts[add+j*6+2].color[1] = g/255.0;
			verts[add+j*6+2].color[2] = b/255.0;
			verts[add+j*6+2].color[3] = a/255.0;

			verts[add+j*6+3].tu = 0;
			verts[add+j*6+3].tv = 0;
			verts[add+j*6+3].loc[0] = x3;
			verts[add+j*6+3].loc[1] = y3;
			verts[add+j*6+3].loc[2] = 1;
			verts[add+j*6+3].color[0] = r/255.0;
			verts[add+j*6+3].color[1] = g/255.0;
			verts[add+j*6+3].color[2] = b/255.0;
			verts[add+j*6+3].color[3] = a/255.0;

			verts[add+j*6+4].tu = 0;
			verts[add+j*6+4].tv = 0;
			verts[add+j*6+4].loc[0] = x2;
			verts[add+j*6+4].loc[1] = y2;
			verts[add+j*6+4].loc[2] = 1;
			verts[add+j*6+4].color[0] = r/255.0;
			verts[add+j*6+4].color[1] = g/255.0;
			verts[add+j*6+4].color[2] = b/255.0;
			verts[add+j*6+4].color[3] = a/255.0;

			verts[add+j*6+5].tu = 0;
			verts[add+j*6+5].tv = 0;
			verts[add+j*6+5].loc[0] = x4;
			verts[add+j*6+5].loc[1] = y4;
			verts[add+j*6+5].loc[2] = 1;
			verts[add+j*6+5].color[0] = r/255.0;
			verts[add+j*6+5].color[1] = g/255.0;
			verts[add+j*6+5].color[2] = b/255.0;
			verts[add+j*6+5].color[3] = a/255.0;
		}

		add += (PointCount-1)*6;
	}

	//批量显示所有的三角形
	glUseProgram(g_programObj_Shp);
	g_MVPLoc_Shp = glGetUniformLocation(g_programObj_Shp, "a_MVP");
	glUniformMatrix4fv(g_MVPLoc_Shp, 1, GL_FALSE, (GLfloat*) den->g_mpv);

	glBindBuffer(GL_ARRAY_BUFFER,m_vboID);
	glBufferData(GL_ARRAY_BUFFER, vertexCount * sizeof(Vertex), verts,
		GL_STATIC_DRAW);

	glVertexAttribPointer(g_positionLoc_Shp, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,loc));
	glEnableVertexAttribArray(g_positionLoc_Shp);

	glVertexAttribPointer(g_colorLoc_Shp, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,color));
	glEnableVertexAttribArray(g_colorLoc_Shp);

	/*glVertexAttribPointer(g_texCoordLoc_Shp, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,tu));
	glEnableVertexAttribArray(g_texCoordLoc_Shp);*/

	glDrawArrays(GL_TRIANGLES,0,vertexCount);

	delete[] verts;
	verts = NULL;
}

int CElementLayer::GetCount()
{
	return m_ElementList.size();
}

void CElementLayer::DeleteAllElement()
{
	vector<CElement *>::iterator it = m_ElementList.begin();
	for (; it!=m_ElementList.end();it++)
	{
		CElement* obj = (CElement*)(*it);
		if (obj)
		{
			delete obj;
			obj = NULL;
		}
	}
	m_ElementList.clear();
}