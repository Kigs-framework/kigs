#ifndef _UISLIDERFILL_H_
#define _UISLIDERFILL_H_

#include "UIDrawableItem.h"

class Texture;

class UISliderFill : public UIDrawableItem
{
public:
	DECLARE_CLASS_INFO(UISliderFill, UIDrawableItem, 2DLayers);

	/**
	 * \brief	constructor
	 * \param	name : instance name
	 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	 */
	UISliderFill(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	inline SP<Texture>	GetVoidTexture() { return myVoidTexture; }
	inline SP<Texture>  GetStartTexture() { if (myStartPositionX > -1 && myStartPositionY > -1) return myStartTexture; else return nullptr; }
	inline SP<Texture>  GetMiddleTexture() { if (myMiddlePositionX > -1 && myMiddlePositionY > -1) return myMiddleTexture; else return nullptr; }
	inline SP<Texture>  GetEndTexture() { if (myEndPositionX > -1 && myEndPositionY > -1) return myEndTexture; else return nullptr; }

	inline void			Get_StartPosition(int& X, int& Y) { X = myStartPositionX; Y = myStartPositionY; }
	inline void			Get_MiddlePosition(int& X, int& Y) { X = myMiddlePositionX; Y = myMiddlePositionY; }
	inline void			Get_MiddleSize(int& X, int& Y) { X = myMiddleSizeX; Y = myMiddleSizeY; }
	inline void			Get_EndPosition(int& X, int& Y) { X = myEndPositionX; Y = myEndPositionY; }
	void				ComputeInitialElementsPosition();

	void				ChangeTexture(kstl::string voidtexturename, kstl::string texturename = "", kstl::string overtexturename = "", kstl::string downtexturename = "");

protected:

	void		InitModifiable()override;
	void		NotifyUpdate(const unsigned int /* labelid */)override;
	bool		isAlpha(float X, float Y)override;
	bool		TriggerMouseClick(int buttonState, int buttonEvent, int X, int Y, bool & catchClick)override;
	void		TreatMove(int X, int Y, int oldValue, int desiredValue, UIItem* sender)override;

	void		RecomputeElementsPosition(float dockX, float dockY, float AnchorX, float AnchorY, int posx, int posy, unsigned int sizeX, unsigned int sizeY, int& Xresult, int& Yresult);
	
	maString			myVoidTextureName;
	maString			myStartTextureName;
	maString			myMiddleTextureName;
	maString			myEndTextureName;

	//! Direction of Slider "Vertical" or "Horizontal")
	maEnum<2>			myDirection;

	SP<Texture>			myStartTexture;
	SP<Texture>			myMiddleTexture;
	SP<Texture>			myEndTexture;
	SP<Texture>			myVoidTexture;

	int					myStartPositionX;
	int					myStartPositionY;
	int					myMiddlePositionX;
	int					myMiddlePositionY;
	int					myMiddleSizeX;
	int					myMiddleSizeY;
	int					myEndPositionX;
	int					myEndPositionY;
};
#endif //_UISLIDERFILL_H_