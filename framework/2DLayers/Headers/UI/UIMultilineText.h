#ifndef _UIMULTILINETEXT_H_
#define _UIMULTILINETEXT_H_

#include "UIText.h"

class Texture;

class UIMultilineText : public UIText
{
public:
	DECLARE_CLASS_INFO(UIMultilineText, UIText, 2DLayers);

	/**
	 * \brief	constructor
	 * \param	name : instance name
	 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	 */
	UIMultilineText(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

protected:



};

#endif //_UIMULTILINETEXT_H_