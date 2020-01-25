// #### 2DLayers Include 
#include "UI/UIItem.h"
#include "ModuleInput.h"
#include "BaseUI2DLayer.h"
#include "NotificationCenter.h"
#include "AttributePacking.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(UIItem, UIItem, 2DLayers);
IMPLEMENT_CLASS_INFO(UIItem)

IMPLEMENT_CONSTRUCTOR(UIItem)
, myIsHidden(*this, false, "IsHidden", false)
, mybIsTouchable(*this, false, "IsTouchable", true)
, mybDisableBlend(*this, false, "DisableBlend", true)
, myIsEnabled(*this, false, "IsEnabled", true)
, myColor(*this, false, "Color", 1.0f, 1.0f, 1.0f)
, myOpacity(*this, false, "Opacity", 1.0f)
, myAlphaMask(nullptr)
, mySwallowInputs(*this, false, "SwallowInputs", false)
, myFocus(false)
{
}


void UIItem::NotifyUpdate(const unsigned int labelid)
{
	if (labelid == myIsHidden.getID())
	{
		PropagateNodeFlags();
	}
	else if(labelid == mySwallowInputs.getID())
	{
		if (mySwallowInputs)
		{
			KigsCore::GetModule<ModuleInput>()->getTouchManager()->registerEvent(this, "ManageInputSwallowEvent", InputSwallow, EmptyFlag);
		}
		else
		{
			KigsCore::GetModule<ModuleInput>()->getTouchManager()->unregisterEvent(this, InputSwallow);
		}
	}
	ParentClassType::NotifyUpdate(labelid);
}

BaseUI2DLayer*	UIItem::getParentLayer()
{
	for (auto p : GetParents())
	{
		if (p->isSubType(UIItem::myClassID))
		{
			BaseUI2DLayer* search = ((UIItem*)p)->getParentLayer();
			if (search)
			{
				return search;
			}
		}
		else if (p->isSubType(BaseUI2DLayer::myClassID))
		{
			return (BaseUI2DLayer*)p;
		}

	}
	return nullptr;
}

bool UIItem::Draw(TravState* state)
{
	if (myIsHidden)
		return false;

	return ParentClassType::Draw(state);
}

Point2D UIItem::GetCoordsInContainer(kfloat X, kfloat Y)
{
	Point2D result;
	// recompute matrix if needed
	SetUpNodeIfNeeded();

	Point2D pt[4];
	GetTransformedPoints(pt);

	Point2D L_PA(X - pt[0].x, Y - pt[0].y);
	Point2D L_PQ = pt[3] - pt[0];
	Point2D L_PR = pt[1] - pt[0];

	float d = (L_PQ.x*L_PR.y - L_PR.x*L_PQ.y);
	result.y = -(L_PA.x*L_PQ.y - L_PQ.x*L_PA.y) / d;
	result.x = (L_PA.x*L_PR.y - L_PR.x*L_PA.y) / d;

	return result;
}

bool UIItem::ContainsPoint(kfloat X, kfloat Y)
{
	if (myIsHidden || !mybIsTouchable)
		return false;

	// recompute matrix if needed
	SetUpNodeIfNeeded();

	Point2D pt[4];
	GetTransformedPoints(pt);

	Point2D L_PA(X - pt[0].x, Y - pt[0].y);
	Point2D L_PQ = pt[3] - pt[0];
	Point2D L_PR = pt[1] - pt[0];

	float d = (L_PQ.x*L_PR.y - L_PR.x*L_PQ.y);
	float n = -(L_PA.x*L_PQ.y - L_PQ.x*L_PA.y) / d;
	float m = (L_PA.x*L_PR.y - L_PR.x*L_PA.y) / d;
	if ((n >= 0 && n <= 1 && m >= 0 && m <= 1) == false)//outside
		return false;

	if (isAlpha(m, n))
	{
		return false;
	}
	return true;
}

bool UIItem::CanInteract(v2f pos, bool must_contain)
{
	return myIsEnabled && mybIsTouchable && !myIsHidden && !IsHiddenFlag() && (!must_contain || ContainsPoint(pos.x, pos.y)) && IsInClip(pos) ;
}

bool UIItem::TriggerMouseMove(bool over, float MouseDeltaX, float MouseDeltaY) { EmitSignal("MouseMove", this, over, MouseDeltaX, MouseDeltaY); return false; }
void UIItem::TriggerMouseSwipe(int idxButton, float Vx, float Vy) { EmitSignal("MouseSwipe", this, idxButton, Vx, Vy); }
void UIItem::TriggerPinch(float Dx, float Dy, float DZ) { EmitSignal("MousePinch", this, Dx, Dy, DZ); }

bool UIItem::TriggerMouseClick(int buttonState, int buttonEvent, int X, int Y, bool& catchClick)
{
	EmitSignal("MouseClick", this, buttonState, buttonEvent, X, Y, &catchClick);
	return false;
}

void  UIItem::MustBeDeleted()
{
	KigsCore::GetNotificationCenter()->postNotificationName("DeleteChild", this);
}

bool UIItem::ManageInputSwallowEvent(InputEvent& ev)
{
	bool b = CanInteract(ev.position.xy);
	if (b) *ev.swallow_mask = 0xFFFFFFFF;
	return b;
}