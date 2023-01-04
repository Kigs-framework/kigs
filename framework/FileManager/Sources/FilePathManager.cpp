#include "PrecompiledHeaders.h"
#include "FilePathManager.h"
#include "Platform/Core/PlatformCore.h"
#include "CoreRawBuffer.h"
#include "ModuleFileManager.h"
#include "JSonFileParser.h"
#include "AsciiParserUtils.h"
#include "CorePackage.h"
#include "PureVirtualFileAccessDelegate.h"

#include <stdio.h>

#ifdef HTTP_PROTOCOL
#include "HTTPConnect.h"
#endif

using namespace Kigs::File;

IMPLEMENT_CLASS_INFO(FilePathManager)

FileHandle::~FileHandle()
{
	if (mStatus & (unsigned int)Open)
	{
		Platform_fclose(this);
	}

	if (mUseVirtualFileAccess)
	{
		delete mVirtualFileAccess;
	}
}


SmartPointer<FileHandle> FileHandle::MakeCopy()
{
	auto result = MakeRefCounted<FileHandle>();

	result->mFileName = mFileName;
	result->mFullFileName = mFullFileName;
	result->mExtension = mExtension;
	result->mDeviceID = mDeviceID;
	result->mUseVirtualFileAccess = mUseVirtualFileAccess;
	if (mUseVirtualFileAccess)
		result->mVirtualFileAccess = mVirtualFileAccess->MakeCopy();
	else
		result->mFile = nullptr;

	result->resetStatus();

	return result;
}

//! constructor
FilePathManager::FilePathManager(const std::string& name, CLASS_NAME_TREE_ARG) : CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG)
, mPackageID(0)
{
	mBundleList.clear();
	mBundleRoot = "";

	mPackageList.clear();
}

//! destructor
FilePathManager::~FilePathManager()
{
	Clear();
}
std::string FilePathManager::MakeValidFileName(const std::string& filename)
{
	std::string result;
	for (int i = 0; i < filename.length(); i++)
	{
		unsigned char current = filename[i];
		if (current < 32)
		{
			current = '_';
		}
		else if((current>32) && (current < 45))
		{
			current = '_';
		}
		else if ((current > 57) && (current < 65))
		{
			current = '_';
		}
		else if ((current > 90) && (current < 97))
		{
			current = '_';
		}
		else if ((current > 122))
		{
			current = '_';
		}
		result += current;
	}
	return result;
}

/*! add a new path for the given extension
	if extension is "*", then add the path to all current extension
*/
void	FilePathManager::AddToPath(const std::string& path, std::string ext, DeviceID deviceID)
{
	std::vector<std::string>& localpath = mPath[ext];
	if (deviceID)
	{
		localpath.push_back(GetDevicePathString(deviceID) + path);
	}
	else
	{
		localpath.push_back(path);
	}
}


//!	remove from path	
void	FilePathManager::RemoveFromPath(const std::string& path, std::string ext, DeviceID deviceID)
{
	bool found = true;
	std::string	toMatch;
	if (deviceID)
	{
		toMatch = GetDevicePathString(deviceID) + path;
	}
	else
	{
		toMatch = path;
	}

	// check if ext path exist

	if (mPath.find(ext) != mPath.end())
	{
		std::vector<std::string>& localpath = mPath[ext];

		found = true;
		while (found)
		{
			found = false;
			std::vector<std::string>::iterator itstring;
			for (itstring = localpath.begin(); itstring != localpath.end(); itstring++)
			{
				if ((*itstring) == toMatch)
				{
					localpath.erase(itstring);
					found = true;
					break;
				}
			}
		}
	}
}

// create a file handle from a filename
SmartPointer<FileHandle> FilePathManager::CreateFileHandle(const std::string& filename)
{
	SmartPointer<FileHandle> result = MakeRefCounted<FileHandle>();
	result->mFileName = filename;
	SetFileInfos(result.get());
	return result;
}

// load a package given a FileHandle
bool	FilePathManager::LoadPackage(SmartPointer<FileHandle> L_File)
{
	CorePackage* newpackage = CorePackage::Open(L_File, L_File->mFileName);
	if (newpackage)
	{
		mPackageList[mPackageID] = newpackage;

		insertPackage(mPackageID);

		mPackageID++;
	}
	return newpackage;
}

int	FilePathManager::GetPackageID(const std::string& filename)
{
	for (auto& pkg : mPackageList)
	{
		if (pkg.second->GetName() == filename) return pkg.first;
	}
	return -1;
}

std::string FilePathManager::GetPackageRootPath(int id)
{
	std::string result = "";

	auto found = mPackageList.find(id);
	if (found != mPackageList.end())
	{
		result = "#PKG" + std::to_string((*found).first) + "#";
		if ((*found).second->getRootFolderName() != "")
		{
			result += (*found).second->getRootFolderName();
		}
	}

	return result;
}
std::string FilePathManager::GetPackageRootPath(const std::string& filename)
{
	int ID = GetPackageID(filename);
	if (ID != -1)
	{
		return GetPackageRootPath(ID);
	}

	return "";
}

