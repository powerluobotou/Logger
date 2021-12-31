#include "Mem.h"
#include <string.h>

namespace Operation {

	CMemory::CMemory() {
		m_ulCurrentPos = 0;
		m_buffer.clear();
	}

	CMemory::~CMemory() {
	}
    
    bool CMemory::IsFile() {
        return false;
    }

	bool CMemory::MFClose() {
		return true;
	}

	int CMemory::MFEof() {
		return m_ulCurrentPos == m_buffer.size() ?  -1 : 0 ;
	}

	int CMemory::MFGetc() {
		int nget = 0;
		if ( 0 == MFRead( &nget, sizeof(char), 1) ) {
			return EOF;
		}
		return nget;
	}

	int CMemory::MFGetPos( fpos_t * pos ) {
		#if WIN32
		*pos = m_ulCurrentPos;
		#else
		 pos->__pos = m_ulCurrentPos;
		#endif
		return 0;
	}

	char * CMemory::MFGets( char * str, int num ) {
		char * szReadBuffer = 0;
		do {
			if (!str || m_ulCurrentPos >= m_buffer.size()) {
				break;
			}

			size_t unReaded = 0;
			if (m_ulCurrentPos + num <= m_buffer.size()) {
				unReaded = num;
			}
			else {
				unReaded = m_buffer.size() - m_ulCurrentPos;
			}

			if ( 0 == unReaded ) {
				break;
			}

			size_t index = 0;
			szReadBuffer = str;
			for (;index < unReaded; index++) {
				memcpy(szReadBuffer + index, m_buffer.data() + m_ulCurrentPos + index, 1);
				if ( '\n' == *(m_buffer.data() + m_ulCurrentPos + index) ) {
					break; 
				}
			}
			
			m_ulCurrentPos += index;
		} while (0);

		return szReadBuffer;
	}

	bool CMemory::MFOpen() {
		m_ulCurrentPos = 0;
		return true;
	}

	int CMemory::MFPutc( int character ) {
		size_t unCount = sizeof(character);
		MFWrite( &character, sizeof(char), unCount );
		return 0;
	}

	int CMemory::MFPuts( const char * str ) {
		size_t unCount = strlen(str);
		MFWrite( str, sizeof(char), unCount );
		return 0;
	}

	size_t CMemory::MFRead( void * ptr, size_t size, size_t count ) {

		unsigned int unReaded = 0;
		do {
			if ( !ptr || m_ulCurrentPos >= m_buffer.size() ) {
				break;
			}

			unsigned int unNeedRead = size * count;
			if ( m_ulCurrentPos + unNeedRead <= m_buffer.size() ) {
				unReaded = unNeedRead;
			}
			else {
				unReaded = m_buffer.size() - m_ulCurrentPos;
			}

			if ( 0 == unReaded ) {
				break;
			}

			memcpy(ptr, m_buffer.data() + m_ulCurrentPos, unReaded);
			m_ulCurrentPos += unReaded;
		} while (0);
		
		return unReaded;
	}

	int CMemory::MFSeek( long offset, int origin ) {
		
		int nRet = EOF;
		if ( offset >= 0 ) {
			if ( SEEK_CUR == origin ) {
				m_ulCurrentPos = m_ulCurrentPos + offset;
				nRet = 0;
			}
			else if ( SEEK_SET == origin ) {
				m_ulCurrentPos = offset;
				nRet = 0;
			}
            else if (SEEK_END == origin) {
                m_ulCurrentPos =  m_buffer.size() + offset;
                nRet = 0;
            }
		}
		else {
			if ( SEEK_CUR == origin) {
#ifdef WIN32
#pragma warning(push)
#pragma warning(disable:4018)
#endif
				if ( m_ulCurrentPos > abs(offset) ) {
					m_ulCurrentPos = m_ulCurrentPos - abs(offset);
					nRet = 0;
				}
#ifdef WIN32				
#pragma warning(pop) 
#endif
			}
            else if ( SEEK_CUR == origin || SEEK_END == origin ) {
#ifdef WIN32				
#pragma warning(push)
#pragma warning(disable:4018)
#endif
                if ( m_buffer.size() > abs(offset) ) {
                    m_ulCurrentPos = m_buffer.size() - abs(offset);
                    nRet = 0;
                }
                else {
                    m_ulCurrentPos = 0;
                }
#ifdef WIN32				
#pragma warning(pop) 
#endif
            }
		}
		return nRet;

	}

	int CMemory::MFSetpos( const fpos_t * pos ) {
		
#ifdef WIN32			
#pragma warning(push)
#pragma warning(disable:4244)
#endif

#ifdef WIN32
		m_ulCurrentPos = *pos;
#else
	   m_ulCurrentPos =pos->__pos; 
#endif
		
#ifdef WIN32			
#pragma warning(pop) 
#endif
		return 0;
	}

	long CMemory::MFTell() {
		return m_ulCurrentPos;
	}

