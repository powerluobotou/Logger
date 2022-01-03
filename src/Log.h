/**
*
*   异步日志系统实现
*	Created by andy_ro@qq.com 2021.11.17
*
*/
#pragma once

#include "../Macro.h"
#include "backtrace.h"

#include <mutex>
#include <shared_mutex>
#include <thread>
#include <condition_variable>

#include "Atomic.h"

#pragma execution_character_set("utf-8")

#ifdef __STACK__
#undef __STACK__
#endif
#define __STACK__       utils::_stack_backtrace().c_str()

#define _PARAM_FATAL     0,__FILE__,__LINE__,__FUNC__,__STACK__
#define _PARAM_ERROR     1,__FILE__,__LINE__,__FUNC__,NULL
#define _PARAM_WARN      2,__FILE__,__LINE__,__FUNC__,NULL
#define _PARAM_INFO      3,__FILE__,__LINE__,__FUNC__,NULL
#define _PARAM_TRACE     4,__FILE__,__LINE__,__FUNC__,__STACK__
#define _PARAM_DEBUG     5,__FILE__,__LINE__,__FUNC__,NULL

namespace LOGGER {

	class Log {
	private:
		Log();
		~Log();
	public:
		static Log* instance();
		void enable();
		void disable();
		void set_timezone(int64_t timezone = MY_CCT);
		void set_level(int level);
		char const* get_level();
		void set_color(int level, int title, int text);
		bool started();
		bool check(int level);
		void wait();
		void init(char const* dir, int level, char const* prename, size_t logsize);
		void write(int level, char const* file, int line, char const* func, char const* stack, uint8_t flag, char const* fmt, ...);
		void write_s(int level, char const* file, int line, char const* func, char const* stack, uint8_t flag, std::string const& msg);
		size_t format(int level, char const* file, int line, char const* func, uint8_t flag, char* msg, size_t size);
		void notify(char const* msg, size_t len, size_t pos, uint8_t flag, char const* stack, size_t stacklen);
		void stdoutbuf(int level, char const* msg, size_t len, size_t pos, uint8_t flag, char const* stack = NULL, size_t stacklen = 0);
		void checkSync(uint8_t flag);
		void stop();
	private:
		void open(char const* path);
		void write(char const* msg, size_t len, size_t pos, uint8_t flag);
		void close();
		void shift(struct tm const& tm, struct timeval const& tv);
		void update(struct tm& tm, struct timeval& tv);
		void get(struct tm& tm, struct timeval& tv);
		bool consume(struct tm const& tm, struct timeval const& tv);
		bool start();
		bool valid();
		void sync();
		void timezoneInfo();
	private:
#ifdef _windows_
		HANDLE fd_ = INVALID_HANDLE_VALUE;
#else
		int fd_ = INVALID_HANDLE_VALUE;
#endif
		pid_t pid_ = 0;
		int day_ = -1;
		size_t size_ = 0;
		std::atomic<int> level_{ LVL_DEBUG };
		char prefix_[256] = { 0 };
		char path_[512] = { 0 };
		int64_t timezone_ = MY_CCT;
		struct timeval tv_ = { 0 };
		struct tm tm_ = { 0 };
		mutable std::shared_mutex tm_mutex_;
		std::thread thread_;
		bool started_ = false;
		std::atomic_bool done_{ false };
		std::atomic_flag starting_{ ATOMIC_FLAG_INIT };
		std::mutex mutex_;
		std::condition_variable cond_;
		typedef std::pair<size_t, uint8_t> Flags;
		typedef std::pair<std::string, std::string> Message;
		typedef std::pair<Message, Flags> MessageT;
		std::vector<MessageT> messages_;
		bool sync_ = false;
		std::mutex sync_mutex_;
		std::condition_variable sync_cond_;
		bool enable_ = false;
		bool isConsoleOpen_ = false;
	};
}
#define _LOG_INSTANCE LOGGER::Log::instance
#define _LOG_INIT LOGGER::Log::instance()->init
#define _LOG LOGGER::Log::instance()->write
#define _LOG_S LOGGER::Log::instance()->write_s
#define _LOG_SET LOGGER::Log::instance()->set_level
#define _LOG_LVL LOGGER::Log::instance()->get_level
#define _LOG_TIMEZONE LOGGER::Log::instance()->set_timezone
#define _LOG_WAIT LOGGER::Log::instance()->wait
#define _LOG_COLOR LOGGER::Log::instance()->set_color
#define _LOG_CONSOLE_OPEN LOGGER::Log::instance()->enable
#define _LOG_CONSOLE_CLOSE LOGGER::Log::instance()->disable
#define _LOG_STOP LOGGER::Log::instance()->stop
#define _LOG_FORMAT LOGGER::Log::instance()->format
#define _LOG_STARTED LOGGER::Log::instance()->started
#define _LOG_CHECK LOGGER::Log::instance()->check
#define _LOG_CHECK_SYNC LOGGER::Log::instance()->checkSync
#define _LOG_CHECK_NOTIFY LOGGER::Log::instance()->notify
#define _LOG_CHECK_STDOUT LOGGER::Log::instance()->stdoutbuf

