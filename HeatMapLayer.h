#pragma once
#include "layer.h"
#include "HeatMap.h"
#include "DataStruct.h"
#include <vector>

class DrawEngine;

class HeatMapLayer :public CLayer
{
public:
	HeatMapLayer(enum VLayerType theLayerType,DrawEngine *den);
	~HeatMapLayer(void){};
public:
	virtual void Draw();
	void loadDataFromJSON(const Json::Value& dataSet);
	void addPoint(const HeatPoint &pt){
		_heatPointSet.push_back(pt);
		_heatMap.addPoint(pt);
	}
	void setReloadData(bool flag){	_reloadData = flag;}
	bool isReloadData(){ return _reloadData;}
private:
	HeatMap _heatMap;
	DrawEngine *den;
	std::vector<HeatPoint> _heatPointSet;
	bool _reloadData;
};

