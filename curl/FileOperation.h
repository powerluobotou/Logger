#pragma once

#include "IMemFileOperation.h"
#include <stdio.h>
#include <string>

//
// http://c.biancheng.net/cpp/html/2514.html
//
namespace MemFileOperation{
	using namespace ToolsInterface;

	class CFileOperation : public IMemFileOperation {
	public:
		explicit CFileOperation( const char* pFilePath );
		~CFileOperation();
	public:
        virtual bool IsFile();
		virtual void MFClearErr();
		virtual bool MFClose();
		virtual int MFEof();
		virtual int MFError();
		virtual int MFFlush();
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
	private:
		FILE * m_stream;
		std::string m_strFilePath;
	public:
		char const * MFPath() { return m_strFilePath.c_str(); }
	};
}