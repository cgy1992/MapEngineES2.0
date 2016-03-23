#pragma once
#include "stdafxppapi.h"
const long READ_BUFFER_SIZE = 32768;
class GetUrl
{
public:
	static GetUrl* Create(pp::Instance* instance_, const std::string& url);
	//启动
	void Start();							//启动
	void OnOpen(int32_t result);			//打开的回调函数
	void ReportResult(const std::string& fname, const std::string& text, bool success);
	void ReadBody();						//读取数据内容
	void OnRead(int32_t result);			//读取的回调函数
	void AppendDataBytes(const char* buffer, int32_t num_bytes);		//保存

private:
	GetUrl(pp::Instance* instance, const std::string& url) 
		: m_pInstance_(instance)
		, m_url_(url)
		, m_UrlRequestInfo_(instance)			//分配一个新的
		, m_UrlLoad_(instance)					//分配一个新的
		, cc_factory_(this)
	{
		pp::Var theUrl(url.c_str());
		m_UrlRequestInfo_.SetURL(theUrl);
		m_UrlRequestInfo_.SetMethod("GET");
		m_UrlRequestInfo_.SetRecordDownloadProgress(true);
		m_buffer_ = new char[READ_BUFFER_SIZE];
	}
	~GetUrl()
	{
		delete[] m_buffer_;
		m_buffer_ = NULL;
	}
	pp::Instance* m_pInstance_;				//实例指针
	std::string m_url_;						//url
	pp::URLLoader m_UrlLoad_;			
	pp::URLRequestInfo m_UrlRequestInfo_;	
	std::string url_response_body_;			
	pp::CompletionCallbackFactory<GetUrl> cc_factory_;
	char* m_buffer_;						//内容存储区
};