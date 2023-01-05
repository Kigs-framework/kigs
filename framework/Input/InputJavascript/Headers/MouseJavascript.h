#pragma once
#include "MouseDevice.h"
#include "ModuleInputJavascript.h"

namespace Kigs
{
	namespace Input
	{
		// ****************************************
		// * MouseJavascript class
		// * --------------------------------------
		/**
		 * \class	MouseJavascript
		 * \file	MouseJavascript.h
		 * \ingroup Input
		 * \brief	Javascript mouse management.
		 */
		 // ****************************************

		class	MouseJavascript : public MouseDevice
		{
			public:
			DECLARE_CLASS_INFO(MouseJavascript,MouseDevice,Input)
			
			MouseJavascript(const std::string& name,DECLARE_CLASS_NAME_TREE_ARG);
			
			bool	Aquire() override;
			bool	Release() override;
			
			void	UpdateDevice() override;

			void	DoInputDeviceDescription() override;
			
			void	IncWheelCount(){mWheelCount++;}
			void	IncButtonCount(){mButtonsCount++;}
			virtual ~MouseJavascript();   
		   
			protected:
			
			
		};    
	}
}