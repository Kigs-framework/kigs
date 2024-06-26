#include "LuaBindings.h"
#include "LuaKigsBindModule.h"
#include "NotificationCenter.h"
#include "KeyboardDevice.h"
#include "DeviceItem.h"
#include "MouseDevice.h"
#include "Node3D.h"
#include "ModuleCoreAnimation.h"
#include "AttributePacking.h"
#include "CoreBaseApplication.h"
#include "TouchInputEventManager.h"
#include "Node2D.h"

#include <algorithm>
#include <tuple>

using namespace LuaIntf;
using namespace Kigs;
using namespace Kigs::Lua;
using namespace Kigs::Core;
using namespace Kigs::Input;
using namespace Kigs::Draw;
using namespace Kigs::Draw2D;
using namespace Kigs::Action;

template<bool notificationLevel, bool isInitT = false, bool isReadOnlyT = false, bool isOrphanT = false>
class maLuaRefHeritage : public CoreModifiableAttributeData<LuaRef, notificationLevel, isInitT, isReadOnlyT, isOrphanT>
{
	DECLARE_ATTRIBUTE_HERITAGE_NO_ASSIGN(maLuaRefHeritage, maLuaRefHeritage, LuaRef, CoreModifiable::ATTRIBUTE_TYPE::LUAREF);
	auto& operator=(const CurrentAttributeType& value)
	{
		mValue = value; 
		return *this; 
	}
public:
};
using maLuaRef = maLuaRefHeritage<false,false,false,false>;
using maLuaRefOrphan = maLuaRefHeritage<false, false, false, true>;

unordered_map<KigsID, const char*> gLuaTypeMap;

IMPLEMENT_CLASS_INFO(LuaNotificationHook);

IMPLEMENT_CONSTRUCTOR(LuaNotificationHook)
{

}


DEFINE_METHOD(LuaNotificationHook, CallLuaFunc)
{
	LuaImGuiStackProtector protector;
	
	mLuaFunc.pushToStack();
	mL.push(mObj);
	if(mL.pcall(1, 0, 0) != 0)
	{
		printf("%s\n", mL.toString(-1));
		mL.pop();
	}
	return false;
}

void Kigs::Lua::PushAttribute(LuaState L, CoreModifiableAttribute* attrib, CoreModifiable* owner)
{
	CoreModifiable::ATTRIBUTE_TYPE type = attrib->getType();
	
	bool isArray = type == CoreModifiable::ATTRIBUTE_TYPE::ARRAY;

	if (type == CoreModifiable::ATTRIBUTE_TYPE::LUAREF)
	{
		LuaRef ref = static_cast<CoreModifiableAttributeData<LuaRef>*>(attrib)->const_ref();
		ref.pushToStack();
		return;
	}
	
	if (isArray)
		type = attrib->getArrayElementType();
	
	bool isInt =
		type == CoreModifiable::ATTRIBUTE_TYPE::CHAR ||
		type == CoreModifiable::ATTRIBUTE_TYPE::INT ||
		type == CoreModifiable::ATTRIBUTE_TYPE::SHORT ||
		type == CoreModifiable::ATTRIBUTE_TYPE::LONG;
	
	bool isUint =
		type == CoreModifiable::ATTRIBUTE_TYPE::UCHAR ||
		type == CoreModifiable::ATTRIBUTE_TYPE::USHORT ||
		type == CoreModifiable::ATTRIBUTE_TYPE::UINT ||
		type == CoreModifiable::ATTRIBUTE_TYPE::ULONG;
	
	
	bool isNumber =
		type == CoreModifiable::ATTRIBUTE_TYPE::FLOAT ||
		type == CoreModifiable::ATTRIBUTE_TYPE::DOUBLE;
	
	
	if (!isArray)
	{
		if (type == CoreModifiable::ATTRIBUTE_TYPE::WEAK_REFERENCE || type == CoreModifiable::ATTRIBUTE_TYPE::STRONG_REFERENCE)
		{
			//NOTE(antoine) thread unsafe in weak_reference's case
			CMSP	ModifiableRef=nullptr;
			attrib->getValue(ModifiableRef, owner);
			L.push(ModifiableRef.get());
		}
		else if (type == CoreModifiable::ATTRIBUTE_TYPE::BOOL)
		{
			bool b;
			attrib->getValue(b, owner);
			lua_pushboolean(L, b);
		}
		else if (isInt)
		{
			s64 value;
			attrib->getValue(value, owner);
			lua_pushinteger(L, value);
		}
		
		else if (isUint)
		{
			u64 value;
			attrib->getValue(value, owner);
			lua_pushinteger(L, value);
		}
		
		else if (isNumber)
		{
			double value;
			attrib->getValue(value, owner);
			lua_pushnumber(L, value);
		}
		else if (type == CoreModifiable::ATTRIBUTE_TYPE::RAWPTR)
		{
			auto it = gLuaTypeMap.find(attrib->id());
			void* ptr;
			attrib->getValue(ptr, owner);

			if (it != gLuaTypeMap.end())
			{
				lua_getglobal(L, it->second);
				lua_getfield(L, -1, "__call");
				L.remove(-2);
				lua_pushlightuserdata(L, ptr);
				L.call(1, 1);
			}
			else
			{
				lua_pushlightuserdata(L, ptr);
			}
		}
		else
		{
			std::string val;
			if(attrib->getValue(val, owner))
				lua_pushstring(L, val.c_str());
			else
				L.push(nullptr);
		}
	}
	else
	{
		size_t size = attrib->getNbArrayColumns();
		bool isvec = false;
		
		double values[4];
		if (size >= 2 && size <= 4 && (isInt || isUint || isNumber))
		{
			isvec = true;
		}
		else
			lua_newtable(L);
		
		for (int i = 0; i < size; ++i)
		{
			if (isInt)
			{
				s64 value;
				attrib->getArrayElementValue(value, owner, 0, i);
				if(isvec)
					values[i] = value;
				else
					lua_pushinteger(L, value);
			}
			else if (isUint)
			{
				u64 value;
				attrib->getArrayElementValue(value, owner, 0, i);
				if(isvec)
					values[i] = value;
				else
					lua_pushinteger(L, value);
			}
			else if (isNumber)
			{
				double value;
				attrib->getArrayElementValue(value, owner, 0, i);
				if(isvec)
					values[i] = value;
				else
					lua_pushnumber(L, value);
			}
			else
			{
				std::string val;
				if(attrib->getArrayElementValue(val, owner, 0, i))
					lua_pushstring(L, val.c_str());
				else
					L.push(nullptr);
			}
			// Add to table
			if (!isvec)
				lua_rawseti(L, -2, i + 1);
		}
		if(isvec)
		{
			if(size == 2)
				L.push(v2f(values[0], values[1]));
			else if(size == 3)
				L.push(v3f(values[0], values[1], values[2]));
			else if(size == 4)
				L.push(v4f(values[0], values[1], values[2], values[3]));
		}
	}
}


