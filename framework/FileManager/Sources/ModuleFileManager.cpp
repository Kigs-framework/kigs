#include "PrecompiledHeaders.h"
#include "ModuleFileManager.h"
#include "RessourceFileManager.h"
#include "Core.h"
#include "Platform/Core/PlatformCore.h"
#include "SmartPointer.h"

#include "Platform/FileManager/FileManager.h"

using namespace Kigs::File;

IMPLEMENT_CLASS_INFO(ModuleFileManager);

//! module init, register FilePathManager
void ModuleFileManager::Init(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params)
{
	BaseInit(core,"FileManager",params);

	core->RegisterMainModuleList(this,FileManagerModuleCoreIndex);

	DECLARE_FULL_CLASS_INFO(core,FilePathManager,FilePathManager,FileManager)
	DECLARE_FULL_CLASS_INFO(core,RessourceFileManager,RessourceFileManager,FileManager)

	PlatformFileManagerModuleInit(core,params);
}

//! module close
void ModuleFileManager::Close()
{
	KigsCore::ReleaseSingleton("FilePathManager");
	PlatformFileManagerModuleClose(KigsCore::Instance());
	BaseClose();
}

//! module update
void ModuleFileManager::Update(const Time::Timer& timer, void* addParam)
{
	BaseUpdate(timer,addParam);
	PlatformFileManagerModuleUpdate(&timer);
}    


SmartPointer<CoreRawBuffer> ModuleFileManager::LoadFileToBuffer(const std::string& pFilename)
{
	u64 len;
	return LoadFile(pFilename.c_str(), len);
}

// utility method 
// load a binary file 
// param 1 : filename
// param 2 : returned file length
// param 3 : start offset 
// return a pointer on the loaded file in memory, must be deleted with delete[] after use
SmartPointer<CoreRawBuffer>	ModuleFileManager::LoadFile(const char *pFilename, u64& filelength, u64 startOffset, unsigned int trailing_zero)
{
	SmartPointer<FileHandle> result = KigsCore::Singleton<FilePathManager>()->FindFullName(pFilename);
	if (result->mStatus&FileHandle::Exist)
	{
		return LoadFile(result.get(), filelength, startOffset, trailing_zero);
	}
	return 0;
}

SmartPointer<CoreRawBuffer>	ModuleFileManager::LoadFile(FileHandle *file,u64& filelength,u64 startOffset, unsigned int trailing_zero)
{
	// init file length to 0
	filelength=0;	
	if (!Platform_fopen(file, "rb"))
	{
		return 0;
	}
	unsigned char* pFile=0;
	SP<CoreRawBuffer> result;
	u64 additionnalSize= trailing_zero;

	if(file->mFile)
	{
		filelength= file->getFileSize() - startOffset;
		
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
			for(unsigned int i=0;i<trailing_zero;i++)
			{
				pFile[filelength+i]=0;
			}
			result = std::make_shared<CoreRawBuffer>(pFile, filelength + additionnalSize);
		}
		Platform_fclose(file);
	}
	return result;
}

//! load a file and add a zero at the end of the char buffer 
SP<CoreRawBuffer>	ModuleFileManager::LoadFileAsCharString(const char *pFilename, u64& filelength, u8 charSize, u64 startOffset)
{
	return LoadFile(pFilename, filelength, startOffset, charSize);
}

SP<CoreRawBuffer>	ModuleFileManager::LoadFileAsCharString(FileHandle *pFilename,u64& filelength, u8 charSize,u64 startOffset)
{
	return LoadFile(pFilename,filelength,startOffset, charSize);
}

bool	ModuleFileManager::SaveFile(const char *filename, unsigned char* data, u64 length)
{
	SmartPointer<FileHandle> File = Platform_fopen(filename, "wb");

	if(!File->mFile) return false;

	Platform_fwrite((void*)data, sizeof(unsigned char), length, File.get());
	Platform_fclose(File.get());
	return true;
}

bool	ModuleFileManager::RemoveFile(const char* filename)
{
	// check if file exist
	SmartPointer<FileHandle> fileHandle = KigsCore::Singleton<FilePathManager>()->FindFullName(filename);
	if (!(fileHandle->mStatus & FileHandle::Exist)) return false;

	return Platform_remove(fileHandle.get());
}


bool	ModuleFileManager::CoreCopyFile(const char *sourceFilename, const char *destFileName, int buffLen)
{
	bool result = true;

	SmartPointer<FileHandle> FileRead = KigsCore::Singleton<FilePathManager>()->FindFullName(sourceFilename);
	Platform_fopen(FileRead.get(), "rb");
	SmartPointer<FileHandle> FileWrite = Platform_fopen(destFileName, "wb");

	result = CoreCopyFile(FileRead, FileWrite, buffLen);

	Platform_fclose(FileWrite.get());
	Platform_fclose(FileRead.get());

	return result;
}

bool	ModuleFileManager::CoreCopyFile(SP<FileHandle> source, SP<FileHandle> dest, int buffLen)
{
	bool result = true;

	if (!source->mFile)
	{
		result = false;
	}
	else
	{
		if (!dest->mFile)
		{
			result = false;
		}
		else
		{
			unsigned char* buffer = new unsigned char[buffLen];
			unsigned int readlen;
			do
			{
				readlen = Platform_fread((void*)buffer, 1, (int)buffLen, source.get());
				if (readlen)
				{
					Platform_fwrite((void*)buffer, 1, (int)readlen, dest.get());
				}

			} while (readlen);

			delete[] buffer;
		}

	}
	return result;
}

bool	ModuleFileManager::CoreCopyPartOfFile(SP<FileHandle> lsource, u64 lstart, u64 lsize, SP<FileHandle> ldest, int lbuffLen)
{
	bool result = true;

	if (!lsource->mFile)
	{
		result = false;
	}
	else
	{
		if (!ldest->mFile)
		{
			result = false;
		}
		else
		{
			// go to asked read position
			Platform_fseek(lsource.get(), lstart, SEEK_SET);
			unsigned char* buffer = new unsigned char[lbuffLen];
			unsigned int readlen;
			u64 remainingLen = lsize;
			do
			{
				long askedLen = lbuffLen;
				if (remainingLen < lbuffLen)
				{
					askedLen = remainingLen;
				}
				readlen = Platform_fread((void*)buffer, 1, askedLen, lsource.get());
				if (readlen)
				{
					Platform_fwrite((void*)buffer, 1, (long)readlen, ldest.get());
				}
				remainingLen -= readlen;
				if (remainingLen == 0)
					break;

			} while (readlen);

			delete[] buffer;
		}

	}
	return result;
}