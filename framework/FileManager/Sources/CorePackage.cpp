#include "PrecompiledHeaders.h"
#include "CorePackage.h"
#include "FilePathManager.h"

#ifdef KIGS_TOOLS
#ifdef WIN32
#include <windows.h>
#include <algorithm>
#endif
#endif

void	CorePackage::ParseFATBuffer(unsigned char*& fatBuffer, FATEntryNode* current)
{
	// read current value
	*(FATEntry*)current = *(FATEntry*)fatBuffer;

	unsigned int NodeNameL = (current->myFileNameSize + 1 + 7) & 0xFFFFFFF8;

	// retreive name
	current->myName = (const char*)fatBuffer + sizeof(FATEntry);
	current->myFastCheckName = current->myName;

	fatBuffer += sizeof(FATEntry) + NodeNameL;

	// then read sons
	for (unsigned int i = 0; i < current->mySonCount; i++)
	{
		FATEntryNode* currentson = new FATEntryNode();
		ParseFATBuffer(fatBuffer, currentson);
		current->mySons.push_back(currentson);
	}


}

void CorePackage::RenameFile(const std::string& from, const std::string& to)
{
	auto fat_entry = (FATEntryNode*)find(from, true);
	fat_entry->myName = to;
	fat_entry->myFastCheckName = to;
}

