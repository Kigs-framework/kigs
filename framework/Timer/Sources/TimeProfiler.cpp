#include "PrecompiledHeaders.h"
#include "TimeProfiler.h"
#include "Timer.h"
#include "Core.h"


IMPLEMENT_CLASS_INFO(TimeProfiler)

TimeProfiler::TimeProfiler(const std::string& name,CLASS_NAME_TREE_ARG) : CoreModifiable(name,PASS_CLASS_NAME_TREE_ARG)
{  
	if(!KigsCore::GetProfileManager()->mIsInit)
	{
		KigsCore::GetProfileManager()->InitAll();
	}
	mStartTime=-1.0;
	mLastDT=mMaxDT=-1.0;
	mMinDT=10000.0;
	mComputedTime=0.0;
}     



TimeProfiler::~TimeProfiler()
{
}    



void TimeProfiler::Start()
{
/*
	if (!this)
		return;
	if( mStartTime != -1.0)
	{
		printf("Timer %s already started\n",getName().c_str());
		return;
	}
	mStartTime=KigsCore::GetProfileManager()->myGlobalTimer->GetTime();
	*/
}

void TimeProfiler::Stop()
{
/*
	if (!this)
		return;
	if(mStartTime == -1.0)
	{
		printf("Timer %s already stopped\n",getName().c_str());
		return;
	}

	myLastDT=myComputedTime;
	myComputedTime=KigsCore::GetProfileManager()->myGlobalTimer->GetTime()-mStartTime;
	if(myComputedTime<myMinDT)
	{
		myMinDT=myComputedTime;
	}
	if(myComputedTime>myMaxDT)
	{
		myMaxDT=myComputedTime;
	}
	myComputedTime+=myLastDT;
	mStartTime=-1.0;
	myWasUpdate=true;
	*/
}

int TimeProfiler::NewProfiler(const std::string& name)
{
	// search if profile already in free profiler slot
	int i;
	for(i=0;i<MAX_PROFILER_COUNT;i++)
	{
		if(KigsCore::GetProfileManager()->mProfilers[i])
		{		
			if(KigsCore::GetProfileManager()->mProfilers[i]->getName() == name)
			{
				return i;
			}
		}
	}

	for(i=0;i<MAX_PROFILER_COUNT;i++)
	{
		if(KigsCore::GetProfileManager()->mProfilers[i]==nullptr)
		{
			KigsCore::GetProfileManager()->mProfilers[i] = KigsCore::GetInstanceOf(name,"TimeProfiler");
			return i;
		}
	}
	return -1;
}


void TimeProfiler::StartProfiler(int handler)
{
	if(handler != -1)
	{
		KigsCore::GetProfileManager()->mProfilers[handler]->Start();	
	}
}

void TimeProfiler::StopProfiler(int handler)
{
	if(handler != -1)
	{
		KigsCore::GetProfileManager()->mProfilers[handler]->Stop();	
	}

}

//! delete all profilers
void TimeProfiler::CloseAll()
{
	KigsCore::GetProfileManager()->CloseAll();
}

