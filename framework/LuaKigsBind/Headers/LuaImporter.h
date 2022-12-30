#pragma once

#include <CoreModifiable.h>
#include <CoreModifiableAttribute.h>

#include <LuaIntf/LuaIntf.h>

struct lua_State;

namespace Kigs
{
	namespace Lua
	{
		using namespace Kigs::Core;
		using namespace LuaIntf;

		class LuaKigsBindModule;

		// ****************************************
		// * LuaImporter class
		// * --------------------------------------
		/**
		 * \class	LuaImporter
		 * \file	LuaImporter.h
		 * \ingroup LuaBind
		 * \brief	Kigs importer using Lua rather than XML
		 */
		 // ****************************************

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


			static int Import(const std::string& fileName, std::vector<CoreModifiable*>& root, CoreModifiable* parent = nullptr);

			virtual ~LuaImporter();

		protected:
			virtual void InitModifiable() override;


			struct ConnectParam
			{
				CoreModifiable* currentNode;
				std::string signal, slot, sender, receiver;
			};

			struct ImportStruct
			{
				std::vector<CoreModifiable*> rootObj;
				std::vector<CMSP> loaded;
				std::vector<PostImportCallback> CBList;
				std::vector<ConnectParam> connects;
				bool noinit = false;
			};

			static int InternalImport(CoreModifiable* parent, LuaIntf::LuaRef table, ImportStruct& import, const std::string& override_name = "");
			static void ParseAttributes(CoreModifiable* current, LuaIntf::LuaRef table);
			static void FinalizeImport(LuaIntf::LuaState L, ImportStruct& import);

			maString Script = BASE_ATTRIBUTE(Script, "");
		};

	}
}