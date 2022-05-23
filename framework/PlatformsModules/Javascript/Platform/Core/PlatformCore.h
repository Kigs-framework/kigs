#ifndef _JAVASCRIPT_CORE_H_
#define _JAVASCRIPT_CORE_H_

#include <stdio.h>

class FileHandle;

template<typename smartPointOn>
class SmartPointer ;

extern bool JSfopen(FileHandle* handle, const char * mode);
extern long int JSfread(void * ptr, long size, long count, FileHandle* handle);
extern long int JSfwrite(const void * ptr, long size, long count, FileHandle* handle);
extern long int JSftell(FileHandle* handle);
extern int JSfseek( FileHandle* handle, long int offset, int origin);
extern int JSfflush( FileHandle* handle);
extern int JSfclose( FileHandle* handle);

extern SmartPointer<FileHandle> JSFindFullName(const kstl::string&	filename);
extern void JSCheckState(FileHandle* handle);
extern void JSCreateFolderTree(FileHandle* hndl);


std::string to_utf8(const wchar_t* buffer, int len);
std::string to_utf8(const std::wstring& str);
std::wstring to_wchar(const char* buffer, int len);
std::wstring to_wchar(const std::string& str);

#endif //_JAVASCRIPT_CORE_H_