#include "PrecompiledHeaders.h"

#include "TravPath.h"


TravPath::TravPath()
{
	myNeedPop=0;
	myCurrentPathIndex=0;
}

        
TravPath::~TravPath() {}



TravPath::PathNeed	TravPath::ComputeNeeds(kstl::vector<int>& currentway)
{
	if(myNeedPop)
	{
		myNeedPop--;
		return pop;
	}

	if(myPath.size()-myCurrentPathIndex)
	{

		if(currentway.size() < myPath[myCurrentPathIndex].myWay.size())
		{
			return push;
		}
		
		if(currentway.size() > myPath[myCurrentPathIndex].myWay.size())
		{
			return pop;
		}

		if (currentway.size() == 0 && myPath[myCurrentPathIndex].myWay.size() == 0)
			return stay;

		if(currentway[currentway.size()-1] == myPath[myCurrentPathIndex].myWay[myPath[myCurrentPathIndex].myWay.size()-1])
		{
			return stay;
		}
	}

	return pop;
}


bool TravPath::IsNeedSameLevel(const kstl::vector<int>& currentway)
{
	if(myNeedPop)
	{
		return false;
	}

	if(myPath.size()-myCurrentPathIndex)
	{
		if(currentway.size() < myPath[myCurrentPathIndex].myWay.size())
		{
			return true;
		}
	}

	return false;
}


void TravPath::PopWay(kstl::vector<int>& currentway)
{
//	myPath.erase(myPath.begin());
	myCurrentPathIndex++;

	myNeedPop=(int)currentway.size();

	if(myPath.size()-myCurrentPathIndex)
	{
		int commun=0;
		int i;
		for(i=0;i<(int)myPath[myCurrentPathIndex].myWay.size();i++)
		{
			if(i>=(int)currentway.size())
			{
				break;
			}
			else if(myPath[myCurrentPathIndex].myWay[(unsigned int)i] != currentway[(unsigned int)i])
			{
				break;
			}
			else
			{
				commun++;
			}
		}
		myNeedPop-=commun;

	}
/*	
	if(myNeedPop == 0)
	{
		if(myPath[myCurrentPathIndex].myWay.size() > currentway.size())
		{
			return 1;
		}
	}

	return myNeedPop;*/
}

