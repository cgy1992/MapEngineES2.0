
// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once
#define _AFXDLL
#ifndef MapEngineES
#define MapEngineES 1


#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif

//#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

// �ر� MFC ��ĳЩ�����������ɷ��ĺ��Եľ�����Ϣ������
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ


#include <afxdisp.h>        // MFC �Զ�����



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // �������Ϳؼ����� MFC ֧��

//#import "c:\Program Files (x86)\common Files\system\ado\msado15.dll"  no_namespace rename("EOF", "adoEOF")
#include <fstream>
//#include "mysql.h"
#include <Mmsystem.h>
#pragma comment(lib, "Winmm.lib")


#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif





//ʹ��GDI+
#include <GdiPlus.h>
#pragma comment(lib,"gdiplus.lib")
using namespace Gdiplus;
GdiplusStartupInput m_gdiplusStartupInput;			// GDI+
ULONG_PTR m_gdiplusToken;							// GDI+
#include <list>
#include <string>
#include<vector>
#include <map>
using namespace  std;

#define PATH_MAX 512

#define GL_GLEXT_PROTOTYPES

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

//void CStringToString(CString theCString, std::string& theString, UINT theCode = CP_ACP)
//{
//	int Length = WideCharToMultiByte(theCode, 0, theCString, -1, NULL, 0, NULL, 0);
//	char* theChar = new char[Length + 1];
//	WideCharToMultiByte(theCode, 0, theCString, -1, theChar, Length, NULL, 0);
//	theChar[Length] = 0;
//	std::string theReturn(theChar);
//	theString = theReturn;
//}
//
//void StringToCString(std::string theString, CString& theCString, UINT theCode = CP_ACP)
//{
//	int len = MultiByteToWideChar(theCode, 0, theString.c_str(), -1, NULL, 0);
//	WCHAR* pCxml = new WCHAR[len + 1];
//	len = MultiByteToWideChar(theCode, 0, theString.c_str(), theString.length(), pCxml, len);
//	pCxml[len] = 0;
//	CString tString(pCxml);
//	theCString = tString;
//	delete[] pCxml;
//}
//const long g_Zlocation = 1;

#endif