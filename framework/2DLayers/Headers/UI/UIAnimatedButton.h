#ifndef _UIANIMATEDBUTTON_H_
#define _UIANIMATEDBUTTON_H_

#include "UIButton.h"
#include "CoreMap.h"
#include "CoreVector.h"
#include "maUSString.h"
#include "SpriteSheetTexture.h"
class Timer;

// ****************************************
// * UIAnimatedButton class
// * --------------------------------------
/**
* \file	UIAnimatedButton.h
* \class	UIAnimatedButton
* \ingroup 2DLayers
* \brief	Textured button with spritesheets
*
*/
// ****************************************

class UIAnimatedButton : public UIButton
{
public:
	DECLARE_CLASS_INFO(UIAnimatedButton, UIButton, 2DLayers);

	/**
	 * \brief	constructor
	 * \param	name : instance name
	 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	 */
	UIAnimatedButton(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);


	inline kstl::string	Get_CurrentAnimation() const { return mCurrentAnimation; }
	inline unsigned int	Get_CurrentFrame() const { return mCurrentFrame; }

	inline void			Set_CurrentFrame(unsigned int currentf) { mCurrentFrame = currentf; }
	inline void			Set_FrameNumber(unsigned int value) { mFrameNumber = value; }

	void				ChangeState() override;
	SpriteSheetTexture* GetSpriteSheetTexture() override;

protected:
	virtual						~UIAnimatedButton();
	void						InitModifiable() override;
	bool						isAlpha(float X, float Y) override;
	void						Update(const Timer& timer, void* addParam) override;
	void						NotifyUpdate(const unsigned int /* labelid */) override;
	void SetTexUV(UIVerticesInfo * aQI) override;

	virtual void								LoadTexture(SmartPointer<SpriteSheetTexture>& a_textptr, const kstl::string& a_filename);

	void								UpdateAnimatedValues();

	SmartPointer<SpriteSheetTexture>	mUpTexturePointer;
	SmartPointer<SpriteSheetTexture>	mDownTexturePointer;
	SmartPointer<SpriteSheetTexture>	mOverTexturePointer;

	SmartPointer<SpriteSheetTexture>	mCurrentTexture;
	kstl::string						mCurrentAnimation;

	maString							mUpTexture;
	maString							mDownTextureName;
	maString							mOverTextureName;

	maString							mDefaultUpAnimation;
	maString							mDefaultDownAnimation;
	maString							mDefaultOverAnimation;
	maBool								mLoop;	// if no loop, stay at the end of the animation
	maUInt								mFramePerSecond;
	unsigned int						mCurrentFrame;
	float								mAnimationSpeed;
	double								mElpasedTime;
	unsigned int						mFrameNumber;
	bool								mPreviousStateDown;
	bool								mPreviousStateOver;
};

#endif //_UIANIMATEDBUTTON_H_