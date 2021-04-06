
#include "FilePathManager.h"
#include "ModuleThread.h"
#include "ModuleFileManager.h"
#include "Thread.h"
#include "JSonFileParser.h"
#include "AMRule.h"
#include "AssetManager.h"
#include "RulesContext.h"
#include <filesystem>


IMPLEMENT_CLASS_INFO(AssetManager);

IMPLEMENT_CONSTRUCTOR(AssetManager)
{

}

void	AssetManager::ProtectedInit()
{
	// Base modules have been created at this point
	// lets say that the update will sleep 1ms
	SetUpdateSleepTime(1);

	CoreCreateModule(ModuleThread, nullptr);

	// retreive args
	kstl::vector<kstl::string>::iterator itArgs = mArgs.begin();
	// skip app name
	itArgs++;

	int totalArgsCount = mArgs.size() - 1;

	if (totalArgsCount <6)
	{
		usage();
		exit(-1);
	}

	for (; itArgs != mArgs.end(); itArgs++)
	{
		kstl::string& current = (*itArgs);

		if (current.at(0) == '-')
		{
			char argtype = current.at(1);
			switch (argtype)
			{
			case 'i':
			{
				// in already there
				if (mFolderIn != "")
				{
					usage();
					exit(-1);
				}
				itArgs++;
				mFolderIn = (*itArgs);
			}
			break;
			case 'o':
			{
				// out already there
				if (mFolderOut != "")
				{
					usage();
					exit(-1);
				}
				itArgs++;
				mFolderOut = (*itArgs);
			}
			break;
			case 't':
			{
				// in already there
				if (mFolderInterm != "")
				{
					usage();
					exit(-1);
				}
				itArgs++;
				mFolderInterm = (*itArgs);
			}
			break;
			case 'p':
			{
				// in already there
				if (mPlatform != "")
				{
					usage();
					exit(-1);
				}
				itArgs++;
				mPlatform = (*itArgs);
			}
			break;
			default:
				break;
			}
		}
	}

	if ((mFolderIn == "") || (mFolderOut == "") || (mFolderInterm == ""))
	{
		usage();
		exit(-1);
	}
		// check if rules are presents

	auto& pathManager = KigsCore::Singleton<FilePathManager>();

	std::string fullRulesPath = mFolderIn + "\\AssetManagerRules" + mPlatform + ".json";

	auto filenamehandle = pathManager->FindFullName(fullRulesPath);

	if (!(filenamehandle->mStatus & FileHandle::Exist))
	{
		printf("Warning %s was not found, try to use AssetManagerRules.json\n", fullRulesPath.c_str());
		mPlatform = "";
		fullRulesPath = mFolderIn + "\\AssetManagerRules.json";
		filenamehandle = pathManager->FindFullName(fullRulesPath);
		if (!(filenamehandle->mStatus & FileHandle::Exist))
		{
			usage();
			exit(-1);
		}
	}

	JSonFileParser L_JsonParser;
	CoreItemSP initP = L_JsonParser.Get_JsonDictionary(filenamehandle);
	if (!initP.isNil())
	{
		if (initRules(initP))
		{
			SP<Thread> workthread = KigsCore::GetInstanceOf("workthread", "Thread");
			mThread = workthread;
			workthread->setMethod(this, "doTheJob");
			workthread->Init();
		}
	}
}

void	AssetManager::ProtectedUpdate()
{
	//std::cout << "Processed file count : " << mBundle.size() << std::endl;
	if (mJobIsDone)
	{

		auto& pathManager = KigsCore::Singleton<FilePathManager>();
		auto filenamehandle = pathManager->FindFullName(mFolderIn + "\\AMLastDone.txt");

		Platform_fopen(filenamehandle.get(), "wt");
		Platform_fwrite("done", 1, 5, filenamehandle.get());
		Platform_fclose(filenamehandle.get());

		mNeedExit = true;
		mThread = nullptr;
	}
}

void	AssetManager::ProtectedClose()
{
	CoreDestroyModule(ModuleThread);
}


void	AssetManager::usage()
{
	printf("Usage : \n");
	printf("AssetManager -i inputFolder -o outputPackage -t intermFolder [-p platform]\n\n");
	printf("-i inputFolder : path of the folder where to find raw assets\n");
	printf("-o outputFolder : path of ouput folder where to set transformed assets\n");
	printf("-t intermFolder : path of an intermediary folder\n");
	printf("-p platform : platform appended to AssetManagerRules filename \n");

	printf("inputFolder should contains raw assets and AssetManagerRules[platform].json\n");
	printf("check associated documentation for AssetManagerRules[platform].json format\n");
}

bool AssetManager::initRules(CoreItemSP rules)
{

	for (auto r : rules)
	{
		CoreItemSP isRule = r["rule"];
		if(!isRule.isNil() && (isRule->size() == 3))
		{
			mRules.push_back({ (std::string)isRule[0],(std::string)isRule[1], (std::string)isRule[2] });
		}
		else
		{
			CoreItemSP isAction = r["action"];
			if (!isAction.isNil() && (isAction->size() == 2))
			{
				mRules.push_back({ (std::string)isAction[0],"",(std::string)isAction[1]});
				mRules.back().setActionOnly(true);
			}
			else
			{
				CoreItemSP isDirRule = r["dir_rule"];
				if (!isDirRule.isNil() && (isDirRule->size() == 3))
				{
					mRules.push_back({ (std::string)isDirRule[0],(std::string)isDirRule[1], (std::string)isDirRule[2] });
					mRules.back().setDirRule(true);
				}
			}
		}
	}

	return mRules.size();
}