// load a package given it's filename
bool	FilePathManager::LoadPackage(const std::string& filename)
{
	// TODO : check if this package was already loaded ?
	auto L_File =FindFullName(filename);
	if (!Platform_fopen(L_File.get(), "rb"))
	{
		return false;
	}
	CorePackage* newpackage = CorePackage::Open(L_File, filename);

	if (newpackage)
	{
		mPackageList[mPackageID] = newpackage;

		insertPackage(mPackageID);

		mPackageID++;
	}
	return newpackage;
}

// unload a package given it's filename
void	FilePathManager::UnloadPackage(const std::string& filename)
{
	// search package with same filename
	
	std::map<int, CorePackage*>::iterator	itP = mPackageList.begin();
	std::map<int, CorePackage*>::iterator	itE = mPackageList.end();

	while( itP != itE )
	{

		if ((*itP).second->GetName() == filename)
		{
			unloadPackage((*itP).first);
			mPackageList.erase(itP);
			break;
		}
	}
}

CorePackage* FilePathManager::GetLoadedPackage(const std::string& filename)
{
	for (auto& pkg : mPackageList)
	{
		if (pkg.second->GetName() == filename) return pkg.second;
	}
	return nullptr;
}

// add package mEntry in bundle list
void	FilePathManager::insertPackage(unsigned int packageID)
{
	if (mPackageList.find(packageID) != mPackageList.end())
	{
		CorePackage* package = mPackageList[packageID];
		CorePackage::CorePackageIterator it = package->begin();
		CorePackage::CorePackageIterator ite = package->end();

		std::string	packagePath = "#PKG";

		char	packageIDStr[10];
		sprintf(packageIDStr, "%d", packageID);

		packagePath += packageIDStr;
		packagePath += "#";

		// retreive all entries
		while (!(it == ite))
		{
			if (!it.isFolder())
			{
				std::string key = it.name();
				mBundleList[key].push_back(packagePath+it.path(false));
			}
			++it;
		}
	}
}

// unload a package given it's package id (protected)
void	FilePathManager::unloadPackage(unsigned int packageID)
{
	if (mPackageList.find(packageID) != mPackageList.end())
	{
		CorePackage* package = mPackageList[packageID];
		CorePackage::CorePackageIterator it = package->begin();
		CorePackage::CorePackageIterator ite = package->end();

		std::string	packagePath = "#PKG";

		char	packageIDStr[10];
		sprintf(packageIDStr, "%d", packageID);

		packagePath += packageIDStr;
		packagePath += "#";

		// retreive all entries
		while (!(it == ite))
		{
			if (!it.isFolder())
			{
				std::string key = it.name();
				
				if (mBundleList.find(key) != mBundleList.end())
				{
					// search good string
					std::vector<std::string>::iterator itStr;
					for (itStr=mBundleList[key].begin(); itStr != mBundleList[key].end();++itStr)
					{
						if ((*itStr) == packagePath + it.path(false))
						{
							mBundleList[key].erase(itStr);
							break;
						}
					}
					if (mBundleList[key].size() == 0)
					{
						auto toErase = mBundleList.find(key);
						mBundleList.erase(toErase);
					}
				}

			}
			++it;
		}

		delete package;

	}
}

// retreive parent directory for the given full path
std::string	FilePathManager::GetParentDirectory(const std::string& fullPath)
{
	int pos = (int)fullPath.rfind("/") + 1;
	int pos1 = (int)fullPath.rfind("\\") + 1;

	if (pos1 > pos)
	{
		pos = pos1;
	}

	if (pos > 0)
	{
		return fullPath.substr(0, pos - 1);
	}

	return "";
}

// retreive extension and short file name 
void	FilePathManager::SetFileInfos(FileHandle* handle)
{
	// already set ? return
	if (handle->mStatus&FileHandle::InfoSet)
	{
		return;
	}

	handle->mFullFileName = handle->mFileName;

	int pos = (int)handle->mFileName.rfind("/") + 1;
	int pos1 = (int)handle->mFileName.rfind("\\") + 1;

	if (pos1 > pos)
	{
		pos = pos1;
	}

	if (pos > 0)
	{
		handle->mFileName = handle->mFileName.substr((unsigned int)pos, handle->mFileName.length() - pos);
	}

	pos = (int)handle->mFileName.rfind(".");
	if (pos != -1)
		handle->mExtension = handle->mFileName.substr((unsigned int)pos, handle->mFileName.length() - pos);

	handle->mStatus |= (unsigned int)FileHandle::InfoSet;
}


