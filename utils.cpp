/**
*
*   全局函数
*	Created by andy_ro@qq.com 2021.11.17
*
*/
#include "utils.h"
#include <errno.h>

#ifdef _windows_
#include <ImageHlp.h>
#elif defined(_linux_)
#include <cxxabi.h>
#include <execinfo.h>
#endif

#include "Log.h"

#pragma execution_character_set("utf-8")

namespace utils {
	//gettid
	/*tid_t*/std::string gettid() {
		std::ostringstream oss;
		oss << std::this_thread::get_id();
		return oss.str();//(tid_t)std::stoull(oss.str());
	}

	//trim_file
	std::string const trim_file(char const* _FILE_) {
		std::string f = _FILE_;
#ifdef _windows_
		f = f.substr(f.find_last_of('\\') + 1, -1);
#else
		f = f.substr(f.find_last_of('/') + 1, -1);
#endif
		return f;
	}

	//trim_func
	std::string const trim_func(char const* _FUNC_) {
		std::string f = _FUNC_;
		f = f.substr(f.find_last_of(':') + 1, -1);
		return f;
	}

	//str_error
	std::string str_error(unsigned errnum) {
#ifdef _windows_
		char* msg;
		FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, errnum, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&msg, 0, NULL);
		std::string str(msg);
		LocalFree(msg);
		return str;
#else
		strerror(errnum);
#endif
	}

	//stack_backtrace
	std::string stack_backtrace() {
		/*
			windows
				SymInitialize
				StackWalk
				CaptureStackBackTrace 获取当前堆栈
				SymGetSymFromAddr
				SymFromAddr 获取符号信息
				SymGetLineFromAddr64 获取文件和行号信息
				SymCleanup

			linux
			   int backtrace(void **buffer, int size);
			   char **backtrace_symbols(void *const *buffer, int size);
			   void backtrace_symbols_fd(void *const *buffer, int size, int fd);
		*/
		std::string stack;
		stack.append("STACK-BACKTRACE:\n");
#ifdef _windows_
		HANDLE process = GetCurrentProcess();
		SymInitialize(process, NULL, TRUE);
#if 1
		static const int MAX_STACK_FRAMES = 20;
		void* pstack[MAX_STACK_FRAMES];
		WORD frames = CaptureStackBackTrace(0, MAX_STACK_FRAMES, pstack, NULL);
		
		for (WORD i = 1; i < frames; ++i) {
			DWORD64 address = (uint64_t)(pstack[i]);
			DWORD64 sym_c = 0;
			
			char buf[sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR)];
			PSYMBOL_INFO psymbol = (PSYMBOL_INFO)buf;
			psymbol->SizeOfStruct = sizeof(SYMBOL_INFO);
			psymbol->MaxNameLen = MAX_SYM_NAME;

			if (SymFromAddr(process, address, &sym_c, psymbol)) {
				
				DWORD options = SymGetOptions();
				options |= SYMOPT_LOAD_LINES;
				options |= SYMOPT_FAIL_CRITICAL_ERRORS;
				options |= SYMOPT_DEBUG;
				SymSetOptions(options);

				DWORD line_c = 0;
				IMAGEHLP_LINE64 line = { 0 };
				line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
				if (SymGetLineFromAddr64(process, address, &line_c, &line)) {
// 					std::ostringstream oss;
// 					oss << "\t"
// 						<< trim_func((char const*)psymbol->Name)
// 						<< " at "
// 						<< trim_file((char const*)line.FileName)
// 						<< ":" << line.LineNumber
// 						<< "(0x" << std::hex << psymbol->Address << std::dec << ")" << std::endl;
//					stack.append(oss.str());
					char s[256];
					snprintf(s, sizeof(s), "\t%s at %s:%d(%#x)\n",
						trim_func((char const*)psymbol->Name).c_str(),
						trim_file((char const*)line.FileName).c_str(),
						line.LineNumber,
						(uint64_t)psymbol->Address);
					stack.append(s);
				}
				else {
// 					std::string s(str_error(GetLastError()));
// 					std::ostringstream oss;
// 					oss << "\t"
// 						<< /*trim_func*/((char const*)psymbol->Name)
// 						<< " at "
// 						<< "(0x" << std::hex << psymbol->Address << std::dec << ")"
// 						//<< "\n\t" << GetLastError() << ":" << s.erase(s.find_last_of('\n')).c_str() << std::endl;
// 						<< std::endl;
// 					stack.append(oss.str());
// 					char s[256];
// 					snprintf(s, sizeof(s), "\t%s at %s:%d(%#x)\n",
// 						/*trim_func*/((char const*)psymbol->Name)/*.c_str()*/,
// 						"_",
// 						0,
// 						(uint64_t)psymbol->Address);
// 					stack.append(s);
				}
			}
			else {
// 				std::string s(str_error(GetLastError()));
// 				std::ostringstream oss;
// 				oss << "\t"
// 					<< /*trim_func*/((char const*)psymbol->Name)
// 					<< " at "
// 					<< "(0x" << std::hex << psymbol->Address << std::dec << ")"
// 					//<< "\n\t" << GetLastError() << ":" << s.erase(s.find_last_of('\n')).c_str() << std::endl;
// 					<< std::endl;
// 				stack.append(oss.str());
// 				char s[256];
// 				snprintf(s, sizeof(s), "\t%s at %s:%d(%#x)\n",
// 					/*trim_func*/((char const*)psymbol->Name)/*.c_str()*/,
// 					"_",
// 					0,
// 					(uint64_t)psymbol->Address);
// 				stack.append(s);
			}
		}
