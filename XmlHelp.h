#pragma once
#include "tinyxml\tinyxml.h"
#include <string>
TiXmlElement* GetNodePointerByName(TiXmlElement* pRootEle, const std::string &strNodeName)   
{    
	// 假如等于根节点名，就退出    
	std::string theValue;
	theValue = pRootEle->Value();
	if (strNodeName.compare(theValue) == 0)     
		return pRootEle;    

	TiXmlElement* pEle = pRootEle;      
	for (pEle = pRootEle->FirstChildElement(); pEle; pEle = pEle->NextSiblingElement())      
	{      
		//递归处理子节点，获取节点指针  
		TiXmlElement* theElement = GetNodePointerByName(pEle, strNodeName);
		if (theElement)
		{
			return theElement;
		}
	} 
	return NULL;
} 