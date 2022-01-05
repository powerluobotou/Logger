/**
*
*   基于CURL库的HTTP操作
*	Created by 萝卜 2021.12.17
*
*/
#pragma once

#include "../../Macro.h"
#include "../SetOperation.h"
#include "curl/curl.h"

namespace Curl {

	class ClientImpl;
	class MultiImpl {
		friend class ClientImpl;
	public:
		MultiImpl();
		~MultiImpl();
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
	};
}