#pragma once

#include <stdio.h>
#include <string>

namespace ToolsInterface{

	class IMemFileOperation {
	public:
		IMemFileOperation(){};
		virtual ~IMemFileOperation(){};
	public:
        virtual bool IsFile() = 0;
		// void clearerr ( FILE * stream )
		// Resets both the error and the eof indicators of the stream.
		/* 
		When a i/o function fails either because of an error or because the end of the file has been reached, 
		one of these internal indicators may be set for the stream. 
		The state of these indicators is cleared by a call to this function, 
		or by a call to any of: rewind, fseek, fsetpos and freopen.
		*/
		virtual void MFClearErr() {};

		// int fclose ( FILE * stream );
		// Closes the file associated with the stream and disassociates it.
		/*
		All internal buffers associated with the stream are disassociated from it and flushed: 
		the content of any unwritten output buffer is written and the content of any unread input buffer is discarded.
		Even if the call fails, the stream passed as parameter will no longer be associated with the file nor its buffers.
		*/
		virtual bool MFClose() = 0;

		// int feof ( FILE * stream );
		// Checks whether the end-of-File indicator associated with stream is set, returning a value different from zero if it is.
		/*
		This indicator is generally set by a previous operation on the stream that attempted to read at or past the end-of-file.
		Notice that stream's internal position indicator may point to the end-of-file for the next operation, 
		but still, the end-of-file indicator may not be set until an operation attempts to read at that point.
		This indicator is cleared by a call to clearerr, rewind, fseek, fsetpos or freopen. 
		Although if the position indicator is not repositioned by such a call, the next i/o operation is likely to set the indicator again.
		*/
		virtual int MFEof() = 0;

		// int ferror ( FILE * stream );
		// Checks if the error indicator associated with stream is set, returning a value different from zero if it is.
		/*
		This indicator is generally set by a previous operation on the stream that failed, and is cleared by a call to clearerr, rewind or freopen.
		*/
		virtual int MFError() {return 0;};

		// int fflush ( FILE * stream );
		// Flush stream
		/*
		If the given stream was open for writing (or if it was open for updating and the last i/o operation was an output operation) any unwritten data in its output buffer is written to the file.
		If stream is a null pointer, all such streams are flushed.
		In all other cases, the behavior depends on the specific library implementation.
		In some implementations, flushing a stream open for reading causes its input buffer to be cleared (but this is not portable expected behavior).
		The stream remains open after this call.
		When a file is closed, either because of a call to fclose or because the program terminates, all the buffers associated with it are automatically flushed.
		*/
		virtual int MFFlush() {return 0;};

		// int fgetc ( FILE * stream );
		// Get character from stream
		/*
		Returns the character currently pointed by the internal file position indicator of the specified stream. 
		The internal file position indicator is then advanced to the next character.
		If the stream is at the end-of-file when called, the function returns EOF and sets the end-of-file indicator for the stream (feof).
		If a read error occurs, the function returns EOF and sets the error indicator for the stream (ferror).
		fgetc and getc are equivalent, except that getc may be implemented as a macro in some libraries.
		*/
		virtual int MFGetc() = 0;

		// int fgetpos ( FILE * stream, fpos_t * pos );
		// Retrieves the current position in the stream.
		/*
		The function fills the fpos_t object pointed by pos with the information needed from the stream's position indicator to restore the stream to its current position (and multibyte state, if wide-oriented) with a call to fsetpos.
		The ftell function can be used to retrieve the current position in the stream as an integer value.
		*/
		virtual int MFGetPos( fpos_t * pos ) = 0;

		// char * fgets ( char * str, int num, FILE * stream );
		// Get string from stream
		/*
		Reads characters from stream and stores them as a C string into str until (num-1) characters have been read or either a newline or the end-of-file is reached, whichever happens first.
		A newline character makes fgets stop reading, but it is considered a valid character by the function and included in the string copied to str.
		A terminating null character is automatically appended after the characters copied to str.
		Notice that fgets is quite different from gets: not only fgets accepts a stream argument, 
		but also allows to specify the maximum size of str and includes in the string any ending newline character.
		*/
		virtual char * MFGets( char * str, int num ) = 0;

