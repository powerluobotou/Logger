/**
*
*   INI配置读取
*	Created by andy_ro@qq.com 2022.01.04
*
*/
#include "../ini.h"
#include "iniImpl.h"
#include "utilsImpl.h"
#include "auth.h"

namespace utils {

	namespace INI {
		Reader::Reader():reader_(new _Reader()) {
		}
		Reader::~Reader() {
			delete reader_;
		}
		bool Reader::parse(char const* filename) {
			AUTHORIZATION_CHECK_B;
			return reader_->parse(filename);
		}
		bool Reader::parse(char const* buf, size_t len) {
			AUTHORIZATION_CHECK_B;
			return reader_->parse(buf, len);
		}
		Section* Reader::get(char const* section) {
			return reader_->get(section);
		}
		std::string Reader::get(char const* section, char const* key) {
			return reader_->get(section, key);
		}
	}

	void CheckVersion(INI::Section& version, std::function<void(int rc)> cb) {
		AUTHORIZATION_CHECK;
		utils::_CheckVersion(version, cb);
	}
}