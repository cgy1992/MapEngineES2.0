// VtronMap.h : PPAPI����ĵ�ͼ������

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
		//�ͷŵ���ͼ
		delete m_BaseLayer;
		m_BaseLayer = NULL;
	}
private:
	map<int,CLayer*> m_Layers;
	CLayer* m_TempLayer;
	long m_ChangeBaseLayerCount;
	int m_LayerId;

public:
	CLayer* m_BaseLayer;//��ͼ��
	//��ȡͼ����Ŀ
	LONG GetLayerCount();

	//CLayer* GetLayerByName(string sLayerName);

	//���ͼ��
	int AddLayer(CLayer *layerObject);//��ͼ;0;�ȵ�㣺1����̬�㣺2 ����ʱ�㣺3
	//bool GetBaseLayer(CBaseLayer* baseLayer);

	//ɾ��ͼ��
	bool RemoveLayer(CLayer *layerObject);
	bool RemoveLayer(LONG layerid);

	//ɾ�����зǵ�ͼ��
	bool RemoveAllLayer();

	//ͨ��������ȡ��ͼ�����0Ϊ��ͼ��
	CLayer *GetLayer(LONG LayerId);

	//����ͼ���������õ�
	typedef enum VLayerType LayerType;
	CLayer *SearchByLayerType(LayerType type,int *layerID = NULL);
	//���صڼ���ͼ�����
	CLayer* GetLayerByNumber(LONG number);

	//�����ǵڼ���ͼ�㣬���صڼ���ͼ��ľ����
	long GetLayerHandle(LONG number);

	//����ͼ��id������ͼ��number
	long GetLayerNumber(LONG id);

	//����ͼ����������ͼ�����
	void GetLayerByName(string layerName,CLayer** layer,LONG& id);

	//���ò��Ƿ�ɼ�
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
		m_LayerId = 1;//��1��ʼ
	}
};
