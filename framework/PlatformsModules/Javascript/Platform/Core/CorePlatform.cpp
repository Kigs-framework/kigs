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

	if (stat(hndl->myFullFileName.c_str(), &info) != 0)
	{
		hndl->resetStatus();
		return;
	}
	hndl->myStatus |= FileHandle::Exist;
	if (info.st_mode & S_IFDIR)
		hndl->myStatus |= FileHandle::IsDIr;

}

void JSCreateFolderTree(FileHandle* hndl)
{
	if ((hndl->myStatus&FileHandle::Exist) == 0)
	{
		SmartPointer<FileHandle> parent = FilePathManager::CreateFileHandle(FilePathManager::GetParentDirectory(hndl->myFullFileName));
		if (parent->myFullFileName != "")
		{
			JSCheckState(parent.get());

			JSCreateFolderTree(parent.get());

			mkdir(parent->myFullFileName.c_str(), 0x770);
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

	if (handle->myStatus&FileHandle::Open) // already opened ? return true
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
	handle->myFile = fopen(handle->myFullFileName.c_str(), mode);

	if (handle->myFile)
	{
		handle->myStatus |= FileHandle::Open;
		handle->myStatus |= FileHandle::Exist;
		handle->setOpeningFlags(flagmode);
		return true;
	}
   handle->resetStatus();
   return false;
}


long int JSfread(void * ptr, long size, long count, FileHandle* handle)
{
	return fread(ptr, size, count, handle->myFile);
}

long int JSfwrite(const void * ptr, long size, long count, FileHandle* handle)
{
	return fwrite(ptr, size, count, handle->myFile);
}

long int JSftell(FileHandle* handle)
{
	return ftell(handle->myFile);
}

int JSfseek(FileHandle* handle, long int offset, int origin)
{
	return fseek(handle->myFile,offset,origin);
}
int JSfflush(FileHandle* handle)
{
	return fflush(handle->myFile);
}
int JSfclose(FileHandle* handle)
{
	int result=fclose(handle->myFile);
	handle->myStatus=0; // reset 
	return result;
}