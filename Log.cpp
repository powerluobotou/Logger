/**
* 
*   异步日志系统实现
*	Created by andy_ro@qq.com 2021.11.17
* 
*/
#include "Log.h"
#include "utils.h"

#ifdef _windows_
#include <process.h>
#endif

#ifdef QT_SUPPORTS
#include <QDebug>
#endif

namespace LOGGER {

	//constructor
	Logger::Logger() :pid_(_getpid()) {
	};

	//destructor
	Logger::~Logger() {
		stop();
	}

	//instance
	Logger* Logger::instance() {
		static Logger logger;
		return &logger;
	}
	
	//set_timezone
	void Logger::set_timezone(int64_t timezone/* = MY_CCT*/) {
		timezone_ = timezone;
	}
	
	//update
	void Logger::update(struct tm& tm, struct timeval& tv) {
		{
			std::unique_lock<std::shared_mutex> lock(tm_mutex_); {
				gettimeofday(&tv_, NULL);
				time_t t = tv_.tv_sec;
				utils::convertUTC(t, tm_, NULL, timezone_);
				tm = tm_;
				tv = tv_;
			}
		}
	}

	//get
	void Logger::get(struct tm& tm, struct timeval& tv) {
		{
			std::shared_lock<std::shared_mutex> lock(tm_mutex_); {
				tm = tm_;
				tv = tv_;
			}
		}
	}

	//set_level
	void Logger::set_level(int level) {
		if (level >= LVL_DEBUG) {
			level_.store(LVL_DEBUG);
		}
		else if (level <= LVL_FATAL) {
			level_.store(LVL_FATAL);
		}
		else {
			level_.store(level);
		}
	}

	//get_level
	char const* Logger::get_level() {
		switch (level_.load()) {
		case LVL_FATAL: return "LVL_FATAL";
		case LVL_ERROR: return "LVL_ERROR";
		case LVL_WARN: return "LVL_WARN";
		case LVL_INFO: return "LVL_INFO";
		case LVL_TRACE: return "LVL_TRACE";
		case LVL_DEBUG: return "LVL_DEBUG";
		}
		return "";
	}

