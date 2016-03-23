#pragma once
#include "Stdafx.h"
#include "stdafxcpp.h"
#include "stdafxwin32.h"
#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/module.h"
#include "ppapi/cpp/var.h"
#include "ppapi/cpp/url_loader.h"
#include "ppapi/cpp/rect.h"
#include "ppapi/cpp/input_event.h"
#include "ppapi/cpp/url_request_info.h"
#include "ppapi/utility/completion_callback_factory.h"
#include "ppapi/cpp/completion_callback.h"
#include "ppapi/cpp/audio.h"
#include "ppapi/cpp/graphics_3d.h"
#include "ppapi/cpp/graphics_3d_client.h"
#include "ppapi/gles2/gl2ext_ppapi.h"
#include "ppapi/cpp/point.h"

//c version的头文件
#include "ppapi/c/pp_stdint.h"
#include "ppapi/c/pp_completion_callback.h"
#include "ppapi/c/pp_errors.h"
#include "ppapi/c/pp_module.h"
#include "ppapi/c/pp_var.h"
#include "ppapi/c/ppb.h"
#include "ppapi/c/ppb_core.h"
#include "ppapi/c/ppb_graphics_3d.h"
#include "ppapi/c/ppb_instance.h"
#include "ppapi/c/ppb_messaging.h"
#include "ppapi/c/ppb_opengles2.h"
#include "ppapi/c/ppb_var.h"
#include "ppapi/c/ppp.h"
#include "ppapi/c/ppp_instance.h"
#include "ppapi/c/ppp_messaging.h"
#include "ppapi/c/ppb_url_loader.h"
#include "ppapi/c/ppb_url_request_info.h"
#include "ppapi/c/ppp_graphics_3d.h"
#include "ppapi/c/ppb_graphics_2d.h"
#include "ppapi/c/ppb_input_event.h"
#include "ppapi/c/ppp_input_event.h"
#include "ppapi/c/ppb_fullscreen.h"
#include "ppapi/c/ppb_view.h"
#include "ppapi/c/ppb_mouse_cursor.h"
#include "ppapi/c/ppb_image_data.h"

void CStringToString(CString theCString, std::string& theString, UINT theCode = CP_ACP)
{
	int Length = WideCharToMultiByte(theCode, 0, theCString, -1, NULL, 0, NULL, 0);
	char* theChar = new char[Length + 1];
	WideCharToMultiByte(theCode, 0, theCString, -1, theChar, Length, NULL, 0);
	theChar[Length] = 0;
	std::string theReturn(theChar);
	theString = theReturn;
}

void StringToCString(std::string theString, CString& theCString, UINT theCode = CP_ACP)
{
	int len = MultiByteToWideChar(theCode, 0, theString.c_str(), -1, NULL, 0);
	WCHAR* pCxml = new WCHAR[len + 1];
	len = MultiByteToWideChar(theCode, 0, theString.c_str(), theString.length(), pCxml, len);
	pCxml[len] = 0;
	CString tString(pCxml);
	theCString = tString;
	delete[] pCxml;
}

//获取CPU的数量
unsigned GetCpuCoreCount()  
{  
	unsigned count = 1; // 至少一个  
	SYSTEM_INFO si;  
	GetSystemInfo(&si);  
	count = si.dwNumberOfProcessors; 
	return count;  
}