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

#define AUTHORIZATION_CHECK_B \
	if (!utils::authCheck()) { \
		return false; \
	}

#define AUTHORIZATION_CHECK_P \
	if (!utils::authCheck()) { \
		return NULL; \
	}

#define AUTHORIZATION_CHECK_I \
	if (!utils::authCheck()) { \
		return -1; \
	}

#define AUTHORIZATION_CHECK_S \
	if (!utils::authCheck()) { \
		return ""; \
	}