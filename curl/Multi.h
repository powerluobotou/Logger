#pragma once

#include "curl/curl.h"
#include "SetOperation.h"
#include <list>
#include <map>

namespace Curl {

	class Client;
	class Multi {
		friend class Client;
	public:
		Multi();
	protected:
		int perform();
		inline bool validate() { return NULL != curlm_; }
	protected:
		int add_handle(CURL* curl);
		int select();
		int info_read();
		int remove_handle(CURL* curl);
	private:
		CURLM* curlm_;
		std::list<CURL*> list_curl_;
	private:
		int add_handles();
		int remove_handles();
	public:
		~Multi();
	};
}