#include "FSM/FSM_Lua.h"
#include "FSM/FSM.h"
#include "FSM/FSM_State.h"


#include "JSonFileParser.h"
#include "LuaKigsBindModule.h"

#include "LuaIntf/LuaIntf.h"

using namespace LuaIntf;



// Internals
namespace
{
	
	
	kstl::string ParseMethod(FSM* fsm, LuaRef method, LuaKigsBindModule* luamodule, const kstl::string& method_name = "")
	{
		if(method.type() == LuaTypeID::STRING)
		{
			return method.toValue<kstl::string>();
		}
		else if(method.type() == LuaTypeID::FUNCTION)
		{
			auto action_name = method_name;
			method.pushToStack();
			luamodule->AddLuaMethod(fsm, action_name);
			fsm->InsertMethod(_S_2_ID(action_name), static_cast<RefCountedClass::ModifiableMethod>(&DynamicMethodLuaGlobalCallback::LuaGlobalCallback), action_name);
			return action_name;
		}
		return "";
	}
	
	void ParseStateMap(FSM* fsm, LuaRef state_map_ref)
	{
		auto luamodule = (LuaKigsBindModule*)KigsCore::GetModule(_S_2_ID("LuaKigsBindModule"));
		fsm->beginStateMap();
		// Each element is a state
		for(auto it : state_map_ref)
		{
			auto state_name = it.key<kstl::string>();
			auto state_desc = it.value<LuaRef>();
			
			fsm->beginStateDescription(state_name);
			
			for(auto it2 : state_desc)
			{
				auto k = it2.key<kstl::string>();
				
				if(k == "actions")
				{
					auto action_array = it2.value<LuaRef>();
					int i = 0;
					for(auto action_ref : action_array)
					{
						auto method_name = state_name + "_action_";
						if(i==0) method_name += "update_state";
						else if(i==1) method_name += "enter_state";
						else if(i==2) method_name += "leave_state";
						
						auto action_name = ParseMethod(fsm, action_ref.value(), luamodule, method_name);
						if(action_name != "")
							fsm->addStateAction((STATE_ACTION_TYPE)i, action_name);
						++i;
					}
				}
				else if(k == "transitions")
				{
					//fsm->beginTransitionMap();
					auto transition_array = it2.value<LuaRef>();
					int i = 0;
					for(auto transition_ref : transition_array)
					{
						auto notif_name = transition_ref.key<kstl::string>();
						auto transition_table = transition_ref.value<LuaRef>();
						
						int size = transition_table.len();
						if(size > 1)
						{
							auto guard_method_name = state_name + "_" + notif_name + "_" + transition_table[1].value<kstl::string>() + "_GUARD";
							
							guard_method_name = ParseMethod(fsm, transition_table[2].value(), luamodule, guard_method_name);
							fsm->addTransition(notif_name, transition_table[1].value<kstl::string>(), guard_method_name);
						}
						else if(size == 1)
						{
							fsm->addTransition(notif_name, transition_table[1].value<kstl::string>());
						}
					}
					//fsm->endTransitionMap();
				}
				else if(k == "instance")
				{
					auto instance_ref = it2.value();
					auto infos = instance_ref["infos"].value();
					auto statetype = infos[1].value<kstl::string>();
					auto statename = infos[2].value<kstl::string>();
					auto initstate = infos[3].value<kstl::string>();
					FSM_State* newstate = (FSM_State*)KigsCore::GetInstanceOf(statename, _S_2_ID(statetype));
					newstate->setValue(LABEL_TO_ID(InitState), initstate);
					ParseStateMap(newstate, instance_ref["StateMap"].value());
					newstate->Init();
					fsm->addStateInstance(newstate);
				}
				else if(k == "importXML")
				{
					FSM_State* newstate = (FSM_State*)CoreModifiable::Import(it2.value<kstl::string>());
					fsm->addStateInstance(newstate);
				}
				else if(k == "importJSON")
				{
					fsm->initStateFromJSONFile(it2.value<kstl::string>());
				}
				else if(k == "reference")
				{
					kstl::string instancename = it2.value<kstl::string>();
					kstl::set<CoreModifiable*> instances;
					fsm->GetSonInstancesByName("FSM_State", instancename, instances);
					if (instances.size() == 1)
					{
						FSM_State* newstate = (FSM_State*)*(instances.begin());
						// get one ref before removing it from this
						newstate->GetRef();
						fsm->removeItem(newstate);
						// then add it as a state
						fsm->addStateInstance(newstate);
					}
				}
			}
			fsm->endStateDescription();
		}
		fsm->endStateMap();
	}
	
	
	void BindStateMap(lua_State* lua)
	{
		LuaState L = lua;
		
		// ARG0 = obj
		// ARG1 = state_map table
		
		LuaRef obj_ref = L.toValue<LuaRef>(1);
		FSM* obj = (FSM*)Lua::objectCast<CoreModifiable>(obj_ref);
		
		
		if(!obj || !L.isTable(2))
		{
			L.push("error");
			L.error();
		}
		
		LuaRef state_map = L.toValue<LuaRef>(2);
		ParseStateMap(obj, state_map);
	}
	
	void FSMLuaRegisterLibrary(lua_State* lua)
	{
		LuaState L = lua;
		LuaBinding(L).beginModule("FSM")
			.addFunction("BindStateMap", &BindStateMap)
			.endModule();
	}
}


// API
void SetupFSMLuaBinding()
{
	auto lua_module = (LuaKigsBindModule*)KigsCore::GetModule(_S_2_ID("LuaKigsBindModule"));
	lua_module->AddLibrary(&FSMLuaRegisterLibrary);
}

