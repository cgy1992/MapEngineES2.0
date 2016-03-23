#pragma once
#include "stdafx.h"
#include "DataStruct.h"


class CLayer
{
public:
	CLayer();
	virtual ~CLayer(void);
	virtual void Draw(){};

	double m_x;//中心点
	double m_y;
	int m_js; //当前显示级别
	int m_PicW;//切片宽高是一致的
	int m_PicH;

	CString m_MapName;//地图名或者图层名
	CString m_sPath;//始终指向本地路径，如果是服务上的地图路径，那么m_sPath就是指向我们构造的缓存图片的 本地路径
	double m_resolution;

	enum VLayerType m_layerType; //图层名称
	void setLayerType(enum VLayerType layerType){ m_layerType = layerType; }

	bool m_bVisible;
	void setVisiable(bool flag){ m_bVisible = flag;}
	bool isVisiable(){ return m_bVisible;}
};

