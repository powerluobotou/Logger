#include "Mem.h"
#include <string.h>

namespace Operation {

	CMemory::CMemory() {
		currentPos_ = 0;
		buffer_.clear();
	}

	CMemory::~CMemory() {
	}

	bool CMemory::Valid() {
		return buffer_.size() > 0;
	}

    bool CMemory::IsFile() {
        return false;
    }

	bool CMemory::Close() {
		return true;
	}

	int CMemory::Eof() {
		return currentPos_ == buffer_.size() ?  -1 : 0 ;
	}

	int CMemory::Getc() {
		int nget = 0;
		if ( 0 == Read( &nget, sizeof(char), 1) ) {
			return EOF;
		}
		return nget;
	}

	int CMemory::GetPos( fpos_t * pos ) {
		#if WIN32
		*pos = currentPos_;
		#else
		 pos->__pos = currentPos_;
		#endif
		return 0;
	}

	char * CMemory::Gets( char * str, int num ) {
		char * szReadBuffer = 0;
		do {
			if (!str || currentPos_ >= buffer_.size()) {
				break;
			}

			size_t unReaded = 0;
			if (currentPos_ + num <= buffer_.size()) {
				unReaded = num;
			}
			else {
				unReaded = buffer_.size() - currentPos_;
			}

			if ( 0 == unReaded ) {
				break;
			}

			size_t index = 0;
			szReadBuffer = str;
			for (;index < unReaded; index++) {
				memcpy(szReadBuffer + index, buffer_.data() + currentPos_ + index, 1);
				if ( '\n' == *(buffer_.data() + currentPos_ + index) ) {
					break; 
				}
			}
			
			currentPos_ += index;
		} while (0);

		return szReadBuffer;
	}

	bool CMemory::Open(Mode mode) {
		currentPos_ = 0;
		return true;
	}

	int CMemory::Putc( int character ) {
		size_t unCount = sizeof(character);
		Write( &character, sizeof(char), unCount );
		return 0;
	}

	int CMemory::Puts( const char * str ) {
		size_t unCount = strlen(str);
		Write( str, sizeof(char), unCount );
		return 0;
	}

	size_t CMemory::Read( void * ptr, size_t size, size_t count ) {

		unsigned int unReaded = 0;
		do {
			if ( !ptr || currentPos_ >= buffer_.size() ) {
				break;
			}

			unsigned int unNeedRead = size * count;
			if ( currentPos_ + unNeedRead <= buffer_.size() ) {
				unReaded = unNeedRead;
			}
			else {
				unReaded = buffer_.size() - currentPos_;
			}

			if ( 0 == unReaded ) {
				break;
			}

			memcpy(ptr, buffer_.data() + currentPos_, unReaded);
			currentPos_ += unReaded;
		} while (0);
		
		return unReaded;
	}

