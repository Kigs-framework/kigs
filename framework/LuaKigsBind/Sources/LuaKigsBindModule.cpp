#include "PrecompiledHeaders.h"
#include "LuaKigsBindModule.h"
#include "Timer.h"

#include "LuaBehaviour.h"
#include "LuaImporter.h"

#include <FilePathManager.h>
#include "lua_kigs.h"
//#include "lua_vector.h"
#include <NotificationCenter.h>

#include <ModuleFileManager.h>


#include "LuaBindings.h"

//#include "ltable.h"

#include <algorithm>


using namespace LuaIntf;

bool gLuaImGuiProtected = false;

IMPLEMENT_CLASS_INFO(LuaKigsBindModule);
#include <deque>


std::deque<int> ImGuiEndStack;
LuaImGuiStackProtector::LuaImGuiStackProtector() 
{
#ifdef LUA_IMGUI
	is_protecting = false;
	if (!gLuaImGuiProtected)
	{
		gLuaImGuiProtected = true;
		is_protecting = true;
		ImGuiEndStack.clear();
	}
#endif
}

void ImEndStack(int);

LuaImGuiStackProtector::~LuaImGuiStackProtector()
{
#ifdef LUA_IMGUI
	if (!is_protecting) return;
	
	if (!ImGuiEndStack.empty())
	{
		KIGS_WARNING("ImGui stack was not empty at the end of the lua script", 0);
	}
	while (!ImGuiEndStack.empty())
	{
		ImEndStack(ImGuiEndStack.back());
		ImGuiEndStack.pop_back();
	}
	gLuaImGuiProtected = false;
#endif
}



IMPLEMENT_CONSTRUCTOR(LuaKigsBindModule)
, myLuaState(nullptr)
, myNeedResort(false)
{
}

// function GetFile(path)
int LuaKigsBindModule::GetFile(lua_State *L) 
{
	kstl::string path = luaL_checkstring(L, 1);
	for (unsigned i = 0; i<path.length(); ++i)
	{
		if (path[i] == '.') path[i] = '/';
	}
	path += ".lua";
	
	u64 len;
	CoreRawBuffer* buffer = ModuleFileManager::LoadFileAsCharString(path.c_str(), len);
	if (buffer) 
	{
		luaL_loadstring(L, buffer->buffer());
		buffer->Destroy();
	}
	else
	{
		kstl::string err = "Could not load : ";
		err += path;
		lua_pushstring(L, err.c_str());
	}
	return 1;
}

int KigsLoadFile(lua_State *L)
{
	std::string path = luaL_checkstring(L, 1);
	u64 len;
	CoreRawBuffer* buffer = ModuleFileManager::LoadFileAsCharString(path.c_str(), len);
	if (buffer)
	{
		if (luaL_loadstring(L, buffer->buffer()) != 0)
		{
			KIGS_MESSAGE(lua_tostring(L, -1));
			lua_pop(L, 1);
			lua_pushnil(L);
		}
		buffer->Destroy();
	}
	else
	{
		lua_pushnil(L);
	}
	return 1;
}

int KigsDoFile(lua_State *L)
{
	KigsLoadFile(L);
	
	LuaIntf::LuaState l = L;
	auto result = l.popValue<LuaRef>();

	if (result)
	{
		auto topA = l.top();
		result.pushToStack();
		if (lua_pcall(L, 0, LUA_MULTRET, 0) == 0)
		{
			int nb_results = l.top() - topA;
			return nb_results;
		}
		else
		{
			KIGS_MESSAGE(lua_tostring(L, -1));
			lua_pop(L, 1);
			lua_error(L);
			return 0;
		}
	}
	else
	{
		lua_error(L);
		return 0;
	}
}


// global lua function
bool	LuaKigsBindModule::ExecuteString(const char* code)
{
	LuaImGuiStackProtector protector;
	if(luaL_dostring(myLuaState, code) != 0)
	{
		kigsprintf("%s\n",lua_tostring(myLuaState, -1));
		lua_pop(myLuaState, 1);
		return false;
	}
	return true;
}

