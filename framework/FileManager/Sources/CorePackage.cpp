#include "PrecompiledHeaders.h"
#include "CorePackage.h"
#include "FilePathManager.h"


#ifdef WIN32
#include <windows.h>
#include <algorithm>
#endif


void	CorePackage::ParseFATBuffer(unsigned char*& fatBuffer, FATEntryNode* current)
{
	// read current value
	*(FATEntry*)current = *(FATEntry*)fatBuffer;

	unsigned int NodeNameL = (current->mFileNameSize + 1 + 7) & 0xFFFFFFF8;

	// retreive name
	current->mName = (const char*)fatBuffer + sizeof(FATEntry);
	current->mFastCheckName = current->mName;

	fatBuffer += sizeof(FATEntry) + NodeNameL;

	// then read sons
	for (unsigned int i = 0; i < current->mSonCount; i++)
	{
		FATEntryNode* currentson = new FATEntryNode();
		ParseFATBuffer(fatBuffer, currentson);
		current->mSons.push_back(currentson);
	}


}

void CorePackage::RenameFile(const std::string& from, const std::string& to)
{
	auto fat_entry = (FATEntryNode*)find(from, true);
	fat_entry->mName = to;
	fat_entry->mFastCheckName = to;
}

CorePackage::FATEntry*	CorePackage::find(const kstl::string& path,bool isFile)
{

	FATEntryNode* currentEntry = mRootFATEntry;

	size_t rfolderPos = path.rfind('/');
	kstl::string_view fullfolderName(path.c_str(), rfolderPos);

	size_t currentFolderPos = 0;
	bool	isOK = false;

	auto& thread_read = GetCurrentThreadRead();

	if (thread_read.mCachedFATEntry)
	{
		if (fullfolderName == thread_read.mCachedFolder)
		{
			currentFolderPos = rfolderPos+1;
			isOK = true;
			currentEntry = thread_read.mCachedFATEntry;
		}
	}
	

	if (!isOK)
	{
		isOK = true;
		size_t folderPos = path.find('/', 0);
		while (folderPos != kstl::string::npos)
		{
			isOK = false;
			kstl::string_view folderName(path.c_str()+currentFolderPos, folderPos - currentFolderPos);
			const KigsID testName = folderName;
			for (auto sonEntry : currentEntry->mSons)
			{
				if (testName == sonEntry->mFastCheckName)
					if (sonEntry->mName == folderName)
					{
						isOK = true;
						currentEntry = sonEntry;
						break;
					}
			}

			if (!isOK)
			{
				break;
			}
			currentFolderPos = folderPos + 1;
			folderPos = path.find('/', currentFolderPos);
		}
		if (rfolderPos != kstl::string::npos)
		{
			thread_read.mCachedFATEntry = currentEntry;
			thread_read.mCachedFolder = fullfolderName;
		}
	}

	if (isOK)
	{
		isOK = false;
		kstl::string_view fileName(path.c_str()+currentFolderPos, path.length() - currentFolderPos);
		const KigsID testName = fileName;
		for (auto sonEntry : currentEntry->mSons)
		{
			// if search for a file, only test files (no sons)
			if (!isFile || (isFile && sonEntry->mSons.size() == 0))
			{
				if(testName == sonEntry->mFastCheckName)
					if (sonEntry->mName == fileName)
					{
						isOK = true;
						currentEntry = sonEntry;
						break;
					}
			}
		}

		if (isOK)
		{
			return currentEntry;
		}

	}

	return 0;
}

