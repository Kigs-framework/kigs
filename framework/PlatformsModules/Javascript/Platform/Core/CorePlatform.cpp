#include "PrecompiledHeaders.h"
#include "Core.h"
#include "FilePathManager.h"
#include "Platform/Core/PlatformCore.h"
#include "PlatformBaseApplication.h"
#include <sys/stat.h>

bool	PlatformBaseApplication::CheckBackKeyPressed()
{
	// TODO check if esc key is pressed here ?

	return false;
}

// get number of core / processor
unsigned int PlatformBaseApplication::getProcessorCount()
{
	// TODO ?
	return 1;
}

// check if file exist and if it's a file or directory
void JSCheckState(FileHandle * hndl)
{
	struct stat info;

	if (stat(hndl->mFullFileName.c_str(), &info) != 0)
	{
		hndl->resetStatus();
		return;
	}
	hndl->mStatus |= FileHandle::Exist;
	if (info.st_mode & S_IFDIR)
		hndl->mStatus |= FileHandle::IsDIr;

}

void JSCreateFolderTree(FileHandle* hndl)
{
	if ((hndl->mStatus&FileHandle::Exist) == 0)
	{
		SmartPointer<FileHandle> parent = FilePathManager::CreateFileHandle(FilePathManager::GetParentDirectory(hndl->mFullFileName));
		if (parent->mFullFileName != "")
		{
			JSCheckState(parent.get());

			JSCreateFolderTree(parent.get());

			mkdir(parent->mFullFileName.c_str(), 0x770);
		}
	}
}



SmartPointer<FileHandle> JSFindFullName(const kstl::string&	filename)
{
	// TODO
	SmartPointer<FileHandle> result;
	const char* filename_cstr = filename.c_str();
	if (filename_cstr[0] != '#')
	{
		return result;
	}


	return result;
}





bool JSfopen(FileHandle* handle, const char * mode)
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
			JSfclose(handle);
		}
	}

	// create parent if in write mode
	if (mode[0] == 'w' || mode[0] == 'a')
	{
		JSCreateFolderTree(handle);
	}
	handle->mFile = fopen(handle->mFullFileName.c_str(), mode);

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


long int JSfread(void * ptr, long size, long count, FileHandle* handle)
{
	return fread(ptr, size, count, handle->mFile);
}

long int JSfwrite(const void * ptr, long size, long count, FileHandle* handle)
{
	return fwrite(ptr, size, count, handle->mFile);
}

long int JSftell(FileHandle* handle)
{
	return ftell(handle->mFile);
}

int JSfseek(FileHandle* handle, long int offset, int origin)
{
	return fseek(handle->mFile,offset,origin);
}
int JSfflush(FileHandle* handle)
{
	return fflush(handle->mFile);
}
int JSfclose(FileHandle* handle)
{
	int result=fclose(handle->mFile);
	handle->mStatus=0; // reset 
	return result;
}