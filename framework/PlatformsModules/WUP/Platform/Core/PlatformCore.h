#ifndef _WINDOWS_CORE_H_
#define _WINDOWS_CORE_H_

#include <stdio.h>
#include <mutex>
#include <string>

#include "../../../../FileManager/Headers/PureVirtualFileAccessDelegate.h"

#include <winrt/Windows.Storage.h>

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

extern SmartPointer<FileHandle>	Win32OpenFilePicker();
extern SmartPointer<FileHandle> Win32FindFullName(const kstl::string&	filename);
extern void Win32CheckState(FileHandle* handle);
extern bool Win32CreateFolderTree(FileHandle* hndl);

extern SmartPointer<FileHandle> Platform_fopen(winrt::Windows::Storage::StorageFile file, const char * mode);

class StorageFileFileAccess : public PureVirtualFileAccessDelegate
{
	public:

	StorageFileFileAccess(winrt::Windows::Storage::StorageFile file) : myFile(file)
	{

	}

	virtual bool		Platform_fopen(FileHandle* handle, const char * mode);

	virtual long int	Platform_fread(void * ptr, long size, long count, FileHandle* handle);

	virtual long int	Platform_fwrite(const void * ptr, long size, long count, FileHandle* handle);

	virtual long int	Platform_ftell(FileHandle* handle);

	virtual int			Platform_fseek(FileHandle* handle, long int offset, int origin);

	virtual int			Platform_fflush(FileHandle* handle);

	virtual int			Platform_fclose(FileHandle* handle);

	PureVirtualFileAccessDelegate* MakeCopy() override;


	static	void		setMainThreadID();
	static std::thread::id		myMainThreadID;

protected:

	StorageFileFileAccess()
	{

	}

	virtual ~StorageFileFileAccess()
	{

	}

	winrt::Windows::Storage::StorageFile myFile = nullptr;
	size_t myFileSize = 0;
	winrt::Windows::Storage::Streams::IRandomAccessStream	myAccessStream = nullptr;
	winrt::Windows::Storage::Streams::DataReader			myDataReader = nullptr;
	winrt::Windows::Foundation::IAsyncOperationWithProgress<unsigned int, unsigned int> mLastWrite = nullptr;
	
};

std::string to_utf8(const wchar_t* buffer, int len);
std::string to_utf8(const std::wstring& str);
std::wstring to_wchar(const char* buffer, int len);
std::wstring to_wchar(const std::string& str);


#endif //_WINDOWS_CORE_H