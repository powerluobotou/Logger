#pragma once

#include "../../Macro.h"

namespace utils {

	void _runAsRoot(std::string const& execname, bool bv = false);

	bool _enablePrivilege(std::string const& path);
}