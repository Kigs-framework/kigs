#ifndef __COREPACKAGE_H__
#define __COREPACKAGE_H__

#include <thread>
#include "CoreModifiable.h"
#include "Platform/Core/PlatformCore.h"
#include "FilePathManager.h"
#include "Core.h"
#include "PureVirtualFileAccessDelegate.h"

// ****************************************
// * CorePackage class
// * --------------------------------------
/**
* \file	CorePackage.h
* \class	CorePackage
* \ingroup FileManager
* \brief	manage packages and files inside packages
* \author	ukn
* \version ukn
* \date	ukn
*/

class FilePathManager;

class CorePackage
{
private:


	CorePackage()
	{
	}

	CorePackage(SmartPointer<FileHandle> fileHandle) : myMainFile(fileHandle)
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
		u32			myHeadID;
		u32			myFATSize;
		u64			myTotalSize;
	};

	struct FATEntry
	{
		u64		myFileOffset;
		u64		myFileSize;
		u32		myFileNameSize;
		u32		mySonCount;
	};

	class FATEntryNode : public FATEntry
	{
	public:
		std::string					myName;
		std::vector<FATEntryNode*>		mySons;
		KigsID							myFastCheckName;

		~FATEntryNode()
		{
			for (auto son : mySons)
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

		package->myFileName = filename;
		
		return package;
	}

	std::string GetName()
	{
		return myFileName;
	}

	class CorePackageIterator
	{
	public:

		CorePackageIterator(const CorePackageIterator& other)
		{
			myPackage = other.myPackage;
			myNodeStack = other.myNodeStack;
			myState = other.myState;
		}

		bool operator==(const CorePackageIterator& other)
		{
			if ((myPackage == other.myPackage) && (myState == other.myState) )
			{
				if (myState == -1)
				{
					return true;
				}
				else if ( (myNodeStack.back().entry == other.myNodeStack.back().entry) && (myNodeStack.back().stackedSonIndex == other.myNodeStack.back().stackedSonIndex) )
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
				return current->myName;
			}
			return "";
		}

		bool	isFolder()
		{
			FATEntryNode*	current = currentEntry();
			if (current)
			{
				return (current->mySons.size()!=0);
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
				for (auto it : myNodeStack)
				{
					if(!it.entry->myName.empty())
						toReturn.push_back(it.entry->myName);
				}
				toReturn.push_back(myNodeStack.back().entry->mySons[myNodeStack.back().stackedSonIndex]->myName);
			}
			return toReturn;
		}

		CorePackageIterator& operator++()
		{
			if (myState == -1)
			{
				return *this;
			}


			FATEntryNode*	current = currentEntry();

			
			// current has unexplored sons ?
			if (myNodeStack.back().entry->mySons.size()>myNodeStack.back().stackedSonIndex)
			{

				myNodeStack.back().stackedSonIndex++;
				if (current->mySons.size())
				{
					myNodeStack.push_back(current);
					return *this;
				}

			}

			while (myNodeStack.back().entry->mySons.size() <= myNodeStack.back().stackedSonIndex)
			{
				myNodeStack.pop_back();
				if (!myNodeStack.size())
				{
					myState = -1;
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
			myPackage(0)
		{
			
		}

		FATEntryNode*	currentEntry()
		{
			if (myState == -1)
			{
				return 0;
			}
		
			return myNodeStack.back().entry->mySons[myNodeStack.back().stackedSonIndex];
		}

		CorePackage*	myPackage;

		struct StackedNode
		{
			StackedNode(FATEntryNode* e) : entry(e), stackedSonIndex(0){}
			FATEntryNode*	entry;
			int				stackedSonIndex;
		};

		std::vector<StackedNode>	myNodeStack;
		int							myState;
	};


	CorePackageIterator	begin()
	{
		CorePackageIterator	toReturn;
		toReturn.myPackage = this;
		toReturn.myNodeStack.push_back(myRootFATEntry);
		toReturn.myState = 0; // ok
		return toReturn;
	}

	CorePackageIterator	end()
	{
		CorePackageIterator	toReturn;
		toReturn.myPackage = this;
		toReturn.myNodeStack.push_back(myRootFATEntry);
		toReturn.myState = -1; // end
		return toReturn;
	}

	FATEntry*	find(const std::string& path, bool isFile);

public:

	~CorePackage() 
	{ 
		if(myMainFile)
			Platform_fclose(myMainFile.get());

		for (auto& thread_read : mThreadRead)
		{
			if(thread_read.second.myFile)
				Platform_fclose(thread_read.second.myFile.get());
		}


#ifdef KIGS_TOOLS
		if (myPackageBuilderStruct)
		{
			KIGS_WARNING("CorePackage destroyed while being in construction", 2);
			delete myPackageBuilderStruct;
		}
#endif

		if (myRootFATEntry)
		{
			delete myRootFATEntry;
		}

	}

	void RenameFile(const std::string& from, const std::string& to);

	template<typename F>
	void  IterateFiles(F func)
	{
		auto f = [&](FATEntryNode* node, const std::string& name)
		{
			func(name + node->myName);
		};
		IterateFATTree(myRootFATEntry, "", f);
	}

	static CorePackage *	CreateNewEmptyPackage()
	{
		CorePackage* package = new CorePackage();

		package->myPackageBuilderStruct = new PackageCreationStruct();
		return package;
	}

	void	AddFile(const std::string& filename, const std::string& filePathInPackage);
	void	AddFolder(const std::string& foldername, const std::string& FolderNameInPackage);
	void	RemoveFile(const std::string& filename);

	void	Export(const std::string& filename);

	void	ImportPackage(CorePackage* to_import);


private:
	SmartPointer<FileHandle>	myMainFile;

	size_t						myFileSize = 0;
	FATEntryNode*				myRootFATEntry = nullptr;
	size_t						myDataStartOffset = 0;
	std::string					myFileName;

	// 1 entry folder cache management (for find)
	//FATEntryNode*				myCachedFATEntry;
	//std::string				myCachedFolder;

	struct ThreadRead
	{
		SmartPointer<FileHandle>	myFile;
		FATEntryNode*				myCachedFATEntry = nullptr;
		std::string				myCachedFolder;
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
			std::string	PhysicalName;
			std::string	PackageName;
		};

		struct FileTreeNode
		{
			std::string				name;
			fileNames*					fileNames;	// null for folder
			std::vector<FileTreeNode>	sons;
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
			toAdd.PhysicalName = filename;
			toAdd.PackageName = filePathInPackage;
			myFileList.push_back(toAdd);
		}

		void	RemoveFile(const std::string& filename)
		{
			for (auto it = myFileList.begin(); it!= myFileList.end(); ++it)
			{
				if (it->PhysicalName == filename)
				{
					myFileList.erase(it);
					break;
				}
			}
		}

		FileTreeNode	getFileTree();

		unsigned int computeFATSize(const FileTreeNode& node);

		void	FillFATExportedStruct(const FileTreeNode& node, FATEntry*& CurrentEntry, u64& currentOffset);

		void ExportFiles(const FileTreeNode& node, SmartPointer<FileHandle>& L_File, unsigned char* tmpBuffer, unsigned int bufferLen);

		FilePathManager* fpm=nullptr;

	private:

		std::vector<fileNames>	myFileList;
	};

	PackageCreationStruct*	myPackageBuilderStruct = nullptr;
	
};

class CorePackageFileAccess : public PureVirtualFileAccessDelegate
{
public:
	CorePackageFileAccess(CorePackage* pack) : myPackage(pack)
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

	CorePackage*					myPackage = nullptr;
	CorePackage::FATEntry*			myFileEntry = nullptr;
	u64								myCurrentReadPos = 0;
};

#endif //__COREPACKAGE_H__
