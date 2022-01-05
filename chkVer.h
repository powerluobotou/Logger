/**
*
*   版本检查
*	Created by andy_ro@qq.com 2022.01.04
*
*/
#pragma once

#include "Macro.h"

namespace utils {
	
	void checkVersion(std::string const& v, std::string const& url, std::string const& dir, std::function<void(int rc)> cb);
}