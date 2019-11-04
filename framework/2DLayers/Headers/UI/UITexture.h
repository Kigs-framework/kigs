#ifndef _UITEXTURE_H_
#define _UITEXTURE_H_

#include "UITexturedItem.h"
#include "Texture.h"

class UITexture : public UITexturedItem
{
public:
	DECLARE_CLASS_INFO(UITexture, UITexturedItem, 2DLayers);

	/**
	 * \brief	constructor
	 * \param	name : instance name
	 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	 */
	UITexture(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	void PreDraw(TravState* state) override;  // recompute all vertices

protected:
	virtual ~UITexture();
};

#endif //_UITEXTURE_H_
