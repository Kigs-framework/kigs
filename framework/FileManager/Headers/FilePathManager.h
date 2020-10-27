#ifndef _FILEPATHMANAGER_H_
#define _FILEPATHMANAGER_H_

#include "CoreModifiable.h"
#include "SmartPointer.h"
#include "maBool.h"

#include <map>

class CorePackage;
class PureVirtualFileAccessDelegate;


// global functions
extern SmartPointer<FileHandle> Platform_fopen(const char* name, const char* mode);

bool		Platform_fopen(FileHandle* handle, const char* mode);
long int	Platform_fread(void* ptr, long size, long count, FileHandle* handle);
long int	Platform_fwrite(const void* ptr, long size, long count, FileHandle* handle);
long int	Platform_ftell(FileHandle* handle);
int			Platform_fseek(FileHandle* handle, long int offset, int origin);
int			Platform_fflush(FileHandle* handle);
int			Platform_fclose(FileHandle* handle);
bool		Platform_remove(FileHandle* handle);



// ****************************************
// * FileHandle class
// * --------------------------------------
/**  \class FileHandle
*    \file	FilePathManager.h
*	 \brief Platform agnostic FileHandle
*	 \ingroup FileManager
*/
// ****************************************

class FileHandle : public GenericRefCountedBaseClass
{
public:
	FileHandle() : GenericRefCountedBaseClass()
		, mFile(0)
		, mDeviceID(0)
		, mUseVirtualFileAccess(false)
		, mStatus(0)
		, mSize(-1)
	{};

	SmartPointer<FileHandle> MakeCopy();

	virtual ~FileHandle();

	union {
		PLATFORM_FILE*					mFile;						// system platform specific file handle 
		PureVirtualFileAccessDelegate*	mVirtualFileAccess;		// delegate class to access specific file (Packages, system specific files...)
	};
	std::string	mFileName;					// short file name 
	std::string	mFullFileName;				// full path with file name
	std::string	mExtension;				// file extension

	unsigned int	mDeviceID;
	bool			mUseVirtualFileAccess;		// this handle use a virtual file access

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
		return mStatus&(Read|Write|Binary|Append);
	}

	// set current opening flags
	inline void setOpeningFlags(unsigned int flags)
	{
		mStatus &= 0xFFFFFFFF^(Read | Write | Binary | Append);
		mStatus |= flags&(Read | Write | Binary | Append);
	}

	// reset flags 
	inline void resetStatus()
	{
		mStatus &= InfoSet;
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

	int	getFileSize()
	{
		int filelength;
		if (mSize != -1)
		{
			return mSize;
		}
		
		bool needOpenClose = !(mStatus & Open);

		if (needOpenClose)
		{
			Platform_fopen(this, "rb");
		}

		Platform_fseek(this, 0, SEEK_END);
		filelength = Platform_ftell(this);
		Platform_fseek(this, 0, SEEK_SET);

		if (needOpenClose)
		{
			Platform_fclose(this);
		}

		return filelength;
	}

	unsigned int	mStatus; // status flag
	int				mSize;
};


class CoreRawBuffer;


// ****************************************
// * FilePathManager class
// * --------------------------------------
/**  \class FilePathManager
*    \file	FilePathManager.h
*	 \brief this class is used to search for file in a defined Path.
*	 \ingroup FileManager
*/
// ****************************************

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
	void	InitBundleRoot(const std::string& root) { mBundleRoot = root; }

	const kigs::unordered_map<std::string, std::vector<std::string>>& GetBundleList() { return mBundleList; }
	const std::string& GetBundleRoot() { return mBundleRoot; }

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
	// get a pointer on a loaded package
	CorePackage* GetLoadedPackage(const std::string& filename);
	// get the id of an already loaded package
	int	GetPackageID(const std::string& filename);
	// retreive package root path ( #PKGid#/rootname )
	std::string GetPackageRootPath(int id);
	std::string GetPackageRootPath(const std::string& filename);


	void RemoveFromBundleList(const std::string& name) { mBundleList.erase(name); }

	// extensions alias management 
	// keep '.' in both alias and extension
	void	AddAlias(const KigsID& alias, const std::string& extension)
	{
		mExtensionAlias[alias] = extension;
	}
	void	RemoveAlias(const KigsID& alias)
	{
		auto found=mExtensionAlias.find(alias);
		if (found != mExtensionAlias.end())
		{
			mExtensionAlias.erase(found);
		}
	}
	std::string	ResolveAlias(const std::string& alias)
	{
		auto found = mExtensionAlias.find(alias);
		if (found != mExtensionAlias.end())
		{
			return (*found).second;
		}
		return alias;
	}

	static std::string MakeValidFileName(const std::string& filename);
	static void	isSpecialDeviceOrPackage(const std::string& fname, bool& SpecialDevice, bool& package);

protected:

	//! destructor
	virtual ~FilePathManager();
	
	// return the string to add to a path to indicate a special device where to search 
	static std::string GetDevicePathString(DeviceID);

	//! For each extension, a path list. 
	kigs::unordered_map<std::string, std::vector<std::string> >		mPath;

	// extern & distant path management (TODO / TO TEST)
	std::string	mExternPath[4];
	std::string	mDistantPath[4];

	// can use bundle or not (only asset path)
	kigs::unordered_map<std::string, std::vector<std::string>> mBundleList;
	std::string											mBundleRoot;

	// retreive extension and short file name 
	static void	SetFileInfos(FileHandle* handle);

	// protected CorePackage management
	friend class CorePackage;
	void	insertPackage(unsigned int packageID);
	void	unloadPackage(unsigned int packageID);
	bool	initHandleFromPackage(const std::string& lpath, SmartPointer<FileHandle> result, bool in_bundle);
	unsigned int				 mPackageID;
	std::map<int, CorePackage*> mPackageList;

	// extension aliases
	// KigsID key is KigsID(alias), second string is original 
	kigs::unordered_map<KigsID, std::string>	mExtensionAlias;

	// if a path is given, FindFullName search only with the current path 
	maBool	mStrictPath = BASE_ATTRIBUTE(StrictPath, false);
};


#endif //_FILEPATHMANAGER_H_
