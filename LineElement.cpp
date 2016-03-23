#include"stdafx.h"
#include "LineElement.h"
#include <GLES2/gl2.h>
#include"Triangulate.h"
//#include"MainWnd.h"
#include "ogrsf_frmts.h"
#include "ogr_geometry.h"

//#define SURFACE  //自由绘笔 针对surface未补点
CLineElement::CLineElement(DrawEngine *den)  
{
	//产生两个VBO对象 数字表示符
	glGenBuffers(1, &m_vboID);
	glGenBuffers(1,&m_iboID);
	m_r = 255;
	m_g = 0;
	m_b = 0;
	m_a = 200;
	m_w = 1;
	g_binitPoint=true;
	m_type = 0;
	//m_type = 1;  //修改为默认的情况 还是需要补拐点
	this->den = den;
	m_cast = true;
	m_PassData = true;
	widthSize = 0;
}

//add by xuyan 以离散的 点集 来创建CLineElement对象
CLineElement::CLineElement(int r,int g,int b,int a,int w,DrawEngine *den,bool bfillPoly,const vector<MapEngineES_Point> &line):m_vctPoint(line)
{
	glGenBuffers(1, &m_vboID);
	glGenBuffers(1,&m_iboID);
	m_r = r;
	m_g = g;
	m_b = b;
	m_a = a;
	m_w = w;
	m_cast = true;  //release出错 就是某些变量未初始化，导致自由绘笔 release时候会crash，debug确不会
	m_type = 1;  //这个值 后续会 根据操作来修改
	this->den = den;
	this->m_bFill = bfillPoly;
}

int CLineElement::AddPoint(double x,double y)
{
	MapEngineES_Point point;
	point.x = x;
	point.y = y;
	m_vctPoint.push_back(point);
	return 1;
}

CLineElement::~CLineElement(void)
{
	glDeleteBuffers(1,&m_vboID);
	glDeleteBuffers(1,&m_iboID);
}

