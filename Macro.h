/**
*
*   宏定义
*	Created by andy_ro@qq.com 2021.11.17
*
*/
#ifndef MACRO_H_INCLUDE
#define MACRO_H_INCLUDE

#if defined(WIN32) || defined(_WIN32)|| defined(WIN64) || defined(_WIN64)
#define _windows_
#elif defined(__linux__) || defined(linux) || defined(__linux) || defined(__gnu_linux__)
#define _linux_
#if defined(__ANDROID__)
#define _android_
#endif
#elif defined(macintosh) || defined(Macintosh) || (defined(__APPLE__) && defined(__MACH__))
#define _apple_
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <iostream>
#include <sstream>
#include <errno.h>
#include <assert.h>

#include <vector>
#include <list>
#include <map>
#include <set>
#include <functional>

#ifdef _windows_
#include <windows.h>
#include <stdint.h>
//#define __FUNC__ __FUNCSIG__//__FUNCTION__
#define __FUNC__ __FUNCTION__//__FUNCSIG__
#define INVALID_HANDLE_VALUE ((HANDLE)(-1))
//#define snprintf     _snprintf // _snprintf_s
#define strcasecmp   _stricmp
#define strncasecmp  _strnicmp
#define strtoull     _strtoui64
#elif defined(_linux_)
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#define __FUNC__ __func__
#define INVALID_HANDLE_VALUE (-1)
#endif

#include <mutex>
#include <shared_mutex>
#include <thread>

#define read_lock(mutex) std::shared_lock<std::shared_mutex> lock(mutex)
#define write_lock(mutex) std::unique_lock<std::shared_mutex> lock(mutex)

#endif