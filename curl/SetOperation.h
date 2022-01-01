#pragma once

#include "IOperation.h"

namespace Operation {

	class CSetOperation {
	public:
		CSetOperation() :op_(NULL) {}
		virtual ~CSetOperation() {}
	public:
		bool Open(Mode mode = Mode::M_READ) {
			if (op_) {
				return op_->Open(mode);
			}
			return false;
		}
		void Flush() {
			if (op_) {
				op_->Flush();
			}
		}
		void Close() {
			if (op_) {
				op_->Close();
			}
		}
		size_t Write(const void* ptr, size_t size, size_t count) {
			if (op_) {
				return op_->Write(ptr, size, count);
			}
			return 0;
		}
		size_t Read(void* ptr, size_t size, size_t count) {
			if (op_) {
				return op_->Read(ptr, size, count);
			}
			return 0;
		}
		void Buffer(char* buffer, size_t size) {
			if (op_) {
				op_->Buffer(buffer, size);
			}
		}
		void GetBuffer(std::string& s) {
			if (op_) {
				op_->Buffer(s);
			}
		}
		void GetBuffer(std::vector<char>& buffer) {
			if (op_) {
				op_->Buffer(buffer);
			}
		}
		void SetOperation(IOperation* op) { op_ = op; }
		IOperation* GetOperation() { return op_; }
    private:
		IOperation* op_;
	};
}