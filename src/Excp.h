/**
*
*   异常处理
*	Created by andy_ro@qq.com 2021.11.20
*
*/
#pragma once

#include "../Macro.h"

#include <exception>
#include <stdexcept>
#include "Log.h"

namespace excp {
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
#define _MY_TRACE_A(fn, err_str) \
	{ \
		try { \
			throw base_error(__FILE__, __LINE__, __FUNC__, err_str); \
		} \
		catch (base_error& e) { \
			std::ostringstream oss; \
			oss << "TRACE[ " << #fn << " ] " << e.what() \
			   << " " << utils::trim_file(e.f_.c_str()) << "(" << e.l_ << ") " << utils::trim_func(e.fn_.c_str()); \
			_LOG_S_TRACE(oss.str()); \
		} \
	}

// TRACE[ __NAME__(__ERRNO__) ] __STRERR__ __FILE__(__LINE__) __FUNC__
#define _MY_TRACE_B(fn, err_no, err_str) \
	{ \
		try { \
			throw base_error(__FILE__, __LINE__, __FUNC__, err_no, err_str); \
		} \
		catch (base_error& e) { \
			std::ostringstream oss; \
			oss << "TRACE[ " << #fn << "(" << e.err_no_ << ") ] " << e.what() \
			   << " " << utils::trim_file(e.f_.c_str()) << "(" << e.l_ << ") " << utils::trim_func(e.fn_.c_str()); \
			_LOG_S_TRACE(oss.str()); \
		} \
	}


// EXCEPTION: __STRERR__ __FILE__(__LINE__) __FUNC__
// EXCEPTION: __NAME__(__ERRNO__) __STRERR__ __FILE__(__LINE__) __FUNC__

#define _MY_TRY() \
	try { \

#define _MY_TRY_TRACE() \
	try { \
	_LOG_DEBUG("")

#define _MY_THROW_A(err_str) (throw excp::function_error(__FILE__, __LINE__, __FUNC__, err_str))
#define _MY_THROW_C(fn_str, err_str) (throw excp::function_error(__FILE__, __LINE__, __FUNC__, fn_str, err_str))
#define _MY_THROW_B(fn_str, err_no, err_str) (throw excp::function_error(__FILE__, __LINE__, __FUNC__, fn_str, err_no, err_str))

#define _FUNC_CATCH() \
	} \
	catch (excp::function_error& e) { \
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
		_LOG_S_FATAL(oss.str());


#define _STD_CATCH() \
	} \
	catch (std::exception const& e) { \
		_LOG_S_FATAL(std::string("EXCEPTION: ") + e.what());


#define _ANY_CATCH() \
	} \
	catch (...) { \
		_LOG_S_FATAL("EXCEPTION: unknown error");

#define _MY_CATCH() \
	_FUNC_CATCH() \
	_STD_CATCH() \
	_ANY_CATCH() \
	}