#pragma once

#include <string>
#include <map>

class FileStruct;

class RulesContext
{

	bool							mVerbose = false;
	std::string						mFolderIn = "";
	std::string						mFolderOut = "";
	std::string						mFolderInterm = "";

	std::map<std::string, std::string>	mReplacements;

	void			protectedSetBasicReplacements();
	void			removeEndingSlash(std::string& lpath);

public:

	RulesContext(const std::string& in, const std::string& interm, const std::string& out) : mFolderIn(in), mFolderInterm(interm), mFolderOut(out)
	{
		removeEndingSlash(mFolderIn);
		removeEndingSlash(mFolderInterm);
		removeEndingSlash(mFolderOut);
	}

	void			setVerbose(bool verbose)
	{
		mVerbose = verbose;
	}

	bool			isVerbose()
	{
		return mVerbose;
	}

	void			setCurrentFile(const FileStruct& file);
	void			setBasicReplacements();
	std::string		parse(const std::string& txt) const;
	
};