#define _LOG_SET_FATAL        _LOG_SET(LVL_FATAL)
#define _LOG_SET_ERROR        _LOG_SET(LVL_ERROR)
#define _LOG_SET_WARN         _LOG_SET(LVL_WARN)
#define _LOG_SET_INFO         _LOG_SET(LVL_INFO)
#define _LOG_SET_TRACE        _LOG_SET(LVL_TRACE)
#define _LOG_SET_DEBUG        _LOG_SET(LVL_DEBUG)

#define _LOG_COLOR_FATAL(a,b) _LOG_COLOR(LVL_FATAL, a, b)
#define _LOG_COLOR_ERROR(a,b) _LOG_COLOR(LVL_ERROR, a, b)
#define _LOG_COLOR_WARN(a,b)  _LOG_COLOR(LVL_WARN,  a, b)
#define _LOG_COLOR_INFO(a,b)  _LOG_COLOR(LVL_INFO,  a, b)
#define _LOG_COLOR_TRACE(a,b) _LOG_COLOR(LVL_TRACE, a, b)
#define _LOG_COLOR_DEBUG(a,b) _LOG_COLOR(LVL_DEBUG, a, b)

//LOG_XXX("%s", msg)
#ifdef _windows_
#define _LOG_FATAL(fmt,...)	 _LOG(_PARAM_FATAL, F_DETAIL|F_SYNC, fmt, ##__VA_ARGS__); _LOG_WAIT(); std::abort()
#define _LOG_ERROR(fmt,...)	 _LOG(_PARAM_ERROR, F_DETAIL,        fmt, ##__VA_ARGS__)
#define _LOG_WARN(fmt,...)	 _LOG(_PARAM_WARN,  F_DETAIL,        fmt, ##__VA_ARGS__)
#define _LOG_INFO(fmt,...)	 _LOG(_PARAM_INFO,  F_DETAIL,        fmt, ##__VA_ARGS__)
#define _LOG_TRACE(fmt,...)	 _LOG(_PARAM_TRACE, F_DETAIL,        fmt, ##__VA_ARGS__)
#define _LOG_DEBUG(fmt,...)	 _LOG(_PARAM_DEBUG, F_DETAIL,        fmt, ##__VA_ARGS__)
#else
#define _LOG_FATAL(args...)  _LOG(_PARAM_FATAL, F_DETAIL|F_SYNC, ##args); _LOG_WAIT(); std::abort()
#define _LOG_ERROR(args...)  _LOG(_PARAM_ERROR, F_DETAIL,        ##args)
#define _LOG_WARN(args...) 	 _LOG(_PARAM_WARN,  F_DETAIL,        ##args)
#define _LOG_INFO(args...)	 _LOG(_PARAM_INFO,  F_DETAIL,        ##args)
#define _LOG_TRACE(args...)	 _LOG(_PARAM_TRACE, F_DETAIL,        ##args)
#define _LOG_DEBUG(args...)  _LOG(_PARAM_DEBUG, F_DETAIL,        ##args)
#endif

