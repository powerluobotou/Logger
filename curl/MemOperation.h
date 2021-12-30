// Copyright (c) 2010 Kingsoft Corporation. All rights reserved.
// Copyright (c) 2010 The KSafe Authors. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#pragma once

#include "IMemFileOperation.h"
//#include <atlbase.h>
//#include <atlcoll.h>
#include<vector>
#include <string>
using namespace std;

namespace MemFileOperation{
	using namespace ToolsInterface;

	class CMemOperation : public IMemFileOperation
	{
	public:
		CMemOperation();
		~CMemOperation();
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
 		CMemOperation(void* lpBuffer, unsigned long ulLength);
// 		bool MFOpen();
// 		bool MFRead(void* lpBuffer, unsigned long ulNumberOfBytesToRead, unsigned long* lpNumberOfBytesRead);
// 		bool MFWrite(const void* lpBuffer, unsigned long ulNumberOfBytesToWrite, unsigned long* lpNumberOfBytesWritten);
// 		unsigned long MFSeek( long offset, int origin );
// 		bool MFClose();
// 		bool IsEmpty();
	public:
		//CAtlArray<BYTE> m_buffer;
		vector<char> m_buffer;
		size_t m_ulCurrentPos;
	};
}