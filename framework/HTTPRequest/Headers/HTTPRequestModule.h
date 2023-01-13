#pragma once

#include "ModuleBase.h"
#include "Core.h"

namespace Kigs
{
	namespace Http
	{
		using namespace Kigs::Core;

		extern SP<ModuleBase> PlatformHTTPRequestModuleInit(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params);


		/*! \defgroup HTTPRequest HTTPRequest module
		*/

		// ****************************************
		// * HTTPRequestModule class
		// * --------------------------------------
		/**
		* \file	HTTPRequestModule.h
		* \class	HTTPRequestModule
		* \ingroup HTTPRequest
		* \ingroup Module
		* \brief Generic Module for HTTP Request classes
		*
		*/
		// ****************************************
		class HTTPRequestModule : public ModuleBase
		{
		public:

			DECLARE_CLASS_INFO(HTTPRequestModule, ModuleBase, HTTPRequestModule)

				//! module constructor 
				HTTPRequestModule(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);
			virtual ~HTTPRequestModule();

			//! module init
			void Init(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params) override;

			//! module close
			void Close() override;

			//! module update
			void Update(const Time::Timer& timer, void* addParam) override;
		};

	}
}