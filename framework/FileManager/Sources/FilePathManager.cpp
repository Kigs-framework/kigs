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
	auto result = OwningRawPtrToSmartPtr(new FileHandle());

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
FilePathManager::FilePathManager(const kstl::string& name, CLASS_NAME_TREE_ARG) : CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG)
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
void	FilePathManager::AddToPath(const kstl::string& path, kstl::string ext, DeviceID deviceID)
{
	kstl::vector<kstl::string>& localpath = mPath[ext];
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
void	FilePathManager::RemoveFromPath(const kstl::string& path, kstl::string ext, DeviceID deviceID)
{
	bool found = true;
	kstl::string	toMatch;
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
		kstl::vector<kstl::string>& localpath = mPath[ext];

		found = true;
		while (found)
		{
			found = false;
			kstl::vector<kstl::string>::iterator itstring;
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
SmartPointer<FileHandle> FilePathManager::CreateFileHandle(const kstl::string& filename)
{
	SmartPointer<FileHandle> result = OwningRawPtrToSmartPtr(new FileHandle());
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
bool	FilePathManager::LoadPackage(const kstl::string& filename)
{
	// TODO : check if this package was already loaded ?
	SmartPointer<FileHandle> L_File = Platform_fopen(filename.c_str(), "rb");
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
void	FilePathManager::UnloadPackage(const kstl::string& filename)
{
	// search package with same filename
	
	kstl::map<int, CorePackage*>::iterator	itP = mPackageList.begin();
	kstl::map<int, CorePackage*>::iterator	itE = mPackageList.end();

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

CorePackage* FilePathManager::GetLoadedPackage(const kstl::string& filename)
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

		kstl::string	packagePath = "#PKG";

		char	packageIDStr[10];
		sprintf(packageIDStr, "%d", packageID);

		packagePath += packageIDStr;
		packagePath += "#";

		// retreive all entries
		while (it != ite)
		{
			if (!it.isFolder())
			{
				kstl::string key = it.name();
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

		kstl::string	packagePath = "#PKG";

		char	packageIDStr[10];
		sprintf(packageIDStr, "%d", packageID);

		packagePath += packageIDStr;
		packagePath += "#";

		// retreive all entries
		while (it != ite)
		{
			if (!it.isFolder())
			{
				kstl::string key = it.name();
				
				if (mBundleList.find(key) != mBundleList.end())
				{
					// search good string
					kstl::vector<kstl::string>::iterator itStr;
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
kstl::string	FilePathManager::GetParentDirectory(const kstl::string& fullPath)
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
SmartPointer<FileHandle>	FilePathManager::FindFullName(const kstl::string&	filename)
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
		if (pos == kstl::string::npos)
		{
			// malformed path
			return result;
		}
		std::string kpkg = filename.substr(0, pos+1);
		initHandleFromPackage(kpkg, result);
			
		return result;
	
	}
	SmartPointer<FileHandle> result = CreateFileHandle(filename);

	// init fullFileName with filename
	kstl::string fullFileName = result->mFileName;

	// search in bundle
	if (mBundleList.size())
	{
		auto foundInBundle = mBundleList.find(result->mFileName);
		if (foundInBundle != mBundleList.end())
		{
			const kstl::vector<kstl::string>& bundlePathVector = (*foundInBundle).second;
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
			else // TODO check compatible with current path
			{
				if (bundlePathVector[0][0] == '#') // check if package
				{
					kstl::string head = bundlePathVector[0].substr(1, 3);
					if (head == "PKG")
					{
						if (initHandleFromPackage(bundlePathVector[0], result))
						{
							return result;
						}
					}
				}

				result->mFullFileName = mBundleRoot + bundlePathVector[0] + result->mFileName;
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

	kstl::string	fileext = "";
	if (result->mExtension != "")
		fileext.append(result->mExtension, 1, result->mExtension.length() - 1);

	kstl::vector<kstl::string> localpath;
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
			kstl::vector<kstl::string>::reverse_iterator	it;

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
					if (pos == kstl::string::npos)
					{
						// malformed path
						return result;
					}
					std::string kpkg = (*it).substr(0, pos + 1);
					initHandleFromPackage(kpkg, result);

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

bool	FilePathManager::initHandleFromPackage(const std::string& lpath, SmartPointer<FileHandle> result)
{
	// search end # (after package ID)
	size_t pos = lpath.find('#', 3);

	if (pos != kstl::string::npos)
	{
		kstl::string pkgIDStr = lpath.substr(4, pos - 4);

		int pkgID = 0;
		if (sscanf(pkgIDStr.c_str(), "%d", &pkgID))
		{
			if (mPackageList.find(pkgID) != mPackageList.end())
			{
				CorePackage* package = mPackageList[pkgID];
				result->mUseVirtualFileAccess = true;
				result->mVirtualFileAccess = new CorePackageFileAccess(package);

				// if lpath only contains #PKGid# use mFullFileName else use lpath
				if ((lpath.length()-1) == pos)
				{
					// remove #PKGid# from full file name
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
kstl::string	FilePathManager::PreferedPath(const kstl::string&	filename)
{
	kstl::string fullFileName = filename;

	int pos = (int)filename.rfind("/") + 1;
	int pos1 = (int)filename.rfind("\\") + 1;

	if (pos1 > pos)
	{
		pos = pos1;
	}

	kstl::string shortfilename;

	if (pos == 0)
	{
		shortfilename = filename;
	}
	else
	{
		shortfilename = filename.substr((unsigned int)pos, filename.length() - pos);
	}

	kstl::string	fileext = "";

	pos = (int)shortfilename.rfind(".") + 1;
	fileext.append(shortfilename, (unsigned int)pos, shortfilename.length() - pos);

	kstl::vector<kstl::string> localpath;
	if (mPath.find(fileext) != mPath.end())
	{
		localpath = mPath[fileext];
	}

	// extension not found
	if (!localpath.size())
	{
		localpath = mPath["*"];
	}


	kstl::vector<kstl::string>::reverse_iterator	it = localpath.rbegin();

	fullFileName = (*it);
	fullFileName += "/";
	fullFileName += shortfilename;

	return fullFileName;
}

// construct path according to device
kstl::string	FilePathManager::DevicePath(const kstl::string&	filename, DeviceID id)
{
	return GetDevicePathString(id) + filename;
}

//! clear all pathes and all struct before destroying the instance
void FilePathManager::Clear()
{
	for (auto it = mPath.begin(); it != mPath.end(); ++it)
	{
		kstl::vector<kstl::string>& list = (*it).second;
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
kstl::string	FilePathManager::GetPathString()
{
	kstl::string result = "";
	for (auto it = mPath.begin(); it != mPath.end(); ++it)
	{
		kstl::vector<kstl::string>& list = (*it).second;

		kstl::vector<kstl::string>::iterator	it1;
		for (it1 = list.begin(); it1 != list.end(); ++it1)
		{
			result += (*it1);
			result += ";";
		}
	}


	return result;
}

//! the returned string will be added at start of the path to be retreived easily by fopen 
kstl::string FilePathManager::GetDevicePathString(DeviceID id)
{
	if (id)
	{
		kstl::string result = "#";

		result += (unsigned char)id;

		result += "#";


		return result;

	}
	return "";
}

// extern & distant path management (TODO / TO TEST)
bool	FilePathManager::AddExternPath(const kstl::string& path, int externPathIndex)
{
	if ((externPathIndex >= 0) && (externPathIndex <= 3))
	{
		mExternPath[externPathIndex] = path;

		return true;
	}
	return false;
}

// extern & distant path management (TODO / TO TEST)

bool	FilePathManager::AddDistantPath(const kstl::string& path, int distantPathIndex)
{
	if ((distantPathIndex >= 0) && (distantPathIndex <= 3))
	{
		mDistantPath[distantPathIndex] = path;
		return true;
	}
	return false;
}

// extern & distant path management (TODO / TO TEST)
bool	FilePathManager::GetDistantPath(int a_IndexPath, kstl::string& a_distantUrl)
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
				kstl::string L_hostname = "";

				if (pathManager->GetDistantPath(L_index, L_hostname))
				{
					HTTPConnect* L_Connection = static_cast<HTTPConnect*>(KigsCore::GetInstanceOf("myConnection", "HTTPConnect"));
					L_Connection->setValue(LABEL_TO_ID(IsSynchronous), true);
					L_Connection->setValue(LABEL_TO_ID(HostName), L_hostname);
					L_Connection->Init();

					kstl::string L_FileName = "fread:";
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
		kstl::string nameWithDevicePath = "";

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
			kstl::string L_hostname = "";

			if (pathManager->GetDistantPath(L_index, L_hostname))
			{
				HTTPConnect* L_Connection = static_cast<HTTPConnect*>(KigsCore::GetInstanceOf("myConnection", "HTTPConnect"));
				L_Connection->setValue(LABEL_TO_ID(IsSynchronous), true);
				L_Connection->setValue(LABEL_TO_ID(HostName), L_hostname);
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
void	FilePathManager::InitBundleList(const kstl::string& filename)
{
	//myBundleList.clear();

	SmartPointer<FileHandle> lFile = FindFullName(filename);

	if (lFile->mStatus&FileHandle::Exist)
	{
		u64 filelen;
		CoreRawBuffer* rawbuffer = ModuleFileManager::LoadFileAsCharString(lFile.get(), filelen,1);

		if (rawbuffer)
		{
			AsciiParserUtils FileParser(rawbuffer);
			AsciiParserUtils line(FileParser);
			while (FileParser.GetLine(line, true))
			{
				AsciiParserUtils word(line);
				// first word is the key
				kstl::string key = "";
				if (line.GetWord(word, ';'))
				{
					key = word;
					kstl::vector<kstl::string> toAdd;
					mBundleList[key] = toAdd;

					// next are the pathes
					while (line.GetWord(word, ';'))
					{
						mBundleList[key].push_back((const kstl::string&)word);
					}
				}
			}
			rawbuffer->Destroy();
		}
	}
}

//! init from config json file
void	FilePathManager::InitWithConfigFile(const kstl::string& filename)
{
	JSonFileParser L_JsonParser;
	CoreItemSP L_Dictionary = L_JsonParser.Get_JsonDictionary(filename);
	if (!L_Dictionary.isNil())
	{
		CoreItemSP pathList = L_Dictionary;
		int nbpath = pathList->size();
		int i;
		for (i = 0; i < nbpath; i += 2)
		{
			CoreItemSP currentpathname = pathList[i];
			CoreItemSP currentpath = pathList[i + 1];
			AddToPath((kstl::string)*currentpathname.get(), (kstl::string)*currentpath.get());
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

bool		Platform_remove(FileHandle* handle)
{
	// use virtual file access ?
	if (handle->mUseVirtualFileAccess)
	{
		return handle->mVirtualFileAccess->Platform_remove(handle);
	}

	return remove(handle->mFullFileName.c_str()) != 0;
}

SmartPointer<FileHandle> Platform_fopen(const char * filename, const char * mode)
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
		SP<FilePathManager> fpm = KigsCore::GetSingleton("FilePathManager");
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

// HTTP file management (should use PureVirtualFileAccessDelegate now)
bool FilePathManager::HTTPfopen(FileHandle* handle, const char * mode, const kstl::string& L_hostname)
{

	// TODO : retreive separated hostname and url


	CMSP L_Connection = KigsCore::GetInstanceOf("myConnection", "HTTPConnect");
	// check that HTTP module is OK
	if (L_Connection->isSubType(LABEL_TO_ID(HTTPConnect)))
	{
		L_Connection->setValue(LABEL_TO_ID(IsSynchronous), true);
		L_Connection->setValue(LABEL_TO_ID(HostName), L_hostname);
		L_Connection->Init();

		// use FILE* pointer to store connection, weird but OK
		handle->mFile = (PLATFORM_FILE*)L_Connection.Pointer();
		L_Connection->GetRef();

		// create HTTPASyncRequest

		kstl::string L_FileName = "fopen:";
		L_FileName += mode;
		L_FileName += ":";

		L_FileName += handle->mFullFileName;

		CMSP request = (KigsCore::GetInstanceOf("HTTPAsyncRequest_file", "HTTPAsyncRequest"));
		request->setValue(LABEL_TO_ID(Type), "GET");
		request->setValue(LABEL_TO_ID(URL), L_FileName);
		request->setValue(LABEL_TO_ID(Connection), L_Connection.Pointer());
		request->Init();

		void* received=nullptr;

		request->getValue(LABEL_TO_ID(ReceivedBuffer), received);
		if (received)
		{
			kstl::string receivedID = ((CoreRawBuffer*)received)->buffer();
			if (receivedID != "")
			{
				// store receivedID as a dynamic attribute on L_Connection
				L_Connection->AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE::STRING, "fileid", receivedID.c_str());
			}
			else
			{
				L_Connection->Destroy();
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
		kstl::string L_FileName = "fread:";
		kstl::string receivedID;
		L_Connection->getValue(LABEL_TO_ID(fileid), receivedID); // retrieve ID
		L_FileName += receivedID + ":";
		sprintf(params, "%li", size);
		L_FileName += params;
		L_FileName += ":";
		sprintf(params, "%li", count);
		L_FileName += params;

		CMSP request = KigsCore::GetInstanceOf("HTTPAsyncRequest_file", "HTTPAsyncRequest");
		request->setValue(LABEL_TO_ID(Type), "GET");
		request->setValue(LABEL_TO_ID(URL), L_FileName);
		request->setValue(LABEL_TO_ID(Connection), L_Connection);
		request->Init();

		void* received=nullptr;

		request->getValue(LABEL_TO_ID(ReceivedBuffer), received);

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
		kstl::string L_FileName = "ftell:";
		kstl::string receivedID;
		L_Connection->getValue(LABEL_TO_ID(fileid), receivedID); // retrieve ID
		L_FileName += receivedID;

		CMSP request = KigsCore::GetInstanceOf("HTTPAsyncRequest_file", "HTTPAsyncRequest");
		request->setValue(LABEL_TO_ID(Type), "GET");
		request->setValue(LABEL_TO_ID(URL), L_FileName);
		request->setValue(LABEL_TO_ID(Connection), L_Connection);
		request->Init();

		void* received=nullptr;

		request->getValue(LABEL_TO_ID(ReceivedBuffer), received);

		if (received)
		{
			kstl::string receivedLenAscii = ((CoreRawBuffer*)received)->buffer();
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
		kstl::string L_FileName = "fseek:";
		kstl::string receivedID;
		L_Connection->getValue(LABEL_TO_ID(fileid), receivedID); // retrieve ID
		L_FileName += receivedID + ":";
		sprintf(params, "%li", offset);
		L_FileName += params;
		L_FileName += ":";
		sprintf(params, "%i", origin);
		L_FileName += params;

		CMSP request = KigsCore::GetInstanceOf("HTTPAsyncRequest_file", "HTTPAsyncRequest");
		request->setValue(LABEL_TO_ID(Type), "GET");
		request->setValue(LABEL_TO_ID(URL), L_FileName);
		request->setValue(LABEL_TO_ID(Connection), L_Connection);
		request->Init();

		void* received=nullptr;

		request->getValue(LABEL_TO_ID(ReceivedBuffer), received);

		if (received)
		{
			kstl::string receivedLenAscii = ((CoreRawBuffer*)received)->buffer();
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
		kstl::string L_FileName = "fflush:";
		kstl::string receivedID;
		L_Connection->getValue(LABEL_TO_ID(fileid), receivedID); // retrieve ID
		L_FileName += receivedID;

		CMSP request = KigsCore::GetInstanceOf("HTTPAsyncRequest_file", "HTTPAsyncRequest");
		request->setValue(LABEL_TO_ID(Type), "GET");
		request->setValue(LABEL_TO_ID(URL), L_FileName);
		request->setValue(LABEL_TO_ID(Connection), L_Connection);
		request->Init();

		void* received=nullptr;

		request->getValue(LABEL_TO_ID(ReceivedBuffer), received);

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

		kstl::string L_FileName = "fclose:";
		kstl::string receivedID;
		L_Connection->getValue(LABEL_TO_ID(fileid), receivedID); // retrieve ID
		L_FileName += receivedID;

		CMSP request = KigsCore::GetInstanceOf("HTTPAsyncRequest_file", "HTTPAsyncRequest");
		request->setValue(LABEL_TO_ID(Type), "GET");
		request->setValue(LABEL_TO_ID(URL), L_FileName);
		request->setValue(LABEL_TO_ID(Connection), L_Connection);
		request->Init();

		void* received=nullptr;

		request->getValue(LABEL_TO_ID(ReceivedBuffer), received);

		if (received)
		{

		}

		L_Connection->Destroy();
		handle->mFile = 0;
	}
	return 0;
}

// Method wrapper to use classic Platform_fopen or VirtualFileAccess delegate
bool		Platform_fopen(FileHandle* handle, const char * mode)
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
long int	Platform_fread(void * ptr, long size, long count, FileHandle* handle)
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
long int	Platform_fwrite(const void * ptr, long size, long count, FileHandle* handle)
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
long int	Platform_ftell(FileHandle* handle)
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
int			Platform_fseek(FileHandle* handle, long int offset, int origin)
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
int			Platform_fflush(FileHandle* handle)
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
int			Platform_fclose(FileHandle* handle)
{
	// use virtual file access ?
	if (handle->mUseVirtualFileAccess)
	{
		return handle->mVirtualFileAccess->Platform_fclose(handle);
	}

	// use classic file access
	return Platform_Hiddenfclose(handle);
}