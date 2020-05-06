#include "PrecompiledHeaders.h"
#include "FilePathManager.h"
#include "Platform/Core/PlatformCore.h"
#include "CoreRawBuffer.h"
#include "ModuleFileManager.h"
#include "JSonFileParser.h"
#include "AsciiParserUtils.h"
#include "CorePackage.h"
#include "PureVirtualFileAccessDelegate.h"

#ifdef HTTP_PROTOCOL
#include "HTTPConnect.h"
#endif

IMPLEMENT_CLASS_INFO(FilePathManager)

FileHandle::~FileHandle()
{
	if (myStatus & (unsigned int)Open)
	{
		Platform_fclose(this);
	}

	if (myUseVirtualFileAccess)
	{
		delete myVirtualFileAccess;
	}
}


SmartPointer<FileHandle> FileHandle::MakeCopy()
{
	auto result = OwningRawPtrToSmartPtr(new FileHandle());

	result->myFileName = myFileName;
	result->myFullFileName = myFullFileName;
	result->myExtension = myExtension;
	result->myDeviceID = myDeviceID;
	result->myUseVirtualFileAccess = myUseVirtualFileAccess;
	if (myUseVirtualFileAccess)
		result->myVirtualFileAccess = myVirtualFileAccess->MakeCopy();
	else
		result->myFile = nullptr;

	result->resetStatus();

	return result;
}

//! constructor
FilePathManager::FilePathManager(const kstl::string& name, CLASS_NAME_TREE_ARG) : CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG)
, myPackageID(0)
{
	myBundleList.clear();
	myBundleRoot = "";

	myPackageList.clear();
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
	kstl::vector<kstl::string>& localpath = myPath[ext];
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

	if (myPath.find(ext) != myPath.end())
	{
		kstl::vector<kstl::string>& localpath = myPath[ext];

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
	result->myFileName = filename;
	SetFileInfos(result.get());
	return result;
}

// load a package given a FileHandle
bool	FilePathManager::LoadPackage(SmartPointer<FileHandle> L_File)
{
	CorePackage* newpackage = CorePackage::Open(L_File, L_File->myFileName);
	if (newpackage)
	{
		myPackageList[myPackageID] = newpackage;

		insertPackage(myPackageID);

		myPackageID++;
	}
	return newpackage;
}

// load a package given it's filename
bool	FilePathManager::LoadPackage(const kstl::string& filename)
{
	// TODO : check if this package was already loaded ?
	SmartPointer<FileHandle> L_File = Platform_fopen(filename.c_str(), "rb");
	CorePackage* newpackage = CorePackage::Open(L_File, filename);

	if (newpackage)
	{
		myPackageList[myPackageID] = newpackage;

		insertPackage(myPackageID);

		myPackageID++;
	}
	return newpackage;
}

// unload a package given it's filename
void	FilePathManager::UnloadPackage(const kstl::string& filename)
{
	// search package with same filename
	
	kstl::map<int, CorePackage*>::iterator	itP = myPackageList.begin();
	kstl::map<int, CorePackage*>::iterator	itE = myPackageList.end();

	while( itP != itE )
	{

		if ((*itP).second->GetName() == filename)
		{
			unloadPackage((*itP).first);
			myPackageList.erase(itP);
			break;
		}
	}
}

CorePackage* FilePathManager::GetLoadedPackage(const kstl::string& filename)
{
	for (auto& pkg : myPackageList)
	{
		if (pkg.second->GetName() == filename) return pkg.second;
	}
	return nullptr;
}

// add package entry in bundle list
void	FilePathManager::insertPackage(unsigned int packageID)
{
	if (myPackageList.find(packageID) != myPackageList.end())
	{
		CorePackage* package = myPackageList[packageID];
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
				myBundleList[key].push_back(packagePath+it.path(false));
			}
			++it;
		}
	}
}

