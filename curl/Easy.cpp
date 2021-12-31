#include "../utils.h"
#include "Easy.h"
#include <assert.h>
#include <string.h>
#include "Client.h"

namespace Curl {

	void Easy::dump_(const char *text, FILE *stream, unsigned char *ptr, size_t size) {
		size_t i;
		size_t c;
		unsigned int width = 0x10;

		fprintf(stream, "%s, %10.10ld bytes (0x%8.8lx)\n",
			text, (long)size, (long)size);

		for (i = 0; i < size; i += width) {
			fprintf(stream, "%4.4lx: ", (long)i);

			/* show hex to the left */
			for (c = 0; c < width; c++) {
				if (i + c < size)
					fprintf(stream, "%02x ", ptr[i + c]);
				else
					fputs("   ", stream);
			}

			/* show data on the right */
			for (c = 0; (c < width) && (i + c < size); c++) {
				char x = (ptr[i + c] >= 0x20 && ptr[i + c] < 0x80) ? ptr[i + c] : '.';
				fputc(x, stream);
			}

			fputc('\n', stream); /* newline */
		}
	}

	int Easy::debugCallback_(CURL *curl, curl_infotype type, char *data, size_t size, void *userp) {
		FILE * fd = (FILE *)((Easy::debug_data_t *)userp)->fd;
		bool & debug_flag = ((Easy::debug_data_t *)userp)->dump_flag_;
		
		fd = fd ? fd : stderr;

		const char *text;
		(void)curl; /* prevent compiler warning */
		switch (type) {
		case CURLINFO_TEXT:
			fprintf(fd, "== Info: %s", data);
		default: /* in case a new one is introduced to shock us */
			return 0;
		// 请求头
		case CURLINFO_HEADER_OUT:
			text = "=> Send header";
			break;
		// 请求体
		case CURLINFO_DATA_OUT:
			text = "=> Send data";
			if (false == debug_flag) {
				dump_(text, fd, (unsigned char *)data, size);
				debug_flag = true;
			}
			return 0;
			break;
		case CURLINFO_SSL_DATA_OUT:
			text = "=> Send SSL data";
			break;
		// 响应头
		case CURLINFO_HEADER_IN:
			text = "<= Recv header";
			break;
		// 响应体
		case CURLINFO_DATA_IN:
			text = "<= Recv data";
			break;
		case CURLINFO_SSL_DATA_IN:
			text = "<= Recv SSL data";
			break;
		}
		dump_(text, fd, (unsigned char *)data, size);
		return 0;
	}

	size_t Easy::readCallback_(void *buffer, size_t size, size_t nmemb, void *stream) {
#if 0
		return stream ? 
			fread(buffer, size, nmemb, (FILE *)stream) : 0;
#else
		return stream ? 
			((Operation::IOperation *)stream)->MFRead(buffer, size, nmemb) : 0;
#endif
	}

	size_t Easy::writeCallback_(void *buffer, size_t size, size_t nmemb, void *stream) {
		return stream ?
			((Easy *)stream)->writeCallback(buffer, size, nmemb) : 0;
	}

	int Easy::progressCallback_(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow, void* args) {
		return clientp ?
			((Easy *)clientp)->progressCallback(dltotal, dlnow, ultotal, ulnow, args) : 0;
	}

	size_t Easy::readCallback(void *buffer, size_t size, size_t nmemb) {
		return size * nmemb;
	}
	
	size_t Easy::writeCallback(void *buffer, size_t size, size_t nmemb) {
		// CURLINFO_CONTENT_LENGTH_DOWNLOAD
		// CURLINFO_CONTENT_LENGTH_UPLOAD
		// CURLINFO_FILETIME
		// CURLINFO_CONTENT_TYPE
		// CURLINFO_TOTAL_TIME
		// CURLINFO_SPEED_DOWNLOAD
		// CURLINFO_SIZE_DOWNLOAD
		int retcode = 0;
		CURLcode easycode = curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &retcode);
		if (CURLE_OK != easycode || retcode != 200) {
			return 0;
		}

