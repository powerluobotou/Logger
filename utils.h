/**
*
*   全局函数
*	Created by andy_ro@qq.com 2021.11.17
*
*/
#ifndef UTILS_H_INCLUDE
#define UTILS_H_INCLUDE

#include "Macro.h"

namespace utils {
	//gettid
	/*tid_t*/std::string gettid();

	//trim_file
	std::string const trim_file(char const* _FILE_);

	//trim_func
	std::string const trim_func(char const* _FUNC_);

	//str_error
	std::string str_error(unsigned errnum);

	//stack_backtrace
	std::string stack_backtrace();
}

#endif