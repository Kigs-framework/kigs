#ifndef _UISPRITE_H_
#define _UISPRITE_H_

#include "UITexturedItem.h"
#include "Texture.h"

class SpriteSheetTexture;
struct FRAMES;

// ****************************************
// * UISprite class
// * --------------------------------------
/**
* \file	UISprite.h
* \class	UISprite
* \ingroup 2DLayers
* \brief	Manage sprite. Obsolete ?
*
* Obsolete ??
*/
// ****************************************

class UISprite : public UITexturedItem
{
public:
	DECLARE_CLASS_INFO(UISprite, UITexturedItem, 2DLayers);

	/**
	 * \brief	constructor
	 * \param	name : instance name
	 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	 */
	UISprite(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	void				ChangeTexture();

protected:
	virtual ~UISprite();

	void InitModifiable()override;
	void NotifyUpdate(const unsigned int /* labelid */)override;
	bool isAlpha(float X, float Y)override;
	void SetTexUV(UIVerticesInfo * aQI)override;

	Point2D mUVMin, mUVMax;

	bool mHasSprite;

	maString				mTexture;
	maString				mSprite;

	SmartPointer<SpriteSheetTexture>	mSpriteSheet;
};

#endif //_UIIMAGE_H_