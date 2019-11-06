#pragma once

#include <CoreModifiable.h>
#include <CoreModifiableAttribute.h>

#include <LuaIntf/LuaIntf.h>


struct lua_State;
class LuaKigsBindModule;

class LuaImporter : public CoreModifiable
{
protected:
	struct PostImportCallback
	{
		LuaIntf::LuaRef func;
		CoreModifiable* obj;
	};

public:
	DECLARE_CLASS_INFO(LuaImporter, CoreModifiable, LuaKigsBind)
	DECLARE_INLINE_CONSTRUCTOR(LuaImporter) {}


	static int Import(const kstl::string& fileName, kstl::vector<CoreModifiable*>& root, CoreModifiable* parent = nullptr);

protected:
	virtual void InitModifiable() override;
	virtual ~LuaImporter();

	struct ConnectParam
	{
		CoreModifiable* currentNode;
		kstl::string signal, slot, sender, receiver;
	};

	struct ImportStruct
	{
		kstl::vector<CoreModifiable*> rootObj;
		kstl::vector<CoreModifiable*> loaded;
		kstl::vector<PostImportCallback> CBList;
		kstl::vector<ConnectParam> connects;
		bool noinit = false;
	};

	static int InternalImport(CoreModifiable* parent, LuaIntf::LuaRef table, ImportStruct& import, const std::string& override_name="");
	static void ParseAttributes(CoreModifiable* current, LuaIntf::LuaRef table);
	static void FinalizeImport(LuaIntf::LuaState L, ImportStruct& import);

	maString _script = BASE_ATTRIBUTE(Script, "");
};

