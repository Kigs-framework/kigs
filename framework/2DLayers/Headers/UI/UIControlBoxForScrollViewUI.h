
#include "UIItem.h"
#include "RenderingScreen.h"
#include "RendererIncludes.h"
#include "RendererDefines.h"
#include "ModuleRenderer.h"
#include "RenderingScreen.h"

// ****************************************
// * UIControlBoxForScrollViewUI class
// * --------------------------------------
/**
* \file	UIControlBoxForScrollViewUI.h
* \class	UIControlBoxForScrollViewUI
* \ingroup 2DLayers
* \brief	???
*
*/
// ****************************************

class UIControlBoxForScrollViewUI : public UIItem
{
public:
	DECLARE_CLASS_INFO(UIControlBoxForScrollViewUI, UIItem, Renderer);
	DECLARE_INLINE_CONSTRUCTOR(UIControlBoxForScrollViewUI)
	, mMoving(false)
	, mHasMoved(false)
	, mTimeSinceClick(0)
	{
		mPriority = -1;
	}

	bool scrollTo(float deltaPos);

	//bool TriggerMouseMove(bool over, float MouseDeltaX, float MouseDeltaY) override;
	//bool TriggerMouseClick(int buttonState, int buttonEvent, int X, int Y, bool & catchClick) override;
	//void TriggerMouseSwipe(int idxButton, float Vx, float Vy) override;
	void CompensatePosition(float deltaPos, int axis) { mPosition[axis] -= deltaPos; }

protected:

	void Update(const Timer& a_timer, void* addParam) override;

	enum eAnimationState
	{
		ANIMATIONSTATE_STOPPED,
		ANIMATIONSTATE_INITIALIZATION,
		ANIMATIONSTATE_RUNNING,
		ANIMATIONSTATE_INTERRUPT,
		ANIMATIONSTATE_RUNNING_INTERRUP,
		ANIMATIONSTATE_PREPARE_REPLACE, ANIMATIONSTATE_RUNNING_REPLACE,
		ANIMATIONSTATE_MOUSECONTROL
	};

	bool	mMoving;
	bool	mHasMoved;
	double	mTimeSinceClick;
	float	mInitialVelocity;
	eAnimationState	mAnimation;
	double	mStartTime;
	double	mEndTime;
	float	mConstantAcceleration;

};