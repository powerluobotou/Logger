#pragma once

#include "Macro.h"

namespace utils {
	
	namespace INI {

		typedef std::map<std::string, std::string> Section;
		typedef std::map<std::string, Section> Sections;

		void _readBuffer(char const* buf, Sections& sections);
		void _readFile(char const* filename, Sections& sections);

		class ReaderImpl {
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
}