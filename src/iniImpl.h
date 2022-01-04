/**
*
*   INI配置读取
*	Created by andy_ro@qq.com 2022.01.04
*
*/
#pragma once

#include "Macro.h"

namespace utils {
	
	namespace INI {

		typedef std::map<std::string, std::string> Section;
		typedef std::map<std::string, Section> Sections;

		void _readBuffer(char const* buf, Sections& sections);
		void _readFile(char const* filename, Sections& sections);

		class _Reader {
		public:
			bool parse(char const* filename);
			bool parse(char const* buf, size_t len);
			Sections const& get();
			Section* get(char const* section);
			std::string get(char const* section, char const* key);
		private:
			Sections m_;
		};
	}

	void _CheckVersion(std::string const& v, std::string const& url, std::function<void(int rc)> cb);
}