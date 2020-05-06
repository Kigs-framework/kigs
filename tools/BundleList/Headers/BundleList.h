#pragma once

#include <string>
#include <vector>
#include <map>

#include <CoreBaseApplication.h>

class BundleList : public CoreBaseApplication
{
public:
	DECLARE_CLASS_INFO(BundleList, CoreBaseApplication, Core);
	DECLARE_CONSTRUCTOR(BundleList);

protected:
	void	ProtectedInit() override;
	void	ProtectedUpdate() override;
	void	ProtectedClose() override;

	void	usage();

	std::map<std::string, std::vector<std::string> >	mBundle;
	std::string											mStartPath = "./";
	std::string											mFilename = "files.bundle";

	bool												mBundleIsDone = false;

	void	CreateBundle();
	void	RecursiveInitFileList(std::string	startDirectory, int cropFilePath);
	void	WriteBundleList();

	CMSP	mThread;

	WRAP_METHODS(CreateBundle);
};
