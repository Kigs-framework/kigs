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

	// execution order of the scripts
	struct ScriptPriorityCompare
	{
		//! overload operator () for comparison
		bool operator()(const LuaBehaviour * a1, const LuaBehaviour * a2) const
		{
			if (a1->GetPriority() == a2->GetPriority())
				return (a1)<(a2);
			return a1->GetPriority()<a2->GetPriority();
		}
	};
	
	DECLARE_CLASS_INFO(LuaBehaviour, CoreModifiable, LuaBind)
    
		//! constructor
	LuaBehaviour(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);
	
    /*! virtual update. 
    */
	void Update(const Timer& /* timer */,void* addParam) override;
	
	inline int GetPriority() const { return myPriority; }


/*	Obsolete ?
	bool push_self_ref(lua_State *L);
	void setField(const char * name, const char* value);*/
	
	void InitLua(kdouble current_time);

protected:
	
	
	//! destructor
	virtual ~LuaBehaviour();
	
	//! init method
	void	InitModifiable() override;
	void	UninitModifiable() override;
	void	NotifyUpdate(const unsigned int  labelid) override;
	
	LuaKigsBindModule*		myLuaModule;
	
	// Lua script or reference on script file
	maString				myScript;
	// add this to autoupdate ?
	maBool					myAutoUpdate;
	maBool					myUpdateWithParent;
	maBool					myEnabled;
	maFloat					myInterval;
	maInt				    myPriority;
	
	bool				    myLuaNeedInit;
	bool				    myHasUpdate;
	kdouble					myLastTime;
	
	CoreModifiable*			myTarget;
	LuaIntf::LuaState       L;
	LuaIntf::LuaRef         Self;
	
	void OnUpdateCallback(CoreModifiable* localthis, CoreModifiable* timer);
	void OnAddItemCallback(CoreModifiable* localthis, CoreModifiable* item);
	void OnRemoveItemCallback(CoreModifiable* localthis, CoreModifiable* item);

	WRAP_METHODS(OnUpdateCallback, OnAddItemCallback, OnRemoveItemCallback);

	bool    SafePCall(int nb_args, int nb_ret);

	DECLARE_METHOD(ReloadScript);

	DECLARE_METHOD(CallLuaMethod);

};    

#endif //_LUABEHAVIOUR_H_
