
#include "FilePathManager.h"
#include "ModuleFileManager.h"
#include "Core.h"

bool SQLiteDB::ExistDatabase(const char * dbName)
{
	SP<FilePathManager>	pathManager =KigsCore::GetSingleton("FilePathManager");
	kstl::string L_Path;
	kstl::string filePath = FilePathManager::DevicePath(dbName, FilePathManager::DB_STORAGE);
	
	SmartPointer<FileHandle> fullfilenamehandle = Platform_fopen(filePath.c_str(), "rb");
	if (fullfilenamehandle->mFile)
	{
		Platform_fclose(fullfilenamehandle.get());
		return true;
	}
	
	return false;
}