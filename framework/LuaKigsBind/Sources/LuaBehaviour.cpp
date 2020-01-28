#include "PrecompiledHeaders.h"
//#include "Luna5.h"
#include "LuaBehaviour.h"
//#include "LuaHook.h"
//#include "CoreModifiableBind.h"
//#include "CoreBind.h"
#include <Timer.h>
#include <CoreBaseApplication.h>
#include <NotificationCenter.h>
//#include <RenderingScreenBind.h>
#include <ModuleBase.h>
#include <LuaKigsBindModule.h>
#include <ModuleFileManager.h>
#include "LuaBindings.h"


using namespace LuaIntf;



IMPLEMENT_CLASS_INFO(LuaBehaviour)


IMPLEMENT_CONSTRUCTOR(LuaBehaviour)
, myLuaModule(nullptr)
, myScript(*this, true, LABEL_AND_ID(Script), "")
, myAutoUpdate(*this, false, LABEL_AND_ID(AutoUpdate), true)
, myUpdateWithParent(*this, false, LABEL_AND_ID(UpdateOnParentUpdate), false)
, myEnabled(*this, false, LABEL_AND_ID(Enabled), true)
, myInterval(*this, false, LABEL_AND_ID(Interval), 0.0)
, myPriority(*this, false, LABEL_AND_ID(ScriptPriority), 0)
, myLuaNeedInit(false)
, myHasUpdate(false)
, myLastTime(-1.0)
, myTarget(nullptr)
{
	CONSTRUCT_METHOD(LuaBehaviour, ReloadScript)
}


void LuaBehaviour::InitLua(kdouble current_time) 
{
	if(!L || !Self) return;
	
	Self.pushToStack();
	L.getField(-1, "init");
	
	if (!lua_isfunction(L, -1)) 
	{
		L.pop(2);
		return;
	}
	
	Self.pushToStack();
	L.push(current_time);
	if (myLuaModule->SafePCall(2, 0))
		myLuaNeedInit = false;
}

LuaBehaviour::~LuaBehaviour()
{
	if (myLuaModule)
	{
		myLuaModule->RemoveFromAutoUpdate(this);
		myLuaModule->RemoveFromInit(this);
	}
	
	if (L && Self)
	{
		Self.pushToStack();
		L.getField(-1, "destroy");
		if (!L.isNil(-1)) 
		{
			Self.pushToStack();
			myLuaModule->SafePCall(1, 0);
		}
		L.pop(1);
		Self = nullptr;
	}
	
	if (myTarget)
	{
		KigsCore::Disconnect(myTarget, "Update", this, "OnUpdateCallback");
		KigsCore::Disconnect(myTarget, "AddItem", this, "OnAddItemCallback");
		KigsCore::Disconnect(myTarget, "RemoveItem", this, "OnRemoveItemCallback");
	}
}    

void LuaBehaviour::Update(const Timer& timer, void* addParam)
{
	kdouble now = timer.GetTime();
	if (myLastTime < 0.0)
		myLastTime = now;
	
	kdouble dt = now - myLastTime;
	if (dt < myInterval) return;
	myLastTime = now;
	
	if (!myHasUpdate) return;
	if (!myEnabled) return;
	
	if (!myLuaNeedInit)
	{ 
		// we can't call update if we still need to init
		Self.pushToStack();
		L.getField(-1, "update");
		lua_pushvalue(L, -2);
		lua_pushnumber(L, now);
		myLuaModule->SafePCall(2, 0);
		L.pop();
	}
}


