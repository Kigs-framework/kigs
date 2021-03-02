#ifndef _UIIMAGE_H_
#define _UIIMAGE_H_

#include "UITexturedItem.h"
#include "TextureFileManager.h"

// ****************************************
// * UIImage class
// * --------------------------------------
/**
* \file	UIImage.h
* \class	UIImage
* \ingroup 2DLayers
* \brief	Just display an image ( texture ) with different modes.
*/
// ****************************************

class UIImage : public UITexturedItem
{
public:

	DECLARE_CLASS_INFO(UIImage, UITexturedItem, 2DLayers);

	/**
	 * \brief	constructor
	 * \param	name : instance name
	 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	 */
	UIImage(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	virtual void ChangeTexture();

protected:



	void InitModifiable() override;
	//virtual void NotifyUpdate(const unsigned int /* labelid */)override;
	bool isAlpha(float X, float Y) override;

};

#endif //_UIIMAGE_H_