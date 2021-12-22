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

#ifdef QT_SUPPORT
#include <QDebug>
#endif

namespace LOGGER {

	//constructor
	Logger::Logger() :pid_(getpid()) {
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

	//set_color
	void Logger::set_color(int level, int clrtitle, int clrtext) {
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
			if (mkdir(dir) < 0) {
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
			(prename && prename[0]) ?
				snprintf(prefix_, sizeof(prefix_), "%s/%s ", ((dir && dir[0]) ? dir : "."), prename) :
				snprintf(prefix_, sizeof(prefix_), "%s/", ((dir && dir[0]) ? dir : "."));
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
	void Logger::write(int level, char const* file, int line, char const* func, char const* stack, bool syn, char const* fmt, ...) {
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
		size_t pos = 0;
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
		case LVL_TRACE:
			pos = snprintf(msg, PATHSZ, "T%d %02d:%02d:%02d.%.6lu %s %s:%d] %s ",
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
		default:
			return;
		}
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
		if (prefix_[0]) {
			notify(msg, pos + n + 1, pos, stack, stack ? strlen(stack) : 0, syn);
		}
		else {
			stdoutbuf(level, msg, pos + n + 1, pos, stack, stack ? strlen(stack) : 0);
			if (syn) {
				sync();
			}
		}
	}

	//write_s
	void Logger::write_s(int level, char const* file, int line, char const* func, char const* stack, bool syn, std::string const& msg) {
		write(level, file, line, func, stack, syn, "%s", msg.c_str());
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
			snprintf(path_, sizeof(path_), "%s%d %04d-%02d-%02d.log",
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
				snprintf(tmp, sizeof(tmp), "%s%d %04d-%02d-%02d %02d.%02d.%02d.log",
					prefix_, pid_, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
				if (stat(tmp, &stStat) < 0) {
				}
				else {//==0 existed
					snprintf(tmp, sizeof(tmp), "%s%d %04d-%02d-%02d %02d.%02d.%02d %.6lu.log",
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
				bool syn = false;
				while (!done_.load()) {
					{
						std::unique_lock<std::mutex> lock(mutex_); {
							cond_.wait(lock); {
								get(tm, tv);
								if ((syn = consume(tm, tv))) {
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
				if (syn) {
					sync();
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
	void Logger::notify(char const* msg, size_t len, size_t pos, char const* stack, size_t stacklen, bool syn) {
		{
			std::unique_lock<std::mutex> lock(mutex_); {
				messages_.emplace_back(
					std::make_pair(
						std::make_pair(msg, stack ? stack : ""),
						std::make_pair(pos, syn)));
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
			bool syn = false;
			for (std::vector<MessageT>::const_iterator it = messages_.begin();
				it != messages_.end(); ++it) {
#define Msg(it) ((it)->first.first)
#define Stack(it) ((it)->first.second)
#define Pos(it) ((it)->second.first)
#define Syn(it)((it)->second.second)
				int level = getlevel(Msg(it).c_str()[0]);
				switch (level) {
				case LVL_FATAL:
				case LVL_TRACE: {
					write(Msg(it).c_str(), Msg(it).size());
					write(Stack(it).c_str(), Stack(it).size());
					stdoutbuf(level,
						Msg(it).c_str(), Msg(it).size(), Pos(it),
						Stack(it).c_str(), Stack(it).size());
					break;
				}
				case LVL_ERROR:
				case LVL_WARN:
				case LVL_INFO:
				case LVL_DEBUG: {
					write(Msg(it).c_str(), Msg(it).size());
					stdoutbuf(level, Msg(it).c_str(), Msg(it).size(), Pos(it));
					break;
				}
				}
				if ((syn = Syn(it))) {
					break;
				}
			}
			messages_.clear();
			return syn;
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

	//stdoutbuf
	//需要调用utils::initConsole()初始化
	void Logger::stdoutbuf(int level, char const* msg, size_t len, size_t pos, char const* stack, size_t stacklen) {
#ifdef QT_CONSOLE
		switch (level) {
		default:
		case LVL_FATAL: qInfo/*qFatal*/() << msg; break;
		case LVL_ERROR: qCritical() << msg; break;
		case LVL_WARN: qWarning() << msg; break;
		case LVL_INFO: qInfo() << msg; break;
		case LVL_TRACE: qInfo() << msg; break;
		case LVL_DEBUG: qDebug() << msg; break;
		}
#elif defined(_windows_)
//foreground color
#define FOREGROUND_Red         (FOREGROUND_RED)//红
#define FOREGROUND_Green       (FOREGROUND_GREEN)//绿
#define FOREGROUND_Blue        (FOREGROUND_BLUE)//蓝
#define FOREGROUND_Yellow      (FOREGROUND_RED|FOREGROUND_GREEN)//黄
#define FOREGROUND_Cyan        (FOREGROUND_GREEN|FOREGROUND_BLUE)//青
#define FOREGROUND_Purple      (FOREGROUND_RED|FOREGROUND_BLUE)//紫
#define FOREGROUND_White       (FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE)//白
#define FOREGROUND_Gray        (FOREGROUND_INTENSITY)//灰
#define FOREGROUND_Black       (0)//黑
#define FOREGROUND_LightRed    (FOREGROUND_INTENSITY|FOREGROUND_Red)//淡红
#define FOREGROUND_HighGreen   (FOREGROUND_INTENSITY|FOREGROUND_Green)//亮绿
#define FOREGROUND_LightBlue   (FOREGROUND_INTENSITY|FOREGROUND_Blue)//淡蓝
#define FOREGROUND_LightYellow (FOREGROUND_INTENSITY|FOREGROUND_Yellow)//淡黄
#define FOREGROUND_HighCyan    (FOREGROUND_INTENSITY|FOREGROUND_Cyan)//亮青
#define FOREGROUND_Pink        (FOREGROUND_INTENSITY|FOREGROUND_Purple)//粉红
#define FOREGROUND_HighWhite   (FOREGROUND_INTENSITY|FOREGROUND_White)//亮白
//background color
#define BACKGROUND_Red         (BACKGROUND_RED)//红
#define BACKGROUND_Green       (BACKGROUND_GREEN)//绿
#define BACKGROUND_Blue        (BACKGROUND_BLUE)//蓝
#define BACKGROUND_Yellow      (BACKGROUND_RED|BACKGROUND_GREEN)//黄
#define BACKGROUND_Cyan        (BACKGROUND_GREEN|BACKGROUND_BLUE)//青
#define BACKGROUND_Purple      (BACKGROUND_RED|BACKGROUND_BLUE)//紫
#define BACKGROUND_White       (BACKGROUND_RED|BACKGROUND_GREEN|BACKGROUND_BLUE)//白
#define BACKGROUND_Gray        (BACKGROUND_INTENSITY)//灰
#define BACKGROUND_Black       (0)//黑
#define BACKGROUND_LightRed    (BACKGROUND_INTENSITY|BACKGROUND_Red)//淡红
#define BACKGROUND_HighGreen   (BACKGROUND_INTENSITY|BACKGROUND_Green)//亮绿
#define BACKGROUND_LightBlue   (BACKGROUND_INTENSITY|BACKGROUND_Blue)//淡蓝
#define BACKGROUND_LightYellow (BACKGROUND_INTENSITY|BACKGROUND_Yellow)//淡黄
#define BACKGROUND_HighCyan    (BACKGROUND_INTENSITY|BACKGROUND_Cyan)//亮青
#define BACKGROUND_Pink        (BACKGROUND_INTENSITY|BACKGROUND_Purple)//粉红
#define BACKGROUND_HighWhite   (BACKGROUND_INTENSITY|BACKGROUND_White)//亮白

		static int const color[][2] = {
			{FOREGROUND_Red, FOREGROUND_LightRed},//FATAL
			{FOREGROUND_Red, FOREGROUND_Purple},//ERROR
			{FOREGROUND_Cyan, FOREGROUND_HighCyan},//WARN
			{FOREGROUND_Pink, FOREGROUND_White},//INFO
			{FOREGROUND_Yellow, FOREGROUND_LightYellow},//TRACE
			{FOREGROUND_HighGreen, FOREGROUND_Gray},//DEBUG
		};
		HANDLE h = ::GetStdHandle(STD_OUTPUT_HANDLE);
		switch (level) {
		case LVL_FATAL:
		case LVL_TRACE: {
			::SetConsoleTextAttribute(h, color[level][0]);
			printf("%.*s", (int)pos, msg);
			::SetConsoleTextAttribute(h, color[level][1]);
			printf("%.*s", (int)len - (int)pos, msg + pos);
			::SetConsoleTextAttribute(h, color[level][0]);
			printf("%.*s", (int)stacklen, stack);//stack
			break;
		}
		case LVL_ERROR:
		case LVL_WARN:
		case LVL_INFO:
		case LVL_DEBUG: {
			::SetConsoleTextAttribute(h, color[level][0]);
			printf("%.*s", (int)pos, msg);
			::SetConsoleTextAttribute(h, color[level][1]);
			printf("%.*s", (int)len - (int)pos, msg + pos);
			break;
		}
		}
		//::CloseHandle(h);
#else
		switch (level) {
		case LVL_FATAL:
		case LVL_TRACE: {
			printf("%.*s", (int)len, msg);
			printf("%.*s", (int)stacklen, stack);//stack
			break;
		}
		case LVL_ERROR:
		case LVL_WARN:
		case LVL_INFO:
		case LVL_DEBUG: {
			printf("%.*s", (int)len, msg);
			break;
		}
		}
#endif
	}

	//sync
	void Logger::sync() {
		{
			std::unique_lock<std::mutex> lock(sync_mutex_); {
				sync_ = true;
				sync_cond_.notify_all();
			}
		}
	}

	//wait
	void Logger::wait() {
		{
			std::unique_lock<std::mutex> lock(sync_mutex_); {
				while (!sync_) {
					sync_cond_.wait(lock);
				}
				sync_ = false;
			}
		}
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