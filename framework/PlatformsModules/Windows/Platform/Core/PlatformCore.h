#ifndef _WINDOWS_CORE_H_
#define _WINDOWS_CORE_H_

#include <stdio.h>

class FileHandle;

template<typename smartPointOn>
class SmartPointer ;

extern bool Win32fopen(FileHandle* handle, const char * mode);
//extern SmartPointer<FileHandle> Win32fopen(const char* name, const char * mode);
extern long int Win32fread(void * ptr, long size, long count, FileHandle* handle);
extern long int Win32fwrite(const void * ptr, long size, long count, FileHandle* handle);
extern long int Win32ftell(FileHandle* handle);
extern int Win32fseek( FileHandle* handle, long int offset, int origin);
extern int Win32fflush( FileHandle* handle);
extern int Win32fclose( FileHandle* handle);

extern SmartPointer<FileHandle> Win32FindFullName(const std::string&	filename);
extern void Win32CheckState(FileHandle* handle);
extern bool Win32CreateFolderTree(FileHandle* hndl);

std::string to_utf8(const wchar_t* buffer, int len);
std::string to_utf8(const std::wstring& str);
std::wstring to_wchar(const char* buffer, int len);
std::wstring to_wchar(const std::string& str);


#endif //_WINDOWS_CORE_H