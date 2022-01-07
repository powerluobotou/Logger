#pragma once

#include "../Macro.h"

namespace utils {
	
	//v [IN] 当前版本号
	//url [IN] 版本服务器url
	//dir [IN] 下载安装文件保存路径
	//cb [IN] 回调函数 -1失败，退出 0成功，退出 1失败，继续
	//m [OUT] 线路配置列表
	void checkVersion(
		std::string const& v,
		std::string const& url,
		std::string const& dir,
		std::function<void(int rc)> cb,
		std::map<std::string, std::string>& m);
}