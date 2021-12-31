#pragma once

#include "IOperation.h"
#include <stdio.h>
#include <string>

namespace Operation {

	class CFile : public IOperation {
	public:
		explicit CFile(const char* pFilePath);
		~CFile();
	public:
		virtual bool isValid();
        virtual bool IsFile();
		virtual void MFClearErr();
		//int fclose ( FILE * stream );
		virtual bool MFClose();
		//int feof ( FILE * stream );
		virtual int MFEof();
		// int ferror ( FILE * stream );
		virtual int MFError();
		//int fflush ( FILE * stream );
		virtual int MFFlush();
		//int fgetc ( FILE * stream );
		virtual int MFGetc();
		//int fgetpos ( FILE * stream, fpos_t * pos );
		virtual int MFGetPos( fpos_t * pos );
		//char * fgets ( char * str, int num, FILE * stream );
		virtual char * MFGets( char * str, int num );
		//FILE * fopen ( const char * filename, const char * mode );
		virtual bool MFOpen(Mode mode = Mode::M_READ);
		// int fputc ( int character, FILE * stream );
		virtual int MFPutc( int character );
		// int fputs ( const char * str, FILE * stream );
		virtual int MFPuts( const char * str );
		//size_t fread ( void * ptr, size_t size, size_t count, FILE * stream );
		virtual size_t MFRead(void * ptr, size_t size, size_t count);
		//int fseek ( FILE * stream, long int offset, int origin );
		virtual int MFSeek( long offset, int origin );
		//int fsetpos ( FILE * stream, const fpos_t * pos );
		virtual int MFSetpos( const fpos_t * pos );
		//long int ftell ( FILE * stream );	
		virtual long MFTell();
		//size_t fwrite ( const void * ptr, size_t size, size_t count, FILE * stream );
		virtual size_t MFWrite( const void * ptr, size_t size, size_t count );
		//void rewind ( FILE * stream );
		virtual void MFRewind();
		virtual void MFBuffer(char *buffer, size_t size);
		virtual void MFBuffer(std::string & s);
		virtual void MFBuffer(std::vector<char>& buffer);
		/* UnSuport
		int putc ( int character, FILE * stream ); use MFPutc
		int getc ( FILE * stream ); use MFGetc
		FILE * freopen ( const char * filename, const char * mode, FILE * stream );
		void setbuf ( FILE * stream, char * buffer );
		int setvbuf ( FILE * stream, char * buffer, int mode, size_t size );
		int fprintf ( FILE * stream, const char * format, ... );
		int fscanf ( FILE * stream, const char * format, ... );
		int ungetc ( int character, FILE * stream );
		int vfprintf ( FILE * stream, const char * format, va_list arg );
		int vfscanf ( FILE * stream, const char * format, va_list arg );
		*/
	private:
		FILE * m_stream;
		std::string m_strFilePath;
	public:
		char const * MFPath() { return m_strFilePath.c_str(); }
	};
}