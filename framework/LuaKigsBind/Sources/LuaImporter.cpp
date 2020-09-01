#include "PrecompiledHeaders.h"
#include <LuaImporter.h>

#include <ModuleFileManager.h>
#include <algorithm>
#include <lua_kigs.h>
#include "LuaKigsBindModule.h"


//#define DEBUG_PRINT
#ifdef DEBUG_PRINT
#define TestPrintf(...) kigsprintf(__VA_ARGS__)
#else
#define TestPrintf(...)
#endif

IMPLEMENT_CLASS_INFO(LuaImporter)

using namespace LuaIntf;

LuaImporter::~LuaImporter()
{
}

void LuaImporter::FinalizeImport(LuaIntf::LuaState L, ImportStruct& import)
{
	for (auto item : import.loaded)
	{
		item->Destroy();
	}

	for (auto& cb : import.CBList)
	{
		cb.func.pushToStack();
		if (cb.obj)
			L.push(cb.obj);

		if (L.pcall(cb.obj ? 1 : 0, 0, 0) != 0)
		{
			char error_msg[1024];
			snprintf(error_msg, 1024, "Post Import callback errro : %s\n", L.popValue<const char*>());
			KIGS_MESSAGE(error_msg);
		}
	}

	for (auto& toconnect : import.connects)
	{
		CoreModifiable* pia = toconnect.currentNode;
		CoreModifiable* pib = toconnect.currentNode;

		if ((toconnect.sender != "this") && (toconnect.sender != "self")) // connection is son of a coremodifiable, one of the instance can be set at this or self
		{
			// else search instance in hierachy
			pia = SearchInstance(toconnect.sender, toconnect.currentNode).get();
		}

		if ((toconnect.receiver != "this") && (toconnect.receiver != "self")) // connection is son of a coremodifiable, one of the instance can be set at this or self
		{
			// else search instance in hierachy
			pib = SearchInstance(toconnect.receiver, toconnect.currentNode).get();
		}

		if (pia && pib)
		{
			KigsCore::Connect(pia, toconnect.signal, pib, toconnect.slot);
		}
		else
		{
			STACK_STRING(str, 1024, "Cannot connect %s to %s", toconnect.sender.c_str(), toconnect.receiver.c_str());
			KIGS_WARNING(str, 3);
		}
	}
}

s32 LuaImporter::Import(const std::string& fileName, std::vector<CoreModifiable*>& root, CoreModifiable* parent)
{
	LuaKigsBindModule * _luamodule = (LuaKigsBindModule*)KigsCore::GetModule("LuaKigsBindModule");
	auto _lua = _luamodule->getLuaState();



	if (_luamodule->ExecuteLuaFile(fileName.c_str()))
	{
		LuaState L = _lua;

		TestPrintf("LUA IMPORT\n");
		auto table = L.popValue<LuaRef>();

		if (!table.isTable())
		{
			KIGS_MESSAGE("LuaImporter requires a table as the return value from the file");
			return 0;
		}

		ImportStruct import;

		InternalImport(parent, table, import);
		FinalizeImport(L, import);

		return root.size();
	}
	else
	{
		TestPrintf("cannot load %s\n", fileName.c_str());
	}
	return 0;
}

