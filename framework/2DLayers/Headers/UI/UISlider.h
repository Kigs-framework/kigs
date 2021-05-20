#ifndef _UISLIDER_H_
#define _UISLIDER_H_

#include "UIDrawableItem.h"
#include "maUSString.h"
#include "Texture.h"

// ****************************************
// * UISlider class
// * --------------------------------------
/**
* \file	UISlider.h
* \class	UISlider
* \ingroup 2DLayers
* \brief	Manage a slider.
*/
// ****************************************

class UISlider : public UIDrawableItem
{
public:
	DECLARE_CLASS_INFO(UISlider, UIDrawableItem, 2DLayers);

	/**
	 * \brief	constructor
	 * \param	name : instance name
	 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	 */
	UISlider(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);
	virtual ~UISlider();

	Texture*		GetTexture();
	void			ChangeTexture(kstl::string texturename, kstl::string overtexturename = "", kstl::string downtexturename = "");
	bool			isPressed() const { return mIsDown; }
	void			ForcedChangeState(bool isdown) { mIsDown = isdown; }
	void			ForcedRatio(unsigned int value);
	void			ResetSlider();
	int				Get_CurrentRatio() const { return mCurrentRatio; }
	void			Set_CurrentRatio(int ratio) { mCurrentRatio = ratio; }
	int				Get_InitialRatio() const { return mInitialRatio; }
	int				Get_Multiplier() { return 100 / mInitialRatio; }
	void			ReloadTexture();

protected:
	
	void InitModifiable()override;
	void NotifyUpdate(const unsigned int /* labelid */)override;

	//bool TriggerMouseMove(bool over, float MouseDeltaX, float MouseDeltaY) override;
	//bool TriggerMouseClick(int buttonState, int buttonEvent, int X, int Y, bool & catchClick) override;
	bool isAlpha(float X, float Y)override;

	bool				mIsDown;
	bool				mIsMouseOver;

	SmartPointer<Texture>	mUpTexturePointer;
	SmartPointer<Texture>	mDownTexturePointer;
	SmartPointer<Texture>	mOverTexturePointer;

	maString			mUpTexture;
	maString			mDownTexture;
	maString			mOverTexture;
	maString			mClickUpAction;
	maUSString			mParameter;
	maString			mMoveAction;
	maInt				mInitialRatio;

	//! Direction of Slider "Vertical" or "Horizontal")
	maEnum<2>			mDirection;
	int					mInitialPosX;
	int					mInitialPosY;
	int					mSlideLength;
	int					mCurrentRatio;
};

#endif