bool	LuaKigsBindModule::ExecuteString(const kstl::string& funcCode)
{
	return ExecuteString(funcCode.c_str());
}


bool LuaKigsBindModule::ExecuteLuaFile(const char* filename, const char* prepend)
{
	FilePathManager*	pathManager = (FilePathManager*)KigsCore::GetSingleton("FilePathManager");
	SmartPointer<FileHandle> lFile = pathManager->FindFullName(filename);

	if ((lFile->myStatus&FileHandle::Exist)==0)
		return false;

	u64 length;
	CoreRawBuffer* buffer = ModuleFileManager::LoadFileAsCharString(filename, length);
	if (buffer)
	{
		std::string str = (char*)buffer->buffer();
		if (prepend)
			str = prepend + str;
		bool ok = ExecuteString(str.c_str());
		buffer->Destroy();

		if (!ok)
		{
			char str[256];
			snprintf(str, 256, "Cannot execute lua file : %s", filename);
			kigsprintf("%s\n", str);
			//KIGS_WARNING(str, 0);
		}
		return ok;
	}
	else
	{
		char str[256];
		snprintf(str, 256, "Cannot find lua file to execute : %s", filename);
		KIGS_WARNING(str, 0);
		return false;
	}
}


bool    LuaKigsBindModule::SafePCall(int nb_args, int nb_ret)
{
	LuaImGuiStackProtector protector;
	if (lua_pcall(myLuaState, nb_args, nb_ret, 0) != 0)
	{
		KIGS_MESSAGE(lua_tostring(myLuaState, -1));
		lua_pop(myLuaState, 1);
		return false;
	}
	return true;
}

bool	LuaKigsBindModule::CallLuaFunction(const kstl::string& funcName)
{
	lua_getglobal(myLuaState, funcName.c_str());
	return SafePCall(0, 0);
}

bool	LuaKigsBindModule::CallCoreModifiableCallback(kstl::vector<CoreModifiableAttribute*>& params, CoreModifiable* localthis)
{
	if (params.size())
	{
		kstl::string funcName = ((maString*)params[0])->const_ref();
		LuaState L = myLuaState;
		
		LuaStackAssert protect{ L };

		int ref;
		if(!localthis->getValue("LUA_REF", ref))
			return false;
		
		L.getRef(ref);
		L.toValue<LuaRef>(-1)["__methods"].value().pushToStack(); // push method table
		L.remove(-2); // remove ref from stack

		int top = L.top();

		L.getField(-1, funcName.c_str());
		
		if(L.isNil(-1))
		{
			L.pop(2); // pop nil and method table
			return false;
		}
		
		L.push(localthis);
		
		// check for result
		CoreModifiableAttribute*	result=0;
		
		int paramCount = 1; // at least one param for this
		int returnCount = 0;
		// push params but not result
		kstl::vector<CoreModifiableAttribute*>::iterator	itparam = params.begin() + 1;
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
				PushAttribute(myLuaState, *itparam);
				paramCount++;
			}
			++itparam;
		}


#ifdef _DEBUG
		//printf("Calling lua %s on %s\n", funcName.c_str(), localthis->getName().c_str());
#endif

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


static char CustomLuaPrintBuffer[8192];

static int CustomLuaPrint(lua_State *L)
{
	int n = lua_gettop(L);  /* number of arguments */
	int i;
	lua_getglobal(L, "tostring");

	char* ptr = CustomLuaPrintBuffer;
	
	for (i = 1; i <= n; i++)
	{
		const char *s;
		size_t l;
		lua_pushvalue(L, -1);  /* function to be called */
		lua_pushvalue(L, i);   /* value to print */
		lua_call(L, 1, 1);
		s = lua_tolstring(L, -1, &l);  /* get result */
		if (s == NULL)
			return luaL_error(L, "'tostring' must return a string to 'print'");
		if (i > 1) *ptr++ = '\t';
		
		while (*s)
		{
			*ptr++ = *s++;
		}
		lua_pop(L, 1);  /* pop result */
	}
	*ptr++ = '\n';
	*ptr = 0;
	printf(CustomLuaPrintBuffer);

	return 0;
}


