#ifndef _DRAWABLE_SWITCH_H_
#define _DRAWABLE_SWITCH_H_

#include "TecLibs/Tec3D.h"
#include "Drawable.h"
#include "TecLibs/3D/3DObject/BBox.h"

// ****************************************
// * DrawableSwitch class
// * --------------------------------------
/**
 * \file	DrawableSwitch.h
 * \class	DrawableSwitch
 * \ingroup Renderer
 * \brief	Draw only one selected son.
 */
 // ****************************************
class DrawableSwitch : public Drawable
{
public:
	DECLARE_CLASS_INFO(DrawableSwitch,Drawable,Renderer)

	/**
	 * \brief	constructor
	 * \fn 		DrawableSwitch(const std::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	 * \param	name : instance name
	 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	 */
	DrawableSwitch(const std::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	//! set current drawable by index in children list
	void	SetCurrentDrawable(int index);
	//! set current drawable by name in children list
	void	SetCurrentDrawable(std::string drawablename);
	//! set current drawable by comparison in children list
	void	SetCurrentDrawable(Drawable* drawable);

	//! overloaded to be called only on current drawable son
	void	DoPreDraw(TravState*) override;
	//! overloaded to be called only on current drawable son
	void	DoDraw(TravState* travstate) override;
	//! overloaded to be called only on current drawable son
	void	DoPostDraw(TravState* travstate) override;

	/**
	 * \brief	update the bounding box
	 * \fn 		virtual bool	BBoxUpdate(double time)
	 * \param	time : world time
	 * \return	FALSE if object without bounding box, TRUE otherwise
	 */
	bool	BBoxUpdate(double time) override 
	{
		if(mNeedBBoxUpdate)
		{
			ComputeLocalBBox(time);
			mNeedBBoxUpdate=false;
		}
		return mChildrenHaveBbox;
	}

	/**
	 * \brief	add item in scene tree
	 * \fn 		bool	addItem(const CMSP& item, ItemPosition pos=Last DECLARE_DEFAULT_LINK_NAME) override ;
	 * \param	item : item to add
	 * \param	linkName : name of the added item
	 * \return	TRUE if the item is added, FALSE otherwise
	 */
	bool	addItem(const CMSP& item, ItemPosition pos=Last DECLARE_DEFAULT_LINK_NAME) override ;
	
	/**
	 * \brief	remove item in scene tree
	 * \fn 		bool	removeItem(const CMSP& item DECLARE_DEFAULT_LINK_NAME) override ;
	 * \param	item : item to remove
	 * \param	linkName : name of the removed item
	 * \return	TRUE if the item is removed, FALSE otherwise
	 */
	bool	removeItem(const CMSP& item DECLARE_DEFAULT_LINK_NAME) override ;

	/**
	 * \brief	retreive the bounding box of the bitmap (point min and point max)
	 * \fn 		void	GetBoundingBox(Point3D& pmin,Point3D& pmax) const  override  
	 * \param	pmin : point min of the bounding box (in/out param)
	 * \param	pmax : point max of the bounding box (in/out param)
	 */
	void	GetNodeBoundingBox(Point3D& pmin,Point3D& pmax) const override {pmin=mBoundingBox.m_Min; pmax=mBoundingBox.m_Max;}

	unsigned int	GetSelfDrawingNeeds() override
	{
		return 0;
	}

protected:
	/**
	 * \brief	calc the bounding box
	 * \fn 		void	ComputeLocalBBox(double time);
	 * \param	time : global time
	 */ 
	void	ComputeLocalBBox(double time);
		
	//! link to my bounding box
	BBox		mBoundingBox;
	//! TRUE if a child has a bounding box
	bool		mChildrenHaveBbox; 
	//! TRUE if a bounding box update is needed
	bool		mNeedBBoxUpdate;

	//! link to the current drawable
	Drawable*	mCurrentDrawable;
};

#endif // _DRAWABLE_SWITCH_H_