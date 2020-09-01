#ifndef __COREPACKAGE_H__
#define __COREPACKAGE_H__

#include <thread>
#include "CoreModifiable.h"
#include "Platform/Core/PlatformCore.h"
#include "FilePathManager.h"
#include "Core.h"
#include "PureVirtualFileAccessDelegate.h"


class FilePathManager;

// ****************************************
// * CorePackage class
// * --------------------------------------
/**
* \file	CorePackage.h
* \class	CorePackage
* \ingroup FileManager
* \brief	Manage packages and files inside packages.
*
*/
// ****************************************

class CorePackage
{
private:


	CorePackage()
	{
	}

	CorePackage(SmartPointer<FileHandle> fileHandle) : mMainFile(fileHandle)
	{
	}

	/**
	* \brief	constructor - do nothing
	* \fn 		CorePackage(const CorePackage&)
	* \param	CorePackage : not used
	*/
	CorePackage(const CorePackage&) {}

	struct KPKGHeader
	{
		u32			mHeadID;
		u32			mFATSize;
		u64			mTotalSize;
	};

	struct FATEntry
	{
		u64		mFileOffset;
		u64		mFileSize;
		u32		mFileNameSize;
		u32		mSonCount;
	};

	class FATEntryNode : public FATEntry
	{
	public:
		std::string					mName;
		std::vector<FATEntryNode*>		mSons;
		KigsID							mFastCheckName;

		~FATEntryNode()
		{
			for (auto son : mSons)
			{
				delete son;
			}
		}
	};

	void IterateFATTree(FATEntryNode* node, std::string current_path, const std::function<void(FATEntryNode*, const std::string&)>& func);

	void ParseFATBuffer(unsigned char*& fatBuffer, FATEntryNode* current);


protected:

	friend class FilePathManager;
	friend class CorePackageFileAccess;
	/**
	* \brief	init CorePackage from a real file
	* \fn 		static BufferedFile *Open(const char *pFilehdl)
	* \param	pFilehdl : filehandle of the file
	*/
	static CorePackage *	Open(SmartPointer<FileHandle> pFilehdl, const std::string& filename)
	{
		if (!Platform_fopen(pFilehdl.get(), "rb"))
			return nullptr;

		CorePackage* package = new CorePackage(pFilehdl);

		if (!package->initFAT())
		{
			delete package;
			return 0;
		}

		package->mFileName = filename;
		
		return package;
	}

	std::string GetName()
	{
		return mFileName;
	}

	class CorePackageIterator
	{
	public:

		CorePackageIterator(const CorePackageIterator& other)
		{
			mPackage = other.mPackage;
			mNodeStack = other.mNodeStack;
			mState = other.mState;
		}

		bool operator==(const CorePackageIterator& other)
		{
			if ((mPackage == other.mPackage) && (mState == other.mState) )
			{
				if (mState == -1)
				{
					return true;
				}
				else if ( (mNodeStack.back().mEntry == other.mNodeStack.back().mEntry) && (mNodeStack.back().mStackedSonIndex == other.mNodeStack.back().mStackedSonIndex) )
					return true;
			}
			return false;
		}

		bool operator!=(const CorePackageIterator& other)
		{
			return !(*this == other);
		}

		std::string	name()
		{
			FATEntryNode*	current = currentEntry();
			if (current)
			{
				return current->mName;
			}
			return "";
		}

		bool	isFolder()
		{
			FATEntryNode*	current = currentEntry();
			if (current)
			{
				return (current->mSons.size()!=0);
			}
			return false;
		}
		std::string	path(bool full=true)
		{
			std::string result="";
			FATEntryNode*	current = currentEntry();
			if (current)
			{
				std::vector<std::string_view>	pathvector = pathVector();
				int i = 0;
				for (i=0;i<(int)pathvector.size()-1;i++)
				{
					result += pathvector[i];
					result += "/";
				}
				if(full)
					result += pathvector[i];
			}
			return result;
		}

