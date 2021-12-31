#pragma once

#include <stdio.h>
#include <string>
#include <vector>

namespace Operation {

	enum class Mode { M_READ, M_WRITE };

	class IOperation {
	public:
		IOperation(){};
		virtual ~IOperation(){};
	public:
		virtual bool isValid() = 0;
        virtual bool IsFile() = 0;
		virtual void MFClearErr() {};
		//int fclose ( FILE * stream );
		virtual bool MFClose() = 0;
		//int feof ( FILE * stream );
		virtual int MFEof() = 0;
		// int ferror ( FILE * stream );
		virtual int MFError() {return 0;};
		//int fflush ( FILE * stream );
		virtual int MFFlush() {return 0;};
		//int fgetc ( FILE * stream );
		virtual int MFGetc() = 0;
		//int fgetpos ( FILE * stream, fpos_t * pos );
		virtual int MFGetPos( fpos_t * pos ) = 0;
		//char * fgets ( char * str, int num, FILE * stream );
		virtual char * MFGets( char * str, int num ) = 0;
		//FILE * fopen ( const char * filename, const char * mode );
		virtual bool MFOpen(Mode mode = Mode::M_READ) = 0;
		// int fputc ( int character, FILE * stream );
		virtual int MFPutc( int character ) = 0;
		// int fputs ( const char * str, FILE * stream );
		virtual int MFPuts( const char * str ) = 0;
		//size_t fread ( void * ptr, size_t size, size_t count, FILE * stream );
		virtual size_t MFRead(void * ptr, size_t size, size_t count) = 0;
		//int fseek ( FILE * stream, long int offset, int origin );
		virtual int MFSeek( long offset, int origin ) = 0;
		//int fsetpos ( FILE * stream, const fpos_t * pos );
		virtual int MFSetpos( const fpos_t * pos ) = 0;
		//long int ftell ( FILE * stream );	
		virtual long MFTell() = 0;
		//size_t fwrite ( const void * ptr, size_t size, size_t count, FILE * stream );
		virtual size_t MFWrite( const void * ptr, size_t size, size_t count ) = 0;
		//void rewind ( FILE * stream );
		virtual void MFRewind() = 0;
		virtual void MFBuffer(char *buffer, size_t size) = 0;
		virtual void MFBuffer(std::string & s) = 0;
		virtual void MFBuffer(std::vector<char>& buffer) = 0;
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
	};

    typedef IOperation* LPIOperation;
};
