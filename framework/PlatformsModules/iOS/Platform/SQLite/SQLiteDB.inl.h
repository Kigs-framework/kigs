
#include "FilePathManager.h"
#include "ModuleFileManager.h"

bool SQLiteDB::ExistDatabase(const char * dbName)
{
	FilePathManager*	pathManager = reinterpret_cast<FilePathManager*>(Core::GetSingleton(_S_2_ID("FilePathManager")));
	kstl::string L_Path;
	kstl::string filePath = FilePathManager::DevicePath(dbName, FilePathManager::APPLICATION_STORAGE);
	
	SmartPointer<FileHandle> fullfilenamehandle = Platform_fopen(filePath.c_str(), "rb");
	if (fullfilenamehandle->myFile)
	{
		Platform_fclose(fullfilenamehandle);
		return true;
	}
	
	return false;
}