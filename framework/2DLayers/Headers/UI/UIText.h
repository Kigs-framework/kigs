#ifndef _UITEXT_H_
#define _UITEXT_H_

#include "UITexturedItem.h"
#include "maUSString.h"

// ****************************************
// * UIText class
// * --------------------------------------
/**
* \file	UIText.h
* \class	UIText
* \ingroup 2DLayers
* \brief	Display a text.
* 
* The text is drawn to a texture each time it is changed. For text with fixed size / font changing often, use UIDynamicText insteed.
* 
*/
// ****************************************

class UIText : public UITexturedItem
{
public:
	DECLARE_CLASS_INFO(UIText, UITexturedItem, 2DLayers);

	/**
	 * \brief	constructor
	 * \param	name : instance name
	 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	 */
	UIText(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);


	inline void				SetAlignment(unsigned int a) { mTextAlignment = a; }
	inline void				SetColor(kfloat R, kfloat G, kfloat B, kfloat A) { mColor[0] = R; mColor[1] = G; mColor[2] = B; mOpacity = A; }

	using					UITexturedItem::SetColor;
	kstl::string			GetText() { return mText.ToString(); }
	kstl::string			GetFontName() const { return mFont; }
	int						GetFontSize() const { return mFontSize; }
	int						GetDirection() const { return mDirection; }
	int						GetLength() const { return mLength; }
				
	void Set_FontSize(int size) { size != 0 ? mFontSize = size : mFontSize = 12; }
	void Set_FontName(const kstl::string& fontName) { fontName != "" ? mFont = fontName : mFont = "arial.ttf"; }

	void			NotifyUpdate(const unsigned int /* labelid */) override;
	
protected:
	void			InitModifiable() override;

	bool			isAlpha(float X, float Y) override;
	//bool			TriggerMouseMove(bool over, float MouseDeltaX, float MouseDeltaY) override;
	//bool			TriggerMouseClick(int buttonState, int buttonEvent, int X, int Y, bool & catchClick) override;

	virtual void	ChangeText(const kstl::string& newText);
	virtual void	ChangeText(const usString& newText);
	
	DECLARE_METHOD(ReloadTexture);
	COREMODIFIABLE_METHODS(ReloadTexture);
	unsigned short*			CutText(const unsigned short* text, bool& flag);

	maUSString				mText;
	maString				mFont;
	maUInt					mFontSize;
	maUInt					mDirection;
	maUInt					mLength;
	maUInt					mTextAlignment;
	maUInt					mMaxWidth;
	maUInt					mMaxLines;
};

#endif //_UITEXT_H_