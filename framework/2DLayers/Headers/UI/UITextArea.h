#ifndef _UITEXTAREA_H_
#define _UITEXTAREA_H_

#include "UIDrawableItem.h"
#include "maUSString.h"

class Texture;
class UIVerticesInfo;

// ****************************************
// * UITextArea class
// * --------------------------------------
/**
* \file	UITextArea.h
* \class	UITextArea
* \ingroup 2DLayers
* \brief	Text input UI item.
*/
// ****************************************

class UITextArea : public UIDrawableItem
{
public:
	DECLARE_CLASS_INFO(UITextArea, UIDrawableItem, 2DLayers);

	/**
	 * \brief	constructor
	 * \param	name : instance name
	 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	 */
	UITextArea(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);
	virtual		~UITextArea();

	usString		GetText() const { return mText.us_str(); }
	std::string	GetFontName() const { return mFont; }
	std::string	GetReleaseAction() const { return mReleaseAction; }
	int				GetFontSize() const { return mFontSize; }
	int				GetLength() const { return mLength; }
	void			SetText(const unsigned short* value) { this->ChangeText(value); mIsDefaultText = false; }
	bool			GetIsDefaultText() const { return mIsDefaultText; }
	unsigned int	GetCol() const { return mCol; }
	unsigned int	GetRow() const { return mRow; }


	void			CreateFirstText();

	//void			UpdateText(std::vector<int> keycodeList);

	//bool			TriggerMouseClick(int buttonState, int buttonEvent, int X, int Y, bool & catchClick)override;
	bool			HasFocus()override { return GetNodeFlag(UIItem_HasFocus); }
	void			LoseFocus()override;
	void			GetFocus()override;

	virtual void	ReloadTexture();

	DECLARE_METHOD(UpdateKeyBoard);
	COREMODIFIABLE_METHODS(UpdateKeyBoard);

protected:
	void		InitModifiable() override;
	void		NotifyUpdate(const unsigned int /* labelid */)override;

	bool		isAlpha(float X, float Y)override;
	void		ProtectedDraw(TravState* state)override;

	void		ChangeText(const unsigned short* newText = NULL);
	void		ChangeText(const std::string& newText)
	{
		usString toChange(newText);
		ChangeText(toChange.us_str());
	}

	maUSString				mText;
	maString				mFont;
	maString				mReleaseAction;
	maUInt					mFontSize;
	maUInt					mLength;
	maUInt					mRow;
	maUInt					mCol;
	bool					mIsDown;
	bool					mStayPressed;
	maBool					mHasDefaultText;
	bool					mIsDefaultText;

	maUInt					mTextAlignment;

	SmartPointer<Texture>	mTexturePointer;
	UIVerticesInfo	*		mTextureQI; // draw BG quad
};
#endif //_UITEXTAREA_H_