void LuaKigsBindModule::LoadLibraries(lua_State* L) 
{	
	luaL_openlibs(L);
	
	setup_bindings(L);
	kstl::vector<LuaCLibrary>::iterator itlib = myLibraries.begin();
	while (itlib != myLibraries.end()) 
	{
		(*itlib).registerLibrary(L);
		itlib++;
	}

	lua_getglobal(L, "_G");
	lua_pushcfunction(L, CustomLuaPrint);
	lua_setfield(L, -2, "print");
	lua_pop(L, 1);

	if (luaL_dostring(L, (const char*)kigsLuaModule) != 0) 
	{
		KIGS_MESSAGE(lua_tostring(L, -1));
	}
	lua_pop(L, 1);
	
	lua_getglobal(L, "package");	// push "package"
	lua_getfield(L, -1, "searchers");					// push "package.searchers"

	lua_pushcfunction(L, &LuaKigsBindModule::GetFile);
	lua_rawseti(L, -2, 2);

	// Table is still on the stack.  Get rid of it now.	
	lua_pop(L, 2);


	lua_getglobal(L, "_G");
	lua_pushcfunction(L, &KigsLoadFile);
	lua_setfield(L, -2, "loadfile");
	lua_pop(L, 1);

	lua_getglobal(L, "_G");
	lua_pushcfunction(L, &KigsDoFile);
	lua_setfield(L, -2, "dofile");
	lua_pop(L, 1);
	
}

//! destructor
LuaKigsBindModule::~LuaKigsBindModule()
{
	/*if (dummynode_)
	{
	 delete dummynode_;
	 dummynode_ = 0;
	}*/
}


void LuaKigsBindModule::AddToAutoUpdate(LuaBehaviour* script) 
{
	myAutoUpdateToAddSet.insert(script);
	myAutoUpdateToRemoveSet.erase(script);
}

void LuaKigsBindModule::RemoveFromAutoUpdate(LuaBehaviour* script) 
{	
	myAutoUpdateToRemoveSet.insert(script);
	myAutoUpdateToAddSet.erase(script);
}

void LuaKigsBindModule::AddLibrary(void(*func)(lua_State*)) 
{
	myLibraries.push_back(LuaCLibrary(func));
	if (myLuaState)
		func(myLuaState);
}


void LuaKigsBindModule::AddToInit(LuaBehaviour* script) 
{
	myToInitSet.push_back(script);
}

void LuaKigsBindModule::RemoveFromInit(LuaBehaviour* script) 
{
	myToInitSet.erase(std::remove_if(myToInitSet.begin(), myToInitSet.end(), [script](LuaBehaviour* a) { return a == script; }), myToInitSet.end());
}




//! module init, register CollisionManager and BSphere objects
void LuaKigsBindModule::Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
    BaseInit(core,"LuaBind",params);

	DECLARE_FULL_CLASS_INFO(core, LuaBehaviour, LuaBehaviour, LuaBind);
	DECLARE_FULL_CLASS_INFO(core, LuaImporter, LuaImporter, LuaBind);
	DECLARE_FULL_CLASS_INFO(core, LuaNotificationHook, LuaNotificationHook, LuaBind);

	myLuaState = luaL_newstate();
	
	LoadLibraries(myLuaState);

}

