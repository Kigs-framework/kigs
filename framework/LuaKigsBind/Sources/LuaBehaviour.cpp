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

using namespace Kigs::Lua;

IMPLEMENT_CLASS_INFO(LuaBehaviour)


IMPLEMENT_CONSTRUCTOR(LuaBehaviour)
, mLuaModule(nullptr)
, mScript(*this,"Script", "")
, mEnabled(*this, "Enabled", true)
, mInterval(*this, "Interval", 0.0)
, mLuaNeedInit(false)
, mHasUpdate(false)
, mLastTime(-1.0)
, mTarget(nullptr)
{
	
}


void LuaBehaviour::InitLua(double current_time) 
{
	if(!mL || !mSelf) return;
	
	mSelf.pushToStack();
	mL.getField(-1, "init");
	
	if (!lua_isfunction(mL, -1)) 
	{
		mL.pop(2);
		return;
	}
	
	mSelf.pushToStack();
	mL.push(current_time);
	if (mLuaModule->SafePCall(2, 0))
		mLuaNeedInit = false;
}

LuaBehaviour::~LuaBehaviour()
{
	if (mL && mSelf)
	{
		mSelf.pushToStack();
		mL.getField(-1, "destroy");
		if (!mL.isNil(-1)) 
		{
			mSelf.pushToStack();
			mLuaModule->SafePCall(1, 0);
		}
		mL.pop(1);
		mSelf = nullptr;
	}
}    

void LuaBehaviour::Update(const Time::Timer& timer, void* addParam)
{
	// if no update method in lua, just return
	if (!mHasUpdate) return;

	// check if lua behaviour is enabled
	if (!mEnabled) return;

	double now = timer.GetTime();
	if (mLastTime < 0.0)
		mLastTime = now;
	
	double dt = now - mLastTime;
	if (dt < mInterval) return;
	mLastTime = now;
	
	// check that lua was init before calling update
	if (!mLuaNeedInit) 
	{ 
		// we can't call update if we still need to init
		mSelf.pushToStack();
		mL.getField(-1, "update");
		lua_pushvalue(mL, -2);
		lua_pushnumber(mL, now);
		mLuaModule->SafePCall(2, 0);
		mL.pop();
	}
}


//! try to init 
void	LuaBehaviour::InitModifiable()
{

	KigsCore::GetNotificationCenter()->addObserver(this, "ReloadScript", "ReloadScript");

	if (GetParentCount() == 0) return;
	CoreModifiable::InitModifiable();
	
	mLuaModule = (LuaKigsBindModule*)KigsCore::GetModule("LuaKigsBindModule").get();
	
	mL = mLuaModule->getLuaState();
	
	if(_isInit)
	{
		if(mScript != "")
		{
			bool ok = false;
			const char * buf = mScript.c_str();
			if (*buf == '#') 
			{
				buf++;
				ok = mLuaModule->ExecuteLuaFile(buf);
			}
			else
			{
				ok = mLuaModule->ExecuteString(mScript.c_str());
			}
			
			
			if (!lua_istable(mL,-1)) {
				KIGS_MESSAGE("Behaviour script must return a table");
				mL.pop();
				UninitModifiable();
				return;
			}
			
			
			mSelf = mL.toValue<LuaRef>(-1);
			// Self is on top of the stack
			
			mL.push((CoreModifiable*)this);
			mL.setField(-2, "script");
			mTarget = GetParents().front();
			
			mL.push(mTarget);
			mL.setField(-2, "target");
			
			KigsCore::Connect(mTarget, "AddItem", this, "OnAddItemCallback");
			KigsCore::Connect(mTarget, "RemoveItem", this, "OnRemoveItemCallback");
	
			mL.getField(-1, "init");
			mLuaNeedInit = mL.isFunction(-1);
			mL.pop();
			
			mL.getField(-1, "update");
			mHasUpdate = mL.isFunction(-1);
			mL.pop();
			
			mL.getField(-1, "WRAP_METHODS");

			std::vector<std::string>	wrappedMethodsList;
			if (mL.isTable(-1))
			{
				LuaRef wrapped = mL.toValue<LuaRef>(-1);
				for (auto w : wrapped)
				{
					std::string functionName = w.value().toValue<std::string>();
					wrappedMethodsList.push_back(functionName);
				}
			}
			mL.pop();
			for(const auto& m: wrappedMethodsList)
			{
				mL.getField(-1, m.c_str());
				if(mL.isFunction(-1))
				{
					mTarget->InsertFunctionNoUnpack(m, [m,this](std::vector<CoreModifiableAttribute*>& params)
					{				
								
						mSelf.pushToStack();
						auto top = mL.top();
						mL.getField(-1, m.c_str());
								
						if (!mL.isNil(-1))
						{
									
							mSelf.pushToStack();

							for (auto attr : params)
							{
								PushAttribute(mL, attr, mTarget);
							}

							if (mL.pcall((int)params.size()+1, LUA_MULTRET, 0) != 0)
							{
								printf("%s\n", mL.toString(-1));
								mL.pop();
							}

							int nb_results = mL.top() - top;
							for (int i = 1; i <= nb_results; ++i)
							{
								params.push_back(MakeAttributeFromLuaStack(mL, top + i));
							}

						}
						else
							mL.pop(1);

						mL.pop(1);
					});
				}
				mL.pop();
			}
			

			InitLua(KigsCore::GetCoreApplication()->GetApplicationTimer()->GetTime());
			
		}

	}
}

