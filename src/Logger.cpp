/**
*
*   异步日志系统实现
*	Created by andy_ro@qq.com 2021.11.17
*
*/
#include "../Logger.h"
#include "Log.h"
#include "../utils.h"

namespace LOGGER {

	//set_timezone
	void Logger::set_timezone(int64_t timezone/* = MY_CCT*/) {
		LOGGER::Log::instance()->set_timezone(timezone);
	}

	//set_level
	void Logger::set_level(int level) {
		LOGGER::Log::instance()->set_level(level);
	}

	//get_level
	char const* Logger::get_level() {
		return LOGGER::Log::instance()->get_level();
	}

	//set_color
	void Logger::set_color(int level, int title, int text) {
		LOGGER::Log::instance()->set_color(level, title, text);
	}

	//init
	void Logger::init(char const* dir, int level, char const* prename, size_t logsize) {
		LOGGER::Log::instance()->init(dir, level, prename, logsize);
	}

	//write
	void Logger::write(int level, char const* file, int line, char const* func, char const* stack, uint8_t flag, char const* fmt, ...) {
		if (LOGGER::Log::instance()->check(level)) {
			static size_t const PATHSZ = 512;
			static size_t const MAXSZ = 81920;
			char msg[PATHSZ + MAXSZ + 2];
			size_t pos = LOGGER::Log::instance()->format(level, file, line, func, flag, msg, PATHSZ);
			va_list ap;
			va_start(ap, fmt);
#ifdef _windows_
			size_t n = vsnprintf_s(msg + pos, MAXSZ, _TRUNCATE, fmt, ap);
#else
			size_t n = vsnprintf(msg + pos, MAXSZ, fmt, ap);
#endif
			va_end(ap);
			msg[pos + n] = '\n';
			msg[pos + n + 1] = '\0';
			if (LOGGER::Log::instance()->started()) {
				LOGGER::Log::instance()->notify(msg, pos + n + 1, pos, flag, stack, stack ? strlen(stack) : 0);
			}
			else {
				LOGGER::Log::instance()->stdoutbuf(level, msg, pos + n + 1, pos, flag, stack, stack ? strlen(stack) : 0);
				LOGGER::Log::instance()->checkSync(flag);
			}
		}
	}

	//write_s
	void Logger::write_s(int level, char const* file, int line, char const* func, char const* stack, uint8_t flag, std::string const& msg) {
		write(level, file, line, func, stack, flag, "%s", msg.c_str());
	}

	//wait
	void Logger::wait() {
		LOGGER::Log::instance()->wait();
	}

	//enable
	void Logger::enable() {
		LOGGER::Log::instance()->enable();
	}

	//disable
	void Logger::disable() {
		LOGGER::Log::instance()->disable();
	}

	//cleanup
	void Logger::cleanup() {
		LOGGER::Log::instance()->stop();
	}
}

/*
int main() {
	//LOG_INIT(".", LVL_DEBUG, "test");
	LOG_SET_DEBUG;
	while (1) {
		for (int i = 0; i < 200000; ++i) {
			LOG_ERROR("Hi%d", i);
		}
	}
	LOG_FATAL("崩溃吧");
	xsleep(1000);
	return 0;
}
*/