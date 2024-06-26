#pragma once

#include "CoreModifiable.h"
#include "TecLibs/Tec3D.h"
#include "SceneNode.h"
#include "TravState.h"

namespace Kigs
{
	namespace Draw
	{
		using namespace Kigs::Scene;
		// ****************************************
		// * Drawable class
		// * --------------------------------------
		/**
		 * \file	Drawable.h
		 * \class	Drawable
		 * \ingroup Renderer
		 * \brief	Base class for all drawable objects.
		 *
		 * Drawable objects can generally be shared ( textures, meshes ... ) to be drawn several times.
		 */
		 // ****************************************

		class Drawable;
		class DrawableCallbackInterface
		{
		public:
			virtual bool operator()(TravState* travstate, Drawable* drawable, int drawable_need) = 0;
		};

		class Drawable : public Scene::SceneNode
		{
		public:
			DECLARE_ABSTRACT_CLASS_INFO(Drawable, SceneNode, Renderer)
				DECLARE_CONSTRUCTOR(Drawable);

			/**
			 * \brief	pre draw method
			 * \fn 		virtual void	DoPreDraw(TravState*);
			 * \param	TravState : camera state
			 */
			virtual void	DoPreDraw(TravState*);

			/**
			 * \brief	draw method
			 * \fn 		virtual void	DoDraw(TravState* travstate);
			 * \param	TravState : camera state
			 */
			virtual void	DoDraw(TravState* travstate);

			/**
			 * \brief	post draw method
			 * \fn 		virtual void	DoPostDraw(TravState* travstate);
			 * \param	travstate : camera state
			 */
			virtual void	DoPostDraw(TravState* travstate);

			/**
			 * \brief	check before calling corresponding DoPredraw
			 * \fn 		virtual void	CheckPostDraw(TravState* travstate);
			 * \param	travstate : camera state
			 */
			inline void	CheckPreDraw(TravState* travstate)
			{
				if (mDrawingNeeds & ((unsigned int)Need_Predraw) && (!travstate->mCurrentPass || IsUsedInRenderPass(travstate->mCurrentPass->pass_mask)))
					DoPreDraw(travstate);
			}
			/**
			 * \brief	check before calling corresponding DoDraw
			 * \fn 		virtual void	CheckPostDraw(TravState* travstate);
			 * \param	travstate : camera state
			 */
			inline void	CheckDraw(TravState* travstate)
			{
				if (mDrawingNeeds & ((unsigned int)Need_Draw) && (!travstate->mCurrentPass || IsUsedInRenderPass(travstate->mCurrentPass->pass_mask)))
					DoDraw(travstate);
			}
			/**
			 * \brief	check before calling corresponding DoPostPredraw
			 * \fn 		virtual void	CheckPostDraw(TravState* travstate);
			 * \param	travstate : camera state
			 */
			inline void	CheckPostDraw(TravState* travstate)
			{
				if (mDrawingNeeds & ((unsigned int)Need_Postdraw) && (!travstate->mCurrentPass || IsUsedInRenderPass(travstate->mCurrentPass->pass_mask)))
					DoPostDraw(travstate);
			}

			bool IsUsedInRenderPass(u32 pass_mask) const { return (u32(mRenderPassMask) & pass_mask) != 0; }

			/**
			 * \brief	update the bounding box
			 * \fn 		virtual bool	BBoxUpdate(double time)=0;
			 * \param	time : world time
			 * \return	FALSE if object without bounding box, TRUE otherwise
			 */
			virtual bool	BBoxUpdate(double time) { return false; }

			/**
			 * \brief	add item in scene tree
			 * \fn 		virtual bool	addItem(const CMSP& item, ItemPosition pos=Last DECLARE_DEFAULT_LINK_NAME);
			 * \param	item : item to add
			 * \param	linkName : name of the added item
			 * \return	TRUE if the item is added, FALSE otherwise
			 */
			bool	addItem(const CMSP& item, ItemPosition pos = Last) override;

			/**
			 * \brief	remove item in scene tree
			 * \fn 		virtual bool	removeItem(const CMSP&  item DECLARE_DEFAULT_LINK_NAME);
			 * \param	item : item to remove
			 * \param	linkName : name of the removed item
			 * \return	TRUE if the item is removed, FALSE otherwise
			 */
			bool	removeItem(const CMSP& item) override;

			/**
			 * \brief	get the global position of the drawable
			 * \fn 		virtual void GetGlobalPosition(mat4 *pLocalToGlobal, float &x, float &y, float &z)
			 * \param	pLocalToGlobal : conversion matrix
			 * \param	x : coordinate on x axis (in/out param)
			 * \param	y : coordinate on y axis (in/out param)
			 * \param	z : coordinate on z axis (in/out param)
			 */
			virtual void GetGlobalPosition(mat4* pLocalToGlobal, float& x, float& y, float& z) { x = (*pLocalToGlobal)[3][0]; y = (*pLocalToGlobal)[3][1]; z = (*pLocalToGlobal)[3][2]; }

			virtual void		UpdateDrawingNeeds();

			inline unsigned int	GetDrawingNeeds()
			{
				return mDrawingNeeds;
			}

			virtual	unsigned int	GetSelfDrawingNeeds()
			{
				return ((unsigned int)Need_Predraw) | ((unsigned int)Need_Draw) | ((unsigned int)Need_Postdraw);
			}

			inline void SetDrawingNeeds(unsigned int needs)
			{
				mDrawingNeeds = needs;
			}

			bool IsSortable() { return mSortable; }

			void SetDrawableCallback(DrawableCallbackInterface* callback) { mCallback = callback; }

			enum	DRAWABLE_DRAWING_NEEDS
			{
				Not_Init = 1,
				Need_Predraw = 2,
				Need_Draw = 4,
				Need_Postdraw = 8
			};

		protected:
			u32		mRenderPassMask = 0xffffffff;
			bool	mSortable = false;

			WRAP_ATTRIBUTES(mRenderPassMask, mSortable);

			//! init the modifiable and set the _isInit flag to true if OK
			void	InitModifiable() override;

			/**
			 * \brief	initialise pre draw method
			 * \fn 		virtual bool PreDraw(TravState*);
			 * \param	TravState : camera state
			 * \return	TRUE if a could PreDraw
			 */
			virtual bool PreDraw(TravState*);

			/**
			 * \brief	initialise draw method
			 * \fn 		virtual bool Draw(TravState*);
			 * \param	TravState : camera state
			 * \return	TRUE if a could draw
			 */
			virtual bool Draw(TravState*);

			/**
			 * \brief	initialise PostDraw method
			 * \fn 		virtual bool PostDraw(TravState*);
			 * \param	travstate : camera state
			 * \return	TRUE if a could PostDraw
			 */
			virtual bool PostDraw(TravState*);

			/**
			 * \brief	update the father node bounding box
			 * \fn 		virtual void FatherNode3DNeedBoundingBoxUpdate();
			 */
			virtual void FatherNode3DNeedBoundingBoxUpdate();

			unsigned int	mDrawingNeeds;
			DrawableCallbackInterface* mCallback = nullptr;
		};

	}
}