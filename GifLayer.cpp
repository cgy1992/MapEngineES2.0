#include "GifLayer.h"
#include "GifElement.h"

CGifLayer::CGifLayer(void)
{
	m_layerType = VLayerType::vGifLayer;
	m_minDelay = -1;
	m_ElementID_ = 1;

	m_ElementMap_.clear();
}


CGifLayer::~CGifLayer(void)
{
	DeleteTheAllItem();
}

void CGifLayer::DeleteTheAllItem()
{
	std::map<long, CElement*>::iterator theBeg = m_ElementMap_.begin();
	std::map<long, CElement*>::iterator theEnd = m_ElementMap_.end();
	for (; theBeg != theEnd; theBeg++)
	{
		CGifElement* theElement = (CGifElement*)(theBeg->second);
		delete theElement;
	}
	m_ElementMap_.clear();
}
int CGifLayer::AddElement(CElement* elementObject)
{
	m_ElementMap_.insert(std::map<long, CElement*>::value_type(m_ElementID_, elementObject));
	CGifElement* gifobj = (CGifElement*)elementObject;
	if (gifobj)
	{
		if (m_minDelay == -1 || gifobj->m_delay - m_minDelay < 0)
		{
			m_minDelay = gifobj->m_delay;
		}
	}
	return m_ElementID_++;
}

double CGifLayer::GetMinDelay()
{
	return m_minDelay;
}


void CGifLayer::Draw(void)
{
	if (m_ElementMap_.size() == 0)
	{
		return;
	}

	std::map<long, CElement*>::iterator it = m_ElementMap_.begin();
	for (; it!=m_ElementMap_.end();it++)
	{
		CGifElement* obj = (CGifElement*)(it->second);
		if (obj)
		{
			obj->RenderUseOgles(m_minDelay);	
		}
	}
}


void CGifLayer::Draw(int move)
{
	if (m_ElementMap_.size() == 0)
	{
		return;
	}

	std::map<long, CElement*>::iterator it = m_ElementMap_.begin();
	for (; it!=m_ElementMap_.end();it++)
	{
		CGifElement* obj = (CGifElement*)(it->second);
		if (obj)
		{
			obj->RenderUseOgles(0);	
		}
	}
}
void CGifLayer::DeleteElement(int ElementHandle)
{
	// 查找map 不需要这样遍历
	//std::map<long, CElement*>::iterator theBeg = m_ElementMap_.begin();
	//std::map<long, CElement*>::iterator theEnd = m_ElementMap_.end();
	//for (; theBeg != theEnd; theBeg++)
	//{
	//	if(theBeg->first == ElementHandle)
	//	{
	//		CGifElement* theElement = (CGifElement*)(theBeg->second);
	//		if (theElement == NULL)
	//		{
	//			MessageBox(NULL,L"gif==null",L"",0);
	//		}
	//		delete theElement;
	//		m_ElementMap_.erase(theBeg);
	//		break;
	//	}
	//}
	std::map<long, CElement*>::iterator iter = m_ElementMap_.find(ElementHandle);
	if(iter != m_ElementMap_.end())
	{
		 CGifElement* theElement = (CGifElement*)(iter->second);
		 if(NULL == theElement)
		 {
		   MessageBox(NULL,L"gif==null",L"",0);
		 }
		 delete theElement;
		 m_ElementMap_.erase(iter);
	}
}


CElement* CGifLayer::GetGifElementByHandle(int ElementHandle)
{
	//std::map<long, CElement*>::iterator theBeg = m_ElementMap_.begin();
	//std::map<long, CElement*>::iterator theEnd = m_ElementMap_.end();
	//for (; theBeg != theEnd; theBeg++)
	//{
	//	if(theBeg->first == ElementHandle)
	//	{
	//		return (theBeg->second);
	//	}
	//}
	//return NULL;
	std::map<long, CElement*>::iterator iter = m_ElementMap_.find(ElementHandle);
	if(iter != m_ElementMap_.end())
	{
		CElement* theElement = iter->second;
		return theElement;
	}
	return NULL;
}

bool CGifLayer::isEmpty()
{
	return m_ElementMap_.empty();
}