static CMSP factory(const std::string& name, const std::string& type)
{
	return KigsCore::GetInstanceOf(name, type);
}

int Kigs::Lua::CoreModifiableSetAttributeLua(lua_State* lua)
{
	//LuaStack
	// Modifiable
	// Attribute Name
	// Value
	
	// Return 0 when ok
	// Return 1 (nil value) on error

	LuaState L = lua;
	auto cm = CppObject::get<CoreModifiable>(L, 1, false);
	


	int nbArgs = L.top()-1;
	
	const std::string name = luaL_checkstring(L, 2);
	
	CoreModifiableAttribute* attr = cm->getAttribute(name);
	
	int value_idx = 3;
	int type = lua_type(L, value_idx);

	if (!attr && type != LUA_TFUNCTION)
	{
		attr = MakeAttributeFromLuaStack(L, value_idx, cm, name);
		/*auto ref = L.toValue<LuaRef>(value_idx);
		attr = new maLuaRef(*cm, false, name, ref);*/
		return 0;
	}

	if (attr && attr->getType() == CoreModifiable::ATTRIBUTE_TYPE::LUAREF)
	{
		auto ref = L.toValue<LuaRef>(value_idx);
		static_cast<CoreModifiableAttributeData<LuaRef>*>(attr)->ref() = ref;
		return 0;
	}
	
	if (nbArgs == 2)
	{
		if (type == LUA_TUSERDATA)
		{
			LuaRef ref = L.toValue<LuaRef>(value_idx);
			v2f* pt2 = nullptr;
			v3f* pt3 = nullptr;
			v4f* pt4 = nullptr;
			CoreModifiable* obj = nullptr;
			char str[128];
			if ((pt2 = LuaIntf::Lua::objectCast<v2f>(ref)))
			{
				snprintf(str, 128, "[%f,%f]", pt2->x, pt2->y);
			}
			else if((pt3 = LuaIntf::Lua::objectCast<v3f>(ref)))
			{
				snprintf(str, 128, "[%f,%f,%f]", pt3->x, pt3->y, pt3->z);
			}
			else if ((pt4 = LuaIntf::Lua::objectCast<v4f>(ref)))
			{
				snprintf(str, 128, "[%f,%f,%f,%f]", pt4->x, pt4->y, pt4->z, pt4->w);
			}
			else if((obj = LuaIntf::Lua::objectCast<CoreModifiable>(ref)))
			{
				attr->setValue(obj, cm);
				return 0;
			}
			else
			{
				lua_pushnil(L);
				return 1;
			}
			
			attr->setValue(str, cm);
			return 0;
		}
		else if (type == LUA_TTABLE)
		{
			size_t size = attr->getNbArrayColumns();
			L.getTableLen(value_idx);
			int table_len = L.popValue<int>();
			
			if (table_len != size)
			{
				lua_pushnil(L);
				return 1;
			}
			
			for (int i = 0; i < table_len; ++i)
			{
				L.getField(value_idx, i + 1);
				bool isnumber = L.isNumber(-1);
				L.pop();
				
				if (!isnumber)
				{
					lua_pushnil(L);
					return 1;
				}
			}
			
			bool was_set = false;
			for (int i = 0; i < size; ++i)
			{
				L.getField(value_idx, i + 1);
				double value = lua_tonumber(L, -1);
				was_set = attr->setArrayElementValue(value, cm, 0, i) || was_set;
				L.pop();
			}
			
			if (was_set)
				return 0;
		}
		else if (type == LUA_TBOOLEAN)
		{
			bool b = lua_toboolean(L, value_idx);
			if (cm->setValue(name, b))
				return 0;
		}
		else if (type == LUA_TNUMBER)
		{
			double v = lua_tonumber(L, value_idx);
			if(cm->setValue(name, v))
				return 0;
		}
		else if (type == LUA_TLIGHTUSERDATA)
		{
			void* ptr = lua_touserdata(L, value_idx);
			if (cm->setValue(name, ptr))
				return 0;
		}
		else if (type == LUA_TFUNCTION)
		{
			auto func = L.toValue<LuaRef>(value_idx);
			cm->InsertFunctionNoUnpack(name, [func, L, cm](std::vector<CoreModifiableAttribute*>& params)
			{
				auto top = L.top();
				func.pushToStack();
				for (auto attr : params)
				{
					PushAttribute(L, attr, cm);
				}
				if (L.pcall((int)params.size(), LUA_MULTRET, 0) != 0)
				{
					printf("%s\n", L.toString(-1));
					L.pop();
				}
				int nb_results = L.top() - top;
				for (int i = 1; i <= nb_results; ++i)
				{
					params.push_back(MakeAttributeFromLuaStack(L, top + i,cm));
				}

			});
			return 0;
		}
		else
		{
			const char* value = lua_tostring(L, value_idx);
			if (cm->setValue(name, value))
				return 0;
		}
	}
	lua_pushnil(L);
	return 1;
}


