#pragma once



#include "CoreModifiable.h"
#include "CoreModifiableAttribute.h"
#include "MiniInstanceFactory.h"


namespace Kigs
{
	namespace Time
	{
		class Timer;
	}
	namespace Core
	{
		class KigsCore;


		/*
			methods declaration to init / close / update modules in dll
			and get the instance of ModuleBase class in the dll
		 */

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
			DECLARE_ABSTRACT_CLASS_INFO(ModuleBase, CoreModifiable, KigsCore)
				DECLARE_INLINE_CONSTRUCTOR(ModuleBase) {}

			// some typedef to point on dll methods
			//! dll methods init
			typedef ModuleBase* (*mDllInitFunc)(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params);

			/**
			 * \brief		pure virtual initialize module
			 *
			 * \fn			virtual void Init(Core* core, const std::vector<CoreModifiableAttribute*>* params)=0;
			 * \param		core : link to the core, NOT NULL
			 * \param		params : list of parameters, CAN BE NULL
			 */
			virtual void Init(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params) = 0;
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
			virtual void Update(const Time::Timer& timer, void* addParam) override = 0;

			/**
			 * \brief		return pointer on core used by this
			 * \fn			Core*	getCore(){return mCore;}
			 * \return		the Core instance
			 */
			KigsCore* getCore() { return mCore; }

		protected:

			/**
		 * \fn			void    RegisterPlatformSpecific(ModuleBase* platform);
		 * \brief		register specific modules
		 * \param		moduleName :name of the module
		 * \param		params : list of parameters
		 */
		void    RegisterPlatformSpecific(SP<ModuleBase> platform);

		/**
			 * \fn			void    BaseInit(Core* core,const std::string& moduleName, const std::vector<CoreModifiableAttribute*>* params);
			 * \brief		protected base init method, should be called by Init/Close and Update methods
			 * \param		moduleName :name of the module
			 * \param		params : list of parameters
			 */
			void    BaseInit(KigsCore* core, const std::string& moduleName, const std::vector<CoreModifiableAttribute*>* params);

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
			void    BaseUpdate(const Time::Timer& timer, void* addParam);



			//! pointer to the Core instance
			KigsCore* mCore = nullptr;

			// to be called by dynamic modules to set static instance of core
			void	StaticInitCore(KigsCore* core)
			{
				KigsCore::mCoreInstance = core;
			}
			
			std::vector<SP<ModuleBase>>    mPlatformModuleList;
		};

	}
}