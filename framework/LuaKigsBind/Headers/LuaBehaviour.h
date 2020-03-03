#ifndef _LUABEHAVIOUR_H_
#define _LUABEHAVIOUR_H_

#include "CoreModifiable.h"
#include "CoreModifiableAttribute.h"

#include <LuaIntf/LuaIntf.h>

#include "AttributePacking.h"

class LuaKigsBindModule;

// ****************************************
// * LuaBehaviour class
// * --------------------------------------
/*!  \class LuaBehaviour
     execute some lua script
*/
// ****************************************
struct lua_State;

class LuaBehaviour : public CoreModifiable
{
	
public:
	
	friend class LuaKigsBindModule;

	
	DECLARE_CLASS_INFO(LuaBehaviour, CoreModifiable, LuaBind)
    
		//! constructor
	LuaBehaviour(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);
	
    /*! virtual update. 
    */
	void Update(const Timer& /* timer */,void* addParam) override;

	void InitLua(kdouble current_time);

protected:
	
	
	//! destructor
	virtual ~LuaBehaviour();
	
	//! init method
	void	InitModifiable() override;
	void	UninitModifiable() override;
	
	LuaKigsBindModule*		myLuaModule;
	
	// Lua script or reference on script file
	maString				myScript;

	// enable / disable lua scripting
	maBool					myEnabled;

	// if interval is set, do update only if interval elapsed
	maFloat					myInterval;
	// last update time ( for interval evaluation)
	kdouble					myLastTime;
	
	// check if lua needs init
	bool				    myLuaNeedInit;

	// check if we need to call lua update method
	bool				    myHasUpdate;

	CoreModifiable*			myTarget;
	LuaIntf::LuaState       L;
	LuaIntf::LuaRef         Self;
	
	void OnAddItemCallback(CoreModifiable* localthis, CoreModifiable* item);
	void OnRemoveItemCallback(CoreModifiable* localthis, CoreModifiable* item);

	WRAP_METHODS(OnAddItemCallback, OnRemoveItemCallback);

	DECLARE_METHOD(ReloadScript);
	COREMODIFIABLE_METHODS(ReloadScript);
};    

#endif //_LUABEHAVIOUR_H_