//render 一条Line
void CLineElement::DrawLine(double xs,double ys,double xe,double ye,int r, int g,int b,int a, int width)
{
	//单位转化:米转像素
	int linew = width;

	if (m_cast && m_unit.Compare(L"metre") == 0)
	{
		double X,Y,pletflon,plon,pwidth,pheight,pToplat,plat,resolution;
		double scale = 0.0;
		CString name;
		den->GetCurrentShowInfo(X,Y ,pwidth,pheight,pletflon,pToplat,plon,plat,scale,name,resolution);

		if (scale<0.00000001f)
		{
			return;
		}
		linew = width*(96*39.7)/scale;//需要验证????
	}

	//double* sp = new double[4];
	float scolor[4]={1.0f,0.0f,0.0f,0.8f};
	int pWidth = linew;
	double prx1 = xs;
	double pry1 = ys;
	long pix1 =0;
	long piy1 = 0;
	den->Projection_to_Pixel(prx1 ,pry1,pix1,piy1);

	double prx2 = xe;
	double pry2 = ye;
	long pix2 =0;
	long piy2 = 0;
	den->Projection_to_Pixel(prx2 ,pry2,pix2,piy2);

	double x1,y1,x2,y2,x3,y3,x4,y4;

	if(pix1==pix2)
	{
		x1=pix1+pWidth/2.0;x2=pix1-pWidth/2.0;
		x3=pix2+pWidth/2.0;x4=pix2-pWidth/2.0;
		y1=y2=piy1;
		y3=y4=piy2;
	}
	else if(piy1==piy2)
	{
		y1=piy1+pWidth/2.0;y2=piy1-pWidth/2.0;
		y3=piy2+pWidth/2.0;y4=piy2-pWidth/2.0;
		x1=x2=pix1;
		x3=x4=pix2;
	}
	else
	{
		double d=(piy2-piy1)/(pix1-pix2);
		y1=pWidth/2.0/sqrt(d*d+1.0)+piy1;x1=d*(y1-piy1) +pix1;
		y2=-pWidth/2.0/sqrt(d*d+1.0)+piy1;x2=d*(y2-piy1)+pix1;
		y3=pWidth/2.0/sqrt(d*d+1.0)+piy2;x3=d*(y3-piy2)+pix2;
		y4=-pWidth/2.0/sqrt(d*d+1.0)+piy2;x4=d*(y4-piy2)+pix2;
	}

	Vertex* verts = new Vertex[4];

	verts[0].tu = 0;
	verts[0].tv = 0;
	verts[0].loc[0] = x1;
	verts[0].loc[1] = y1;
	verts[0].loc[2] = 1;
	verts[0].color[0] = r/255.0;
	verts[0].color[1] = g/255.0;
	verts[0].color[2] = b/255.0;
	verts[0].color[3] = a/255.0;

	verts[1].tu = 0;
	verts[1].tv = 0;
	verts[1].loc[0] = x2;
	verts[1].loc[1] = y2;
	verts[1].loc[2] = 1;
	verts[1].color[0] = r/255.0;
	verts[1].color[1] = g/255.0;
	verts[1].color[2] = b/255.0;
	verts[1].color[3] = a/255.0;

	verts[2].tu = 0;
	verts[2].tv = 0;
	verts[2].loc[0] = x3;
	verts[2].loc[1] = y3;
	verts[2].loc[2] = 1;
	verts[2].color[0] = r/255.0;
	verts[2].color[1] = g/255.0;
	verts[2].color[2] = b/255.0;
	verts[2].color[3] = a/255.0;

	verts[3].tu = 0;
	verts[3].tv = 0;
	verts[3].loc[0] = x4;
	verts[3].loc[1] = y4;
	verts[3].loc[2] = 1;
	verts[3].color[0] = r/255.0;
	verts[3].color[1] = g/255.0;
	verts[3].color[2] = b/255.0;
	verts[3].color[3] = a/255.0;
	//加上这3行颜色才能出来
	glUseProgram(g_programObj_Shp);
	g_MVPLoc_Shp = glGetUniformLocation(g_programObj_Shp, "a_MVP");
	glUniformMatrix4fv(g_MVPLoc_Shp, 1, GL_FALSE, (GLfloat*) den->g_mpv);

	glBindBuffer(GL_ARRAY_BUFFER,m_vboID);
	glBufferData(GL_ARRAY_BUFFER, 4* sizeof(Vertex), verts,GL_STATIC_DRAW);

	glVertexAttribPointer(g_positionLoc_Shp, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,loc));
	glEnableVertexAttribArray(g_positionLoc_Shp);

	glVertexAttribPointer(g_colorLoc_Shp, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,color));
	glEnableVertexAttribArray(g_colorLoc_Shp);

	/*glVertexAttribPointer(g_texCoordLoc_Shp, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,tu));
	glEnableVertexAttribArray(g_texCoordLoc_Shp);*/

	glDrawArrays(GL_TRIANGLE_STRIP,0,4);//GL_TRIANGLE_STRIP

	delete[] verts;
	verts = NULL;


	//delete sp;
	//sp = NULL;
}

