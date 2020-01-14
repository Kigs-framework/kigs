#ifndef _UIIMAGE_H_
#define _UIIMAGE_H_

#include "UITexturedItem.h"
#include "TextureFileManager.h"

class UIImage : public UITexturedItem
{
public:
	enum DisplayMode
	{
		AUTO=0,
		RESIZE=1,
		SCALE=2,
		// TODO
		SHRINK=3,
		REPEAT=4
	};

	DECLARE_CLASS_INFO(UIImage, UITexturedItem, 2DLayers);

	/**
	 * \brief	constructor
	 * \param	name : instance name
	 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	 */
	UIImage(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	virtual void ChangeTexture();

protected:
	void InitModifiable() override;
	virtual void NotifyUpdate(const unsigned int /* labelid */)override;
	bool isAlpha(float X, float Y) override;
	void ComputeRealSize()override;

	kstl::string			myCurrentTextureName;

	Point2D					myAutoresizeValue;


	maBool					myKeepRatio = BASE_ATTRIBUTE(KeepRatio, true);
	maEnum<5>				myDisplayMode = BASE_ATTRIBUTE(DisplayMode, "Auto", "Resize", "Scale", "Shrink", "Repeat");
	//maBool					myAutoresizeTexture;
	maString				myTextureName;
	SmartPointer<SpriteSheetTexture>	mySpriteSheetTexture;
};

#endif //_UIIMAGE_H_