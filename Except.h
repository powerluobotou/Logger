/**
*
*   异常处理
*	Created by andy_ro@qq.com 2021.11.20
*
*/
#ifndef EXCEPT_H_INCLUDE
#define EXCEPT_H_INCLUDE

#include "Macro.h"

#include <exception>
#include <stdexcept>
#include "utils.h"
#include "Log.h"

//https://www.csdndocs.com/article/5961151
//https://developer.aliyun.com/article/317590
#ifdef _windows_// /EHa
#define _TRY_   __try
#define _CATCH_ __except
#else
#define _TRY_   try
#define _CATCH_ catch
#endif

namespace EXCEPT {
	//base_error
	class base_error : public std::runtime_error {
	public:
		base_error(char const* f, int l, char const* fn, char const* err_str) :f_(f), l_(l), fn_(fn), std::runtime_error(err_str), err_no_(0XFFFFFFFFL) {}
		base_error(char const* f, int l, char const* fn, int err_no, char const* err_str) :f_(f), l_(l), fn_(fn), std::runtime_error(err_str), err_no_(err_no) {}
		virtual ~base_error() noexcept {}
	public:
		int const err_no_;
		int l_;
		std::string const f_, fn_;
	};

	//function_error
	class function_error : public base_error {
	public:
		function_error(char const* f, int l, char const* fn, char const* err_str) :base_error(f, l, fn, err_str), fn_str_("") {}
		function_error(char const* f, int l, char const* fn, char const* fn_str, char const* err_str) :base_error(f, l, fn, err_str), fn_str_(fn_str) {}
		function_error(char const* f, int l, char const* fn, char const* fn_str, int err_no, char const* err_str) :base_error(f, l, fn, err_no, err_str), fn_str_(fn_str) {}
		~function_error() noexcept {}
	public:
		std::string const fn_str_;
	};
}

// TRACE[ __NAME__ ] __STRERR__ __FILE__(__LINE__) __FUNC__
#define MY_TRACE_A(fn, err_str) \
	{ \
		try { \
			throw base_error(__FILE__, __LINE__, __FUNC__, err_str); \
		} \
		catch (base_error& e) { \
			std::ostringstream oss; \
			oss << "TRACE[ " << #fn << " ] " << e.what() \
			   << " " << utils::trim_file(e.f_) << "(" << e.l_ << ") " << utils::trim_func(e.fn_); \
			LOG_TRACE(oss.str().c_str()); \
		} \
	}

// TRACE[ __NAME__(__ERRNO__) ] __STRERR__ __FILE__(__LINE__) __FUNC__
#define MY_TRACE_B(fn, err_no, err_str) \
	{ \
		try { \
			throw base_error(__FILE__, __LINE__, __FUNC__, err_no, err_str); \
		} \
		catch (base_error& e) { \
			std::ostringstream oss; \
			oss << "TRACE[ " << #fn << "(" << e.err_no_ << ") ] " << e.what() \
			   << " " << utils::trim_file(e.f_) << "(" << e.l_ << ") " << utils::trim_func(e.fn_); \
			LOG_TRACE(oss.str().c_str()); \
		} \
	}


// EXCEPTION: __STRERR__ __FILE__(__LINE__) __FUNC__
// EXCEPTION: __NAME__(__ERRNO__) __STRERR__ __FILE__(__LINE__) __FUNC__

#define MY_TRY()	\
	try { \

#define MY_THROW_A(err_str) (throw function_error(__FILE__, __LINE__, __FUNC__, err_str))
#define MY_THROW_C(fn_str, err_str) (throw function_error(__FILE__, __LINE__, __FUNC__, fn_str, err_str))
#define MY_THROW_B(fn_str, err_no, err_str) (throw function_error(__FILE__, __LINE__, __FUNC__, fn_str, err_no, err_str))

#define MY_CATCH() \
	} \
	catch (EXCEPT::function_error& e) { \
		std::ostringstream oss; \
		e.err_no_ == 0XFFFFFFFFL ? \
		( \
		e.fn_str_.empty() ? \
		oss << "EXCEPTION: " << e.what() \
		   << " " << utils::trim_file(e.f_.c_str()) << "(" << e.l_ << ") " << utils::trim_func(e.fn_.c_str()) \
		   : \
		oss << "EXCEPTION: " << e.fn_str_ << " " << e.what() \
		   << " " << utils::trim_file(e.f_.c_str()) << "(" << e.l_ << ") " << e.fn_ \
		 ) : \
		oss << "EXCEPTION: " << e.fn_str_ << "(" << e.err_no_ << ") " << e.what() \
		   << " " << utils::trim_file(e.f_.c_str()) << "(" << e.l_ << ") " << utils::trim_func(e.fn_.c_str()); \
		LOG_S_FATAL(oss.str().c_str()); \
		LOG_S_ERROR(utils::stack_backtrace()); \
		abort(); \
	} \
	catch (const std::exception& e) { \
			LOG_S_ERROR(e.what()); \
			LOG_S_ERROR(utils::stack_backtrace()); \
			abort(); \
	} \
	catch (...) { \
		LOG_S_ERROR(utils::stack_backtrace()); \
		abort(); \
	} \

#endif