/*! search the given file in the pathes corresponding to its extension, or in the "all extension" * path
	and if found return the full filename with path
*/
SmartPointer<FileHandle>	FilePathManager::FindFullName(const std::string&	filename)
{
	bool isDevice, isPackage;
	isSpecialDeviceOrPackage(filename, isDevice, isPackage);

	// if the file is in special '#' directory
	if (isDevice)
	{
		return Platform_FindFullName(filename);
	}
	else if(isPackage)
	{
		
		SmartPointer<FileHandle> result = CreateFileHandle(filename);
		size_t pos = filename.find('#', 3);
		if (pos == std::string::npos)
		{
			// malformed path
			return result;
		}
		std::string kpkg = filename.substr(0, pos+1);
		initHandleFromPackage(kpkg, result, false);
			
		return result;
	
	}
	SmartPointer<FileHandle> result = CreateFileHandle(filename);

	// init fullFileName with filename
	std::string fullFileName = result->mFileName;
	std::string	fileext = "";
	if (result->mExtension != "")
		fileext.append(result->mExtension, 1, result->mExtension.length() - 1);

	// search in bundle
	if (mBundleList.size())
	{
		auto foundInBundle = mBundleList.find(result->mFileName);
		if (foundInBundle != mBundleList.end())
		{
			const std::vector<std::string>& bundlePathVector = (*foundInBundle).second;
			if (bundlePathVector.size() == 0) // bundle root
			{
				result->mFullFileName = mBundleRoot + result->mFileName;
#ifdef _DEBUG
				Platform_CheckState(result.get());
#else
				result->mStatus |= FileHandle::Exist; // if not debug, if is in bundle, suppose it really exist without having to test
#endif
				if (result->mStatus&FileHandle::Exist)
					return result;

			}
			else 
			{
				int bundlePathVectorIndex = 0;
				if (bundlePathVector.size() > 1) // several pathes available
				{
					std::vector<std::string>* localpath = nullptr;
					if (mPath.find(fileext) != mPath.end()) // if this extension is in pathes map 
					{
						localpath = &mPath[fileext];
					}
					else
					{
						localpath = &mPath["*"];
					}
					bool foundinbundle = false;
					for (int i = 0; i < bundlePathVector.size(); i++)
					{
						for (auto& lp : *localpath)
						{
							size_t pos = bundlePathVector[i].find(lp);
							if (pos != std::string::npos)
							{
								if ((lp.length() + pos) == (bundlePathVector[i].length()-1))
								{
									if (pos==0)
									{
										foundinbundle = true;
										break;
									}
									if (pos<=7) // can be a kpkg ?
									{
										std::string head = bundlePathVector[bundlePathVectorIndex].substr(1, 3);
										if (head == "PKG")
										{
											foundinbundle = true;
											break;
										}
									}
								}
							}
						}
						if (foundinbundle)
						{
							bundlePathVectorIndex = i;
							break;
						}
					}
				}

				if (bundlePathVector[bundlePathVectorIndex][0] == '#') // check if package
				{
					std::string head = bundlePathVector[bundlePathVectorIndex].substr(1, 3);
					if (head == "PKG")
					{
						if (initHandleFromPackage(bundlePathVector[bundlePathVectorIndex], result, true))
						{
							return result;
						}
					}
				}

				result->mFullFileName = mBundleRoot + bundlePathVector[bundlePathVectorIndex] + result->mFileName;
#ifdef _DEBUG
				Platform_CheckState(result.get());
#else
				result->mStatus |= FileHandle::Exist; // if not debug, if is in bundle, suppose it really exist without having to test
#endif
				if (result->mStatus&FileHandle::Exist)
					return result;
			}

			return result;
		}
	}

	// if given filename already has a path, search with only the path
	if (result->mFileName != result->mFullFileName)
	{
		Platform_CheckState(result.get());
		if ((result->mStatus & FileHandle::Exist) || mStrictPath)
			return result;
	}



	std::vector<std::string> localpath;
	if (mPath.find(fileext) != mPath.end())
	{
		localpath = mPath[fileext];
	}


	bool finished = false;
	bool searchall = false;

	while (!finished)
	{
		// extension not found
		if (!localpath.size())
		{
			localpath = mPath["*"];
			searchall = true;
		}

		if (localpath.size())
		{
			std::vector<std::string>::reverse_iterator	it;

			// search from last added path to first one
			for (it = localpath.rbegin(); it != localpath.rend(); ++it)
			{
				result->mFullFileName = (*it);
				result->mFullFileName += "/";
				result->mFullFileName += result->mFileName;

				bool isDevice, isPackage;
				isSpecialDeviceOrPackage((*it), isDevice, isPackage);

				if (isDevice)
				{
					SmartPointer<FileHandle> specialresult=Platform_FindFullName(result->mFullFileName);
					Platform_CheckState(specialresult.get());
					if (specialresult->mStatus&FileHandle::Exist)
						return specialresult;
				}
				else if (isPackage)
				{
					size_t pos = (*it).find('#', 3);
					if (pos == std::string::npos)
					{
						// malformed path
						return result;
					}
					std::string kpkg = (*it).substr(0, pos + 1);
					initHandleFromPackage(kpkg, result, false);

					return result;
				}
				else
				{
					Platform_CheckState(result.get());
					if (result->mStatus&FileHandle::Exist)
						return result;
				}
			}
		}

		if (searchall)
		{
			finished = true;
		}
		localpath = mPath["*"];
		searchall = true;
	}

	result->mFullFileName = filename;
	Platform_CheckState(result.get());

	return result;

}

