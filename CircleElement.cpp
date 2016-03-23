#include "stdafx.h"
#include "CircleElement.h"
#include <GLES2/gl2.h>
#include"Triangulate.h"
#define pi 3.1415926
#include "VtronMap.h"
class CVtronMap;
extern CVtronMap g_LayerManager;//容器
double xbegin = 0;			//起点
double ybegin = 0;
double xend = 0;			//终点
double yend = 0;

CCircleElement::CCircleElement(DrawEngine *den)
{
	m_Rinner = 0;
	glGenBuffers(1, &m_vboID);
	glGenBuffers(1, &m_iboID);
	this->den = den;
	m_bBufferVedio = false;
}


CCircleElement::~CCircleElement(void)
{
	glDeleteBuffers(1,&m_vboID);
	glDeleteBuffers(1,&m_iboID);
}

// 画圆环
void CCircleElement::RenderUseOgles()
{
	glUseProgram(g_programObj_Shp);
	g_MVPLoc = glGetUniformLocation(g_programObj_Shp, "a_MVP");
	glUniformMatrix4fv(g_MVPLoc, 1, GL_FALSE, (GLfloat*) den->g_mpv);

	//单位转化:米转像素
	double radius1 = m_Rinner;  //内半径
	double radius2 = m_Router;  //外半径
	if (m_unit.Compare(L"metre") == 0)
	{
		double X,Y,pletflon,plon,pwidth,pheight,pToplat,plat,resolution;
		double scale = 0.0;
		CString name;
		den->GetCurrentShowInfo(X,Y ,pwidth,pheight,pletflon,pToplat,plon,plat,scale,name,resolution);

		if (scale<0.00000001f)
		{
			return;
		}
		radius1 = m_Rinner*(96*39.7)/scale;//需要验证????
		radius2 = m_Router*(96*39.7)/scale;
	}

	if (m_Rinner==0)  // 内半径
	{
		float start_angle = 0;
		float end_angle = 2 * pi;
		float delta_angle = pi/180;
		int numPoints = 360*3;

		long cenx = 0.0;
		long ceny = 0.0;
		den->Projection_to_Pixel(m_CenX,m_CenY,cenx,ceny);

		Vertex *verts = new Vertex[numPoints];

		int index = 0;

		for(double j = start_angle;j <= end_angle; j+= delta_angle)
		{
			float vxout = cenx + radius2 * cos(j);
			float vyout = ceny + radius2 * sin(j);

			float vxout1 = cenx + radius2 * cos(j+delta_angle);
			float vyout1 = ceny + radius2 * sin(j+delta_angle);


			verts[index].tu = 0;
			verts[index].tv = 0;
			verts[index].loc[0] = cenx;
			verts[index].loc[1] = ceny;
			verts[index].loc[2] = 1;
			verts[index].color[0] = m_r/255.0;
			verts[index].color[1] = m_g/255.0;
			verts[index].color[2] = m_b/255.0;
			verts[index].color[3] = m_a/255.0;

			verts[index+1].tu = 0;
			verts[index+1].tv = 0;
			verts[index+1].loc[0] = vxout;
			verts[index+1].loc[1] = vyout;
			verts[index+1].loc[2] = 1;
			verts[index+1].color[0] = m_r/255.0;
			verts[index+1].color[1] = m_g/255.0;
			verts[index+1].color[2] = m_b/255.0;
			verts[index+1].color[3] = 50/255.0;

			verts[index+2].tu = 0;
			verts[index+2].tv = 0;
			verts[index+2].loc[0] = vxout1;
			verts[index+2].loc[1] = vyout1;
			verts[index+2].loc[2] = 1;
			verts[index+2].color[0] = m_r/255.0;
			verts[index+2].color[1] = m_g/255.0;
			verts[index+2].color[2] = m_b/255.0;
			verts[index+2].color[3] = 50/255.0;

			index += 3;
		}

//使用三角
		glBindBuffer(GL_ARRAY_BUFFER,m_vboID);
		glBufferData(GL_ARRAY_BUFFER, numPoints* sizeof(Vertex), verts,
			GL_STATIC_DRAW);

		glVertexAttribPointer(g_positionLoc_Shp, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,loc));
		glEnableVertexAttribArray(g_positionLoc_Shp);

		glVertexAttribPointer(g_colorLoc_Shp, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,color));
		glEnableVertexAttribArray(g_colorLoc_Shp);

		/*glVertexAttribPointer(g_texCoordLoc_Shp, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,tu));
		glEnableVertexAttribArray(g_texCoordLoc_Shp);*/

		glDrawArrays(GL_TRIANGLES,0,numPoints);

		delete verts;
		verts = NULL;
	}
	else
	{
		//三角扇形化圆环
		float start_angle = 0;
		float end_angle = 2 * pi;
		float delta_angle = pi/180;
		int numPoints = 360*6;

		long cenx = 0.0;
		long ceny = 0.0;
		den->Projection_to_Pixel(m_CenX,m_CenY,cenx,ceny);

		LONG pixf,piyf;
		int index = 0;
		Vertex* verts = new Vertex[2160];

		for(double j = start_angle;j <= end_angle; j+= delta_angle)
		{
			float vxout = cenx + radius2 * cos(j);
			float vyout = ceny + radius2 * sin(j);

			float vxout1 = cenx + radius2 * cos(j+delta_angle);
			float vyout1 = ceny + radius2 * sin(j+delta_angle);

			float vxin = cenx + radius1 * cos(j);
			float vyin = ceny + radius1 * sin(j);

			float vxin1 = cenx + radius1 * cos(j+delta_angle);
			float vyin1 = ceny + radius1 * sin(j+delta_angle);

			verts[index].tu = 0;
			verts[index].tv = 0;
			verts[index].loc[0] = vxin;
			verts[index].loc[1] = vyin;
			verts[index].loc[2] = 1;
			verts[index].color[0] = m_r/255.0;
			verts[index].color[1] = m_g/255.0;
			verts[index].color[2] = m_b/255.0;
			verts[index].color[3] = 180/255.0;

			verts[index+1].tu = 0;
			verts[index+1].tv = 0;
			verts[index+1].loc[0] = vxout;
			verts[index+1].loc[1] = vyout;
			verts[index+1].loc[2] = 1;
			verts[index+1].color[0] = m_r/255.0;
			verts[index+1].color[1] = m_g/255.0;
			verts[index+1].color[2] = m_b/255.0;
			verts[index+1].color[3] = 50/255.0;

			verts[index+2].tu = 0;
			verts[index+2].tv = 0;
			verts[index+2].loc[0] = vxin1;
			verts[index+2].loc[1] = vyin1;
			verts[index+2].loc[2] = 1;
			verts[index+2].color[0] = m_r/255.0;
			verts[index+2].color[1] = m_g/255.0;
			verts[index+2].color[2] = m_b/255.0;
			verts[index+2].color[3] = 180/255.0;

			verts[index+3].tu = 0;
			verts[index+3].tv = 0;
			verts[index+3].loc[0] = vxin1;
			verts[index+3].loc[1] = vyin1;
			verts[index+3].loc[2] = 1;
			verts[index+3].color[0] = m_r/255.0;
			verts[index+3].color[1] = m_g/255.0;
			verts[index+3].color[2] = m_b/255.0;
			verts[index+3].color[3] = 180/255.0;

			verts[index+4].tu = 0;
			verts[index+4].tv = 0;
			verts[index+4].loc[0] = vxout;
			verts[index+4].loc[1] = vyout;
			verts[index+4].loc[2] = 1;
			verts[index+4].color[0] = m_r/255.0;
			verts[index+4].color[1] = m_g/255.0;
			verts[index+4].color[2] = m_b/255.0;
			verts[index+4].color[3] = 50/255.0;

			verts[index+5].tu = 0;
			verts[index+5].tv = 0;
			verts[index+5].loc[0] = vxout1;
			verts[index+5].loc[1] = vyout1;
			verts[index+5].loc[2] = 1;
			verts[index+5].color[0] = m_r/255.0;
			verts[index+5].color[1] = m_g/255.0;
			verts[index+5].color[2] = m_b/255.0;
			verts[index+5].color[3] = 50/255.0;

			index += 6;
		}

		glBindBuffer(GL_ARRAY_BUFFER,m_vboID);
		glBufferData(GL_ARRAY_BUFFER, numPoints* sizeof(Vertex), verts,GL_STATIC_DRAW);

		glVertexAttribPointer(g_positionLoc_Shp, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,loc));
		glEnableVertexAttribArray(g_positionLoc_Shp);

		glVertexAttribPointer(g_colorLoc_Shp, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,color));
		glEnableVertexAttribArray(g_colorLoc_Shp);

		/*glVertexAttribPointer(g_texCoordLoc_Shp, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex,tu));
		glEnableVertexAttribArray(g_texCoordLoc_Shp);*/

		glDrawArrays(GL_TRIANGLES,0,numPoints);

		delete verts;
		verts = NULL;
	}


	if (m_bBufferVedio)
	{
		//RenderUseOglesForVideo();
	}
	return;
}

