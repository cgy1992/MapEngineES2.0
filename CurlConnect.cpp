#include "CurlConnect.h"

CurlConnect* CurlConnect::m_instance_ = NULL;

CurlConnect* CurlConnect::GetInstance()
{
	static CurlLock theLock;
	//theLock.Lock(); //这种方式 效率不高
	if(m_instance_ == NULL) //需要创建时，才加锁
	{
		theLock.Lock();
		if(m_instance_ == NULL)
			m_instance_ = new CurlConnect();
		theLock.UnLock();
	}
	//theLock.UnLock();
	return m_instance_;
}

bool CurlConnect::GetDataFromByHttp(const std::string& theURL, std::string& theText)
{
	CURL* theCurl;
	CURLcode theReturn;
	theText = "";
	theCurl = curl_easy_init();
	theReturn = curl_easy_setopt(theCurl, CURLOPT_URL, theURL.c_str());
	theReturn = curl_easy_setopt(theCurl, CURLOPT_NOSIGNAL, 1); 
	theReturn = curl_easy_setopt(theCurl, CURLOPT_LOW_SPEED_LIMIT, 1L);
	theReturn = curl_easy_setopt(theCurl, CURLOPT_LOW_SPEED_TIME, 3L);
	theReturn = curl_easy_setopt(theCurl, CURLOPT_WRITEFUNCTION, curl_writer);
	theReturn = curl_easy_setopt(theCurl, CURLOPT_WRITEDATA, &theText);
	theReturn = curl_easy_perform(theCurl);
	curl_easy_cleanup(theCurl);
	if(CURLE_OK == theReturn)
		return true;
	else 
		return false;
}

CURL* CurlConnect::curl_easy_handler(const std::string & sUrl, std::string & sRsp, unsigned int& uiTimeout)
{
	CURL * curl = curl_easy_init();
	sRsp = "";
	curl_easy_setopt(curl, CURLOPT_URL, sUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	// write function //
	//curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, 1L);
	//curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, uiTimeout);
	//curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, uiTimeout);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curl_writer);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &sRsp);
	return curl;
}

void CurlConnect::WateForMulitCurl(CURLM* curl_m)
{
	//等待数据的填充
	/*
	* 调用curl_multi_perform函数执行curl请求
	* url_multi_perform返回CURLM_CALL_MULTI_PERFORM时，表示需要继续调用该函数直到返回值不是CURLM_CALL_MULTI_PERFORM为止
	* running_handles变量返回正在处理的easy curl数量，running_handles为0表示当前没有正在执行的curl请求
	*/
	int running_handles;
	while (CURLM_CALL_MULTI_PERFORM == curl_multi_perform(curl_m, &running_handles))
	{
		//cout << running_handles << endl;
	}

	/**
	* 为了避免循环调用curl_multi_perform产生的cpu持续占用的问题，采用select来监听文件描述符
	*/
	while (running_handles)
	{
		if (-1 == curl_multi_select(curl_m))
		{
			//cerr << "select error" << endl;
			break;
		} 
		else 
		{
			// select监听到事件，调用curl_multi_perform通知curl执行相应的操作 //
			while (CURLM_CALL_MULTI_PERFORM == curl_multi_perform(curl_m, &running_handles))
			{
				//cout << "select: " << running_handles << endl;
			}
		}
		//cout << "select: " << running_handles << endl;
	}
}
int CurlConnect::curl_multi_select(CURLM* curl_m)
{
	int ret = 0;

	struct timeval timeout_tv;
	fd_set  fd_read;
	fd_set  fd_write;
	fd_set  fd_except;
	int     max_fd = -1;

	// 注意这里一定要清空fdset,curl_multi_fdset不会执行fdset的清空操作  //
	FD_ZERO(&fd_read);
	FD_ZERO(&fd_write);
	FD_ZERO(&fd_except);

	// 设置select超时时间  //
	timeout_tv.tv_sec = 1;
	timeout_tv.tv_usec = 0;

	// 获取multi curl需要监听的文件描述符集合 fd_set //
	curl_multi_fdset(curl_m, &fd_read, &fd_write, &fd_except, &max_fd);

	/**
	* When max_fd returns with -1,
	* you need to wait a while and then proceed and call curl_multi_perform anyway.
	* How long to wait? I would suggest 100 milliseconds at least,
	* but you may want to test it out in your own particular conditions to find a suitable value.
	*/
	if (-1 == max_fd)
	{
		return -1;
	}

	/**
	* 执行监听，当文件描述符状态发生改变的时候返回
	* 返回0，程序调用curl_multi_perform通知curl执行相应操作
	* 返回-1，表示select错误
	* 注意：即使select超时也需要返回0，具体可以去官网看文档说明
	*/
	int ret_code = ::select(max_fd + 1, &fd_read, &fd_write, &fd_except, &timeout_tv);
	switch(ret_code)
	{
	case -1:
		/* select error */
		ret = -1;
		break;
	case 0:
		/* select timeout */
	default:
		/* one or more of curl's file descriptors say there's data to read or write*/
		ret = 0;
		break;
	}

	return ret;
}
