# Logger

Async Logger  c++


异步日志系统

Atomic.h 原子操作相关

Except.h Except.cpp 异常处理模块 try catch throw

Log.h Log.cpp 日志系统 LOG_FATAL 配合MY_TRY()，MY_CATCH()使用，程序崩溃捕获跟踪调用堆栈

例子

```c++
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
```