void CStringAToUtf8(const CStringA& from, CStringA& to)
{
	//ANSI转换为宽字符
	int nLen = MultiByteToWideChar(CP_ACP, 0, from, -1, NULL, 0);
	WCHAR* szWideBuf = new WCHAR[nLen + 1];
	memset(szWideBuf, 0, sizeof(szWideBuf));
	MultiByteToWideChar(CP_ACP, 0, from, -1, szWideBuf, nLen);
	//宽字符转换成utf-8
	nLen = WideCharToMultiByte(CP_UTF8, 0, szWideBuf, -1, NULL, 0, NULL, NULL);
	char* szUtf8 = new char[nLen + 1];
	memset(szUtf8, 0, nLen + 1);
	WideCharToMultiByte(CP_UTF8, 0, szWideBuf, -1, szUtf8, nLen, NULL, NULL);
	to.Format("%s", szUtf8);
	delete[] szUtf8;
}

//void CCircleElement::RenderUseOglesForVideo()
//{
//	glUseProgram(g_programObj);
//	g_MVPLoc = glGetUniformLocation(g_programObj, "a_MVP");
//	glUniformMatrix4fv(g_MVPLoc, 1, GL_FALSE, (GLfloat*) den->g_mpv);
//
//	//单位转化:米转像素
//	double radius1 = m_Rinner;
//	double radius2 = m_Router;
//	if (m_unit.Compare(L"metre") == 0)
//	{
//		double X,Y,pletflon,plon,pwidth,pheight,pToplat,plat,resolution;
//		double scale = 0.0;
//		CString name;
//		den->GetCurrentShowInfo(X,Y ,pwidth,pheight,pletflon,pToplat,plon,plat,scale,name,resolution);
//		
//		if (scale<0.00000001f)
//		{
//			return;
//		}
//		radius1 = m_Rinner*(96*39.7)/scale;//需要验证????
//		radius2 = m_Router*(96*39.7)/scale;
//	}
//
//	long cenx = 0.0;
//	long ceny = 0.0;
//	den->Projection_to_Pixel(m_CenX,m_CenY,cenx,ceny);
//	double xbegin = cenx-radius2;//*cos(45.0);
//	double ybegin = ceny+radius2;//*sin(45.0);
//	double xend = cenx+radius2;//*cos(45.0);
//	double yend = ceny-radius2;//*sin(45.0);
//
//	double x0,y0,x1,y1;
//
//	den->Pixel_to_Projection(xbegin,ybegin,x0,y0);
//	den->Pixel_to_Projection(xend,yend,x1,y1);
//
//	int layercount = g_LayerManager.GetLayerCount();		//不包括底图
//	while(layercount > 0)
//	{
//		vector<MapEngineES_Point> theChooseReturnValue;
//		vector<MapEngineES_Point> theReturnValue;
//		//if(isExistInShapefileForVideo(layercount, x0, y1, x1, y0, theReturnValue))
//		if(isExistInShapefileForVideo(layercount, x0, y0, x1, y1, theReturnValue))
//		{
//			//图层路径
//			CString szExePath = L"";
//			::GetModuleFileName(NULL,szExePath.GetBufferSetLength(MAX_PATH+1),MAX_PATH);
//			szExePath = szExePath.Left(szExePath.ReverseFind('\\'));		
//			szExePath = szExePath.Left(szExePath.ReverseFind('\\'));	
//
//			int size = theReturnValue.size();
//			int id = 0;
//			for (int i=0;i<size;i++)
//			{			
//				long pix,piy;
//				Projection_to_Pixel(theReturnValue[i].x,theReturnValue[i].y,pix,piy);
//
//				//用圆的外接正方形作了空间索引以后，在进行筛选
//				double xx = (pix-cenx)*(pix-cenx);
//				double yy = (piy-ceny)*(piy-ceny);
//				double d = sqrt((xx+yy)*1.0);
//				if (d>radius2)
//				{
//					theReturnValue[i].fid = -1;
//					continue;
//				}
//
//				id++;
//				CString path ;
//				path.Format(L"%s\\ICON\\VIDEOTIP\\%d.png",szExePath,id);
//				if (PathFileExists(path))
//				{				
//					Tile *tile =  new Tile(path,pix,piy,0,0);
//					tile->RenderUseOgles();
//					delete tile;
//
//				}
//
//			}	
//
//			CStringA retValue;			//返回的字符串格式FID ;
//			retValue.Format("%d", theReturnValue[0].fid);
//			//往js发送信息
//			for(int i=1;i<theReturnValue.size();i++)
//			{
//				if (theReturnValue[i].fid != -1)
//				{
//					CStringA fidString;
//					retValue += ",";
//					fidString.Format("%d", theReturnValue[i].fid);
//					retValue += fidString;
//				}
//			}
//			CStringA ret;
//			ret.Format("{\"CmdName\":\"VIDEOSEARCH\",\"SearchResult\":[%s]}", retValue);
//			PostMessage(ret.GetBuffer());
//			break;
//		}
//		layercount--;
//	}
//
//	return;
//}