//! try to init 
void	LuaBehaviour::InitModifiable()
{

	KigsCore::GetNotificationCenter()->addObserver(this, "ReloadScript", "ReloadScript");

	if (GetParentCount() == 0) return;
	CoreModifiable::InitModifiable();
	
	myLuaModule = (LuaKigsBindModule*)KigsCore::GetModule("LuaKigsBindModule");
	
	L = myLuaModule->getLuaState();
	
	if(_isInit)
	{
		if(myScript.const_ref() != "")
		{
			bool ok = false;
			const char * buf = myScript.c_str();
			if (*buf == '#') 
			{
				buf++;
				ok = myLuaModule->ExecuteLuaFile(buf);
			}
			else
			{
				ok = myLuaModule->ExecuteString(myScript.c_str());
			}
			
			
			if (!lua_istable(L,-1)) {
				KIGS_MESSAGE("Behaviour script must return a table");
				L.pop();
				UninitModifiable();
				return;
			}
			
			
			Self = L.toValue<LuaRef>(-1);
			// Self is on top of the stack
			
			L.push((CoreModifiable*)this);
			L.setField(-2, "script");
			myTarget = GetParents().front();
			
			L.push(myTarget);
			L.setField(-2, "target");
			
			KigsCore::Connect(myTarget, "Update", this, "OnUpdateCallback");
			KigsCore::Connect(myTarget, "AddItem", this, "OnAddItemCallback");
			KigsCore::Connect(myTarget, "RemoveItem", this, "OnRemoveItemCallback");
	
			L.getField(-1, "init");
			myLuaNeedInit = L.isFunction(-1);
			L.pop();
			
			L.getField(-1, "update");
			myHasUpdate = L.isFunction(-1);
			L.pop();
			
			if (myLuaNeedInit) 
			{
				myLuaModule->AddToInit(this);
			}
			
			// search for methods to add to CoreModifiable
			if (myTarget)
			{
				LuaRef table(L, -1);

				for (auto& e : table) {
					std::string key = e.key<std::string>();
					if (key.substr(0, 10) == "CoreMethod")
					{
						std::string methodName = key.substr(10, key.size() - 10);
						if (methodName.length())
						{
							LuaRef value = e.value<LuaRef>();
							if (value.isFunction())
							{
								myTarget->InsertMethod(methodName, static_cast<RefCountedClass::ModifiableMethod>(&LuaBehaviour::CallLuaMethod), key);
							}
						}
					}
				}
			}
		}
		
		myAutoUpdate.changeNotificationLevel(Owner);
		myPriority.changeNotificationLevel(Owner);
		if (myAutoUpdate && myHasUpdate)
		{
			myLuaModule->AddToAutoUpdate(this);
		}
	}
}

void LuaBehaviour::UninitModifiable() 
{
	if (myLuaModule)
	{
		myLuaModule->RemoveFromAutoUpdate(this);
		myLuaModule->RemoveFromInit(this);
	}
	CoreModifiable::UninitModifiable();
	Self = nullptr;
	
	myLastTime = -1.0;
	KigsCore::Instance()->GetNotificationCenter()->removeObserver(this);
	
	this->EmptyItemList();
	
	if (myTarget)
	{
		KigsCore::Disconnect(myTarget, "Update", this, "OnUpdateCallback");
		KigsCore::Disconnect(myTarget, "AddItem", this, "OnAddItemCallback");
		KigsCore::Disconnect(myTarget, "RemoveItem", this, "OnRemoveItemCallback");
	}
	myLuaModule = nullptr;
}


void LuaBehaviour::NotifyUpdate(const unsigned labelid) {
	
	if (!myLuaModule) return;
	
	if (labelid == myAutoUpdate.getLabelID()) {
		if (myAutoUpdate) 
			myLuaModule->AddToAutoUpdate(this);
		else
			myLuaModule->RemoveFromAutoUpdate(this);
	}
	else if (labelid == myPriority.getLabelID()) {
		myLuaModule->SetNeedResort();
	}
	
}


