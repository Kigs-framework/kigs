#pragma once

#include "ModuleBase.h"
#include "Core.h"
#include "CameraModule.h"

namespace Kigs
{
	namespace Camera
	{
		class ModuleCameraWindows;
		extern ModuleCameraWindows* gInstanceModuleCameraWindows;

		// ****************************************
		// * ModuleCameraWindows class
		// * --------------------------------------
		/*!  \class ModuleCameraWindows
			 this class is the module manager class.
		*/
		// ****************************************

		class ModuleCameraWindows : public ModuleBase
		{
		public:

			DECLARE_CLASS_INFO(ModuleCameraWindows, ModuleBase, ModuleCameraWindows)

				//! module constructor 
				ModuleCameraWindows(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);

			//! module init
			void Init(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params);

			//! module close
			void Close();

			//! module update
			virtual void Update(const Time::Timer& timer, void* addParam);
			//! destructor
			virtual ~ModuleCameraWindows();
		protected:

		};

	}
}