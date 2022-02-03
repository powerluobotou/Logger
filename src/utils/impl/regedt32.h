#pragma once

#include "../../Macro.h"

namespace utils {

#if defined(_windows_)
	//hkey HKEY_LOCAL_MACHINE
	//subkey "SOFTWARE\\WOW6432Node\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\{57a73df6-4ba9-4c1d-bbbb-517289ff6c13}"
	//valName "DisplayName"
	std::string _regQuery(HKEY hkey, char const* subkey, char const* valName);
#endif

}