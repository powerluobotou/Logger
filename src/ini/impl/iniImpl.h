﻿#pragma once

#include "../../Macro.h"

namespace utils {
	
	namespace INI {
		
		class Section;
		class Sections;

		void _readBuffer(char const* buf, Sections& sections);
		void _readFile(char const* filename, Sections& sections);

		class ReaderImpl {
		public:
			ReaderImpl();
			~ReaderImpl();
		public:
			bool parse(char const* filename);
			bool parse(char const* buf, size_t len);
			Sections const& get();
			Section* get(char const* section);
			std::string get(char const* section, char const* key);
			std::string get(char const* section, char const* key, bool& hasKey);
			void set(char const* section, char const* key, char const* value, char const* filename);
		private:
			void write(char const* filename);
		private:
			Sections* m_;
		};
	}
}