//批量render 多条路线，数据保存在CLineElement::m_vctPoint 这个vector容器中  里面会根据m_type值的不同，作出不同的绘制选择
void CLineElement::DrawLines(int r, int g,int b,int a, int w)
{
	////单位转化:米转像素
	int pWidth = w;  //由js的cmd发来 这里的值是100  表示 Line的宽度
	double d;
	// xuyan 放开的，线宽随着分辨率的不同 而不同,让BufferLine也支持写死 LineWidth
	if (m_cast && m_unit.Compare(L"metre") == 0 )  //这里的默认值 是"" 只有路况 才是metre  lineLuKuang->m_unit = L"metre", 
	{
		double X,Y,pletflon,plon,pwidth,pheight,pToplat,plat,resolution;
		double scale = 0.0;
		CString name;
		den->GetCurrentShowInfo(X,Y ,pwidth,pheight,pletflon,pToplat,plon,plat,scale,name,resolution);

		if (scale<0.00000001f)
		{
			return;
		}
		//pWidth = pWidth*(96*39.7)/scale;//需要验证????
		double an = pWidth / (2 * 3.1415926 * 6378137.0) * 360;
		//d = w / (2 * 3.1415926 * 6378137.0) * 360;
	     pWidth = an/resolution;
	}
	if(!m_cast) //只有路况层 与 线路层 才会进入这个函数
	{
	    //设置新的 LineWith	
		int currLevel = ((CBaseLayer *)den->g_LayerManager.m_BaseLayer)->GetCurrLevel();
		int offset = currLevel - (den->m_level);
		if( offset >= widthSize)  //越界后，不再放大
			offset = widthSize - 1;
		else if(offset <= 0)
			offset = 0;
		pWidth = m_LineWidth[offset];
		
	}

	//原始顶点个数
	int count = m_vctPoint.size();  //组成Line 的离散的 点的个数
	if (count<2)
	{
		return;
	}
	/***step:1********************************指定 shader**************************************************/
	glUseProgram(g_programObj_Shp);  //这里面的着色器 是在MainWnd.cpp中的void InitProgram( void )中完成初始化
	glUniformMatrix4fv(g_MVPLoc_Shp, 1, GL_FALSE, (GLfloat*) den->g_mpv);
	if (w==1) //像素=1
	{
		Vertex* verts = new Vertex[count]; 

		for (int i=0 ;i<count;i++)
		{
			long x1;
			long y1;
			den->Projection_to_Pixel(m_vctPoint[i].x ,m_vctPoint[i].y,x1,y1);  //经纬度 到OpenGL绘制时使用的逻辑坐标值

			verts[i].tu = 0;
			verts[i].tv = 0;
			verts[i].loc[0] = x1;
			verts[i].loc[1] = y1;
			verts[i].loc[2] = 1;
			verts[i].color[0] = m_r/255.0;
			verts[i].color[1] = m_g/255.0;
			verts[i].color[2] = m_b/255.0;
			verts[i].color[3] = m_a/255.0;
		}
		glBindBuffer(GL_ARRAY_BUFFER,m_vboID);
		glBufferData(GL_ARRAY_BUFFER, count* sizeof(Vertex), verts,GL_STATIC_DRAW);

		glVertexAttribPointer(g_positionLoc_Shp, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,loc));
		glEnableVertexAttribArray(g_positionLoc_Shp);

		glVertexAttribPointer(g_colorLoc_Shp, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,color));
		glEnableVertexAttribArray(g_colorLoc_Shp);

		glDrawArrays(GL_LINE_STRIP,0,count);//GL_TRIANGLE_STRIP

		delete[] verts;
		verts = NULL;
