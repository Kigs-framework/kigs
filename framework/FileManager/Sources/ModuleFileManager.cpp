#include "PrecompiledHeaders.h"
#include "ModuleFileManager.h"
#include "RessourceFileManager.h"
#include "Core.h"
#include "Platform/Core/PlatformCore.h"
#include "SmartPointer.h"

#ifdef _KIGS_ONLY_STATIC_LIB_
#include "Platform/FileManager/FileManager.h"
#endif

IMPLEMENT_CLASS_INFO(ModuleFileManager);

//! module init, register FilePathManager
void ModuleFileManager::Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
	BaseInit(core,"FileManager",params);

	core->RegisterMainModuleList(this,FileManagerModuleCoreIndex);

	DECLARE_FULL_CLASS_INFO(core,FilePathManager,FilePathManager,FileManager)
	DECLARE_FULL_CLASS_INFO(core,RessourceFileManager,RessourceFileManager,FileManager)

#ifdef _KIGS_ONLY_STATIC_LIB_
	PlatformFileManagerModuleInit(core,params);
#endif
}

//! module close
void ModuleFileManager::Close()
{
	KigsCore::ReleaseSingleton("FilePathManager");
#ifdef _KIGS_ONLY_STATIC_LIB_
	PlatformFileManagerModuleClose(KigsCore::Instance());
#endif
	BaseClose();
}

//! module update
void ModuleFileManager::Update(const Timer& timer, void* addParam)
{
	BaseUpdate(timer,addParam);
#ifdef _KIGS_ONLY_STATIC_LIB_
	PlatformFileManagerModuleUpdate(&timer);
#endif
}    

// utility method 
// load a binary file 
// param 1 : filename
// param 2 : returned file length
// param 3 : start offset 
// return a pointer on the loaded file in memory, must be deleted with delete[] after use
CoreRawBuffer*	ModuleFileManager::LoadFile(const char *pFilename, u64& filelength, u64 startOffset, bool AsCharString)
{
	SmartPointer<FileHandle> result = KigsCore::Singleton<FilePathManager>()->FindFullName(pFilename);
	if (result->myStatus&FileHandle::Exist)
	{
		return LoadFile(result.get(), filelength, startOffset, AsCharString);
	}
	return 0;
}

CoreRawBuffer*	ModuleFileManager::LoadFile(FileHandle *file,u64& filelength,u64 startOffset,bool AsCharString)
{
	// init file length to 0
	filelength=0;	
	if (!Platform_fopen(file, "rb"))
	{
		return 0;
	}
	unsigned char* pFile=0;
	CoreRawBuffer*	result=0;
	u64 additionnalSize=0;
	if(AsCharString)
	{
		additionnalSize=1;
	}

	if(file->myFile)
	{
		if (file->mySize == -1)
		{
			Platform_fseek(file, 0, SEEK_END);
			filelength = Platform_ftell(file) - startOffset;
			Platform_fseek(file, 0, SEEK_SET);
		}
		else
		{
			filelength = file->mySize - startOffset;
		}
		pFile = new unsigned char[filelength+additionnalSize];

		// goto offset
		if(startOffset)
			Platform_fseek(file,(int)startOffset,SEEK_SET);

		size_t readlen=Platform_fread((void*) pFile, 1, (int)filelength, file);

		if( filelength != readlen )
		{
			delete[] pFile;
			pFile = NULL;
		}
		else
		{
			if(AsCharString)
			{
				pFile[filelength]=0;
			}
			result=new CoreRawBuffer(pFile,filelength+additionnalSize);
		}
		Platform_fclose(file);
	}
	return result;
}

//! load a file and add a zero at the end of the char buffer 
CoreRawBuffer*	ModuleFileManager::LoadFileAsCharString(const char *pFilename, u64& filelength, u64 startOffset)
{
	return LoadFile(pFilename, filelength, startOffset, true);
}

CoreRawBuffer*	ModuleFileManager::LoadFileAsCharString(FileHandle *pFilename,u64& filelength,u64 startOffset)
{
	return LoadFile(pFilename,filelength,startOffset,true);
}

bool	ModuleFileManager::SaveFile(const char *filename, unsigned char* data, u64 length)
{
	SmartPointer<FileHandle> File = Platform_fopen(filename, "wb");

	if(!File->myFile) return false;

	Platform_fwrite((void*)data, sizeof(unsigned char), length, File.get());
	Platform_fclose(File.get());
	return true;
}

bool	ModuleFileManager::CoreCopyFile(const char *sourceFilename, const char *destFileName, int buffLen)
{
	bool result = true;

	SmartPointer<FileHandle> FileRead = KigsCore::Singleton<FilePathManager>()->FindFullName(sourceFilename);
	if (!FileRead->myFile)
	{
		result=false;
	}
	else
	{
		SmartPointer<FileHandle> FileWrite = Platform_fopen(destFileName, "wb");
		if (!FileWrite->myFile)
		{
			result = false;
		}
		else
		{
			unsigned char* buffer = new unsigned char[buffLen];
			unsigned int readlen;
			do
			{
				readlen = Platform_fread((void*)buffer, 1, (int)buffLen, FileRead.get());
				if (readlen)
				{
					Platform_fwrite((void*)buffer, 1, (int)buffLen, FileWrite.get());
				}

			} while (readlen);

			Platform_fclose(FileWrite.get());

			delete[] buffer;
		}


		Platform_fclose(FileRead.get());
	}
	return result;
}
