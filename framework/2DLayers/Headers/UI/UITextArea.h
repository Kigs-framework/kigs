#ifndef _UITEXTAREA_H_
#define _UITEXTAREA_H_

#include "UIDrawableItem.h"
#include "maUSString.h"

class Texture;
class UIVerticesInfo;

class UITextArea : public UIDrawableItem
{
public:
	DECLARE_CLASS_INFO(UITextArea, UIDrawableItem, 2DLayers);

	/**
	 * \brief	constructor
	 * \param	name : instance name
	 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	 */
	UITextArea(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	//void			GetColor(kfloat &R, kfloat &G, kfloat &B, kfloat &A) { R = myTextColor[0]; G = myTextColor[1]; B = myTextColor[2]; A = myTextColor[3]; }
	usString		GetText() const { return myText.us_str(); }
	kstl::string	GetFontName() const { return myFont; }
	kstl::string	GetReleaseAction() const { return myReleaseAction; }
	int				GetFontSize() const { return myFontSize; }
	int				GetLength() const { return myLength; }
	void			SetText(const unsigned short* value) { this->ChangeText(value); myIsDefaultText = false; }
	bool			GetIsDefaultText() const { return myIsDefaultText; }
	unsigned int	GetCol() const { return myCol; }
	unsigned int	GetRow() const { return myRow; }


	void			CreateFirstText();

	//void			UpdateText(kstl::vector<int> keycodeList);

	//bool			TriggerMouseClick(int buttonState, int buttonEvent, int X, int Y, bool & catchClick)override;
	bool			HasFocus()override { return myFocus; }
	void			LoseFocus()override;
	void			GetFocus()override;

	virtual void	ReloadTexture();

	DECLARE_METHOD(UpdateKeyBoard);
	COREMODIFIABLE_METHODS(UpdateKeyBoard);
protected:
	virtual		~UITextArea();

	void		InitModifiable() override;
	void		NotifyUpdate(const unsigned int /* labelid */)override;

	bool		isAlpha(float X, float Y)override;
	void		ProtectedDraw(TravState* state)override;

	void		ChangeText(const unsigned short* newText = NULL);
	void		ChangeText(const kstl::string& newText)
	{
		usString toChange(newText);
		ChangeText(toChange.us_str());
	}

	//maVect4DF				myTextColor;
	maUSString				myText;
	maString				myFont;
	maString				myReleaseAction;
	maUInt					myFontSize;
	maUInt					myLength;
	maUInt					myRow;
	maUInt					myCol;
	bool					myIsDown;
	bool					myStayPressed;
	maBool					myHasDefaultText;
	bool					myIsDefaultText;

	maUInt					myTextAlign;

	SmartPointer<Texture>	myTexture;
	UIVerticesInfo	*		mTextureQI; // draw BG quad
};
#endif //_UITEXTAREA_H_