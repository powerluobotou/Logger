﻿/**
*
*   基于消息队列的异步日志系统
*	Created by 萝卜 2021.11.17
*
*/
#ifndef LOG_H_INCLUDE
#define LOG_H_INCLUDE

#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>

#include <stdio.h>
#if defined(WIN32) || defined(_WIN32)|| defined(WIN64) || defined(_WIN64)
#include <io.h>
#include <direct.h>
#include "gettimeofday.h"
#else
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#endif
#include <string>
#include <vector>
#include <map>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <shared_mutex>
#include <iostream>
#include <sstream>
#include <functional>
#include <atomic>
#include "Atomic.h"

#if defined(WIN32) || defined(_WIN32)|| defined(WIN64) || defined(_WIN64)
#include <windows.h>
#include <time.h>
#include <sys/timeb.h>
//#define __FUNC__ __FUNCSIG__//__FUNCTION__
#define __FUNC__ __FUNCTION__//__FUNCSIG__
#define INVALID_HANDLE_VALUE ((HANDLE)(-1))
#else
#include <unistd.h>
#include <time.h> 
#include <sys/time.h>
#define __FUNC__ __func__
#define INVALID_HANDLE_VALUE (-1)
#endif

#define read_lock(mutex) std::shared_lock<std::shared_mutex> lock(mutex)
#define write_lock(mutex) std::unique_lock<std::shared_mutex> lock(mutex)

#define LVL_FATAL	0
#define LVL_ERROR	1
#define LVL_WARN	2
#define LVL_INFO	3
#define LVL_TRACE	4
#define LVL_DEBUG	5

#define PARAM_FATAL 	0,__FILE__,__LINE__,__FUNC__
#define PARAM_ERROR 	1,__FILE__,__LINE__,__FUNC__
#define PARAM_WARN 		2,__FILE__,__LINE__,__FUNC__
#define PARAM_INFO		3,__FILE__,__LINE__,__FUNC__
#define PARAM_TRACE		4,__FILE__,__LINE__,__FUNC__
#define PARAM_DEBUG 	5,__FILE__,__LINE__,__FUNC__

typedef int pid_t;
typedef int tid_t;

namespace LOGGER {

	class Logger {
	public:
		Logger();
		~Logger();
		static Logger* instance() {
			static Logger logger;
			return &logger;
		}
		void init_log(char const* dir, int level, char const* pre_name = NULL, int log_size = 100000000);
		void write_log(int level, char const* file, int line, char const* func, char const* fmt, ...);
	private:
		void open_fd();
		void close_fd();
		void shift_fd(struct tm const& tm, struct timeval const& tv);
		void update(struct tm& tm, struct timeval& tv);
		void get(struct tm& tm, struct timeval& tv);
	private:
		void start();
		void notify(char const* msg);
		void consume(struct tm const& tm, struct timeval const& tv);
		void stop();
	private:
#if defined(WIN32) || defined(_WIN32)|| defined(WIN64) || defined(_WIN64)
		HANDLE fd_ = INVALID_HANDLE_VALUE;
#else
		int fd_ = INVALID_HANDLE_VALUE;
#endif
		pid_t pid_ = 0;
	private:
		int level_ = 0;
		int log_day_ = -1;
		//int log_num_ = 0;
		int log_size_ = 8192;
	private:
		char path_[512] = { 0 };
		char prefix_[256] = { 0 };
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

#define LOG_INIT LOGGER::Logger::instance()->init_log
#define LOG	LOGGER::Logger::instance()->write_log

#if defined(WIN32) || defined(_WIN32)|| defined(WIN64) || defined(_WIN64)
#define LOG_FATAL(fmt,...)	LOG(PARAM_FATAL, fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt,...)	LOG(PARAM_ERROR, fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt,...)	LOG(PARAM_WARN,  fmt, ##__VA_ARGS__)
#define LOG_INFO(fmt,...)	LOG(PARAM_INFO,  fmt, ##__VA_ARGS__)
#define LOG_DEBUG(fmt,...)	LOG(PARAM_DEBUG, fmt, ##__VA_ARGS__)
#define LOG_TRACE(fmt,...)	LOG(PARAM_TRACE, fmt, ##__VA_ARGS__)
#else
#define LOG_FATAL(args...) 	LOG(PARAM_FATAL, ##args)
#define LOG_ERROR(args...) 	LOG(PARAM_ERROR, ##args)
#define LOG_WARN(args...) 	LOG(PARAM_WARN, ##args)
#define LOG_INFO(args...)	LOG(PARAM_INFO, ##args)
#define LOG_DEBUG(args...) 	LOG(PARAM_DEBUG, ##args)
#define LOG_TRACE(args...)	LOG(PARAM_TRACE, ##args)
#endif

#endif