CoreModifiableAttribute* Kigs::Lua::MakeAttributeFromLuaStack(lua_State* lua, int idx, CoreModifiable* owner, const std::string& name)
{
	LuaState L = lua;
	switch (L.type(idx))
	{
	case LUA_TBOOLEAN:
		return MakeAttribute(L.toBool(idx), owner, name);
		break;
	case LUA_TNUMBER:
		return MakeAttribute(L.toNumber(idx), owner, name);
		break;
	case LUA_TSTRING:
		return MakeAttribute(L.toString(idx), owner, name);
		break;
	case LUA_TLIGHTUSERDATA:
	{
		LuaRef ref = L.toValue<LuaRef>(idx);
		auto mt = ref.getMetaTable();
		std::string typestr = "ParamPtr";
		if (mt)
		{
			auto typeref = mt["___type"];
			if (typeref.value())
			{
				typestr = typeref.value().toValue<std::string>();
				typestr = typestr.substr(6);
				typestr = typestr.substr(0, typestr.size() - 1);
			}
		}
		return MakeAttribute(L.toUserData(idx), owner, owner ? name : typestr);
		break;
	}
		
	case LUA_TUSERDATA:
		{
			LuaRef ref = L.toValue<LuaRef>(idx);

			v2f* pt2 = nullptr;
			v3f* pt3 = nullptr;
			v4f* pt4 = nullptr;
			CoreModifiable* obj = nullptr;

			if ((pt2 = LuaIntf::Lua::objectCast<v2f>(ref)))
			{
				return MakeAttribute(std::move(*pt2), owner, name);
			}
			else if ((pt3 = LuaIntf::Lua::objectCast<v3f>(ref)))
			{
				return MakeAttribute(std::move(*pt3), owner, name);
			}
			else if ((pt4 = LuaIntf::Lua::objectCast<v4f>(ref)))
			{
				return MakeAttribute(std::move(*pt4), owner, name);
			}
			else if ((obj = LuaIntf::Lua::objectCast<CoreModifiable>(ref)))
			{
				return MakeAttribute(obj, owner, name);
			}
			else
			{
				// lua object might get freed by the gc
				/*
				void * ptr = ref.toPtr();
				return MakeAttribute(ptr, nullptr, typestr);
				*/
			}
			break;
		}
	}
	LuaRef ref = L.toValue<LuaRef>(idx);
	if (owner) return new maLuaRef(*owner, name, ref);
	else return new maLuaRefOrphan("ParamLuaRef", ref);
	//return MakeAttribute(nullptr, nullptr);
}



static int CoreModifiableMethodCaller(lua_State* lua)
{
	LuaState L = lua;
	//CoreModifiable* cm = (CoreModifiable*)L.toUserData(lua_upvalueindex(1));
	const std::string name = L.toString(lua_upvalueindex(1));
	
	LuaRef ref = L.toValue<LuaRef>(1);
	CoreModifiable* cm = LuaIntf::Lua::objectCast<CoreModifiable>(ref);
	if(!cm)
	{
		L.push("Attempting to call a method but the first argument is not a CoreModifiable");
		L.error();
	}
	
	if(!cm->HasMethod(name))
	{
		char str[256];
		snprintf(str, 256, "Attempting to call '%s' on '%s'. Method not found", name.c_str(), cm->GetRuntimeID().c_str());
		L.push(str);
		L.error();
	}
	
	int t = L.top();
	
	std::vector<CoreModifiableAttribute*> p;
	for (int i = 1; i < t; ++i)
	{
		p.push_back(MakeAttributeFromLuaStack(L, i + 1));
	}

#ifdef _DEBUG
	//printf("Calling %s on %s\n", name, cm->getName().c_str());
#endif

	int args = (int)p.size();
	bool result = cm->CallMethod(name, p, nullptr, cm);
	int nb_result = 1;
	for(int i=args; i<p.size(); ++i)
	{
		PushAttribute(L, p[i],cm);
		nb_result++;
	}
	
	for (auto attr : p)
		delete attr;

	L.push(result);
	return nb_result;
}

static int CoreModifiableGetAttributeLua(lua_State* lua)
{
	LuaState L = lua;
	
	auto cm = CppObject::get<CoreModifiable>(L, 1, false);
	
	const std::string name = L.checkString(2);
	
	CoreModifiableAttribute* attrib = cm->getAttribute(name);
	if (attrib)
	{
		PushAttribute(L, attrib,cm);
	}
	else if(cm->HasMethod(name))
	{
		//lua_pushlightuserdata(L, cm);
		lua_pushstring(L, name.c_str());
		lua_pushcclosure(L, &CoreModifiableMethodCaller, 1);
	}
	else
	{
		lua_pushnil(L);
	}
	
	return 1;
}

static int CoreModifiableEmit(CoreModifiable* obj, lua_State* lua)
{
	LuaState L = lua;
	std::string notif = L.checkString(2);
	obj->EmitSignal(notif);
	return 0;
}



int CoreModifiableAddHook(CoreModifiable* obj, lua_State* lua)
{
	LuaState L = lua;
	
	const char* notif = L.checkString(2);
	
	if(L.isString(3))
	{
		const char* method = L.toString(3);
		KigsCore::GetNotificationCenter()->addObserver(obj, method, notif);
	}
	else if(L.isFunction(3))
	{
		std::vector<CMSP> insts;
		obj->GetSonInstancesByType("LuaNotificationHook", insts, false);
		
		auto hook_obj = std::find_if(insts.begin() ,insts.end(), [notif](CMSP& o){ return o->getName() == notif;});
		SP<LuaNotificationHook> hook = nullptr;
		if(hook_obj != insts.end())
		{
			hook = (*hook_obj);
		}
		else
		{
			hook = KigsCore::GetInstanceOf(notif, "LuaNotificationHook");
			obj->addItem(hook);
		}
		
		hook->mL = L;
		hook->mLuaFunc = L.toValue<LuaRef>(3);
		hook->mObj = obj;
		
		KigsCore::GetNotificationCenter()->addObserver(hook.get(), "CallLuaFunc", notif);
	}
	else
	{
		L.push("error");
		L.error();
	}
	return 0;
}



