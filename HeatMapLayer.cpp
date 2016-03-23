#include "HeatMapLayer.h"
#include "DrawEngine.h"

HeatMapLayer::HeatMapLayer(enum VLayerType theLayerType,DrawEngine *den):
 _heatMap(den->g_CtrlW,den->g_CtrlH,den)
,den(den)
,_reloadData(true)  //默认让它 平移重新的加载数据(projection_to_pixel)
{
	setLayerType(theLayerType);
}

void HeatMapLayer::Draw(){
	if(isVisiable())
	{
		if(isReloadData()){
			_heatMap.clear();
			_heatMap.addPoints(_heatPointSet);
		}
		_heatMap.update();
		_heatMap.display();
	}
}

void HeatMapLayer::loadDataFromJSON(const Json::Value& dataSet)
{
	HeatPoint pt;

	Json::Value pointSize = dataSet["PointSize"];
	if(pointSize != Json::nullValue)
		pt.setSize(pointSize.asInt());

	Json::Value intensity = dataSet["Intensity"];
	if(intensity != Json::nullValue)
		pt.setIntensity(intensity.asDouble());

	Json::Value pointSet = dataSet["PointSet"];

	if(pointSet != Json::nullValue){
		for(int i = 0;i < pointSet.size();++i){
			Json::Value pos = pointSet[i];
			double geoX = pos["x"].asDouble();
			double geoY = pos["y"].asDouble();
			pt.setGeoPos(geoX,geoY);
			_heatPointSet.push_back(pt);
		}
	}

	if(0 != _heatPointSet.size()){
		_heatMap.addPoints(_heatPointSet);
	}
}
