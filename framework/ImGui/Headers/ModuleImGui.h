#pragma once

#include "ModuleBase.h"
#include "imgui.h"

#include "AttributePacking.h"

namespace Kigs
{
	namespace Gui
	{
		using namespace Core;
		class ImGuiLayer;

		/*! \defgroup ImGUIModule Kigs ImGUI integration
		*/

		// ****************************************
		// * ModuleImGui class
		// * --------------------------------------
		/**
		* \file	ModuleImGui.h
		* \class	ModuleImGui
		* \ingroup ImGUIModule
		* \ingroup Module
		* \brief Generic Module for ImGUI integration classes.
		*
		*/
		// ****************************************

		class ModuleImGui : public ModuleBase
		{
		public:
			DECLARE_CLASS_INFO(ModuleImGui, ModuleBase, ImGui)
				DECLARE_INLINE_CONSTRUCTOR(ModuleImGui) {}

			//! init module
			void Init(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params) override;
			//! close module
			void Close() override;

			//! update module
			void Update(const Time::Timer& timer, void* addParam) override;

			// Quickly create a layer for debug purposes (uses the IMEditor's one if available)
			ImGuiLayer* CreateDebugLayer(float bg_opacity = -1.0f);


			WRAP_METHODS(SetupDebugLayer);
		protected:

			void SetupDebugLayer()
			{
				CreateDebugLayer();
			}
		};

	}
}



