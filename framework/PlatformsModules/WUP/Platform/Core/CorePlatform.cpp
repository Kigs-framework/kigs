#include "../../../../Core/Headers/Core.h"

#include "../../../../FileManager/Headers/FilePathManager.h"
#include "Platform/Core/PlatformCore.h"
#include "Platform/Core/PlatformBaseApplication.h"

#include "../../../../Core/Headers/ModuleBase.h"
#include "../../../../Core/Headers/CoreBaseApplication.h"

#include <stdio.h>

#include <codecvt>
#include <locale>

#include <cstdint>
#include <string>
#include <thread>

#include <ppltasks.h>

using namespace concurrency;

#include "winrt/Windows.Storage.Streams.h"
#include "winrt/Windows.Storage.FileProperties.h"

void WUPprintf(const char* fmt, ...)
{
	char str[4096];
	va_list argptr;
	va_start(argptr, fmt);
	vsnprintf_s(str, 4096, fmt, argptr);
	va_end(argptr);
	OutputDebugStringA(str);
}

/*
using namespace ABI::Windows;
using namespace Microsoft::WRL;

HRESULT GetTempFolderPath()
{
	HRESULT hr;
	::ComPtr<Storage::IApplicationDataStatics> applicationDataStatics;

	hr = ABI::Windows::Foundation::GetActivationFactory(Wrappers::HStringReference(RuntimeClass_Windows_Storage_ApplicationData).Get(), &applicationDataStatics);
	if (FAILED(hr))
	{
		return hr;
	}

	ComPtr<Storage::IApplicationData> applicationData;
	hr = applicationDataStatics->get_Current(&applicationData);
	if (FAILED(hr))
	{
		return hr;
	}

	ComPtr<Storage::IStorageFolder> storageFolder;
	hr = applicationData->get_LocalFolder(&storageFolder);
	if (FAILED(hr))
	{
		return hr;
	}

	ComPtr<Storage::IStorageItem> storageItem;
	hr = storageFolder.As(&storageItem);
	if (FAILED(hr))
	{
		return hr;
	}

	HSTRING folderName;
	hr = storageItem->get_Path(&folderName);
	if (FAILED(hr))
	{
		return hr;
	}

	UINT32 length;
	PCWSTR value = WindowsGetStringRawBuffer(folderName, &length);
	//path = value;
	WindowsDeleteString(folderName);
	return S_OK;
}*/

// check if file exist and if it's a file or directory
void Win32CheckState(::FileHandle * hndl)
{
	WIN32_FILE_ATTRIBUTE_DATA InfoFile;
	BOOL ret = ::GetFileAttributesExA(hndl->mFullFileName.c_str(), GetFileExInfoStandard, &InfoFile);
	if (!ret)
		return;

	//int attr = GetFileAttributesA(hndl->mFullFileName.c_str());
	if (InfoFile.dwFileAttributes == -1)
	{
		// file doesn't exist
		hndl->resetStatus();
		return;
	}
	hndl->mStatus |= ::FileHandle::Exist;
	if ((InfoFile.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) != 0)
		hndl->mStatus |= ::FileHandle::IsDIr;
}

bool Win32CreateFolderTree(::FileHandle* hndl)
{
	//TODO
/*	if ((hndl->mStatus&::FileHandle::Exist) == 0)
	{
		SmartPointer<::FileHandle> parent = FilePathManager::Create::FileHandle(FilePathManager::GetParentDirectory(hndl->mFullFileName));
		if (parent->mFullFileName != "")
		{
			Win32CheckState(parent);

			Win32CreateFolderTree(parent);
			return CreateDirectoryA(parent->mFullFileName.c_str(), NULL);
		}
	}*/

	return true;
}


SmartPointer<::FileHandle>	Win32OpenFilePicker()
{
	/*
	auto fop = ref new FileOpenPicker();

	fop->ViewMode = PickerViewMode::List;
	fop->SuggestedStartLocation = PickerLocationId::ComputerFolder;
	fop->FileTypeFilter->Append(".xml");
	
	auto pick_async = fop->PickSingleFileAsync();
	if (!WaitForAsyncOperation(pick_async)) return nullptr;

	auto f = pick_async->GetResults();
	if (!f) return nullptr;

	auto stream_async = f->OpenReadAsync(); 
	if (!WaitForAsyncOperation(stream_async)) return nullptr;
	auto stream = stream_async->GetResults();
	if (!stream) return nullptr;

	auto reader = ref new DataReader(stream);
	if (!WaitForAsyncOperation(reader->LoadAsync(stream->Size))) return nullptr;

	{
		//std::lock_guard<std::mutex> lk{ gPickedFileMutex };
		gPickedFileData.resize(stream->Size);
		reader->ReadBytes(Platform::ArrayReference<u8>(gPickedFileData.data(), stream->Size));
	}
	*/
	return {};
	/*auto p = f->Path;
	usString test = (u16*)p->Data();

	SmartPointer<::FileHandle> hdl = FilePathManager::CreateFileHandle(test.ToString());
	hdl->mFullFileName = test.ToString();
	return hdl;*/
}


