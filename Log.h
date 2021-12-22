/**
*
*   异步日志系统实现
*	Created by andy_ro@qq.com 2021.11.17
*
*/
#ifndef LOG_H_INCLUDE
#define LOG_H_INCLUDE

#include "Macro.h"

#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>

#ifdef _windows_
#include <io.h>
#include <direct.h>
#include "gettimeofday.h"
#else
#include <sys/time.h>
#include <dirent.h>
#endif

#include <mutex>
#include <shared_mutex>
#include <thread>
#include <condition_variable>

#include "Atomic.h"

#pragma execution_character_set("utf-8")

#define __STACK__       utils::stack_backtrace().c_str()

#define LVL_FATAL       0
#define LVL_ERROR       1
#define LVL_WARN        2
#define LVL_INFO        3
#define LVL_TRACE       4
#define LVL_DEBUG       5

#define PARAM_FATAL     0,__FILE__,__LINE__,__FUNC__,__STACK__
#define PARAM_ERROR     1,__FILE__,__LINE__,__FUNC__,NULL
#define PARAM_WARN      2,__FILE__,__LINE__,__FUNC__,NULL
#define PARAM_INFO      3,__FILE__,__LINE__,__FUNC__,NULL
#define PARAM_TRACE     4,__FILE__,__LINE__,__FUNC__,__STACK__
#define PARAM_DEBUG     5,__FILE__,__LINE__,__FUNC__,NULL

typedef int pid_t;
typedef int tid_t;

namespace LOGGER {

	class Logger {
	private:
		Logger();
		~Logger();
	public:
		static Logger* instance();
		void set_timezone(int64_t timezone = MY_CCT);
		void set_level(int level);
		char const* get_level();
		void set_color(int level, int title, int text);
		void init(char const* dir, int level, char const* prename = NULL, size_t logsize = 100000000);
		void write(int level, char const* file, int line, char const* func, char const* stack, bool syn, char const* fmt, ...);
		void write_s(int level, char const* file, int line, char const* func, char const* stack, bool syn, std::string const& msg);
		void wait();
	private:
		void open(char const* path);
		void write(char const* msg, size_t len);
		void close();
		void shift(struct tm const& tm, struct timeval const& tv);
		void update(struct tm& tm, struct timeval& tv);
		void get(struct tm& tm, struct timeval& tv);
		void stdoutbuf(int level, char const* msg, size_t len, size_t pos, char const* stack = NULL, size_t stacklen = 0);
		bool start();
		bool valid();
		void notify(char const* msg, size_t len, size_t pos, char const* stack, size_t stacklen, bool syn = false);
		bool consume(struct tm const& tm, struct timeval const& tv);
		void sync();
		void stop();
		void timezoneinfo();
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
		typedef std::pair<size_t, bool> Flags;
		typedef std::pair<std::string, std::string> Message;
		typedef std::pair<Message, Flags> MessageT;
		std::vector<MessageT> messages_;
		bool sync_ = false;
		std::mutex sync_mutex_;
		std::condition_variable sync_cond_;
	};
}

#define LOG_INIT LOGGER::Logger::instance()->init
#define LOG LOGGER::Logger::instance()->write
#define LOG_S LOGGER::Logger::instance()->write_s
#define LOG_SET LOGGER::Logger::instance()->set_level
#define LOG_TIMEZONE LOGGER::Logger::instance()->set_timezone
#define LOG_WAIT LOGGER::Logger::instance()->wait
#define LOG_COLOR LOGGER::Logger::instance()->set_color

#define LOG_SET_FATAL        LOG_SET(LVL_FATAL)
#define LOG_SET_ERROR        LOG_SET(LVL_ERROR)
#define LOG_SET_WARN         LOG_SET(LVL_WARN)
#define LOG_SET_INFO         LOG_SET(LVL_INFO)
#define LOG_SET_TRACE        LOG_SET(LVL_TRACE)
#define LOG_SET_DEBUG        LOG_SET(LVL_DEBUG)

#define LOG_COLOR_FATAL(a,b) LOG_COLOR(LVL_FATAL, a, b)
#define LOG_COLOR_ERROR(a,b) LOG_COLOR(LVL_ERROR, a, b)
#define LOG_COLOR_WARN(a,b)  LOG_COLOR(LVL_WARN,  a, b)
#define LOG_COLOR_INFO(a,b)  LOG_COLOR(LVL_INFO,  a, b)
#define LOG_COLOR_TRACE(a,b) LOG_COLOR(LVL_TRACE, a, b)
#define LOG_COLOR_DEBUG(a,b) LOG_COLOR(LVL_DEBUG, a, b)

