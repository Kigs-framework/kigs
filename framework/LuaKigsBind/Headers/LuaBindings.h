#pragma once

#include "CoreModifiable.h"

#include "LuaKigsBindModule.h"
#include "LuaIntf/LuaIntf.h"

void setup_bindings(lua_State* L);

void PushAttribute(LuaIntf::LuaState L, CoreModifiableAttribute* attrib);
int CoreModifiableSetAttributeLua(lua_State* lua);
CoreModifiableAttribute* MakeAttributeFromLuaStack(lua_State* lua, int idx, CoreModifiable* owner = nullptr, const std::string& name = "ParamLuaRef");

LuaIntf::LuaRef GetModifiableLuaData(lua_State* lua, CoreModifiable* obj);

class LuaStackAssert
{
public:
	LuaStackAssert(lua_State* lua) 
#ifdef _DEBUG
		: mAt{ lua_gettop(lua) }, mLua{ lua }  
#endif
	{}
	~LuaStackAssert()
	{
#ifdef _DEBUG
		KIGS_ASSERT(mAt == lua_gettop(mLua));
#endif
	}
private:
	int mAt = 0;
	lua_State* mLua;
};

class LuaNotificationHook : public CoreModifiable
{
	public:
	DECLARE_CLASS_INFO(LuaNotificationHook, CoreModifiable, LuaKigsBindModule);
	DECLARE_CONSTRUCTOR(LuaNotificationHook);
	
	
	DECLARE_METHOD(CallLuaFunc);
	COREMODIFIABLE_METHODS(CallLuaFunc);
	CoreModifiable* obj;
	LuaIntf::LuaState L;
	LuaIntf::LuaRef lua_func;
};


namespace LuaIntf
{
	LUA_USING_LIST_TYPE(kstl::vector);
}


class LuaChildIterator : public LuaIntf::CppFunctor
{
	public:
	LuaChildIterator(CoreModifiable* obj)
	{
		_obj = obj;
		_i = 0;
	}
	
	virtual int run(lua_State* lua) override
	{
		if(!_obj) return 0;
		
		LuaIntf::LuaState L = lua;
		if(_i < _obj->getItems().size())
			L.push(_obj->getItems()[_i++].myItem);
		else
			return 0;
		return 1;
	}
	CoreModifiable* _obj;
	int _i;
};

inline int xpairs_childs(lua_State* L)
{
	auto cm = LuaIntf::CppObject::get<CoreModifiable>(L, 1, false);
	return LuaIntf::CppFunctor::make<LuaChildIterator>(L, cm);
}


namespace detail
{
	template<typename T>
	typename std::enable_if<!std::is_pointer<T>::value, T>::type GetLuaGlobal(LuaIntf::LuaRef ref)
	{
		T result = {};

		if (ref.isValid())
		{
			result = ref.toValue<T>();
		}
		else
		{
			KIGS_WARNING("cannot get global", 0);
		}
		return result;
	}

	template<typename T>
	typename std::enable_if<std::is_pointer<T>::value, T>::type GetLuaGlobal(LuaIntf::LuaRef ref)
	{
		T result = {};

		if (ref.isValid())
		{
			switch (ref.type())
			{

			case LuaIntf::LuaTypeID::LIGHTUSERDATA:
				result = (T)ref.toPtr();
				break;
			case LuaIntf::LuaTypeID::USERDATA:
			{
				T* ptr = nullptr;

				if ((ptr = LuaIntf::Lua::objectCast<T>(ref)))
				{
					result = *ptr;
				}
				break;
			}
			default:
				KIGS_WARNING("cannot get global", 0);
			}
		}
		else
		{
			KIGS_WARNING("cannot get global", 0);
		}
		return result;
	}
}


template<typename T>
T GetLuaGlobal(const kstl::string& name)
{
	LuaIntf::LuaState L = KigsCore::GetModule<LuaKigsBindModule>()->getLuaState();

	auto vec = SplitStringByCharacter(name, '.');
	L.getGlobal(vec[0].c_str());
	LuaIntf::LuaRef ref = L.toValue<LuaIntf::LuaRef>(-1);
	L.pop();

	for (u32 i = 1; i < vec.size(); ++i)
	{
		if (!ref.isTable()) break;
		ref = ref[vec[i]];
	}
	
	return detail::GetLuaGlobal<T>(ref);
}