SmartPointer<::FileHandle>	Win32FindFullName(const kstl::string&	filename)
{
	
	if (filename[0] != '#')
	{
		return {};
	}
	//TODO
	kstl::string fullFileName = "";

	FilePathManager::DeviceID	id = (FilePathManager::DeviceID)(filename[1]);

	switch (id)
	{
	case FilePathManager::CARD_STORAGE:
		return {};
		break;
	case FilePathManager::DEVICE_STORAGE:
		return {};
		break;
	case FilePathManager::APPLICATION_STORAGE:
	{
		auto local_folder = winrt::Windows::Storage::ApplicationData::Current().LocalFolder();
		auto path = local_folder.Path();
		fullFileName = to_utf8(path.data());
		fullFileName += "\\";
	}
	case FilePathManager::DB_STORAGE:				// Database is in application storage for us on win32
	{
		/*char resultchar[256];
		if (SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, resultchar) == S_OK)
		{
			fullFileName += resultchar;
			fullFileName += "\\";
		}*/
	}
	break;
	case FilePathManager::DOCUMENT_FOLDER:
	{
		// Determine availability of all files within Pictures library.
		//auto fileList = Windows::Storage::KnownFolders::DocumentsLibrary->GetFileAsync();

	}
	break;
	case FilePathManager::DISTANT_FOLDER1:
	case FilePathManager::DISTANT_FOLDER2:
	case FilePathManager::DISTANT_FOLDER3:
	case FilePathManager::DISTANT_FOLDER4:
		break;

	default:
		if ((id >= FilePathManager::RESERVED1) && (id < FilePathManager::DRIVEA))
		{
			return {};
		}
		else if (id >= FilePathManager::DRIVEA)
		{
			fullFileName += (char)('A' + (int)(id - FilePathManager::DRIVEA));
			fullFileName += "://";
		}
	}
	fullFileName += (const char*)(&(filename.c_str()[3]));

	SmartPointer<::FileHandle> result = FilePathManager::CreateFileHandle((const char*)(&(filename.c_str()[3])));

	result->mDeviceID = id;
	result->mFullFileName = fullFileName;
	result->mVirtualFileAccess = new StorageFileFileAccess(nullptr);
	result->mUseVirtualFileAccess = true;

	Win32CheckState(result.get());
	return result;
}

bool Win32fopen(::FileHandle* handle, const char * mode)
{
	unsigned int flagmode = ::FileHandle::OpeningFlags(mode);

	if (handle->mStatus&::FileHandle::Open) // already opened ? return true
	{
		// check if open mode is the same
		if (flagmode == handle->getOpeningFlags())
		{
			return true;
		}
		else
		{
			fclose(handle->mFile);
		}
	}

	// create parent if in write mode
	if (flagmode & ::FileHandle::Write)
	{
		Win32CreateFolderTree(handle);
	}
	
	auto err = _wfopen_s(&handle->mFile, to_wchar(handle->mFullFileName).c_str(), to_wchar(mode).c_str());
	
	if (handle->mFile)
	{
		handle->mStatus |= ::FileHandle::Open;
		handle->mStatus |= ::FileHandle::Exist;
		handle->setOpeningFlags(flagmode);
		return true;
	}
	handle->resetStatus();
	return false;
}

long int Win32fread(void * ptr, long size, long count, ::FileHandle* handle)
{
	return fread(ptr, size, count, handle->mFile);
}

long int Win32fwrite(const void * ptr, long size, long count, ::FileHandle* handle)
{
	return fwrite(ptr, size, count, handle->mFile);
}

long int Win32ftell(::FileHandle* handle)
{
	return ftell(handle->mFile);
}

int Win32fseek(::FileHandle* handle, long int offset, int origin)
{
	return fseek(handle->mFile, offset, origin);
}
int Win32fflush(::FileHandle* handle)
{
	return fflush(handle->mFile);
}
int Win32fclose(::FileHandle* handle)
{
	int result = fclose(handle->mFile);
	handle->resetStatus(); // reset 
	return result;
}

