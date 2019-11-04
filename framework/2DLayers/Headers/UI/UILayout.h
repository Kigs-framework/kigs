#ifndef _UILayout_H_
#define _UILayout_H_

#include "UIPanel.h"

#include "AttributePacking.h"

// Adjust the position of all children according to a layout algorithm

class UILayout : public UIPanel
{
public:
	DECLARE_ABSTRACT_CLASS_INFO(UILayout, UIPanel, 2DLayers);

	/**
	* \brief	constructor
	* \param	name : instance name
	* \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	*/
	UILayout(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	bool			addItem(CoreModifiable *item, ItemPosition pos = Last DECLARE_DEFAULT_LINK_NAME)override;
	bool			removeItem(CoreModifiable* item DECLARE_DEFAULT_LINK_NAME)override;

	inline void		NeedRecomputeLayout() { myNeedRecompute = true; }

	WRAP_METHODS(NeedRecomputeLayout);
protected:
	void			NotifyUpdate(const unsigned int labelid) override;

	void			Update(const Timer& timer, void*) override;
	void			ProtectedDraw(TravState*) override;
	virtual void	RecomputeLayout() {};
	
	bool			myNeedRecompute;
};

#endif //_UILayout_H_