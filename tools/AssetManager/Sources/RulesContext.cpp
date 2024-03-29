#include "RulesContext.h"
#include "AssetManager.h"
#include "Core.h"
#include <algorithm>

void	RetreiveShortNameAndExt(const std::string& filename, std::string& shortname, std::string& fileext)
{
	int pos = static_cast<int>(filename.rfind("/")) + 1;
	int pos1 = static_cast<int>(filename.rfind("\\")) + 1;

	if (pos1 > pos)
	{
		pos = pos1;
	}

	bool	filenameIsShortfilename = false;

	if (pos == 0)
	{
		shortname = filename;
		filenameIsShortfilename = true;
	}
	else
	{
		shortname = filename.substr(static_cast<unsigned int>(pos), filename.length() - pos);
	}

	fileext = "";
	pos = static_cast<int>(shortname.rfind(".")) + 1;
	if (pos)
	{
		fileext.append(shortname, static_cast<unsigned int>(pos), shortname.length() - pos);
		shortname = shortname.substr(0, pos - 1);
	}

}

void			RulesContext::removeEndingSlash(std::string& lpath)
{
	while (lpath.length() && ( (lpath[lpath.length() - 1] == '\\') || (lpath[lpath.length() - 1] == '/') ))
	{
		lpath = lpath.substr(0, lpath.length() - 1);
	}
}

void			RulesContext::protectedSetBasicReplacements()
{
	mReplacements.clear();
	mReplacements["{iFolder}"] = mFolderIn;
	mReplacements["{tFolder}"] = mFolderInterm;
	mReplacements["{oFolder}"] = mFolderOut;
}

void			RulesContext::setBasicReplacements()
{
	protectedSetBasicReplacements();
	for (auto& pathes : mReplacements)
	{
		Kigs::Core::replaceAll(pathes.second, "/", "\\");
	}
}

void			RulesContext::setCurrentFile(const FileStruct& file)
{
	protectedSetBasicReplacements();
	// compute available replacements
	std::string shortname;
	std::string ext;

	RetreiveShortNameAndExt(file.mFileInfos.cFileName,shortname,ext);
	mReplacements["{iExt}"] = ext;
	mReplacements["{iShortFileName}"] = shortname;
	mReplacements["{iFileName}"] = file.mFileInfos.cFileName;

	std::string insidepath = "";
	for (const auto& p : file.mFolders)
	{
		insidepath += "\\" + p ;
	}

	mReplacements["{iPath}"] = mFolderIn + insidepath;
	mReplacements["{tPath}"] = mFolderInterm + insidepath;
	mReplacements["{oPath}"] = mFolderOut + insidepath;

	mReplacements["{iFullPathName}"] = mFolderIn + insidepath + "\\" + file.mFileInfos.cFileName;

	for (auto& pathes : mReplacements)
	{
		Kigs::Core::replaceAll(pathes.second, "/", "\\");
	}
}
std::string		RulesContext::parse(const std::string& txt) const
{
	std::string result = txt;
	for (const auto& r : mReplacements)
	{
		Kigs::Core::replaceAll(result, r.first, r.second);
	}
	return result;
}
