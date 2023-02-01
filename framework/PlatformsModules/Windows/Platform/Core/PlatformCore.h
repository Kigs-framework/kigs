#pragma once

#include "SmartPointer.h"

#include <stdio.h>

namespace Kigs
{
	namespace File
	{
		class FileHandle;
	}
	namespace Core
	{
		extern bool Win32fopen(File::FileHandle* handle, const char* mode);
		//extern SmartPointer<FileHandle> Win32fopen(const char* name, const char * mode);
		extern long int Win32fread(void* ptr, long size, long count, File::FileHandle* handle);
		extern long int Win32fwrite(const void* ptr, long size, long count, File::FileHandle* handle);
		extern long int Win32ftell(File::FileHandle* handle);
		extern int Win32fseek(File::FileHandle* handle, long int offset, int origin);
		extern int Win32fflush(File::FileHandle* handle);
		extern int Win32fclose(File::FileHandle* handle);

		extern SmartPointer<File::FileHandle> Win32FindFullName(const std::string& filename);
		extern void Win32CheckState(File::FileHandle* handle);
		extern bool Win32CreateFolderTree(File::FileHandle* hndl);

		std::string to_utf8(const wchar_t* buffer, size_t len);
		std::string to_utf8(const std::wstring& str);
		std::wstring to_wchar(const char* buffer, size_t len);
		std::wstring to_wchar(const std::string& str);

	}
}