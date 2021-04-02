#pragma once

#include <string>
class RulesContext;
class FileStruct;

// base class
class AMRule
{
	// regex to know if the rule can be activated
	std::string	mCondition;

	// output file to check if the date is older than input 
	std::string mOutput;
	// actions
	std::string	mAction;

	// set to true when no file is needed
	bool	mIsActionOnly = false;

public:

	AMRule(const std::string& cond, const std::string& ofile,const std::string& act) : mCondition(cond), mOutput(ofile),mAction(act)
	{

	}

	void	setActionOnly(bool isaction)
	{
		mIsActionOnly = true;
	}

	bool	isActionOnly()
	{
		return mIsActionOnly;
	}

	bool match(FileStruct&, RulesContext&);

	bool treat(FileStruct&, RulesContext& , bool forceAction );

	bool action(RulesContext&, bool forceAction,bool somethingChanged);

};