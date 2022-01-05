#include "chkVer.h"
#include "chkVerImpl.h"
#include "auth.h"

namespace utils {

	void checkVersion(std::string const& v, std::string const& url, std::string const& dir, std::function<void(int rc)> cb) {
		AUTHORIZATION_CHECK;
		utils::_checkVersion(v, url, dir, cb);
	}
}