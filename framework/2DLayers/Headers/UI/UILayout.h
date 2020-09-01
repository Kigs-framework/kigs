#ifndef _UILayout_H_
#define _UILayout_H_

#include "UIDrawableItem.h"

#include "AttributePacking.h"


// ****************************************
// * UILayout class
// * --------------------------------------
/**
* \file	UILayout.h
* \class	UILayout
* \ingroup 2DLayers
* \brief	Adjust the position of all children according to a layout algorithm
*/
// ****************************************

class UILayout : public UIDrawableItem
{
public:
	DECLARE_ABSTRACT_CLASS_INFO(UILayout, UIDrawableItem, 2DLayers);

	/**
	* \brief	constructor
	* \param	name : instance name
	* \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	*/
	UILayout(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	bool			addItem(const CMSP& item, ItemPosition pos = Last DECLARE_DEFAULT_LINK_NAME)override;
	bool			removeItem(const CMSP& item DECLARE_DEFAULT_LINK_NAME)override;

	inline void		NeedRecomputeLayout() { mNeedRecompute = true; }

	WRAP_METHODS(NeedRecomputeLayout);
protected:
	void			NotifyUpdate(const unsigned int labelid) override;

	void			Update(const Timer& timer, void*) override;
	void			ProtectedDraw(TravState*) override;
	virtual void	RecomputeLayout() {};
	
	bool			mNeedRecompute;
};

#endif //_UILayout_H_