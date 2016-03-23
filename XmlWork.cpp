#include "../stdafx.h"
#include "XmlWork.h"


CXmlWork::CXmlWork(void)
{
}


CXmlWork::~CXmlWork(void)
{
}

/*!   
*  \brief 获取xml文件的声明。   
*   
*  \param XmlFile xml文件全路径。   
*  \param strVersion  Version属性值   
*  \param strStandalone Standalone属性值   
*  \param strEncoding Encoding属性值   
*  \return 是否成功。true为成功，false表示失败。   
*/   
bool CXmlWork::GetXmlDeclare(std::string XmlFile,   
                   std::string &strVersion,    
                   std::string &strStandalone,    
                   std::string &strEncoding)    
{    
    // 定义一个TiXmlDocument类指针    
    TiXmlDocument *pDoc = new TiXmlDocument();    
    if (NULL==pDoc)    
    {    
        return false;    
    }    
	pDoc->LoadFile(XmlFile.c_str()); //不知道 为什么会缺少一个版本 不能接收string 作为参数

    TiXmlNode* pXmlFirst = pDoc->FirstChild();   
    if (NULL != pXmlFirst)      
     {      
          TiXmlDeclaration* pXmlDec = pXmlFirst->ToDeclaration();      
          if (NULL != pXmlDec)      
          {      
              strVersion = pXmlDec->Version();    
              strStandalone = pXmlDec->Standalone();    
              strEncoding = pXmlDec->Encoding();    
          }    
      }    
      return true;    
} 

bool CXmlWork::GetNodePointerByName(TiXmlElement* pRootEle,std::string &strNodeName,TiXmlElement* &Node)   
{    
     // 假如等于根节点名，就退出  确保这个pRootELe是非空的
     if (strNodeName==pRootEle->Value())    
     {    
         Node = pRootEle;    
         return true;    
     }  

     TiXmlElement* pEle = pRootEle;      
     for (pEle = pRootEle->FirstChildElement(); pEle; pEle = pEle->NextSiblingElement())      
     {      
          //递归处理子节点，获取节点指针    
          if(GetNodePointerByName(pEle,strNodeName,Node))
		  {
              return true; 
		  }
     }      
     return false;    
} 