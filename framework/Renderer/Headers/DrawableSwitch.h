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
 * \ingroup Drawable
 * \ingroup RendererDrawable
 * \brief	
 * \author	ukn
 * \version ukn
 * \date	ukn
 *
 * Module Dependency :<br><ul><li>ModuleRenderer</li></ul>
 */
// ****************************************
class DrawableSwitch : public Drawable
{
public:
	DECLARE_CLASS_INFO(DrawableSwitch,Drawable,Renderer)

	/**
	 * \brief	constructor
	 * \fn 		DrawableSwitch(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	 * \param	name : instance name
	 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	 */
	DrawableSwitch(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	//! set current drawable by index in children list
	void	SetCurrentDrawable(int index);
	//! set current drawable by name in children list
	void	SetCurrentDrawable(kstl::string drawablename);
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
	 * \fn 		virtual bool	BBoxUpdate(kdouble time)
	 * \param	time : world time
	 * \return	FALSE if object without bounding box, TRUE otherwise
	 */
	bool	BBoxUpdate(kdouble time) override 
	{
		if(myNeedBBoxUpdate)
		{
			ComputeLocalBBox(time);
			myNeedBBoxUpdate=false;
		}
		return myChildrenHaveBbox;
	}

	/**
	 * \brief	add item in scene tree
	 * \fn 		bool	addItem(CoreModifiable *item, ItemPosition pos=Last DECLARE_DEFAULT_LINK_NAME) override ;
	 * \param	item : item to add
	 * \param	linkName : name of the added item
	 * \return	TRUE if the item is added, FALSE otherwise
	 */
	bool	addItem(CoreModifiable *item, ItemPosition pos=Last DECLARE_DEFAULT_LINK_NAME) override ;
	
	/**
	 * \brief	remove item in scene tree
	 * \fn 		bool	removeItem(CoreModifiable* item DECLARE_DEFAULT_LINK_NAME) override ;
	 * \param	item : item to remove
	 * \param	linkName : name of the removed item
	 * \return	TRUE if the item is removed, FALSE otherwise
	 */
	bool	removeItem(CoreModifiable* item DECLARE_DEFAULT_LINK_NAME) override ;

	/**
	 * \brief	retreive the bounding box of the bitmap (point min and point max)
	 * \fn 		void	GetBoundingBox(Point3D& pmin,Point3D& pmax) const  override  
	 * \param	pmin : point min of the bounding box (in/out param)
	 * \param	pmax : point max of the bounding box (in/out param)
	 */
	void	GetNodeBoundingBox(Point3D& pmin,Point3D& pmax) const override {pmin=myBoundingBox.m_Min; pmax=myBoundingBox.m_Max;}

	unsigned int	GetSelfDrawingNeeds() override
	{
		return 0;
	}

protected:
	/**
	 * \brief	calc the bounding box
	 * \fn 		void	ComputeLocalBBox(kdouble time);
	 * \param	time : global time
	 */ 
	void	ComputeLocalBBox(kdouble time);
		
	//! link to my bounding box
	BBox		myBoundingBox;
	//! TRUE if a child has a bounding box
	bool		myChildrenHaveBbox; 
	//! TRUE if a bounding box update is needed
	bool		myNeedBBoxUpdate;

	//! link to the current drawable
	Drawable*	myCurrentDrawable;
};

#endif // _DRAWABLE_SWITCH_H_