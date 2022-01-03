/**
*
*   堆栈跟踪
*	Created by andy_ro@qq.com 2021.11.17
*
*/
#pragma once

#include "Macro.h"

namespace utils {

	std::string _stack_backtrace();

	void _enableCrashDump();
}