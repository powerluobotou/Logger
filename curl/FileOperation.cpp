#include "FileOperation.h"

namespace MemFileOperation{
	CFileOperation::CFileOperation( const char* pFilePath ) {
        m_stream = NULL;
        m_strFilePath = std::string(pFilePath);
        MFOpen();
	}

	CFileOperation::~CFileOperation() {
        MFClose();
	}

    bool CFileOperation::IsFile() {
        return true;
    }

	void CFileOperation::MFClearErr() {
	}

	bool CFileOperation::MFClose() {
		if (m_stream) {
			fclose(m_stream);
		}
		return true;
	}

	int CFileOperation::MFEof() {
		if ( m_stream ) {
			return feof( m_stream );
		}
		return 0x0010;
	}

	int CFileOperation::MFError() {
		if ( m_stream ) {
			return ferror( m_stream );
		}
		return 0x0020;
	}

	int CFileOperation::MFFlush() {
		if ( m_stream ) {
			return fflush( m_stream );
		}
		return -1;
	}

	int CFileOperation::MFGetc() {
		if ( m_stream ) {
			return fgetc( m_stream );
		}
		return -1;
	}

	int CFileOperation::MFGetPos( fpos_t * pos ) {
		if ( m_stream && pos ) {
			return fgetpos( m_stream, pos );
		}
		return -1;
	}

	char * CFileOperation::MFGets( char * str, int num ) {
		if ( m_stream && str ) {
			return fgets( str, num, m_stream );
		}
		return 0;
	}

	bool CFileOperation::MFOpen() {
#ifdef WIN32
#pragma warning(push)
#pragma warning(disable:4996)
#endif
		m_stream = fopen(m_strFilePath.c_str(), "rb");
		
#ifdef WIN32		
#pragma warning(pop)
#endif
		if ( 0 == m_stream ) {
#ifdef WIN32			
#pragma warning(push)
#pragma warning(disable:4996)
#endif
			m_stream = fopen(m_strFilePath.c_str(), "wb+");
#ifdef WIN32			
#pragma warning(pop)
#endif
		}
		return 0 == m_stream ? false : true;
	}

	int CFileOperation::MFPutc( int character ) {
		if ( m_stream ) {
			return fputc( character, m_stream );
		}
		return EOF;
	}

	int CFileOperation::MFPuts( const char * str ) {
		if ( m_stream ) {
			return fputs( str, m_stream );
		}
		return EOF;
	}

	size_t CFileOperation::MFRead( void * ptr, size_t size, size_t count ) {
		if ( m_stream ) {
			return fread( ptr, size, count, m_stream );
		}
		return 0;
	}

	int CFileOperation::MFSeek( long offset, int origin ) {
		if ( m_stream ) {
			return fseek( m_stream, offset, origin );
		}
		return EOF;
	}

	int CFileOperation::MFSetpos( const fpos_t * pos ) {
		if ( m_stream && pos ) {
			return fsetpos( m_stream, pos );
		}
		return EOF;
	}

	long CFileOperation::MFTell() {
		if ( m_stream ) {
			return ftell( m_stream );
		}
		return EOF;
	}

	size_t CFileOperation::MFWrite( const void * ptr, size_t size, size_t count ) {
		if ( m_stream ) {
			size_t n = fwrite( ptr, size, count, m_stream ); 
			if ( n != 0 ) {
				return n;
			}
			int nerror = ferror(m_stream);
			if ( 0x20 != nerror ) {
				return n;
			}
			long lPos = ftell(m_stream);
			if ( 0 == lPos ) {
#ifdef WIN32				
#pragma warning(push)
#pragma warning(disable:4996)
#endif
				m_stream = freopen(m_strFilePath.c_str(), "wb+", m_stream);
#ifdef WIN32				
#pragma warning(pop) 
#endif
				return fwrite( ptr, size, count, m_stream ); 
			}
		}
		return 0;
	}

	void CFileOperation::MFRewind() {
		if ( m_stream ) {
			rewind( m_stream );
		}
	}

	void CFileOperation::MFBuffer(char *buffer, size_t size) {

	}

	void CFileOperation::MFBuffer(std::string & s)
	{

	}
};