	size_t CMemory::MFWrite( const void * ptr, size_t size, size_t count ) {
		
		size_t unWritten = 0;
		do {
			size_t unNeedWrite = size * count;
			if ( 0 == unNeedWrite ) {
				break;
			}

			size_t unRealPos = m_ulCurrentPos;
			if ( unRealPos > m_buffer.size() ) {
				unRealPos = m_buffer.size();
			}

			size_t unOriCount = m_buffer.size(); 
			if ( unRealPos + unNeedWrite > unOriCount ) {
				m_buffer.resize( unRealPos + unNeedWrite );
			}

			void* pDest = (void *)memcpy(m_buffer.data() + unRealPos, (char*)ptr, unNeedWrite );
			if ( (void *)-1 == pDest ) {
				m_buffer.resize( unOriCount );
			}
			else {
				unWritten = unNeedWrite;
				unRealPos += unWritten;
				m_ulCurrentPos = unRealPos;
			}
		} while (0);
		return unWritten;
	}

	void CMemory::MFRewind() {
		m_ulCurrentPos = 0;
	}

	void CMemory::MFBuffer(char *buffer, size_t size)
	{
		memset(buffer, 0, size);
		memcpy(buffer, m_buffer.data(), MFTell());
	}

	void CMemory::MFBuffer(std::string & s)
	{
		s.clear();
		s.append((char *)m_buffer.data(), (long)MFTell());
	}

	CMemory::CMemory( void* lpBuffer, unsigned long ulLength ) : m_ulCurrentPos(0)
	{
		m_buffer.resize(ulLength);
		memcpy(m_buffer.data(), lpBuffer, ulLength);
	}

// 
// 
// 	bool CMemory::MFOpen() {
// 		return true;
// 	}
// 
// 	bool CMemory::MFRead( void* lpBuffer, unsigned long ulNumberOfBytesToRead, unsigned long* lpNumberOfBytesRead ) {
// 		bool bRet = false;
// 		unsigned long dwReaded;
// 
// 		do {
// 			if ( !lpBuffer || !ulNumberOfBytesToRead ) {
// 				break;
// 			}
// 
// 			if ( m_ulCurrentPos + ulNumberOfBytesToRead <= m_buffer.GetCount() ) {
// 				dwReaded = ulNumberOfBytesToRead;
// 			}
// 			else {
// 				dwReaded = (unsigned long)m_buffer.GetCount() - m_ulCurrentPos;
// 			}
// 
// 			if ( 0 == dwReaded ) {
// 				break;
// 			}
// 
// 			memcpy(lpBuffer, m_buffer.GetData() + m_ulCurrentPos, dwReaded);
// 			m_ulCurrentPos += dwReaded;
// 
// 			if ( lpNumberOfBytesRead ) {
// 				*lpNumberOfBytesRead = dwReaded;
// 			}
// 
// 			bRet = true;
// 		} while (0);
// 		
// 		return bRet;
// 	}
// 
// 	bool CMemory::MFWrite( const void* lpBuffer, unsigned long ulNumberOfBytesToWrite, unsigned long* lpNumberOfBytesWritten )
// 	{
// 		bool bRet = false;
// 		unsigned long dwWritten = ulNumberOfBytesToWrite;
// 
// 		do {
// 			if ( !lpBuffer || !ulNumberOfBytesToWrite ) {
// 				break;
// 			}
// 
// 			if ( m_ulCurrentPos + ulNumberOfBytesToWrite > m_buffer.GetCount() ) {
// 				m_buffer.SetCount(m_ulCurrentPos + ulNumberOfBytesToWrite);
// 			}
// 
// 			memcpy(m_buffer.GetData() + m_ulCurrentPos, lpBuffer, dwWritten);
// 			m_ulCurrentPos += dwWritten;
// 
// 			if ( lpNumberOfBytesWritten ) {
// 				*lpNumberOfBytesWritten = dwWritten;
// 			}
// 
// 			bRet = true;
// 		} while (0);
// 		
// 		return bRet;
// 	}
// 
// 	unsigned long CMemory::MFSeek( long offset, int origin )
// 	{
// 		unsigned long retval = INVALID_SET_FILE_POINTER;
// 
// 		do {
// 			if ( FILE_CURRENT == origin ) {
// 				if ( offset > 0 ) {
// 					if ( m_ulCurrentPos + offset < m_buffer.GetCount() ) {
// 						m_ulCurrentPos += offset;
// 					}
// 					else {
// 						break;
// 					}
// 				}
// 				else {
// 					if (m_ulCurrentPos >= offset) {
// 						m_ulCurrentPos += offset;
// 					}
// 					else {
// 						break;
// 					}
// 				}
// 			}
// 			else if (FILE_END == origin) {
// 				if ( offset > 0 ) {
// 					break;
// 				}
// 
// 				if ( m_buffer.GetCount() + offset < 0 ) {
// 					break;
// 				}
// 				m_ulCurrentPos = (unsigned long)m_buffer.GetCount() + offset;
// 			}
// 			else if (FILE_BEGIN == origin) {
// 				if ( offset < 0 ) {
// 					break;
// 				}
// 
// 				if ( offset >= m_buffer.GetCount() ) {
// 					break;
// 				}
// 
// 				m_ulCurrentPos = offset;
// 			}
// 		} while (0);
// 
// 		return m_ulCurrentPos;
// 	}
// 
// 	bool CMemory::MFClose() {
// 		return true;
// 	}
// 
// 	bool CMemory::IsEmpty() {
// 		return m_buffer.GetCount() == 0 ? true : false;
// 	}

};

