#ifndef _UIANIMATEDBUTTON_H_
#define _UIANIMATEDBUTTON_H_

#include "UIButton.h"
#include "CoreMap.h"
#include "CoreVector.h"
#include "maUSString.h"
#include "SpriteSheetTexture.h"
class Timer;

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


	inline kstl::string	Get_CurrentAnimation() const { return myCurrentAnimation; }
	inline unsigned int	Get_CurrentFrame() const { return m_CurrentFrame; }

	inline void			Set_CurrentFrame(unsigned int currentf) { m_CurrentFrame = currentf; }
	inline void			Set_FrameNumber(unsigned int value) { m_FrameNumber = value; }

	void				ChangeState() override;
	SpriteSheetTexture* GetSpriteSheetTexture() override;

protected:
	virtual								~UIAnimatedButton();
	void						InitModifiable() override;
	bool						isAlpha(float X, float Y) override;
	void						Update(const Timer& timer, void* addParam) override;
	void						NotifyUpdate(const unsigned int /* labelid */) override;
	void SetTexUV(UIVerticesInfo * aQI) override;

	virtual void								LoadTexture(SmartPointer<SpriteSheetTexture>& a_textptr, const kstl::string& a_filename);

	void								UpdateAnimatedValues();

	SmartPointer<SpriteSheetTexture>	myUpTexture;
	SmartPointer<SpriteSheetTexture>	myDownTexture;
	SmartPointer<SpriteSheetTexture>	myOverTexture;

	SmartPointer<SpriteSheetTexture>	myCurrentTexture;
	kstl::string						myCurrentAnimation;

	maString							myUpTextureName;
	maString							myDownTextureName;
	maString							myOverTextureName;

	maString							myDefaultUpAnimation;
	maString							myDefaultDownAnimation;
	maString							myDefaultOverAnimation;
	maBool								m_AnimationLoop;	// if no loop, stay at the end of the animation
	maUInt								m_FramePerSecond;
	unsigned int						m_CurrentFrame;
	float								m_AnimationSpeed;
	double								m_dElpasedTime;
	unsigned int						m_FrameNumber;
	bool								mybPreviousStateDown;
	bool								mybPreviousStateOver;
};

#endif //_UIANIMATEDBUTTON_H_