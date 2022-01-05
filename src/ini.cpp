#include "../ini.h"
#include "iniImpl.h"
#include "utilsImpl.h"
#include "auth.h"

namespace utils {

	namespace INI {
		Reader::Reader() :impl_(new ReaderImpl()) {
		}
		Reader::~Reader() {
			delete impl_;
		}
		bool Reader::parse(char const* filename) {
			AUTHORIZATION_CHECK_B;
			return impl_->parse(filename);
		}
		bool Reader::parse(char const* buf, size_t len) {
			AUTHORIZATION_CHECK_B;
			return impl_->parse(buf, len);
		}
		Sections const& Reader::get() {
			return impl_->get();
		}
		Section* Reader::get(char const* section) {
			return impl_->get(section);
		}
		std::string Reader::get(char const* section, char const* key) {
			return impl_->get(section, key);
		}
	}
}