//! module close
void LuaKigsBindModule::Close()
{
    BaseClose();
	kstl::set<CoreModifiable*> instances;
	CoreModifiable::GetInstances("LuaBehaviour", instances);
	kstl::set<CoreModifiable*>::iterator it = instances.begin();
	while (it != instances.end()) {
		LuaBehaviour* behaviour = (LuaBehaviour*)*it;
		behaviour->UninitModifiable();
		it++;
	}
	
	if (myLuaState)
		lua_close(myLuaState);
	myLuaState = 0;
}    

//! module update
void LuaKigsBindModule::Update(const Timer& timer,void* addParam)
{
	
	BaseUpdate(timer, addParam);
	// Add to auto update
	kstl::set < LuaBehaviour* >::iterator it2 = myAutoUpdateToAddSet.begin();
	while (it2 != myAutoUpdateToAddSet.end()) 
	{
		myBehaviourSet.push_back(*it2);
		it2++;
	}
	myNeedResort = myNeedResort || !myAutoUpdateToAddSet.empty();
	myAutoUpdateToAddSet.clear();
	
	// Remove from auto update
	it2 = myAutoUpdateToRemoveSet.begin();
	kstl::vector<LuaBehaviour*>::iterator current_end = myBehaviourSet.end();
	while (it2 != myAutoUpdateToRemoveSet.end()) 
	{
		current_end = std::remove_if(myBehaviourSet.begin(), myBehaviourSet.end(), [&](LuaBehaviour* a) { return a == *it2; });
		it2++;
	}
	myBehaviourSet.erase(current_end, myBehaviourSet.end());
	// Removing from auto update doesn't change the relative order of the remaining items
	myAutoUpdateToRemoveSet.clear();
	
	auto sorter = [](LuaBehaviour* a1, LuaBehaviour* a2)
	{
		if (a1->GetPriority() == a2->GetPriority())
			return (a1) < (a2);
		return a1->GetPriority() < a2->GetPriority();
	};
	// Resort script order
	if (myNeedResort) 
	{
		myNeedResort = false;
		std::sort(myBehaviourSet.begin(), myBehaviourSet.end(), sorter);
	}
	
	std::sort(myToInitSet.begin(), myToInitSet.end(), sorter);
	// Init scripts
	kstl::vector<LuaBehaviour*>::iterator itset = myToInitSet.begin();
	while (itset != myToInitSet.end())
	{
		(*itset)->InitLua(timer.GetTime());
		itset++;
	}
	myToInitSet.clear();
	
	
	// auto update
	Timer& t = (Timer&)timer;
	if (t.GetState() == Timer::NORMAL) 
	{
		itset = myBehaviourSet.begin();
		while (itset != myBehaviourSet.end())
		{
			(*itset)->CallUpdate(timer,addParam);
			itset++;
		}
	}
	
}



void LuaKigsBindModule::ReleaseRefs(CoreModifiable* obj)
{
	
}




void LuaKigsBindModule::AddLuaMethodFromStack(CoreModifiable* obj, const kstl::string& func_name)
{
	LuaState L = myLuaState;
	{
		LuaStackAssert protect{ L };

		LuaRef ref = GetModifiableLuaData(L, obj);
		LuaRef methods = ref["__methods"];

		methods.pushToStack();
		L.pushValueAt(-2); // push func
		L.setField(-2, func_name.c_str()); // pop func
		L.pop(); // pop ref table
	}
	L.pop(); // Pop function on the top of the stack;
}

void LuaKigsBindModule::AddLuaMethod(CoreModifiable* obj, const kstl::string& func_name, const kstl::string& code)
{
	if(ExecuteString(code))
	{
		AddLuaMethodFromStack(obj, func_name);
#ifdef KIGS_TOOLS
		char str[256];
		snprintf(str, 256, "%d$LUA_CODE$%s", CharToID::GetID(func_name), func_name.c_str());
		obj->AddDynamicAttribute(STRING, str, code.c_str());
#endif
	}
}

