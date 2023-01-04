#pragma once

#include "ModuleBase.h"
#include "AttributePacking.h"

struct lua_State;

namespace Kigs
{
	namespace Lua
	{
		using namespace Kigs::Core;
		class LuaBehaviour;
		class ScriptRunner;

		//#define LUA_IMGUI

		/*! \defgroup LuaBind Lua Kigs integration
		 *
		*/

		extern bool gLuaImGuiProtected;

		class LuaImGuiStackProtector
		{
		public:
			LuaImGuiStackProtector();
			~LuaImGuiStackProtector();
		private:
#ifdef LUA_IMGUI
			bool is_protecting;
#endif
		};

		struct LuaCLibrary
		{
			LuaCLibrary(void(*func)(lua_State* L)) : registerLibrary(func) {}
			void(*registerLibrary)(lua_State* L);
		};

		// ****************************************
		// * LuaKigsBindModule class
		// * --------------------------------------
		/**
		 * \file	LuaKigsBindModule.h
		 * \class	LuaKigsBindModule
		 * \ingroup LuaBind
		 * \ingroup Module
		 * \brief	Generic module for LUA integration.
		 */
		 // ****************************************

		class LuaKigsBindModule : public ModuleBase
		{
		public:
			DECLARE_CLASS_INFO(LuaKigsBindModule, ModuleBase, LuaKigsBind)
				DECLARE_CONSTRUCTOR(LuaKigsBindModule);

			void LoadLibraries(lua_State* L);
			//! module init
			void Init(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params) override;

			//! module close
			void Close() override;


			void AddToAutoUpdate(LuaBehaviour* script);
			void RemoveFromAutoUpdate(LuaBehaviour* script);


			lua_State* getLuaState() { return mLuaState; }

			void ReleaseRefs(CoreModifiable* obj);

			void AddLibrary(void(*func)(lua_State*));

			static int GetFile(lua_State* L);

			// global lua function
			bool	ExecuteString(const std::string& funcCode);
			bool	ExecuteString(const char* str);
			bool	CallLuaFunction(const std::string& funcName);
			bool    SafePCall(int nb_args, int nb_ret);
			bool    ExecuteLuaFile(const char* filename, const char* prepend = nullptr);

			//std::vector<CoreModifiableAttribute*> params : contains all the params passed to the method
			// params[0] is a maString containing method name, params[1...n] are passed to lua
			// if params contain a parameter named result, then the lua returned value is set in this param 

			// Add a dynamic lua method that sits on the top of the lua stack, the function is popped from the stack
			void AddLuaMethodFromStack(CoreModifiable* obj, const std::string& func_name);

			// Add a dynamic lua method with a string that looks like :
			/*
			"function f(this, arg1, arg2)
				this:DoStuff(arg1 + arg2)
			end
			*/
			void AddLuaMethod(CoreModifiable* obj, const std::string& func_name, const std::string& code);

			void ConnectToLua(CoreModifiable* e, const std::string& signal, CoreModifiable* r, const std::string& code);

			bool	CallCoreModifiableCallback(std::vector<CoreModifiableAttribute*>& params, CoreModifiable* localthis);

			//! destructor
			virtual ~LuaKigsBindModule();
		protected:

			void Update(const Time::Timer& timer, void* addParam) override
			{
				// nothing to do
			}

			DECLARE_METHOD(RegisterLuaMethod);
			COREMODIFIABLE_METHODS(RegisterLuaMethod);
			void SetValueLua(CoreModifiable* cm, const std::string& attrname, const std::string& code);

			template<typename T>
			T GetGlobal(const std::string& name);

			WRAP_METHODS(SetValueLua, AddLuaMethod, ConnectToLua);

			lua_State* mLuaState;

			std::vector<LuaCLibrary> mLibraries;
		};



		DEFINE_DYNAMIC_INLINE_METHOD(CoreModifiable, LuaGlobalCallback)
		{
			if (params.size()) // params[0] must contain method name (maString)
			{
				SP<LuaKigsBindModule> lua = KigsCore::GetModule("LuaKigsBindModule");
				return lua->CallCoreModifiableCallback(params, this);
			}
			return false;
		}

	}
}
