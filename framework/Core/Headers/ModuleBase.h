#ifndef _MODULEBASE_H_
#define _MODULEBASE_H_

class KigsCore;
class Timer;

#include "CoreModifiable.h"
#include "CoreModifiableAttribute.h"
#include "MiniInstanceFactory.h"

// ****************************************
// * ModuleDescription class
// * --------------------------------------
/**
 * \class	ModuleDescription
 * \ingroup Core
 * \brief	read a module description in a XML file
 */
// ****************************************
class ModuleDescription
{
public:
	ModuleDescription() = default;

	/**
	 * \fn		bool    Init(const kstl::string& fileName);
	 * \brief	read the XML file
	 * \param	fileName : name of the file to read
	 * \return	TRUE if the init succeed, FALSE otherwise
	 */
    bool    Init(const kstl::string& fileName);

	/**
	 * \fn		kstl::vector<kstl::string>&    GetDllList();
	 * \brief	return the list of dll associated with this module
	 * \return	the list of dll
	 */
    kstl::vector<kstl::string>&    GetDllList();

protected:
	//! list of dll
    kstl::vector<kstl::string>  mDllList;
	//! TRUE if the module has been initialized
    bool                        mIsInitialised = false;
	//! TRUE if the file is good?
    static bool                 mIsGoodFile;
	//! name of the module
    kstl::string                mModuleName;
	//! current reading depth
    static int                  mCurrentReadDepth;
};

class ModuleBase;

/*
	methods declaration to init / close / update modules in dll
	and get the instance of ModuleBase class in the dll
 */

class ModuleBase;

#if !defined(_KIGS_ONLY_STATIC_LIB_)
#if BUILDING_DLL
#ifndef DLLIMPORT
#define DLLIMPORT __declspec (dllexport)
#endif
extern "C"  DLLIMPORT ModuleBase*	 ModuleInit(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params);
#endif
#endif

struct DynamicModuleHandleAndPointer
{
	void*			mHandle = nullptr;
	ModuleBase*		mInstance = nullptr;
};

// ****************************************
// * ModuleBase class
// * --------------------------------------
/**
 * \class	ModuleBase
 * \file	ModuleBase.h
 * \ingroup Core
 * \brief	Base class for Module
 */
 // ****************************************
class ModuleBase : public CoreModifiable
{
public:
	// this class is an abstract class and inherits CoreModifiable
    DECLARE_ABSTRACT_CLASS_INFO(ModuleBase,CoreModifiable,KigsCore)
	DECLARE_INLINE_CONSTRUCTOR(ModuleBase) {}

	// some typedef to point on dll methods
	//! dll methods init
	typedef ModuleBase*		(*mDllInitFunc)(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params );
	//! dll methods close
	typedef void			(*mDllCloseFunc)(KigsCore* core );
	//! dll methods update
    typedef void			(*mDllUpdateFunc)(const Timer* timer);
	//! dll methods get function
	typedef void			(*mDLLGetModuleFunc)(ModuleBase** instance);

	/**
	 * \brief		pure virtual initialize module 
	 * \fn			virtual void Init(Core* core, const kstl::vector<CoreModifiableAttribute*>* params)=0;
	 * \param		core : link to the core, NOT NULL
	 * \param		params : list of parameters, CAN BE NULL
	 */
    virtual void Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)=0;
	using  CoreModifiable::Init;
	
	/**
	 * \brief		pure virtual close module 
	 * \fn			virtual void Close()=0;
	 */
    virtual void Close() = 0;

	/**
	 * \brief		pure virtual update module 
	 * \fn			virtual void Update(const Timer& timer,void* addParam) = 0;
	 * \param		timer : timer global
	 */
	virtual void Update(const Timer& timer, void* addParam) override = 0;

	/**
	 * \brief		return pointer on core used by this
	 * \fn			Core*	getCore(){return mCore;}
	 * \return		the Core instance
	 */
	KigsCore*	getCore(){return mCore;}

protected:
	/**
	 * \fn			void    BaseInit(Core* core,const kstl::string& moduleName, const kstl::vector<CoreModifiableAttribute*>* params);
	 * \brief		protected base init method, should be called by Init/Close and Update methods
	 * \param		moduleName :name of the module
	 * \param		params : list of parameters
	 */
    void    BaseInit(KigsCore* core,const kstl::string& moduleName, const kstl::vector<CoreModifiableAttribute*>* params);

#ifdef _KIGS_ONLY_STATIC_LIB_

	/**
	 * \fn			void    RegisterDynamic(ModuleBase* dynamic);
	 * \brief		in static mode only register specific modules
	 * \param		moduleName :name of the module
	 * \param		params : list of parameters
	 */
    void    RegisterDynamic(ModuleBase* dynamic);
#endif
	/**
	 * \fn			void    BaseClose();
	 * \brief		protected base close method, should be called by Init/Close and Update methods
	 */
    void    BaseClose();

	/**
	 * \fn			void    BaseUpdate(const Timer& timer, void* addParam);
	 * \brief		protected base update method, should be called by Init/Close and Update methods
	 * \param		timer : global timer
	 */
	void    BaseUpdate(const Timer& timer, void* addParam);

	

	//! pointer to the Core instance
    KigsCore* mCore = nullptr;

	//! list of dll handles
    kstl::vector<DynamicModuleHandleAndPointer>    mDynamicModuleList;

	// to be called by dynamic modules to set static instance of core
	void	StaticInitCore(KigsCore* core)
	{
		KigsCore::mCoreInstance = core;
	}
};

#endif //_MODULEBASE_H_