void LuaKigsBindModule::ConnectToLua(CoreModifiable* e, const kstl::string& signal, CoreModifiable* r, const kstl::string& code)
{
	AddLuaMethod(r, code, code);
	r->InsertMethod(code, static_cast<RefCountedClass::ModifiableMethod>(&DynamicMethodLuaGlobalCallback::LuaGlobalCallback), code);
	KigsCore::Connect(e, signal, r, code);
}

void LuaKigsBindModule::SetValueLua(CoreModifiable* cm, const kstl::string& attrname, const kstl::string& code)
{
	LuaState L = myLuaState;
	LuaStackAssert protect{ L };
	int topA = L.top();
	L.pushCFunction(&CoreModifiableSetAttributeLua);

	L.push(cm);
	L.push(attrname);

	int topB = L.top();

	ExecuteString(code.c_str());
	int nb_result_code = L.top() - topB;

	if (nb_result_code != 1)
	{
		STACK_STRING(str, 1024, "Attribute %s on %s: lua code must return exactly one value", attrname.c_str(), cm->getName().c_str());
		KIGS_WARNING(str,0);
		L.pop(3 + nb_result_code);
	}
	else
	{
		if (SafePCall(3, LUA_MULTRET))
		{
			int nb_results = L.top() - topA;
			if (nb_results)
			{
				STACK_STRING(str, 1024, "Attribute %s on %s: attribute not found", attrname.c_str(), cm->getName().c_str());
				KIGS_WARNING(str,0);
				L.pop();
			}
		}
	}	
}

DEFINE_METHOD(LuaKigsBindModule, RegisterLuaMethod)
{
	if (params.size() > 2)
	{
		bool	isGlobal = false; // manage global lua code

		maString* privateName = ((maString*)getParameter(params, "pName"));
		if (!privateName)
		{
			return false;
		}
		maString* privateCode = ((maString*)getParameter(params, "pCode"));
		if (!privateCode)
		{
			return false;
		}
		maReference* target = ((maReference*)getParameter(params, "pThis"));
		if (!target)
		{
			return false;
		}

		
#ifdef KIGS_TOOLS
		XMLNodeBase* xmlattr = nullptr;
		maRawPtr* xml = (maRawPtr*)getParameter(params, "pXML");
		if (xml)
		{
			void* val = nullptr;
			xml->getValue(val);
			xmlattr = (XMLNodeBase*)val;
		}
#endif
		maString* cbType = ((maString*)getParameter(params, "cbType"));
		CoreModifiable* coreModifiableTarget = (CoreModifiable*) &(target->const_ref());

		if (cbType) // callback type is given, so attach to the good one
		{
			kstl::string callbackType = (cbType)->const_ref();

			if (callbackType == "Global") // not a member method, just global code
			{
				isGlobal = true;
			}
			else
			{
				KigsCore::Connect(coreModifiableTarget, callbackType, coreModifiableTarget, privateName->const_ref() CONNECT_PASS_PARAM);
			}
		}


		if (!isGlobal)
		{
			coreModifiableTarget->InsertMethod(privateName->const_ref(), static_cast<RefCountedClass::ModifiableMethod>(&DynamicMethodLuaGlobalCallback::LuaGlobalCallback), privateName->const_ref() CONNECT_PASS_PARAM);
			// if code is empty, suppose it was previously defined, don't create new code
			if (privateCode->const_ref() != "")
			{
				AddLuaMethod(coreModifiableTarget, privateName->const_ref(), privateCode->const_ref());
			}
		}
		else // global code
		{
			LuaState L = myLuaState;
			LuaStackAssert protect{ L };
			int top = L.top();
			if (ExecuteString(privateCode->const_ref()))
			{
				int nb_returns = L.top() - top;
				if (nb_returns)
				{
					if (nb_returns == 1 && (L.isFunction(-1) || L.isCFunction(-1)))
					{
						L.push((CoreModifiable*)&target->const_ref());
						SafePCall(1, 0);
					}
					else
						L.pop(nb_returns);
				}
			}

		}
	}
	return true;
}