bool	FilePathManager::initHandleFromPackage(const std::string& lpath, SmartPointer<FileHandle> result, bool in_bundle)
{
	// search end # (after package ID)
	size_t pos = lpath.find('#', 3);

	if (pos != std::string::npos)
	{
		std::string pkgIDStr = lpath.substr(4, pos - 4);

		int pkgID = 0;
		if (sscanf(pkgIDStr.c_str(), "%d", &pkgID))
		{
			if (mPackageList.find(pkgID) != mPackageList.end())
			{
				CorePackage* package = mPackageList[pkgID];
				result->mUseVirtualFileAccess = true;
				result->mVirtualFileAccess = new CorePackageFileAccess(package);

				// if lpath only contains #PKGid# use mFullFileName else use lpath
				if (!in_bundle)
				{					
					result->mFullFileName = result->mFullFileName.substr(pos + 1, result->mFullFileName.length() - pos - 1);
				}
				else
				{
					result->mFullFileName = lpath.substr(pos + 1, lpath.length() - pos - 1) + result->mFileName;
				}

				if (package->find(result->mFullFileName, true))
				{
					result->mStatus |= FileHandle::Exist;
				}
				return true;
			}
		}
	}
	return false;
}

// construct the "prefered path" according to filename extension
std::string	FilePathManager::PreferedPath(const std::string&	filename)
{
	std::string fullFileName = filename;

	int pos = (int)filename.rfind("/") + 1;
	int pos1 = (int)filename.rfind("\\") + 1;

	if (pos1 > pos)
	{
		pos = pos1;
	}

	std::string shortfilename;

	if (pos == 0)
	{
		shortfilename = filename;
	}
	else
	{
		shortfilename = filename.substr((unsigned int)pos, filename.length() - pos);
	}

	std::string	fileext = "";

	pos = (int)shortfilename.rfind(".") + 1;
	fileext.append(shortfilename, (unsigned int)pos, shortfilename.length() - pos);

	std::vector<std::string> localpath;
	if (mPath.find(fileext) != mPath.end())
	{
		localpath = mPath[fileext];
	}

	// extension not found
	if (!localpath.size())
	{
		localpath = mPath["*"];
	}


	std::vector<std::string>::reverse_iterator	it = localpath.rbegin();

	fullFileName = (*it);
	fullFileName += "/";
	fullFileName += shortfilename;

	return fullFileName;
}

// construct path according to device
std::string	FilePathManager::DevicePath(const std::string&	filename, DeviceID id)
{
	return GetDevicePathString(id) + filename;
}

//! clear all pathes and all struct before destroying the instance
void FilePathManager::Clear()
{
	for (auto it = mPath.begin(); it != mPath.end(); ++it)
	{
		std::vector<std::string>& list = (*it).second;
		list.clear();
	}

	mPath.clear();
	mBundleList.clear();

	for (auto itpackage : mPackageList)
	{
		delete itpackage.second;
	}

	mPackageList.clear();
}

// return the full path, seperate by ";"
std::string	FilePathManager::GetPathString()
{
	std::string result = "";
	for (auto it = mPath.begin(); it != mPath.end(); ++it)
	{
		std::vector<std::string>& list = (*it).second;

		std::vector<std::string>::iterator	it1;
		for (it1 = list.begin(); it1 != list.end(); ++it1)
		{
			result += (*it1);
			result += ";";
		}
	}


	return result;
}

//! the returned string will be added at start of the path to be retreived easily by fopen 
std::string FilePathManager::GetDevicePathString(DeviceID id)
{
	if (id)
	{
		std::string result = "#";

		result += (unsigned char)id;

		result += "#";


		return result;

	}
	return "";
}

// extern & distant path management (TODO / TO TEST)
bool	FilePathManager::AddExternPath(const std::string& path, int externPathIndex)
{
	if ((externPathIndex >= 0) && (externPathIndex <= 3))
	{
		mExternPath[externPathIndex] = path;

		return true;
	}
	return false;
}

// extern & distant path management (TODO / TO TEST)