bool	CorePackage::initFAT()
{
	Platform_fseek(mMainFile.get(), 0, SEEK_END);
	mFileSize = Platform_ftell(mMainFile.get());
	Platform_fseek(mMainFile.get(), 0, SEEK_SET);

	// read header kpkg

	KPKGHeader	head;

	Platform_fread(&head, 1, sizeof(KPKGHeader), mMainFile.get());


	if (head.mTotalSize != mFileSize)
	{
		return false;
	}

	if (head.mHeadID != 'kpkg') // little / big endian problem here ?
	{
		return false;
	}

	unsigned char* fatBuffer = (unsigned char*)malloc(head.mFATSize);

	Platform_fread(fatBuffer, 1, head.mFATSize, mMainFile.get());

	// create root

	mRootFATEntry = new FATEntryNode;

	// make a copy because ParseFATBuffer move pointer while reading file
	unsigned char* filledfatBuffer = fatBuffer;

	ParseFATBuffer(filledfatBuffer, mRootFATEntry);

	free(fatBuffer);

	mDataStartOffset = sizeof(KPKGHeader) + head.mFATSize;

	return true;
}


void	CorePackage::AddFile(const kstl::string& filename, const kstl::string& filePathInPackage)
{
	if (!mPackageBuilderStruct)
	{
		KIGS_ERROR("can't add new file to an existing package",1);
		return;
	}
	mPackageBuilderStruct->AddFile(filename, filePathInPackage);
}

void	CorePackage::RemoveFile(const kstl::string& filename)
{
	if (!mPackageBuilderStruct)
	{
		KIGS_ERROR("can't remove file to an existing package", 1);
		return;
	}
	mPackageBuilderStruct->RemoveFile(filename);
}

void CorePackage::IterateFATTree(FATEntryNode* node, std::string current_path, const std::function<void(FATEntryNode*, const std::string&)>& func)
{
	if(node->mSons.empty()) 
		func(node, current_path);
	else if(node->mName.size())
	{
		current_path += node->mName + "/";
	}
	for (auto n : node->mSons)
	{
		IterateFATTree(n, current_path, func);
	}
}

void	CorePackage::ImportPackage(CorePackage* to_import)
{
	IterateFATTree(to_import->mRootFATEntry, "", [this](FATEntryNode* node, const std::string& path)
	{
		AddFile(path + node->mName, path + node->mName);
	});
}



#ifdef WIN32
void	CorePackage::RecursiveAddFolder(const kstl::string& foldername, const kstl::string& FolderNameInPackage, int cropFilePath)
{
	HANDLE			hFind;

	// Iterate through dirs

	std::vector<std::string>	subDirectoryList;
	subDirectoryList.clear();


	WIN32_FIND_DATAA wfd;

	std::string search = foldername + "*";

	hFind = ::FindFirstFileA(search.c_str(), &wfd);

	if (hFind != INVALID_HANDLE_VALUE)
	{
		do
		{

			std::string asciifilename = wfd.cFileName;

			// FIRST check if its a dir
			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{

				// only valid directory
				if (!(asciifilename[0] == '.'))
				{
					subDirectoryList.push_back(foldername + asciifilename + "/");
				}
			}
			else
			{
				if (!(asciifilename[0] == '.'))
				{
					std::string croppedPath = foldername.substr(cropFilePath, foldername.length() - cropFilePath);

					AddFile(foldername + asciifilename, FolderNameInPackage + croppedPath + asciifilename);

				}

			}

		} while (::FindNextFileA(hFind, &wfd));

		::FindClose(hFind);
	}

	// recurse now
	unsigned int i;
	for (i = 0; i<subDirectoryList.size(); i++)
	{
		RecursiveAddFolder(subDirectoryList[i], FolderNameInPackage, cropFilePath);
	}
}
#endif


void	CorePackage::AddFolder(const kstl::string& foldername, const kstl::string& FolderNameInPackage)
{

	if (!mPackageBuilderStruct)
	{
		KIGS_ERROR("can't add new folder to an existing package", 1);
		return;
	}

#ifdef WIN32// recursive folder search only available on windows at the moment

	kstl::string fullfoldername = foldername+"\\";

	RecursiveAddFolder(fullfoldername, FolderNameInPackage, foldername.length());

	
#endif
}