//LOG_S_XXX(msg)
#define _LOG_S_FATAL(msg)     _LOG_S(_PARAM_FATAL, F_DETAIL|F_SYNC, msg); _LOG_WAIT(); std::abort()
#define _LOG_S_ERROR(msg)     _LOG_S(_PARAM_ERROR, F_DETAIL,        msg)
#define _LOG_S_WARN(msg)      _LOG_S(_PARAM_WARN,  F_DETAIL,        msg)
#define _LOG_S_INFO(msg)      _LOG_S(_PARAM_INFO,  F_DETAIL,        msg)
#define _LOG_S_TRACE(msg)     _LOG_S(_PARAM_TRACE, F_DETAIL,        msg)
#define _LOG_S_DEBUG(msg)     _LOG_S(_PARAM_DEBUG, F_DETAIL,        msg)

//LOG_XXX("%s", msg)
#ifdef _windows_
#define _LOG_FATAL_SYN(fmt,...)	 _LOG(_PARAM_FATAL, F_DETAIL|F_SYNC, fmt, ##__VA_ARGS__); _LOG_WAIT()
#else
#define _LOG_FATAL_SYN(args...)  _LOG(_PARAM_FATAL, F_DETAIL|F_SYNC, ##args); _LOG_WAIT()
#endif

//LOG_S_XXX(msg)
#define _LOG_S_FATAL_SYN(msg)    _LOG_S(_PARAM_FATAL, F_DETAIL|F_SYNC, msg); _LOG_WAIT()

//TLOG_XXX("%s", msg)
#ifdef _windows_
#define _TLOG_FATAL(fmt,...) _LOG(_PARAM_FATAL, F_TMSTMP|F_SYNC, fmt, ##__VA_ARGS__); _LOG_WAIT(); std::abort()
#define _TLOG_ERROR(fmt,...) _LOG(_PARAM_ERROR, F_TMSTMP,        fmt, ##__VA_ARGS__)
#define _TLOG_WARN(fmt,...)	 _LOG(_PARAM_WARN,  F_TMSTMP,        fmt, ##__VA_ARGS__)
#define _TLOG_INFO(fmt,...)	 _LOG(_PARAM_INFO,  F_TMSTMP,        fmt, ##__VA_ARGS__)
#define _TLOG_TRACE(fmt,...) _LOG(_PARAM_TRACE, F_TMSTMP,        fmt, ##__VA_ARGS__)
#define _TLOG_DEBUG(fmt,...) _LOG(_PARAM_DEBUG, F_TMSTMP,        fmt, ##__VA_ARGS__)
#else
#define _TLOG_FATAL(args...)  _LOG(_PARAM_FATAL, F_TMSTMP|F_SYNC, ##args); _LOG_WAIT(); std::abort()
#define _TLOG_ERROR(args...)  _LOG(_PARAM_ERROR, F_TMSTMP,        ##args)
#define _TLOG_WARN(args...)   _LOG(_PARAM_WARN,  F_TMSTMP,        ##args)
#define _TLOG_INFO(args...)	  _LOG(_PARAM_INFO,  F_TMSTMP,        ##args)
#define _TLOG_TRACE(args...)  _LOG(_PARAM_TRACE, F_TMSTMP,        ##args)
#define _TLOG_DEBUG(args...)  _LOG(_PARAM_DEBUG, F_TMSTMP,        ##args)
#endif

//TLOG_S_XXX(msg)
#define _TLOG_S_FATAL(msg)     _LOG_S(_PARAM_FATAL, F_TMSTMP|F_SYNC, msg); _LOG_WAIT(); std::abort()
#define _TLOG_S_ERROR(msg)     _LOG_S(_PARAM_ERROR, F_TMSTMP,        msg)
#define _TLOG_S_WARN(msg)      _LOG_S(_PARAM_WARN,  F_TMSTMP,        msg)
#define _TLOG_S_INFO(msg)      _LOG_S(_PARAM_INFO,  F_TMSTMP,        msg)
#define _TLOG_S_TRACE(msg)     _LOG_S(_PARAM_TRACE, F_TMSTMP,        msg)
#define _TLOG_S_DEBUG(msg)     _LOG_S(_PARAM_DEBUG, F_TMSTMP,        msg)

