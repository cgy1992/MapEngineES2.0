#pragma once
#include "stdafxppapi.h"
//#include "curl/curl.h"
#include "curl.h"
#pragma comment(lib,"libcurl_imp.lib")   //指定链接的的 静态库

//CURL从网络服务拿数据的使用
size_t curl_writer(void *buffer, size_t size, size_t count, void * stream)
{
	std::string * pStream = static_cast<std::string *>(stream);
	(*pStream).append((char *)buffer, size * count);
	return size * count;
};

class CurlLock
{
private:
	CRITICAL_SECTION m_curl_cs_;  // 临界区 kernel object
public:
	CurlLock() { InitializeCriticalSection(&m_curl_cs_); }
	~CurlLock() { DeleteCriticalSection(&m_curl_cs_); }
	void Lock() { EnterCriticalSection(&m_curl_cs_); }
	void UnLock() { LeaveCriticalSection(&m_curl_cs_); }
};

class CurlConnect
{
private:
	CurlConnect()
	{
		curl_global_init(CURL_GLOBAL_ALL); 
	}

	~CurlConnect()
	{
		curl_global_cleanup();
	}


	static CurlConnect* m_instance_; //连接实例

	//class DeleteCurlConnect
	//{
	//public:
	//	DeleteCurlConnect() 
	//	{
	//	}
	//	~DeleteCurlConnect()
	//	{
	//		if(m_instance_)
	//			delete m_instance_;
	//		m_instance_ = NULL;
	//	}
	//};
	//
	//// just for delete
	//static DeleteCurlConnect m_DeleteInstance_; //该成员未初始化，不知怎么通过compile？？

public:
	static CurlConnect* GetInstance();

	//从网络获取数据，数据内容放在theText中
	bool GetDataFromByHttp(const std::string& theURL, std::string& theText);

	/**
	* 使用select函数监听multi curl文件描述符的状态
	* 监听成功返回0，监听失败返回-1
	*/
	int curl_multi_select(CURLM * curl_m);

	/**
	* 生成一个easy curl对象，进行一些简单的设置操作
	const std::string & sUrl,		//网络URL
	std::string & sRsp,			//返回的信息内容字符串
	unsigned int& uiTimeout);		//等待时间
	*/
	CURL* curl_easy_handler(const std::string & sUrl, std::string & sRsp, unsigned int& uiTimeout);

	// waite for data
	void WateForMulitCurl(CURLM* curl_m);
};