void LuaImporter::InitModifiable()
{
	CoreModifiable::InitModifiable();

	if (IsInit())
	{
		CoreModifiable* parent = getFirstParent(CoreModifiable::mClassID);

		if (!parent)
		{
			UninitModifiable();
			return;
		}

		LuaKigsBindModule* _luamodule = (LuaKigsBindModule*)KigsCore::GetModule("LuaKigsBindModule");
		if (_luamodule->ExecuteLuaFile(Script.c_str()))
		{
			LuaState L = _luamodule->getLuaState();

			TestPrintf("LUA IMPORT\n");
			auto table = L.popValue<LuaRef>();

			if (!table.isTable())
			{
				KIGS_MESSAGE("LuaImporter requires a table as the return value from the file");
				return;
			}

			ImportStruct import;

			InternalImport(parent, table, import);
			FinalizeImport(L, import);

		}
		else
		{
			TestPrintf("cannot load %s\n", Script.c_str());
		}
	}
}
s32 LuaImporter::InternalImport(CoreModifiable* parent, LuaIntf::LuaRef table, ImportStruct& import, const std::string& override_name)
{
	if (!table.has("_info"))
	{
		for (auto& it : table)
		{
			if (it.value().isFunction())
				import.CBList.push_back({ it.value(), parent });
			else
				InternalImport(parent, it.value(), import);
		}
		return import.rootObj.size();
	}

	// TODO(antoine): references / includes

	LuaRef infos = table["_info"];
	// Single object

	std::string name = override_name.empty() ? infos["name"].value().toValue<std::string>() : override_name;
	CMSP current = nullptr;
	bool needinit = true;
	bool is_aggregate = infos.has("_aggregate") && infos["_aggregate"].value().toValue<bool>();
	bool need_add = true;

	if (infos.has("type"))
	{
		std::string type = infos["type"].value().toValue<std::string>();
		if (parent)
		{
			auto& items = parent->getItems();
			auto it = std::find_if(items.begin(), items.end(), [current](ModifiableItemStruct str) { return str.mItem == current; });
			if (it != items.end())
			{
				current = it->mItem;
				needinit = false;
			}
		}
		if (needinit)
		{
			current = KigsCore::GetInstanceOf(name, type);
			current->GetRef();
			import.loaded.push_back(current.get());
			import.rootObj.push_back(current.get());
		}

	}
	else if (infos.has("ref"))
	{

		auto ref_type = infos["ref"].value().toValue<std::string>();
		auto it = std::find_if(import.loaded.begin(), import.loaded.end(), [&](CoreModifiable* item) { return item->getName() == name && item->isSubType(CharToID::GetID(ref_type)); });

		if (it != import.loaded.end())
		{
			current = CMSP(*it, GetRefTag{});
		}
		else
		{
			std::vector<CMSP> instances=GetInstancesByName(ref_type, name);

			if (instances.size())
			{
				current = instances[0];
				import.loaded.push_back(current.get());
				import.rootObj.push_back(current.get());
			}
		}
		needinit = false;
	}
	else if (infos.has("xml") || infos.has("lua"))
	{
		if (infos.has("xml"))
		{
			current = CoreModifiable::Import(infos["xml"].value().toValue<std::string>(),false,false,nullptr,name);
			if (current)
			{
				current->GetRef();
				import.loaded.push_back(current.get());
				import.rootObj.push_back(current.get());
				needinit = false;
			}
		}
		else if (infos.has("lua"))
		{
			u64 length;
			CoreRawBuffer* buffer = ModuleFileManager::LoadFileAsCharString(infos["lua"].value().toValue<std::string>().c_str(), length,1);
			if (buffer)
			{
				LuaKigsBindModule* _luamodule = (LuaKigsBindModule*)KigsCore::GetModule("LuaKigsBindModule");
				luaL_dostring(_luamodule->getLuaState(), buffer->buffer());

				LuaState L = _luamodule->getLuaState();
				auto table_include = L.popValue<LuaRef>();

				InternalImport(parent, table_include, import);

				// TODO what if multiple object?
				if (current)
				{
					current->setName(name);
				}
				buffer->Destroy();
			}
		}
	}
	else if (infos.has("connect"))
	{
		ConnectParam connect;
		connect.sender = infos["connect"].value()["sender"].value<std::string>();
		connect.receiver = infos["connect"].value()["receiver"].value<std::string>();
		connect.signal = infos["connect"].value()["signal"].value<std::string>();
		connect.slot = infos["connect"].value()["slot"].value<std::string>();
		connect.currentNode = parent;
		import.connects.push_back(connect);
	}
	else if (infos.has("path"))
	{
		current = SearchInstance(infos["name"].value<std::string>(), parent);
		if (current)
		{
			needinit = false;
			import.loaded.push_back(current.get());
			need_add = false;
		}
	}


	if (current)
	{
		TestPrintf("item %s\n", current->getName().c_str());
		if (table.has("_attrs"))
		{
			ParseAttributes(current.get(), table["_attrs"]);
		}

		if (table.has("_childs"))
		{
			LuaRef childs_table = table["_childs"];

			std::vector<CoreModifiable*> root;
			InternalImport(current.get(), childs_table, import);

		}

		if (!import.noinit && needinit)
			current->Init(); // mimic xml import behaviour

		if (parent && need_add)
		{
			auto& items = parent->getItems();
			if (std::find_if(items.begin(), items.end(), [current](ModifiableItemStruct str) { return str.mItem == current; }) == items.end())
			{
				if (is_aggregate)
					parent->aggregateWith(current);
				else
					parent->addItem(current);

				if (!import.noinit && !current->IsInit())
					current->Init(); // mimic xml import behaviour				
			}
		}
	}
	return 1;
}

