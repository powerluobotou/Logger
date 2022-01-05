#include "auth.h"
#include "utilsImpl.h"
#include "LoggerImpl.h"

namespace utils {

	typedef bool (*AuthCallback)(time_t const expired, bool& noOk, int64_t timezone);

	static struct auth_cb_t {
		bool noOk;
		int64_t timezone;
		time_t expired;
		AuthCallback cb;
	}s_authcb = { false, MY_CCT, 0, NULL };

	static bool checkExpired(time_t const expired, bool& noOk, int64_t timezone) {
		noOk = false;
		struct tm tm = { 0 };
		time_t t_now = 0;
		utils::_convertUTC(time(NULL), tm, &t_now, timezone);
		if (t_now <= expired) {//unexpired
			return false;
		}
		__LOG_CONSOLE_OPEN();
		std::string s = utils::_strfTime(expired, timezone);
		__TLOG_ERROR("auth expired %s", s.c_str());
		__LOG_CONSOLE_CLOSE(5000);
		noOk = true;
		return true;
	}

	void regAuthCallback(char const* expired, int64_t timezone) {
		s_authcb.cb = checkExpired;
		s_authcb.expired = utils::_strpTime(expired, timezone);
		s_authcb.timezone = timezone;
	}

	bool authExpired() {
		if (s_authcb.expired) {
			if (s_authcb.noOk) {
				return true;
			}
			return checkExpired(
				s_authcb.expired,
				s_authcb.noOk,
				s_authcb.timezone);
		}
		return false;
	}
}

#ifdef AUTHORIZATION_SUPPORT
static struct __init_t {
	__init_t() {
		RegAuthCallback("2022-04-24 10:00:00", MY_GST);
	}
}__x;
#endif