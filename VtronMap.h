// VtronMap.h : PPAPI插件的地图管理类

#pragma once
#include "BaseLayer.h"
#include <vector>
#include <map>
using namespace std;
// CVtronMap
class  CVtronMap
{
public:
	CVtronMap()
	{
		 m_BaseLayer = NULL;
		 m_ChangeBaseLayerCount = 0;
		 m_LayerId = 1;
		 m_TempLayer = NULL;
	}

	~CVtronMap()
	{
		int LayerCount = m_Layers.size();
		for (int i=0;i<LayerCount;i++)
		{		
			CLayer *layer = m_Layers[i];
			if (layer)
			{
				delete layer;
				layer = NULL;
			}
		}
		//释放掉地图
		delete m_BaseLayer;
		m_BaseLayer = NULL;
	}
private:
	map<int,CLayer*> m_Layers;
	CLayer* m_TempLayer;
	long m_ChangeBaseLayerCount;
	int m_LayerId;

public:
	CLayer* m_BaseLayer;//底图层
	//获取图层数目
	LONG GetLayerCount();

	//CLayer* GetLayerByName(string sLayerName);

	//添加图层
	int AddLayer(CLayer *layerObject);//底图;0;热点层：1，动态层：2 ，临时层：3
	//bool GetBaseLayer(CBaseLayer* baseLayer);

	//删除图层
	bool RemoveLayer(CLayer *layerObject);
	bool RemoveLayer(LONG layerid);

	//删除所有非底图层
	bool RemoveAllLayer();

	//通过索引获取到图层对象：0为底图层
	CLayer *GetLayer(LONG LayerId);

	//根据图层类型来得到
	typedef enum VLayerType LayerType;
	CLayer *SearchByLayerType(LayerType type,int *layerID = NULL);
	//返回第几个图层对象
	CLayer* GetLayerByNumber(LONG number);

	//根据是第几个图层，返回第几个图层的句柄号
	long GetLayerHandle(LONG number);

	//根据图层id，返回图层number
	long GetLayerNumber(LONG id);

	//根据图层名，返回图层对象
	void GetLayerByName(string layerName,CLayer** layer,LONG& id);

	//设置层是否可见
	bool SetLayerVisible(LONG LayerID,bool bVisible);

	void release()
	{
		if (m_BaseLayer)
		{
			delete m_BaseLayer;
			m_BaseLayer = NULL;
		}
		int LayerCount = m_Layers.size();
		for (int i=0;i<LayerCount;i++)
		{		
			CLayer *layer = m_Layers[i];
			if (layer)
			{
				delete layer;
				layer = NULL;
			}
		}

		m_Layers.clear();
		m_LayerId = 1;//从1开始
	}
};
