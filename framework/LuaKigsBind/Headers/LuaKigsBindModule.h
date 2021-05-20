#ifndef _LUAKIGSBINDMODULE_H_
#define _LUAKIGSBINDMODULE_H_

#include "ModuleBase.h"
#include "AttributePacking.h"

class LuaBehaviour;
class ScriptRunner;
struct lua_State;

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
	LuaCLibrary(void(*func)(lua_State* L)) : registerLibrary(func){}
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
    void Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params) override;
	
	//! module close
    void Close() override;
	
	
	void AddToAutoUpdate(LuaBehaviour* script);
	void RemoveFromAutoUpdate(LuaBehaviour* script);
	

	lua_State* getLuaState(){ return mLuaState; }
	
	void ReleaseRefs(CoreModifiable* obj);
	
	void AddLibrary(void(*func)(lua_State*));
	
	static int GetFile(lua_State *L);
	
	// global lua function
	bool	ExecuteString(const kstl::string& funcCode);
	bool	ExecuteString(const char* str);
	bool	CallLuaFunction(const kstl::string& funcName);
	bool    SafePCall(int nb_args, int nb_ret);
	bool    ExecuteLuaFile(const char* filename, const char* prepend=nullptr);
	
	//kstl::vector<CoreModifiableAttribute*> params : contains all the params passed to the method
	// params[0] is a maString containing method name, params[1...n] are passed to lua
	// if params contain a parameter named result, then the lua returned value is set in this param 
	
	// Add a dynamic lua method that sits on the top of the lua stack, the function is popped from the stack
	void AddLuaMethodFromStack(CoreModifiable* obj, const kstl::string& func_name);
	
	// Add a dynamic lua method with a string that looks like :
	/*
	"function f(this, arg1, arg2)
		this:DoStuff(arg1 + arg2)
	end
	*/
	void AddLuaMethod(CoreModifiable* obj, const kstl::string& func_name, const kstl::string& code);
	
	void ConnectToLua(CoreModifiable* e, const kstl::string& signal, CoreModifiable* r, const kstl::string& code);

	bool	CallCoreModifiableCallback(kstl::vector<CoreModifiableAttribute*>& params,CoreModifiable* localthis);

	//! destructor
	virtual ~LuaKigsBindModule();
protected:
	
	void Update(const Timer& timer, void* addParam) override
	{
		// nothing to do
	}
	
	DECLARE_METHOD(RegisterLuaMethod);
	COREMODIFIABLE_METHODS(RegisterLuaMethod);
	void SetValueLua(CoreModifiable* cm, const kstl::string& attrname, const kstl::string& code);
	
	template<typename T>
	T GetGlobal(const kstl::string& name);

	WRAP_METHODS(SetValueLua, AddLuaMethod, ConnectToLua);

	lua_State*		mLuaState;
	
	kstl::vector<LuaCLibrary> mLibraries;
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




#endif //_LUAKIGSBINDMODULE_H_
