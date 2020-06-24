#ifndef _UITEXT_H_
#define _UITEXT_H_

#include "UITexturedItem.h"
#include "maUSString.h"

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


	inline void				SetAlignment(unsigned int a) { myTextAlign = a; }
	inline void				SetColor(kfloat R, kfloat G, kfloat B, kfloat A) { myColor[0] = R; myColor[1] = G; myColor[2] = B; myOpacity = A; }

	using					UITexturedItem::SetColor;
	kstl::string			GetText() { return myText.ToString(); }
	kstl::string			GetFontName() const { return myFont; }
	int						GetFontSize() const { return myFontSize; }
	int						GetDirection() const { return myDirection; }
	int						GetLength() const { return myLength; }
				
	void Set_FontSize(int size) { size != 0 ? myFontSize = size : myFontSize = 12; }
	void Set_FontName(const kstl::string& fontName) { fontName != "" ? myFont = fontName : myFont = "arial.ttf"; }

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

	maUSString				myText;
	maString				myFont;
	maUInt					myFontSize;
	maUInt					myDirection;
	maUInt					myLength;
	maUInt					myTextAlign;
	maUInt					myMaxWidth;
	maUInt					myMaxLines;
};

#endif //_UITEXT_H_