/**
*
*   授权过期时间
*	Created by andy_ro@qq.com 2021.11.17
*
*/
#pragma once

#include "utilsImpl.h"

#define AUTHORIZATION_SUPPORT

namespace utils {
	
	void regAuthCallback(char const* expired, int64_t timezone);

	bool authExpired();
}

#ifdef AUTHORIZATION_SUPPORT
#define AUTHORIZATION_CHECK \
	if (utils::authExpired()) { \
		return; \
	}

#define AUTHORIZATION_CHECK_B \
	if (utils::authExpired()) { \
		return false; \
	}

#define AUTHORIZATION_CHECK_P \
	if (utils::authExpired()) { \
		return NULL; \
	}

#define AUTHORIZATION_CHECK_R \
	if (utils::authExpired()) { \
		return -1; \
	}

#define AUTHORIZATION_CHECK_I \
	if (utils::authExpired()) { \
		return 0; \
	}

#define AUTHORIZATION_CHECK_S \
	if (utils::authExpired()) { \
		return ""; \
	}
#define RegAuthCallback utils::regAuthCallback
#else
#define AUTHORIZATION_CHECK
#define AUTHORIZATION_CHECK_B
#define AUTHORIZATION_CHECK_P
#define AUTHORIZATION_CHECK_R
#define AUTHORIZATION_CHECK_I
#define AUTHORIZATION_CHECK_S
#define RegAuthCallback
#endif