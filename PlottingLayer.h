#pragma once
#include "Layer.h"
#include <set>
//#include "CELLShader.hpp"


/*
 标绘图层
*/
class DrawEngine;
class CMarkerElement;

class PlottingLayer :
	public CLayer
{
	typedef int StyleId;
	typedef unsigned LabelId;
public:
	PlottingLayer(DrawEngine *den, VLayerType layerType = vPlottingLayer);
	~PlottingLayer(){ delete _rendMark; _rendMark = NULL; }
public: //提供的API接口
	LabelId addMark(const MarkPoint &pt); //增加标绘，返回图标的ID 便于管理
	void deleteMarkById(LabelId lableId);
	void modifyMarkPosById(LabelId lableId, const GeoPos &pos);//修改 单label的位置
	void clearMark(); //清空所有的标绘
	void synMark(); //大墙端 同步所有的 图标
	virtual void Draw();
private:
	//PROGRAM_POINT _shader;
	CMarkerElement *_rendMark;
	DrawEngine *_den;
	VLayerType _layerType; //图层类型
	LabelId _labelId;  //标识 mark的唯一ID，初始化
	std::set<StyleId> _styleSet;
	std::map<LabelId, MarkPoint> _markData; //分类绘制
};

