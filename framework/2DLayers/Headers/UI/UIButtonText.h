#ifndef _UIBUTTONTEXT_H_
#define _UIBUTTONTEXT_H_

#include "UIButton.h"
#include "maUSString.h"

// ****************************************
// * UIButtonText class
// * --------------------------------------
/**
* \file	UIButtonText.h
* \class	UIButtonText
* \ingroup 2DLayers
* \brief	Three state button with different texts.
*
* Probably obsolete. Perhaps too keep has an upgrador
*/
// ****************************************

class UIButtonText : public UIButton
{
public:
	DECLARE_CLASS_INFO(UIButtonText, UIButton, 2DLayers);

	/**
	 * \brief	constructor
	 * \param	name : instance name
	 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	 */
	UIButtonText(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	void				ChangeTexture(kstl::string & texturename, kstl::string & overtexturename, kstl::string & downtexturename);
	void				ChangeTextureColor(Vector4D& UpColor, Vector4D& OverColor, Vector4D& DownColor);
	void				GetUpColor(float& R, float& G, float& B);
	void				GetOverColor(float& R, float& G, float& B);
	void				GetDownColor(float& R, float& G, float& B);

protected:
	
	void InitModifiable()override;
	void NotifyUpdate(const unsigned int labelid)override;
	void ChangeState()override;

	void				ChangeTextTexture(const kstl::string & text, unsigned int texture);
	void				ChangeTextTexture(const unsigned short* text, unsigned int texture);
	unsigned short*		CutText(unsigned short* text, bool& flag);
	char*				CutText(const char* text, bool& flag);

	DECLARE_METHOD(ReloadTexture);
	COREMODIFIABLE_METHODS(ReloadTexture);

	SmartPointer<Texture>				mUpTexturePointer;
	SmartPointer<Texture>				mDownTexturePointer;
	SmartPointer<Texture>				mOverTexturePointer;

	maString			mUpText;
	maString			mDownText;
	maString			mOverText;

	maVect4DF			mUpColor;
	maVect4DF			mOverColor;
	maVect4DF			mDownColor;

	maString			mFont;
	maUInt				mFontSize;
	maUInt				mLength;
	maUInt				mTextAlignment;
};

#endif //_UIBUTTONTEXT_H_