#pragma once

#include "Easy.h"
#include "Multi.h"

#define CHECKPTR_BREAK(x) \
		if (NULL == (x)) { \
			break;  \
		}

#define CHECKCURLE_BREAK(x) \
		if (CURLE_OK != (x)) { \
			break;  \
		}

#define CHECKCURLM_BREAK(x) \
		if (CURLM_OK != (x)) { \
			break;  \
		}

namespace Curl {

	enum REQState {
		eNetError,
		eContinue,
		eTimeout,
		eSuc,
		eFailed,
		eInterrupt,
	};
	class Easy;
	class Multi;
	class Client {
	public:
		Client();
		Client(bool sync);
		int check(char const* url, double& size);
		int get(
			char const* url,
			std::list<std::string> const* headers,
			std::string* resp = NULL,
			char const* spath = NULL,
			bool dump = true, FILE* fd = stderr);
		int post(
			char const* url,
			std::list<std::string> const* headers,
			char const* spost,
			std::string* resp,
			char const* spath = NULL,
			bool dump = true, FILE* fd = stderr);
		int upload(
			char const* url,
			std::list<FMParam> const* params,
			std::string* resp,
			Functor callback,
			char const* spath = NULL,
			bool dump = true, FILE* fd = stderr);
		int download(
			char const* url,
			char const* savepath,
			Functor callback,
			char const* spath = NULL,
			bool dump = true, FILE* fd = stderr);
		int perform();
	private:
		Multi* multi_;
		std::list<Easy*> list_easy_;
	public:
		~Client();
	};
}