#else
		HANDLE thread = GetCurrentThread();
		CONTEXT ctx = { 0 };
		{
			GetThreadContext(thread, &ctx);
			__asm {call $ + 5}
			__asm {pop eax}
			__asm {mov ctx.Eip, eax}
			__asm {mov ctx.Ebp, ebp}
			__asm {mov ctx.Esp, esp}
		}

		STACKFRAME sf = { 0 };
		sf.AddrPC.Offset = ctx.Eip;
		sf.AddrPC.Mode = AddrModeFlat;
		sf.AddrFrame.Offset = ctx.Ebp;
		sf.AddrFrame.Mode = AddrModeFlat;
		sf.AddrStack.Offset = ctx.Esp;
		sf.AddrStack.Mode = AddrModeFlat;
		int c = 0;
		while (StackWalk(IMAGE_FILE_MACHINE_I386,
			process, thread, &sf, &ctx,
			NULL,
			SymFunctionTableAccess, SymGetModuleBase, NULL)) {
			if (c++ == 0)
				continue;
			DWORD64 sym_c = 0;
			char symbol[sizeof(IMAGEHLP_SYMBOL64) + MAX_SYM_NAME * sizeof(TCHAR)];
			PIMAGEHLP_SYMBOL64 psymbol = (PIMAGEHLP_SYMBOL64)&symbol;
			psymbol->SizeOfStruct = sizeof(IMAGEHLP_SYMBOL64);
			psymbol->MaxNameLength = MAX_SYM_NAME;

			if (SymGetSymFromAddr64(process, sf.AddrPC.Offset, &sym_c, psymbol)) {

				DWORD options = SymGetOptions();
				options |= SYMOPT_LOAD_LINES;
				options |= SYMOPT_FAIL_CRITICAL_ERRORS;
				options |= SYMOPT_DEBUG;
				SymSetOptions(options);

				DWORD line_c = 0;
				IMAGEHLP_LINE64 line = { 0 };
				line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
				if (SymGetLineFromAddr64(process, sf.AddrPC.Offset, &line_c, &line)) {
// 					std::ostringstream oss;
// 					oss << "\t"
// 						<< trim_func((char const*)psymbol->Name)
// 						<< " at "
// 						<< trim_file((char const*)line.FileName)
// 						<< ":" << line.LineNumber
// 						<< "(0x" << std::hex << psymbol->Address << std::dec << ")" << std::endl;
// 					stack.append(oss.str());
					char s[256];
					snprintf(s, sizeof(s), "\t%s at %s:%d(%#x)\n",
						trim_func((char const*)psymbol->Name).c_str(),
						trim_file((char const*)line.FileName).c_str(),
						line.LineNumber,
						(uint64_t)psymbol->Address);
					stack.append(s);
				}
				else {
// 					std::string s(str_error(GetLastError()));
// 					std::ostringstream oss;
// 					oss << "\t"
// 						<< /*trim_func*/((char const*)psymbol->Name)
// 						<< " at "
// 						<< "(0x" << std::hex << psymbol->Address << std::dec << ")"
// 						//<< "\n\t" << GetLastError() << ":" << s.erase(s.find_last_of('\n')).c_str() << std::endl;
// 						<< std::endl;
// 					stack.append(oss.str());
// 					char s[256];
// 					snprintf(s, sizeof(s), "\t%s at %s:%d(%#x)\n",
// 						/*trim_func*/((char const*)psymbol->Name)/*.c_str()*/,
// 						"_",
// 						0,
// 						(uint64_t)psymbol->Address);
// 					stack.append(s);
				}
			}
			else {
// 				std::string s(str_error(GetLastError()));
// 				std::ostringstream oss;
// 				oss << "\t"
// 					<< /*trim_func*/((char const*)psymbol->Name)
// 					<< " at "
// 					<< "(0x" << std::hex << psymbol->Address << std::dec << ")"
// 					//<< "\n\t" << GetLastError() << ":" << s.erase(s.find_last_of('\n')).c_str() << std::endl;
// 					<< std::endl;
// 				stack.append(oss.str());
// 				char s[256];
// 				snprintf(s, sizeof(s), "\t%s at %s:%d(%#x)\n",
// 					/*trim_func*/((char const*)psymbol->Name)/*.c_str()*/,
// 					"_",
// 					0,
// 					(uint64_t)psymbol->Address);
// 				stack.append(s);
			}
		}
