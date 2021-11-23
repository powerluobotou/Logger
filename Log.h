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
#include <time.h>

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

#define LVL_FATAL       0
#define LVL_ERROR       1
#define LVL_WARN        2
#define LVL_INFO        3
#define LVL_TRACE       4
#define LVL_DEBUG       5

#define PARAM_FATAL     0,__FILE__,__LINE__,__FUNC__
#define PARAM_ERROR     1,__FILE__,__LINE__,__FUNC__
#define PARAM_WARN      2,__FILE__,__LINE__,__FUNC__
#define PARAM_INFO      3,__FILE__,__LINE__,__FUNC__
#define PARAM_TRACE     4,__FILE__,__LINE__,__FUNC__
#define PARAM_DEBUG     5,__FILE__,__LINE__,__FUNC__

typedef int pid_t;
typedef int tid_t;

namespace LOGGER {

	class Logger {
	public:
		Logger();
		~Logger();
		static Logger* instance();
		void set_level(int level);
		char const* get_level();
		void init(char const* dir, int level, char const* prename = NULL, size_t logsize = 100000000);
		void write(int level, char const* file, int line, char const* func, char const* fmt, ...);
		void write_s(int level, char const* file, int line, char const* func, std::string const& msg);
	private:
		void open(char const* path);
		void write(char const* msg, size_t len);
		void close();
		void shift(struct tm const& tm, struct timeval const& tv);
		void update(struct tm& tm, struct timeval& tv);
		void get(struct tm& tm, struct timeval& tv);
		void stdout_stream(int level, char const* msg, size_t len);
	private:
		void start();
		void notify(char const* msg);
		void consume(struct tm const& tm, struct timeval const& tv);
		void stop();
	private:
#ifdef _windows_
		HANDLE fd_ = INVALID_HANDLE_VALUE;
#else
		int fd_ = INVALID_HANDLE_VALUE;
#endif
		pid_t pid_ = 0;
	private:
		int day_ = -1;
		size_t size_ = 0;
		std::atomic<int> level_ = LVL_DEBUG;
	private:
		char prefix_[256] = { 0 };
		char path_[512] = { 0 };
	private:
		struct timeval tv_ = { 0 };
		struct tm tm_ = { 0 };
		mutable std::shared_mutex tm_mutex_;
	private:
		std::mutex mutex_;
		std::condition_variable cond_;
		std::vector<std::string> messages_;
		std::atomic_bool done_ = false;
		std::atomic_flag started_ = ATOMIC_FLAG_INIT;
		std::thread thread_;
	};
}

#define LOG_INIT LOGGER::Logger::instance()->init
#define LOG LOGGER::Logger::instance()->write
#define LOG_S LOGGER::Logger::instance()->write_s
#define LOG_SET LOGGER::Logger::instance()->set_level

#define LOG_SET_FATAL       LOG_SET(LVL_FATAL)
#define LOG_SET_ERROR       LOG_SET(LVL_ERROR)
#define LOG_SET_WARN        LOG_SET(LVL_WARN)
#define LOG_SET_INFO        LOG_SET(LVL_INFO)
#define LOG_SET_TRACE       LOG_SET(LVL_TRACE)
#define LOG_SET_DEBUG       LOG_SET(LVL_DEBUG)

//LOG_XXX("%s", msg)
#ifdef _windows_
#define LOG_FATAL(fmt,...)	LOG(PARAM_FATAL, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt,...)	LOG(PARAM_ERROR, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt,...)	LOG(PARAM_WARN,  fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt,...)	LOG(PARAM_INFO,  fmt, ##__VA_ARGS__)
#define LOG_TRACE(fmt,...)	LOG(PARAM_TRACE, fmt, ##__VA_ARGS__)
#define LOG_DEBUG(fmt,...)	LOG(PARAM_DEBUG, fmt, ##__VA_ARGS__)
#else
#define LOG_FATAL(args...) 	LOG(PARAM_FATAL, ##args)
#define LOG_ERROR(args...) 	LOG(PARAM_ERROR, ##args)
#define LOG_WARN(args...) 	LOG(PARAM_WARN, ##args)
#define LOG_INFO(args...)	LOG(PARAM_INFO, ##args)
#define LOG_TRACE(args...)	LOG(PARAM_TRACE, ##args)
#define LOG_DEBUG(args...) 	LOG(PARAM_DEBUG, ##args)
#endif

//LOG_S_XXX(msg)
#define LOG_S_FATAL(msg)    LOG_S(PARAM_FATAL, msg)
#define LOG_S_ERROR(msg)    LOG_S(PARAM_ERROR, msg)
#define LOG_S_WARN(msg)     LOG_S(PARAM_WARN,  msg)
#define LOG_S_INFO(msg)     LOG_S(PARAM_INFO,  msg)
#define LOG_S_TRACE(msg)    LOG_S(PARAM_TRACE, msg)
#define LOG_S_DEBUG(msg)    LOG_S(PARAM_DEBUG, msg)

#endif