SmartPointer<::FileHandle> Platform_fopen(winrt::Windows::Storage::StorageFile file, const char * mode)
{
	SmartPointer<::FileHandle> fullfilenamehandle;
	usString str = (u16*)file.Name().data();

	fullfilenamehandle = FilePathManager::CreateFileHandle(str.ToString());
	fullfilenamehandle->mFullFileName = str.ToString();

	fullfilenamehandle->mUseVirtualFileAccess = true;
	fullfilenamehandle->mVirtualFileAccess = new StorageFileFileAccess(file);

	Platform_fopen(fullfilenamehandle.get(), mode);

	return fullfilenamehandle;
}


template<typename TResult>
bool WaitForAsyncOperation(winrt::Windows::Foundation::IAsyncOperation<TResult> op)
{
	for (;;)
	{
		auto s = op.Status();
		if (s == winrt::Windows::Foundation::AsyncStatus::Completed)
			break;

		else if (op.Status() != winrt::Windows::Foundation::AsyncStatus::Started)
		{
			return false;
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	return true;
}

std::vector<u8> GetDataFromIBuffer(winrt::Windows::Storage::Streams::IBuffer buf)
{
	auto reader = winrt::Windows::Storage::Streams::DataReader::FromBuffer(buf);
	auto len = buf.Length();
	std::vector<u8> data(reader.UnconsumedBufferLength());

	if (!data.empty())
		reader.ReadBytes(winrt::array_view<uint8_t>(&data[0], &data[0] + data.size()));

	return data;
}

template<typename TResult,typename progressType>
bool WaitForAsyncOperation(winrt::Windows::Foundation::IAsyncOperationWithProgress<TResult, progressType> op)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(5));

	for (;;)
	{
		auto s = op.Status();
		if (s == winrt::Windows::Foundation::AsyncStatus::Completed)
			break;

		else if (op.Status() != winrt::Windows::Foundation::AsyncStatus::Started)
		{
			return false;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(25));
	}
	return true;
}

inline bool WaitForAsyncAction(winrt::Windows::Foundation::IAsyncAction op)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(5));

	for (;;)
	{
		auto s = op.Status();
		if (s == winrt::Windows::Foundation::AsyncStatus::Completed)
			break;

		else if (op.Status() != winrt::Windows::Foundation::AsyncStatus::Started)
		{
			return false;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(25));
	}
	return true;
}


bool		StorageFileFileAccess::Platform_fopen(::FileHandle* handle, const char * mode)
{
	unsigned int flags = ::FileHandle::OpeningFlags(mode);


	if (handle->mDeviceID != FilePathManager::APPLICATION_STORAGE && flags&::FileHandle::Write)
		return false;

	if (!mFile)
	{
		auto local_folder = winrt::Windows::Storage::ApplicationData::Current().LocalFolder();

		auto name = handle->mFileName;
		auto name_platform = to_wchar(name);

		auto get_item = local_folder.TryGetItemAsync(name_platform);
		if (!WaitForAsyncOperation(get_item)) return false;

		auto item = get_item.GetResults();
		mFile = item.as<winrt::Windows::Storage::StorageFile>(); 

		if (!mFile && flags&::FileHandle::Read)
			return false;

		if (!mFile)
		{
			auto get_file = local_folder.CreateFileAsync(name_platform, mode[0] == 'a' ? winrt::Windows::Storage::CreationCollisionOption::OpenIfExists : winrt::Windows::Storage::CreationCollisionOption::ReplaceExisting);
			if (!WaitForAsyncOperation(get_file)) return false;
			mFile = get_file.GetResults();
		}
	}
	

	auto properties = mFile.GetBasicPropertiesAsync();

	if (!WaitForAsyncOperation(properties)) return false;

	//auto readAccess = mFile->OpenReadAsync();
	auto openasync = mFile.OpenAsync(flags&::FileHandle::Write ? winrt::Windows::Storage::FileAccessMode::ReadWrite : winrt::Windows::Storage::FileAccessMode::Read);
	if (!WaitForAsyncOperation(openasync)) return false;
	mFileSize = properties.GetResults().Size();
	mAccessStream = openasync.GetResults();
	handle->setOpeningFlags(flags);

	return true;
}

void		StorageFileFileAccess::setMainThreadID()
{
	mMainThreadID= std::this_thread::get_id();
}

std::thread::id		StorageFileFileAccess::mMainThreadID;


