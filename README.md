# Logger

Async Logger  c++

### 异步日志系统 windows/Linux
* 支持日志前缀，日志内容 按不同颜色显示

* 支持异常处理，异常捕获 TRY THROW CATCH

* 支持 LOG_FATAL 跟踪及断点打印堆栈信息

* 同步输出控制台及日志文件

### 例子

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