	int CMemory::Seek( long offset, int origin ) {
		
		int nRet = EOF;
		if ( offset >= 0 ) {
			if ( SEEK_CUR == origin ) {
				currentPos_ = currentPos_ + offset;
				nRet = 0;
			}
			else if ( SEEK_SET == origin ) {
				currentPos_ = offset;
				nRet = 0;
			}
            else if (SEEK_END == origin) {
                currentPos_ =  buffer_.size() + offset;
                nRet = 0;
            }
		}
		else {
			if ( SEEK_CUR == origin) {
#ifdef WIN32
#pragma warning(push)
#pragma warning(disable:4018)
#endif
				if ( currentPos_ > abs(offset) ) {
					currentPos_ = currentPos_ - abs(offset);
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
                if ( buffer_.size() > abs(offset) ) {
                    currentPos_ = buffer_.size() - abs(offset);
                    nRet = 0;
                }
                else {
                    currentPos_ = 0;
                }
#ifdef WIN32				
#pragma warning(pop) 
#endif
            }
		}
		return nRet;

	}

	int CMemory::Setpos( const fpos_t * pos ) {
		
#ifdef WIN32			
#pragma warning(push)
#pragma warning(disable:4244)
#endif

#ifdef WIN32
		currentPos_ = *pos;
#else
	   currentPos_ =pos->__pos; 
#endif
		
#ifdef WIN32			
#pragma warning(pop) 
#endif
		return 0;
	}

	long CMemory::Tell() {
		return currentPos_;
	}

	size_t CMemory::Write( const void * ptr, size_t size, size_t count ) {
		
		size_t unWritten = 0;
		do {
			size_t unNeedWrite = size * count;
			if ( 0 == unNeedWrite ) {
				break;
			}

			size_t unRealPos = currentPos_;
			if ( unRealPos > buffer_.size() ) {
				unRealPos = buffer_.size();
			}

			size_t unOriCount = buffer_.size(); 
			if ( unRealPos + unNeedWrite > unOriCount ) {
				buffer_.resize( unRealPos + unNeedWrite );
			}

			void* pDest = (void *)memcpy(buffer_.data() + unRealPos, (char*)ptr, unNeedWrite );
			if ( (void *)-1 == pDest ) {
				buffer_.resize( unOriCount );
			}
			else {
				unWritten = unNeedWrite;
				unRealPos += unWritten;
				currentPos_ = unRealPos;
			}
		} while (0);
		return unWritten;
	}

	void CMemory::Rewind() {
		currentPos_ = 0;
	}

	void CMemory::Buffer(char *buffer, size_t size)
	{
		memset(buffer, 0, size);
		memcpy(buffer, buffer_.data(), Tell());
	}

	void CMemory::Buffer(std::string & s)
	{
		s.clear();
		s.append((char *)buffer_.data(), (long)Tell());
	}

	void CMemory::Buffer(std::vector<char>& buffer) {
	}

	CMemory::CMemory( void* lpBuffer, unsigned long ulLength ) : currentPos_(0)
	{
		buffer_.resize(ulLength);
		memcpy(buffer_.data(), lpBuffer, ulLength);
	}
// 
// 
// 	bool CMemory::Open() {
// 		return true;
// 	}
// 
// 	bool CMemory::Read( void* lpBuffer, unsigned long ulNumberOfBytesToRead, unsigned long* lpNumberOfBytesRead ) {
// 		bool bRet = false;
// 		unsigned long dwReaded;
// 
// 		do {
// 			if ( !lpBuffer || !ulNumberOfBytesToRead ) {
// 				break;
// 			}
// 
// 			if ( currentPos_ + ulNumberOfBytesToRead <= buffer_.GetCount() ) {
// 				dwReaded = ulNumberOfBytesToRead;
// 			}
// 			else {
// 				dwReaded = (unsigned long)buffer_.GetCount() - currentPos_;
// 			}
// 
// 			if ( 0 == dwReaded ) {
// 				break;
// 			}
// 
// 			memcpy(lpBuffer, buffer_.GetData() + currentPos_, dwReaded);
// 			currentPos_ += dwReaded;
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
// 	bool CMemory::Write( const void* lpBuffer, unsigned long ulNumberOfBytesToWrite, unsigned long* lpNumberOfBytesWritten )
// 	{
// 		bool bRet = false;
// 		unsigned long dwWritten = ulNumberOfBytesToWrite;
// 
// 		do {
// 			if ( !lpBuffer || !ulNumberOfBytesToWrite ) {
// 				break;
// 			}
// 
// 			if ( currentPos_ + ulNumberOfBytesToWrite > buffer_.GetCount() ) {
// 				buffer_.SetCount(currentPos_ + ulNumberOfBytesToWrite);
// 			}
// 
// 			memcpy(buffer_.GetData() + currentPos_, lpBuffer, dwWritten);
// 			currentPos_ += dwWritten;
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
// 	unsigned long CMemory::Seek( long offset, int origin )
// 	{
// 		unsigned long retval = INVALID_SET_FILE_POINTER;
// 
// 		do {
// 			if ( FILE_CURRENT == origin ) {
// 				if ( offset > 0 ) {
// 					if ( currentPos_ + offset < buffer_.GetCount() ) {
// 						currentPos_ += offset;
// 					}
// 					else {
// 						break;
// 					}
// 				}
// 				else {
// 					if (currentPos_ >= offset) {
// 						currentPos_ += offset;
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
// 				if ( buffer_.GetCount() + offset < 0 ) {
// 					break;
// 				}
// 				currentPos_ = (unsigned long)buffer_.GetCount() + offset;
// 			}
// 			else if (FILE_BEGIN == origin) {
// 				if ( offset < 0 ) {
// 					break;
// 				}
// 
// 				if ( offset >= buffer_.GetCount() ) {
// 					break;
// 				}
// 
// 				currentPos_ = offset;
// 			}
// 		} while (0);
// 
// 		return currentPos_;
// 	}
// 
// 	bool CMemory::Close() {
// 		return true;
// 	}
// 
// 	bool CMemory::IsEmpty() {
// 		return buffer_.GetCount() == 0 ? true : false;
// 	}
}