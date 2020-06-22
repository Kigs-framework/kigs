#ifndef _UISLIDER_H_
#define _UISLIDER_H_

#include "UIDrawableItem.h"
#include "maUSString.h"
#include "Texture.h"

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

	Texture*		GetTexture();
	void			ChangeTexture(kstl::string texturename, kstl::string overtexturename = "", kstl::string downtexturename = "");
	bool			isPressed() const { return myIsDown; }
	void			ForcedChangeState(bool isdown) { myIsDown = isdown; }
	void			ForcedRatio(unsigned int value);
	void			ResetSlider();
	int				Get_CurrentRatio() const { return myCurrentRatio; }
	void			Set_CurrentRatio(int ratio) { myCurrentRatio = ratio; }
	int				Get_InitialRatio() const { return myInitialRatio; }
	int				Get_Multiplier() { return 100 / myInitialRatio; }
	void			ReloadTexture();

protected:
	virtual ~UISlider();
	void InitModifiable()override;
	void NotifyUpdate(const unsigned int /* labelid */)override;

	//bool TriggerMouseMove(bool over, float MouseDeltaX, float MouseDeltaY) override;
	//bool TriggerMouseClick(int buttonState, int buttonEvent, int X, int Y, bool & catchClick) override;
	bool isAlpha(float X, float Y)override;

	bool				myIsDown;
	bool				myIsMouseOver;

	SmartPointer<Texture>	myUpTexture;
	SmartPointer<Texture>	myDownTexture;
	SmartPointer<Texture>	myOverTexture;

	maString			myUpTextureName;
	maString			myDownTextureName;
	maString			myOverTextureName;
	maString			myClickUpAction;
	maUSString			myParameter;
	maString			myMoveAction;
	maInt				myInitialRatio;

	//! Direction of Slider "Vertical" or "Horizontal")
	maEnum<2>			myDirection;
	int					myInitialPosX;
	int					myInitialPosY;
	int					mySlideLength;
	int					myCurrentRatio;
};

#endif