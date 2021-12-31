#pragma once

#include "IOperation.h"
#include<vector>
#include <string>

namespace MemFileOperation {
	
	using namespace ToolsInterface;

	class CMemory : public IOperation {
	public:
		CMemory();
		~CMemory();
	public:
        virtual bool IsFile();
		virtual bool MFClose();
		virtual int MFEof();
		virtual int MFGetc();
		virtual int MFGetPos( fpos_t * pos );
		virtual char * MFGets( char * str, int num );
		virtual bool MFOpen();
		virtual int MFPutc( int character );
		virtual int MFPuts( const char * str );
		virtual size_t MFRead(void * ptr, size_t size, size_t count);
		virtual int MFSeek( long offset, int origin );
		virtual int MFSetpos( const fpos_t * pos );
		virtual long MFTell();
		virtual size_t MFWrite( const void * ptr, size_t size, size_t count );
		virtual void MFRewind();
		virtual void MFBuffer(char *buffer, size_t size);
		virtual void MFBuffer(std::string & s);
 		CMemory(void* lpBuffer, unsigned long ulLength);
		//bool MFOpen();
		//bool MFRead(void* lpBuffer, unsigned long ulNumberOfBytesToRead, unsigned long* lpNumberOfBytesRead);
		//bool MFWrite(const void* lpBuffer, unsigned long ulNumberOfBytesToWrite, unsigned long* lpNumberOfBytesWritten);
		//unsigned long MFSeek( long offset, int origin );
		//bool MFClose();
		//bool IsEmpty();
	public:
		std::vector<char> m_buffer;
		size_t m_ulCurrentPos;
	};
}