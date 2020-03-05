#include "PrecompiledHeaders.h"
#include "../../../../Core/Headers/Core.h"
#include "../../../../FileManager/Headers/FilePathManager.h"
#include "Platform/Core/PlatformCore.h"
#include "Platform/Core/PlatformBaseApplication.h"
//#include <shlobj.h>
#include <sys/stat.h>

#include "../../../../Core/Headers/ModuleBase.h"
#include "../../../../Core/Headers/CoreBaseApplication.h"

//#include "../../../../HTTPRequest/Headers/HTTPConnect.h"

//#import <Foundation/Foundation.h>


extern void fopenBundle(MAC_FILE** file, const char* filename, const char* ext, const char* directory);
extern long int  freadBundle(void * ptr, long size, long count, MAC_FILE* file);
extern bool checkstateBundle(const char* filename);
extern int  fcloseBundle(MAC_FILE* file);

extern void getDocumentFolder(char* folder);
extern void getApplicationFolder(char* folder);

extern const char* gRessourcePath;

void IPhoneCheckState(FileHandle* hndl)
{
	if (hndl->myDeviceID == 0)
	{
		if (checkstateBundle(hndl->myFullFileName.c_str()))
		{
			hndl->myStatus |= FileHandle::Exist;
		}
		return;
	}
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
void IPhoneCreateFolderTree(FileHandle* hndl)
{
	if ((hndl->myStatus&FileHandle::Exist) == 0)
	{
		SmartPointer<FileHandle> parent = FilePathManager::CreateFileHandle(FilePathManager::GetParentDirectory(hndl->myFullFileName));
		if (parent->myFullFileName != "")
		{
			IPhoneCheckState(parent);

			IPhoneCreateFolderTree(parent);

			mkdir(parent->myFullFileName.c_str(), 0x770);
		}
	}
}

SmartPointer<FileHandle> IPhoneFindFullName(const kstl::string&	filename)
{
	if (filename[0] != '#')
	{
		return 0;
	}

	kstl::string fullFileName = "";

	FilePathManager::DeviceID	id = (FilePathManager::DeviceID)(filename[1]);
		
	switch(id)
	{
		case FilePathManager::CARD_STORAGE:
			// No CARD on ios
			return 0;
			break;
		case FilePathManager::DEVICE_STORAGE:
			// No DEVICE on ios
			return 0;
			break;
		case FilePathManager::APPLICATION_STORAGE:
		case FilePathManager::DB_STORAGE:
		{
			char resultchar[512];
			getApplicationFolder(resultchar);
			fullFileName += resultchar;
			fullFileName += "/";
			break;
		}
		case FilePathManager::DOCUMENT_FOLDER:
		{			
			char resultchar[512];
			getDocumentFolder(resultchar);
			fullFileName += resultchar;
			fullFileName += "/";
		}
		break;
        default:
            break;
	}
	
	fullFileName += (const char*)(&(filename.c_str()[3]));

	SmartPointer<FileHandle> result = FilePathManager::CreateFileHandle((const char*)(&(filename.c_str()[3])));

	result->myDeviceID = id;
	result->myFullFileName = fullFileName;
	IPhoneCheckState(result);
	return result;
}

bool IPhonefopen (FileHandle *handle, const char* mode)
{
	if (handle->myDeviceID == 0)
	{
		fopenBundle(&(handle->myFile), handle->myFileName.c_str(), handle->myExtension.c_str(), handle->myFullFileName.c_str());
	}
	else
	{
		handle->myFile->myFile = fopen(handle->myFullFileName.c_str(), mode);
	}
	if (handle->myFile)
	{
		handle->myStatus |= FileHandle::Open;
		return true;
	}
	return false;
}



long int IPhonefread(void * ptr, long size, long count, FileHandle* handle)
{
    if(handle && handle->myFile)
    {

		if (handle->myDeviceID == 0)
		{
			return freadBundle(ptr,size,count,handle->myFile);
		}

        if(handle->myFile->myFile)
            return fread(ptr, size, count, handle->myFile->myFile);
    }
    return 0;
}

long int IPhonefwrite(const void * ptr, long size, long count, FileHandle* handle)
{
    if(handle && handle->myFile && handle->myFile->myFile)
        return fwrite(ptr, size, count, handle->myFile->myFile);
    return 0;
}

long int IPhoneftell(FileHandle* handle)
{
    long int L_retValue = 0;
    if(handle && handle->myFile)
    {
        if(handle->myFile->myFile)
            return ftell(handle->myFile->myFile);

        L_retValue = handle->myFile->myCurrentPos;
    }
    return L_retValue;
}

int IPhonefseek(FileHandle* handle, long int offset, int origin)
{
        if(!handle ||!handle->myFile)
            return 1;
        if(handle->myFile->myFile)
            return fseek(handle->myFile->myFile,offset,origin);

        switch(origin)
        {
            case SEEK_SET :
                if(offset<handle->myFile->mySize)
                {
                    handle->myFile->myCurrentPos=offset;
                    return 0;
                }
                break;
            case SEEK_CUR :
                if((offset+handle->myFile->myCurrentPos)<handle->myFile->mySize)
                {
                    handle->myFile->myCurrentPos+=offset;
                    return 0;
                }
                break;
            case SEEK_END :
                if(((long)handle->myFile->mySize-offset)>=0)
                {
                    handle->myFile->myCurrentPos=(handle->myFile->mySize-offset);
                    return 0;
                }
                break;
            default : return 1;
        }
        
        return 3;
}

int IPhonefflush(FileHandle* handle)
{
    if(handle && handle->myFile && handle->myFile->myFile)
        return fflush(handle->myFile->myFile);

    return 0;
}

int IPhonefclose(FileHandle* handle)
{
    int result=0;
    if(handle && handle->myFile)
    {
        if(handle->myFile->myFile)
        {
            result = fclose(handle->myFile->myFile);
        }

		if (handle->myDeviceID == 0)
		{
			result=fcloseBundle(handle->myFile);
		}

        
        handle->myFile->myData = 0;
        delete handle->myFile;
        handle->myStatus=0; // reset
    }
    return result;
}