//TLOG_XXX("%s", msg)
#ifdef _windows_
#define _TLOG_FATAL_SYN(fmt,...)  _LOG(_PARAM_FATAL, F_TMSTMP|F_SYNC, fmt, ##__VA_ARGS__); _LOG_WAIT()
#else
#define _TLOG_FATAL_SYN(args...)  _LOG(_PARAM_FATAL, F_TMSTMP|F_SYNC, ##args); _LOG_WAIT()
#endif

//TLOG_S_XXX(msg)
#define _TLOG_S_FATAL_SYN(msg)    _LOG_S(_PARAM_FATAL, F_TMSTMP|F_SYNC, msg); _LOG_WAIT()

//PLOG_XXX("%s", msg)
#ifdef _windows_
#define _PLOG_FATAL(fmt,...)  _LOG(_PARAM_FATAL, F_SYNC, fmt, ##__VA_ARGS__); _LOG_WAIT(); std::abort()
#define _PLOG_ERROR(fmt,...)  _LOG(_PARAM_ERROR, F_PURE, fmt, ##__VA_ARGS__)
#define _PLOG_WARN(fmt,...)   _LOG(_PARAM_WARN,  F_PURE, fmt, ##__VA_ARGS__)
#define _PLOG_INFO(fmt,...)   _LOG(_PARAM_INFO,  F_PURE, fmt, ##__VA_ARGS__)
#define _PLOG_TRACE(fmt,...)  _LOG(_PARAM_TRACE, F_PURE, fmt, ##__VA_ARGS__)
#define _PLOG_DEBUG(fmt,...)  _LOG(_PARAM_DEBUG, F_PURE, fmt, ##__VA_ARGS__)
#else
#define _PLOG_FATAL(args...)  _LOG(_PARAM_FATAL, F_SYNC, ##args); _LOG_WAIT(); std::abort()
#define _PLOG_ERROR(args...)  _LOG(_PARAM_ERROR, F_PURE, ##args)
#define _PLOG_WARN(args...)   _LOG(_PARAM_WARN,  F_PURE, ##args)
#define _PLOG_INFO(args...)   _LOG(_PARAM_INFO,  F_PURE, ##args)
#define _PLOG_TRACE(args...)  _LOG(_PARAM_TRACE, F_PURE, ##args)
#define _PLOG_DEBUG(args...)  _LOG(_PARAM_DEBUG, F_PURE, ##args)
#endif

//PLOG_S_XXX(msg)
#define _PLOG_S_FATAL(msg)     _LOG_S(_PARAM_FATAL, F_SYNC, msg); _LOG_WAIT(); std::abort()
#define _PLOG_S_ERROR(msg)     _LOG_S(_PARAM_ERROR, F_PURE, msg)
#define _PLOG_S_WARN(msg)      _LOG_S(_PARAM_WARN,  F_PURE, msg)
#define _PLOG_S_INFO(msg)      _LOG_S(_PARAM_INFO,  F_PURE, msg)
#define _PLOG_S_TRACE(msg)     _LOG_S(_PARAM_TRACE, F_PURE, msg)
#define _PLOG_S_DEBUG(msg)     _LOG_S(_PARAM_DEBUG, F_PURE, msg)

//PLOG_XXX("%s", msg)
#ifdef _windows_
#define _PLOG_FATAL_SYN(fmt,...)  _LOG(_PARAM_FATAL, F_SYNC, fmt, ##__VA_ARGS__); _LOG_WAIT()
#else
#define _PLOG_FATAL_SYN(args...)  _LOG(_PARAM_FATAL, F_SYNC, ##args); _LOG_WAIT()
#endif

//PLOG_S_XXX(msg)
#define _PLOG_S_FATAL_SYN(msg)    _LOG_S(_PARAM_FATAL, F_SYNC, msg); _LOG_WAIT()