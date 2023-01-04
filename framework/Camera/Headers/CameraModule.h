#pragma once

#include "ModuleBase.h"
#include "Core.h"


namespace Kigs
{
	namespace Camera
	{
		using namespace Kigs::Core;
		/*! \defgroup Camera
		*  Webcam / video management
		*/

		SP<ModuleBase> PlatformCameraModuleInit(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params);

		// ****************************************
		// * CameraModule class
		// * --------------------------------------
		/**
		* \file	CameraModule.h
		* \class	CameraModule
		* \ingroup Camera
		* \ingroup Module
		* \brief Generic Camera management module

		*/
		// ****************************************

		class CameraModule : public ModuleBase
		{
		public:

			DECLARE_CLASS_INFO(CameraModule, ModuleBase, CameraModule)

				//! module constructor 
				CameraModule(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);

			//! module init
			void Init(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params) override;

			//! module close
			void Close() override;

			//! module update
			void Update(const Time::Timer& timer, void* addParam) override;
			//! destructor
			virtual ~CameraModule();
		protected:

		};

	}
}