		std::vector<std::string_view>	pathVector()
		{
			std::vector<std::string_view> toReturn;
			FATEntryNode*	current = currentEntry();
			if (current)
			{
				for (auto it : mNodeStack)
				{
					if(!it.mEntry->mName.empty())
						toReturn.push_back(it.mEntry->mName);
				}
				toReturn.push_back(mNodeStack.back().mEntry->mSons[mNodeStack.back().mStackedSonIndex]->mName);
			}
			return toReturn;
		}

		CorePackageIterator& operator++()
		{
			if (mState == -1)
			{
				return *this;
			}


			FATEntryNode*	current = currentEntry();

			
			// current has unexplored sons ?
			if (mNodeStack.back().mEntry->mSons.size()>mNodeStack.back().mStackedSonIndex)
			{

				mNodeStack.back().mStackedSonIndex++;
				if (current->mSons.size())
				{
					mNodeStack.push_back(current);
					return *this;
				}

			}

			while (mNodeStack.back().mEntry->mSons.size() <= mNodeStack.back().mStackedSonIndex)
			{
				mNodeStack.pop_back();
				if (!mNodeStack.size())
				{
					mState = -1;
					break;
				}
			}

			return *this;
		}

		CorePackageIterator operator++(int)
		{
			CorePackageIterator	tmp(*this);
			operator++();
			return tmp;
		}

	protected:
		
		friend class CorePackage;
		CorePackageIterator() :
			mPackage(0)
		{
			
		}

		FATEntryNode*	currentEntry()
		{
			if (mState == -1)
			{
				return 0;
			}
		
			return mNodeStack.back().mEntry->mSons[mNodeStack.back().mStackedSonIndex];
		}

		CorePackage*	mPackage;

		struct StackedNode
		{
			StackedNode(FATEntryNode* e) : mEntry(e), mStackedSonIndex(0){}
			FATEntryNode*	mEntry;
			int				mStackedSonIndex;
		};

		std::vector<StackedNode>	mNodeStack;
		int							mState;
	};


	CorePackageIterator	begin()
	{
		CorePackageIterator	toReturn;
		toReturn.mPackage = this;
		toReturn.mNodeStack.push_back(mRootFATEntry);
		toReturn.mState = 0; // ok
		return toReturn;
	}

	CorePackageIterator	end()
	{
		CorePackageIterator	toReturn;
		toReturn.mPackage = this;
		toReturn.mNodeStack.push_back(mRootFATEntry);
		toReturn.mState = -1; // end
		return toReturn;
	}

	FATEntry*	find(const std::string& path, bool isFile);

public:

	~CorePackage() 
	{ 
		if(mMainFile)
			Platform_fclose(mMainFile.get());

		for (auto& thread_read : mThreadRead)
		{
			if(thread_read.second.mFile)
				Platform_fclose(thread_read.second.mFile.get());
		}


#ifdef KIGS_TOOLS
		if (mPackageBuilderStruct)
		{
			KIGS_WARNING("CorePackage destroyed while being in construction", 2);
			delete mPackageBuilderStruct;
		}
#endif

		if (mRootFATEntry)
		{
			delete mRootFATEntry;
		}

	}

	void RenameFile(const std::string& from, const std::string& to);

	template<typename F>
	void  IterateFiles(F func)
	{
		auto f = [&](FATEntryNode* node, const std::string& name)
		{
			func(name + node->mName);
		};
		IterateFATTree(mRootFATEntry, "", f);
	}

	static CorePackage *	CreateNewEmptyPackage()
	{
		CorePackage* package = new CorePackage();

		package->mPackageBuilderStruct = new PackageCreationStruct();
		return package;
	}

	void	AddFile(const std::string& filename, const std::string& filePathInPackage);
	void	AddFolder(const std::string& foldername, const std::string& FolderNameInPackage);
	void	RemoveFile(const std::string& filename);

	void	Export(const std::string& filename);

