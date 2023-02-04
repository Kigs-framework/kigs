#pragma once

#include <stdio.h>
#include <mutex>
#include <string>

#include "../../../../FileManager/Headers/PureVirtualFileAccessDelegate.h"

#include <winrt/Windows.Storage.h>

namespace Kigs
{
	namespace File
	{
		class FileHandle;
	}
	namespace Core
	{
		template<typename smartPointOn>
		class SmartPointer;

		extern bool Win32fopen(File::FileHandle* handle, const char* mode);
		//extern SmartPointer<FileHandle> Win32fopen(const char* name, const char * mode);
		extern long int Win32fread(void* ptr, long size, long count, File::FileHandle* handle);
		extern long int Win32fwrite(const void* ptr, long size, long count, File::FileHandle* handle);
		extern long int Win32ftell(File::FileHandle* handle);
		extern int Win32fseek(File::FileHandle* handle, long int offset, int origin);
		extern int Win32fflush(File::FileHandle* handle);
		extern int Win32fclose(File::FileHandle* handle);

		extern SmartPointer<File::FileHandle>	Win32OpenFilePicker();
		extern SmartPointer<File::FileHandle> Win32FindFullName(const std::string& filename);
		extern void Win32CheckState(File::FileHandle* handle);
		extern bool Win32CreateFolderTree(File::FileHandle* hndl);

		std::string to_utf8(const wchar_t* buffer, size_t len);
		std::string to_utf8(const std::wstring& str);
		std::wstring to_wchar(const char* buffer, size_t len);
		std::wstring to_wchar(const std::string& str);

	}
	namespace File
	{
		extern Core::SmartPointer<File::FileHandle> Platform_fopen(winrt::Windows::Storage::StorageFile file, const char* mode);

		class StorageFileFileAccess : public PureVirtualFileAccessDelegate
		{
		public:

			StorageFileFileAccess(winrt::Windows::Storage::StorageFile file) : mFile(file)
			{

			}

			virtual bool		Platform_fopen(File::FileHandle* handle, const char* mode) override;

			virtual long int	Platform_fread(void* ptr, long size, long count, File::FileHandle* handle) override;

			virtual long int	Platform_fwrite(const void* ptr, long size, long count, File::FileHandle* handle) override;

			virtual long int	Platform_ftell(File::FileHandle* handle) override;

			virtual int			Platform_fseek(File::FileHandle* handle, long int offset, int origin) override;

			virtual int			Platform_fflush(File::FileHandle* handle) override;

			virtual int			Platform_fclose(File::FileHandle* handle) override;

			virtual bool			Platform_remove(File::FileHandle* handle)  override;

			PureVirtualFileAccessDelegate* MakeCopy() override;


			static	void		setMainThreadID();
			static std::thread::id		mMainThreadID;

		protected:

			StorageFileFileAccess()
			{

			}

			virtual ~StorageFileFileAccess()
			{

			}

			winrt::Windows::Storage::StorageFile mFile = nullptr;
			size_t mFileSize = 0;
			winrt::Windows::Storage::Streams::IRandomAccessStream	mAccessStream = nullptr;
			winrt::Windows::Storage::Streams::DataReader			mDataReader = nullptr;
			winrt::Windows::Foundation::IAsyncOperationWithProgress<unsigned int, unsigned int> mLastWrite = nullptr;

		};
	}

}