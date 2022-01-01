#pragma once

#include "IOperation.h"
#include<vector>
#include <string>

namespace Operation {
	
	class CMemory : public IOperation {
	public:
		CMemory();
		~CMemory();
	public:
		virtual bool Valid();
        virtual bool IsFile();
		virtual bool Close();
		virtual int Eof();
		virtual int Getc();
		virtual int GetPos( fpos_t * pos );
		virtual char * Gets( char * str, int num );
		virtual bool Open(Mode mode = Mode::M_READ);
		virtual int Putc( int character );
		virtual int Puts( const char * str );
		virtual size_t Read(void * ptr, size_t size, size_t count);
		virtual int Seek( long offset, int origin );
		virtual int Setpos( const fpos_t * pos );
		virtual long Tell();
		virtual size_t Write( const void * ptr, size_t size, size_t count );
		virtual void Rewind();
		virtual void Buffer(char *buffer, size_t size);
		virtual void Buffer(std::string & s);
		virtual void Buffer(std::vector<char>& buffer);
 		CMemory(void* lpBuffer, unsigned long ulLength);
		//bool Open();
		//bool Read(void* lpBuffer, unsigned long ulNumberOfBytesToRead, unsigned long* lpNumberOfBytesRead);
		//bool Write(const void* lpBuffer, unsigned long ulNumberOfBytesToWrite, unsigned long* lpNumberOfBytesWritten);
		//unsigned long Seek( long offset, int origin );
		//bool Close();
		//bool IsEmpty();
	public:
		std::vector<char> m_buffer;
		size_t m_ulCurrentPos;
	};
}