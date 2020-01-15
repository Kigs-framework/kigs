#ifndef _TIMEPROFILER_H_
#define _TIMEPROFILER_H_

#include "CoreModifiable.h"
#include "Timer.h"

/**
 * \def MAX_PROFILER_COUNT
 * \ingroup Timer
 * \brief number max of profiler
 */
#define MAX_PROFILER_COUNT	32

// ****************************************
// * TimeProfiler class
// * --------------------------------------
/**
 * \file	TimeProfiler.h
 * \class	TimeProfiler
 * \ingroup Timer
 * \brief	timer profiling
 * \author	ukn
 * \version ukn
 * \date	ukn
 *
 * Module Dependency :<br><ul><li>ModuleTimer</li></ul>
 */
// ****************************************
class TimeProfiler : public CoreModifiable
{
public:
    DECLARE_CLASS_INFO(TimeProfiler,CoreModifiable,TimeProfiler)

	//! constructor
    TimeProfiler(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	/**
	 * \brief		create a profiler
	 * \fn			static int NewProfiler(const kstl::string& name);
	 * \param		name : name of the new profiler, NOT NULL
	 * \return		the the handler of the new profiler, -1 if the profiler can't be created
	 * 
	 * get the handler of the profiler named 'name', if not existe create it. 
	 * return -1 if there is no slot available.
	 */
	static int NewProfiler(const kstl::string& name);

	/**
	 * \brief		start a profiler for the given profile handler
	 * \fn			static volatile void StartProfiler(int handler);
	 * \param		handler : handler of the profiler to start
	 */
	static volatile void StartProfiler(int handler);

	/**
	 * \brief		stop the profiler for the given profile handler
	 * \fn			static volatile void StopProfiler(int handler);
	 * \param		handler : handler of the profiler to stop
	 */
	static volatile void StopProfiler(int handler);

	//! delete all profilers
	static void CloseAll();

	//! show the profilers stats
	static void ShowProfilers();

	//! same as show, but dump to a file (not implemented)
	static void DumpProfilers();

	/**
	 * \brief		getter to myComputedTime
	 * \fn			kdouble GetTime()
	 * \return		the computed time for this profiler
	 */
	kdouble GetTime()
	{
		return myComputedTime;
	}
	kdouble GetPreviousTime()
	{
		return myLastDT;
	}

	kdouble GetMinDT()
	{
		return myMinDT;
	}
	kdouble GetMaxDT()
	{
		return myMaxDT;
	}

	void ResetMinMaxDT()
	{
		myMinDT = 10000.0f;
		myMaxDT = -1.0f;
	}

	/**
	 * \brief		getter to myWasUpdate
	 * \fn			bool	WasUpdate()
	 * \return		the value of myWasUpdate
	 */
	bool	WasUpdate()
	{
		return myWasUpdate;
	}

	/**
	 * \brief		reset of the update time mechanisme
	 * \fn			void	ClearUpdate()
	 */
	void	ClearUpdate()
	{
		myWasUpdate=false;
		myComputedTime=KDOUBLE_CONST(0.0);
	}

protected:
	/**
	 * \brief		start this profiler
	 * \fn			void	Start();
	 */
	void	Start();

	/**
	 * \brief		stop this profiler
	 * \fn			void	Stop();
	 */
	void	Stop();

	//! start time of the profiler
	kdouble myStartTime;
	//! computed time of the profiler
	kdouble	myComputedTime;
	//! last deltaTime computed
	kdouble	myLastDT;
	//! max deltaTime seen
	kdouble	myMaxDT;
	//! min deltaTime seen
	kdouble	myMinDT;
	//! TRUE if the profile was updated
	bool	myWasUpdate=false;

	/**
	 * \brief	destructor
	 * \fn 		~TimeProfiler();
	 */
	virtual ~TimeProfiler();
};

// ****************************************
// * GlobalProfilerManager class
// * --------------------------------------
/**
 * \class	GlobalProfilerManager
 * \ingroup Manager
 * \brief	used by the Core to store and access to the list of profilers
 * \author	ukn
 * \version ukn
 * \date	ukn
 */
// ****************************************
class GlobalProfilerManager 
{
public:
	/**
	 * \brief	constructor
	 * \fn 		GlobalProfilerManager()
	 */
	GlobalProfilerManager()
	{
		myGlobalTimer=0;
		myProfileCount=0;
		int i;
		for(i=0;i<MAX_PROFILER_COUNT;i++)
		{
			myProfilers[i]=0;
		}
		myProfileDrawingObject=0;
		myIsInit=false;
		myMethodID=CharToID::GetID("DrawProfiles");
	}
	
	/**
	 * \brief	destructor
	 * \fn 		~GlobalProfilerManager();
	 */
	~GlobalProfilerManager()
	{
		// nothing to do here
	}

	/**
	 * \brief	init timer and graphic object
	 * \fn 		void			InitAll();
	 */
	void			InitAll();
	/**
	 * \brief	close all profiler
	 * \fn 		void			CloseAll();
	 */
	void			CloseAll();

	//! link to the timer
	SP<Timer>			myGlobalTimer;
	//! number of profiler
	int				myProfileCount;
	//! list of profiler
	SP<TimeProfiler>	myProfilers[MAX_PROFILER_COUNT];
	//! link to the drawing object
	CMSP				myProfileDrawingObject;
	//! TRUE if the instance is initialized
	bool			myIsInit;

	//! empty list of params, used to call graphic object method 
	kstl::vector<CoreModifiableAttribute*>		myEmptyParams;
	//! draw method ID
	unsigned int	myMethodID;
};


/**
 * \def DECLAREPROFILE(name)
 * \ingroup Timer
 * \brief if DOPROFILE defined, add a profile 'name'
 */
/**
 * \def STARTPROFILE(name)
 * \ingroup Timer
 * \brief if DOPROFILE defined, start the profile 'name'
 */
/**
 * \def ENDPROFILE(name)
 * \ingroup Timer
 * \brief if DOPROFILE defined, stop the profile 'name'
 */
/**
 * \def PRINTPROFILES
 * \ingroup Timer
 * \brief if DOPROFILE defined, print all profiles
 */
/**
 * \def CLOSEPROFILER
 * \ingroup Timer
 * \brief if DOPROFILE defined, close all profiles
 */

// uncomment and rebuild to use profiler
//#define DOPROFILE

#ifdef DOPROFILE

#define DECLAREPROFILE(name) static int name##_handler=TimeProfiler::NewProfiler(#name);
#define STARTPROFILE(name)	TimeProfiler::StartProfiler(name##_handler);
#define ENDPROFILE(name)	TimeProfiler::StopProfiler(name##_handler);

#define PRINTPROFILES		TimeProfiler::ShowProfilers();

#define CLOSEPROFILER		TimeProfiler::CloseAll();

#else

#define DECLAREPROFILE(name) 
#define STARTPROFILE(name)
#define ENDPROFILE(name)

#define PRINTPROFILES

#define CLOSEPROFILER		

#endif

#endif
