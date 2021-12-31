#pragma once

#include "curl/curl.h"
#include "SetOperation.h"
#include "ParamDefine.h"
#include <list>
#include <string>
#include <functional>

//multipart/form-data详细介绍 http://blog.csdn.net/yankai0219/article/details/8159701
//C++ curl库 源码编译及使用（VS2019）https://blog.csdn.net/xray2/article/details/120496410

namespace Curl {

	class Client;
	class Easy;

	typedef std::function<size_t(Easy* easy, void* buffer, size_t size, size_t nmemb)> OnBuffer;
	typedef std::function<void(Easy* easy, double ltotal, double lnow)> OnProgress;
	
	class Easy : public Operation::CSetOperation {
		friend class Client;
		enum { EUpload, EDownload };
	public:
		Easy();
		~Easy();
	protected:
		int buildGet(
			char const* url,
			std::list<std::string> const* headers,
			char const* spath = NULL,
			bool dump = true, FILE *fd = stderr);
		int buildPost(
			char const* url,
			std::list<std::string> const* headers,
			char const* spost,
			char const* spath = NULL,
			bool dump = true, FILE *fd = stderr);
		int buildUpload(
			char const* url,
			std::list<FMParam> const* params,
			OnProgress onProgress,
			char const* spath = NULL,
			bool dump = true, FILE *fd = stderr);
		int buildDownload(
			char const* url,
			OnBuffer onBuffer,
			OnProgress onProgress,
			char const* spath = NULL,
			bool dump = true, FILE *fd = stderr);
		int perform();

	protected:
		int check(char const* url, double & size);
		int setTimeout();
		int setDebug(bool dump = true, FILE *fd = stderr);
		int setUrl(char const* url);
		int addHeader(std::list<std::string> const* headers);
		int addPost(std::list<FMParam> const* params, char const* spost);
		int setProxy(char const* sproxy = NULL, char const* sagent = NULL);
		int setSSLCA(char const* spath = NULL);
		int setCallback(void *readcbk = NULL, void *writecbk = NULL, void *progresscbk = NULL);
	protected:
		CURL *curl_;
		curl_httppost *formpost_;
		curl_httppost *lastptr_;
		curl_slist *headerlist_;
		unsigned long lasttime_;
		OnProgress progress_cb_;
		OnBuffer buffer_cb_;
		int mode_;
		bool finished_;
	private:
		struct debug_data_t{
			bool dump_flag_;
			FILE *fd;
		};
		debug_data_t debug_data_;
	protected:
		size_t readCallback(void *buffer, size_t size, size_t nmemb);
		size_t writeCallback(void *buffer, size_t size, size_t nmemb);
		int progressCallback(double dltotal, double dlnow, double ultotal, double ulnow);
	private:
		static int debugCallback_(CURL *curl, curl_infotype type, char *data, size_t size, void *userp);
		static void dump_(const char *text, FILE *stream, unsigned char *ptr, size_t size);
		static size_t readCallback_(void *buffer, size_t size, size_t nmemb, void *stream);
		static size_t writeCallback_(void *buffer, size_t size, size_t nmemb, void *stream);
		static int progressCallback_(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow);
	private:
		bool formAdd(CURL *curl, FMParam const& param);
		bool formAdd(CURL *curl, std::list<FMParam> const& params);
		void purge();
	};
}