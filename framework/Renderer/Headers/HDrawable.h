#ifndef _HDRAWABLE_H_
#define _HDRAWABLE_H_

#include "CoreModifiable.h"
#include "TecLibs/Tec3D.h"
#include "Drawable.h"

class TravState;

// ****************************************
// * HDrawable class
// * --------------------------------------
/**
 * \file	HDrawable.h
 * \class	HDrawable
 * \ingroup Drawable
 * \ingroup RendererDrawable
 * \brief	A HDrawable is Drawable with a different draw policy
 * \author	ukn
 * \version ukn
 * \date	ukn
 */
// ****************************************
class HDrawable : public Drawable 
{
public:
	DECLARE_ABSTRACT_CLASS_INFO(HDrawable,Drawable,Renderer)

	/**
	 * \brief	constructor
	 * \fn 		HDrawable(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	 * \param	name : instance name
	 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	 */
	HDrawable(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	/**
	 * \brief	pre draw method
	 * \fn 		virtual void	DoPreDraw(TravState*);
	 * \param	TravState : camera state
	 */
	void	DoPreDraw(TravState*) override;

	/**
	 * \brief	draw method
	 * \fn 		virtual void	DoDraw(TravState* travstate);
	 * \param	travstate : camera state
	 */
	void	DoDraw(TravState* travstate) override;

	/**
	 * \brief	post draw method
	 * \fn 		virtual void	DoPostDraw(TravState* travstate);
	 * \param	travstate : camera state
	 */
	void	DoPostDraw(TravState* travstate) override;

	unsigned int	GetSelfDrawingNeeds() override
	{
		return ((unsigned int)Need_Predraw)|((unsigned int)Need_Draw);
	}

	void		UpdateDrawingNeeds() override;

protected:
	/**
	 * \brief	destructor
	 * \fn 		~HDrawable();
	 */
	virtual ~HDrawable();
};

#endif //_HDRAWABLE_H_