int CoreModifiableAddDynamicAttribute(CoreModifiable* obj, lua_State* lua)
{
	LuaState L = lua;
	const std::string name = L.checkString(2);
	int type = L.type(3);
	switch (type) {
		case LUA_TNUMBER:
		{
			char str[128];
			snprintf(str, 128, "%f", L.toNumber(3));
			obj->AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE::DOUBLE, name, str);
			break;
		}
		case LUA_TBOOLEAN:
		obj->AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE::BOOL, name, L.toBool(3));
		break;
		case LUA_TSTRING:
		obj->AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE::STRING, name, L.toString(3));
		break;
		case LUA_TUSERDATA:
		{
			LuaRef ref = L.toValue<LuaRef>(3);
			v2f* pt2 = nullptr;
			v3f* pt3 = nullptr;
			v4f* pt4 = nullptr;
			char str[128];
			if ((pt2 = LuaIntf::Lua::objectCast<v2f>(ref)))
			{
				snprintf(str, 128, "[%f,%f]", pt2->x, pt2->y);
			}
			else if((pt3 = LuaIntf::Lua::objectCast<v3f>(ref)))
			{
				snprintf(str, 128, "[%f,%f,%f]", pt3->x, pt3->y, pt3->z);
			}
			else if ((pt4 = LuaIntf::Lua::objectCast<v4f>(ref)))
			{
				snprintf(str, 128, "[%f,%f,%f,%f]", pt4->x, pt4->y, pt4->z, pt4->w);
			}
			else
				break;
			obj->AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE::ARRAY, name, str);
			break;
		}
		
		default:
		break;
	}
	return 0;
}




std::vector<CMSP> CoreModifiableByName(const char* name)
{
	return	CoreModifiable::GetInstancesByName("CoreModifiable", name);
}

std::vector<CMSP> CoreModifiableByType(const char* type)
{
	return CoreModifiable::GetInstances((std::string)type);
}

CMSP CoreModifiableGet(const char* name)
{
	return CoreModifiable::GetFirstInstanceByName("CoreModifiable", name);
}


std::vector<CMSP> CoreModifiableGetSonByType(CoreModifiable* obj, const char* type, bool rec)
{
	std::vector<CMSP> insts;
	obj->GetSonInstancesByType((std::string)type, insts, rec);
	return insts;
}


std::vector<CMSP> CoreModifiableGetSonByName(CoreModifiable* obj, const char* name, bool rec)
{
	std::vector<CMSP> insts;
	obj->GetSonInstancesByName("CoreModifiable", name, insts, rec);
	return insts;
}



std::vector<CoreModifiable*> CoreModifiableChildList(CoreModifiable* obj)
{
	std::vector<CoreModifiable*> result; result.reserve(obj->getItems().size());
	for(auto mis : obj->getItems())
	{
		result.push_back(mis.mItem.get());
	}
	return result;
}



DEFINE_DYNAMIC_METHOD(CoreModifiable, LuaReleaseCallbacks)
{
	SP<LuaKigsBindModule> lua = KigsCore::GetModule<LuaKigsBindModule>();
	if (!lua) return false;

	for (auto pair : getAttributes())
	{
		CoreModifiableAttribute* attr = pair.second;

		if (pair.first == "LUA_REF")
		{
			LuaState L = lua->getLuaState();
			int ref;
			if (attr->getValue(ref,this))
			{
				L.unref(ref);
			}
		}
	}
	return false;
}

LuaRef Kigs::Lua::GetModifiableLuaData(lua_State* lua, CoreModifiable* obj)
{
	LuaState L = lua;
	LuaStackAssert protect{ lua };
	int ref;
	if (obj->getValue("LUA_REF", ref))
	{
		L.getRef(ref); // push table
	}
	else
	{
		L.newTable(0, 1);  // push new table LUA_REF
		L.newTable(0, 0);  // push new table __methods
		L.setField(-2, "__methods"); // pop __methods
		L.pushValueAt(-1); // push table
		ref = L.ref(); // pops new table
		obj->AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE::INT, "LUA_REF", ref);
		obj->InsertMethod("LuaReleaseCallbacks", static_cast<CoreModifiable::ModifiableMethod>(&DynamicMethodLuaReleaseCallbacks::LuaReleaseCallbacks));
		KigsCore::Connect(obj, "Destroy", obj, "LuaReleaseCallbacks");
	}
	auto result = L.toValue<LuaRef>(-1);
	L.pop();  // pop table
	return result;
}


void QuickAnim(Kigs::Action::ModuleCoreAnimation* m, Kigs::Core::CoreModifiable* target, const char* str)
{
	auto seq = m->createSequenceFromString(target->SharedFromThis(), str);
	m->addSequence(seq.get());
	m->startSequenceAtFirstUpdate(seq.get());
}