void	CorePackage::Export(const kstl::string& filename)
{

	if (!mPackageBuilderStruct)
	{
		KIGS_ERROR("Package was already exported", 1);
		return;
	}
	

	PackageCreationStruct::FileTreeNode filetree=mPackageBuilderStruct->getFileTree();

	// compute FAT size
	unsigned int fatSize = mPackageBuilderStruct->computeFATSize(filetree);

	unsigned char* FATBlock = (unsigned char*)malloc(fatSize + sizeof(KPKGHeader));

	KPKGHeader*	header = (KPKGHeader*)FATBlock;

	header->mHeadID = 'kpkg';
	header->mFATSize = fatSize;
	header->mTotalSize = 0; // to set later

	FATEntry*	firstEntry = (FATEntry*)(header+1);

	u64 offset=0;
	mPackageBuilderStruct->mFPM = (FilePathManager*)KigsCore::GetSingleton("FilePathManager").get();
	mPackageBuilderStruct->FillFATExportedStruct(filetree, firstEntry, offset);

	// now total size can be set
	header->mTotalSize = fatSize + sizeof(KPKGHeader) + offset;

	// write header + FAT

	SmartPointer<FileHandle> L_File = Platform_fopen(filename.c_str(), "wb");

	Platform_fwrite(FATBlock, fatSize + sizeof(KPKGHeader), 1, L_File.get());

	free(FATBlock);

	// create temp buffer
	unsigned char* tmpbuffer =(unsigned char*) malloc(1024 * 1024);
	mPackageBuilderStruct->ExportFiles(filetree, L_File,tmpbuffer,1024*1024);

	free(tmpbuffer);

	Platform_fclose(L_File.get());
	// exit build mode
	delete mPackageBuilderStruct;
	mPackageBuilderStruct = 0;

}

void replaceAll(std::string& source, const std::string& from, const std::string& to)
{
	std::string newString;
	newString.reserve(source.length());  // avoids a few memory allocations

	std::string::size_type lastPos = 0;
	std::string::size_type findPos;

	while (std::string::npos != (findPos = source.find(from, lastPos)))
	{
		newString.append(source, lastPos, findPos - lastPos);
		newString += to;
		lastPos = findPos + from.length();
	}

	// Care for the rest after last occurrence
	newString += source.substr(lastPos);

	source.swap(newString);
}

kstl::vector<kstl::string> RetreivePath(kstl::string filename, kstl::string& shortfilename)
{
	kstl::vector<kstl::string>	folders;

	replaceAll(filename, "\\", "/");

	kstl::string	remaining = filename;

	bool finish = false;
	while (!finish)
	{
		int pos = remaining.find("/");
		if (pos == kstl::string::npos)
		{
			finish = true;
			shortfilename = remaining;
			break;
		}

		kstl::string	current = remaining.substr(0, pos);
		if (current != "")
		{
			folders.push_back(current);
		}
		while (remaining[pos] == '/')
		{
			pos++;
		}

		remaining = remaining.substr(pos);

	}

	return folders;
	
}

void CorePackage::PackageCreationStruct::ExportFiles(const FileTreeNode& node, SmartPointer<FileHandle>& L_File, unsigned char* tmpBuffer, unsigned int bufferLen)
{
	if (node.mFileNames) // this is a file (not a folder)
	{
		auto L_ReadFile = mFPM->FindFullName(node.mFileNames->mPhysicalName.c_str());
		if (L_ReadFile->mStatus&FileHandle::Exist && Platform_fopen(L_ReadFile.get(), "rb"))
		{
			Platform_fseek(L_ReadFile.get(), 0, SEEK_END);
			u64 filesize = Platform_ftell(L_ReadFile.get());
			Platform_fseek(L_ReadFile.get(), 0, SEEK_SET);
			// round file size on 4 bytes
			u64 padFileSize = (4 - (filesize & 3)) & 3;
			u64 remaining = filesize;
			
			while (remaining)
			{
				unsigned int toRead = bufferLen;
				if (remaining < bufferLen)
				{
					toRead = remaining;
				}
				Platform_fread(tmpBuffer, 1, toRead, L_ReadFile.get());
				Platform_fwrite(tmpBuffer, 1, toRead, L_File.get());
				remaining -= toRead;
			}

			if (padFileSize)
			{
				unsigned int	zeros = 0;
				Platform_fwrite(&zeros, padFileSize, 1, L_File.get());
			}

			Platform_fclose(L_ReadFile.get());
			
		}
		else
		{
			STACK_STRING(err, 2048, "Couldn't open file (errno: %d, statusflag: %d): %s", errno, L_ReadFile->mStatus, node.mFileNames->mPhysicalName.c_str());
			KIGS_ERROR(err, 3);
		}
	}


	for (auto nodeson : node.mSons)
	{
		ExportFiles(nodeson, L_File,tmpBuffer,bufferLen);
	}
}


