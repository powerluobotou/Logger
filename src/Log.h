/**
*
*   异步日志系统实现
*	Created by andy_ro@qq.com 2021.11.17
*
*/
#ifndef LOG_H_INCLUDE
#define LOG_H_INCLUDE

#include "../Macro.h"

#include <mutex>
#include <shared_mutex>
#include <thread>
#include <condition_variable>

#include "Atomic.h"

#pragma execution_character_set("utf-8")

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

#endif