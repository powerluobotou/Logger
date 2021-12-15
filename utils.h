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

	namespace uuid {
		//createUUID
		std::string createUUID();
	}
	
	//initConsole
	void initConsole();

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

	//convertUTC
	void convertUTC(time_t const t, struct tm& tm, time_t* tp = NULL, int64_t timezone = MY_CCT);

	//timezoneInfo
	void timezoneInfo(struct tm const& tm, int64_t timezone);

	//ws2str
	std::string ws2str(std::wstring const& ws);

	//str2ws
	std::wstring str2ws(std::string const& str);
	
	//is_utf8
	bool is_utf8(char const* str, size_t len);
	
	//utf82GBK
	std::string utf82GBK(char const* utf8, size_t len);
}

#endif