DEFINE_METHOD(LuaBehaviour, ReloadScript) 
{
	if (!Self)
	{
		Init();
		return false;
	}
	
	bool ok = false;
	const char * buf = myScript.c_str();

	if (*buf == '#')
	{
		buf++;
		ok = myLuaModule->ExecuteLuaFile(buf);
	}
	else
	{
		ok = myLuaModule->ExecuteString(myScript.c_str());
	}
	if (!ok) return false;

	if (!lua_istable(L, -1))
	{
		KIGS_MESSAGE("Behaviour script must return a table");
		L.pop();
	}
	else
	{
		L.getGlobal("update_behaviour");
		Self.pushToStack();
		L.pushValueAt(-3);
		myLuaModule->SafePCall(2, 0);
	}
	L.pop();

	return false;
}


bool    LuaBehaviour::SafePCall(int nb_args, int nb_ret)
{
	LuaImGuiStackProtector protector;
	if (lua_pcall(L, nb_args, nb_ret, 0) != 0)
	{
		KIGS_MESSAGE(lua_tostring(L, -1));
		lua_pop(L, 1);
		return false;
	}
	return true;
}

DEFINE_METHOD(LuaBehaviour, CallLuaMethod)
{
	if (params.size())
	{
		kstl::string funcName = ((maString*)params[0])->const_ref();

		int top = L.top();
		L.getField(-1, funcName.c_str());

		if (L.isNil(-1))
		{
			L.pop(2); // pop nil and method table
			return false;
		}

		// check for result
		CoreModifiableAttribute* result = 0;

		int paramCount = 1; // at least one param for this
		int returnCount = 0;
		// push params but not result
		kstl::vector<CoreModifiableAttribute*>::iterator	itparam = params.begin() + 1; // skip function name
		kstl::vector<CoreModifiableAttribute*>::iterator	itend = params.end();
		while (itparam != itend)
		{
			if ((*itparam)->getLabelID() == "Result")
			{
				result = (*itparam);
				returnCount++;
			}
			else
			{
				PushAttribute(L, *itparam);
				paramCount++;
			}
			++itparam;
		}

		LuaImGuiStackProtector protector;
		if (returnCount == 0)
			returnCount = LUA_MULTRET;

		if (SafePCall(paramCount, returnCount))
		{
			int nb_results = L.top() - top;
			if (returnCount == nb_results)
			{
				if (!L.isNil(-1))
				{
					if (returnCount > 0)
					{
						if (!L.isNumber(-1)) // should be number or string
						{
							result->setValue(L.toString(-1));
						}
						else
						{
							result->setValue(L.toNumber(-1));
						}
					}
					else if (L.isBool(-1))
					{
						bool b = L.toBool(-1);
						L.pop();
						return b;
					}
				}
			}
			else
			{
				for (int i = 0; i < nb_results; ++i)
				{
					params.push_back(MakeAttributeFromLuaStack(L, -nb_results + i));
				}
			}
			L.pop(nb_results);
		}


		L.pop(); // Pop method table;
	}
	return false;
}

void LuaBehaviour::OnUpdateCallback(CoreModifiable* localthis, CoreModifiable* timer)
{
	if (myUpdateWithParent)
		Update(*(Timer*)timer,0);
}

void LuaBehaviour::OnAddItemCallback(CoreModifiable* localthis, CoreModifiable* item)
{
	if(!L || !Self) return;
	
	Self.pushToStack();
	L.getField(-1, "addItem");

	if (!L.isNil(-1))
	{
		Self.pushToStack();
		L.push(item);
		myLuaModule->SafePCall(2, 0);
	}
	else
		L.pop(1);

	L.pop(1);
}

void LuaBehaviour::OnRemoveItemCallback(CoreModifiable* localthis, CoreModifiable* item)
{
	if(!L || !Self) return;
	
	Self.pushToStack();
	L.getField(-1, "removeItem");
	if (!L.isNil(-1))
	{
		Self.pushToStack();
		L.push(item);
		myLuaModule->SafePCall(2, 0);
	}
	else
		L.pop(1);
	L.pop(1);
	
	return;
}