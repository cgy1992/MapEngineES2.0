#pragma once
#include "../tinyxml\tinyxml.h"
class CXmlWork
{
public:
	CXmlWork(void);
	~CXmlWork(void);

	bool GetXmlDeclare(std::string XmlFile,std::string &strVersion,std::string &strStandalone,std::string &strEncoding);
	bool GetNodePointerByName(TiXmlElement* pRootEle,std::string &strNodeName,TiXmlElement* &Node); 

private:

};

