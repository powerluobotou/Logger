/**
*
*   授权过期时间
*	Created by andy_ro@qq.com 2021.11.17
*
*/
#pragma once

#include "utils.h"

namespace utils {
	
	typedef bool (*AuthCallback)(char const* expired, bool& noOk, int64_t timezone);

	//checkExpired
	bool checkExpired(char const* expired, bool& noOk, int64_t timezone);

	//regAuthCallback
	void regAuthCallback(AuthCallback cb, char const* expired);

	//authCheck
	bool authCheck();
}

#define AUTHORIZATION_CHECK \
	if (!utils::authCheck()) { \
		return; \
	}

#define AUTHORIZATION_CHECK_R \
	if (!utils::authCheck()) { \
		return false; \
	}