// unload a package given it's package id (protected)
void	FilePathManager::unloadPackage(unsigned int packageID)
{
	if (myPackageList.find(packageID) != myPackageList.end())
	{
		CorePackage* package = myPackageList[packageID];
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
				
				if (myBundleList.find(key) != myBundleList.end())
				{
					// search good string
					kstl::vector<kstl::string>::iterator itStr;
					for (itStr=myBundleList[key].begin(); itStr != myBundleList[key].end();++itStr)
					{
						if ((*itStr) == packagePath + it.path(false))
						{
							myBundleList[key].erase(itStr);
							break;
						}
					}
					if (myBundleList[key].size() == 0)
					{
						auto toErase = myBundleList.find(key);
						myBundleList.erase(toErase);
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
	if (handle->myStatus&FileHandle::InfoSet)
	{
		return;
	}

	handle->myFullFileName = handle->myFileName;

	int pos = (int)handle->myFileName.rfind("/") + 1;
	int pos1 = (int)handle->myFileName.rfind("\\") + 1;

	if (pos1 > pos)
	{
		pos = pos1;
	}

	if (pos > 0)
	{
		handle->myFileName = handle->myFileName.substr((unsigned int)pos, handle->myFileName.length() - pos);
	}

	pos = (int)handle->myFileName.rfind(".");
	if (pos != -1)
		handle->myExtension = handle->myFileName.substr((unsigned int)pos, handle->myFileName.length() - pos);

	handle->myStatus |= (unsigned int)FileHandle::InfoSet;
}


/*! search the given file in the pathes corresponding to its extension, or in the "all extension" * path
	and if found return the full filename with path
*/
SmartPointer<FileHandle>	FilePathManager::FindFullName(const kstl::string&	filename)
{

	// if the file is in special '#' directory
	if (filename[0] == '#')
		return Platform_FindFullName(filename);

	SmartPointer<FileHandle> result = CreateFileHandle(filename);

	// init fullFileName with filename
	kstl::string fullFileName = result->myFileName;

	// search in bundle
	if (myBundleList.size())
	{
		auto foundInBundle = myBundleList.find(result->myFileName);
		if (foundInBundle != myBundleList.end())
		{
			const kstl::vector<kstl::string>& bundlePathVector = (*foundInBundle).second;
			if (bundlePathVector.size() == 0) // bundle root
			{
				result->myFullFileName = myBundleRoot + result->myFileName;
#ifdef _DEBUG
				Platform_CheckState(result.get());
#else
				result->myStatus |= FileHandle::Exist; // if not debug, if is in bundle, suppose it really exist without having to test
#endif
				if (result->myStatus&FileHandle::Exist)
					return result;

			}
			else // TODO check compatible with current path
			{
				if (bundlePathVector[0][0] == '#') // check if package
				{
					kstl::string head = bundlePathVector[0].substr(1, 3);
					if (head == "PKG")
					{
						// search end # (after package ID)
						size_t pos=bundlePathVector[0].find('#', 3); 

						if (pos != kstl::string::npos)
						{
							kstl::string pkgIDStr = bundlePathVector[0].substr(4, pos - 4);

							int pkgID=0;
							if (sscanf(pkgIDStr.c_str(), "%d", &pkgID))
							{
								if (myPackageList.find(pkgID) != myPackageList.end())
								{
									CorePackage* package = myPackageList[pkgID];
									result->myUseVirtualFileAccess = true;
									result->myVirtualFileAccess = new CorePackageFileAccess(package);

									result->myFullFileName = bundlePathVector[0].substr(pos+1, bundlePathVector[0].length() - pos -1 ) + result->myFileName;

									result->myStatus |= FileHandle::Exist;
									return result;
								}
							}
						}
					}
				}

				result->myFullFileName = myBundleRoot + bundlePathVector[0] + result->myFileName;
#ifdef _DEBUG
				Platform_CheckState(result.get());
#else
				result->myStatus |= FileHandle::Exist; // if not debug, if is in bundle, suppose it really exist without having to test
#endif
				if (result->myStatus&FileHandle::Exist)
					return result;
			}

			return result;
		}
	}

	kstl::string	fileext = "";
	if (result->myExtension != "")
		fileext.append(result->myExtension, 1, result->myExtension.length() - 1);

	// if given filename already has a path, first search with only the path
	if (result->myFileName != result->myFullFileName)
	{
		Platform_CheckState(result.get());
		if (result->myStatus & FileHandle::Exist)
			return result;
	}

	kstl::vector<kstl::string> localpath;
	if (myPath.find(fileext) != myPath.end())
	{
		localpath = myPath[fileext];
	}


	bool finished = false;
	bool searchall = false;

	while (!finished)
	{
		// extension not found
		if (!localpath.size())
		{
			localpath = myPath["*"];
			searchall = true;
		}

		if (localpath.size())
		{
			kstl::vector<kstl::string>::reverse_iterator	it;

			// search from last added path to first one
			for (it = localpath.rbegin(); it != localpath.rend(); ++it)
			{
				result->myFullFileName = (*it);
				result->myFullFileName += "/";
				result->myFullFileName += result->myFileName;

				if ((*it)[0] == '#')
				{
					SmartPointer<FileHandle> specialresult=Platform_FindFullName(result->myFullFileName);
					Platform_CheckState(specialresult.get());
					if (specialresult->myStatus&FileHandle::Exist)
						return specialresult;
				}
				else
				{
					Platform_CheckState(result.get());
					if (result->myStatus&FileHandle::Exist)
						return result;
				}
			}
		}

		if (searchall)
		{
			finished = true;
		}
		localpath = myPath["*"];
		searchall = true;
	}

	result->myFullFileName = filename;
	Platform_CheckState(result.get());

	return result;

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
	if (myPath.find(fileext) != myPath.end())
	{
		localpath = myPath[fileext];
	}

	// extension not found
	if (!localpath.size())
	{
		localpath = myPath["*"];
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
	for (auto it = myPath.begin(); it != myPath.end(); ++it)
	{
		kstl::vector<kstl::string>& list = (*it).second;
		list.clear();
	}

	myPath.clear();
	myBundleList.clear();

	for (auto itpackage : myPackageList)
	{
		delete itpackage.second;
	}

	myPackageList.clear();
}

// return the full path, seperate by ";"
kstl::string	FilePathManager::GetPathString()
{
	kstl::string result = "";
	for (auto it = myPath.begin(); it != myPath.end(); ++it)
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
		myExternPath[externPathIndex] = path;

		return true;
	}
	return false;
}

// extern & distant path management (TODO / TO TEST)

bool	FilePathManager::AddDistantPath(const kstl::string& path, int distantPathIndex)
{
	if ((distantPathIndex >= 0) && (distantPathIndex <= 3))
	{
		myDistantPath[distantPathIndex] = path;
		return true;
	}
	return false;
}

// extern & distant path management (TODO / TO TEST)
bool	FilePathManager::GetDistantPath(int a_IndexPath, kstl::string& a_distantUrl)
{
	if ((a_IndexPath >= 0) && (a_IndexPath <= 3))
	{
		a_distantUrl = myDistantPath[a_IndexPath];
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

	if (lFile->myStatus&FileHandle::Exist)
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
					myBundleList[key] = toAdd;

					// next are the pathes
					while (line.GetWord(word, ';'))
					{
						myBundleList[key].push_back((const kstl::string&)word);
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

SmartPointer<FileHandle> Platform_fopen(const char * filename, const char * mode)
{
	SmartPointer<FileHandle> fullfilenamehandle;
	if (filename[0] == '#')
	{
		fullfilenamehandle = Platform_FindFullName(filename);
		if (fullfilenamehandle)
		{
			Platform_fopen(fullfilenamehandle.get(), mode);
		}
	}
	else
	{

		fullfilenamehandle = FilePathManager::CreateFileHandle(filename);
		fullfilenamehandle->myFullFileName = filename;
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
		handle->myFile = (PLATFORM_FILE*)L_Connection.Pointer();
		L_Connection->GetRef();

		// create HTTPASyncRequest

		kstl::string L_FileName = "fopen:";
		L_FileName += mode;
		L_FileName += ":";

		L_FileName += handle->myFullFileName;

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
				handle->myFile = 0;
			}
		}

	}


	return (handle->myFile != 0);
}

// HTTP file management (should use PureVirtualFileAccessDelegate now)
long int FilePathManager::HTTPfread(void * ptr, long size, long count, FileHandle* handle)
{
	long int receivedLen = 0;
	if (handle->myFile)
	{
		CoreModifiable* L_Connection = (CoreModifiable*)handle->myFile;

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
	if (handle->myFile)
	{
		// TODO !
		//CoreModifiable* L_Connection = (CoreModifiable*)handle->myFile;
	}
	return 0;
}
// HTTP file management (should use PureVirtualFileAccessDelegate now)
long int FilePathManager::HTTPftell(FileHandle* handle)
{
	long int receivedLen = 0;
	if (handle->myFile)
	{
		CoreModifiable* L_Connection = (CoreModifiable*)handle->myFile;

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
	if (handle->myFile)
	{
		CoreModifiable* L_Connection = (CoreModifiable*)handle->myFile;
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
	if (handle->myFile)
	{
		CoreModifiable* L_Connection = (CoreModifiable*)handle->myFile;
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
	if (handle->myFile)
	{
		CoreModifiable* L_Connection = (CoreModifiable*)handle->myFile;

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
		handle->myFile = 0;
	}
	return 0;
}

// Method wrapper to use classic Platform_fopen or VirtualFileAccess delegate
bool		Platform_fopen(FileHandle* handle, const char * mode)
{
	// use virtual file access ?
	if (handle->myUseVirtualFileAccess)
	{
		return handle->myVirtualFileAccess->Platform_fopen(handle, mode);
	}

	// use classic file access
	return Platform_Hiddenfopen(handle, mode);
}

// Method wrapper to use classic Platform_fopen or VirtualFileAccess delegate
long int	Platform_fread(void * ptr, long size, long count, FileHandle* handle)
{
	// use virtual file access ?
	if (handle->myUseVirtualFileAccess)
	{
		return handle->myVirtualFileAccess->Platform_fread(ptr,size,count,handle);
	}
	// use classic file access
	return Platform_Hiddenfread(ptr,size,count,handle);
}

// Method wrapper to use classic Platform_fopen or VirtualFileAccess delegate
long int	Platform_fwrite(const void * ptr, long size, long count, FileHandle* handle)
{
	// use virtual file access ?
	if (handle->myUseVirtualFileAccess)
	{
		return handle->myVirtualFileAccess->Platform_fwrite(ptr,size,count,handle);
	}

	// use classic file access

	return Platform_Hiddenfwrite(ptr, size, count, handle);
}

// Method wrapper to use classic Platform_fopen or VirtualFileAccess delegate
long int	Platform_ftell(FileHandle* handle)
{
	// use virtual file access ?
	if (handle->myUseVirtualFileAccess)
	{
		return handle->myVirtualFileAccess->Platform_ftell(handle);
	}

	// use classic file access

	return Platform_Hiddenftell(handle);
}

// Method wrapper to use classic Platform_fopen or VirtualFileAccess delegate
int			Platform_fseek(FileHandle* handle, long int offset, int origin)
{
	// use virtual file access ?
	if (handle->myUseVirtualFileAccess)
	{
		return handle->myVirtualFileAccess->Platform_fseek(handle,offset,origin);
	}

	// use classic file access

	return Platform_Hiddenfseek(handle, offset, origin);
}

// Method wrapper to use classic Platform_fopen or VirtualFileAccess delegate
int			Platform_fflush(FileHandle* handle)
{
	// use virtual file access ?
	if (handle->myUseVirtualFileAccess)
	{
		return handle->myVirtualFileAccess->Platform_fflush(handle);
	}

	// use classic file access
	return  Platform_Hiddenfflush(handle);
}

// Method wrapper to use classic Platform_fopen or VirtualFileAccess delegate
int			Platform_fclose(FileHandle* handle)
{
	// use virtual file access ?
	if (handle->myUseVirtualFileAccess)
	{
		return handle->myVirtualFileAccess->Platform_fclose(handle);
	}

	// use classic file access
	return Platform_Hiddenfclose(handle);
}