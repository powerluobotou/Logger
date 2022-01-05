#pragma once

#include "Args.h"
#include "SetOperation.h"

namespace Curl {

	typedef std::function<size_t(Operation::CSetOperation* obj, void* buffer, size_t size, size_t nmemb)> OnBuffer;
	typedef std::function<void(Operation::CSetOperation* obj, double ltotal, double lnow)> OnProgress;

	class ClientImpl;
	class Client {
	public:
		Client();
		Client(bool sync);
		~Client();
		int check(char const* url, double& size);
		int get(
			char const* url,
			std::list<std::string> const* headers,
			std::string* resp = NULL,
			char const* spath = NULL,
			bool dump = true, FILE* fd = stderr);
		int post(
			char const* url,
			std::list<std::string> const* headers,
			char const* spost,
			std::string* resp,
			char const* spath = NULL,
			bool dump = true, FILE* fd = stderr);
		int upload(
			char const* url,
			std::list<Operation::Args> const* args,
			std::string* resp,
			OnProgress onProgress,
			char const* spath = NULL,
			bool dump = true, FILE* fd = stderr);
		int download(
			char const* url,
			char const* savepath,
			OnBuffer onBuffer,
			OnProgress onProgress,
			char const* spath = NULL,
			bool dump = true, FILE* fd = stderr);
	private:
		ClientImpl* impl_;
	};
}