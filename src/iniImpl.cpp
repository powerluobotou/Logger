/**
*
*   INI配置读取
*	Created by andy_ro@qq.com 2022.01.04
*
*/
#include "iniImpl.h"
#include "utilsImpl.h"

#include "Log.h"
#include "excp.h"

#include "auth.h"

#include "../curl/Client.h"
#include "../curl/Mem.h"
#include "../curl/File.h"

#include "mymd5.h"

namespace utils {

	namespace INI {

		void _readIni(char const* buf, Sections& sections) {
			sections.clear();
			std::string st(buf);
			std::string field;
			while (!st.empty()) {
				std::string::size_type pos = st.find_first_of("\r\n");
				if (pos == -1) {
					if (!st.empty()) {
						std::string s = st.substr(0, pos);
						if (!s.empty()) {
							if (s.length() > 2 && s[0] == '[' && s[s.length() - 1] == ']') {
								field = s.substr(1, s.length() - 2);
								std::map<std::string, std::string>& m = sections[field];
							}
							else if (!field.empty()) {
								std::string::size_type pos = s.find_first_of('=');
								if (pos != -1) {
									std::map<std::string, std::string>& m = sections[field];
									std::string key = s.substr(0, pos);
									std::string val = s.substr(pos + 1, -1);
									m[key] = val;
								}
							}
						}
					}
					break;
				}
				std::string s = st.substr(0, pos);
				if (!s.empty()) {
					if (s.length() > 2 && s[0] == '[' && s[s.length() - 1] == ']') {
						field = s.substr(1, s.length() - 2);
						std::map<std::string, std::string>& m = sections[field];
					}
					else if (!field.empty()) {
						std::string::size_type pos = s.find_first_of('=');
						if (pos != -1) {
							std::map<std::string, std::string>& m = sections[field];
							std::string key = s.substr(0, pos);
							std::string val = s.substr(pos + 1, -1);
							m[key] = val;
						}
					}
				}
				else {
					break;
				}
				st = st.substr(pos + 2, -1);
			};
		}

		bool _Reader::parse(char const* buf, size_t len) {
			m_.clear();
			if (buf && buf[0]) {
				utils::INI::_readIni(buf, m_);
			}
			return m_.size() > 0;
		}
		Section* _Reader::get(char const* section) {
			if (section && section[0]) {
				Sections::iterator it = m_.find(section);
				if (it != m_.end()) {
					return &it->second;
				}
			}
			return NULL;
		}
		std::string _Reader::get(char const* section, char const* key) {
			if (section && section[0]) {
				Sections::iterator it = m_.find(section);
				if (it != m_.end()) {
					Section& ref = it->second;
					if (key && key[0]) {
						Section::iterator ir = ref.find(key);
						if (ir != ref.end()) {
							return ir->second;
						}
					}
				}
			}
			return "";
		}
	}

	//-1失败，退出 0成功，退出 1失败，继续
	void _CheckVersion(INI::Section& version, std::function<void(int rc)> cb) {
		__MY_TRY();
		std::string url = version["download"];
		std::string::size_type pos = url.find_last_of('/');
		std::string filename = url.substr(pos + 1, -1);
		std::string path = utils::_GetModulePath() + "/download";
		utils::_mkDir(path.c_str());
		path += "/" + filename;
		Operation::CFile f(path.c_str());
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
					xsleep(10000);
					__LOG_CONSOLE_CLOSE();
					cb(0);//成功，退出
				}
			}
			__TLOG_DEBUG("校验失败，重新下载安装包... %s", url.c_str());
		}
		else {
			__TLOG_DEBUG("开始下载安装包... %s", url.c_str());
		}
		std::vector<char> data;
		Curl::Client req(true);
		if (req.download(
			url.c_str(),
			path.c_str(),
			[&](Curl::Easy* easy, void* buffer, size_t size, size_t nmemb) -> size_t {
				size_t n = easy->Write(buffer, size, nmemb);
				if (n > 0) {
					int offset = data.size();
					data.resize(offset + n);
					memcpy(&data[offset], buffer, n);
				}
				return n;
			},
			[&](Curl::Easy* easy, double ltotal, double lnow) {
				Operation::CFile* f = (Operation::CFile*)easy->GetOperation();
				std::string path = f->Path();
				std::string::size_type pos = path.find_last_of('\\');
				std::string filename = path.substr(pos + 1, -1);
				__TLOG_INFO("下载进度 %.2f%% 路径 %s", (lnow / ltotal) * 100, path.c_str());
				if (lnow == ltotal) {
					f->Flush();
					f->Close();
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
						xsleep(10000);
						__LOG_CONSOLE_CLOSE();
						cb(0);//成功，退出
					}
					else {
						__PLOG_ERROR("校验失败，请检查安装包[版本号/大小/MD5值]\n");
						xsleep(10000);
						__LOG_CONSOLE_CLOSE();
						cb(1);//失败，继续
					}
				}
			}, NULL, false, stdout)) {
			__PLOG_ERROR("更新失败，下载包可能被占用，请关闭后重试");
			xsleep(10000);
			__LOG_CONSOLE_CLOSE();
			cb(-1);//失败，退出
		}
		__MY_CATCH();
	}
}