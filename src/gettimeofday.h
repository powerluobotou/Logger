/**
*
*   gettimeofday
*	Created by andy_ro@qq.com 2021.11.17
*
*/
#ifndef GETTIMEOFDAY_H_INCLUDE
#define GETTIMEOFDAY_H_INCLUDE

#include "../Macro.h"

#ifdef _windows_

#include <chrono>

// struct timeval {
// 	time_t tv_sec;
// 	long tv_usec;
// };

#define kSecsPerMillisecond 1e-3
#define kSecsPerMicrosecond 1e-6
#define kSecsPerNanosecond  1e-9
#define kMillisecsPerSecond 1000/*1e3*/
#define kMicrosecsPerSecond 1000000/*1e6*/
#define kNanosecsPerSecond  1000000000/*1e9*/

//frequency
LARGE_INTEGER frequency();

//counter
LARGE_INTEGER counter();

//gettickcount
double gettickcount();

//gettimeofday
void gettimeofday(struct timeval* tv, struct timezone* tz);

#endif

//gettime
double gettime();

#endif