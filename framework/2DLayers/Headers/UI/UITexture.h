#ifndef _UITEXTURE_H_
#define _UITEXTURE_H_

#include "UITexturedItem.h"
#include "Texture.h"

// ****************************************
// * UITexture class
// * --------------------------------------
/**
* \file	UITexture.h
* \class	UITexture
* \ingroup 2DLayers
* \brief	Draw a texture.
*
* Obsolete ? Same as UIImage ?
*
*/
// ****************************************

class UITexture : public UITexturedItem
{
public:
	DECLARE_CLASS_INFO(UITexture, UITexturedItem, 2DLayers);

	/**
	 * \brief	constructor
	 * \param	name : instance name
	 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	 */
	UITexture(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	void PreDraw(TravState* state) override;  // recompute all vertices

protected:
	virtual ~UITexture();
};

#endif //_UITEXTURE_H_
