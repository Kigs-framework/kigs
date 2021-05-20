#include "PrecompiledHeaders.h"
#include "../../../../Core/Headers/Core.h"
#include "../../../../FileManager/Headers/FilePathManager.h"
#include "Platform/Core/PlatformCore.h"
#include "Platform/Core/PlatformBaseApplication.h"

#include "../../../../Core/Headers/ModuleBase.h"
#include "../../../../Core/Headers/CoreBaseApplication.h"

#ifdef KIGS_HTTPREQUEST_AVAILABLE
#include "../../../../HTTPRequest/Headers/HTTPConnect.h"
#endif

#include <shlobj.h>
#include <Shlwapi.h>
#include <direct.h>

// check if file exist and if it's a file or directory
void Win32CheckState(FileHandle * hndl)
{
	std::wstring wfilename = to_wchar(hndl->mFullFileName);
	int attr = GetFileAttributesW(wfilename.c_str());
	if (attr == -1)
	{
		// file doesn't exist
		hndl->resetStatus();
		return;
	}
	hndl->mStatus |= FileHandle::Exist;
	if ((attr&FILE_ATTRIBUTE_DIRECTORY) != 0)
		hndl->mStatus |= FileHandle::IsDIr;

}

bool Win32CreateFolderTree(FileHandle* hndl)
{
	if ((hndl->mStatus&FileHandle::Exist) == 0)
	{
		SmartPointer<FileHandle> parent = FilePathManager::CreateFileHandle(FilePathManager::GetParentDirectory(hndl->mFullFileName));
		if (parent->mFullFileName != "")
		{
			Win32CheckState(parent.get());
			Win32CreateFolderTree(parent.get());
			std::wstring wfilename = to_wchar(parent->mFullFileName);
			return CreateDirectoryW(wfilename.c_str(), NULL);
		}
	}

	return true;
}

SmartPointer<FileHandle> Win32FindFullName(const kstl::string&	filename)
{
	if (filename[0] != '#')
	{
		return 0;
	}

	kstl::string fullFileName = "";

	FilePathManager::DeviceID	id = (FilePathManager::DeviceID)(filename[1]);

	switch (id)
	{
	case FilePathManager::CARD_STORAGE:
		return 0;
		break;
	case FilePathManager::DEVICE_STORAGE:
		return 0;
		break;
	case FilePathManager::APPLICATION_STORAGE:
	case FilePathManager::DB_STORAGE:				// Database is in application storage for us on win32
	{
		char resultchar[256];
		if (SHGetFolderPathA(NULL, CSIDL_APPDATA, NULL, 0, resultchar) == S_OK)
		{
			fullFileName += resultchar;
			fullFileName += "\\";
		}
	}
	break;
	case FilePathManager::DOCUMENT_FOLDER:
	{
		char resultchar[256];
		if (SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, 0, resultchar) == S_OK)
		{
			fullFileName += resultchar;
			fullFileName += "\\";
		}
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
			return 0;
		}
		else if (id >= FilePathManager::DRIVEA)
		{
			fullFileName += (char)('A' + (int)(id - FilePathManager::DRIVEA));
			fullFileName += "://";
		}
	}
	fullFileName += (const char*)(&(filename.c_str()[3]));

	SmartPointer<FileHandle> result = FilePathManager::CreateFileHandle((const char*)(&(filename.c_str()[3])));

	result->mDeviceID = id;
	result->mFullFileName = fullFileName;
	Win32CheckState(result.get());
	return result;
}

bool Win32fopen(FileHandle* handle, const char * mode)
{
	unsigned int flagmode = FileHandle::OpeningFlags(mode);

	if (handle->mStatus&FileHandle::Open) // already opened ? return true
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
	if (flagmode & FileHandle::Write)
	{
		Win32CreateFolderTree(handle);
	}

	std::wstring wfilename = to_wchar(handle->mFullFileName);
	wchar_t wmode[64] = {};
	for (int i=0; i<strlen(mode); ++i)
	{
		wmode[i] = mode[i];
	}

	handle->mFile = _wfopen(wfilename.c_str(), wmode);

	if (handle->mFile)
	{
		handle->mStatus |= FileHandle::Open;
		handle->mStatus |= FileHandle::Exist;
		handle->setOpeningFlags(flagmode);
		return true;
	}
	handle->resetStatus();
	return false;
}

long int Win32fread(void * ptr, long size, long count, FileHandle* handle)
{
	return fread(ptr, size, count, handle->mFile);
}

long int Win32fwrite(const void * ptr, long size, long count, FileHandle* handle)
{
	return fwrite(ptr, size, count, handle->mFile);
}

long int Win32ftell(FileHandle* handle)
{
	return ftell(handle->mFile);
}

int Win32fseek(FileHandle* handle, long int offset, int origin)
{
	return fseek(handle->mFile, offset, origin);
}
int Win32fflush(FileHandle* handle)
{
	return fflush(handle->mFile);
}
int Win32fclose(FileHandle* handle)
{
	int result = fclose(handle->mFile);
	handle->resetStatus(); // reset 
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
