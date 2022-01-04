#pragma once

#include <string>
#include <vector>
#include "IOperation.h"

typedef struct _FMParam_ {
	std::string strkey;
	Operation::LPIOperation value;
	bool postasfile;
	struct FileInfo {
		char szfilename[128];
	};

	struct MemInfo {
		bool bMulti;
	};

	union {
		FileInfo fileinfo;
		MemInfo meminfo;
	};
}FMParam, * PFMParam;

typedef std::vector<FMParam> FMParams;
typedef FMParams::iterator FMParamsIter;
typedef FMParams::const_iterator FMParamsCIter;
