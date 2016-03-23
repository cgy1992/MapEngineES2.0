#include "../stdafx.h"
#include "XmlWork.h"


CXmlWork::CXmlWork(void)
{
}


CXmlWork::~CXmlWork(void)
{
}

/*!   
*  \brief ��ȡxml�ļ���������   
*   
*  \param XmlFile xml�ļ�ȫ·����   
*  \param strVersion  Version����ֵ   
*  \param strStandalone Standalone����ֵ   
*  \param strEncoding Encoding����ֵ   
*  \return �Ƿ�ɹ���trueΪ�ɹ���false��ʾʧ�ܡ�   
*/   
bool CXmlWork::GetXmlDeclare(std::string XmlFile,   
                   std::string &strVersion,    
                   std::string &strStandalone,    
                   std::string &strEncoding)    
{    
    // ����һ��TiXmlDocument��ָ��    
    TiXmlDocument *pDoc = new TiXmlDocument();    
    if (NULL==pDoc)    
    {    
        return false;    
    }    
	pDoc->LoadFile(XmlFile.c_str()); //��֪�� Ϊʲô��ȱ��һ���汾 ���ܽ���string ��Ϊ����

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
     // ������ڸ��ڵ��������˳�  ȷ�����pRootELe�Ƿǿյ�
     if (strNodeName==pRootEle->Value())    
     {    
         Node = pRootEle;    
         return true;    
     }  

     TiXmlElement* pEle = pRootEle;      
     for (pEle = pRootEle->FirstChildElement(); pEle; pEle = pEle->NextSiblingElement())      
     {      
          //�ݹ鴦���ӽڵ㣬��ȡ�ڵ�ָ��    
          if(GetNodePointerByName(pEle,strNodeName,Node))
		  {
              return true; 
		  }
     }      
     return false;    
} 