void	AssetManager::doTheJob()
{
	std::vector<std::string> dirlist;
	recursiveSearchFiles(mFolderIn,dirlist);
	// if we are here, AssetManagerRules.json exist

	std::string Rulesfile = "AssetManagerRules" + mPlatform + ".json";

	FileStruct* fs=getFileStructFromName(Rulesfile);
	FileStruct* lastDone = getFileStructFromName("AMLastDone.txt");

	// if rules have changed, redo everything

	if ((!lastDone) || (getLargeInteger(&lastDone->mFileInfos.ftLastWriteTime).QuadPart < getLargeInteger(&fs->mFileInfos.ftLastWriteTime).QuadPart))
	{
		mResetAll = true;
		std::filesystem::remove_all(mFolderInterm); // Delete directory
		std::filesystem::create_directory(mFolderInterm); // and recreate intermediary
	}

	removeFileStructFromlistUsingPattern("AssetManagerRules");
	removeFileStructFromlist("AMLastDone.txt");

	runRules();

	mJobIsDone = true;
}

void	AssetManager::recursiveSearchFiles(std::string	startDirectory,std::vector<std::string>& currentDirectoryList)
{
	HANDLE			hFind;

	// Iterate through dirs

	std::vector<std::string>	subDirectoryList;
	subDirectoryList.clear();

	WIN32_FIND_DATAA wfd;

	std::string search = startDirectory + "\\*";

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

					FileStruct toAdd;
					toAdd.mFolders = currentDirectoryList;
					toAdd.mFileInfos = wfd;
					mFileList.push_back(toAdd);

					currentDirectoryList.push_back(asciifilename);
					recursiveSearchFiles(startDirectory + "\\" + asciifilename + "\\", currentDirectoryList);
					currentDirectoryList.pop_back();
				}
			}
			else
			{
				FileStruct toAdd;
				toAdd.mFolders = currentDirectoryList;
				toAdd.mFileInfos = wfd;
				mFileList.push_back(toAdd);
			}

		} while (::FindNextFileA(hFind, &wfd));

		::FindClose(hFind);
	}

}

FileStruct* AssetManager::getFileStructFromName(const std::string& name)
{
	for (auto& fs : mFileList)
	{
		if (fs.mFileInfos.cFileName == name)
		{
			return &fs;
		}
	}
	return nullptr;
}

void	AssetManager::removeFileStructFromlist(const std::string& name)
{
	std::vector<FileStruct>::iterator fs;
	for (fs= mFileList.begin();fs!=mFileList.end();++fs)
	{
		if ((*fs).mFileInfos.cFileName == name)
		{
			mFileList.erase(fs);
			break;
		}
	}
	
}

void	AssetManager::removeFileStructFromlistUsingPattern(const std::string& pattern)
{
	std::vector<FileStruct>::iterator fs = mFileList.begin();
	while(fs != mFileList.end())
	{
		std::string name = (*fs).mFileInfos.cFileName;
		if (name.find(pattern) != std::string::npos)
		{
			fs=mFileList.erase(fs);
		}
		else
		{
			++fs;
		}
	}
}


void	AssetManager::runRules()
{
	RulesContext	context(mFolderIn, mFolderInterm, mFolderOut);
	// rules are sorted by priority
	bool	SomethingChanged = false;

	for (auto r : mRules)
	{
		if (r.isActionOnly()) // action execute without file
		{
			context.setBasicReplacements();
			SomethingChanged |=r.action(context, mResetAll, SomethingChanged);
		}
		else // this is a rule
		{
			for (auto& f : mFileList) // check all files
			{
				if (r.isDirRule() ^ ((f.mFileInfos.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)!=0))
				{
					continue;
				}
				
				if (!f.mWasMatched) // this file was not already catched by a rule
				{
					context.setCurrentFile(f);
					f.mWasMatched = r.match(f, context); // check if it matches
					if (f.mWasMatched)
					{
						// then try to treat it
						f.mWasTreated = r.treat(f, context, mResetAll);
						SomethingChanged |= f.mWasTreated;

						if (r.isDirRule()) // set all files in dir as matched
						{
							matchAllDir(f);
						}
					}
				}
			}
		}
	}
}

void		AssetManager::matchAllDir(FileStruct& d)
{
	for (auto& f : mFileList) // mark all files under d directory as matched
	{
		if (!f.mWasMatched)
			f.mWasMatched = f.isInDirectory(d);
	}
}

bool		FileStruct::isInDirectory(const FileStruct& d)
{
	size_t s= mFolders.size();
	if (s <= d.mFolders.size())
	{
		return false;
	}
	for (size_t i=0;i< d.mFolders.size();i++)
	{
		if (d.mFolders[i] != mFolders[i])
		{
			return false;
		}
	}

	if (mFolders[d.mFolders.size()] != d.mFileInfos.cFileName)
	{
		return false;
	}
	return true;
}