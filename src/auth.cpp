/**
*
*   授权过期时间
*	Created by andy_ro@qq.com 2021.11.17
*
*/
#include "auth.h"

namespace utils {

	static struct auth_cb_t {
		bool noOk;
		int64_t timezone;
		std::string expired;
		AuthCallback cb;
	}s_authcb/* = { false, 0, "", NULL }*/;

	//checkExpired
	bool checkExpired(char const* expired, bool& noOk, int64_t timezone) {
		if (!expired) {
			time_t t_zone_expired = utils::strpTime(expired, timezone);
			struct tm tm = { 0 };
			time_t t_zone_now = 0;
			utils::convertUTC(time(NULL), tm, &t_zone_now, timezone);
			if (t_zone_now <= t_zone_expired) {//unexpired
				noOk = true;
				return false;
			}
		}
		return true;
	}


	//regAuthCallback
	void regAuthCallback(AuthCallback cb, char const* expired) {
		s_authcb.cb = cb;
		s_authcb.expired = expired;
	}

	//authCheck
	bool authCheck() {
		if (s_authcb.noOk) {
			return false;
		}
		if (s_authcb.expired.empty()) {
			return true;
		}
		return s_authcb.cb(
			s_authcb.expired.c_str(),
			s_authcb.noOk,
			s_authcb.timezone);
	}
}