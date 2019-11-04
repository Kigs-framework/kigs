#pragma once

#include "UIDrawableItem.h"

class Timer;

class UIAnimatedSprite : public UIDrawableItem
{
public:
	DECLARE_CLASS_INFO(UIAnimatedSprite, UIDrawableItem, 2DLayers);
	DECLARE_INLINE_CONSTRUCTOR(UIAnimatedSprite){}

	SpriteSheetTexture* GetSpriteSheetTexture() override;


	std::string	Get_CurrentAnimation() const { return myCurrentAnimation.const_ref(); }
	unsigned int Get_CurrentFrame() const {return m_CurrentFrame;}

	void Set_CurrentFrame(unsigned int currentf) { m_CurrentFrame = currentf; }
	void Set_FrameNumber(unsigned int value) { m_FrameNumber = value; }

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

	maString			myFileName = BASE_ATTRIBUTE(FileName, "");
	maString			myCurrentAnimation = BASE_ATTRIBUTE(CurrentAnimation, "");;
	maUInt				m_FramePerSecond = BASE_ATTRIBUTE(FramePerSecond, 0);;
	maBool				m_AnimationLoop = BASE_ATTRIBUTE(Loop, true);
	unsigned int		m_CurrentFrame = 0;
	double				m_dElpasedTime = 0.0;
	unsigned int		m_FrameNumber = 0;

	v2f mCurrentSourceSize;
};