	//init
	void Logger::init(char const* dir, int level, char const* prename, size_t logsize) {
#if 1
		struct stat stStat;
		if (stat(dir, &stStat) < 0) {
#else
		if (access(dir, 0) < 0) {
#endif
#ifdef _windows_
			if (_mkdir(dir) < 0) {
				return;
			}
#else
			if (mkdir(dir, /*0777*/S_IRWXU | S_IRWXG | S_IRWXO) < 0) {
				return;
			}
#endif
		}
		if (start()) {
			//打印level_及以下级别日志
			level_.store(level);
			size_ = logsize;
			prename ?
				snprintf(prefix_, sizeof(prefix_), "%s/%s_", (dir ? dir : "."), prename) :
				snprintf(prefix_, sizeof(prefix_), "%s/", (dir ? dir : "."));
			timezoneinfo();
		}
	}
	
	//timezoneinfo
	void Logger::timezoneinfo() {
		struct tm tm = { 0 };
		utils::convertUTC(time(NULL), tm, NULL, timezone_);
		utils::timezoneInfo(tm, timezone_);
	}

	//write
	void Logger::write(int level, char const* file, int line, char const* func, char const* stack, char const* fmt, ...) {
		//打印level_及以下级别日志
		if (level > level_.load()) {
			return;
		}
		struct tm tm;
		struct timeval tv;
		update(tm, tv);
		static size_t const PATHSZ = 512;
		static size_t const MAXSZ = 81920;
		char msg[PATHSZ + MAXSZ + 2];
		size_t pos = 0, n;
		switch (level) {
		case LVL_FATAL:
			pos = snprintf(msg, PATHSZ, "F%d %02d:%02d:%02d.%.6lu %s %s:%d] %s ",
				pid_,
				tm.tm_hour, tm.tm_min, tm.tm_sec, (unsigned long)tv.tv_usec,
				utils::gettid().c_str(),
				utils::trim_file(file).c_str(), line, utils::trim_func(func).c_str());
			break;
		case LVL_ERROR:
			pos = snprintf(msg, PATHSZ, "E%d %02d:%02d:%02d.%.6lu %s %s:%d] %s ",
				pid_,
				tm.tm_hour, tm.tm_min, tm.tm_sec, (unsigned long)tv.tv_usec,
				utils::gettid().c_str(),
				utils::trim_file(file).c_str(), line, utils::trim_func(func).c_str());
			break;
		case LVL_WARN:
			pos = snprintf(msg, PATHSZ, "W%d %02d:%02d:%02d.%.6lu %s %s:%d] %s ",
				pid_,
				tm.tm_hour, tm.tm_min, tm.tm_sec, (unsigned long)tv.tv_usec,
				utils::gettid().c_str(),
				utils::trim_file(file).c_str(), line, utils::trim_func(func).c_str());
			break;
		case LVL_INFO:
			pos = snprintf(msg, PATHSZ, "I%d %02d:%02d:%02d.%.6lu %s %s:%d] %s ",
				pid_,
				tm.tm_hour, tm.tm_min, tm.tm_sec, (unsigned long)tv.tv_usec,
				utils::gettid().c_str(),
				utils::trim_file(file).c_str(), line, utils::trim_func(func).c_str());
			break;
		case LVL_DEBUG:
			pos = snprintf(msg, PATHSZ, "D%d %02d:%02d:%02d.%.6lu %s %s:%d] %s ",
				pid_,
				tm.tm_hour, tm.tm_min, tm.tm_sec, (unsigned long)tv.tv_usec,
				utils::gettid().c_str(),
				utils::trim_file(file).c_str(), line, utils::trim_func(func).c_str());
			break;
		case LVL_TRACE:
			pos = snprintf(msg, PATHSZ, "T%d %02d:%02d:%02d.%.6lu %s %s:%d] %s ",
				pid_,
				tm.tm_hour, tm.tm_min, tm.tm_sec, (unsigned long)tv.tv_usec,
				utils::gettid().c_str(),
				utils::trim_file(file).c_str(), line, utils::trim_func(func).c_str());
			break;
		default:
			return;
		}
		va_list ap;
		va_start(ap, fmt);
#ifdef _windows_
		n = vsnprintf_s(msg + pos, MAXSZ, _TRUNCATE, fmt, ap);
#else
		n = vsnprintf(msg + pos, MAXSZ, fmt, ap);
#endif
		va_end(ap);
#if 0
		pos = strlen(msg);
#else
		pos += n;
#endif
		msg[pos] = '\n';
		msg[pos + 1] = '\0';
		if (prefix_[0]) {
			notify(msg, stack);
		}
		else {
			stdout_stream(msg, pos + 1);
			if (stack) {
				backtraceF(stack, strlen(stack), true);
			}
		}
	}

	//write_s
	void Logger::write_s(int level, char const* file, int line, char const* func, char const* stack, std::string const& msg) {
		write(level, file, line, func, stack, "%s", msg.c_str());
	}

	//open
	void Logger::open(char const* path) {
		assert(path);
#ifdef _windows_
		fd_ = CreateFileA(path, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (INVALID_HANDLE_VALUE == fd_) {
			int err = GetLastError();
			std::string errmsg = utils::str_error(err);
			fprintf(stderr, "open %s error[%d:%s]\n", path, err, errmsg.c_str());
		}
		else {
			SetFilePointer(fd_, 0, NULL, FILE_END);
		}
#else
		fd_ = open(path, O_WRONLY | O_CREAT | O_APPEND | O_LARGEFILE, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		if (fd_ == INVALID_HANDLE_VALUE) {
			std::string errmsg = utils::str_error(errno);
			fprintf(stderr, "open %s error[%d:%s]\n", path, errno, errmsg.c_str());
	    }
#endif
	}

	//write
	void Logger::write(char const* msg, size_t len) {
#ifdef _windows_
		if (fd_ != INVALID_HANDLE_VALUE) {
			long size = 0;
			WriteFile(fd_, msg, len, (LPDWORD)&size, NULL);
		}
#else
		if (fd_ != INVALID_HANDLE_VALUE) {
			(void)write(fd_, msg, len);
		}
#endif
	}

	//close
	void Logger::close() {
#ifdef _windows_
		if (INVALID_HANDLE_VALUE != fd_) {
			CloseHandle(fd_);
			fd_ = INVALID_HANDLE_VALUE;
		}
#else
		if (INVALID_HANDLE_VALUE != fd_) {
			close(fd_);
			fd_ = INVALID_HANDLE_VALUE;
		}
#endif
	}

	//shift
	void Logger::shift(struct tm const& tm, struct timeval const& tv) {
		assert(prefix_[0]);
		if (tm.tm_mday != day_) {
			close();
			snprintf(path_, sizeof(path_), "%s%d_%04d-%02d-%02d.log",
				prefix_, pid_, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
			open(path_);
			day_ = tm.tm_mday;
		}
		else {
			struct stat stStat;
			if (stat(path_, &stStat) < 0) {
				return;
			}
			if (stStat.st_size < size_) {
			}
			else {
				close();
				char tmp[512];
				snprintf(tmp, sizeof(tmp), "%s%d_%04d-%02d-%02d.%02d%02d%02d.log",
					prefix_, pid_, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

				if (stat(tmp, &stStat) == 0) {
					snprintf(tmp, sizeof(tmp), "%s%d_%04d-%02d-%02d.%02d%02d%02d.%.6lu.log",
						prefix_, pid_, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, (unsigned long)tv.tv_usec);
					printf("newFile: %s\n", tmp);
				}
				if (rename(path_, tmp) < 0) {
					return;
				}
				open(path_);
				day_ = tm.tm_mday;
			}
		}
	}

	//start
	bool Logger::start() {
		if (!starting_.test_and_set() && !started_) {
			thread_ = std::thread([this](Logger* logger) {
				struct tm tm = { 0 };
				struct timeval tv = { 0 };
				bool abort_ = false;
				while (!done_.load()) {
					{
						std::unique_lock<std::mutex> lock(mutex_); {
							cond_.wait(lock); {
								get(tm, tv);
								if ((abort_ = consume(tm, tv))) {
									done_.store(true);
									break;
								}
							}
						}
					}
					std::this_thread::yield();
				}
				close();
				started_ = false;
				if (abort_) {
					abort();
				}
			}, this);
			if ((started_ = valid())) {
				thread_.detach();
			}
			starting_.clear();
		}
		return started_;
	}

	//valid
	bool Logger::valid() {
		//detach()后get_id()/joinable()无效
#if 0
		std::ostringstream oss;
		oss << thread_.get_id();
		return oss.str() != "0";
#else
		return thread_.joinable();
#endif
	}

	//notify
	void Logger::notify(char const* msg, char const* stack) {
		{
			std::unique_lock<std::mutex> lock(mutex_); {
				messages_.emplace_back(msg);
				if (stack) {
					messages_.emplace_back(stack);
				}
				cond_.notify_all();
			}
		}
		std::this_thread::yield();
	}

	//getlevel
	static int getlevel(char const c) {
		switch (c) {
		case 'F': return LVL_FATAL;
		case 'E':return LVL_ERROR;
		case 'W':return LVL_WARN;
		case 'I':return LVL_INFO;
		case 'T':return LVL_TRACE;
		case 'D':return LVL_DEBUG;
		default:
			return -1;
		}
	}

	//consume
	bool Logger::consume(struct tm const& tm, struct timeval const& tv) {
		if (!messages_.empty()) {
			shift(tm, tv);
			bool abort_ = false;
			for (std::vector<std::string>::const_iterator it = messages_.begin();
				it != messages_.end(); ++it) {
				if (getlevel(it->c_str()[0]) == -1 &&
					(abort_ = backtraceF(it->c_str(), it->size()))) {
					break;
				}
				else {
					write(it->c_str(), it->size());
					stdout_stream(it->c_str(), it->size());
				}
			}
			messages_.clear();
			return abort_;
		}
		return false;
	}

	//stop
	void Logger::stop() {
		done_.store(true);
		if (thread_.joinable()) {
			thread_.join();
		}
	}

	//stdout_stream
	void Logger::stdout_stream(char const* msg, size_t len) {
#ifdef QT_SUPPORTS
		int level = getlevel(msg[0]);
		switch (level) {
		default:
		case LVL_FATAL: qInfo/*qFatal*/() << msg; break;
		case LVL_ERROR: qCritical() << msg; break;
		case LVL_WARN: qWarning() << msg; break;
		case LVL_INFO: qInfo() << msg; break;
		case LVL_TRACE: qInfo() << msg; break;
		case LVL_DEBUG: qDebug() << msg; break;
		}
#else
		//需要调用utils::initConsole()初始化
		if (utils::is_utf8(msg, len)) {
			std::string s(utils::utf82GBK(msg, len));
			printf("%.*s", s.length(), s.c_str());
		}
		else {
			printf("%.*s", len, msg);
		}
#endif
	}

	//backtraceF
	bool Logger::backtraceF(char const* stack, size_t len, bool abort_) {
		if (stack) {
			write(stack, len);
			stdout_stream(stack, len);
			if (abort_) {
				abort();
			}
			return true;
		}
		return false;
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
