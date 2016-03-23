#ifndef _LOGFILE_
#define _LOGFILE_

#include "stdafx.h"
#include <assert.h>
#include <time.h>
#include <stdio.h>
//#define _UNICODE // 该宏将LPCTSTR 等同于 const char *  必须放置在Windows.h之前
#include <windows.h>
#include<tchar.h>
/*
不能从const char *转换为LPCWSTR?
解决方法：
项目菜单——项目属性(最后一个)——配置属性——常规——项目默认值——字符集，将使用Unicode字符集改为未设置即可。
*/
class LogFile
{
protected: //供内部和子类使用
	
//依据[文件名称]来打开文件， 指针到文件尾
	bool OpenFile()
	{
		if(IsOpen())
			return true;
		if(!_szFileName)
			return false;
		_hFile =  CreateFile(
			_szFileName, 
			GENERIC_WRITE,  //写
			FILE_SHARE_READ | FILE_SHARE_WRITE, //允许并发读写，及可以在该文件上打开第二个HANDLE
			NULL,
			OPEN_EXISTING, //打开现有文件，如果文件不存在则失败
			FILE_ATTRIBUTE_NORMAL,
			NULL 
			);
		if(!IsOpen() && GetLastError() == 2)//打开不成功， 且因为文件不存在， 创建文件
			_hFile =  CreateFile(
			_szFileName, 
			GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_ALWAYS,  //打开文件，如果文件不存在则创建它
			FILE_ATTRIBUTE_NORMAL,
			NULL 
			); 
		if(IsOpen())
			SetFilePointer(_hFile, 0, NULL, FILE_END); //将文件指针置到尾端
		return IsOpen();
	}
	//这个接口未使用到 
	DWORD Write(LPCVOID lpBuffer, DWORD dwLength)
	{
		DWORD dwWriteLength = 0;
		if(IsOpen())
			WriteFile(_hFile, lpBuffer, dwLength, &dwWriteLength, NULL);
		return dwWriteLength;
	}
	//写文件
	virtual void WriteLog( LPCVOID lpBuffer, DWORD dwLength)//写日志, 可以扩展修改
	{
		time_t now; // 秒 的总数目
		char temp[21]; //时间戳
		DWORD dwWriteLength;
		if(IsOpen())
		{	
			if(! append_flag_) //不是添加
			{
				time(&now);
				strftime(temp, 20, "%Y-%m-%d %H:%M:%S", localtime(&now)); //格式化时间字符串
				WriteFile(_hFile, "#-----------------------------", 26, &dwWriteLength, NULL);
				WriteFile(_hFile, temp, 19, &dwWriteLength, NULL); //时间戳
				WriteFile(_hFile, "-------------------------#\r\n", 28, &dwWriteLength, NULL);
			}
			//需要写入的日志内容
			WriteFile(_hFile, lpBuffer, dwLength, &dwWriteLength, NULL);
			WriteFile(_hFile, "\r\n", 2, &dwWriteLength, NULL); //换行

			FlushFileBuffers(_hFile); //冲刷到磁盘文件
		}
	}
	void Lock()  { ::EnterCriticalSection(&_csLock); }
	void Unlock() { ::LeaveCriticalSection(&_csLock); }
protected: //modify by xuyan 从public接口中移送至 protected中
	bool IsOpen()
	{
		return _hFile != INVALID_HANDLE_VALUE;
	}
	void Log(LPCVOID lpBuffer, DWORD dwLength)//追加日志内容
	{
		assert(lpBuffer);
		__try
		{
			Lock();
			if(!OpenFile())
				return;
			WriteLog(lpBuffer, dwLength);
		}
		__finally
		{
			Unlock();
		} 
	}
public: //构造函数
	LogFile(const TCHAR *szFileName = _T("NINGBO.log")):noisy(false),append_flag_(false)
	//设定日志文件名，构造函数，默认是不输出信息的
	{
		_szFileName = NULL;
		_hFile = INVALID_HANDLE_VALUE;
		::InitializeCriticalSection(&_csLock); //初始化临界区
		SetFileName(szFileName);//修改文件名(相当于重新生成一个新的日志文件)， 同时关闭上一个日志文件
	}
	virtual ~LogFile()
	{
		::DeleteCriticalSection(&_csLock); //释放临界区
		Close();
		if(_szFileName)
			delete []_szFileName;
	}

