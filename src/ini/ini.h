#pragma once

#include "../Macro.h"

namespace utils {
	
	namespace INI {
		typedef std::map<std::string, std::string> Section;
		typedef std::map<std::string, Section> Sections;
		class ReaderImpl;
		class Reader {
		public:
			Reader();
			~Reader();
		public:
			bool parse(char const* filename);
			bool parse(char const* buf, size_t len);
			Sections const& get();
			Section* get(char const* section);
			std::string get(char const* section, char const* key);
			std::string get(char const* section, char const* key, bool& hasKey);
		private:
			ReaderImpl* impl_;
		};
	}
}