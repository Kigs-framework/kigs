#ifndef _DRAWABLE_H_
#define _DRAWABLE_H_

#include "CoreModifiable.h"
#include "TecLibs/Tec3D.h"
#include "SceneNode.h"
#include "TravState.h"


// ****************************************
// * Drawable class
// * --------------------------------------
/**
 * \file	Drawable.h
 * \class	Drawable
 * \ingroup Drawable
 * \ingroup RendererDrawable
 * \brief	A Drawable is the base class for any drawable object
 * \author	ukn
 * \version ukn
 * \date	ukn
 *
 * Exported parameters :<br>
 * <ul>
 * <li>
 *		bool <strong>TransarencyFlag</strong> : TRUE if transparent
 * </li>
 * <li>
 *		Uint <strong>PassMask</strong> : pass mask
 * </li>
 * <li>
 *		Uint <strong>PassCount</strong> : number of pass done
 * </li>
 * </ul>
 */
// ****************************************
class Drawable : public SceneNode 
{
public:	
	DECLARE_ABSTRACT_CLASS_INFO(Drawable,SceneNode,Renderer)
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
		if(myDrawingNeeds&((unsigned int)Need_Predraw) && (!travstate->mCurrentPass || IsUsedInRenderPass(travstate->mCurrentPass->pass_mask)))
			DoPreDraw(travstate);
	}
	/**
	 * \brief	check before calling corresponding DoDraw
	 * \fn 		virtual void	CheckPostDraw(TravState* travstate);
	 * \param	travstate : camera state
	 */
	inline void	CheckDraw(TravState* travstate)
	{
		if(myDrawingNeeds&((unsigned int)Need_Draw) && (!travstate->mCurrentPass || IsUsedInRenderPass(travstate->mCurrentPass->pass_mask)))
			DoDraw(travstate);
	}
	/**
	 * \brief	check before calling corresponding DoPostPredraw
	 * \fn 		virtual void	CheckPostDraw(TravState* travstate);
	 * \param	travstate : camera state
	 */
	inline void	CheckPostDraw(TravState* travstate)
	{
		if(myDrawingNeeds&((unsigned int)Need_Postdraw) && (!travstate->mCurrentPass || IsUsedInRenderPass(travstate->mCurrentPass->pass_mask)))
			DoPostDraw(travstate);
	}
	
	bool IsUsedInRenderPass(u32 pass_mask) const { return (u32(mRenderPassMask) & pass_mask) != 0; }

	/**
	 * \brief	update the bounding box
	 * \fn 		virtual bool	BBoxUpdate(kdouble time)=0;
	 * \param	time : world time
	 * \return	FALSE if object without bounding box, TRUE otherwise
	 */
	virtual bool	BBoxUpdate(kdouble time) { return false; }

	/**
	 * \brief	add item in scene tree
	 * \fn 		virtual bool	addItem(CoreModifiable *item, ItemPosition pos=Last DECLARE_DEFAULT_LINK_NAME);
	 * \param	item : item to add
	 * \param	linkName : name of the added item
	 * \return	TRUE if the item is added, FALSE otherwise
	 */
	bool	addItem(CoreModifiable *item, ItemPosition pos = Last) override;
	
	/**
	 * \brief	remove item in scene tree
	 * \fn 		virtual bool	removeItem(CoreModifiable* item DECLARE_DEFAULT_LINK_NAME);
	 * \param	item : item to remove
	 * \param	linkName : name of the removed item
	 * \return	TRUE if the item is removed, FALSE otherwise
	 */
	bool	removeItem(CoreModifiable* item) override;
           
	/**
	 * \brief	get the global position of the drawable
	 * \fn 		virtual void GetGlobalPosition(Matrix3x4 *pLocalToGlobal, kfloat &x, kfloat &y, kfloat &z)
	 * \param	pLocalToGlobal : conversion matrix
	 * \param	x : coordinate on x axis (in/out param)
	 * \param	y : coordinate on y axis (in/out param)
	 * \param	z : coordinate on z axis (in/out param)
	 */
	virtual void GetGlobalPosition(Matrix3x4 *pLocalToGlobal, kfloat &x, kfloat &y, kfloat &z) {x=pLocalToGlobal->e[3][0];y=pLocalToGlobal->e[3][1];z=pLocalToGlobal->e[3][2];}

	virtual void		UpdateDrawingNeeds();

	inline unsigned int	GetDrawingNeeds()
	{
		return myDrawingNeeds;
	}

	virtual	unsigned int	GetSelfDrawingNeeds()
	{
		return ((unsigned int)Need_Predraw)|((unsigned int)Need_Draw)|((unsigned int)Need_Postdraw);
	}

	inline void SetDrawingNeeds(unsigned int needs)
	{
		myDrawingNeeds=needs;
	}

	bool IsSortable() { return mSortable; }

protected:
	maUInt mRenderPassMask = BASE_ATTRIBUTE(RenderPassMask, 0xffffffff);
	maBool mSortable = BASE_ATTRIBUTE(Sortable, false);

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

	enum	DRAWABLE_DRAWING_NEEDS
	{
		Not_Init		= 1,
		Need_Predraw	= 2,
		Need_Draw		= 4,
		Need_Postdraw	= 8
	};

	unsigned int	myDrawingNeeds;
}; 

#endif //_DRAWABLE_H_
