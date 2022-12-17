#ifndef _DRAWABLECALLBACK_H_
#define _DRAWABLECALLBACK_H_

#include "Drawable.h"

// ****************************************
// * DrawableCallback class
// * --------------------------------------
/**
 * \file	DrawableCallback.h
 * \class	DrawableCallback
 * \ingroup Renderer
 * \brief	Post notification when in Predraw, Draw and Postdraw. 
 *

 */
 // ****************************************
class DrawableCallback : public Drawable
{
public:
	DECLARE_CLASS_INFO(DrawableCallback,Drawable,Renderer)

	/**
	 * \brief	constructor
	 * \fn 		DrawableCallback(const std::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	 * \param	name : instance name
	 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	 */
	DrawableCallback(const std::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	//! overloaded 
	void	DoPreDraw(TravState*) override;
	//! overloaded 
	void	DoDraw(TravState* travstate) override;
	//! overloaded 
	void	DoPostDraw(TravState* travstate) override;

	unsigned int GetSelfDrawingNeeds() override
	{
		return ((unsigned int)Need_Predraw)|((unsigned int)Need_Draw)|((unsigned int)Need_Postdraw);
	}
};

#endif // _DRAWABLECALLBACK_H_