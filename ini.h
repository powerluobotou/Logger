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
		class _Reader;
		class Reader {
		public:
			Reader();
			~Reader();
		public:
			bool parse(char const* filename);
			bool parse(char const* buf, size_t len);
			Section* get(char const* section);
			std::string get(char const* section, char const* key);
		private:
			_Reader* reader_;
		};
	}

	void CheckVersion(INI::Section& version, std::function<void(int rc)> cb);
}