#ifndef _FILEPATHMANAGER_H_
#define _FILEPATHMANAGER_H_

#include "CoreModifiable.h"
#include "SmartPointer.h"

class CorePackage;
class PureVirtualFileAccessDelegate;

// ****************************************
// * FilePathManager class
// * --------------------------------------
/*!  \class FilePathManager
	 this class is used to search for file in a defined Path.
	 
	 Enrich the path like this :

	 auto&	pathManager=KigsCore::Singleton<FilePathManager>();
	 pathManager->AddToPath("../Data/textures","tga"); // relative to execution directory
	 pathManager->AddToPath("../Data","*"); // search all extension in Data

	 Search for a file like this :

	 auto&	pathManager=KigsCore::Singleton<FilePathManager>();

	 std::string fullfilename;
	 // search for file "fileName" and return its full path in fullfilename
	 if(pathManager->FindFullName(fileName,fullfilename))
	 {
		PLATFORM_FILE* animfile=Platform_fopen(fullfilename.c_str(),"rb");
		// ...
	 }	
	 \ingroup FileManager
*/
// ****************************************

class FileHandle : public RefCountedBaseClass
{
public:
	FileHandle(): RefCountedBaseClass()
		, myFile(0)
		, myDeviceID(0)
		, myUseVirtualFileAccess(false)
		, myStatus(0)
		, mySize(-1)
	{};

	SmartPointer<FileHandle> MakeCopy();

	virtual ~FileHandle();

	union {
		PLATFORM_FILE*					myFile;						// system platform specific file handle 
		PureVirtualFileAccessDelegate*	myVirtualFileAccess;		// delegate class to access specific file (Packages, system specific files...)
	};
	std::string	myFileName;					// short file name 
	std::string	myFullFileName;				// full path with file name
	std::string	myExtension;				// file extension

	unsigned int	myDeviceID;
	bool			myUseVirtualFileAccess;		// this handle use a virtual file access

	// convert string opening flag (ie : "rb" "wb"...) in unsigned int flag  
	static inline unsigned int OpeningFlags(const char* mode)
	{
		unsigned int result = 0;
		if (mode[0] == 'r')
		{
			result |= Read;
		}
		else if (mode[0] == 'w')
		{
			result |= Write;
		}
		else if (mode[0] == 'a')
		{
			result |= Write;
			result |= Append;
		}

		int i = 1;
		char c=mode[i];
		while (c != 0)
		{
			if(c=='b')
				result |= Binary;
			if (c == '+')
			{
				result |= Read;
				result |= Write;
			}
			
			c = mode[++i];
		}

		return result;
	}

	// retreive current opening flags
	inline unsigned int getOpeningFlags()
	{
		return myStatus&(Read|Write|Binary|Append);
	}

	// set current opening flags
	inline void setOpeningFlags(unsigned int flags)
	{
		myStatus &= 0xFFFFFFFF^(Read | Write | Binary | Append);
		myStatus |= flags&(Read | Write | Binary | Append);
	}

	// reset flags 
	inline void resetStatus()
	{
		myStatus &= InfoSet;
	}

	enum FileHandleStatusFlag
	{
		NotInit				=	0,
		Open				=	1,
		Closed				=	1<<1,
		Read				=	1<<2,
		Write				=	1<<3,
		Binary				=	1<<4,
		Append              =   1<<5,
		IsDIr				=	1<<6,
		Exist				=	1<<7,
		InfoSet				=   1<<8,
	};

	unsigned int	myStatus; // status flag
	int				mySize;

protected:
	// ProtectedDestroy must ve overload, but nothing to do here
	void	ProtectedDestroy() override {}
};



class CoreRawBuffer;
class FilePathManager : public CoreModifiable
{
public:

	enum	DeviceID
	{
		RESSOURCES					=	0,
		CARD_STORAGE				=	1,
		DEVICE_STORAGE				=	2,
		APPLICATION_STORAGE			=	3,
		RESERVED1					=	4,
		DOCUMENT_FOLDER				=	5,
		DB_STORAGE					=	6,
		DRIVEA						=	10,
		DISTANT_FOLDER1				=	20,
		DISTANT_FOLDER2				=	21,
		DISTANT_FOLDER3				=	22,
		DISTANT_FOLDER4				=	23,
		EXTERN_FOLDER1				=	30,
		EXTERN_FOLDER2				=	31,
		EXTERN_FOLDER3				=	32,
		EXTERN_FOLDER4				=	33,

	};
	
	DECLARE_CLASS_INFO(FilePathManager,CoreModifiable,FileManager)
	