template<typename Vec>
int LuaScalarMult(lua_State* lua)
{
	LuaState L = lua;
	
	LuaRef ref = L.toValue<LuaRef>(1);
	LuaRef ref2 = L.toValue<LuaRef>(2);
	auto left = LuaIntf::Lua::objectCast<Vec>(ref);
	auto right  = LuaIntf::Lua::objectCast<Vec>(ref2);
	if(left)
	{
		if(L.isNumber(2))
			L.push(*left * L.toNumber(2));
		else
		{
			char str[256];
			snprintf(str, 256, "Trying to call mutl a vector with a %s", L.typeNameAt(2));
			L.push(str);
			L.error();
		}
	}
	else if(right)
	{
		if(L.isNumber(1))
			L.push(L.toNumber(1)* *right);
		else
		{
			char str[256];
			snprintf(str, 256, "Trying to call mutl a vector with a %s", L.typeNameAt(1));
			L.push(str);
			L.error();
		}
	}
	else
	{
		L.push("error");
		L.error();
	}
	return 1;
}


int LuaMatrix4Mult(lua_State* lua)
{
	LuaState L = lua;
	
	LuaRef ref_left = L.toValue<LuaRef>(1);
	LuaRef ref_right = L.toValue<LuaRef>(2);
	
	auto left = LuaIntf::Lua::objectCast<const mat4>(ref_left);
	auto right = LuaIntf::Lua::objectCast<const mat4>(ref_right);
	
	
	if (left && right)
	{
		L.push(*left * *right);
		return 1;
	}
	
	if (left)
	{
		auto v = LuaIntf::Lua::objectCast<const v3f>(ref_right);
		if (v)
		{
			L.push(*left * v4f(*v,1.0f));
			return 1;
		}
		
		auto scalar = L.checkNumber(2);
		L.push(*left * scalar);
		return 1;
	}
	
	if (right)
	{
		auto scalar = L.checkNumber(1);
		L.push(scalar * *right);
		return 1;
	}
	L.push("error");
	L.error();
	return 0;
}


#include "GLSLDebugDraw.h"

class v2fExtention : public v2f
{
public:

	using v2f::operator=;

	bool operator==(const v2f& other) const
	{
		return (this->x==other.x) && (this->y == other.y);
	}

	v2f& normalize()
	{
		*this=glm::normalize(*this);
		return *this;
	}

	v2f normalized() const
	{
		return glm::normalize(*this);
	}
};

class v3fExtention : public v3f
{
public:

	using v3f::operator=;

	bool operator==(const v3f& other) const
	{
		return (this->x == other.x) && (this->y == other.y) && (this->z == other.z);
	}

	v3f& normalize()
	{
		*this = glm::normalize(*this);
		return *this;
	}

	v3f normalized() const
	{
		return glm::normalize(*this);
	}
};

class v4fExtention : public v4f
{
public:

	using v4f::operator=;

	bool operator==(const v4f& other) const
	{
		return (this->x == other.x) && (this->y == other.y) && (this->z == other.z) && (this->w == other.w);
	}

	v4f& normalize()
	{
		*this = glm::normalize(*this);
		return *this;
	}

	v4f normalized() const
	{
		return glm::normalize(*this);
	}
};

