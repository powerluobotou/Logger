﻿#pragma once

#include "../Macro.h"

namespace utils {
	
	namespace INI {
#ifdef USEKVMAP
		class Section : public std::map<std::string, std::string> {
		};
#else
		typedef std::pair<std::string, std::string> Item;
		class Section : public std::vector<Item> {
		public:
			std::string& operator[](std::string const& key);
		};
#endif
		class Sections : public std::map<std::string, Section> {
		};
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
			void set(char const* section, char const* key, char const* value, char const* filename);
		private:
			ReaderImpl* impl_;
		};
	}
}