	const TCHAR * GetFileName()
	{
		return _szFileName;
	}
	//修改文件名(相当于重新生成一个新的日志文件)， 同时关闭上一个日志文件
	void SetFileName(const TCHAR *szName)
	{
		assert(szName);
		if(_szFileName)//如果持有pre fileName
			delete []_szFileName;
		Close(); //关闭pre Handle，并将句柄置为 invalid
		_szFileName = new TCHAR[_tcslen(szName) + 1];
		assert(_szFileName);
		_tcscpy(_szFileName, szName);
	}
	void on(){ noisy = true;}
	void off(){noisy = false;}
	//在日志中添加一条 新的记录
	void Log(const TCHAR *szText)
	{
	   if(!noisy)
		   return;
	   append_flag_ = false;
	   	CString cstStr(szText); //使用CString封装
		if(sizeof(char) != sizeof(TCHAR)) //在unicode编码的情况下
		{
			//Unicode 下 将 CString --> char *  //因为[文件系统的API接口 参数是  char*]
			int len =WideCharToMultiByte(CP_ACP,0,cstStr,-1,NULL,0,NULL,NULL);  
			char *ptxtTemp =new char[len +1];  
			WideCharToMultiByte(CP_ACP,0,cstStr,-1,ptxtTemp,len,NULL,NULL ); 
			Log(ptxtTemp,strlen(ptxtTemp)); //LPCVOID
			delete[] ptxtTemp;
		}
		else
		{
			char *content = (LPSTR)(LPCTSTR)cstStr;
			Log(content,strlen(content));
		}	
	}
	void append(const TCHAR *szText)
	{
		append_flag_ = true;
		if(!noisy)
			return;
		CString cstStr(szText); //使用CString封装
		if(sizeof(char) != sizeof(TCHAR))
		{
			//Unicode 下 将 CString --> char *  //因为[文件系统的API接口 参数是  char*]
			int len =WideCharToMultiByte(CP_ACP,0,cstStr,-1,NULL,0,NULL,NULL);  
			char *ptxtTemp =new char[len +1];  
			WideCharToMultiByte(CP_ACP,0,cstStr,-1,ptxtTemp,len,NULL,NULL ); 
			Log(ptxtTemp,strlen(ptxtTemp)); //LPCVOID
			delete[] ptxtTemp;
		}
		else //直接使用 强制类型转换
		{
			char *content = (LPSTR)(LPCTSTR)cstStr;
			Log(content,strlen(content));
		}	
	}
	//手动关闭打开的文件
	void Close()
	{
		if(IsOpen())
		{
			CloseHandle(_hFile);
			_hFile = INVALID_HANDLE_VALUE;
		}
	}
//增设一个方法，每间隔5s就log一次
protected:
	CRITICAL_SECTION _csLock;
	TCHAR * _szFileName; //磁盘的绝对路径
	HANDLE _hFile; //文件句柄
	bool noisy; //true表示 输出日志 初始化false
	bool append_flag_;// true不写时间戳，追加到上一条记录中， 初始化false
private://屏蔽函数，其子类不可copy
		LogFile(const LogFile&);
		LogFile&operator = (const LogFile&);
};



//class LogFileEx : public LogFile  
//{
//protected:
//	void SetPath(const char *szPath)  //
//	{
//		assert(szPath);
//		WIN32_FIND_DATA wfd; //该结构体用于保存，找到的文件有关的信息
//		char temp[MAX_PATH + 1] = {0}; //最长路径名
//		if(FindFirstFile(szPath, &wfd) == INVALID_HANDLE_VALUE && CreateDirectory(szPath, NULL) == 0)
//		{
//			strcat(strcpy(temp, szPath), " Create Fail. Exit Now! Error ID :");
//			ltoa(GetLastError(), temp + strlen(temp), 10); //错误码
//			MessageBox(NULL, temp, "Class LogFileEx", MB_OK);
//			exit(1);
//		}
//		else
//		{
//			GetFullPathName(szPath, MAX_PATH, temp, NULL); // 这里szPath使用的是相对路径  
//			_szPath = new char[strlen(temp) + 1];
//			assert(_szPath);
//			strcpy(_szPath, temp);
//		}
//	}
//public:
//	enum LOG_TYPE{YEAR = 0, MONTH = 1, DAY = 2};
//	//默认是当前目录
//	/*
//	 param1：文件的目录
//	 param2: 日志文件的生成周期
//	*/
//	LogFileEx(const char *szPath = ".", LOG_TYPE iType = MONTH)  
//	{
//		_szPath = NULL;
//		SetPath(szPath);
//		_iType = iType;
//		memset(_szLastDate, 0, 9); //默认 最后日期是空
//	}
////只负责是否本类中的新分配的资源
//	~LogFileEx()
//	{
//		if(_szPath)
//			delete []_szPath;
//	}
//	const char * GetPath()
//	{
//		return _szPath;
//	}
//	void Log(LPCVOID lpBuffer, DWORD dwLength)
//	{
//		assert(lpBuffer);
//		char temp[10];
//		static const char format[3][10] = {"%Y", "%Y-%m", "%Y%m%d"}; //打印时间的格式
//
//		__try
//		{
//			Lock();
//			//returns the number of seconds elapsed since midnight (00:00:00), January 1, 1970
//			time_t now = time(NULL); //秒
//			strftime(temp, 9, format[_iType], localtime(&now)); // localtime 返回 struct tm*
//			if(strcmp(_szLastDate, temp) != 0)//更换文件名
//			{
//				strcat(strcpy(_szFileName, _szPath), "//"); //
//				strcat(strcat(_szFileName, temp), ".log"); //链式操作,以时间作为文件名
//				strcpy(_szLastDate, temp); //保存时间，用以比较
//				Close(); //关闭已经打开的文件
//			}
//			if(!OpenFile()) //文件打开失败返回
//				return;
//
//			WriteLog(lpBuffer, dwLength);
//		}
//		__finally
//		{
//			Unlock();
//		}
//	}
//	void Log(const char *szText)
//	{
//		Log(szText, strlen(szText));
//	}
//private://屏蔽函数,禁用值语义
//	LogFileEx(const LogFileEx&);
//	LogFileEx&operator = (const LogFileEx&);
//protected:
//	char *_szPath; //支持设置存放路径
//	char _szLastDate[9];
//	//TCHAR *_szPath; //支持设置存放路径
//	//TCHAR _szLastDate[9];
//	int _iType;
//};

#endif