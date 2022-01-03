/**
*
*   异步日志系统实现
*	Created by andy_ro@qq.com 2021.11.17
*
*/
#include "../Logger.h"
#include "Log.h"
#include "../utils.h"
#include "auth.h"

namespace LOGGER {

	//set_timezone
	void Logger::set_timezone(int64_t timezone/* = MY_CCT*/) {
		AUTHORIZATION_CHECK;
		_LOG_TIMEZONE(timezone);
	}

	//set_level
	void Logger::set_level(int level) {
		AUTHORIZATION_CHECK;
		_LOG_SET(level);
	}

	//get_level
	char const* Logger::get_level() {
		AUTHORIZATION_CHECK_P;
		return _LOG_LVL();
	}

	//set_color
	void Logger::set_color(int level, int title, int text) {
		AUTHORIZATION_CHECK;
		_LOG_COLOR(level, title, text);
	}

	//init
	void Logger::init(char const* dir, int level, char const* prename, size_t logsize) {
		AUTHORIZATION_CHECK;
		_LOG_INIT(dir, level, prename, logsize);
	}

	//write
	void Logger::write(int level, char const* file, int line, char const* func, char const* stack, uint8_t flag, char const* fmt, ...) {
		AUTHORIZATION_CHECK;
		if (_LOG_CHECK(level)) {
			static size_t const PATHSZ = 512;
			static size_t const MAXSZ = 81920;
			char msg[PATHSZ + MAXSZ + 2];
			size_t pos = _LOG_FORMAT(level, file, line, func, flag, msg, PATHSZ);
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
			if (_LOG_STARTED()) {
				_LOG_CHECK_NOTIFY(msg, pos + n + 1, pos, flag, stack, stack ? strlen(stack) : 0);
			}
			else {
				_LOG_CHECK_STDOUT(level, msg, pos + n + 1, pos, flag, stack, stack ? strlen(stack) : 0);
				_LOG_CHECK_SYNC(flag);
			}
		}
	}

	//write_s
	void Logger::write_s(int level, char const* file, int line, char const* func, char const* stack, uint8_t flag, std::string const& msg) {
		AUTHORIZATION_CHECK;
		write(level, file, line, func, stack, flag, "%s", msg.c_str());
	}

	//wait
	void Logger::wait() {
		AUTHORIZATION_CHECK;
		_LOG_WAIT();
	}

	//enable
	void Logger::enable() {
		AUTHORIZATION_CHECK;
		_LOG_CONSOLE_OPEN();
	}

	//disable
	void Logger::disable() {
		AUTHORIZATION_CHECK;
		_LOG_CONSOLE_CLOSE();
	}

	//cleanup
	void Logger::cleanup() {
		AUTHORIZATION_CHECK;
		_LOG_STOP();
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