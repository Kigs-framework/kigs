#pragma once

#include "CoreModifiable.h"
#include "CoreModifiableAttribute.h"

#include <LuaIntf/LuaIntf.h>

#include "AttributePacking.h"

struct lua_State;

namespace Kigs
{
	using namespace ::LuaIntf;
	namespace Lua
	{
		using namespace Kigs::Core;

		class LuaKigsBindModule;

		// ****************************************
		// * LuaBehaviour class
		// * --------------------------------------
		/**
		 * \class	LuaBehaviour
		 * \file	LuaBehaviour.h
		 * \ingroup LuaBind
		 * \brief	Add LUA methods to a CoreModifiable instance.
		 */
		 // ****************************************
		class LuaBehaviour : public CoreModifiable
		{

		public:

			friend class LuaKigsBindModule;


			DECLARE_CLASS_INFO(LuaBehaviour, CoreModifiable, LuaBind)

				//! constructor
				LuaBehaviour(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);

			/*! virtual update.
			*/
			void Update(const Time::Timer& /* timer */, void* addParam) override;

			void InitLua(double current_time);

			//! destructor
			virtual ~LuaBehaviour();

		protected:
			//! init method
			void	InitModifiable() override;
			void	UninitModifiable() override;

			LuaKigsBindModule* mLuaModule;

			// Lua script or reference on script file
			maString				mScript;

			// enable / disable lua scripting
			maBool					mEnabled;

			// if interval is set, do update only if interval elapsed
			maFloat					mInterval;
			// last update time ( for interval evaluation)
			double					mLastTime;

			// check if lua needs init
			bool				    mLuaNeedInit;

			// check if we need to call lua update method
			bool				    mHasUpdate;

			CoreModifiable* mTarget;
			LuaIntf::LuaState       mL;
			LuaIntf::LuaRef         mSelf;

			void OnAddItemCallback(CoreModifiable* localthis, CoreModifiable* item);
			void OnRemoveItemCallback(CoreModifiable* localthis, CoreModifiable* item);

			WRAP_METHODS(OnAddItemCallback, OnRemoveItemCallback);

			DECLARE_METHOD(ReloadScript);
			COREMODIFIABLE_METHODS(ReloadScript);
		};
	}
}
