#ifndef _UIBUTTONIMAGE_H_
#define _UIBUTTONIMAGE_H_

#include "UIButton.h"
#include "maUSString.h"

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

	SmartPointer<Texture>				myUpTexture;
	SmartPointer<Texture>				myDownTexture;
	SmartPointer<Texture>				myOverTexture;

	maString	myUpTextureName;
	maString	myDownTextureName;
	maString	myOverTextureName;
};

#endif //_UIBUTTONIMAGE_H_