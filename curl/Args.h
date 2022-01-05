/**
*
*   全局定义
*	Created by 萝卜 2021.12.17
*
*/
#pragma once

#include "IOperation.h"

namespace Curl {
	enum REQState {
		eNetError,
		eContinue,
		eTimeout,
		eSuc,
		eFailed,
		eInterrupt,
	};
}

namespace Operation {
	struct Args {
		Operation::IOperation* value;
		std::string key;
		bool postAsFile;
		struct FileInfo {
			char filename[128];
		};
		struct MemInfo {
			bool multi;
		};
		union {
			FileInfo fi;
			MemInfo mi;
		};
	};
}