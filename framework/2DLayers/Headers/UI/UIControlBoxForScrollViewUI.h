
#include "UIItem.h"
#include "RenderingScreen.h"
#include "RendererIncludes.h"
#include "RendererDefines.h"
#include "ModuleRenderer.h"
#include "RenderingScreen.h"

class UIControlBoxForScrollViewUI : public UIItem
{
public:
	DECLARE_CLASS_INFO(UIControlBoxForScrollViewUI, UIItem, Renderer);
	DECLARE_INLINE_CONSTRUCTOR(UIControlBoxForScrollViewUI)
	, mMoving(false)
	, hasMoved(false)
	, timeSinceClick(0)
	{
		myPriority = -1;
	}

	bool scrollTo(kfloat deltaPos);

	//bool TriggerMouseMove(bool over, float MouseDeltaX, float MouseDeltaY) override;
	//bool TriggerMouseClick(int buttonState, int buttonEvent, int X, int Y, bool & catchClick) override;
	//void TriggerMouseSwipe(int idxButton, kfloat Vx, kfloat Vy) override;
	void CompensatePosition(kfloat deltaPos, int axis) { myPosition[axis] -= deltaPos; }

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
	bool	hasMoved;
	double	timeSinceClick;
	kfloat	myInitialVelocity;
	eAnimationState	mAnimation;
	kdouble	mStartTime;
	kdouble	mEndTime;
	kfloat	myConstantAcceleration;

};