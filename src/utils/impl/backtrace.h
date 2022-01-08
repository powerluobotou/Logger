#pragma once

#include "../../Macro.h"

namespace utils {

	std::string _stack_backtrace();

	void _crash_coredump(std::function<void()> cb);
}