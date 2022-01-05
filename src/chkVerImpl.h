#pragma once

#include "Macro.h"

namespace utils {
	
	void _checkVersion(std::string const& v, std::string const& url, std::string const& dir, std::function<void(int rc)> cb);
}