//填充多边形
		if(/*den->m_brightUp &&*/ m_bFill) //右键按下后才填充，如果不加这个bool，用户拖动过程中会出现三角片的动画
		{
			den->FillPoly(m_vctPoint,m_r,m_g,m_b,m_a);
			//den->m_brightUp = false;
		}

	}
	else
	{
		//求出要绘制的线的顶点个数
		int triangleCount = (count-1)*2;//每个四边形包含2个三角形，6个顶点id
		int triangleVCount = triangleCount * 3;  //三角形 的顶点个数

		Vertex* verts = new Vertex[triangleVCount];//矩形,调用对象的默认constructor
		Vertex* vertspoint = new Vertex[count];//圆点填 或圆 type=2,3
		
		for (int i=0;i<count-1;i++)   //迭代Line中所有的离散的Point
		{
			double xs = m_vctPoint[i].x;
			double ys = m_vctPoint[i].y;
			long pix1 =0;
			long piy1 = 0;
			den->Projection_to_Pixel(xs ,ys,pix1,piy1);

			double xe = m_vctPoint[i+1].x;
			double ye = m_vctPoint[i+1].y;
			long pix2 =0;
			long piy2 = 0;
			den->Projection_to_Pixel(xe ,ye,pix2,piy2);

			double x1=0.0;
			double y1 = .0;
			double x2 =.0;
			double y2 = .0;
			double x3 = .0;
			double y3 = .0;
			double x4 = .0;
			double y4 = .0;
//构建 两个三角形 的vertex信息
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
			else  //相邻的两个 点 之间的位置 不是特殊位置
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
			
			
			//顶点数组  三角形的构造 1，2，3  3，2，4
			int num = i*6+5;
			if(num <triangleVCount) //防止out of index
			{

				verts[i*6+0].tu = 0.0f;
				verts[i*6+0].tv = 0.0f;
				verts[i*6+0].loc[0] = static_cast<float>(x1);
				verts[i*6+0].loc[1] = static_cast<float>(y1);
				verts[i*6+0].loc[2] = 1.0f;
				verts[i*6+0].color[0] = r/255.0f;
				verts[i*6+0].color[1] = g/255.0f;
				verts[i*6+0].color[2] = b/255.0f;
				verts[i*6+0].color[3] = a/255.0f;

				verts[i*6+1].tu = 0.0f;
				verts[i*6+1].tv = 0.0f;
				verts[i*6+1].loc[0] = static_cast<float>(x2);
				verts[i*6+1].loc[1] = static_cast<float>(y2);
				verts[i*6+1].loc[2] = 1.0f;
				verts[i*6+1].color[0] = r/255.0f;
				verts[i*6+1].color[1] = g/255.0f;
				verts[i*6+1].color[2] = b/255.0f;
				verts[i*6+1].color[3] = a/255.0f;

				verts[i*6+2].tu = 0.0f;
				verts[i*6+2].tv = 0.0f;
				verts[i*6+2].loc[0] = static_cast<float>(x3);
				verts[i*6+2].loc[1] = static_cast<float>(y3);
				verts[i*6+2].loc[2] = 1.0f;
				verts[i*6+2].color[0] = r/255.0f;
				verts[i*6+2].color[1] = g/255.0f;
				verts[i*6+2].color[2] = b/255.0f;
				verts[i*6+2].color[3] = a/255.0f;

				verts[i*6+3].tu = 0.0f;
				verts[i*6+3].tv = 0.0f;
				verts[i*6+3].loc[0] = static_cast<float>(x3);
				verts[i*6+3].loc[1] = static_cast<float>(y3);
				verts[i*6+3].loc[2] = 1.0f;
				verts[i*6+3].color[0] = r/255.0f;
				verts[i*6+3].color[1] = g/255.0f;
				verts[i*6+3].color[2] = b/255.0f;
				verts[i*6+3].color[3] = a/255.0f;

				verts[i*6+4].tu = 0.0f;
				verts[i*6+4].tv = 0.0f;
				verts[i*6+4].loc[0] = static_cast<float>(x2);
				verts[i*6+4].loc[1] = static_cast<float>(y2);
				verts[i*6+4].loc[2] = 1.0f;
				verts[i*6+4].color[0] = r/255.0f;
				verts[i*6+4].color[1] = g/255.0f;
				verts[i*6+4].color[2] = b/255.0f;
				verts[i*6+4].color[3] = a/255.0f;

				verts[i*6+5].tu = 0.0f;
				verts[i*6+5].tv = 0.0f;
				verts[i*6+5].loc[0] = static_cast<float>(x4);
				verts[i*6+5].loc[1] = static_cast<float>(y4);
				verts[i*6+5].loc[2] = 1.0f;
				verts[i*6+5].color[0] = r/255.0f;
				verts[i*6+5].color[1] = g/255.0f;
				verts[i*6+5].color[2] = b/255.0f;
				verts[i*6+5].color[3] = a/255.0f;
			}

			//补点:圆
			vertspoint[i].tu = 0;
			vertspoint[i].tv = 0;
			vertspoint[i].loc[0] = pix1;
			vertspoint[i].loc[1] = piy1;
			vertspoint[i].loc[2] = 1;
			vertspoint[i].color[0] = r/255.0;
			vertspoint[i].color[1] = g/255.0;
			vertspoint[i].color[2] = b/255.0;
			vertspoint[i].color[3] = a/255.0;
			//末端点 也补圆
			if(i+1 == count -1)
			{
				vertspoint[i+1].tu = 0;
				vertspoint[i+1].tv = 0;
				vertspoint[i+1].loc[0] = pix2;
				vertspoint[i+1].loc[1] = piy2;
				vertspoint[i+1].loc[2] = 1;
				vertspoint[i+1].color[0] = r/255.0;
				vertspoint[i+1].color[1] = g/255.0;
				vertspoint[i+1].color[2] = b/255.0;
				vertspoint[i+1].color[3] = a/255.0;			
			}

		}

		//// 绘制矩形 两个离散点之间的线段(直线是有宽度的) 就是一个矩形
/***step:2************************指定存放图形定点的 buffer大小***********************************************************/
		glBindBuffer(GL_ARRAY_BUFFER,m_vboID);
		glBufferData(GL_ARRAY_BUFFER, triangleVCount * sizeof(Vertex), verts,  GL_STATIC_DRAW); //每次都传输数据
/***step:3*****************一个Vertex在buffer中的内存存放格式*************************************************************/
		glVertexAttribPointer(g_positionLoc_Shp, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,loc));
		glEnableVertexAttribArray(g_positionLoc_Shp);

		glVertexAttribPointer(g_colorLoc_Shp, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,color));
		glEnableVertexAttribArray(g_colorLoc_Shp);

		//glVertexAttribPointer(g_texCoordLoc_Shp, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,tu));
		//glEnableVertexAttribArray(g_texCoordLoc_Shp);