void Kigs::Lua::setup_bindings(lua_State* lua)
{
	LuaState L = lua;
	
	LuaBinding(L).beginClass<CMSP>("CMSP")
		.addConstructor(LUA_ARGS())
		.addFunction("get", [](const CMSP* v) -> CoreModifiable* { return v->get(); })
		.endClass();

	LuaBinding(L).beginClass<v2f>("v2f")
		.addConstructor(LUA_ARGS(float, float))
		.addVariable("x", &v2f::x, true)
		.addVariable("y", &v2f::y, true)
		.addFunction("__add", static_cast<v2f & (v2f::*)(const v2f & other)>(&v2f::operator+=))
		.addFunction("__sub", static_cast<v2f & (v2f::*)(const v2f & other)>(&v2f::operator-=))
		.addMetaFunction("__unm", [](const v2f& other)->v2f {return v2f{ -other.x,-other.y }; })
		.addFunction("__eq", static_cast<bool (v2f::*)(const v2f & other) const>(&v2fExtention::operator==))
		.addMetaFunction("__mul", &LuaScalarMult<v2f>)
		.addMetaFunction("__div", [](const v2f& other, float d)->v2f {return v2f{ other.x/d,other.y/d }; })
		.addStaticFunction("Dot", LUA_FN(float, glm::dot, const v2f&, const v2f&))
		.addStaticFunction("Cross", [](const v2f& first, const v2f& second)->float {return first.x*second.y-first.y*second.x; })
		.addStaticFunction("Norm", LUA_FN(float, glm::length, const v2f&))
		.addStaticFunction("length2", LUA_FN(float, glm::length2, const v2f&))
		.addFunction("normalize", static_cast<v2f & (v2f::*)()>(&v2fExtention::normalize))
		.addFunction("normalized", static_cast<v2f  (v2f::*)() const>(&v2fExtention::normalized))
		.addFunction("copy", static_cast<v2f & (v2f::*)(const v2f & other)>(&v2f::operator=))
		.endClass();
	
	
	LuaBinding(L).beginClass<v3f>("v3f")
		.addConstructor(LUA_ARGS(float, float, float))
		.addVariable("x", &v3f::x, true)
		.addVariable("y", &v3f::y, true)
		.addVariable("z", &v3f::z, true)
	
		.addVariable("xy", &v3f::x, true)
		.addVariable("yz", &v3f::y, true)
	
		.addFunction("__add", static_cast<v3f & (v3f::*)(const v3f & other)>(&v3f::operator+=))
		.addFunction("__sub", static_cast<v3f & (v3f::*)(const v3f & other)>(&v3f::operator-=))
		.addMetaFunction("__unm", [](const v3f& other)->v3f {return v3f{ -other.x,-other.y, -other.z }; })
		.addFunction("__eq", static_cast<bool (v3f::*)(const v3f & other) const>(&v3fExtention::operator==))
		.addMetaFunction("__mul", &LuaScalarMult<v3f>)
		.addMetaFunction("__div", [](const v3f& other, float d)->v3f {return v3f{ other.x / d,other.y / d,other.z / d }; })
		.addStaticFunction("Dot", LUA_FN(float, glm::dot, const v3f&, const v3f&))
		.addStaticFunction("Cross", [](const v3f& first, const v3f& second)->v3f {return glm::cross(first, second); })
		.addStaticFunction("Norm", LUA_FN(float, glm::length, const v3f&))
		.addStaticFunction("length2", LUA_FN(float, glm::length2, const v3f&))
		.addFunction("normalize", static_cast<v3f & (v3f::*)()>(&v3fExtention::normalize))
		.addFunction("normalized", static_cast<v3f(v3f::*)() const>(&v3fExtention::normalized))
		.addFunction("copy", static_cast<v3f & (v3f::*)(const v3f & other)>(&v3f::operator=))
	
		.endClass();
	
	
	LuaBinding(L).beginClass<v4f>("v4f")
		.addConstructor(LUA_ARGS(float, float, float, float))
		.addVariable("x", &v4f::x, true)
		.addVariable("y", &v4f::y, true)
		.addVariable("z", &v4f::z, true)
		.addVariable("w", &v4f::w, true)
	
		.addVariable("xyz", &v4f::x, true)
		.addVariable("yzw", &v4f::y, true)
	
		.addFunction("__add", static_cast<v4f & (v4f::*)(const v4f & other)>(&v4f::operator+=))
		.addFunction("__sub", static_cast<v4f & (v4f::*)(const v4f & other)>(&v4f::operator-=))
		.addMetaFunction("__unm", [](const v4f& other)->v4f {return v4f{ -other.x,-other.y, -other.z, -other.w }; })
		.addFunction("__eq", static_cast<bool (v4f::*)(const v4f & other) const>(&v4fExtention::operator==))
		.addMetaFunction("__mul", &LuaScalarMult<v4f>)
		.addMetaFunction("__div", [](const v4f& other, float d)->v4f {return v4f{ other.x / d,other.y / d,other.z / d,other.w / d }; })
		.addStaticFunction("Dot", LUA_FN(float, glm::dot, const v4f&, const v4f&))
		.addStaticFunction("Norm", LUA_FN(float, glm::length, const v4f&))
		.addStaticFunction("length2", LUA_FN(float, glm::length2, const v4f&))
		.addFunction("normalize", static_cast<v4f & (v4f::*)()>(&v4fExtention::normalize))
		.addFunction("normalized", static_cast<v4f(v4f::*)() const>(&v4fExtention::normalized))
		.addFunction("copy", static_cast<v4f & (v4f::*)(const v4f & other)>(&v4f::operator=))

		.endClass();
	
	LuaBinding(L).beginClass<mat4>("mat4")
		.addFactory([]() { mat4 result(1.0f); return result; })

		// TODO
		/*.addVariable("XAxis", &mat4:::XAxis)
		.addVariable("YAxis", &mat4::YAxis)
		.addVariable("ZAxis", &mat4::ZAxis)
		.addVariable("Pos", &mat4::Pos)


		.addMetaFunction("__mul", &LuaMatrix3x4Mult)

		.addMetaFunction("__add", [](const mat4* m1, const mat4* m2) -> mat4 { return *m1 + *m2; })
		.addMetaFunction("__sub", [](const mat4* m1, const mat4* m2) -> mat4 { return *m1 - *m2; })

		.addFunction("setRotationX", &mat4::SetRotationX)
		.addFunction("setRotationY", &mat4::SetRotationY)
		.addFunction("setRotationZ", &mat4::SetRotationZ)

		.addFunction("preRotateX", &mat4::PreRotateX)
		.addFunction("preRotateY", &mat4::PreRotateY)
		.addFunction("preRotateZ", &mat4::PreRotateZ)

		.addFunction("postRotateX", &mat4::PostRotateX)
		.addFunction("postRotateY", &mat4::PostRotateY)
		.addFunction("postRotateZ", &mat4::PostRotateZ)

		.addFunction("setScale", &mat4::SetScale)
		.addFunction("preScale", &mat4::PreScale)
		.addFunction("postScale", &mat4::PostScale)

		.addFunction("setTranslation", &mat4::SetTranslation)
		.addFunction("preTranslate", &mat4::PreTranslate)
		.addFunction("postTranslate", &mat4::PostTranslate)

		.addFunction("setRotationXYZ", &mat4::SetRotationXYZ)
		.addFunction("setRotationZYX", &mat4::SetRotationZYX)

		.addFunction("preRotateXYZ", &mat4::PreRotateXYZ)
		.addFunction("preRotateZYX", &mat4::PreRotateZYX)
		.addFunction("postRotateXYZ", &mat4::PostRotateXYZ)
		.addFunction("postRotateZYX", &mat4::PostRotateZYX)

		.addFunction("setIdentity", &mat4::SetIdentity)
		.addFunction("setNull", &mat4::SetNull)

		.addFunction("isIdentity", &glm::isIdentity)*/
		/*
   .addStaticFunction("Inv", LUA_FN(mat4, Inv, const mat4&))
   .addStaticFunction("Det", LUA_FN(float, Det, const mat4&))
   .addStaticFunction("Norm", LUA_FN(float, Norm, const mat4&))
   .addStaticFunction("length2", LUA_FN(float, length2, const mat4&))
   .addStaticFunction("Trace", LUA_FN(float, Trace, const mat4&))
   */
	.addFunction("transformPoint", [](const mat4* m, v3f* v) { *v = transformPoint3(*m,*v); })
		.addFunction("transformPoints", [](const mat4* m, const std::vector<v3f*> l) { for (auto v : l)  *v=transformPoint3(*m,*v); })
		.addFunction("transformVector", [](const mat4* m, v3f* v) { *v=transformVector3(*m,*v); })
		.addFunction("transformVectors", [](const mat4* m, const std::vector<v3f*> l) { for (auto v : l) *v=transformVector3(*m,*v); })
	
		.addFunction("copy", [](const mat4* m) -> mat4 { return *m; })
	
	
		.endClass();
	
	auto cm = LuaBinding(L).beginClass<CoreModifiable>("CoreModifiable")
		.addFactory(&factory, LUA_ARGS(std::string, std::string)) 
		//.addStaticFunction("Import", LUA_FN(CoreModifiable*, CoreModifiable::Import, const std::string&, bool, bool), LUA_ARGS(const std::string&, _opt<bool>, _opt<bool>))
		.addFunction("name", &CoreModifiable::getName)
		.addFunction("addItem", &CoreModifiable::addItem, LUA_ARGS(const CMSP&, _def<CoreModifiable::ItemPosition, CoreModifiable::Last>))
		.addFunction("removeItem", &CoreModifiable::removeItem, LUA_ARGS(const CMSP&))
		.addFunction("aggregateWith", &CoreModifiable::aggregateWith, LUA_ARGS(const CMSP&, _def<CoreModifiable::ItemPosition, CoreModifiable::Last>))
		.addFunction("removeAggregateWith", &CoreModifiable::removeAggregateWith, LUA_ARGS(const CMSP&) )
		.addFunction("parents", &CoreModifiable::GetParents)
		.addFunction("childs", &CoreModifiableChildList)
		.addFunction("items", &CoreModifiableChildList)
		.addFunction("getSonsByType", &CoreModifiableGetSonByType, LUA_ARGS(const char*, _opt<bool>))
		.addFunction("getSonsByName", &CoreModifiableGetSonByName, LUA_ARGS(const char*, _opt<bool>))
		.addFunction("init", &CoreModifiable::Init)
		.addFunction("uid", &CoreModifiable::getUID)
		.addFunction("type", &CoreModifiable::getExactType)
		.addFunction("isSubType", [](CoreModifiable* obj, const char* type) { return obj->isSubType((std::string)type); })
		.addFunction("addAttribute", &CoreModifiableAddDynamicAttribute)
		.addFunction("onEvent", &CoreModifiableAddHook)
		.addFunction("getByPath", [](CoreModifiable* obj, const char* path) { return CoreModifiable::SearchInstance(path, obj); })
		.addFunction("getLuaData", [](CoreModifiable* obj, lua_State* lua)
		{
			LuaRef ref = GetModifiableLuaData(lua, obj);
			ref.pushToStack();
			return 1;
		})
		.addFunction("emit", &CoreModifiableEmit)
		.addFunction("importAsSon", [](CoreModifiable* obj, const char* path)
		{
			auto cm = CoreModifiable::Import(path);
			if (cm)
				obj->addItem(cm);
			return cm.get();
		});
	
	
	LuaRef meta = cm.meta();
	LuaRef ___class = meta.rawget("___class");
	___class.pushToStack();
	L.push("___get_indexed");
	L.pushCFunction(&CoreModifiableGetAttributeLua);
	lua_rawset(L, -3);
	
	L.push("___set_indexed");
	L.pushCFunction(&CoreModifiableSetAttributeLua);
	lua_rawset(L, -3);
	
	L.pop();
	
	cm.endClass();

#define BEGIN_INPUT_EVENT_BINDING(name)\
	gLuaTypeMap[#name] = #name;\
	LuaBinding(L).beginClass<name>(#name)\
	.addFactory([](LuaState L) { return (name*)L.toUserData(1); })\
	.addPropertyReadOnly("position", [](name* ev){ return ev->position;})\
	.addPropertyReadOnly("state", [](name* ev){ return ev->state;})\
	.addPropertyReadOnly("type", [](name* ev){ return ev->type;})\
	.addPropertyReadOnly("touch_id", [](name* ev){ return ev->touch_id;})\
	.addFunction("swallowAll", [](name* ev) { *ev->swallow_mask = 0xFFFFFFFF; })\
	.addFunction("swallowEvent", [](name* ev, int type) { *ev->swallow_mask |= 1 << type; })
	
	BEGIN_INPUT_EVENT_BINDING(ClickEvent)
		.addVariableRef("button_state_mask", &ClickEvent::button_state_mask)
		.addVariableRef("click_count", &ClickEvent::click_count)
		.endClass();

	BEGIN_INPUT_EVENT_BINDING(SwipeEvent)
		.addVariableRef("direction", &SwipeEvent::direction)
		.endClass();

	BEGIN_INPUT_EVENT_BINDING(ScrollEvent)
		.addVariableRef("main_direction", &ScrollEvent::main_direction)
		.addVariableRef("offset", &ScrollEvent::offset)
		.addVariableRef("speed", &ScrollEvent::speed)
		.addVariableRef("start_position", &ScrollEvent::start_position)
		.endClass();

	BEGIN_INPUT_EVENT_BINDING(PinchEvent)
		.addVariableRef("p1", &PinchEvent::p1)
		.addVariableRef("p2", &PinchEvent::p2)
		.addVariableRef("p1_start", &PinchEvent::p1_start)
		.addVariableRef("p2_start", &PinchEvent::p2_start)
		.endClass();
	
	BEGIN_INPUT_EVENT_BINDING(DirectTouchEvent)
		.addVariableRef("touch_state", &DirectTouchEvent::touch_state)
		.addConstant("TouchHover", DirectTouchEvent::TouchState::TouchHover)
		.addConstant("TouchDown", DirectTouchEvent::TouchState::TouchDown)
		.addConstant("TouchUp", DirectTouchEvent::TouchState::TouchUp)
		.endClass();


	LuaBinding(L).beginExtendClass<Input::KeyboardDevice, CoreModifiable>("KeyboardDevice")
		.addFunction("getKey", [](Input::KeyboardDevice* kb, int key) -> bool { return kb->getKeyState(key).GetTypedValue(int);  })
		.endClass();
	
	
	LuaBinding(L).beginExtendClass<MouseDevice, CoreModifiable>("MouseDevice")
		.addFunction("getButton", [](MouseDevice* mouse, int button) -> bool { return mouse->getButtonState((MouseDevice::MOUSE_BUTTONS)button); })
		.addFunction("getPos", [](MouseDevice* mouse) { v2f pos; mouse->getPos(pos.x, pos.y); return pos; })
		.endClass();
	
	LuaBinding(L).beginExtendClass<Node2D, CoreModifiable>("Node2D")
		.addFunction("getFather", &Node2D::getFather)
		.addFunction("isInClip", &Node2D::IsInClip)
		.addFunction("isHiddenFlag", &Node2D::IsHiddenFlag)
		.endClass();

	LuaBinding(L).beginExtendClass<UIItem, Node2D>("UIItem")
		.addFunction("canInteract", &UIItem::CanInteract)
		.addFunction("containsPoint", [](UIItem* item, v2f pos) { return item->ContainsPoint(pos.x, pos.y); })
		.endClass();

	LuaBinding(L).beginExtendClass<ModuleCoreAnimation, CoreModifiable>("ModuleCoreAnimation")
		.addFunction("quickAnim", &QuickAnim)
		.endClass();

	gLuaTypeMap["CoreSequence"] = "CoreSequence";
	LuaBinding(L).beginClass<CoreSequence>("CoreSequence")
		.addFactory([](LuaState L) { return (CoreSequence*)L.toUserData(1); })\
		.addFunction("getTarget", &CoreSequence::getTarget)
		.addFunction("getID", [](CoreSequence* seq) { return (unsigned int)seq->getID().toUInt(); })
		.endClass();

	LuaBinding(L).beginExtendClass<Node3D, CoreModifiable>("Node3D")
	
		.addProperty("matrix", &Node3D::GetLocal, &Node3D::ChangeMatrix)
	
		.addFunction("getLocal", &Node3D::GetLocal)
		.addFunction("changeMatrix", &Node3D::ChangeMatrix)
		.addFunction("getLocalToGlobal", &Node3D::GetLocalToGlobal)
		.addFunction("getGlobalToLocal", &Node3D::GetGlobalToLocal)
	
		//.addFunction("setPosition", [](Node3D* node, v3f* v) { node->setPosition(v->x, v->y, v->z); })
		//.addFunction("getPosition", [](Node3D* node) -> v3f { return node->GetLocal().GetTranslation(); })
	
		.endClass();
	
	LuaBinding(L).beginModule("iterator")
		.addFunction("childs", &xpairs_childs)
		.endModule();
	
	LuaBinding(L).beginModule("core")
		.addFunction("ByName", &CoreModifiableByName)
		.addFunction("ByType", &CoreModifiableByType)
		.addFunction("GetFirstInstanceByName", &CoreModifiableGet)
		.addFunction("GetModule", [](const char* id) { return (CoreModifiable*)KigsCore::GetModule((std::string)id).get(); })
		.addFunction("PostNotification", [](const char* notif, CoreModifiable* sender, CoreModifiable* data) 
					 {
					 KigsCore::GetNotificationCenter()->postNotificationName((std::string)notif, sender, data);
					 }, LUA_ARGS(const char*, _opt<CoreModifiable*>, _opt<CoreModifiable*>))
	
	
		.addFunction("AddAutoUpdate", [](CoreModifiable* obj)
					 {
					 KigsCore::GetCoreApplication()->AddAutoUpdate(obj);
					 })
		.addFunction("RemoveAutoUpdate", [](CoreModifiable* obj)
					 {
					 KigsCore::GetCoreApplication()->RemoveAutoUpdate(obj);
					 })
		.addFunction("SID", [](const char* str) { return CharToID::GetID((std::string_view)str); })
		.addFunction("Connect", [](CoreModifiable* s, const char* si, CoreModifiable* r, const char* sl) { KigsCore::Connect(s, (std::string_view)si, r, (std::string_view)sl); })
		.addFunction("Disconnect", [](CoreModifiable* s, const char* si, CoreModifiable* r, const char* sl) { KigsCore::Disconnect(s, (std::string_view)si, r, (std::string_view)sl); })
		.addFunction("Import", [](const char* filename) { return CoreModifiable::Import(filename); })
		.endModule();


	LuaBinding(L).beginModule("input")
		.beginModule("state")
		.addConstant("Possible", StatePossible)
		.addConstant("Began", StateBegan)
		.addConstant("Changed", StateChanged)
		.addConstant("Ended", StateEnded)
		.addConstant("Cancelled", StateCancelled)
		.addConstant("Failed", StateFailed)
		.addConstant("Recognized", StateRecognized)
		.endModule()
		.beginModule("event_type")
		.addConstant("Click", InputEventType::Click)
		.addConstant("Swipe", InputEventType::Swipe)
		.addConstant("Pinch", InputEventType::Pinch)
		.addConstant("Scroll", InputEventType::Scroll)
		.addConstant("DragAndDrop", InputEventType::DragAndDrop)
		.addConstant("DirectTouch", InputEventType::DirectTouch)
		.addConstant("InputSwallow", InputEventType::InputSwallow)
		.addConstant("UserDefinedStart", InputEventType::UserDefinedStart)
		.endModule()
		.endModule();
	
	
#ifdef LUA_IMGUI
	LuaBinding(L).beginModule("dd")
		.addFunction("line", [](v3f from, v3f to, v3f color, int duration) { dd::line(from, to, color, duration, true); })
		.addFunction("sphere", [](v3f center, float radius, v3f color, int duration) { dd::sphere(center, color, radius, duration, true); })
		.addFunction("xzSquareGrid", [](float mins, float maxs, float y, float step, v3f color, int duration=0) {
					 dd::xzSquareGrid(mins, maxs, y, step, color, duration, true);
					 })
		.endModule();
#endif
}

