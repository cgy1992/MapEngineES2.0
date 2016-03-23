#include "ShapefileLayer.h"

void CShapefileLayer::Draw()
{
	if(m_bExistFile_ && m_ShapefileType_ != OGRwkbGeometryType::wkbUnknown)
		(this->*m_Function[m_ShapefileType_])();
}
/*
	1	3
	0	2
 */
void CShapefileLayer::DrawPointLayer()
{
	m_pLayer_->ResetReading();
	if (m_PointStore_.m_PointRecorderVector_.empty())
	{
		//第一次读取
		PointInfoStore::PointRecorder_ theRecorder;
		theRecorder.bAdd_ = false;				//新增加
		theRecorder.bChanged_ = false;			//坐标位置改变
		theRecorder.bDelete_ = false;			//删除
		theRecorder.bEdit_ = false;				//编辑过的
		OGRFeature* pFeature = m_pLayer_->GetNextFeature();
		while (pFeature)
		{
			OGRGeometry* theGeometry = pFeature->GetGeometryRef();
			OGRPoint* thePoint = (OGRPoint*)theGeometry;
			theRecorder.iFid_ = pFeature->GetFID();
			theRecorder.pointx_ = thePoint->getX();
			theRecorder.pointy_ = thePoint->getY();
			m_PointStore_.m_PointRecorderVector_.push_back(theRecorder);
			OGRFeature::DestroyFeature(pFeature);
			pFeature = m_pLayer_->GetNextFeature();
		}
	}
	int PointNumber = m_PointStore_.m_PointRecorderVector_.size();
	PointInfoStore::PointRecorder_ theCurrentPoint;
	std::vector<Vertex> theVertexVector;
	Vertex verts[6];

	int showWidth = m_Style->m_ImageWidth_;
	int showHeight = m_Style->m_ImageHeight_;

	if(m_Height > 0 && m_Width > 0)
	{
		showWidth = m_Width;
		showHeight = m_Height;
	}
	//组装数据
	for (int i = 0; i < PointNumber; i++)			//遍历
	{
		int pix = 0;
		int piy = 0;
		theCurrentPoint = m_PointStore_.m_PointRecorderVector_[i];
		ProjectionToPix(theCurrentPoint.pointx_, theCurrentPoint.pointy_, pix, piy);
		if(pix > 0 && pix < m_CtrlWidth && piy < m_CtrlHeight && piy > 0)
		{
			//0
			verts[0].tu = 0;
			verts[0].tv = 0;
			verts[0].loc[0] = pix - showWidth / 2;
			verts[0].loc[1] = piy - showHeight / 2;
			verts[0].loc[2] = g_Zlocation;
			theVertexVector.push_back(verts[0]);
			//1
			verts[1].tu = 0;
			verts[1].tv = 1;
			verts[1].loc[0] = pix - showWidth / 2;
			verts[1].loc[1] = piy + showHeight / 2;
			verts[1].loc[2] = g_Zlocation;
			theVertexVector.push_back(verts[1]);
			//2
			verts[2].tu = 1;
			verts[2].tv = 0;
			verts[2].loc[0] = pix + showWidth / 2;
			verts[2].loc[1] = piy - showHeight / 2;
			verts[2].loc[2] = g_Zlocation;
			theVertexVector.push_back(verts[2]);
			//2
			verts[3].tu = 1;
			verts[3].tv = 0;
			verts[3].loc[0] = pix + showWidth / 2;
			verts[3].loc[1] = piy - showHeight / 2;
			verts[3].loc[2] = g_Zlocation;
			theVertexVector.push_back(verts[3]);
			//1
			verts[4].tu = 0;
			verts[4].tv = 1;
			verts[4].loc[0] = pix - showWidth / 2;
			verts[4].loc[1] = piy + showHeight / 2;
			verts[4].loc[2] = g_Zlocation;
			theVertexVector.push_back(verts[4]);
			//3
			verts[5].tu = 1;
			verts[5].tv = 1;
			verts[5].loc[0] = pix + showWidth / 2;
			verts[5].loc[1] = piy + showHeight / 2;
			verts[5].loc[2] = g_Zlocation;
			theVertexVector.push_back(verts[5]);
		}
	}
	//绘制
	CMarkerElement theElement;
	theElement.m_Style = m_Style;
	theElement.RenderManyUseOgles(theVertexVector);
}
void CShapefileLayer::OpenShapefile(const std::string& filename)
{
	m_FileName_ = filename;
	OGRRegisterAll();
	std::string pszDriverName = "ESRI Shapefile";
	//CPLSetConfigOption("SHAPE_ENCODING", "");				//支持中文

	OGRSFDriver* poDriver = OGRSFDriverRegistrar::GetRegistrar()->GetDriverByName(pszDriverName.c_str());
	m_pDataSource_ = poDriver->Open(m_FileName_.c_str(), false);				//打开shapefile文件，获取数据源
	if(m_pDataSource_ == NULL)
	{
		m_bExistFile_ = false;
		return;
	}
	m_pLayer_ = m_pDataSource_->GetLayer(0);						//获取shapefile第0层
	if(m_pLayer_ == NULL)
	{
		m_bExistFile_ = false;
		return ;
	}
	int theFeatureCount = m_pLayer_->GetFeatureCount();				//层里面的数据数量
    OGRFeature *poFeature = NULL;									//读取的数据指针
	m_pLayer_->ResetReading();										//重新读取
	m_ShapefileType_ = m_pLayer_->GetLayerDefn()->GetGeomType();	//类型
	m_bExistFile_ = true;
}