void	CorePackage::PackageCreationStruct::FillFATExportedStruct(const FileTreeNode& node, FATEntry*& CurrentEntry,u64& currentOffset)
{
	CurrentEntry->mFileNameSize = node.mName.length();
	// add 1 for 0 ended string, then align to 8 bytes
	unsigned int NodeNameL = (node.mName.length() + 1 + 7) & 0xFFFFFFF8;
	unsigned int NodeNameP = NodeNameL - node.mName.length();

	CurrentEntry->mFileOffset = 0;
	CurrentEntry->mFileSize = 0;
	CurrentEntry->mSonCount = node.mSons.size();

	if (node.mFileNames) // this is a file (not a folder)
	{
		SmartPointer<FileHandle> L_File = mFPM->FindFullName(node.mFileNames->mPhysicalName);
		if (L_File->mStatus&FileHandle::Exist && Platform_fopen(L_File.get(), "rb"))
		{
			Platform_fseek(L_File.get(), 0, SEEK_END);
			u64 filesize= Platform_ftell(L_File.get());
			Platform_fseek(L_File.get(), 0, SEEK_SET);
			Platform_fclose(L_File.get());
			CurrentEntry->mFileSize = filesize;
			CurrentEntry->mFileOffset = currentOffset;

			// round file size on 4 bytes

			u64 padFileSize = (4-(filesize & 3)) & 3;

			currentOffset += filesize + padFileSize;
		}

	}


	unsigned char* currentCharEntry = (unsigned char*)CurrentEntry;
	currentCharEntry += sizeof(FATEntry);

	// copy name
	memcpy(currentCharEntry, node.mName.c_str(), node.mName.length());
	memset(currentCharEntry+ node.mName.length(), 0, NodeNameP);

	currentCharEntry += NodeNameL;

	CurrentEntry = (FATEntry*)currentCharEntry;

	for (auto nodeson : node.mSons)
	{
		FillFATExportedStruct(nodeson, CurrentEntry, currentOffset);
	}

}

unsigned int CorePackage::PackageCreationStruct::computeFATSize(const FileTreeNode& node)
{
	unsigned int result = 0;

	// add 1 for 0 ended string, then align to 8 bytes
	result += (node.mName.length()+1+7)&0xFFFFFFF8;
	result += sizeof(FATEntry);

	for (auto nodeson : node.mSons)
	{
		result += computeFATSize(nodeson);
	}

	return result;
}

