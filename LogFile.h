#ifndef _LOGFILE_
#define _LOGFILE_

#include "stdafx.h"
#include <assert.h>
#include <time.h>
#include <stdio.h>
//#define _UNICODE // �ú꽫LPCTSTR ��ͬ�� const char *  ���������Windows.h֮ǰ
#include <windows.h>
#include<tchar.h>
/*
���ܴ�const char *ת��ΪLPCWSTR?
���������
��Ŀ�˵�������Ŀ����(���һ��)�����������ԡ������桪����ĿĬ��ֵ�����ַ�������ʹ��Unicode�ַ�����Ϊδ���ü��ɡ�
*/
class LogFile
{
protected: //���ڲ�������ʹ��
	
//����[�ļ�����]�����ļ��� ָ�뵽�ļ�β
	bool OpenFile()
	{
		if(IsOpen())
			return true;
		if(!_szFileName)
			return false;
		_hFile =  CreateFile(
			_szFileName, 
			GENERIC_WRITE,  //д
			FILE_SHARE_READ | FILE_SHARE_WRITE, //��������д���������ڸ��ļ��ϴ򿪵ڶ���HANDLE
			NULL,
			OPEN_EXISTING, //�������ļ�������ļ���������ʧ��
			FILE_ATTRIBUTE_NORMAL,
			NULL 
			);
		if(!IsOpen() && GetLastError() == 2)//�򿪲��ɹ��� ����Ϊ�ļ������ڣ� �����ļ�
			_hFile =  CreateFile(
			_szFileName, 
			GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_ALWAYS,  //���ļ�������ļ��������򴴽���
			FILE_ATTRIBUTE_NORMAL,
			NULL 
			); 
		if(IsOpen())
			SetFilePointer(_hFile, 0, NULL, FILE_END); //���ļ�ָ���õ�β��
		return IsOpen();
	}
	//����ӿ�δʹ�õ� 
	DWORD Write(LPCVOID lpBuffer, DWORD dwLength)
	{
		DWORD dwWriteLength = 0;
		if(IsOpen())
			WriteFile(_hFile, lpBuffer, dwLength, &dwWriteLength, NULL);
		return dwWriteLength;
	}
	//д�ļ�
	virtual void WriteLog( LPCVOID lpBuffer, DWORD dwLength)//д��־, ������չ�޸�
	{
		time_t now; // �� ������Ŀ
		char temp[21]; //ʱ���
		DWORD dwWriteLength;
		if(IsOpen())
		{	
			if(! append_flag_) //�������
			{
				time(&now);
				strftime(temp, 20, "%Y-%m-%d %H:%M:%S", localtime(&now)); //��ʽ��ʱ���ַ���
				WriteFile(_hFile, "#-----------------------------", 26, &dwWriteLength, NULL);
				WriteFile(_hFile, temp, 19, &dwWriteLength, NULL); //ʱ���
				WriteFile(_hFile, "-------------------------#\r\n", 28, &dwWriteLength, NULL);
			}
			//��Ҫд�����־����
			WriteFile(_hFile, lpBuffer, dwLength, &dwWriteLength, NULL);
			WriteFile(_hFile, "\r\n", 2, &dwWriteLength, NULL); //����

			FlushFileBuffers(_hFile); //��ˢ�������ļ�
		}
	}
	void Lock()  { ::EnterCriticalSection(&_csLock); }
	void Unlock() { ::LeaveCriticalSection(&_csLock); }
protected: //modify by xuyan ��public�ӿ��������� protected��
	bool IsOpen()
	{
		return _hFile != INVALID_HANDLE_VALUE;
	}
	void Log(LPCVOID lpBuffer, DWORD dwLength)//׷����־����
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
public: //���캯��
	LogFile(const TCHAR *szFileName = _T("NINGBO.log")):noisy(false),append_flag_(false)
	//�趨��־�ļ��������캯����Ĭ���ǲ������Ϣ��
	{
		_szFileName = NULL;
		_hFile = INVALID_HANDLE_VALUE;
		::InitializeCriticalSection(&_csLock); //��ʼ���ٽ���
		SetFileName(szFileName);//�޸��ļ���(�൱����������һ���µ���־�ļ�)�� ͬʱ�ر���һ����־�ļ�
	}
	virtual ~LogFile()
	{
		::DeleteCriticalSection(&_csLock); //�ͷ��ٽ���
		Close();
		if(_szFileName)
			delete []_szFileName;
	}

