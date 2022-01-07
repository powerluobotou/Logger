#include "../test.h"
#include "../utils.h"
#include "utilsImpl.h"
#include "backtrace.h"
#include "mymd5.h"
#include "auth.h"

#ifdef _windows_
//#include <DbgHelp.h>
#include <ImageHlp.h>
#include <comutil.h>
#pragma comment(lib, "comsuppw.lib")
#pragma comment(lib, "Dbghelp.lib")
#elif defined(_linux_)
#include <cxxabi.h>
#include <execinfo.h>
#include <locale> 
#include <codecvt>
#endif

#include "timer.h"

namespace utils {
	
	void Test() {
		utils::testTimer();
	}
}