	void	ImportPackage(CorePackage* to_import);


private:
	SmartPointer<FileHandle>	mMainFile;

	size_t						mFileSize = 0;
	FATEntryNode*				mRootFATEntry = nullptr;
	size_t						mDataStartOffset = 0;
	std::string					mFileName;

	// 1 mEntry folder cache management (for find)
	//FATEntryNode*				mCachedFATEntry;
	//std::string				mCachedFolder;

	struct ThreadRead
	{
		SmartPointer<FileHandle>	mFile;
		FATEntryNode*				mCachedFATEntry = nullptr;
		std::string				mCachedFolder;
	};

	kigs::unordered_map<std::thread::id, ThreadRead> mThreadRead;

	ThreadRead& GetCurrentThreadRead() { return mThreadRead[std::this_thread::get_id()]; }

	// return true if everything is OK, else return false
	bool	initFAT();


#ifdef WIN32
	void	RecursiveAddFolder(const std::string& foldername, const std::string& FolderNameInPackage, int cropFilePath);
#endif

	class PackageCreationStruct
	{

	public:

		struct fileNames
		{
			std::string	mPhysicalName;
			std::string	mPackageName;
		};

		struct FileTreeNode
		{
			std::string				mName;
			fileNames*					mFileNames;	// null for folder
			std::vector<FileTreeNode>	mSons;
		};

		PackageCreationStruct() 
		{

		}
		~PackageCreationStruct()
		{

		}

		void	AddFile(const std::string& filename, const std::string& filePathInPackage)
		{
			fileNames	toAdd;
			toAdd.mPhysicalName = filename;
			toAdd.mPackageName = filePathInPackage;
			mFileList.push_back(toAdd);
		}

		void	RemoveFile(const std::string& filename)
		{
			for (auto it = mFileList.begin(); it!= mFileList.end(); ++it)
			{
				if (it->mPhysicalName == filename)
				{
					mFileList.erase(it);
					break;
				}
			}
		}

		FileTreeNode	getFileTree();

		unsigned int computeFATSize(const FileTreeNode& node);

		void	FillFATExportedStruct(const FileTreeNode& node, FATEntry*& CurrentEntry, u64& currentOffset);

		void ExportFiles(const FileTreeNode& node, SmartPointer<FileHandle>& L_File, unsigned char* tmpBuffer, unsigned int bufferLen);

		FilePathManager* mFPM=nullptr;

	private:

		std::vector<fileNames>	mFileList;
	};

	PackageCreationStruct*	mPackageBuilderStruct = nullptr;
	
};

// ****************************************
// * CorePackageFileAccess class
// * --------------------------------------
/**
* \file	CorePackage.h
* \class	CorePackageFileAccess
* \ingroup FileManager
* \brief	File access delegate for integration in Kigs generic file management
*
*/
// ****************************************

class CorePackageFileAccess : public PureVirtualFileAccessDelegate
{
public:
	CorePackageFileAccess(CorePackage* pack) : mPackage(pack)
	{

	}

	bool		Platform_fopen(FileHandle* handle, const char * mode) override;
	long int	Platform_fread(void * ptr, long size, long count, FileHandle* handle) override;
	long int	Platform_fwrite(const void * ptr, long size, long count, FileHandle* handle) override;
	long int	Platform_ftell(FileHandle* handle) override;
	int			Platform_fseek(FileHandle* handle, long int offset, int origin) override;
	int			Platform_fflush(FileHandle* handle) override;
	int			Platform_fclose(FileHandle* handle) override;
	PureVirtualFileAccessDelegate* MakeCopy() override;

protected:

	CorePackageFileAccess()
	{

	}

	virtual ~CorePackageFileAccess()
	{

	}

	CorePackage*					mPackage = nullptr;
	CorePackage::FATEntry*			mFileEntry = nullptr;
	u64								mCurrentReadPos = 0;
};

#endif //__COREPACKAGE_H__
