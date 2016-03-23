#pragma once
#include "stdafxppapi.h"
#include "MarkerElement.h"
#include "Layer.h"
class CShapefileLayer
	: public CLayer
{
private:
	bool m_bExistFile_;
	std::string m_FileName_;			//文件名字
	 
private:
	OGRDataSource* m_pDataSource_;	//数据源
	OGRLayer* m_pLayer_;			//层数
public:
	OGRwkbGeometryType m_ShapefileType_;		//类型

public:
	void Draw();
	CShapefileLayer()
		: CLayer()
		, m_bExistFile_(false)
		, m_FileName_("")
		, m_pDataSource_(NULL)
		, m_pLayer_(NULL)
		, m_ShapefileType_(wkbUnknown)
		, m_Style(NULL)
		, m_Height(0)
		, m_Width(0)
	{
		m_layerType = VLayerType::vHotLayer;
		m_Function[OGRwkbGeometryType::wkbPoint] = &CShapefileLayer::DrawPointLayer;
	}
	~CShapefileLayer() 
	{
		if(m_pDataSource_)
		{
			OGRDataSource::DestroyDataSource(m_pDataSource_);
			m_pDataSource_ = NULL;
		}
		if(m_pLayer_)
		{
		}
	}
	void OpenShapefile(const std::string& filename);
	//显示的样式
	CMarkerStyle* m_Style;				//没有创建	
	int m_Height;				
	int m_Width;

	PointInfoStore m_PointStore_;			//该结构可以用于编


private:
	typedef void (CShapefileLayer::*DrawLayer)();
	std::map<OGRwkbGeometryType, DrawLayer> m_Function;
	void DrawPointLayer();							//绘制点层
	
};