#endif
		SymCleanup(process);
#elif defined(_linux_)
		static const int MAX_STACK_FRAMES = 200;
		void* pstack[MAX_STACK_FRAMES];
		int nptrs = ::backtrace(pstack, MAX_STACK_FRAMES);
		char** strings = ::backtrace_symbols(pstack, nptrs);
		if (strings) {
			bool demangle = false;
			size_t len = 256;
			char* demangled = demangle ? static_cast<char*>(::malloc(len)) : NULL;
			for (int i = 1; i < nptrs; ++i) {
				if (demangled) {
					//https://panthema.net/2008/0901-stacktrace-demangled/bin/exception_test(_ZN3Bar4testEv+0x79) [0x401909]
					char* left_par = NULL;
					char* plus = NULL;
					for (char* p = strings[i]; *p; ++p) {
						if (*p == '(')
							left_par = p;
						else if (*p == '+')
							plus = p;
					}
					if (left_par && plus) {
						*plus = '\0';
						int status = 0;
						char* ret = abi::__cxa_demangle(left_par + 1, demangled, &len, &status);
						*plus = '+';
						if (status == 0) {
							demangled = ret;
							stack.append(strings[i], left_par + 1);
							stack.append(demangled);
							stack.append(plus);
							stack.emplace_back('\n');
							continue;
						}
					}
				}
				stack.append(strings[i]);
				stack.emplace_back('\n');
			}
			if (demangled)
				free(demangled);
			free(strings);
		}
#endif
		return stack;
	}

	//https://www.cplusplus.com/reference/ctime/gmtime/
	//https://www.cplusplus.com/reference/ctime/mktime/?kw=mktime
	//https://www.runoob.com/cprogramming/c-standard-library-time-h.html
	//UTCTime
	struct tm UTCTime(time_t const t) {
		struct tm tm_utc = { 0 };
		gmtime_s(&tm_utc, &t);//UTC/GMT
		return tm_utc;
	}

	//convertUTC
	struct tm convertUTC(time_t const t, time_t* tp, int64_t timezone) {
		struct tm tm_utc = { 0 };
		gmtime_s(&tm_utc, &t);//UTC/GMT
		time_t t_utc = mktime(&tm_utc);
		switch (timezone) {
		case MY_UTC: {
			return tm_utc;
		}
		case MY_MST:
		case MY_BST:
		case MY_CCT:
		default: {
			struct tm tm = { 0 };
			//(UTC+08:00) Beijing(China) (tm_hour + MY_CCT) % 24
			time_t t_zone = t_utc + timezone * 3600;
			//time_t -> tm
			localtime_s(&tm, &t_zone);
			//tm -> time_t
			assert(t_zone == mktime(&tm));
			if (tp) {
				*tp = t_zone;
			}
			if (t_zone != mktime(&tm)) {
				LOG_S_FATAL("t_zone != mktime(&tm)");
			}
			if (timezone == MY_CCT) {
				char msg[512];
				snprintf(msg, sizeof msg, "%04d-%02d-%02d %02d:%02d:%02d",
					tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
				LOG_DEBUG("Beijing (China) %s", msg);
			}
			return tm;
		}
		}
	}
}