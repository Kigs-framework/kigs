#pragma once

#include "UIItem.h"
#include "maReference.h"

namespace Kigs
{
	namespace Draw2D
	{
		// ****************************************
		// * UI3DLinkedItem class
		// * --------------------------------------
		/**
		* \file	UI3DLinkedItem.h
		* \class	UI3DLinkedItem
		* \ingroup 2DLayers
		* \brief	adjust position of the UIItem to match a 3D position in a Scene3D
		*
		* Suppose the UI2DLayer cover the same surface as the attached camera.
		*/
		// ****************************************
		class UI3DLinkedItem : public UIItem
		{
		public:
			DECLARE_CLASS_INFO(UI3DLinkedItem, UIItem, 2DLayers);

			/**
			* \brief	constructor
			* \param	name : instance name
			* \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
			*/
			UI3DLinkedItem(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);

		protected:
			void InitModifiable() override;
			void NotifyUpdate(const unsigned int /* labelid */) override;
			void Update(const Time::Timer& timer, void* /*addParam*/) override;

			bool							mUseUpOrientation = false;
			v3f								m3DPosition = { 0.0f, 0.0f, 0.0f };
			std::weak_ptr<CoreModifiable>	mCamera;
			std::weak_ptr<CoreModifiable>	mNode;

			WRAP_ATTRIBUTES(mUseUpOrientation, m3DPosition, mCamera, mNode);
		};

	}
}
