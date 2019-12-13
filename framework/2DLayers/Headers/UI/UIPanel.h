#ifndef _UIPANEL_H_
#define _UIPANEL_H_

#include "UIDrawableItem.h"

class UIPanel : public UIDrawableItem
{
public:
	DECLARE_CLASS_INFO(UIPanel, UIDrawableItem, 2DLayers);

	/**
	 * \brief	constructor
	 * \param	name : instance name
	 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	 */
	UIPanel(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	void GetColor(kfloat &R,kfloat &G,kfloat &B, kfloat &A) { R=myColor[0]; G=myColor[1]; B=myColor[2]; A= GetOpacity(); }

protected:
	bool isAlpha(float X, float Y) override;

};

#endif