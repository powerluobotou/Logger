#pragma once

#include "IOperation.h"

namespace Operation {

	class CSetOperation {
	public:
		CSetOperation(){ m_pFMOp = NULL; };
		virtual ~CSetOperation(){};
	public:
        void Open() {
            if (m_pFMOp) {
                m_pFMOp->MFOpen();
            }
        }
		void Flush() {
			if (m_pFMOp) {
				m_pFMOp->MFFlush();
			}
		}
        void Close() {
            if (m_pFMOp) {
				m_pFMOp->MFClose(); 
            }
        }
        size_t Write( const void * ptr, size_t size, size_t count ) {
            if (m_pFMOp) {
                return m_pFMOp->MFWrite(ptr, size, count);
            }
            return 0;
        }
		size_t Read(void * ptr, size_t size, size_t count) {
			if (m_pFMOp) {
				return m_pFMOp->MFRead(ptr, size, count);
			}
			return 0;
		}
		void GetBuffer(std::string & s) {
			if (m_pFMOp) {
				return m_pFMOp->MFBuffer(s);
			}
		}
		void SetOperation(IOperation* pFMop){m_pFMOp = pFMop;};
		IOperation* GetOperation() { return m_pFMOp; }
    private:
		IOperation* m_pFMOp;
	};
}