CorePackage::FATEntry*	CorePackage::find(const kstl::string& path,bool isFile)
{

	FATEntryNode* currentEntry = myRootFATEntry;

	size_t rfolderPos = path.rfind('/');
	kstl::string_view fullfolderName(path.c_str(), rfolderPos);

	size_t currentFolderPos = 0;
	bool	isOK = false;

	auto& thread_read = GetCurrentThreadRead();

	if (thread_read.myCachedFATEntry)
	{
		if (fullfolderName == thread_read.myCachedFolder)
		{
			currentFolderPos = rfolderPos+1;
			isOK = true;
			currentEntry = thread_read.myCachedFATEntry;
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
			for (auto sonEntry : currentEntry->mySons)
			{
				if (testName == sonEntry->myFastCheckName)
					if (sonEntry->myName == folderName)
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
			thread_read.myCachedFATEntry = currentEntry;
			thread_read.myCachedFolder = fullfolderName;
		}
	}

	if (isOK)
	{
		isOK = false;
		kstl::string_view fileName(path.c_str()+currentFolderPos, path.length() - currentFolderPos);
		const KigsID testName = fileName;
		for (auto sonEntry : currentEntry->mySons)
		{
			// if search for a file, only test files (no sons)
			if (!isFile || (isFile && sonEntry->mySons.size() == 0))
			{
				if(testName == sonEntry->myFastCheckName)
					if (sonEntry->myName == fileName)
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
	Platform_fseek(myMainFile.get(), 0, SEEK_END);
	myFileSize = Platform_ftell(myMainFile.get());
	Platform_fseek(myMainFile.get(), 0, SEEK_SET);

	// read header kpkg

	KPKGHeader	head;

	Platform_fread(&head, 1, sizeof(KPKGHeader), myMainFile.get());


	if (head.myTotalSize != myFileSize)
	{
		return false;
	}

	if (head.myHeadID != 'kpkg') // little / big endian problem here ?
	{
		return false;
	}

	unsigned char* fatBuffer = (unsigned char*)malloc(head.myFATSize);

	Platform_fread(fatBuffer, 1, head.myFATSize, myMainFile.get());

	// create root

	myRootFATEntry = new FATEntryNode;

	// make a copy because ParseFATBuffer move pointer while reading file
	unsigned char* filledfatBuffer = fatBuffer;

	ParseFATBuffer(filledfatBuffer, myRootFATEntry);

	free(fatBuffer);

	myDataStartOffset = sizeof(KPKGHeader) + head.myFATSize;

	return true;
}

#ifdef KIGS_TOOLS

void	CorePackage::AddFile(const kstl::string& filename, const kstl::string& filePathInPackage)
{
	if (!myPackageBuilderStruct)
	{
		KIGS_ERROR("can't add new file to an existing package",1);
		return;
	}
	myPackageBuilderStruct->AddFile(filename, filePathInPackage);
}

void	CorePackage::RemoveFile(const kstl::string& filename)
{
	if (!myPackageBuilderStruct)
	{
		KIGS_ERROR("can't remove file to an existing package", 1);
		return;
	}
	myPackageBuilderStruct->RemoveFile(filename);
}

void CorePackage::IterateFATTree(FATEntryNode* node, std::string current_path, const std::function<void(FATEntryNode*, const std::string&)>& func)
{
	if(node->mySons.empty()) 
		func(node, current_path);
	else if(node->myName.size())
	{
		current_path += node->myName + "/";
	}
	for (auto n : node->mySons)
	{
		IterateFATTree(n, current_path, func);
	}
}

void	CorePackage::ImportPackage(CorePackage* to_import)
{
	IterateFATTree(to_import->myRootFATEntry, "", [this](FATEntryNode* node, const std::string& path)
	{
		AddFile(path + node->myName, path + node->myName);
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

	if (!myPackageBuilderStruct)
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

	if (!myPackageBuilderStruct)
	{
		KIGS_ERROR("Package was already exported", 1);
		return;
	}
	

	PackageCreationStruct::FileTreeNode filetree=myPackageBuilderStruct->getFileTree();

	// compute FAT size
	unsigned int fatSize = myPackageBuilderStruct->computeFATSize(filetree);

	unsigned char* FATBlock = (unsigned char*)malloc(fatSize + sizeof(KPKGHeader));

	KPKGHeader*	header = (KPKGHeader*)FATBlock;

	header->myHeadID = 'kpkg';
	header->myFATSize = fatSize;
	header->myTotalSize = 0; // to set later

	FATEntry*	firstEntry = (FATEntry*)(header+1);

	u64 offset=0;
	myPackageBuilderStruct->fpm = KigsCore::GetSingleton<FilePathManager>();
	myPackageBuilderStruct->FillFATExportedStruct(filetree, firstEntry, offset);

	// now total size can be set
	header->myTotalSize = fatSize + sizeof(KPKGHeader) + offset;

	// write header + FAT

	SmartPointer<FileHandle> L_File = Platform_fopen(filename.c_str(), "wb");

	Platform_fwrite(FATBlock, fatSize + sizeof(KPKGHeader), 1, L_File.get());

	free(FATBlock);

	// create temp buffer
	unsigned char* tmpbuffer =(unsigned char*) malloc(1024 * 1024);
	myPackageBuilderStruct->ExportFiles(filetree, L_File,tmpbuffer,1024*1024);

	free(tmpbuffer);

	Platform_fclose(L_File.get());
	// exit build mode
	delete myPackageBuilderStruct;
	myPackageBuilderStruct = 0;

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
	if (node.fileNames) // this is a file (not a folder)
	{
		auto L_ReadFile = fpm->FindFullName(node.fileNames->PhysicalName.c_str());
		if (L_ReadFile->myStatus&FileHandle::Exist && Platform_fopen(L_ReadFile.get(), "rb"))
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
			STACK_STRING(err, 2048, "Couldn't open file (errno: %d, statusflag: %d): %s", errno, L_ReadFile->myStatus, node.fileNames->PhysicalName.c_str());
			KIGS_ERROR(err, 3);
		}
	}


	for (auto nodeson : node.sons)
	{
		ExportFiles(nodeson, L_File,tmpBuffer,bufferLen);
	}
}


void	CorePackage::PackageCreationStruct::FillFATExportedStruct(const FileTreeNode& node, FATEntry*& CurrentEntry,u64& currentOffset)
{
	CurrentEntry->myFileNameSize = node.name.length();
	// add 1 for 0 ended string, then align to 8 bytes
	unsigned int NodeNameL = (node.name.length() + 1 + 7) & 0xFFFFFFF8;
	unsigned int NodeNameP = NodeNameL - node.name.length();

	CurrentEntry->myFileOffset = 0;
	CurrentEntry->myFileSize = 0;
	CurrentEntry->mySonCount = node.sons.size();

	if (node.fileNames) // this is a file (not a folder)
	{
		SmartPointer<FileHandle> L_File = fpm->FindFullName(node.fileNames->PhysicalName);
		if (L_File->myStatus&FileHandle::Exist && Platform_fopen(L_File.get(), "rb"))
		{
			Platform_fseek(L_File.get(), 0, SEEK_END);
			u64 filesize= Platform_ftell(L_File.get());
			Platform_fseek(L_File.get(), 0, SEEK_SET);
			Platform_fclose(L_File.get());
			CurrentEntry->myFileSize = filesize;
			CurrentEntry->myFileOffset = currentOffset;

			// round file size on 4 bytes

			u64 padFileSize = (4-(filesize & 3)) & 3;

			currentOffset += filesize + padFileSize;
		}

	}


	unsigned char* currentCharEntry = (unsigned char*)CurrentEntry;
	currentCharEntry += sizeof(FATEntry);

	// copy name
	memcpy(currentCharEntry, node.name.c_str(), node.name.length());
	memset(currentCharEntry+ node.name.length(), 0, NodeNameP);

	currentCharEntry += NodeNameL;

	CurrentEntry = (FATEntry*)currentCharEntry;

	for (auto nodeson : node.sons)
	{
		FillFATExportedStruct(nodeson, CurrentEntry, currentOffset);
	}

}

unsigned int CorePackage::PackageCreationStruct::computeFATSize(const FileTreeNode& node)
{
	unsigned int result = 0;

	// add 1 for 0 ended string, then align to 8 bytes
	result += (node.name.length()+1+7)&0xFFFFFFF8;
	result += sizeof(FATEntry);

	for (auto nodeson : node.sons)
	{
		result += computeFATSize(nodeson);
	}

	return result;
}

CorePackage::PackageCreationStruct::FileTreeNode	CorePackage::PackageCreationStruct::getFileTree()
{
	FileTreeNode	root;
	root.fileNames = 0;
	root.name = ""; 

	kstl::vector<fileNames>::iterator	it;
	for (it = myFileList.begin(); it != myFileList.end(); ++it)
	{
		kstl::string	fileInPackage = (*it).PackageName;
		kstl::string    shortfilename = "";
		kstl::vector<kstl::string> path=RetreivePath(fileInPackage, shortfilename);

		FileTreeNode* currentFileTreeNode = &root;

		for (auto itFolder : path)
		{
			bool foundFolder = false;
			// check if folder is already a son of currentFileTreeNode
			for (auto& SonFolder : currentFileTreeNode->sons)
			{
				if ((SonFolder.fileNames == 0) && (SonFolder.name == itFolder))
				{
					currentFileTreeNode = &SonFolder;
					foundFolder = true;
					break;
				}
			}
			
			if (!foundFolder)
			{
				FileTreeNode	toAdd;
				toAdd.fileNames = 0;
				toAdd.name = itFolder;
				currentFileTreeNode->sons.push_back(toAdd);
				currentFileTreeNode = &currentFileTreeNode->sons.back();
			}
		}

		FileTreeNode	toAdd;
		toAdd.fileNames = &(*it);
		toAdd.name = shortfilename;
		currentFileTreeNode->sons.push_back(toAdd);

	}

	return root;
}

#endif

bool		CorePackageFileAccess::Platform_fopen(FileHandle* handle, const char * mode)
{
	unsigned int flags=FileHandle::OpeningFlags(mode);
	if (flags&FileHandle::Write)
	{
		return false;
	}

	handle->setOpeningFlags(flags);

	myCurrentReadPos = 0;

	myFileEntry = myPackage->find(handle->myFullFileName,true);

	auto& th = myPackage->GetCurrentThreadRead();
	if (!th.myFile)
	{
		th.myFile = myPackage->myMainFile->MakeCopy();
		::Platform_fopen(th.myFile.get(), "rb");
	}

	if(myFileEntry)
		return true;

	return false;
}

long int	CorePackageFileAccess::Platform_fread(void * ptr, long size, long count, FileHandle* handle)
{
	if (myFileEntry)
	{
		long available = myFileEntry->myFileSize - myCurrentReadPos;
		long wanted = size*count;
		if (wanted > available)
		{
			wanted = available;
		}

		auto& th = myPackage->GetCurrentThreadRead();

		::Platform_fseek(th.myFile.get(), myCurrentReadPos + myPackage->myDataStartOffset + myFileEntry->myFileOffset, SEEK_SET);
		long read  =::Platform_fread(ptr, size, wanted/size, th.myFile.get());
		myCurrentReadPos += read * size;
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

	if (myFileEntry)
	{
		return myCurrentReadPos;
	}

	return -1L;
}

int			CorePackageFileAccess::Platform_fseek(FileHandle* handle, long int offset, int origin)
{

	if (myFileEntry)
	{
		long int newpos = myCurrentReadPos;
		switch (origin)
		{
		case SEEK_SET:
			newpos = offset;
			break;
		case SEEK_CUR:
			newpos += offset;
			break;
		case SEEK_END:
			newpos = myFileEntry->myFileSize - offset;
			break;
		}


		if (newpos < 0 )
		{
			newpos = 0;
		}
		if (newpos > myFileEntry->myFileSize)
		{
			newpos = myFileEntry->myFileSize;
		}

		myCurrentReadPos = newpos;

		return 0;
	}


	return -1;
}

int			CorePackageFileAccess::Platform_fflush(FileHandle* handle)
{
	if (myFileEntry)
	{
		auto& th = myPackage->GetCurrentThreadRead();
		return ::Platform_fflush(th.myFile.get());
	}
	return -1;
}

int			CorePackageFileAccess::Platform_fclose(FileHandle* handle)
{
	if (myFileEntry)
	{
		handle->resetStatus();
	}
	return 0;
}

PureVirtualFileAccessDelegate* CorePackageFileAccess::MakeCopy()
{
	auto result = new CorePackageFileAccess();
	result->myPackage = myPackage;
	result->myFileEntry = myFileEntry;
	result->myCurrentReadPos = 0;
	return result;
}