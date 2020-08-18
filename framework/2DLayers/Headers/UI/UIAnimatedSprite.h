#pragma once

#include "UIDrawableItem.h"

class Timer;

// ****************************************
// * UIAnimatedSprite class
// * --------------------------------------
/**
* \file	UIAnimatedSprite.h
* \class	UIAnimatedSprite
* \ingroup 2DLayers
* \brief	Display an animated texture
*
*/
// ****************************************

class UIAnimatedSprite : public UIDrawableItem
{
public:
	DECLARE_CLASS_INFO(UIAnimatedSprite, UIDrawableItem, 2DLayers);
	DECLARE_INLINE_CONSTRUCTOR(UIAnimatedSprite){}

	SpriteSheetTexture* GetSpriteSheetTexture() override;


	std::string	Get_CurrentAnimation() const { return mCurrentAnimation.const_ref(); }
	unsigned int Get_CurrentFrame() const {return mCurrentFrame;}

	void Set_CurrentFrame(unsigned int currentf) { mCurrentFrame = currentf; }
	void Set_FrameNumber(unsigned int value) { mFrameNumber = value; }

protected:
	void InitModifiable() override;
	bool isAlpha(float X, float Y) override;
	void Update(const Timer& timer, void* addParam) override;
	void NotifyUpdate(const unsigned int labelid) override;

	void SetVertexArray(UIVerticesInfo * aQI) override;
	void SetTexUV(UIVerticesInfo * aQI) override;

	void ComputeRealSize() override;

	int GetTransparencyType() override;
	void PreDraw(TravState* state) override; 
	void PostDraw(TravState* state) override;

	void ChangeAnimation(const std::string& newAnimation);
	void ChangeSpriteSheet(const std::string& filename);

	SmartPointer<SpriteSheetTexture> myTexture;

	maString			mFileName = BASE_ATTRIBUTE(FileName, "");
	maString			mCurrentAnimation = BASE_ATTRIBUTE(CurrentAnimation, "");;
	maUInt				mFramePerSecond = BASE_ATTRIBUTE(FramePerSecond, 0);;
	maBool				mLoop = BASE_ATTRIBUTE(Loop, true);
	unsigned int		mCurrentFrame = 0;
	double				mElpasedTime = 0.0;
	unsigned int		mFrameNumber = 0;

	v2f					mCurrentSourceSize;
};