long int	StorageFileFileAccess::Platform_fread(void * ptr, long size, long count, ::FileHandle* handle)
{
	if (mAccessStream)
	{
		long int readSize = size * count;
		
		if (mDataReader == nullptr)
		{
			mDataReader = winrt::Windows::Storage::Streams::DataReader(mAccessStream);
			mDataReader.InputStreamOptions(winrt::Windows::Storage::Streams::InputStreamOptions::ReadAhead);
		}

		if (std::this_thread::get_id() == mMainThreadID)
		{
			WaitForAsyncOperation(mDataReader.LoadAsync(readSize));
		}
		else
		{
			create_task([&]() { mDataReader.LoadAsync(readSize).get(); }).get();
		}

		if (mDataReader.UnconsumedBufferLength() < (readSize))
		{
			readSize = mDataReader.UnconsumedBufferLength();
		}

		if (readSize)
		{
			mDataReader.ReadBytes(winrt::array_view<uint8_t>((uint8_t*)ptr, (uint8_t*)ptr + readSize));
		}

		return readSize / size;
		

		
		/*Windows::Storage::Streams::Buffer^ buffer = ref new Windows::Storage::Streams::Buffer(size*count);
		
		auto resultRead=  mAccessStream->ReadAsync(buffer, size*count, InputStreamOptions::None);

		if (!WaitForAsyncOperation(resultRead)) return false;

		Windows::Storage::Streams::IBuffer^ resultbuffer = resultRead->GetResults();

		auto databuffer=GetDataFromIBuffer(resultbuffer);

		memcpy(ptr, databuffer.data(), resultbuffer->Length);

		return resultbuffer->Length;*/
		
	}

	return -1;
}

long int	StorageFileFileAccess::Platform_fwrite(const void * ptr, long size, long count, ::FileHandle* handle)
{
	if (mAccessStream)
	{
		if (mLastWrite)
			WaitForAsyncOperation(mLastWrite);

		winrt::Windows::Storage::Streams::DataWriter writer{ };
		writer.WriteBytes(winrt::array_view<const uint8_t>{(const uint8_t*)ptr, (const uint8_t*)ptr + (u32)(size*count)});
		mLastWrite = mAccessStream.WriteAsync(writer.DetachBuffer());
		//if (!WaitForAsyncOperation(flush)) return -1;
		return size * count;
	}
	return 0;
}

long int	StorageFileFileAccess::Platform_ftell(::FileHandle* handle)
{
	if (mAccessStream)
	{
		return mAccessStream.Position();
	}
	return -1L;
}

int			StorageFileFileAccess::Platform_fseek(::FileHandle* handle, long int offset, int origin)
{
	if (mAccessStream)
	{
		long int newpos = mAccessStream.Position();
		switch (origin)
		{
		case SEEK_SET:
			newpos = offset;
			break;
		case SEEK_CUR:
			newpos += offset;
			break;
		case SEEK_END:
			newpos = mFileSize - offset;
			break;
		}

		if (newpos < 0)
		{
			newpos = 0;
		}
		if (newpos > mFileSize)
		{
			newpos = mFileSize;
		}

		mAccessStream.Seek(newpos);

		return 0;
	}
	return -1;
}

int			StorageFileFileAccess::Platform_fflush(::FileHandle* handle)
{
	if (mAccessStream)
	{
		auto result= mAccessStream.FlushAsync();
		if (!WaitForAsyncOperation(result)) return -1;

		return 0;
	}
	return -1;
}

int			StorageFileFileAccess::Platform_fclose(::FileHandle* handle)
{
	if (mAccessStream)
	{
		if (mLastWrite)
			WaitForAsyncOperation(mLastWrite);
		
		mLastWrite = nullptr;
		mAccessStream = nullptr;

		mFileSize = 0;

		mDataReader = nullptr;
		//mFile = nullptr;
		handle->resetStatus();
	}
	return 0;
}

PureVirtualFileAccessDelegate* StorageFileFileAccess::MakeCopy()
{
	auto result = new StorageFileFileAccess(mFile);
	return result;
}

std::string to_utf8(const wchar_t* buffer, int len)
{
	int nChars = ::WideCharToMultiByte(
		CP_UTF8,
		0,
		buffer,
		len,
		NULL,
		0,
		NULL,
		NULL);
	if (nChars == 0) return "";

	std::string newbuffer;
	newbuffer.resize(nChars);
	::WideCharToMultiByte(
		CP_UTF8,
		0,
		buffer,
		len,
		const_cast<char*>(newbuffer.c_str()),
		nChars,
		NULL,
		NULL);

	return newbuffer;
}

std::string to_utf8(const std::wstring& str)
{
	return to_utf8(str.c_str(), (int)str.size());
}

std::wstring to_wchar(const char* buffer, int len)
{
	int nChars = ::MultiByteToWideChar(
		CP_UTF8,
		0,
		buffer,
		len,
		NULL,
		0);

	if (nChars == 0) return L"";

	std::wstring newbuffer;
	newbuffer.resize(nChars);
	::MultiByteToWideChar(
		CP_UTF8,
		0,
		buffer,
		len,
		const_cast<wchar_t*>(newbuffer.c_str()),
		nChars);

	return newbuffer;
}

std::wstring to_wchar(const std::string& str)
{
	return to_wchar(str.c_str(), (int)str.size());
}

