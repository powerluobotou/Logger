/**
* 
*   基于消息队列的异步日志系统
*	Created by andy_ro@qq.com 2021.11.17
* 
*/
#include "Log.h"

//gettid
static /*tid_t*/std::string gettid() {
	std::ostringstream oss;
	oss << std::this_thread::get_id();
	return oss.str();//(tid_t)std::stoull(oss.str());
}

//trim_file
static std::string const trim_file(char const* _FILE_) {
	std::string _file_ = _FILE_;
#if defined(WIN32) || defined(_WIN32)|| defined(WIN64) || defined(_WIN64)
	_file_ = _file_.substr(_file_.find_last_of('\\') + 1, -1);
#else
	_file_ = _file_.substr(_file_.find_last_of('/') + 1, -1);
#endif
	return _file_;
}

//trim_func
static std::string const trim_func(char const* _FUNC_) {
	std::string _func_ = _FUNC_;
	_func_ = _func_.substr(_func_.find_last_of(':') + 1, -1);
	return _func_;
}

#if defined(WIN32) || defined(_WIN32)|| defined(WIN64) || defined(_WIN64)
//get_error_string
static std::string get_error_string(unsigned err_no) {
	char* msg;
	FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, err_no, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&msg, 0, NULL);
	std::string str(msg);
	LocalFree(msg);
	return str;
}
#endif

namespace LOGGER {

	//constructor
	Logger::Logger() :pid_(getpid()) {
	};

	//destructor
	Logger::~Logger() {
		stop();
		close_fd();
	}
	
