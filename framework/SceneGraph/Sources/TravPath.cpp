#include "PrecompiledHeaders.h"

#include "TravPath.h"


TravPath::TravPath()
{
	mNeedPop=0;
	mCurrentPathIndex=0;
}

        
TravPath::~TravPath() {}



TravPath::PathNeed	TravPath::ComputeNeeds(kstl::vector<int>& currentway)
{
	if(mNeedPop)
	{
		mNeedPop--;
		return pop;
	}

	if(mPath.size()-mCurrentPathIndex)
	{

		if(currentway.size() < mPath[mCurrentPathIndex].mWay.size())
		{
			return push;
		}
		
		if(currentway.size() > mPath[mCurrentPathIndex].mWay.size())
		{
			return pop;
		}

		if (currentway.size() == 0 && mPath[mCurrentPathIndex].mWay.size() == 0)
			return stay;

		if(currentway[currentway.size()-1] == mPath[mCurrentPathIndex].mWay[mPath[mCurrentPathIndex].mWay.size()-1])
		{
			return stay;
		}
	}

	return pop;
}


bool TravPath::IsNeedSameLevel(const kstl::vector<int>& currentway)
{
	if(mNeedPop)
	{
		return false;
	}

	if(mPath.size()-mCurrentPathIndex)
	{
		if(currentway.size() < mPath[mCurrentPathIndex].mWay.size())
		{
			return true;
		}
	}

	return false;
}


void TravPath::PopWay(kstl::vector<int>& currentway)
{
//	mPath.erase(mPath.begin());
	mCurrentPathIndex++;

	mNeedPop=(int)currentway.size();

	if(mPath.size()-mCurrentPathIndex)
	{
		int commun=0;
		int i;
		for(i=0;i<(int)mPath[mCurrentPathIndex].mWay.size();i++)
		{
			if(i>=(int)currentway.size())
			{
				break;
			}
			else if(mPath[mCurrentPathIndex].mWay[(unsigned int)i] != currentway[(unsigned int)i])
			{
				break;
			}
			else
			{
				commun++;
			}
		}
		mNeedPop-=commun;

	}
/*	
	if(mNeedPop == 0)
	{
		if(mPath[mCurrentPathIndex].mWay.size() > currentway.size())
		{
			return 1;
		}
	}

	return mNeedPop;*/
}