void LuaImporter::ParseAttributes(CoreModifiable* current, LuaRef table)
{

	LuaKigsBindModule* _luamodule = (LuaKigsBindModule*)KigsCore::GetModule("LuaKigsBindModule");

	// TODO(antoine) : modifiers 
	for (auto it : table)
	{
		std::string attr_name = it.key().toValue<std::string>();
		TestPrintf("\t attr %s", attr_name.c_str());
		bool addModifier = it.value().isTable() && it.value().has("_modifier");
		LuaRef modifier;

		LuaRef value = it.value();
		if (addModifier)
		{
			TestPrintf(" (modifier)");
			modifier = it.value()["_modifier"].value();
			for (auto& itmod : it.value())
			{
				if (!itmod.value().isIdenticalTo(modifier))
				{
					value = itmod.value();
					break;
				}
			}
		}

		CoreModifiableAttribute* attribute = current->getAttribute(attr_name);

		if (attribute)
		{
			ATTRIBUTE_TYPE attr_type = attribute->getType();

			bool was_eval = false;
			auto value_type = value.type();
			if (value_type == LuaTypeID::STRING)
			{
				std::string tmpvalue = value.toValue<std::string>();
				if (CoreModifiable::AttributeNeedEval(tmpvalue))
				{
					CoreModifiable::EvalAttribute(tmpvalue, current, attribute);
					attribute->setValue(tmpvalue);
					was_eval = true;
				}
			}
			else if (value_type == LuaTypeID::BOOLEAN)
			{
				was_eval = true;
				attribute->setValue(value.toValue<bool>());
			}
			else if (value_type == LuaTypeID::NUMBER)
			{
				was_eval = true;
				switch (attr_type)
				{
				case ATTRIBUTE_TYPE::BOOL:
					attribute->setValue(value.toValue<bool>());
					break;
				case ATTRIBUTE_TYPE::CHAR:
					attribute->setValue(value.toValue<char>());
					break;
				case ATTRIBUTE_TYPE::SHORT:
					attribute->setValue(value.toValue<s16>());
					break;
				case ATTRIBUTE_TYPE::INT:
					attribute->setValue(value.toValue<s32>());
					break;
				case ATTRIBUTE_TYPE::LONG:
					attribute->setValue(value.toValue<s64>());
					break;
				case ATTRIBUTE_TYPE::UCHAR:
					attribute->setValue(value.toValue<u8>());
					break;
				case ATTRIBUTE_TYPE::USHORT:
					attribute->setValue(value.toValue<u16>());
					break;
				case ATTRIBUTE_TYPE::UINT:
					attribute->setValue(value.toValue<u32>());
					break;
				case ATTRIBUTE_TYPE::ULONG:
					attribute->setValue(value.toValue<u64>());
					break;
				case ATTRIBUTE_TYPE::FLOAT:
					attribute->setValue(value.toValue<kfloat>());
					break;
				case ATTRIBUTE_TYPE::DOUBLE:
					attribute->setValue(value.toValue<kdouble>());
					break;
				default:
					was_eval = false;
					break;
				}
			}

			if (!was_eval)
			{
				switch (attr_type)
				{
				case ATTRIBUTE_TYPE::REFERENCE:
				case ATTRIBUTE_TYPE::USSTRING:
				case ATTRIBUTE_TYPE::STRING:
					attribute->setValue(value.toValue<std::string>());
					break;

				case ATTRIBUTE_TYPE::ENUM:
					if (value.type() == LuaTypeID::STRING)
						attribute->setValue(value.toValue<std::string>());
					else
						attribute->setValue(value.toValue<s32>());
					break;

				case ATTRIBUTE_TYPE::ARRAY:
				{
					ATTRIBUTE_TYPE array_type = attribute->getArrayElementType();
					s32 n = value.len();

					for (s32 i = 0; i < n; ++i)
					{
						switch (array_type)
						{
						case ATTRIBUTE_TYPE::BOOL:
							attribute->setArrayElementValue(value[i + 1].value().toValue<bool>(), 0, i);
							break;
						case ATTRIBUTE_TYPE::CHAR:
							attribute->setArrayElementValue(value[i + 1].value().toValue<s8>(), 0, i);
							break;
						case ATTRIBUTE_TYPE::SHORT:
							attribute->setArrayElementValue(value[i + 1].value().toValue<s16>(), 0, i);
							break;
						case ATTRIBUTE_TYPE::INT:
							attribute->setArrayElementValue(value[i + 1].value().toValue<s32>(), 0, i);
							break;
						case ATTRIBUTE_TYPE::LONG:
							attribute->setArrayElementValue(value[i + 1].value().toValue<s64>(), 0, i);
							break;
						case ATTRIBUTE_TYPE::UCHAR:
							attribute->setArrayElementValue(value[i + 1].value().toValue<u8>(), 0, i);
							break;
						case ATTRIBUTE_TYPE::USHORT:
							attribute->setArrayElementValue(value[i + 1].value().toValue<u16>(), 0, i);
							break;
						case ATTRIBUTE_TYPE::UINT:
							attribute->setArrayElementValue(value[i + 1].value().toValue<u32>(), 0, i);
							break;
						case ATTRIBUTE_TYPE::ULONG:
							attribute->setArrayElementValue(value[i + 1].value().toValue<u64>(), 0, i);
							break;
						case ATTRIBUTE_TYPE::FLOAT:
							attribute->setArrayElementValue(value[i + 1].value().toValue<kfloat>(), 0, i);
							break;
						case ATTRIBUTE_TYPE::DOUBLE:
							attribute->setArrayElementValue(value[i + 1].value().toValue<kdouble>(), 0, i);
							break;
						case ATTRIBUTE_TYPE::USSTRING:
						case ATTRIBUTE_TYPE::STRING:
							attribute->setArrayElementValue(value[i + 1].value().toValue<std::string>(), 0, 1);
							break;
						default:
							break;
						}

					}
					break;
				}
				case ATTRIBUTE_TYPE::COREITEM:
					attribute->setValue(value.toValue<std::string>());
					break;
				case ATTRIBUTE_TYPE::UNKNOWN: break;
				default: break;

				}
			}
		}
		else
		{
			if (attr_name.substr(0, 8) == "Callback")
			{
				auto name = attr_name.substr(8, attr_name.size() - 8);
				assert(value.type() == LuaTypeID::STRING);
				KigsCore::Connect(current, name, current, value.toValue<std::string>());
				//@TODO: proper connect support (with inline lua funcs)
			}
			else
			{
				TestPrintf(" DYNAMIC");
				// TODO(antoine) Allow for non string dynamic evals!!!

				switch (value.type())
				{
					// Not supported
					// TODO(antoine) support for ref
				case LuaTypeID::NONE:
				case LuaTypeID::NIL:
				case LuaTypeID::THREAD:
				case LuaTypeID::FUNCTION:
				{
					value.pushToStack();
					_luamodule->AddLuaMethodFromStack(current, attr_name);
					current->InsertMethod(attr_name, static_cast<CoreModifiable::ModifiableMethod>(&DynamicMethodLuaGlobalCallback::LuaGlobalCallback), attr_name);
				}
				break;
				case LuaTypeID::USERDATA:
				case LuaTypeID::LIGHTUSERDATA:
					break;

				case LuaTypeID::STRING:
				{
					std::string tmpvalue;
					attribute = current->AddDynamicAttribute(ATTRIBUTE_TYPE::STRING, attr_name);
					tmpvalue = value.toValue<std::string>();
					if (CoreModifiable::AttributeNeedEval(tmpvalue))
					{
						CoreModifiable::EvalAttribute(tmpvalue, current, attribute);
					}
					attribute->setValue(tmpvalue);
				}
				break;
				case LuaTypeID::NUMBER:
					attribute = current->AddDynamicAttribute(ATTRIBUTE_TYPE::FLOAT, attr_name, value.toValue<kfloat>());
					break;
				case LuaTypeID::TABLE:
				{
					s32 count = value.len();
					if (count)
					{
						std::array<float, 16> elems;
						KIGS_ASSERT(count <= 16);
						for (s32 i = 0; i < count; ++i)
						{
							elems[i] = value[i + 1].value().toValue<kfloat>();
						}
						attribute = current->AddDynamicVectorAttribute(attr_name, elems.data(), count);
					}
				}
				break;
				case LuaTypeID::BOOLEAN:
					attribute = current->AddDynamicAttribute(ATTRIBUTE_TYPE::BOOL, attr_name, value.toValue<bool>());
					break;



				default: break;
				}
			}
		}

		TestPrintf("\n");
		if (addModifier)
		{
			AttachedModifierBase* toAdd = 0;
			std::string modifiertype = modifier["type"].value().toValue<std::string>();

			auto& instancemap = KigsCore::Instance()->GetDefaultCoreItemOperatorConstructMap();

			auto itfound = instancemap.find(modifiertype);

			if (itfound != instancemap.end())
			{
				toAdd = (AttachedModifierBase*)itfound->second();
			}

			if (toAdd != nullptr)
			{
				bool isGetter = true;
				if (modifier.has("isGetter") && !modifier["isGetter"].value().toValue<bool>())
				{
					isGetter = false;
				}

				std::string modvalue = "";

				if (modifier.has("value"))
				{
					modvalue = modifier["value"].value().toValue<std::string>();
				}

				toAdd->Init(attribute, isGetter, modvalue);
				attribute->attachModifier(toAdd);
			}

		}

	}

}