bool	FilePathManager::AddDistantPath(const std::string& path, int distantPathIndex)
{
	if ((distantPathIndex >= 0) && (distantPathIndex <= 3))
	{
		mDistantPath[distantPathIndex] = path;
		return true;
	}
	return false;
}

// extern & distant path management (TODO / TO TEST)
bool	FilePathManager::GetDistantPath(int a_IndexPath, std::string& a_distantUrl)
{
	if ((a_IndexPath >= 0) && (a_IndexPath <= 3))
	{
		a_distantUrl = mDistantPath[a_IndexPath];
		return true;
	}
	return false;
}

// extern & distant path management (TODO / TO TEST)
#ifdef HTTP_PROTOCOL
CoreRawBuffer* FilePathManager::GetDistantFile(const char* filename)
{
	if (filename[0] == '#')
	{
		CoreRawBuffer* answer = NULL;
		SmartPointer<FileHandle> L_fullfilenamehandle = Platform_FindFullName(filename);
		if (L_fullfilenamehandle)
		{
			FilePathManager::DeviceID	id = (FilePathManager::DeviceID)(filename[1]);
			if (id >= FilePathManager::DISTANT_FOLDER1 && id <= FilePathManager::DISTANT_FOLDER4)
			{
				unsigned int L_index = 0;
				if (id == FilePathManager::DISTANT_FOLDER2)
					L_index = 1;
				else if (id == FilePathManager::DISTANT_FOLDER3)
					L_index = 2;
				else if (id == FilePathManager::DISTANT_FOLDER4)
					L_index = 3;

				FilePathManager* pathManager = static_cast<FilePathManager*>(KigsCore::GetSingleton("FilePathManager"));
				std::string L_hostname = "";

				if (pathManager->GetDistantPath(L_index, L_hostname))
				{
					HTTPConnect* L_Connection = static_cast<HTTPConnect*>(KigsCore::GetInstanceOf("myConnection", "HTTPConnect"));
					L_Connection->setValue("IsSynchronous", true);
					L_Connection->setValue("HostName", L_hostname);
					L_Connection->Init();

					std::string L_FileName = "fread:";
					L_FileName += (const char*)(&filename[3]);

					HTTPAsyncRequest* L_reponse = L_Connection->retreiveGetRequest(L_FileName.c_str());
					if (L_reponse)
					{
						L_reponse->Init();

						L_reponse->GetAnswer(answer);
						L_reponse->Destroy();
					}
					L_Connection->Close();
					L_Connection->Destroy();
				}
			}
		}
		return answer;
	}
	return NULL;
}


bool FilePathManager::WriteDistantFile(const char* filename, const void* a_content, unsigned long a_contentSize, const char * mode)
{
	bool L_returnedValue = false;
	if (filename[0] == '#')
	{
		usString answer = "";
		std::string nameWithDevicePath = "";

		FilePathManager::DeviceID	id = (FilePathManager::DeviceID)(filename[1]);
		if (id >= FilePathManager::DISTANT_FOLDER1 && id <= FilePathManager::DISTANT_FOLDER4)
		{
			unsigned int L_index = 0;
			if (id == FilePathManager::DISTANT_FOLDER2)
				L_index = 1;
			else if (id == FilePathManager::DISTANT_FOLDER3)
				L_index = 2;
			else if (id == FilePathManager::DISTANT_FOLDER4)
				L_index = 3;

			FilePathManager* pathManager = static_cast<FilePathManager*>(KigsCore::GetSingleton("FilePathManager"));
			std::string L_hostname = "";

			if (pathManager->GetDistantPath(L_index, L_hostname))
			{
				HTTPConnect* L_Connection = static_cast<HTTPConnect*>(KigsCore::GetInstanceOf("myConnection", "HTTPConnect"));
				L_Connection->setValue("IsSynchronous", true);
				L_Connection->setValue("HostName", L_hostname);
				L_Connection->Init();

				char* L_RequestBuffer = NULL;
				unsigned int L_Decal = 0;
				if (strcmp(mode, "wb") == 0)
				{
					L_RequestBuffer = new char[7 + a_contentSize + strlen(filename) - 2 + 1];
					memcpy(L_RequestBuffer, "fwrite:", 7);
					L_Decal += 7;
				}
				else if (strcmp(mode, "ab") == 0)
				{
					L_RequestBuffer = new char[8 + a_contentSize + strlen(filename) - 2 + 1];
					memcpy(L_RequestBuffer, "fappend:", 8);
					L_Decal += 8;
				}

				memcpy(L_RequestBuffer + L_Decal, (const char*)(&filename[3]), strlen(filename) - 3);
				L_Decal += strlen(filename) - 3;
				L_RequestBuffer[L_Decal++] = ' ';
				memcpy(L_RequestBuffer + L_Decal, a_content, a_contentSize);
				L_Decal += a_contentSize;
				L_RequestBuffer[L_Decal++] = 0;

				HTTPAsyncRequest* L_reponse = L_Connection->retreivePostRequest("", L_RequestBuffer, L_Decal);
				if (L_reponse)
				{
					L_reponse->Init();

					L_reponse->GetAnswer(answer);

					if (answer == "true")
						L_returnedValue = true;

					L_reponse->Destroy();
				}
				delete[] L_RequestBuffer;

				L_Connection->Close();
				L_Connection->Destroy();
			}
		}
	}
	return L_returnedValue;
}
#endif

