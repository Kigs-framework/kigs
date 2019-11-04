
#include "FilePathManager.h"
#include "ModuleFileManager.h"

bool SQLiteDB::ExistDatabase(const char * dbName)
{
	FilePathManager*	pathManager = reinterpret_cast<FilePathManager*>(KigsCore::GetSingleton("FilePathManager"));
	kstl::string filePath = FilePathManager::DevicePath(dbName, FilePathManager::DB_STORAGE);
	
	SmartPointer<FileHandle> fullfilenamehandle = Platform_fopen(filePath.c_str(), "rb");
	if (fullfilenamehandle->myFile)
	{
		Platform_fclose(fullfilenamehandle.get());
		return true;
	}
	
	return false;
}