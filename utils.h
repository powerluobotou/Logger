/**
*
*   全局函数
*	Created by andy_ro@qq.com 2021.11.17
*
*/
#ifndef UTILS_H_INCLUDE
#define UTILS_H_INCLUDE

#include "Macro.h"

#define MY_MST (-7)
#define MY_BST (+1)
#define MY_UTC (+0)//UTC/GMT
//(UTC+08:00) Beijing(China)
#define MY_CCT (+8)

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

	//UTCTime
	struct tm UTCTime(time_t const t);

	//convertUTC
	struct tm convertUTC(time_t const t, time_t* tp = NULL, int64_t timezone = MY_CCT);
}

#endif