/**step:4********************************依据buffer中的 vertex信息来构成 图元*****************************************************/
		glDrawArrays(GL_TRIANGLES,0,triangleVCount);
		delete[] verts;
		verts = NULL;
/*********************************************************************************************************************************/
#ifdef SURFACE
	return; //在surface上屏蔽了补点
#endif

		//使用圆点着色器,补圆点  //自由绘笔
	     if (m_type == 1)
		{
			if (g_binitPoint)  //初始值为true
			{
				CreateCirlePointShader();  //创建点的shader
				g_binitPoint = false;
			}
/*************************************这里是添加的两行代码 *******************************************/
	//在点的shader中添加 varying一个表示大小的变量
			GLint pointsize = glGetAttribLocation(g_programObj_CP,"pointsize"); //step1:取出 着色器中 pointsize这个属性
			glVertexAttrib1f(pointsize,m_w); //step2:使用用户设置的线宽 为这个 pointsize 赋值
/******************************************************************************************************/

			glUseProgram(g_programObj_CP);
			g_MVPLoc_Cp = glGetUniformLocation(g_programObj_CP, "a_MVP");
			glUniformMatrix4fv(g_MVPLoc_Cp, 1, GL_FALSE, (GLfloat*) den->g_mpv);

			glBindBuffer(GL_ARRAY_BUFFER,m_vboID);
			//glBufferData(GL_ARRAY_BUFFER, (count-1) * sizeof(Vertex), vertspoint,GL_STATIC_DRAW);
			glBufferData(GL_ARRAY_BUFFER, count * sizeof(Vertex), vertspoint,GL_STATIC_DRAW); //为了末端也补点
			glVertexAttribPointer(g_positionLoc_CP, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,loc));
			glEnableVertexAttribArray(g_positionLoc_CP);

			glVertexAttribPointer(g_colorLoc_CP, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,color));
			glEnableVertexAttribArray(g_colorLoc_CP);

			//glDrawArrays(GL_POINTS,0,count-1);  //因为最后线段的 终点是不需要补圆的,所以总共才只需要补 count-1个圆
			glDrawArrays(GL_POINTS,0,count);
		}
		
		//补圆:速度慢
		if (m_type == 2)//缓冲   非常慢
		{
			double xs = m_vctPoint[count-1].x;
			double ys = m_vctPoint[count-1].y;
			long pix1 =0;
			long piy1 = 0;
			den->Projection_to_Pixel(xs ,ys,pix1,piy1);
			
			//补最后一个点的信息
			int i = count-1;
			vertspoint[i].tu = 0;
			vertspoint[i].tv = 0;
			vertspoint[i].loc[0] = pix1;
			vertspoint[i].loc[1] = piy1;
			vertspoint[i].loc[2] = 1;
			vertspoint[i].color[0] = r/255.0;
			vertspoint[i].color[1] = g/255.0;
			vertspoint[i].color[2] = b/255.0;
			vertspoint[i].color[3] = a/255.0;
			DrawCirclepat(vertspoint,count,pWidth/2,r,g,b,a);
		}
		delete[] vertspoint;
		vertspoint = NULL;

	}

}

