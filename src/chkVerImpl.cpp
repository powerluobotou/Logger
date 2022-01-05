#include "chkVerImpl.h"
#include "utilsImpl.h"
#include "iniImpl.h"

#include "LoggerImpl.h"
#include "excp.h"

#include "../curl/impl/ClientImpl.h"
//#include "../curl/impl/MemImpl.h"
#include "../curl/impl/FileImpl.h"

#include "mymd5.h"

namespace utils {

	//-1失败，退出 0成功，退出 1失败，继续
	void _updateVersion(INI::Section& version, std::string const& dir, std::function<void(int rc)> cb) {
		__MY_TRY();
		std::string url = version["download"];
		std::string::size_type pos = url.find_last_of('/');
		std::string filename = url.substr(pos + 1, -1);
		if (!utils::_mkDir(dir.c_str())) {
			__PLOG_ERROR("创建下载目录失败..%s\n\t1.可能权限不够，请选择其它盘重新安装，不要安装在C盘，或以管理员身份重新启动!", dir.c_str());
			__LOG_CONSOLE_CLOSE(5000);
			cb(-1);//失败，退出
			return;
		}
		std::string path = dir;
		//std::string path = utils::_GetModulePath() + "/download";
		path += "/" + filename;
		Operation::FileImpl f(path.c_str());
		if (f.Valid()) {
			std::vector<char> data;
			f.Buffer(data);
			f.Close();
			__PLOG_DEBUG("安装包已存在! 共 %d 字节，准备校验...", data.size());
			if (data.size() > 0) {
				char md5[32 + 1] = { 0 };
				MD5Encode32(&data.front(), data.size(), md5, 0);
				if (atol(version["size"].c_str()) == data.size() &&
					strncasecmp(md5, version["md5"].c_str(), strlen(md5)) == 0) {
					__PLOG_DEBUG("校验成功，开始安装新版程序包...");
					std::string content = version["context"];
					utils::_replaceEscChar(content);
					__PLOG_WARN("*******************************************");
					//PLOG_WARN(content.c_str(), m["no"].c_str());
					__PLOG_WARN(content.c_str());
					__PLOG_WARN("*******************************************");
					//::WinExec(path.c_str(), SW_SHOWMAXIMIZED);
					::ShellExecuteA(NULL, "open", path.c_str(), NULL, NULL, SW_SHOWNORMAL);
					__LOG_CONSOLE_CLOSE(5000);
					cb(0);//成功，退出
					return;
				}
			}
			__TLOG_DEBUG("校验失败，重新下载安装包... %s", url.c_str());
		}
		else {
			__TLOG_DEBUG("开始下载安装包... %s", url.c_str());
		}
		std::vector<char> data;
		Curl::ClientImpl req(true);
		if (req.download(
			url.c_str(),
			path.c_str(),
			[&](Operation::COperation* obj, void* buffer, size_t size, size_t nmemb) -> size_t {
				size_t n = obj->Write(buffer, size, nmemb);
				if (n > 0) {
					int offset = data.size();
					data.resize(offset + n);
					memcpy(&data[offset], buffer, n);
				}
				return n;
			},
			[&](Operation::COperation* obj, double ltotal, double lnow) {
				//Operation::FileImpl* f = (Operation::FileImpl*)obj->GetOperation();
				//std::string path = f->Path();
				std::string path = obj->GetOperation()->Path();
				std::string::size_type pos = path.find_last_of('\\');
				std::string filename = path.substr(pos + 1, -1);
				__TLOG_INFO("下载进度 %.2f%% 路径 %s", (lnow / ltotal) * 100, path.c_str());
				if (lnow == ltotal) {
					obj->GetOperation()->Flush();
					obj->GetOperation()->Close();
					__PLOG_DEBUG("下载完成! 共 %.0f 字节，准备校验...", ltotal);
					char md5[32 + 1] = { 0 };
					MD5Encode32(&data.front(), data.size(), md5, 0);
					if (atol(version["size"].c_str()) == data.size() &&
						strncasecmp(md5, version["md5"].c_str(), strlen(md5)) == 0) {
						__PLOG_DEBUG("校验成功，开始安装新版程序包...");
						std::string content = version["context"];
						utils::_replaceEscChar(content);
						__PLOG_WARN("*******************************************");
						//PLOG_WARN(content.c_str(), version["no"].c_str());
						__PLOG_WARN(content.c_str());
						__PLOG_WARN("*******************************************");
						//::WinExec(path.c_str(), SW_SHOWMAXIMIZED);
						::ShellExecuteA(NULL, "open", path.c_str(), NULL, NULL, SW_SHOWNORMAL);
						__LOG_CONSOLE_CLOSE(5000);
						cb(0);//成功，退出
					}
					else {
						__PLOG_ERROR("校验失败，请检查安装包[版本号/大小/MD5值]\n");
						__LOG_CONSOLE_CLOSE(5000);
						cb(1);//失败，继续
					}
				}
			}, NULL, false, stdout) < 0) {
			__PLOG_ERROR("更新失败，失败原因可能：\n\t1.下载包打开失败，目录or文件?\n\t2.下载包可能被占用，请关闭后重试.\n\t3.可能权限不够，请选择其它盘重新安装，不要安装在C盘，或以管理员身份重新启动!\n\t4.检查下载链接%s是否有效!\n\t5.请检查本地网络.", url.c_str());
			__LOG_CONSOLE_CLOSE(5000);
			cb(-1);//失败，退出
		}
		__MY_CATCH();
	}

	//-1失败，退出 0成功，退出 1失败，继续
	void _checkVersion(std::string const& v, std::string const& url, std::string const& dir, std::function<void(int rc)> cb) {
		__LOG_CONSOLE_OPEN();
		__TLOG_DEBUG("正在检查版本信息 %s", url.c_str());
		__PLOG_DEBUG("当前版本号 %s", v.c_str());
		__MY_TRY();
		//text/plain
		//text/html
		//application/json;charset=UTF-8
		//application/octet-stream
		std::list<std::string> header;
		header.push_back("Content-Type: text/plain;charset=UTF-8");
		std::string vi;
		Curl::ClientImpl req;
		if (req.get(url.c_str(), &header, &vi) < 0) {
			__PLOG_ERROR("下载失败，失败原因可能：\n\t1.检查链接%s是否有效!\n\t2.请检查本地网络.", url.c_str());
			__LOG_CONSOLE_CLOSE(5000);
			cb(1);//失败，继续
		}
		else {
			//__LOG_WARN(vi.c_str());
			utils::INI::Section* version = NULL;
			utils::INI::ReaderImpl reader;
			if (reader.parse(vi.c_str(), vi.length())) {
				version = reader.get("version");
				if (version && v != (*version)["no"]) {
					__PLOG_WARN("发现新版本 %s\n文件大小 %s 字节\nMD5值 %s\n准备更新...",
						(*version)["no"].c_str(),
						(*version)["size"].c_str(),
						(*version)["md5"].c_str());
					utils::_updateVersion(*version, dir, cb);
					return;
				}
				__PLOG_INFO("版本检查完毕，没有发现新版本.");
				__LOG_CONSOLE_CLOSE(2000);
				cb(1);//失败，继续
			}
			else {
				__PLOG_ERROR("版本配置解析失败.");
				__LOG_CONSOLE_CLOSE(5000);
				cb(1);//失败，继续
			}
		}
		__MY_CATCH();
	}
}