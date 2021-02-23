#ifndef _UIBUTTONIMAGE_H_
#define _UIBUTTONIMAGE_H_

#include "UIButton.h"
#include "maUSString.h"

// ****************************************
// * UIButtonImage class
// * --------------------------------------
/**
* \file	UIButtonImage.h
* \class	UIButtonImage
* \ingroup 2DLayers
* \brief	Button with three textures ( Up, Down , Over )
*
*/
// ****************************************

class UIButtonImage : public UIButton
{
public:
	DECLARE_CLASS_INFO(UIButtonImage, UIButton, 2DLayers);

	/**
	 * \brief	constructor
	 * \param	name : instance name
	 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	 */
	UIButtonImage(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	void ChangeTexture(kstl::string texturename, kstl::string overtexturename = "", kstl::string downtexturename = "");

protected:
	virtual ~UIButtonImage();
	void InitModifiable()override;
	void NotifyUpdate(const unsigned int /* labelid */)override;

	void	ChangeState()override;

	void	ReloadTexture();

	SmartPointer<TextureHandler>				mUpTexturePointer;
	SmartPointer<TextureHandler>				mDownTexturePointer;
	SmartPointer<TextureHandler>				mOverTexturePointer;

	maString	mUpTexture;
	maString	mDownTexture;
	maString	mOverTexture;
};

#endif //_UIBUTTONIMAGE_H_