void LuaBehaviour::UninitModifiable() 
{
	
	CoreModifiable::UninitModifiable();
	mSelf = nullptr;
	
	mLastTime = -1.0;
	KigsCore::Instance()->GetNotificationCenter()->removeObserver(this);
	
	this->EmptyItemList();
	
	if (mTarget)
	{
		KigsCore::Disconnect(mTarget, "AddItem", this, "OnAddItemCallback");
		KigsCore::Disconnect(mTarget, "RemoveItem", this, "OnRemoveItemCallback");
	}
	mLuaModule = nullptr;
}



DEFINE_METHOD(LuaBehaviour, ReloadScript) 
{
	if (!mSelf)
	{
		Init();
		return false;
	}
	
	bool ok = false;
	const char * buf = mScript.c_str();

	if (*buf == '#')
	{
		buf++;
		ok = mLuaModule->ExecuteLuaFile(buf);
	}
	else
	{
		ok = mLuaModule->ExecuteString(mScript.c_str());
	}
	if (!ok) return false;

	if (!lua_istable(mL, -1))
	{
		KIGS_MESSAGE("Behaviour script must return a table");
		mL.pop();
	}
	else
	{
		mL.getGlobal("update_behaviour");
		mSelf.pushToStack();
		mL.pushValueAt(-3);
		mLuaModule->SafePCall(2, 0);
	}
	mL.pop();

	return false;
}

void LuaBehaviour::OnAddItemCallback(CoreModifiable* localthis, CoreModifiable* item)
{
	if(!mL || !mSelf) return;
	
	mSelf.pushToStack();
	mL.getField(-1, "addItem");

	if (!mL.isNil(-1))
	{
		mSelf.pushToStack();
		mL.push(item);
		mLuaModule->SafePCall(2, 0);
	}
	else
		mL.pop(1);

	mL.pop(1);
}

void LuaBehaviour::OnRemoveItemCallback(CoreModifiable* localthis, CoreModifiable* item)
{
	if(!mL || !mSelf) return;
	
	mSelf.pushToStack();
	mL.getField(-1, "removeItem");
	if (!mL.isNil(-1))
	{
		mSelf.pushToStack();
		mL.push(item);
		mLuaModule->SafePCall(2, 0);
	}
	else
		mL.pop(1);
	mL.pop(1);
	
	return;
}