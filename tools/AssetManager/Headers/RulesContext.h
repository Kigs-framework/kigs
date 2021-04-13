#pragma once

#include <string>
#include <map>

class FileStruct;

class RulesContext
{

	std::string						mFolderIn = "";
	std::string						mFolderOut = "";
	std::string						mFolderInterm = "";

	std::map<std::string, std::string>	mReplacements;

public:

	RulesContext(const std::string& in, const std::string& interm, const std::string& out) : mFolderIn(in), mFolderInterm(interm), mFolderOut(out)
	{

	}

	void			setCurrentFile(const FileStruct& file);
	void			setBasicReplacements();
	std::string		parse(const std::string& txt) const;

};