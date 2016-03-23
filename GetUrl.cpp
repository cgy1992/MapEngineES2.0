#include "GetUrl.h"
#ifdef WIN32
#undef PostMessage
#endif
GetUrl* GetUrl::Create(pp::Instance* instance, const std::string& url) 
{
	return new GetUrl(instance, url);
}
void GetUrl::Start()
{
	if(m_buffer_ == NULL)
		exit(0);
	pp::CompletionCallback theCbFun = cc_factory_.NewCallback(&GetUrl::OnOpen);
	//打开
	m_UrlLoad_.Open(m_UrlRequestInfo_, theCbFun);
}
void GetUrl::OnOpen(int32_t result)
{
	if (result == PP_OK)
	{
		//确定接受的字节数目
		int64_t bytes_received = 0;
		int64_t total_bytes_to_be_received = 0;
		//查询下载进度
		if (m_UrlLoad_.GetDownloadProgress(&bytes_received, &total_bytes_to_be_received)) 
		{
			if (total_bytes_to_be_received > 0) 
			{
				url_response_body_.reserve(total_bytes_to_be_received);
			}
		}
		//不需要查询下载进度
		m_UrlRequestInfo_.SetRecordDownloadProgress(false);
		ReadBody();

	}
	else
	{
		ReportResult(m_url_, "get file", false);
	}
}
void GetUrl::ReportResult(const std::string& fname, const std::string& text, bool success) 
{
	if (m_pInstance_) 
	{
		if (success)
		{
			//成功
			pp::Var var_result(fname + "\t true" + text);
			m_pInstance_->PostMessage(var_result);
		} 
		else
		{
			//失败
			pp::Var var_result(fname + "\t false" + text);
			m_pInstance_->PostMessage(var_result);
			delete this;
		}
	}
}

void GetUrl::ReadBody()
{
	//读取的回调函数定义
	pp::CompletionCallback cc = cc_factory_.NewCallback(&GetUrl::OnRead);
	int32_t result = PP_OK;
	do
	{
		//读取一部分
		result = m_UrlLoad_.ReadResponseBody(m_buffer_, READ_BUFFER_SIZE, cc);
		if (result > 0)
		{
			//保存内容
			AppendDataBytes(m_buffer_, result);
		}
	}while(result > 0);
}

void GetUrl::AppendDataBytes(const char* buffer, int32_t num_bytes)
{
	if (num_bytes < 0)
	{
	} 
	else
	{
		num_bytes = num_bytes;
		std::string theBuffer(buffer, num_bytes);
		url_response_body_ += theBuffer;
	}
}

void GetUrl::OnRead(int32_t result)
{
	if (result == PP_OK)
	{
		delete[] m_buffer_;
		m_buffer_ = NULL;
		ReportResult(m_url_, url_response_body_, true);
	} 
	else if(result > 0)
	{
		AppendDataBytes(m_buffer_, result);
		ReadBody();
	}
}