	const TCHAR * GetFileName()
	{
		return _szFileName;
	}
	//�޸��ļ���(�൱����������һ���µ���־�ļ�)�� ͬʱ�ر���һ����־�ļ�
	void SetFileName(const TCHAR *szName)
	{
		assert(szName);
		if(_szFileName)//�������pre fileName
			delete []_szFileName;
		Close(); //�ر�pre Handle�����������Ϊ invalid
		_szFileName = new TCHAR[_tcslen(szName) + 1];
		assert(_szFileName);
		_tcscpy(_szFileName, szName);
	}
	void on(){ noisy = true;}
	void off(){noisy = false;}
	//����־�����һ�� �µļ�¼
	void Log(const TCHAR *szText)
	{
	   if(!noisy)
		   return;
	   append_flag_ = false;
	   	CString cstStr(szText); //ʹ��CString��װ
		if(sizeof(char) != sizeof(TCHAR)) //��unicode����������
		{
			//Unicode �� �� CString --> char *  //��Ϊ[�ļ�ϵͳ��API�ӿ� ������  char*]
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
		CString cstStr(szText); //ʹ��CString��װ
		if(sizeof(char) != sizeof(TCHAR))
		{
			//Unicode �� �� CString --> char *  //��Ϊ[�ļ�ϵͳ��API�ӿ� ������  char*]
			int len =WideCharToMultiByte(CP_ACP,0,cstStr,-1,NULL,0,NULL,NULL);  
			char *ptxtTemp =new char[len +1];  
			WideCharToMultiByte(CP_ACP,0,cstStr,-1,ptxtTemp,len,NULL,NULL ); 
			Log(ptxtTemp,strlen(ptxtTemp)); //LPCVOID
			delete[] ptxtTemp;
		}
		else //ֱ��ʹ�� ǿ������ת��
		{
			char *content = (LPSTR)(LPCTSTR)cstStr;
			Log(content,strlen(content));
		}	
	}
	//�ֶ��رմ򿪵��ļ�
	void Close()
	{
		if(IsOpen())
		{
			CloseHandle(_hFile);
			_hFile = INVALID_HANDLE_VALUE;
		}
	}
//����һ��������ÿ���5s��logһ��
protected:
	CRITICAL_SECTION _csLock;
	TCHAR * _szFileName; //���̵ľ���·��
	HANDLE _hFile; //�ļ����
	bool noisy; //true��ʾ �����־ ��ʼ��false
	bool append_flag_;// true��дʱ�����׷�ӵ���һ����¼�У� ��ʼ��false
private://���κ����������಻��copy
		LogFile(const LogFile&);
		LogFile&operator = (const LogFile&);
};



//class LogFileEx : public LogFile  
//{
//protected:
//	void SetPath(const char *szPath)  //
//	{
//		assert(szPath);
//		WIN32_FIND_DATA wfd; //�ýṹ�����ڱ��棬�ҵ����ļ��йص���Ϣ
//		char temp[MAX_PATH + 1] = {0}; //�·����
//		if(FindFirstFile(szPath, &wfd) == INVALID_HANDLE_VALUE && CreateDirectory(szPath, NULL) == 0)
//		{
//			strcat(strcpy(temp, szPath), " Create Fail. Exit Now! Error ID :");
//			ltoa(GetLastError(), temp + strlen(temp), 10); //������
//			MessageBox(NULL, temp, "Class LogFileEx", MB_OK);
//			exit(1);
//		}
//		else
//		{
//			GetFullPathName(szPath, MAX_PATH, temp, NULL); // ����szPathʹ�õ������·��  
//			_szPath = new char[strlen(temp) + 1];
//			assert(_szPath);
//			strcpy(_szPath, temp);
//		}
//	}
//public:
//	enum LOG_TYPE{YEAR = 0, MONTH = 1, DAY = 2};
//	//Ĭ���ǵ�ǰĿ¼
//	/*
//	 param1���ļ���Ŀ¼
//	 param2: ��־�ļ�����������
//	*/
//	LogFileEx(const char *szPath = ".", LOG_TYPE iType = MONTH)  
//	{
//		_szPath = NULL;
//		SetPath(szPath);
//		_iType = iType;
//		memset(_szLastDate, 0, 9); //Ĭ�� ��������ǿ�
//	}
////ֻ�����Ƿ����е��·������Դ
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
//		static const char format[3][10] = {"%Y", "%Y-%m", "%Y%m%d"}; //��ӡʱ��ĸ�ʽ
//
//		__try
//		{
//			Lock();
//			//returns the number of seconds elapsed since midnight (00:00:00), January 1, 1970
//			time_t now = time(NULL); //��
//			strftime(temp, 9, format[_iType], localtime(&now)); // localtime ���� struct tm*
//			if(strcmp(_szLastDate, temp) != 0)//�����ļ���
//			{
//				strcat(strcpy(_szFileName, _szPath), "//"); //
//				strcat(strcat(_szFileName, temp), ".log"); //��ʽ����,��ʱ����Ϊ�ļ���
//				strcpy(_szLastDate, temp); //����ʱ�䣬���ԱȽ�
//				Close(); //�ر��Ѿ��򿪵��ļ�
//			}
//			if(!OpenFile()) //�ļ���ʧ�ܷ���
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
//private://���κ���,����ֵ����
//	LogFileEx(const LogFileEx&);
//	LogFileEx&operator = (const LogFileEx&);
//protected:
//	char *_szPath; //֧�����ô��·��
//	char _szLastDate[9];
//	//TCHAR *_szPath; //֧�����ô��·��
//	//TCHAR _szLastDate[9];
//	int _iType;
//};

#endif