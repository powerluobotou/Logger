/**
*
*   全局函数
*	Created by andy_ro@qq.com 2021.11.17
*
*/
#include "utils.h"
#include <errno.h>

#ifdef _windows_
#include <tchar.h>
#include <ImageHlp.h>
#include <comutil.h>
#pragma comment(lib, "comsuppw.lib")
#elif defined(_linux_)
#include <cxxabi.h>
#include <execinfo.h>
#include <locale> 
#include <codecvt>
#endif

#include "Log.h"

#pragma execution_character_set("utf-8")

namespace utils {

	//initConsole
	void initConsole() {
#if defined(_windows_) && !defined(QT_SUPPORTS)
		AllocConsole();
		HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
#if _MSC_VER > 1920
		freopen("CONOUT$", "w", stdout);
#else
		int tp = _open_osfhandle((long)h, _O_TEXT);
		FILE* fp = _fdopen(tp, "w");
		*stdout = *fp;
		setvbuf(stdout, NULL, _IONBF, 0);
#endif
		SMALL_RECT rc = { 5,5,400,300 };
		::SetConsoleWindowInfo(h, TRUE, &rc);
		CONSOLE_FONT_INFOEX cfi;
		ZeroMemory(&cfi, sizeof(cfi));
		cfi.cbSize = sizeof(cfi);
		cfi.dwFontSize = {0, 12};
		cfi.FontFamily = FF_DONTCARE;
		cfi.FontWeight = FW_LIGHT;
		lstrcpy(cfi.FaceName, _T("Consolas"));
		::SetCurrentConsoleFontEx(h, false, &cfi);
#endif
	}

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
	//convertUTC
	void convertUTC(time_t const t, struct tm& tm, time_t* tp, int64_t timezone) {
		switch (timezone) {
		case MY_UTC: {
#ifdef _windows_
			gmtime_s(&tm, &t);//UTC/GMT
#else
			gmtime_s(&t, &tm);//UTC/GMT
#endif
			if (tp) {
				//tm -> time_t
				time_t t_zone = mktime(&tm);
				//tm -> time_t
				assert(t_zone == mktime(&tm));
				*tp = t_zone;
				if (t_zone != mktime(&tm)) {
					LOG_S_FATAL("t_zone != mktime(&tm)");
				}
			}
			break;
		}
		case MY_PST:
		case MY_MST:
		case MY_EST:
		case MY_BST:
		case MY_GST:
		case MY_CCT:
		case MY_JST:
		default: {
			struct tm tm_utc = { 0 };
#ifdef _windows_
			gmtime_s(&tm_utc, &t);//UTC/GMT
#else
			gmtime_s(&t, &tm_utc);//UTC/GMT
#endif
			//tm -> time_t
			time_t t_utc = mktime(&tm_utc);
			//(UTC+08:00) Asia/shanghai, Beijing(China) (tm_hour + MY_CCT) % 24
			time_t t_zone = t_utc + timezone * 3600;
#ifdef _windows_
			//time_t -> tm
			localtime_s(&tm, &t_zone);
#else
			//time_t -> tm
			localtime_r(&t_zone, &tm);
#endif
			//tm -> time_t
			assert(t_zone == mktime(&tm));
			if (tp) {
				*tp = t_zone;
			}
			if (t_zone != mktime(&tm)) {
				LOG_S_FATAL("t_zone != mktime(&tm)");
			}
			break;
		}
		}
	}

	//timezoneInfo
	void timezoneInfo(struct tm const& tm, int64_t timezone) {
		switch (timezone) {
		case MY_EST: {
			LOG_INFO("America/New_York %04d-%02d-%02d %02d:%02d:%02d",
				tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
			break;
		}
		case MY_BST: {
			LOG_INFO("Europe/London %04d-%02d-%02d %02d:%02d:%02d",
				tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
			break;
		}
		case MY_GST: {
			LOG_INFO("Asia/Dubai %04d-%02d-%02d %02d:%02d:%02d",
				tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
			break;
		}
		case MY_CCT: {
			LOG_INFO("Beijing (China) %04d-%02d-%02d %02d:%02d:%02d",
				tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
			break;
		}
		case MY_JST: {
			LOG_INFO("Asia/Tokyo %04d-%02d-%02d %02d:%02d:%02d",
				tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
			break;
		}
		default:
			break;
		}
	}

	namespace uuid {
		static std::random_device              rd;
		static std::mt19937                    gen(rd());
		static std::uniform_int_distribution<> dis(0, 15);
		static std::uniform_int_distribution<> dis2(8, 11);
		//createUUID
		std::string createUUID() {
			std::stringstream ss;
			int i;
			ss << std::hex;
			for (i = 0; i < 8; i++) {
				ss << dis(gen);
			}
			ss << "-";
			for (i = 0; i < 4; i++) {
				ss << dis(gen);
			}
			ss << "-4";
			for (i = 0; i < 3; i++) {
				ss << dis(gen);
			}
			ss << "-";
			ss << dis2(gen);
			for (i = 0; i < 3; i++) {
				ss << dis(gen);
			}
			ss << "-";
			for (i = 0; i < 12; i++) {
				ss << dis(gen);
			};
			return ss.str();
		}
	}

	//ws2str
	std::string ws2str(std::wstring const& ws) {
#ifdef _windows_
		_bstr_t const t = ws.c_str();
		return (char const*const)t;
#else
		std::wstring_convert<std::codecvt_utf8<wchar_t>> cvt;
		return cvt.to_bytes(ws);
#endif
	}

	//str2ws
	std::wstring str2ws(std::string const& str) {
#ifdef _windows_
		_bstr_t const t = str.c_str();
		return (wchar_t const*const)t;
#else
		std::wstring_convert<std::codecvt_utf8<wchar_t>> cvt;
		return cvt.from_bytes(str);
#endif
	}
}