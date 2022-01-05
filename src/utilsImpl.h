#pragma once

#include "Macro.h"

namespace utils {

	namespace uuid {
		std::string _createUUID();
	}

	void _replaceAll(std::string& s, std::string const& src, std::string const& dst);
	
	void _replaceEscChar(std::string& s);

	std::string _GetModulePath(std::string* filename = NULL);

	bool _mkDir(char const* dir);

	/*tid_t*/std::string _gettid();

	std::string const _trim_file(char const* _FILE_);

	std::string const _trim_func(char const* _FUNC_);

	std::string _str_error(unsigned errnum);

	void _convertUTC(time_t const t, struct tm& tm, time_t* tp = NULL, int64_t timezone = MY_CCT);

	std::string _strfTime(time_t const t, int64_t timezone);
	
	time_t _strpTime(char const* s, int64_t timezone);

	void _timezoneInfo(struct tm const& tm, int64_t timezone);

	std::string _ws2str(std::wstring const& ws);

	std::wstring _str2ws(std::string const& str);
	
	bool _is_utf8(char const* str, size_t len);
	
	std::string _gbk2UTF8(const char* gbk, size_t len);

	std::string _utf82GBK(char const* utf8, size_t len);

	unsigned int _now_ms();
}