//! init bundle list
void	FilePathManager::InitBundleList(const std::string& filename)
{
	//myBundleList.clear();

	SmartPointer<FileHandle> lFile = FindFullName(filename);

	if (lFile->mStatus&FileHandle::Exist)
	{
		u64 filelen;
		auto rawbuffer = ModuleFileManager::LoadFileAsCharString(lFile.get(), filelen,1);
		if (rawbuffer)
		{
			AsciiParserUtils FileParser(rawbuffer);
			AsciiParserUtils line(FileParser);
			while (FileParser.GetLine(line, true))
			{
				AsciiParserUtils word(line);
				// first word is the key
				std::string key = "";
				if (line.GetWord(word, ';'))
				{
					key = word;
					std::vector<std::string> toAdd;
					mBundleList[key] = toAdd;

					// next are the pathes
					while (line.GetWord(word, ';'))
					{
						mBundleList[key].push_back((const std::string&)word);
					}
				}
			}
		}
	}
}

//! init from config json file
void	FilePathManager::InitWithConfigFile(const std::string& filename)
{
	JSonFileParser L_JsonParser;
	CoreItemSP L_Dictionary = L_JsonParser.Get_JsonDictionary(filename);
	if (L_Dictionary)
	{
		CoreItemSP pathList = L_Dictionary;
		size_t nbpath = pathList->size();
		size_t i;
		for (i = 0; i < nbpath; i += 2)
		{
			CoreItemSP currentpathname = (*pathList)[(int)i];
			CoreItemSP currentpath = (*pathList)[(int)i + 1];
			AddToPath((std::string)*currentpathname.get(), (std::string)*currentpath.get());
		}

	}
}

void	FilePathManager::isSpecialDeviceOrPackage(const std::string& fname,bool& SpecialDevice, bool& package)
{
	SpecialDevice = false;
	package = false;
	if (fname[0] == '#')
	{
		if ((fname[1] == 'P') && (fname[2] == 'K') && (fname[3] == 'G'))
		{
			package = true;
		}
		else
		{
			SpecialDevice = true;
		}
	}
}

bool		Kigs::File::Platform_remove(FileHandle* handle)
{
	// use virtual file access ?
	if (handle->mUseVirtualFileAccess)
	{
		return handle->mVirtualFileAccess->Platform_remove(handle);
	}

	return remove(handle->mFullFileName.c_str()) != 0;
}

SmartPointer<FileHandle> Kigs::File::Platform_fopen(const char * filename, const char * mode)
{
	SmartPointer<FileHandle> fullfilenamehandle;

	bool isDevice, isPackage;
	FilePathManager::isSpecialDeviceOrPackage(filename, isDevice, isPackage);

	if (isDevice)
	{
		fullfilenamehandle = Platform_FindFullName(filename);
		if (fullfilenamehandle)
		{
			Platform_fopen(fullfilenamehandle.get(), mode);
		}
	}
	else if (isPackage)
	{
		SP<FilePathManager> fpm = KigsCore::Singleton<FilePathManager>();
		fullfilenamehandle = fpm->FindFullName(filename);
		Platform_fopen(fullfilenamehandle.get(), mode);
	}
	else
	{
		fullfilenamehandle = FilePathManager::CreateFileHandle(filename);
		fullfilenamehandle->mFullFileName = filename;
		Platform_fopen(fullfilenamehandle.get(), mode);
	}

	return fullfilenamehandle;
}

std::set<CMSP> FilePathManager::mHTTPConnections;

// HTTP file management (should use PureVirtualFileAccessDelegate now)
bool FilePathManager::HTTPfopen(FileHandle* handle, const char * mode, const std::string& L_hostname)
{

	// TODO : retreive separated hostname and url


	CMSP L_Connection = KigsCore::GetInstanceOf("myConnection", "HTTPConnect");
	// check that HTTP module is OK
	if (L_Connection->isSubType("HTTPConnect"))
	{
		L_Connection->setValue("IsSynchronous", true);
		L_Connection->setValue("HostName", L_hostname);
		L_Connection->Init();

		// use FILE* pointer to store connection, weird but OK
		handle->mFile = (PLATFORM_FILE*)L_Connection.get();
		mHTTPConnections.insert(L_Connection);
		// create HTTPASyncRequest

		std::string L_FileName = "fopen:";
		L_FileName += mode;
		L_FileName += ":";

		L_FileName += handle->mFullFileName;

		CMSP request = (KigsCore::GetInstanceOf("HTTPAsyncRequest_file", "HTTPAsyncRequest"));
		request->setValue("Type", "GET");
		request->setValue("URL", L_FileName);
		request->setValue("Connection", L_Connection.get());
		request->Init();

		void* received=nullptr;

		request->getValue("ReceivedBuffer", received);
		if (received)
		{
			std::string receivedID = ((CoreRawBuffer*)received)->buffer();
			if (receivedID != "")
			{
				// store receivedID as a dynamic attribute on L_Connection
				L_Connection->AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE::STRING, "fileid", receivedID.c_str());
			}
			else
			{
				mHTTPConnections.erase(L_Connection);
				handle->mFile = 0;
			}
		}

	}


	return (handle->mFile != 0);
}


