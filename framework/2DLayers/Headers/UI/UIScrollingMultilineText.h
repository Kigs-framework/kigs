#ifndef _UISCROLLINGMULTILINETEXT_H_
#define _UISCROLLINGMULTILINETEXT_H_

#include "UIScrollingText.h"

// ****************************************
// * UIScrollingMultilineText class
// * --------------------------------------
/**
* \file	UIScrollingMultilineText.h
* \class	UIScrollingMultilineText
* \ingroup 2DLayers
* \brief	?? Obsolete ??
*/
// ****************************************

class UIScrollingMultilineText : public UIScrollingText
{
public:
	DECLARE_CLASS_INFO(UIScrollingMultilineText, UIScrollingText,2DLayers);

	/**
	 * \brief	constructor
	 * \param	name : instance name
	 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	 */
	UIScrollingMultilineText(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);


protected:


};

#endif //_UISCROLLINGMULTILINETEXT_H_