		// FILE * fopen ( const char * filename, const char * mode );
		// Open file
		/*
		Opens the file whose name is specified in the parameter filename and associates it with a stream that can be identified in future operations by the FILE pointer returned.
		The operations that are allowed on the stream and how these are performed are defined by the mode parameter.
		The returned stream is fully buffered by default if it is known to not refer to an interactive device (see setbuf).
		The returned pointer can be disassociated from the file by calling fclose or freopen. All opened files are automatically closed on normal program termination.
		The running environment supports at least FOPEN_MAX files open simultaneously.
		*/
		virtual bool MFOpen() = 0;

		// int fputc ( int character, FILE * stream );
		// Writes a character to the stream and advances the position indicator.
		/*
		The character is written at the position indicated by the internal position indicator of the stream, which is then automatically advanced by one.
		*/
		virtual int MFPutc( int character ) = 0;

		// int fputs ( const char * str, FILE * stream );
		// Writes the C string pointed by str to the stream.
		/*
		The function begins copying from the address specified (str) until it reaches the terminating null character ('\0'). 
		This terminating null-character is not copied to the stream.
		Notice that fputs not only differs from puts in that the destination stream can be specified, 
		but also fputs does not write additional characters, while puts appends a newline character at the end automatically.
		*/
		virtual int MFPuts( const char * str ) = 0;

		// size_t fread ( void * ptr, size_t size, size_t count, FILE * stream );
		// Read block of data from stream
		/*
		Reads an array of count elements, each one with a size of size bytes, from the stream and stores them in the block of memory specified by ptr.
		The position indicator of the stream is advanced by the total amount of bytes read.
		The total amount of bytes read if successful is (size*count).
		*/
		virtual size_t MFRead(void * ptr, size_t size, size_t count) = 0;

		// int fseek ( FILE * stream, long int offset, int origin );
		// Sets the position indicator associated with the stream to a new position.
		/*
		For streams open in binary mode, the new position is defined by adding offset to a reference position specified by origin.
		For streams open in text mode, offset shall either be zero or a value returned by a previous call to ftell, and origin shall necessarily be SEEK_SET.
		If the function is called with other values for these arguments, support depends on the particular system and library implementation (non-portable).
		The end-of-file internal indicator of the stream is cleared after a successful call to this function, and all effects from previous calls to ungetc on this stream are dropped.
		On streams open for update (read+write), a call to fseek allows to switch between reading and writing.
		*/
		virtual int MFSeek( long offset, int origin ) = 0;

		// int fsetpos ( FILE * stream, const fpos_t * pos );
		// Restores the current position in the stream to pos.
		/*
		The internal file position indicator associated with stream is set to the position represented by pos, 
		which is a pointer to an fpos_t object whose value shall have been previously obtained by a call to fgetpos.
		The end-of-file internal indicator of the stream is cleared after a successful call to this function, 
		and all effects from previous calls to ungetc on this stream are dropped.
		On streams open for update (read+write), a call to fsetpos allows to switch between reading and writing.
		A similar function, fseek, can be used to set arbitrary positions on streams open in binary mode.
		*/
		virtual int MFSetpos( const fpos_t * pos ) = 0;

		// long int ftell ( FILE * stream );
		// Returns the current value of the position indicator of the stream.
		/*
		For binary streams, this is the number of bytes from the beginning of the file.
		For text streams, the numerical value may not be meaningful but can still be used to restore the position to the same position later using fseek 
		(if there are characters put back using ungetc still pending of being read, the behavior is undefined).
		*/		
		virtual long MFTell() = 0;

		// size_t fwrite ( const void * ptr, size_t size, size_t count, FILE * stream );
		// Write block of data to stream
		/*
		Writes an array of count elements, each one with a size of size bytes, from the block of memory pointed by ptr to the current position in the stream.
		The position indicator of the stream is advanced by the total number of bytes written.
		Internally, the function interprets the block pointed by ptr as if it was an array of (size*count) elements of type unsigned char, 
		and writes them sequentially to stream as if fputc was called for each byte.
		*/
		virtual size_t MFWrite( const void * ptr, size_t size, size_t count ) = 0;
		
		// void rewind ( FILE * stream );
		// Sets the position indicator associated with stream to the beginning of the file.
		/*
		The end-of-file and error internal indicators associated to the stream are cleared after a successful call to this function, 
		and all effects from previous calls to ungetc on this stream are dropped.
		On streams open for update (read+write), a call to rewind allows to switch between reading and writing.
		*/
		virtual void MFRewind() = 0;

		virtual void MFBuffer(char *buffer, size_t size) = 0;

		virtual void MFBuffer(std::string & s) = 0;
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

    typedef IMemFileOperation* LPIMemFileOperation;
};
