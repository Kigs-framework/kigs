
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
			case 'v':
			{
				mVerbose = true;
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

	auto pathManager = KigsCore::Singleton<FilePathManager>();

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
		JSonFileParser json;
		json.Export(static_cast<CoreMap<std::string>* >(mFileHierarchy.get()),mFolderIn + "\\AMLastDone.json");

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
	printf("AssetManager -i inputFolder -o outputPackage -t intermFolder [-p platform] [-v] \n\n");
	printf("-i inputFolder : path of the folder where to find raw assets\n");
	printf("-o outputFolder : path of ouput folder where to set transformed assets\n");
	printf("-t intermFolder : path of an intermediary folder\n");
	printf("-p platform : platform appended to AssetManagerRules filename \n");
	printf("-v : verbose mode \n");

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

CoreItemSP		AssetManager::createJSONFromFileList()
{
	CoreItemSP root = CoreItemSP::getCoreMap();
	std::vector<CoreItemSP> folderlist;
	folderlist.push_back(root);

	std::vector<std::string> localfolderlist;

	for (auto& fs : mFileList)
	{
		if (localfolderlist != fs.mFolders)
		{
			if (localfolderlist.size() >= fs.mFolders.size())
			{
				localfolderlist.pop_back();
				folderlist.pop_back();
			}
		}
		CoreItemSP newfile = CoreItemSP::getCoreMap();
		newfile->set("UID", (u64)fs.mUID);
		folderlist.back()->set(fs.mFileInfos.cFileName, newfile);
		if (fs.mFileInfos.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			folderlist.push_back(newfile);
			localfolderlist.push_back(fs.mFileInfos.cFileName);
		}
	}

	return root;
}


void	AssetManager::doTheJob()
{
	std::vector<std::string> dirlist;
	recursiveSearchFiles(mFolderIn,dirlist);
	mFileHierarchy=createJSONFromFileList();

	// if we are here, AssetManagerRules.json exist

	std::string Rulesfile = "AssetManagerRules" + mPlatform + ".json";

	FileStruct* fs=getFileStructFromName(Rulesfile);

	JSonFileParser L_JsonParser;
	CoreItemSP initP = L_JsonParser.Get_JsonDictionary(mFolderIn + "\\AMLastDone.json");

	// if rules have changed, redo everything
	if (initP.isNil() || (((u64)initP[Rulesfile]["UID"]) != fs->mUID))
	{
		mResetAll = true;
		std::filesystem::remove_all(mFolderInterm); // Delete directory
		std::filesystem::create_directory(mFolderInterm); // and recreate intermediary
	}

	removeFileStructFromlistUsingPattern("AssetManagerRules");
	removeFileStructFromlist("AMLastDone.json");

	runRules(initP);

	mJobIsDone = true;
}

std::pair<u64, u64>	AssetManager::recursiveSearchFiles(std::string	startDirectory,std::vector<std::string>& currentDirectoryList)
{
	HANDLE			hFind;

	// Iterate through dirs

	std::vector<std::string>	subDirectoryList;
	subDirectoryList.clear();

	WIN32_FIND_DATAA wfd;

	std::string search = startDirectory + "\\*";

	hFind = ::FindFirstFileA(search.c_str(), &wfd);

	u64 returnedmaxtime = 0;
	u64 returnedUID = 0;

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
					std::pair<u64,u64> returnedPair=recursiveSearchFiles(startDirectory + "\\" + asciifilename + "\\", currentDirectoryList);
					currentDirectoryList.pop_back();

					if (returnedmaxtime < returnedPair.first)
					{
						returnedmaxtime = returnedPair.first;
					}
					FileStruct* currentFS = getCurrentFileStructure(currentDirectoryList, wfd.cFileName);
					currentFS->mUID = returnedPair.second;
					currentFS->mFileTouchTime = returnedmaxtime;
					returnedUID ^= currentFS->mUID;
				}
			}
			else
			{
				FileStruct toAdd;
				toAdd.mFolders = currentDirectoryList;
				toAdd.mFileInfos = wfd;
				
				toAdd.mFileTouchTime = getLargeInteger(&wfd.ftLastWriteTime).QuadPart;
				if (returnedmaxtime < toAdd.mFileTouchTime)
				{
					returnedmaxtime = toAdd.mFileTouchTime;
				}
				toAdd.mUID = toAdd.mFileTouchTime ^ toAdd.mFileInfos.nFileSizeLow ^ toAdd.mFileInfos.nFileSizeHigh;
				mFileList.push_back(toAdd);
				returnedUID ^= toAdd.mUID;
			}

		} while (::FindNextFileA(hFind, &wfd));

		::FindClose(hFind);
	}
	return { returnedmaxtime,returnedUID };
}

FileStruct* AssetManager::getCurrentFileStructure(const std::vector<std::string>& currentDirectoryList, const std::string& name)
{
	for (auto& fs : mFileList)
	{
		if (fs.mFileInfos.cFileName == name)
		{
			if (fs.mFolders == currentDirectoryList)
			{
				return &fs;
			}
		}
	}
	return nullptr;
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

CoreItemSP	AssetManager::getCorrespondingItem(CoreItemSP fileInfos, FileStruct& str)
{
	CoreItemSP current = fileInfos;
	for (auto f : str.mFolders)
	{
		current = current[f];
		if (current.isNil())
			return current;
	}

	current = current[(std::string)str.mFileInfos.cFileName];
	return current;
}


void	AssetManager::runRules(CoreItemSP fileinfos)
{
	RulesContext	context(mFolderIn, mFolderInterm, mFolderOut);
	context.setVerbose(mVerbose);
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
						u64 prevUID = 0;
						u64 currentUID = 0;
						if (!mResetAll)
						{
							CoreItemSP checkPreviousState = getCorrespondingItem(fileinfos, f);
							if (!checkPreviousState.isNil())
							{
								prevUID = checkPreviousState["UID"];
							}
							CoreItemSP checkCurrentState = getCorrespondingItem(mFileHierarchy, f);
							currentUID = checkCurrentState["UID"];
						}

						if ((prevUID != currentUID) || (mResetAll))
						{
							// then try to treat it
 							f.mWasTreated = r.treat(f, context, mResetAll || (prevUID != currentUID));
							SomethingChanged |= f.mWasTreated;
						}

						if (r.isDirRule()) // set all files in dir as matched
						{
							matchAllDir(f);
						}
					}
				}
			}
		}
	}

	if ((!SomethingChanged) && mVerbose)
	{
		printf("AssetManager : nothing changed\n");
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