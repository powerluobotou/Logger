/**
*
*   授权过期时间
*	Created by andy_ro@qq.com 2021.11.17
*
*/
#include "auth.h"
#include "utilsImpl.h"
#include "Log.h"

namespace utils {

	typedef bool (*AuthCallback)(char const* expired, bool& noOk, int64_t timezone);

	static struct auth_cb_t {
		bool noOk;
		int64_t timezone;
		std::string expired;
		AuthCallback cb;
	}s_authcb/* = { false, 0, "", NULL }*/;

	bool checkExpired(char const* expired, bool& noOk, int64_t timezone) {
		noOk = false;
		time_t t_zone_expired = utils::_strpTime(expired, timezone);
		struct tm tm = { 0 };
		time_t t_zone_now = 0;
		utils::_convertUTC(time(NULL), tm, &t_zone_now, timezone);
		if (t_zone_now <= t_zone_expired) {//unexpired
			return false;
		}
		_LOG_CONSOLE_OPEN();
		_PLOG_ERROR("auth expired");
		noOk = true;
		return true;
	}

	void regAuthCallback(char const* expired, int64_t timezone) {
		s_authcb.cb = checkExpired;
		s_authcb.expired = expired;
		s_authcb.timezone = timezone;
	}

	bool authCheck() {
		if (!s_authcb.expired.empty()) {
			if (s_authcb.noOk) {
				_PLOG_ERROR("auth expired");
				return false;
			}
			return checkExpired/*s_authcb.cb*/(
				s_authcb.expired.c_str(),
				s_authcb.noOk,
				s_authcb.timezone);
		}
		return true;
	}
}

#ifdef AUTHORIZATION_SUPPORT
static struct __init_t {
	__init_t() {
		RegAuthCallback("2021-01-03 12:39:00", MY_GST);
	}
}__x;
#endif