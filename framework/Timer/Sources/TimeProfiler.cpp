#include "PrecompiledHeaders.h"
#include "TimeProfiler.h"
#include "Timer.h"
#include "Core.h"

//FILE*	myDump=0;

// ****************************************
// * TimeProfiler class
// * --------------------------------------
/**
 * \file	TimeProfiler.cpp
 * \ingroup Module
 * \brief	managing timer profiling (source)
 * \author	ukn
 * \version ukn
 * \date	ukn
 */
// ****************************************
IMPLEMENT_CLASS_INFO(TimeProfiler)

TimeProfiler::TimeProfiler(const kstl::string& name,CLASS_NAME_TREE_ARG) : CoreModifiable(name,PASS_CLASS_NAME_TREE_ARG)
{  
	if(!KigsCore::GetProfileManager()->myIsInit)
	{
		KigsCore::GetProfileManager()->InitAll();
	}
	myStartTime=KDOUBLE_CONST(-1.0);
	myLastDT=myMaxDT=KDOUBLE_CONST(-1.0);
	myMinDT=KDOUBLE_CONST(10000.0);
	myComputedTime=KDOUBLE_CONST(0.0);
}     



TimeProfiler::~TimeProfiler()
{
}    



void TimeProfiler::Start()
{
/*
	if (!this)
		return;
	if( myStartTime != KDOUBLE_CONST(-1.0))
	{
		printf("Timer %s already started\n",getName().c_str());
		return;
	}
	myStartTime=KigsCore::GetProfileManager()->myGlobalTimer->GetTime();
	*/
}

void TimeProfiler::Stop()
{
/*
	if (!this)
		return;
	if(myStartTime == KDOUBLE_CONST(-1.0))
	{
		printf("Timer %s already stopped\n",getName().c_str());
		return;
	}

	myLastDT=myComputedTime;
	myComputedTime=KigsCore::GetProfileManager()->myGlobalTimer->GetTime()-myStartTime;
	if(myComputedTime<myMinDT)
	{
		myMinDT=myComputedTime;
	}
	if(myComputedTime>myMaxDT)
	{
		myMaxDT=myComputedTime;
	}
	myComputedTime+=myLastDT;
	myStartTime=KDOUBLE_CONST(-1.0);
	myWasUpdate=true;
	*/
}

int TimeProfiler::NewProfiler(const kstl::string& name)
{
	// search if profile already in free profiler slot
	int i;
	for(i=0;i<MAX_PROFILER_COUNT;i++)
	{
		if(KigsCore::GetProfileManager()->myProfilers[i])
		{		
			if(KigsCore::GetProfileManager()->myProfilers[i]->getName() == name)
			{
				return i;
			}
		}
	}

	for(i=0;i<MAX_PROFILER_COUNT;i++)
	{
		if(KigsCore::GetProfileManager()->myProfilers[i]==nullptr)
		{
			KigsCore::GetProfileManager()->myProfilers[i]=KigsCore::GetInstanceOf(name,"TimeProfiler");
			return i;
		}
	}
	return -1;
}


volatile void TimeProfiler::StartProfiler(int handler)
{
	if(handler != -1)
	{
		KigsCore::GetProfileManager()->myProfilers[handler]->Start();	
	}
}

volatile void TimeProfiler::StopProfiler(int handler)
{
	if(handler != -1)
	{
		KigsCore::GetProfileManager()->myProfilers[handler]->Stop();	
	}

}

//! delete all profilers
void TimeProfiler::CloseAll()
{
	KigsCore::GetProfileManager()->CloseAll();
}

void TimeProfiler::ShowProfilers()
{
	KigsCore::GetProfileManager()->myProfileCount++;

	// if a graphic object exist, then call it and return
	if(KigsCore::GetProfileManager()->myProfileDrawingObject)
	{
		KigsCore::GetProfileManager()->myProfileDrawingObject->CallMethod(KigsCore::GetProfileManager()->myMethodID,KigsCore::GetProfileManager()->myEmptyParams);
		return;
	}

	printf("_______________________________________________\n");
	printf("Profiler's stats : \n\n");

	kdouble max=KDOUBLE_CONST(0.0);
	kstl::string maxname;
	kstl::string minname;

	kdouble min=KDOUBLE_CONST(10000.0);
	kdouble sum=KDOUBLE_CONST(0.0);

	SP<TimeProfiler> GLOBAL(nullptr);

	int i;
	for(i=0;i<MAX_PROFILER_COUNT;i++)
	{
		SP<TimeProfiler>&	current=KigsCore::GetProfileManager()->myProfilers[i];
		if(current)
		{
			if(current->myWasUpdate)
			{
				if(	current->getName() != "GLOBAL")
				{
					kdouble currenttime=current->GetTime();

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
		SP<TimeProfiler>&	current=KigsCore::GetProfileManager()->myProfilers[i];
		if(current)
		{
			if(current->myWasUpdate)
			{
				if(	current->getName() != "GLOBAL")
				{
					printf("%s : %lf seconds , %lf percent \n",current->getName().c_str(),CastToDouble(current->GetTime()),CastToDouble(current->GetTime()*KDOUBLE_CONST(100.0)/(kdouble)sum));
				}
				current->myWasUpdate=false;
				current->myComputedTime=KDOUBLE_CONST(0.0);
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
/*	kstl::set<CoreModifiable*>	instances;
	CoreModifiable::GetInstances("TimeProfiler",instances);

	fprintf(myDump,"_______________________________________________\n");
	fprintf(myDump,"Profiler's stats N° %d: at time : %lf\n\n",myProfileCount,myGlobalTimer->GetTime());

	kdouble max=KFLOAT_CONST(0.0);
	kstl::string maxname;
	kstl::string minname;

	kdouble min=KFLOAT_CONST(10000.0);
	kdouble sum=KFLOAT_CONST(0.0);

	kstl::set<CoreModifiable*>::iterator	it;

	TimeProfiler* GLOBAL=0;

	for(it=instances.begin();it!=instances.end();++it)
	{
		TimeProfiler*	current=(TimeProfiler*)(*it);
		if(	current->getName() != "GLOBAL")
		{
			kdouble currenttime=current->GetTime();

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
			kdouble currenttime=current->GetTime();
			fprintf(myDump,"%s : %lf seconds , %lf percent \n",current->getName().c_str(),currenttime,KFLOAT_CONST(100.0)*currenttime/sum);
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
	myGlobalTimer=KigsCore::GetInstanceOf("GlobalTimer","Timer");
	myGlobalTimer->Init();
	// check is a graphic object can be created
	myProfileDrawingObject=KigsCore::GetInstanceOf("myProfileDrawingObject","ProfileDrawingObject");
	
	// if returned object is not a "real" ProfileDrawingObject, then just destroy it
	if(!myProfileDrawingObject->HasMethod(myMethodID))
	{
		myProfileDrawingObject=nullptr;
	}

	myIsInit=true;
}

void	GlobalProfilerManager::CloseAll()
{
	int i;
	for(i=0;i<MAX_PROFILER_COUNT;i++)
	{
		myProfilers[i]=nullptr;
	}
	
	myGlobalTimer = nullptr;
	myProfileDrawingObject= nullptr;
	
}
