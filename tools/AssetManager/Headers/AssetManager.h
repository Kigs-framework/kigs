#pragma once

#include "CoreBaseApplication.h"
#include "CoreItem.h"
#include "AMRule.h"

#include <windows.h>

class FileStruct
{
public:
	std::vector<std::string>	mFolders;
	bool						mWasTreated = false;
	bool						mWasMatched = false;
	WIN32_FIND_DATAA			mFileInfos;
	u64							mUID = 0;
	u64							mFileTouchTime = 0;
	bool						isInDirectory(const FileStruct&);
};

inline ULARGE_INTEGER	getLargeInteger(FILETIME* t)
{
	ULARGE_INTEGER	result;
	result.LowPart = t->dwLowDateTime;
	result.HighPart = t->dwHighDateTime;
	return result;
}

namespace Kigs
{
	using namespace Kigs::Core;

	class AssetManager : public CoreBaseApplication
	{
	public:
		DECLARE_CLASS_INFO(AssetManager, CoreBaseApplication, Core);
		DECLARE_CONSTRUCTOR(AssetManager);

	protected:

		void	ProtectedInit() override;
		void	ProtectedUpdate() override;
		void	ProtectedClose() override;

		void	doTheJob();

		void	usage();

		bool	initRules(CoreItemSP rules);
		void	runRules(CoreItemSP fileInfos);

		CoreItemSP	getCorrespondingItem(CoreItemSP fileInfos, FileStruct& str);

		// return date and some UID
		std::pair<u64, u64>	recursiveSearchFiles(std::string	startDirectory, std::vector<std::string>& currentDirectoryList);

		FileStruct* getFileStructFromName(const std::string& name);
		FileStruct* getCurrentFileStructure(const std::vector<std::string>& currentDirectoryList, const std::string& name);

		void			removeFileStructFromlist(const std::string& name);
		void			removeFileStructFromlistUsingPattern(const std::string& pattern);
		void			matchAllDir(FileStruct&);

		CoreItemSP		createJSONFromFileList();

		CMSP							mThread;
		std::string						mFolderIn = "";
		std::string						mFolderOut = "";
		std::string						mFolderInterm = "";
		std::string						mPlatform = "";
		std::vector<FileStruct>			mFileList;
		bool							mJobIsDone = false;
		bool							mResetAll = false;
		bool							mVerbose = false;

		std::vector<AMRule>				mRules;

		CoreItemSP						mFileHierarchy;

		WRAP_METHODS(doTheJob);
	};
}