CorePackage::PackageCreationStruct::FileTreeNode	CorePackage::PackageCreationStruct::getFileTree()
{
	FileTreeNode	root;
	root.mFileNames = 0;
	root.mName = ""; 

	kstl::vector<fileNames>::iterator	it;
	for (it = mFileList.begin(); it != mFileList.end(); ++it)
	{
		kstl::string	fileInPackage = (*it).mPackageName;
		kstl::string    shortfilename = "";
		kstl::vector<kstl::string> path=RetreivePath(fileInPackage, shortfilename);

		FileTreeNode* currentFileTreeNode = &root;

		for (auto itFolder : path)
		{
			bool foundFolder = false;
			// check if folder is already a son of currentFileTreeNode
			for (auto& SonFolder : currentFileTreeNode->mSons)
			{
				if ((SonFolder.mFileNames == 0) && (SonFolder.mName == itFolder))
				{
					currentFileTreeNode = &SonFolder;
					foundFolder = true;
					break;
				}
			}
			
			if (!foundFolder)
			{
				FileTreeNode	toAdd;
				toAdd.mFileNames = 0;
				toAdd.mName = itFolder;
				currentFileTreeNode->mSons.push_back(toAdd);
				currentFileTreeNode = &currentFileTreeNode->mSons.back();
			}
		}

		FileTreeNode	toAdd;
		toAdd.mFileNames = &(*it);
		toAdd.mName = shortfilename;
		currentFileTreeNode->mSons.push_back(toAdd);

	}

	return root;
}

bool		CorePackageFileAccess::Platform_fopen(FileHandle* handle, const char * mode)
{
	unsigned int flags=FileHandle::OpeningFlags(mode);
	if (flags&FileHandle::Write)
	{
		return false;
	}

	handle->setOpeningFlags(flags);

	mCurrentReadPos = 0;

	mFileEntry = mPackage->find(handle->mFullFileName,true);

	auto& th = mPackage->GetCurrentThreadRead();
	if (!th.mFile)
	{
		th.mFile = mPackage->mMainFile->MakeCopy();
		::Platform_fopen(th.mFile.get(), "rb");
	}

	if(mFileEntry)
		return true;

	return false;
}

long int	CorePackageFileAccess::Platform_fread(void * ptr, long size, long count, FileHandle* handle)
{
	if (mFileEntry)
	{
		long available = mFileEntry->mFileSize - mCurrentReadPos;
		long wanted = size*count;
		if (wanted > available)
		{
			wanted = available;
		}

		auto& th = mPackage->GetCurrentThreadRead();

		::Platform_fseek(th.mFile.get(), mCurrentReadPos + mPackage->mDataStartOffset + mFileEntry->mFileOffset, SEEK_SET);
		long read  =::Platform_fread(ptr, size, wanted/size, th.mFile.get());
		mCurrentReadPos += read * size;
		return read;
	}

	return -1;
}

long int	CorePackageFileAccess::Platform_fwrite(const void * ptr, long size, long count, FileHandle* handle)
{
	// can not write in package
	return -1;
}

long int	CorePackageFileAccess::Platform_ftell(FileHandle* handle)
{

	if (mFileEntry)
	{
		return mCurrentReadPos;
	}

	return -1L;
}

int			CorePackageFileAccess::Platform_fseek(FileHandle* handle, long int offset, int origin)
{

	if (mFileEntry)
	{
		long int newpos = mCurrentReadPos;
		switch (origin)
		{
		case SEEK_SET:
			newpos = offset;
			break;
		case SEEK_CUR:
			newpos += offset;
			break;
		case SEEK_END:
			newpos = mFileEntry->mFileSize - offset;
			break;
		}


		if (newpos < 0 )
		{
			newpos = 0;
		}
		if (newpos > mFileEntry->mFileSize)
		{
			newpos = mFileEntry->mFileSize;
		}

		mCurrentReadPos = newpos;

		return 0;
	}


	return -1;
}

int			CorePackageFileAccess::Platform_fflush(FileHandle* handle)
{
	if (mFileEntry)
	{
		auto& th = mPackage->GetCurrentThreadRead();
		return ::Platform_fflush(th.mFile.get());
	}
	return -1;
}

int			CorePackageFileAccess::Platform_fclose(FileHandle* handle)
{
	if (mFileEntry)
	{
		handle->resetStatus();
	}
	return 0;
}

PureVirtualFileAccessDelegate* CorePackageFileAccess::MakeCopy()
{
	auto result = new CorePackageFileAccess();
	result->mPackage = mPackage;
	result->mFileEntry = mFileEntry;
	result->mCurrentReadPos = 0;
	return result;
}