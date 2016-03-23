#include "VtronMap.h"

int CVtronMap::AddLayer(CLayer *layerObject)
{ 
	unsigned int bretHandle = -1; //返回该图层的 ID号
	if (layerObject == nullptr)
	{
		bretHandle = -1;
		return bretHandle;
	}
	if (layerObject->m_layerType==VLayerType::vBaseLayer)
	{	
		//底图
		if (m_BaseLayer)
		{
			//切换地图
			delete m_BaseLayer;
			m_BaseLayer = NULL;
		}
		m_BaseLayer = (layerObject);//static_cast<CBaseLayer*>
		if (m_BaseLayer)
		{
			bretHandle = 0;
		}	
	}
	else
	{
		//热点
		//m_Layers.push_back(layerObject);
		bretHandle = m_LayerId;
		m_Layers.insert(map<int,CLayer*>::value_type(m_LayerId,layerObject)); //创建一个pair对象
		m_LayerId++;
	}

    return bretHandle;
}

bool CVtronMap::RemoveLayer(CLayer *layerObject)
{
	bool bret = true;
	if (layerObject == nullptr)
	{
		bret = false;
		return bret;
	}

	if (m_BaseLayer == layerObject)
	{
		delete m_BaseLayer;
	    m_BaseLayer = nullptr;
	}
	else
	{
		map<int,CLayer*>::iterator it = m_Layers.begin();
		for (;it!=m_Layers.end();it++)
		{
			CLayer* Object = (*it).second;
			if (Object == layerObject)
			{
				delete Object;
				Object=NULL;
		        m_Layers.erase(it);
				break;
			}
		}
	}

	return bret;
}

bool CVtronMap::RemoveLayer(LONG layerid)
{
	bool bret = true;
	if(layerid==0 || m_Layers.size() == 0)
	{
		return false;
	}

	map<int,CLayer*>::iterator it = m_Layers.begin();
	for (;it!=m_Layers.end();it++)
	{
		if((*it).first == layerid)
		{
			CLayer* Object = (*it).second;
			if(Object)
			{
				delete Object;
				Object=NULL;
				m_Layers.erase(it);
			}
			break;
		}
	}

	return bret;
}

bool CVtronMap::RemoveAllLayer()//不能删除底图
{
	bool bret = true;

	map<int,CLayer*>::iterator it = m_Layers.begin();
	for (;it!=m_Layers.end();it++)
	{

		CLayer* Object = (*it).second;
		if(Object)
		{
			delete Object;
			Object=NULL;
		}
	}

	m_Layers.clear();
	return bret;
}
CLayer *CVtronMap::SearchByLayerType(LayerType type,int *layerID)
{
	CLayer *res = NULL;
	LONG layerCount = GetLayerCount();
	for(int i = 0;i < layerCount;++i)
	{
		CLayer *layer = GetLayerByNumber(i+1); //因为 0 是底图
		//考虑到有的图层已经删除，而查找是 for的线性循环 ，所以才有了 这里 if判断
		if (layer == NULL)
		{
			continue;
		}
		if(type == layer->m_layerType)
		{
			res = layer;
			if(layerID)
			 *layerID = i + 1;
		}
	}
	return res;
}
CLayer* CVtronMap::GetLayerByNumber(LONG number)
{
	CLayer *layerObject = NULL;
	if (number == 0)//baseLayer
	{
		layerObject = m_BaseLayer;
	}
	else
	{
		/*
		if(m_Layers.size() == 0)
		{
			return NULL;
		}
		
		map<int, CLayer*>::iterator it = m_Layers.begin();
		for (int i = 1; it != m_Layers.end(); it++)
		{
			if(i == number)
			{
				layerObject = it->second;
				break;
			}
			i++;
		}
		*/
		//modify by xuyan
		map<int, CLayer*>::iterator it = m_Layers.find(number);
		layerObject = (it != m_Layers.end())? it->second:NULL;
	}
	return layerObject;
}
//通过句柄，或者通过sizeid查找
CLayer *CVtronMap::GetLayer(LONG index)
{
	CLayer *layerObject = NULL;
	if (index==0)//baseLayer
	{
		layerObject = m_BaseLayer;
	}
	else
	{
		if(m_Layers.size() == 0)
		{
			return NULL;
		}
		//layerObject = m_Layers[index-1];

		map<int,CLayer*>::iterator it = m_Layers.begin();
		for (;it!=m_Layers.end();it++)
		{
			if((*it).first == index)
			{
				layerObject = (*it).second;
				break;
			}
		}
	}
	return layerObject;
}

//费函数 好像是判断 层号为number的 layer是否存在，这里只需要使用 count或find便可以完成  xuyan
long CVtronMap::GetLayerHandle(LONG number)
{
	int result = -1;
	if (number == 0)//baseLayer
	{
	}
	else
	{
		if(m_Layers.size() == 0)
		{
		}
		map<int, CLayer*>::iterator it = m_Layers.begin();
		for (int i = 1; it != m_Layers.end(); it++)
		{
			if(i == number)
			{
				result = it->first;
				break;
			}
			i++;
		}
	}
	return result;
}
//费函数 xuyan
long CVtronMap::GetLayerNumber(LONG id)
{
	if (id<0)
	{
		return -1;
	}

	int number = 1;
	map<int,CLayer*>::iterator it = m_Layers.begin();
	for (;it!=m_Layers.end();it++)
	{

		if((*it).first == id)
		{
			return number;
		}

		number++;
	}

	return -1;
}

//获取除地图外 所有的图层
LONG CVtronMap::GetLayerCount()
{
	return m_Layers.size();
}

//设置层是否可见:显示隐藏功能
bool CVtronMap::SetLayerVisible(LONG LayerID,bool bVisible)
{
	if (LayerID<0)
	{
		return false;
	}

	map<int,CLayer*>::iterator it = m_Layers.begin();
	for (;it!=m_Layers.end();it++)
	{
		if((*it).first == LayerID)
		{
			CLayer* layerObject = (*it).second;
			layerObject->m_bVisible = bVisible;
			return true;
		}
	}
	return false;
}

void CVtronMap::GetLayerByName(string layerName,CLayer** layer,LONG& id)
{
	CLayer* layerObject = NULL;
	map<int, CLayer*>::iterator it = m_Layers.begin();
	for (; it != m_Layers.end(); it++)
	{
		USES_CONVERSION; 
		CString tmpName = A2W(layerName.c_str());
		layerObject = (*it).second;
		if(layerObject->m_MapName.Compare(tmpName)==0)
		{
			*layer = layerObject;
			id = (*it).first;  //获取图层的ID号
			break;
		}
	}

}