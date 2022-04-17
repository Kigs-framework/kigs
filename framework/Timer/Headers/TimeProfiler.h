#ifndef _TIMEPROFILER_H_
#define _TIMEPROFILER_H_

#include "CoreModifiable.h"
#include "Timer.h"


#define MAX_PROFILER_COUNT	32

// ****************************************
// * TimeProfiler class
// * --------------------------------------
/**
 * \class	TimeProfiler
 * \file	TimeProfiler.h
 * \ingroup TimerModule
 * \brief	Utility class to mesure time spent in parts of the code.
 * 
 *  ?? Obsolete ??
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
	 * \fn			static void StartProfiler(int handler);
	 * \param		handler : handler of the profiler to start
	 */
	static void StartProfiler(int handler);

	/**
	 * \brief		stop the profiler for the given profile handler
	 * \fn			static void StopProfiler(int handler);
	 * \param		handler : handler of the profiler to stop
	 */
	static void StopProfiler(int handler);

	//! delete all profilers
	static void CloseAll();

	//! show the profilers stats
	static void ShowProfilers();

	//! same as show, but dump to a file (not implemented)
	static void DumpProfilers();

	/**
	 * \brief		getter to mComputedTime
	 * \fn			kdouble GetTime()
	 * \return		the computed time for this profiler
	 */
	kdouble GetTime()
	{
		return mComputedTime;
	}
	kdouble GetPreviousTime()
	{
		return mLastDT;
	}

	kdouble GetMinDT()
	{
		return mMinDT;
	}
	kdouble GetMaxDT()
	{
		return mMaxDT;
	}

	void ResetMinMaxDT()
	{
		mMinDT = 10000.0f;
		mMaxDT = -1.0f;
	}

	/**
	 * \brief		getter to mWasUpdate
	 * \fn			bool	WasUpdate()
	 * \return		the value of mWasUpdate
	 */
	bool	WasUpdate()
	{
		return mWasUpdate;
	}

	/**
	 * \brief		reset of the update time mechanisme
	 * \fn			void	ClearUpdate()
	 */
	void	ClearUpdate()
	{
		mWasUpdate=false;
		mComputedTime=KDOUBLE_CONST(0.0);
	}

	/**
	 * \brief	destructor
	 * \fn 		~TimeProfiler();
	 */
	virtual ~TimeProfiler();

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
	kdouble mStartTime;
	//! computed time of the profiler
	kdouble	mComputedTime;
	//! last deltaTime computed
	kdouble	mLastDT;
	//! max deltaTime seen
	kdouble	mMaxDT;
	//! min deltaTime seen
	kdouble	mMinDT;
	//! TRUE if the profile was updated
	bool	mWasUpdate=false;
};

// ****************************************
// * GlobalProfilerManager class
// * --------------------------------------
/**
 * \class	GlobalProfilerManager
 * \file	TimeProfiler.h
 * \ingroup ModuleTimer
 * \brief	Utility class to manage all TimeProfiler instances.
 * 
 *  ?? Obsolete ??
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
		mGlobalTimer=0;
		mProfileCount=0;
		int i;
		for(i=0;i<MAX_PROFILER_COUNT;i++)
		{
			mProfilers[i]=0;
		}
		mProfileDrawingObject=0;
		mIsInit=false;
		mMethodID=CharToID::GetID("DrawProfiles");
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
	SP<Timer>			mGlobalTimer;
	//! number of profiler
	int				mProfileCount;
	//! list of profiler
	SP<TimeProfiler>	mProfilers[MAX_PROFILER_COUNT];
	//! link to the drawing object
	CMSP				mProfileDrawingObject;
	//! TRUE if the instance is initialized
	bool			mIsInit;

	//! empty list of params, used to call graphic object method 
	kstl::vector<CoreModifiableAttribute*>		mEmptyParams;
	//! draw method ID
	unsigned int	mMethodID;
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