// HTTP file management (should use PureVirtualFileAccessDelegate now)
long int FilePathManager::HTTPfread(void * ptr, long size, long count, FileHandle* handle)
{
	long int receivedLen = 0;
	if (handle->mFile)
	{
		CoreModifiable* L_Connection = (CoreModifiable*)handle->mFile;

		char params[32];
		// push parameters
		std::string L_FileName = "fread:";
		std::string receivedID;
		L_Connection->getValue("fileid", receivedID); // retrieve ID
		L_FileName += receivedID + ":";
		sprintf(params, "%li", size);
		L_FileName += params;
		L_FileName += ":";
		sprintf(params, "%li", count);
		L_FileName += params;

		CMSP request = KigsCore::GetInstanceOf("HTTPAsyncRequest_file", "HTTPAsyncRequest");
		request->setValue("Type", "GET");
		request->setValue("URL", L_FileName);
		request->setValue("Connection", L_Connection);
		request->Init();

		void* received=nullptr;

		request->getValue("ReceivedBuffer", received);

		if (received)
		{
			CoreRawBuffer* receivedbuffer = ((CoreRawBuffer*)received);
			receivedLen = receivedbuffer->length();

			memcpy(ptr, receivedbuffer->buffer(), receivedLen);
		}

	}
	return receivedLen;
}
// HTTP file management (should use PureVirtualFileAccessDelegate now)
long int FilePathManager::HTTPfwrite(const void * ptr, long size, long count, FileHandle* handle)
{
	if (handle->mFile)
	{
		// TODO !
		//CoreModifiable* L_Connection = (CoreModifiable*)handle->mFile;
	}
	return 0;
}
// HTTP file management (should use PureVirtualFileAccessDelegate now)
long int FilePathManager::HTTPftell(FileHandle* handle)
{
	long int receivedLen = 0;
	if (handle->mFile)
	{
		CoreModifiable* L_Connection = (CoreModifiable*)handle->mFile;

		// push parameters
		std::string L_FileName = "ftell:";
		std::string receivedID;
		L_Connection->getValue("fileid", receivedID); // retrieve ID
		L_FileName += receivedID;

		CMSP request = KigsCore::GetInstanceOf("HTTPAsyncRequest_file", "HTTPAsyncRequest");
		request->setValue("Type", "GET");
		request->setValue("URL", L_FileName);
		request->setValue("Connection", L_Connection);
		request->Init();

		void* received=nullptr;

		request->getValue("ReceivedBuffer", received);

		if (received)
		{
			std::string receivedLenAscii = ((CoreRawBuffer*)received)->buffer();
			if (receivedLenAscii != "")
			{
				sscanf(receivedLenAscii.c_str(), "%li", &receivedLen);
			}
		}

	}
	return receivedLen;
}

// HTTP file management (should use PureVirtualFileAccessDelegate now)
int FilePathManager::HTTPfseek(FileHandle* handle, long int offset, int origin)
{
	long int receivedLen = 0;
	if (handle->mFile)
	{
		CoreModifiable* L_Connection = (CoreModifiable*)handle->mFile;
		// push parameters
		char params[32];
		std::string L_FileName = "fseek:";
		std::string receivedID;
		L_Connection->getValue("fileid", receivedID); // retrieve ID
		L_FileName += receivedID + ":";
		sprintf(params, "%li", offset);
		L_FileName += params;
		L_FileName += ":";
		sprintf(params, "%i", origin);
		L_FileName += params;

		CMSP request = KigsCore::GetInstanceOf("HTTPAsyncRequest_file", "HTTPAsyncRequest");
		request->setValue("Type", "GET");
		request->setValue("URL", L_FileName);
		request->setValue("Connection", L_Connection);
		request->Init();

		void* received=nullptr;

		request->getValue("ReceivedBuffer", received);

		if (received)
		{
			std::string receivedLenAscii = ((CoreRawBuffer*)received)->buffer();
			if (receivedLenAscii != "")
			{
				sscanf(receivedLenAscii.c_str(), "%li", &receivedLen);
			}
		}

	}
	return receivedLen;

}