	//update
	void Logger::update(struct tm& tm, struct timeval& tv) {
		{
			std::unique_lock<std::shared_mutex> lock(tm_mutex_); {
#if defined(WIN32) || defined(_WIN32)|| defined(WIN64) || defined(_WIN64)
				gettimeofday(&tv_/*, NULL*/);
				time_t time = tv_.tv_sec;
				localtime_s(&tm_, &time);
#else
				gettimeofday(&tv_, NULL);
				localtime_r(&tv_.tv_sec, &tm_);
#endif
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

	//init_log
	void Logger::init_log(char const* dir, int level, char const* pre_name, int log_size) {
		close_fd();
		snprintf(prefix_, sizeof(prefix_), "%s/%s", dir, pre_name);
		level_ = level;
		log_size_ = log_size;
		struct tm tm;
		struct timeval tv;
		update(tm, tv);
		snprintf(path_, sizeof(path_), "%s.%d-%04d%02d%02d.log",
			prefix_, pid_, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday);
#if 1
		struct stat stStat;
		if (stat(dir, &stStat) < 0) {
#else
		if (access(dir, 0) < 0) {
#endif
#if defined(WIN32) || defined(_WIN32)|| defined(WIN64) || defined(_WIN64)
			if (mkdir(dir) < 0) {
				return;
			}
#else
			if (mkdir(dir, /*0777*/S_IRWXU | S_IRWXG | S_IRWXO) < 0) {
				return;
			}
#endif
		}
		start();
	}

	//write_log
	void Logger::write_log(int level, char const* file, int line, char const* func, char const* fmt, ...) {
		if (level > level_)
			return;
		struct tm tm;
		struct timeval tv;
		update(tm, tv);
		char log_buf[8192];
		size_t pos = 0;
		switch (level) {
		case LVL_FATAL:
			pos = snprintf(log_buf, sizeof(log_buf), "F%d %02d:%02d:%02d.%.6lu %s %s:%d] %s ",
				pid_,
				tm.tm_hour, tm.tm_min, tm.tm_sec, (unsigned long)tv.tv_usec,
				gettid().c_str(),
				trim_file(file).c_str(), line, trim_func(func).c_str());
			break;
		case LVL_ERROR:
			pos = snprintf(log_buf, sizeof(log_buf), "E%d %02d:%02d:%02d.%.6lu %s %s:%d] %s ",
				pid_,
				tm.tm_hour, tm.tm_min, tm.tm_sec, (unsigned long)tv.tv_usec,
				gettid().c_str(),
				trim_file(file).c_str(), line, trim_func(func).c_str());
			break;
		case LVL_WARN:
			pos = snprintf(log_buf, sizeof(log_buf), "W%d %02d:%02d:%02d.%.6lu %s %s:%d] %s ",
				pid_,
				tm.tm_hour, tm.tm_min, tm.tm_sec, (unsigned long)tv.tv_usec,
				gettid().c_str(),
				trim_file(file).c_str(), line, trim_func(func).c_str());
			break;
		case LVL_INFO:
			pos = snprintf(log_buf, sizeof(log_buf), "I%d %02d:%02d:%02d.%.6lu %s %s:%d] %s ",
				pid_,
				tm.tm_hour, tm.tm_min, tm.tm_sec, (unsigned long)tv.tv_usec,
				gettid().c_str(),
				trim_file(file).c_str(), line, trim_func(func).c_str());
			break;
		case LVL_DEBUG:
			pos = snprintf(log_buf, sizeof(log_buf), "D%d %02d:%02d:%02d.%.6lu %s %s:%d] %s ",
				pid_,
				tm.tm_hour, tm.tm_min, tm.tm_sec, (unsigned long)tv.tv_usec,
				gettid().c_str(),
				trim_file(file).c_str(), line, trim_func(func).c_str());
			break;
		case LVL_TRACE:
			pos = snprintf(log_buf, sizeof(log_buf), "T%d %02d:%02d:%02d.%.6lu %s %s:%d] %s ",
				pid_,
				tm.tm_hour, tm.tm_min, tm.tm_sec, (unsigned long)tv.tv_usec,
				gettid().c_str(),
				trim_file(file).c_str(), line, trim_func(func).c_str());
			break;
		default:
			return;
		}
		va_list ap;
		va_start(ap, fmt);
#if defined(WIN32) || defined(_WIN32)|| defined(WIN64) || defined(_WIN64)
		vsnprintf_s(log_buf + pos, sizeof(log_buf) - pos - 1, _TRUNCATE, fmt, ap);
#else
		vsnprintf(log_buf + pos, sizeof(log_buf) - pos - 1, fmt, ap);
#endif
		va_end(ap);
		pos = strlen(log_buf);
		log_buf[pos] = '\n';
		log_buf[pos + 1] = 0;
		notify(log_buf);
	}

#if 0
	void Logger::write_log_buf(char const* msg) {
#if defined(WIN32) || defined(_WIN32)|| defined(WIN64) || defined(_WIN64)
		long size = 0;
		WriteFile(fd_, msg, strlen(msg), (LPDWORD)&size, NULL);
#else
		(void)write(fd_, msg, strlen(msg));
#endif
	}
#endif

	//open_fd
	void Logger::open_fd() {
		if (!path_[0]) {
			return;
		}
#if defined(WIN32) || defined(_WIN32)|| defined(WIN64) || defined(_WIN64)
		fd_ = CreateFileA(path_, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (INVALID_HANDLE_VALUE == fd_)
		{
			int err = GetLastError();
			std::string errmsg = get_error_string(err);
			fprintf(stderr, "open %s error[%d:%s]\n", path_, err, errmsg.c_str());
		}
		else
		{
			SetFilePointer(fd_, 0, NULL, FILE_END);
		}
#else
		fd_ = open(path_, O_WRONLY | O_CREAT | O_APPEND | O_LARGEFILE, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		if (fd_ == INVALID_HANDLE_VALUE)
			fprintf(stderr, "open %s error[%d:%s]\n", path_, errno, strerror(errno));
#endif
	}

	//close_fd
	void Logger::close_fd() {
#if defined(WIN32) || defined(_WIN32)|| defined(WIN64) || defined(_WIN64)
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
	
	//shift_fd
	void Logger::shift_fd(struct tm const& tm, struct timeval const& tv) {
		struct stat stStat;
		if (stat(path_, &stStat) < 0) {
			return;
		}
		if (stStat.st_size < log_size_) {
		}
		else {
			close_fd();
			char tmp[512];
			snprintf(tmp, sizeof(tmp), "%s.%d-%04d%02d%02d-%02d%02d%02d",
				prefix_, pid_, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);

			if (stat(tmp, &stStat) == 0) {
				snprintf(tmp, sizeof(tmp), "%s.%d-%04d%02d%02d-%02d%02d%02d.%.6lu.log",
					prefix_, pid_, tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec, (unsigned long)tv.tv_usec);
				printf("newFile: %s\n", tmp);
			}
			if (rename(path_, tmp) < 0) {
				return;
			}
			open_fd();
			log_day_ = tm.tm_mday;
		}
	}

	//start
	void Logger::start() {
		if (!started_.test_and_set()) {
			thread_ = std::thread([this](Logger* logger) {
				struct tm tm = { 0 };
				struct timeval tv = { 0 };
				while (!done_.load()) {
					get(tm, tv);
					std::unique_lock<std::mutex> lock(mutex_); {
						cond_.wait(lock);
						consume(tm, tv);
						std::this_thread::yield();
					}
				}
				started_.clear();
				}, this);
			thread_.detach();
		}
	}
	
	//notify
	void Logger::notify(char const* msg) {
		{
			std::unique_lock<std::mutex> lock(mutex_); {
				messages_.push_back(msg);
				cond_.notify_all();
			}
		}
		std::this_thread::yield();
	}

	//consume
	void Logger::consume(struct tm const& tm, struct timeval const& tv) {
		if (!messages_.empty()) {
			if (tm.tm_mday != log_day_) {
				close_fd();
				open_fd();
				log_day_ = tm.tm_mday;
			}
			for (std::vector<std::string>::const_iterator it = messages_.begin();
				it != messages_.end(); ++it) {
#if defined(WIN32) || defined(_WIN32)|| defined(WIN64) || defined(_WIN64)
				if (fd_ != INVALID_HANDLE_VALUE) {
					long size = 0;
					WriteFile(fd_, it->c_str(), it->length(), (LPDWORD)&size, NULL);
				}
				else
					printf(it->c_str());
#else
				if (fd_ != INVALID_HANDLE_VALUE) {
					(void)write(fd_, it->c_str(), it->length());
				}
				else
					printf(it->c_str());
#endif
				shift_fd(tm, tv);
			}
			messages_.clear();
		}
	}

	//stop
	void Logger::stop() {
		started_.clear();
		done_.store(true);
		if (thread_.joinable()) {
			thread_.join();
		}
	}
}

/*
int main()
{
	Log::instance()->init_log (".", 7, "conn");
	while(1)
	{
		for(int i =0; i < 200000; i++)
		{
			LOG_ERROR("Hi%d", i);
		}
	}
	return 0;
}
*/