//LOG_XXX("%s", msg)
#ifdef _windows_
#define LOG_FATAL(fmt,...)	 LOG(PARAM_FATAL, true,  fmt, ##__VA_ARGS__); LOG_WAIT(); abort();
#define LOG_ERROR(fmt,...)	 LOG(PARAM_ERROR, false, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt,...)	 LOG(PARAM_WARN,  false, fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt,...)	 LOG(PARAM_INFO,  false, fmt, ##__VA_ARGS__)
#define LOG_TRACE(fmt,...)	 LOG(PARAM_TRACE, false, fmt, ##__VA_ARGS__)
#define LOG_DEBUG(fmt,...)	 LOG(PARAM_DEBUG, false, fmt, ##__VA_ARGS__)
#else
#define LOG_FATAL(args...) 	 LOG(PARAM_FATAL, true,  ##args); LOG_WAIT(); abort();
#define LOG_ERROR(args...) 	 LOG(PARAM_ERROR, false, ##args)
#define LOG_WARN(args...) 	 LOG(PARAM_WARN,  false, ##args)
#define LOG_INFO(args...)	 LOG(PARAM_INFO,  false, ##args)
#define LOG_TRACE(args...)	 LOG(PARAM_TRACE, false, ##args)
#define LOG_DEBUG(args...) 	 LOG(PARAM_DEBUG, false, ##args)
#endif

//LOG_S_XXX(msg)
#define LOG_S_FATAL(msg)     LOG_S(PARAM_FATAL, true,  msg); LOG_WAIT(); abort();
#define LOG_S_ERROR(msg)     LOG_S(PARAM_ERROR, false, msg)
#define LOG_S_WARN(msg)      LOG_S(PARAM_WARN,  false, msg)
#define LOG_S_INFO(msg)      LOG_S(PARAM_INFO,  false, msg)
#define LOG_S_TRACE(msg)     LOG_S(PARAM_TRACE, false, msg)
#define LOG_S_DEBUG(msg)     LOG_S(PARAM_DEBUG, false, msg)

//LOG_XXX("%s", msg)
#ifdef _windows_
#define LOG_FATAL_SYN(fmt,...)	 LOG(PARAM_FATAL, true, fmt, ##__VA_ARGS__); LOG_WAIT();
#define LOG_ERROR_SYN(fmt,...)	 LOG(PARAM_ERROR, true, fmt, ##__VA_ARGS__); LOG_WAIT();
#define LOG_WARN_SYN(fmt,...)	 LOG(PARAM_WARN,  true, fmt, ##__VA_ARGS__); LOG_WAIT();
#define LOG_INFO_SYN(fmt,...)	 LOG(PARAM_INFO,  true, fmt, ##__VA_ARGS__); LOG_WAIT();
#define LOG_TRACE_SYN(fmt,...)	 LOG(PARAM_TRACE, true, fmt, ##__VA_ARGS__); LOG_WAIT();
#define LOG_DEBUG_SYN(fmt,...)	 LOG(PARAM_DEBUG, true, fmt, ##__VA_ARGS__); LOG_WAIT();
#else
#define LOG_FATAL_SYN(args...) 	 LOG(PARAM_FATAL, true, ##args); LOG_WAIT();
#define LOG_ERROR_SYN(args...) 	 LOG(PARAM_ERROR, true, ##args); LOG_WAIT();
#define LOG_WARN_SYN(args...) 	 LOG(PARAM_WARN,  true, ##args); LOG_WAIT();
#define LOG_INFO_SYN(args...)	 LOG(PARAM_INFO,  true, ##args); LOG_WAIT();
#define LOG_TRACE_SYN(args...)	 LOG(PARAM_TRACE, true, ##args); LOG_WAIT();
#define LOG_DEBUG_SYN(args...) 	 LOG(PARAM_DEBUG, true, ##args); LOG_WAIT();
#endif

//LOG_S_XXX(msg)
#define LOG_S_FATAL_SYN(msg)     LOG_S(PARAM_FATAL, true, msg); LOG_WAIT();
#define LOG_S_ERROR_SYN(msg)     LOG_S(PARAM_ERROR, true, msg); LOG_WAIT();
#define LOG_S_WARN_SYN(msg)      LOG_S(PARAM_WARN,  true, msg); LOG_WAIT();
#define LOG_S_INFO_SYN(msg)      LOG_S(PARAM_INFO,  true, msg); LOG_WAIT();
#define LOG_S_TRACE_SYN(msg)     LOG_S(PARAM_TRACE, true, msg); LOG_WAIT();
#define LOG_S_DEBUG_SYN(msg)     LOG_S(PARAM_DEBUG, true, msg); LOG_WAIT();

#endif