// HTTP file management (should use PureVirtualFileAccessDelegate now)
int FilePathManager::HTTPfflush(FileHandle* handle)
{
	if (handle->mFile)
	{
		CoreModifiable* L_Connection = (CoreModifiable*)handle->mFile;
		std::string L_FileName = "fflush:";
		std::string receivedID;
		L_Connection->getValue("fileid", receivedID); // retrieve ID
		L_FileName += receivedID;

		CMSP request = KigsCore::GetInstanceOf("HTTPAsyncRequest_file", "HTTPAsyncRequest");
		request->setValue("Type", "GET");
		request->setValue("URL", L_FileName);
		request->setValue("Connection", L_Connection);
		request->Init();

		void* received=nullptr;

		request->getValue("ReceivedBuffer", received);

		if (received)
		{

		}

	}
	return 0;
}

// HTTP file management (should use PureVirtualFileAccessDelegate now)
int FilePathManager::HTTPfclose(FileHandle* handle)
{
	if (handle->mFile)
	{
		CoreModifiable* L_Connection = (CoreModifiable*)handle->mFile;

		std::string L_FileName = "fclose:";
		std::string receivedID;
		L_Connection->getValue("fileid", receivedID); // retrieve ID
		L_FileName += receivedID;

		CMSP request = KigsCore::GetInstanceOf("HTTPAsyncRequest_file", "HTTPAsyncRequest");
		request->setValue("Type", "GET");
		request->setValue("URL", L_FileName);
		request->setValue("Connection", L_Connection);
		request->Init();

		void* received=nullptr;

		request->getValue("ReceivedBuffer", received);

		if (received)
		{

		}
		mHTTPConnections.erase(L_Connection->SharedFromThis());
		handle->mFile = 0;
	}
	return 0;
}

// Method wrapper to use classic Platform_fopen or VirtualFileAccess delegate
bool		Kigs::File::Platform_fopen(FileHandle* handle, const char * mode)
{
	// use virtual file access ?
	if (handle->mUseVirtualFileAccess)
	{
		return handle->mVirtualFileAccess->Platform_fopen(handle, mode);
	}

	// use classic file access
	return Platform_Hiddenfopen(handle, mode);
}

// Method wrapper to use classic Platform_fopen or VirtualFileAccess delegate
long int	Kigs::File::Platform_fread(void * ptr, long size, long count, FileHandle* handle)
{
	// use virtual file access ?
	if (handle->mUseVirtualFileAccess)
	{
		return handle->mVirtualFileAccess->Platform_fread(ptr,size,count,handle);
	}
	// use classic file access
	return Platform_Hiddenfread(ptr,size,count,handle);
}

// Method wrapper to use classic Platform_fopen or VirtualFileAccess delegate
long int	Kigs::File::Platform_fwrite(const void * ptr, long size, long count, FileHandle* handle)
{
	// use virtual file access ?
	if (handle->mUseVirtualFileAccess)
	{
		return handle->mVirtualFileAccess->Platform_fwrite(ptr,size,count,handle);
	}

	// use classic file access

	return Platform_Hiddenfwrite(ptr, size, count, handle);
}

// Method wrapper to use classic Platform_fopen or VirtualFileAccess delegate
long int	Kigs::File::Platform_ftell(FileHandle* handle)
{
	// use virtual file access ?
	if (handle->mUseVirtualFileAccess)
	{
		return handle->mVirtualFileAccess->Platform_ftell(handle);
	}

	// use classic file access

	return Platform_Hiddenftell(handle);
}

// Method wrapper to use classic Platform_fopen or VirtualFileAccess delegate
int			Kigs::File::Platform_fseek(FileHandle* handle, long int offset, int origin)
{
	// use virtual file access ?
	if (handle->mUseVirtualFileAccess)
	{
		return handle->mVirtualFileAccess->Platform_fseek(handle,offset,origin);
	}

	// use classic file access

	return Platform_Hiddenfseek(handle, offset, origin);
}

// Method wrapper to use classic Platform_fopen or VirtualFileAccess delegate
int			Kigs::File::Platform_fflush(FileHandle* handle)
{
	// use virtual file access ?
	if (handle->mUseVirtualFileAccess)
	{
		return handle->mVirtualFileAccess->Platform_fflush(handle);
	}

	// use classic file access
	return  Platform_Hiddenfflush(handle);
}

// Method wrapper to use classic Platform_fopen or VirtualFileAccess delegate
int			Kigs::File::Platform_fclose(FileHandle* handle)
{
	// use virtual file access ?
	if (handle->mUseVirtualFileAccess)
	{
		return handle->mVirtualFileAccess->Platform_fclose(handle);
	}

	// use classic file access
	return Platform_Hiddenfclose(handle);
}