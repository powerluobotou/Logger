/**
*
*   授权过期时间
*	Created by andy_ro@qq.com 2021.11.17
*
*/
#pragma once

#include "utils.h"

#define AUTHORIZATION_SUPPORT

namespace utils {
	
	void regAuthCallback(char const* expired, int64_t timezone);

	bool authCheck();
}

#ifdef AUTHORIZATION_SUPPORT
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

#define AUTHORIZATION_CHECK_R \
	if (!utils::authCheck()) { \
		return -1; \
	}

#define AUTHORIZATION_CHECK_I \
	if (!utils::authCheck()) { \
		return 0; \
	}

#define AUTHORIZATION_CHECK_S \
	if (!utils::authCheck()) { \
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