void CLineElement::DrawCircle(double CenX,double CenY,double w,double r,double g,double b,double a)
{
	float start_angle = 0;
	float end_angle = 2 * pi;
	float delta_angle = pi/180;
	int numPoints = 361;

	//long cenx = 0.0;
	//long ceny = 0.0;
	//Projection_to_Pixel(m_CenX,m_CenY,cenx,ceny);

	Vector2dVector tmp;
	LONG pixf,piyf;
	int index = 0;
	for(double i = start_angle;i <= end_angle; i+= delta_angle)
	{
		float vx = CenX + w * cos(i);
		float vy = CenY + w * sin(i);
		tmp.push_back(Vector2d(vx,vy));
		index++;
	}

	Vector2dVector result;
	Triangulate::Process(tmp,result);	
	int tcount = result.size();

	Vertex* verts = new Vertex[tcount];
	for(int  j = 0; j < tcount ; j++ )
	{

		verts[j].tu = 0;
		verts[j].tv = 0;
		verts[j].loc[0] = result[j].GetX();
		verts[j].loc[1] = result[j].GetY();
		verts[j].loc[2] = 1;
		verts[j].color[0] = r;
		verts[j].color[1] = g;
		verts[j].color[2] = b;
		verts[j].color[3] = a;
	}

	glBindBuffer(GL_ARRAY_BUFFER,m_vboID);
	glBufferData(GL_ARRAY_BUFFER, tcount* sizeof(Vertex), verts,
		GL_STATIC_DRAW);

	glVertexAttribPointer(g_positionLoc_Shp, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,loc));
	glEnableVertexAttribArray(g_positionLoc_Shp);

	glVertexAttribPointer(g_colorLoc_Shp, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,color));
	glEnableVertexAttribArray(g_colorLoc_Shp);

	/*glVertexAttribPointer(g_texCoordLoc_Shp, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,tu));
	glEnableVertexAttribArray(g_texCoordLoc_Shp);*/

	glDrawArrays(GL_TRIANGLES,0,tcount);
}


void CLineElement::RenderUseOgles()  //线路高亮，BufferLine会使用这个render函数
{
	if(m_type == 0) //画一条线
	{
		//type=0:简单绘线，不显示拐点
		DrawLine(m_vctPoint[0].x, m_vctPoint[0].y, m_vctPoint[1].x, m_vctPoint[1].y, m_r, m_g, m_b, m_a, m_w);
	}
	else //绘制多条线, 线选操作时，
	{
		DrawLines(m_r,m_g,m_b,m_a,m_w);  //BufferLine就是使用的这个函数
	}

}

void CLineElement::CreateCirlePointShader()
{
	//初始化着色器：矢量
	g_CPvertexShader = compileShader(GL_VERTEX_SHADER, g_VShaderCirclePointData);//g_VShaderCirclePointData
	g_CPfragmentShader = compileShader(GL_FRAGMENT_SHADER,g_FShaderCirclePointData);
	g_programObj_CP = glCreateProgram();
	glAttachShader(g_programObj_CP, g_CPvertexShader);
	glAttachShader(g_programObj_CP, g_CPfragmentShader);
	glLinkProgram(g_programObj_CP);


	g_positionLoc_CP = glGetAttribLocation(g_programObj_CP, "a_position");
	//g_texCoordLoc_CP = glGetAttribLocation(g_programObj_CP, "a_texCoord");
	g_colorLoc_CP = glGetAttribLocation(g_programObj_CP, "a_color");
	g_MVPLoc_Cp = glGetUniformLocation(g_programObj_CP, "a_MVP");

	//判断是否连接上
	GLint link;
	glGetProgramiv(g_programObj_CP,GL_LINK_STATUS,&link);
	if (!link)
	{
		glDeleteProgram(g_programObj_CP);
		g_programObj = 0;
		return;
	}

	g_MVPLoc_Cp = glGetUniformLocation(g_programObj_CP, "a_MVP");
	glUniformMatrix4fv(g_MVPLoc_Cp, 1, GL_FALSE, (GLfloat*) den->g_mpv);//使用全区的mpv
}