		// 上传模式, 则写内存
		// 下载模式, 则写文件
		return Write(buffer, size, nmemb);
	}

	int Easy::progressCallback(double dltotal, double dlnow, double ultotal, double ulnow, void* args) {
		if (this->progresscbk_) {

			const unsigned int elapseTime = 500;
			unsigned long tickcount_ = utils::now_ms();
			
			if ((EUpload == this->mode_ && 0 != ultotal) && (ultotal == ulnow || (tickcount_ - this->lasttime_ >= elapseTime))) {

				this->lasttime_ = tickcount_;
				
				if (!this->finished_)
					this->progresscbk_(this, ultotal, ulnow, args);

				if (ultotal == ulnow)
					this->finished_ = true;
			}
			else if ((EDownload == this->mode_ && 0 != dltotal) && (dltotal == dlnow || (tickcount_ - this->lasttime_ >= elapseTime))) {
				
				this->lasttime_ = tickcount_;
				
				if (!this->finished_)
					this->progresscbk_(this, dltotal, dlnow, args);

				if (dltotal == dlnow)
					this->finished_ = true;
			}
		}
		
		return 0;
	}

	Easy::Easy() {
		headerlist_ = NULL;
		formpost_ = NULL;
		lastptr_ = NULL;
		curl_ = NULL;
		lasttime_ = 0;
		progresscbk_ = NULL;
		mode_ = EDownload;
		finished_ = false;
		memset(&debug_data_, 0, sizeof(debug_data_));
		//::curl_global_init(CURL_GLOBAL_DEFAULT);
	}

	int Easy::buildGet(
		char const* url,
		std::list<std::string> const* headers,
		char const* spath,
		bool dump, FILE *fd) {
#if 0
		purge();
#endif
		int rc = -1;
		do {
			if (!curl_) {
				curl_ = ::curl_easy_init();
				CHECKPTR_BREAK(curl_);
			}
			if (0 != setDebug(dump, fd))
				break;
			if (0 != setUrl(url))
				break;
			if (0 != addHeader(headers))
				break;
			if (0 != setCallback(/*NULL*/(void *)readCallback_, (void *)writeCallback_, /*NULL*/(void *)progressCallback_))
				break;
			if (0 != setTimeout())
				break;
			if (0 != setSSLCA(spath))
				break;
#if 0
			if (0 != perform())
				break;
			if (NULL != resp) {
				GetBuffer(*resp);
			}
#endif
			rc = 0;
		} while (0);
#if 0
		purge();
#endif
		return rc;
	}

	int Easy::buildPost(
		char const* url,
		std::list<std::string> const* headers,
		char const* spost,
		char const* spath,
		bool dump, FILE *fd) {
#if 0
		purge();
#endif
		int rc = -1;
		do {
			if (!curl_) {
				curl_ = ::curl_easy_init();
				CHECKPTR_BREAK(curl_);
			}
			if (0 != setDebug(dump, fd))
				break;
			if (0 != setUrl(url))
				break;
			if (0 != addHeader(headers))
				break;
			if (0 != addPost(NULL, spost))
				break;
			if (0 != setCallback(/*NULL*/(void *)readCallback_, (void *)writeCallback_, /*NULL*/(void *)progressCallback_))
				break;
			if (0 != setTimeout())
				break;
			if (0 != setProxy(NULL, NULL))
				break;
			if (0 != setSSLCA(spath))
				break;
#if 0
			if (0 != perform())
				break;
			if (NULL != resp) {
				GetBuffer(*resp);
			}
#endif
			rc = 0;
		} while (0);
#if 0
		purge();
#endif
		return rc;
	}

	int Easy::buildUpload(
		char const* url,
		std::list<FMParam> const* params,
		Functor callback,
		char const* spath,
		bool dump, FILE *fd) {
#if 0
		purge();
#endif
		int rc = -1;
		do {
			if (!curl_) {
				curl_ = ::curl_easy_init();
				CHECKPTR_BREAK(curl_);
			}
			if (0 != setDebug(dump, fd))
				break;
			if (0 != setUrl(url))
				break;
			if (0 != addHeader(NULL))
				break;
			if (0 != addPost(params, NULL))
				break;
			if (0 != setCallback(/*NULL*/(void *)readCallback_, (void *)writeCallback_, /*NULL*/(void *)progressCallback_))
				break;
			if (0 != setTimeout())
				break;
			if (0 != setProxy(NULL, NULL))
				break;
			if (0 != setSSLCA(spath))
				break;
#if 0
			if (0 != perform())
				break;
			if (NULL != resp) {
				GetBuffer(*resp);
			}
#endif
			this->finished_ = false;
			this->mode_ = EUpload;
			this->progresscbk_ = callback;
			rc = 0;
		} while (0);
#if 0
		purge();
#endif
		return rc;
	}

	int Easy::buildDownload(char const* url,
		Functor callback,
		char const* spath,
		bool dump, FILE *fd) {
		this->finished_ = false;
		this->mode_ = EDownload;
		this->progresscbk_ = callback;
		return this->buildGet(url, NULL, spath, dump, fd);
	}

	int Easy::addHeader(std::list<std::string> const* headers) {
		int rc = -1;
		do {
			CURLcode easycode;
			// header
			if (headerlist_) {
				::curl_slist_free_all(headerlist_);
				headerlist_ = NULL; /* init to NULL is important */
			}
			if (headers) {
				for (std::list<std::string>::const_iterator it = headers->begin();
					it != headers->end();
					++it) {
					headerlist_ = ::curl_slist_append(headerlist_, it->c_str());
				}
			}
			/* Disable "Expect: 100-continue" */
			headerlist_ = ::curl_slist_append(headerlist_, "Expect:");
			easycode = curl_easy_setopt(curl_, CURLOPT_HTTPHEADER, headerlist_);
			CHECKCURLE_BREAK(easycode);
			rc = 0;
		} while (0);
		return rc;
	}

	int Easy::setTimeout() {
		int rc = -1;
		do {
//			CURLcode easycode;
//			// timeout
//			easycode = curl_easy_setopt(curl_, CURLOPT_NOSIGNAL, 1L);
//			CHECKCURLE_BREAK(easycode);
//			easycode = curl_easy_setopt(curl_, CURLOPT_CONNECTTIMEOUT, 3);
//			CHECKCURLE_BREAK(easycode);
//			easycode = curl_easy_setopt(curl_, CURLOPT_TIMEOUT, 3); // recv timeout
//			easycode = curl_easy_setopt(curl_, CURLOPT_TIMEOUT_MS, 30);
//			CHECKCURLE_BREAK(easycode);
			rc = 0;
		} while (0);
		return rc;
	}

	int Easy::setUrl(char const* url) {
		int rc = -1;
		do {
			CURLcode easycode;
			// url
			easycode = curl_easy_setopt(curl_, CURLOPT_URL, url);
			CHECKCURLE_BREAK(easycode);
			rc = 0;
		} while (0);
		return rc;
	}

	int Easy::setDebug(bool dump, FILE *fd) {
		memset(&debug_data_, 0, sizeof(debug_data_));
		debug_data_.fd = fd;
		debug_data_.dump_flag_ = dump;
		int rc = -1;
		do {
			// debug
			if (dump) {
				CURLcode easycode;
				easycode = curl_easy_setopt(curl_, CURLOPT_VERBOSE, 1L);
				CHECKCURLE_BREAK(easycode);
				easycode = curl_easy_setopt(curl_, CURLOPT_USERPWD, "SUREN:SUREN");
				CHECKCURLE_BREAK(easycode);
				easycode = curl_easy_setopt(curl_, CURLOPT_DEBUGFUNCTION, debugCallback_);
				CHECKCURLE_BREAK(easycode);
				easycode = curl_easy_setopt(curl_, CURLOPT_DEBUGDATA, &debug_data_/*fd*/);
				CHECKCURLE_BREAK(easycode);
			}
			rc = 0;
		} while (0);
		return rc;
	}

	int Easy::setProxy(char const* sproxy, char const* sagent) {
		int rc = -1;
		do {
			CURLcode easycode;
#if 0
			easycode = curl_easy_setopt(curl_, CURLOPT_HEADER, 1L);
			CHECKCURLE_BREAK(easycode);
#endif
			easycode = curl_easy_setopt(curl_, CURLOPT_AUTOREFERER, 1L);
			CHECKCURLE_BREAK(easycode);
			easycode = curl_easy_setopt(curl_, CURLOPT_FOLLOWLOCATION, 1L);
			CHECKCURLE_BREAK(easycode);
			CHECKCURLE_BREAK(easycode);
			easycode = curl_easy_setopt(curl_, CURLOPT_MAXREDIRS, 1L);
			CHECKCURLE_BREAK(easycode);
			easycode = curl_easy_setopt(curl_, CURLOPT_UNRESTRICTED_AUTH, 1L);
			CHECKCURLE_BREAK(easycode);
			if (sproxy) {
				easycode = curl_easy_setopt(curl_, CURLOPT_PROXY, sproxy);
				CHECKCURLE_BREAK(easycode);
			}
			if (sagent) {
				easycode = curl_easy_setopt(curl_, CURLOPT_USERAGENT, sagent);
				CHECKCURLE_BREAK(easycode);
			}
			else {
				easycode = curl_easy_setopt(curl_, CURLOPT_USERAGENT, "libcurl-agent/1.0");
				CHECKCURLE_BREAK(easycode);
			}
			rc = 0;
		} while (0);
		return rc;
	}

	int Easy::setCallback(void *readcbk, void *writecbk, void *progresscbk) {
		int rc = -1;
		do {
			CURLcode easycode;
			if (readcbk) {
				easycode = curl_easy_setopt(curl_, CURLOPT_READFUNCTION, readcbk/*readCallback_*/);
				CHECKCURLE_BREAK(easycode);
				easycode = curl_easy_setopt(curl_, CURLOPT_READDATA, this);
				CHECKCURLE_BREAK(easycode);
			}
			if (writecbk){
				easycode = curl_easy_setopt(curl_, CURLOPT_WRITEFUNCTION, writecbk/*writeCallback_*/);
				CHECKCURLE_BREAK(easycode);
				easycode = curl_easy_setopt(curl_, CURLOPT_WRITEDATA, this);
				CHECKCURLE_BREAK(easycode);
			}
			if (progresscbk) {
				easycode = curl_easy_setopt(curl_, CURLOPT_NOPROGRESS, 0L);
				CHECKCURLE_BREAK(easycode);
				easycode = curl_easy_setopt(curl_, CURLOPT_PROGRESSFUNCTION, progresscbk/*progressCallback_*/);
				CHECKCURLE_BREAK(easycode);
				easycode = curl_easy_setopt(curl_, CURLOPT_PROGRESSDATA, this);
				CHECKCURLE_BREAK(easycode);
			}
			rc = 0;
		} while (0);
		return rc;
	}

	int Easy::setSSLCA(char const* spath) {
		int rc = -1;
		do {
			CURLcode easycode;
			if (!spath) {
				easycode = curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYPEER, 0L);
				CHECKCURLE_BREAK(easycode);
				easycode = curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYHOST, 0L);
				CHECKCURLE_BREAK(easycode);
			}
			else {
				// 缺省 PEM，另外支持 DER
 				//easycode = curl_easy_setopt(curl_, CURLOPT_SSLCERTTYPE, "PEM");
 				//CHECKCURLE_BREAK(easycode);
				easycode = curl_easy_setopt(curl_, CURLOPT_SSL_VERIFYPEER, 1L);
				CHECKCURLE_BREAK(easycode);
				easycode = curl_easy_setopt(curl_, CURLOPT_CAINFO, spath);
				CHECKCURLE_BREAK(easycode);
			}
			rc = 0;
		} while (0);
		return rc;
	}

	int Easy::addPost(std::list<FMParam> const* params, char const* spost) {
		int rc = -1;
		do {
			// form post
			if (params) {
				if (!formAdd(curl_, *params)) {
					break;
				}
			}
			CURLcode easycode;
			if (!formpost_) {
				if (spost) {
					easycode = curl_easy_setopt(curl_, CURLOPT_POST, 1L);
					CHECKCURLE_BREAK(easycode);
					easycode = curl_easy_setopt(curl_, CURLOPT_POSTFIELDS, spost);
					CHECKCURLE_BREAK(easycode);
					easycode = curl_easy_setopt(curl_, CURLOPT_POSTFIELDSIZE, strlen(spost));
					CHECKCURLE_BREAK(easycode);
				}
			}
			else {
				if (spost) {
					easycode = curl_easy_setopt(curl_, CURLOPT_COPYPOSTFIELDS, spost);
					CHECKCURLE_BREAK(easycode);
				}
				easycode = curl_easy_setopt(curl_, CURLOPT_HTTPPOST, formpost_);
				CHECKCURLE_BREAK(easycode);
			}
			rc = 0;
		} while (0);

		return rc;
	}

	bool Easy::formAdd(CURL *curl, std::list<FMParam> const& params) {
		bool suc = true;
		do {
			CHECKPTR_BREAK(curl_);
			
			if (formpost_) {
				::curl_formfree(formpost_);
				formpost_ = NULL;
				lastptr_ = NULL;
			}
			for (std::list<FMParam>::const_iterator it = params.begin();
				it != params.end();
				++it) {
				if (!formAdd(curl, *it)) {
					suc = false;
					break;
				}
			}
		} while (0);
		return suc;
	}

	bool Easy::formAdd(CURL *curl, FMParam const& param) {
		param.value->MFSeek(0L, SEEK_END);
		size_t size = param.value->MFTell();
		param.value->MFSeek(0L, SEEK_SET);

		//multipart/form-data
		return CURLE_OK == ::curl_formadd(&formpost_,
			&lastptr_,
			CURLFORM_COPYNAME, param.strkey.c_str(),			// name=""
			CURLFORM_STREAM, param.value,						// CURLOPT_READFUNCTION
			CURLFORM_CONTENTSLENGTH, size,
			CURLFORM_FILENAME, param.fileinfo.szfilename,		// filename=""
			CURLFORM_CONTENTTYPE, "application/octet-stream",
			CURLFORM_END);
#if 0
		::curl_formadd(&formpost_,
			&lastptr_,
			CURLFORM_COPYNAME, "submit",						// name=""
			CURLFORM_COPYCONTENTS, "submit",					// value=""
			CURLFORM_END);
#endif

#if 0
		// https://curl.haxx.se/libcurl/c/curl_formadd.html

		//////////////////////////////////////////////////////////////////////////
		// 上传本地文件
		//////////////////////////////////////////////////////////////////////////
		::curl_formadd(&formpost_,
			&lastptr_,
			CURLFORM_COPYNAME, "file1",							// name=""
			CURLFORM_FILE, "E:/svn/setup/acl-v3.1.3.zip",		// filename=""
			CURLFORM_CONTENTTYPE, "application/octet-stream",	// Content-Type: 
			CURLFORM_END);
		::curl_formadd(&formpost_,
			&lastptr_,
			CURLFORM_COPYNAME, "file2",							// name=""
			CURLFORM_FILE, "C:/Users/Administrator/Desktop/abc.png",
			CURLFORM_CONTENTTYPE, "application/octet-stream",	// Content-Type: 
			CURLFORM_END);
#endif

#if 0
		//////////////////////////////////////////////////////////////////////////
		// 上传本地文件(断点续传)
		//////////////////////////////////////////////////////////////////////////
		FILE* fp = fopen("C:/Users/Administrator/Desktop/abc.png", "rb");
		fseek(fp, 0L, SEEK_END);
		size_t size = ftell(fp);
		fseek(fp, 0L, SEEK_SET);

		::curl_formadd(&formpost_,
			&lastptr_,
			CURLFORM_COPYNAME, "file1",							// name=""
			CURLFORM_STREAM, fp,								// CURLOPT_READFUNCTION
			CURLFORM_CONTENTSLENGTH, size,
			CURLFORM_FILENAME, "*.png",							// filename=""
			CURLFORM_CONTENTTYPE, "application/octet-stream",
			CURLFORM_END);
#endif

#if 0
		//////////////////////////////////////////////////////////////////////////
		// 读取并发送文件内容
		//////////////////////////////////////////////////////////////////////////
		::curl_formadd(&formpost_,
			&lastptr_,
			CURLFORM_COPYNAME, "filecontent",
			CURLFORM_FILECONTENT, "F:/1.txt",
			CURLFORM_END);
#endif

#if 0
		//////////////////////////////////////////////////////////////////////////
		// 上传自定义内容文件
		//////////////////////////////////////////////////////////////////////////
		char* buf = "abcdefg0123456789\r\n";
		::curl_formadd(&formpost_,
			&lastptr_,
			CURLFORM_COPYNAME, "file1",							// name=""
			CURLFORM_BUFFER, "*.txt",							// filename=""
			CURLFORM_BUFFERPTR, buf,
			CURLFORM_BUFFERLENGTH, strlen(buf),
			CURLFORM_CONTENTTYPE, "text/plain",					// Content-Type: 
			CURLFORM_END);
#endif

#if 0
		//////////////////////////////////////////////////////////////////////////
		// 批量提交表单文件
		//////////////////////////////////////////////////////////////////////////
		struct curl_forms forms[3];
		forms[0].option = CURLFORM_FILE;
		forms[0].value = "E:/svn/setup/acl-v3.1.3.zip";
		forms[1].option = CURLFORM_FILE;
		forms[1].value = "C:/Users/Administrator/Desktop/abc.png";
		forms[2].option = CURLFORM_END;
		::curl_formadd(&formpost_,
			&lastptr_,
			CURLFORM_COPYNAME, "file1",
			CURLFORM_ARRAY, forms,
			CURLFORM_CONTENTTYPE, "multipart/mixed",
			CURLFORM_END);
		::curl_formadd(&formpost_,
			&lastptr_,
			CURLFORM_COPYNAME, "end",
			CURLFORM_COPYCONTENTS, "end",
			CURLFORM_END);
#endif
	}

	int Easy::perform() {
		int rc = -1;
#if 0
		Open();
#endif
		do {
			CHECKPTR_BREAK(curl_);

			CURLcode easycode;	
			easycode = ::curl_easy_perform(curl_);
			CHECKCURLE_BREAK(easycode);

			rc = 0;
		} while (0);
#if 0
		Close();
		purge();
#endif
		return rc;
	}

	int Easy::check(char const* url, double & size) {
#if 0
		purge();
#endif
		size = 0;
		int rc = -1;
		do {
			if (!curl_) {
				curl_ = ::curl_easy_init();
				CHECKPTR_BREAK(curl_);
			}
			CURLcode easycode;
			easycode = curl_easy_setopt(curl_, CURLOPT_URL, url);
			CHECKCURLE_BREAK(easycode);
			easycode = curl_easy_setopt(curl_, CURLOPT_NOBODY, 1L);
			CHECKCURLE_BREAK(easycode);
			easycode = curl_easy_setopt(curl_, CURLOPT_FILETIME, 1L);
			CHECKCURLE_BREAK(easycode);
			easycode = curl_easy_setopt(curl_, CURLOPT_HEADER, 0L);
			CHECKCURLE_BREAK(easycode);
			easycode = ::curl_easy_perform(curl_);
			CHECKCURLE_BREAK(easycode);

			int retcode = 0;
			easycode = curl_easy_getinfo(curl_, CURLINFO_RESPONSE_CODE, &retcode);
			CHECKCURLE_BREAK(easycode);
			if (200 == retcode) {
				rc = 0;
#if 0
				// 
				time_t file_time = 0;
				easycode = curl_easy_getinfo(curl_, CURLINFO_FILETIME, &file_time);
				CHECKCURLE_BREAK(easycode);
#endif
#if 1
				// 
				double file_size = 0;
				easycode = curl_easy_getinfo(curl_, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &file_size);
				CHECKCURLE_BREAK(easycode);
				size = file_size;
#endif
#if 0
				//
				double file_sec = 0;
				easycode = curl_easy_getinfo(curl_, CURLINFO_TOTAL_TIME, &file_sec);
				CHECKCURLE_BREAK(easycode);
#endif
				//printf("size[%.3f] sec => %.3f(s) time => %s",file_size, file_sec, -1 != file_time ? ctime((time_t *)&file_time) : "<NIL>");
			}
		} while (0);
#if 1
		purge();
#endif
		return rc;
	}

	void Easy::purge() {
		if (headerlist_) {
			::curl_slist_free_all(headerlist_);
			headerlist_ = NULL;
		}

		if (formpost_) {
			::curl_formfree(formpost_);
			formpost_ = NULL;
			lastptr_ = NULL;
		}

		if (curl_) {
			::curl_easy_cleanup(curl_);
			curl_ = NULL;
		}
	}

	Easy::~Easy() {
		purge();
		//curl_global_cleanup();
	}
}