void TimeProfiler::ShowProfilers()
{
	KigsCore::GetProfileManager()->mProfileCount++;

	// if a graphic object exist, then call it and return
	if(KigsCore::GetProfileManager()->mProfileDrawingObject)
	{
		KigsCore::GetProfileManager()->mProfileDrawingObject->CallMethod(KigsCore::GetProfileManager()->mMethodID,KigsCore::GetProfileManager()->mEmptyParams);
		return;
	}

	printf("_______________________________________________\n");
	printf("Profiler's stats : \n\n");

	double max=0.0;
	std::string maxname;
	std::string minname;

	double min=10000.0;
	double sum=0.0;

	SP<TimeProfiler> GLOBAL(nullptr);

	int i;
	for(i=0;i<MAX_PROFILER_COUNT;i++)
	{
		SP<TimeProfiler>	current=KigsCore::GetProfileManager()->mProfilers[i];
		if(current)
		{
			if(current->mWasUpdate)
			{
				if(	current->getName() != "GLOBAL")
				{
					double currenttime=current->GetTime();

					sum+=currenttime;
					if(currenttime>max)
					{
						max=currenttime;
						maxname=current->getName().c_str();
					}
					if(currenttime<min)
					{
						min=currenttime;
						minname=current->getName().c_str();	
					}
				}
				else
				{
					GLOBAL=current;
				}
			}
		}
	}

	for(i=0;i<MAX_PROFILER_COUNT;i++)
	{
		SP<TimeProfiler>	current=KigsCore::GetProfileManager()->mProfilers[i];
		if(current)
		{
			if(current->mWasUpdate)
			{
				if(	current->getName() != "GLOBAL")
				{
					printf("%s : %lf seconds , %lf percent \n",current->getName().c_str(),CastToDouble(current->GetTime()),CastToDouble(current->GetTime()*100.0/(double)sum));
				}
				current->mWasUpdate=false;
				current->mComputedTime=0.0;
			}
		}
	}
	printf("_______________________________________________\n");
	if(0!=GLOBAL)
	{
		printf("Global loop profiled time : %lf \n",CastToDouble(GLOBAL->GetTime()));
	}
	printf("Total profiled time : %lf \n",CastToDouble(sum));
	printf("Min dt : %lf %s\n",CastToDouble(min),minname.c_str());
	printf("Max dt : %lf %s\n",CastToDouble(max),maxname.c_str());
	printf("_______________________________________________\n\n\n");
}


void TimeProfiler::DumpProfilers()
{
/*	std::set<CoreModifiable*>	instances;
	CoreModifiable::GetInstances("TimeProfiler",instances);

	fprintf(myDump,"_______________________________________________\n");
	fprintf(myDump,"Profiler's stats N° %d: at time : %lf\n\n",myProfileCount,myGlobalTimer->GetTime());

	double max=0.0;
	std::string maxname;
	std::string minname;

	double min=10000.0;
	double sum=0.0;

	std::set<CoreModifiable*>::iterator	it;

	TimeProfiler* GLOBAL=0;

	for(it=instances.begin();it!=instances.end();++it)
	{
		TimeProfiler*	current=(TimeProfiler*)(*it);
		if(	current->getName() != "GLOBAL")
		{
			double currenttime=current->GetTime();

			sum+=currenttime;
			if(currenttime>max)
			{
				max=currenttime;
				maxname=current->getName().c_str();
			}
			if(currenttime<min)
			{
				min=currenttime;
				minname=current->getName().c_str();	
			}
		}
		else
		{
			GLOBAL=current;
		}
	}

	for(it=instances.begin();it!=instances.end();++it)
	{
		TimeProfiler*	current=(TimeProfiler*)(*it);
		if(	current ->getName() != "GLOBAL")
		{
			double currenttime=current->GetTime();
			fprintf(myDump,"%s : %lf seconds , %lf percent \n",current->getName().c_str(),currenttime,100.0*currenttime/sum);
		}
	}
	fprintf(myDump,"_______________________________________________\n");
	if(0!=GLOBAL)
	{
		fprintf(myDump,"Global loop profiled time : %lf \n",GLOBAL->GetTime());
	}
	fprintf(myDump,"Total profiled time : %lf \n",sum);
	fprintf(myDump,"Min dt : %lf %s\n",min,minname.c_str());
	fprintf(myDump,"Max dt : %lf %s\n",max,maxname.c_str());
	fprintf(myDump,"_______________________________________________\n\n\n");
*/}

// init timer and graphic object
void	GlobalProfilerManager::InitAll()
{
	// init common timer for all profilers
	mGlobalTimer = KigsCore::GetInstanceOf("GlobalTimer","Timer");
	mGlobalTimer->Init();
	// check is a graphic object can be created
	mProfileDrawingObject=KigsCore::GetInstanceOf("myProfileDrawingObject","ProfileDrawingObject");
	
	// if returned object is not a "real" ProfileDrawingObject, then just destroy it
	if(!mProfileDrawingObject->HasMethod(mMethodID))
	{
		mProfileDrawingObject=nullptr;
	}

	mIsInit=true;
}

void	GlobalProfilerManager::CloseAll()
{
	int i;
	for(i=0;i<MAX_PROFILER_COUNT;i++)
	{
		mProfilers[i]=nullptr;
	}
	
	mGlobalTimer = nullptr;
	mProfileDrawingObject= nullptr;
	
}
