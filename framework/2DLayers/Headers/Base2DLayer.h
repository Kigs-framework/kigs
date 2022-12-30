#pragma once

#include "Abstract2DLayer.h"

#include "PrecompiledHeaders.h"
#include "GlobalEnum.h"
#include "Core.h"
#include "TinyImage.h"
#include "maReference.h"
#include <deque>

namespace Kigs
{
	namespace Draw
	{
		class RenderingScreen;
		class Texture;
	}
	namespace Input
	{
		class ModuleInput;
	}
	namespace Draw2D
	{
		using namespace Kigs::Core;

		class UIItem;
		class MouseVelocityComputer;
		class MultiTouchPinch;
		class TravState;

		// ****************************************
		// * Base2DLayer class
		// * --------------------------------------
		/**
		* \file	Base2DLayer.h
		* \class	Base2DLayer
		* \ingroup 2DLayers
		* \brief	Obsolete 2D Layer.
		*
		* Now use UI2DLayer insteed.
		*/
		// ****************************************
		class Base2DLayer : public Abstract2DLayer
		{
		public:
			DECLARE_ABSTRACT_CLASS_INFO(Base2DLayer, Abstract2DLayer, 2DLayers);

			/**
			 * \brief	constructor
			 * \param	name : instance name
			 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
			 */
			Base2DLayer(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);

			//! no camera for 2D Layer, throw an error
			void  addCamera(CoreModifiable* /*camera*/) override { KIGS_ERROR("Trying to add a camera on a 2D Layer", 1); }
			//! no camera for 2D Layer, throw an error
			bool  removeCamera(CoreModifiable* /*camera*/) override { KIGS_ERROR("Trying to remove a camera from a 2D Layer", 1); return false; }

			/**
			 * \brief	update the texture from a buffer (used with camera)
			 * \param	buffer : pixel color buffer
			 */
			virtual void updateLayerFromBuffer(unsigned char* buffer, unsigned int width, unsigned int height, std::string name, Pict::TinyImage::ImageFormat format = Pict::TinyImage::ABGR_16_1555_DIRECT_COLOR) = 0;

			/**
			 * \brief	call directly CoreModifiable addItem (not Scene3D or Node3D)
			 * \param	item : item to add
			 * \param	linkName : name of the added item
			 * \return	TRUE if the item is added, FALSE otherwise
			 */
			bool	addItem(const CMSP& item, ItemPosition pos = Last DECLARE_DEFAULT_LINK_NAME) override;

			/**
			 * \fn 		virtual bool	removeItem(const CMSP& item DECLARE_DEFAULT_LINK_NAME);
			 * \brief	call directly CoreModifiable addItem (not Scene3D or Node3D)
			 * \param	item : item to remove
			 * \param	linkName : name of the removed item
			 * \return	TRUE if the item is removed, FALSE otherwise
			 */
			bool	removeItem(const CMSP& item DECLARE_DEFAULT_LINK_NAME) override;

			/**
			 * \brief	draw the scene3D
			 * \fn 		virtual void            TravDraw(TravState* state)=0;
			 * \param	state : the current TravState
			 * \return	TRUE if this node is visible or we are in a "all visible" branch of the scenegraph
			 */
			void TravDraw(Scene::TravState* state) override = 0;

			/**
			 * \brief	call cull process for this node and all sons at visible positions
			 * \fn 		virtual void TravCull(TravState* state);
			 * \param	state : travstate
			 */
			void TravCull(Scene::TravState* state) override = 0;

			bool ChangeTexture(std::string TextureName);

			void GetSize(float& X, float& Y);


			void UpdateLayer();


			// *******************************************************
			// Tiles layer method
			// *******************************************************
			/**
			 * \brief	load the tile data
			 * \fn 		virtual	void	LoadData()=0;
			 */
			void	LoadData();

			/**
			 * \brief	set the tile info
			 * \fn 		void	SetTileInfo(unsigned short info,unsigned int posX,unsigned int posY);
			 * \param	info : index of the info
			 * \param	posX : pos on X axis of the tile
			 * \param	posY : pos on Y axis of the tile
			 */
			void	SetTileInfo(unsigned short info, unsigned int posX, unsigned int posY);

			/**
			 * \brief	set the scrolling values of the layer (used by virtual screen)
			 * \fn 		void setScroll(int scrollX, int scrollY)
			 * \param	scrollX : scrolling on X axis
			 * \param	scrollY : scrolling on Y axis
			 */
			void setScroll(int scrollX, int scrollY);

			void setScreenPos(int scrX, int scrY);

			void GetPositionOnScreen(float X, float Y, float& PosX, float& PosY);
			void GetPositionOnLayer(float X, float Y, float& PosX, float& PosY);

			// *******************************************************
			// Direct draw method for bitmap layer only
			// *******************************************************
			virtual void ClearDirectDrawList();

			virtual void AddDirectDraw(DirectDrawStruct* toAdd)
			{
				mDirectDrawList.push_back(toAdd);
			}

			virtual void RemoveDirectDraw(DirectDrawStruct* toRemove);

			virtual void DrawRectangle(float X, float Y, int sizeX, int sizeY, float r, float g, float b) = 0;

			virtual bool UsePalette() { return false; }

		protected:
			/**
			 * \brief	destructor
			 * \fn 		~Base2DLayer();
			 */
			virtual ~Base2DLayer();

			/**
			 * \fn 		void InitModifiable() override
			 * \brief	init the modifiable and set the isInit flag to true if OK
			 */
			void	InitModifiable() override;

			//! type of the layer ("Character","Bitmap" or "Sprite")
			maEnum<3>			mType;
			maBool				mUsePalette;
			//! type of the layer color mode ("16","256","256x16","DC")
			maEnum<4>			mColorMode;
			//! name of the linked virtualScreen
			maString			mVirtualScreenName;
			//! repeat mode
			maBool				mRepeatX;
			maBool				mRepeatY;

			//! pointer to the texture (from a bitmap)
			Texture* mBitmapTexture;

			//! file name (texture file)
			maString			mFileName;

			//! Scroll on X axis of the screen
			int					mScrollX;
			//! Scroll on Y axis of the screen
			int					mScrollY;

			// position on X axis of the screen on the layer
			int					mLayerOffsetX;
			// position on Y axis of the screen on the layer
			int					mLayerOffsetY;

			// position on X axis of the screen on the layer
			int					mScreenOffsetX;
			// position on Y axis of the screen on the layer
			int					mScreenOffsetY;

			maInt				mScreenPosX;
			maInt				mScreenPosY;
			int					mOldScreenPosX;
			int					mOldScreenPosY;

			bool				mShouldSetOffset;
			bool				mShouldUploadData;

			bool mSecondTouch;

			std::vector<DirectDrawStruct*> mDirectDrawList;
		};
	}
}
