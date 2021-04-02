#include "AMRule.h"
#include "AssetManager.h"
#include "RulesContext.h"
#include <regex>

bool AMRule::match(FileStruct& f, RulesContext& ctx)
{
	if (mIsActionOnly)
		false;

	if (mCondition.length()) // there's a condition ?
	{
		// check if regex match
		std::string cond = ctx.parse(mCondition);
		std::string fullpathname = ctx.parse("{iFullPathName}");

		std::regex re(cond);
		std::cmatch m;
		if (!std::regex_search(fullpathname.c_str(), m, re))
		{
			return false;
		}
	}
	return true;
}

bool AMRule::treat(FileStruct& f, RulesContext& ctx,bool forceAction)
{
	if (mIsActionOnly)
		false;

	std::string out = ctx.parse(mOutput);
	bool doAction = forceAction || (out.length()==0);

	if (!doAction) // need to check date ?
	{
		// check out date
		WIN32_FIND_DATAA wfd;

		HANDLE			hFind;
		hFind = ::FindFirstFileA(out.c_str(), &wfd);

		if (hFind != INVALID_HANDLE_VALUE)
		{
			if (getLargeInteger(&f.mFileInfos.ftLastWriteTime).QuadPart > getLargeInteger(&wfd.ftLastWriteTime).QuadPart)
			{
				doAction = true;
			}
		}
		else // out file does not exist ? then we have to create it
		{
			doAction = true;
		}
	}

	if (doAction)
	{
		std::string action = ctx.parse(mAction);
		system(action.c_str());
		return true;
	}

	return false;
}

bool AMRule::action(RulesContext& ctx, bool forceAction, bool somethingChanged)
{
	if (!mIsActionOnly)
		false;
	bool doAction = forceAction || (mCondition =="always");

	if (!doAction)
	{
		if ((mCondition == "changed") && somethingChanged)
		{
			doAction = true;
		}
	}

	if (doAction)
	{
		std::string action = ctx.parse(mAction);
		system(action.c_str());
		return true;
	}
	return false;
}