	//! constructor
	FilePathManager(const std::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	//! add a path to the current PATH for the given extension
	void	AddToPath(const std::string& path,std::string ext="*",DeviceID deviceID=RESSOURCES);
	
	//!	remove from path	
	void	RemoveFromPath(const std::string& path,std::string ext="*",DeviceID deviceID=RESSOURCES);

	//! init bundle list
	void	InitBundleList(const std::string& filename);

	//! init bundle root
	void	InitBundleRoot(const std::string& root) { myBundleRoot = root; }

	const kigs::unordered_map<std::string, std::vector<std::string>>& GetBundleList() { return myBundleList; }
	const std::string& GetBundleRoot() { return myBundleRoot; }

	//! init from config json file
	void	InitWithConfigFile(const std::string& filename);

	//! find the fullPath for the given file
	SmartPointer<FileHandle> FindFullName(const std::string&	filename);

	//! construct the "prefered path" according to filename extension
	std::string	PreferedPath(const std::string&	filename);

	//! construct path according to device
	static std::string	DevicePath(const std::string&	filename,DeviceID);

	// extern & distant path management (TODO / TO TEST)
	bool	AddExternPath(const std::string& path, int externPathIndex);
	bool	AddDistantPath(const std::string& path, int distantPathIndex);
	bool	GetDistantPath(int a_IndexPath, std::string& a_distantUrl);

	//! clear all pathes
	void Clear();

#ifdef HTTP_PROTOCOL
	//! Open and get a distant file
	CoreRawBuffer*	GetDistantFile(const char* filename);

	bool		WriteDistantFile(const char* filename, const void* a_content, unsigned long a_contentSize, const char * mode);
#endif

	//! return the full path, seperate by ";"
	std::string	GetPathString();

	// HTTP file management (should use PureVirtualFileAccessDelegate now)
	static bool HTTPfopen(FileHandle* handle, const char * mode, const std::string& L_hostname);
	static long int HTTPfread(void * ptr, long size, long count, FileHandle* handle);
	static long int HTTPfwrite(const void * ptr, long size, long count, FileHandle* handle);
	static long int HTTPftell(FileHandle* handle);
	static int HTTPfseek(FileHandle* handle, long int offset, int origin);
	static int HTTPfflush(FileHandle* handle);
	static int HTTPfclose(FileHandle* handle);

	// create a file handle from a filename
	static SmartPointer<FileHandle> CreateFileHandle(const std::string& filename);

	// retreive parent directory for the given full path
	static std::string	GetParentDirectory(const std::string& fullPath);

	// package management :
	// load a package given a FileHandle
	bool	LoadPackage(SmartPointer<FileHandle> toload);
	// load a package given it's filename
	bool	LoadPackage(const std::string& filename);
	// unload a package given it's filename
	void	UnloadPackage(const std::string& filename);

	CorePackage* GetLoadedPackage(const std::string& filename);

	void RemoveFromBundleList(const std::string& name) { myBundleList.erase(name); }

protected:

	//! destructor
	virtual ~FilePathManager();
	
	// return the string to add to a path to indicate a special device where to search 
	static std::string GetDevicePathString(DeviceID);

	//! For each extension, a path list. 
	kigs::unordered_map<std::string, std::vector<std::string> >		myPath;

	// extern & distant path management (TODO / TO TEST)
	std::string	myExternPath[4];
	std::string	myDistantPath[4];

	// can use bundle or not (only asset path)
	kigs::unordered_map<std::string, std::vector<std::string>> myBundleList;
	std::string											myBundleRoot;

	// retreive extension and short file name 
	static void	SetFileInfos(FileHandle* handle);

	// protected CorePackage management
	friend class CorePackage;
	void	insertPackage(unsigned int packageID);
	void	unloadPackage(unsigned int packageID);
	unsigned int				 myPackageID;
	std::map<int, CorePackage*> myPackageList;

};

extern SmartPointer<FileHandle> Platform_fopen(const char* name, const char * mode);

bool		Platform_fopen(FileHandle* handle, const char * mode);
long int	Platform_fread(void * ptr, long size, long count, FileHandle* handle);
long int	Platform_fwrite(const void * ptr, long size, long count, FileHandle* handle);
long int	Platform_ftell(FileHandle* handle);
int			Platform_fseek(FileHandle* handle, long int offset, int origin);
int			Platform_fflush(FileHandle* handle);
int			Platform_fclose(FileHandle* handle);

#endif //_FILEPATHMANAGER_H_
