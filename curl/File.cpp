#include "File.h"
#include "../Logger.h"

namespace Operation {

	CFile::CFile(const char* pFilePath)
		: m_stream(NULL)
		, m_strFilePath(pFilePath) {
		Open();
	}

	CFile::~CFile() {
		Close();
	}
	
	bool CFile::Valid() {
		return m_stream != NULL;
	}
	
	bool CFile::IsFile() {
		return true;
	}

	void CFile::ClearErr() {
	}

	bool CFile::Close() {
		if (m_stream) {
			fclose(m_stream);
			m_stream = NULL;
		}
		return true;
	}

	int CFile::Eof() {
		if (m_stream) {
			//http://c.biancheng.net/cpp/html/2514.html
			return feof(m_stream);
		}
		return 0x0010;
	}

	int CFile::Error() {
		if (m_stream) {
			return ferror(m_stream);
		}
		return 0x0020;
	}

	int CFile::Flush() {
		if (m_stream) {
			return fflush(m_stream);
		}
		return -1;
	}

	int CFile::Getc() {
		if (m_stream) {
			return fgetc(m_stream);
		}
		return -1;
	}

	int CFile::GetPos(fpos_t* pos) {
		if (m_stream && pos) {
			return fgetpos(m_stream, pos);
		}
		return -1;
	}

	char* CFile::Gets(char* str, int num) {
		if (m_stream && str) {
			return fgets(str, num, m_stream);
		}
		return 0;
	}

	bool CFile::Open(Mode mode) {
#ifdef WIN32
#pragma warning(push)
#pragma warning(disable:4996)
#endif
		Close();
		if (mode == Mode::M_WRITE) {
			m_stream = fopen(m_strFilePath.c_str(), "wb+");
			//if (!m_stream) {
			//	PLOG_ERROR("文件可能被占用，写方式打开失败");
			//}
			return m_stream != NULL;
		}
		m_stream = fopen(m_strFilePath.c_str(), "rb");

#ifdef WIN32
#pragma warning(pop)
#endif
		if (0 == m_stream) {
#ifdef WIN32
#pragma warning(push)
#pragma warning(disable:4996)
#endif
			m_stream = fopen(m_strFilePath.c_str(), "wb+");
#ifdef WIN32
#pragma warning(pop)
#endif
		}
		return m_stream != NULL;
	}

	int CFile::Putc(int character) {
		if (m_stream) {
			return fputc(character, m_stream);
		}
		return EOF;
	}

	int CFile::Puts(const char* str) {
		if (m_stream) {
			return fputs(str, m_stream);
		}
		return EOF;
	}

	size_t CFile::Read(void* ptr, size_t size, size_t count) {
		if (m_stream) {
			//fread(ptr, 1, length, m_stream);
			return fread(ptr, size, count, m_stream);
		}
		return 0;
	}

	int CFile::Seek(long offset, int origin) {
		if (m_stream) {
			//SEEK_SET/SEEK_CUR/SEEK_END
			return fseek(m_stream, offset, origin);
		}
		return EOF;
	}

	int CFile::Setpos(const fpos_t* pos) {
		if (m_stream && pos) {
			return fsetpos(m_stream, pos);
		}
		return EOF;
	}

	long CFile::Tell() {
		if (m_stream) {
			//fseek(m_stream, 0L, SEEK_END);
			return ftell(m_stream);
		}
		return EOF;
	}

	size_t CFile::Write(const void* ptr, size_t size, size_t count) {
		if (m_stream) {
			size_t n = fwrite(ptr, size, count, m_stream);
			if (n != 0) {
				return n;
			}
			//写失败，可能读方式打开
#ifdef WIN32
#else
			int nerror = ferror(m_stream);
			if (0x20 != nerror) {
				return n;
			}
#endif
			long lPos = ftell(m_stream);
			if (0 == lPos) {
#ifdef WIN32
#pragma warning(push)
#pragma warning(disable:4996)
#endif
				//写方式重新打开
				m_stream = freopen(m_strFilePath.c_str(), "wb+", m_stream);
#ifdef WIN32
#pragma warning(pop)
#endif
				//if (!m_stream) {
				//	PLOG_ERROR("文件可能被占用，写方式打开失败");
				//}
				return m_stream ? fwrite(ptr, size, count, m_stream) : 0;
			}
		}
		return 0;
	}

	void CFile::Rewind() {
		if (m_stream) {
			rewind(m_stream);
		}
	}

	void CFile::Buffer(char* buffer, size_t size) {
	}

	void CFile::Buffer(std::string& s) {
	}

	void CFile::Buffer(std::vector<char>& buffer) {
		buffer.clear();
		if (m_stream) {
			fseek(m_stream, 0L, SEEK_END);
			size_t length = ftell(m_stream);
			if (length > 0) {
				buffer.resize(length);
				fseek(m_stream, 0L, SEEK_SET);
				size_t n = fread(&buffer.front(), 1, length, m_stream);
				(void)n;
			}
		}
	}
};
