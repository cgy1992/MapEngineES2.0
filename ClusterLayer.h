#pragma once
#include "Layer.h"
#include "CELLShader.hpp"
#include "CELLMath.hpp"
#include "CELLFont.h"
#include "stdafxppapi.h"
#include <set>
#include <vector>
#include <map>

class DrawEngine;

class ClusterLayer :public CLayer
{
	typedef std::pair<GeoPos, std::vector<GeoPos> > ClusterPoint;
	typedef int PointID;
	typedef int Index2Vec;
	enum{ SMALL=2,MIDDLE=10};
public:
	ClusterLayer(enum VLayerType theLayerType, DrawEngine *den);
	~ClusterLayer(){}
public:
	virtual void Draw();
	void loadDateFromJson(const Json::Value& dataInfo);
	void cluster();
	void addPoint(const GeoPos &pt){ _dataSet.push_back(pt); }
	void addStyleId(int styleId){ _compoundStyleId.push_back(styleId); }
	void setReloadData(bool flag){ _reloadData = flag; } //地图缩放时，重新加载数据
	bool isReloadData(){ return _reloadData; }
	void setInterval(int interval){ _interval = interval; }

	//不展开
	void spreadCompoundPoint(int pointId){
		if (_isSpreadIndex.count(pointId))
			_isSpreadIndex[pointId] = !_isSpreadIndex[pointId];
	}

	std::vector<int> searchByPoint(PP_Point& mousePoint); //返回击中 点的ID
	bool getRgn(CRgn &pointArea);
private:
	int distance(const GeoPos &pt1, const GeoPos &pt2);
	void clear(){
		_clusterDataSet.clear();
		_compoundPointIndex.clear();
		_isSpreadIndex.clear();
		_compoundCount.clear();
		_compoundSort.clear();
	}
	void DrawBySort(int pointSize, int clusterCount, GLuint textureId);
	//由不同的聚合点的 数目 来设定搜索矩形的不同大小
	void setRectByClusterCount(const PixPoint &pos,int clusterCount, RECT &rect){
		if (clusterCount < SMALL){
			SetRect(&rect, pos.x_ - _pointSize[0] / 2, pos.y_ - _pointSize[0] / 2, pos.x_ + _pointSize[0] / 2, pos.y_ + _pointSize[0] / 2);
			return;
		}
		if (clusterCount >= SMALL && clusterCount < MIDDLE){
			SetRect(&rect, pos.x_ - _pointSize[1] / 2, pos.y_ - _pointSize[1] / 2, pos.x_ + _pointSize[1] / 2, pos.y_ + _pointSize[1] / 2);
			return;
		}
		if (clusterCount >= MIDDLE){
			SetRect(&rect, pos.x_ - _pointSize[2] / 2, pos.y_ - _pointSize[2] / 2, pos.x_ + _pointSize[2] / 2, pos.y_ + _pointSize[2] / 2);
			return;
		}
	}
private:
	DrawEngine  *den;
	std::vector<GeoPos> _dataSet; //原始数据
	std::vector<int> _compoundStyleId;  //get CMarkerStyle* 对象
	std::vector<int> _pointSize;
	int _interval; //聚合的间隔默认10像素
	PROGRAM_POINT _shader;
	CELLFont        _font;
	bool _reloadData;  //表示是否需要重新聚合数据
private: //聚合分析的数据
	std::vector<ClusterPoint> _clusterDataSet; //聚合完成后的需要显示的点(原始数据 经过处理后)
	std::map<PointID, Index2Vec> _compoundPointIndex; //由点的ID 找到与它聚合的点
	std::map<int, bool> _isSpreadIndex; //是否散开
	//使用map<int,vector<GeoPos> >   //其中key 就是聚合的数目 （分类的目的是便于 绘制）
	std::set<int> _compoundCount; //聚合数目的种类
	std::multimap<int, GeoPos> _compoundSort; // 按聚合点数目 分类的GeoPos

	//针对聚合的单点应该区分开
	std::map<PointID,vector<ClusterPoint> > _singlePointSet;
};