void CLineElement::DrawCirclepat(Vertex* verts_circle,int Count,double w,double r,double g,double b,double a)
{
	float start_angle = 0;
	float end_angle = 2 * pi;
	float delta_angle = pi/180;
	int numPoints = 361;
	
	for (int i=0; i<Count; i++)
	{
		Vertex *verts = new Vertex[361];

		double CenX = verts_circle[i].loc[0];
		double CenY = verts_circle[i].loc[1];

		LONG pixf,piyf;
		int index = 0;
		for(double j = start_angle;j <= end_angle; j+= delta_angle)
		{
			float vx = CenX + w * cos(j);
			float vy = CenY + w * sin(j);
			verts[index].tu = 0;
			verts[index].tv = 0;
			verts[index].loc[0] = vx;
			verts[index].loc[1] = vy;
			verts[index].loc[2] = 1;
			verts[index].color[0] = r/255;
			verts[index].color[1] = g/255;
			verts[index].color[2] = b/255;
			verts[index].color[3] = a/255;
			index++;
		}

		glBindBuffer(GL_ARRAY_BUFFER,m_vboID);
		glBufferData(GL_ARRAY_BUFFER, 361* sizeof(Vertex), verts,GL_STATIC_DRAW);

		glVertexAttribPointer(g_positionLoc_Shp, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,loc));
		glEnableVertexAttribArray(g_positionLoc_Shp);

		glVertexAttribPointer(g_colorLoc_Shp, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,color));
		glEnableVertexAttribArray(g_colorLoc_Shp);

		/*glVertexAttribPointer(g_texCoordLoc_Shp, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,tu));
		glEnableVertexAttribArray(g_texCoordLoc_Shp);*/

		glDrawArrays(GL_TRIANGLE_FAN,0,361);//GL_TRIANGLES

		delete verts;
		verts = NULL;
	}

}

//增加一个参数用于 补拐点，add by xy WFSLine
//该函数的功能是批量的用于绘制 三角形，其中pointVec存放的便是vertex集合
void CLineElement::RenderManyOgles(std::vector<Vertex>& theTriangleVector,std::vector<Vertex> &pointVec)
{
	if(theTriangleVector.empty())
		return;
	
	
	glUseProgram(g_programObj_Shp);
	// follow two statement modify by wangyuejiao
	//g_MVPLoc_Shp = glGetUniformLocation(g_programObj_Shp, "a_MVP"); //这个全局变量，已经在InitProgram中初始化
	//glUniformMatrix4fv(g_MVPLoc_Shp, 1, GL_FALSE, (GLfloat*) den->g_mpv);

	//add by wangyuejiao
	GLfloat matTranslation[16];
	identity_matrix(matTranslation);
	memcpy(matTranslation, den->g_mpv, sizeof(Matrix_t));
	multiply_matrix(matTranslation, g_mapTranslationMatrix, matTranslation);
	glUniformMatrix4fv(g_MVPLoc_Shp, 1, GL_FALSE, (GLfloat*) matTranslation);
	//end by wangyuejiao
	glBindBuffer(GL_ARRAY_BUFFER,m_vboID); //VBO 让显卡分配内存空间，并让这块buff成为当前活动的VBO
	
	/*  pass the data to GPU outside not in draw function  modify by xuyan
	if(m_PassData) //决定是否向 显卡更新 新的数据
	{
		glBufferData(GL_ARRAY_BUFFER, theTriangleVector.size() * sizeof(Vertex), (GLvoid*)&(theTriangleVector[0]),GL_STATIC_DRAW); //初始化这块VBO
	}	
	*/
	glEnableVertexAttribArray(g_positionLoc_Shp); //顶点数组
	/*
	启用VAO中对应的顶点属性数组
	index：指定了需要启用的顶点属性数组的索引
	*/
	//分割混合数组
	glVertexAttribPointer(g_positionLoc_Shp, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,loc));
	
	glEnableVertexAttribArray(g_colorLoc_Shp);
	glVertexAttribPointer(g_colorLoc_Shp, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,color));
	glDrawArrays(GL_TRIANGLES, 0, theTriangleVector.size());
}
