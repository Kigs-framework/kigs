#pragma once


//#ifdef _DEBUG

#include <android/log.h>
#define  LOG_TAG    "kigsPrintf"


//#else // no debug
//#define printf(...)
//#endif

#define MEMSIZE_FOR_MEMORY_MANAGER 0x800000

// declare overloaded android fopen / fclose ... methods

namespace Kigs
{

	namespace Core
	{
		template<typename smartPointOn>
		class SmartPointer;
	}
	namespace File
	{
		struct ANDROIDFILE;
		class FileHandle;

		extern bool Android_fopen(FileHandle* handle, const char* mode);

		extern long int Android_fread(void* ptr, long size, long count, FileHandle* handle);
		extern long int Android_fwrite(const void* ptr, long size, long count, FileHandle* handle);
		extern long int Android_ftell(FileHandle* handle);
		extern int Android_fseek(FileHandle* handle, long int offset, int origin);
		extern int Android_fflush(FileHandle* handle);
		extern int Android_fclose(FileHandle* handle);
		extern Core::SmartPointer<FileHandle> Android_FindFullName(const std::string& filename);
		extern void Android_CheckState(FileHandle* handle);
		extern bool Android_CreateFolderTree(FileHandle* hndl);
	}
	namespace Core
	{
		std::string to_utf8(const wchar_t* buffer, int len);
		std::string to_utf8(const std::wstring& str);
		std::wstring to_wchar(const char* buffer, int len);
		std::wstring to_wchar(const std::string& str);

	}
}