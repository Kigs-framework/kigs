#include "PrecompiledHeaders.h"

#include "Node2D.h"
#include "TravState.h"
#include "SceneGraphDefines.h"
#include "ModuleSceneGraph.h"
#include "ModuleRenderer.h"
#include "Core.h"
#include "Drawable.h"
#include "CullingObject.h"
#include "CoordinateSystem.h"
#include "Camera.h"
#include "Core.h"
#include "NotificationCenter.h"
#include "Base2DLayer.h"
#include "UI/UILayout.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(Node2D, Node2D, 2DLayers);
IMPLEMENT_CLASS_INFO(Node2D)

IMPLEMENT_CONSTRUCTOR(Node2D)
, myParent(nullptr)
, myPriority(*this, false, "Priority", 0)
, mySizeX(*this, false, "SizeX", 0)
, mySizeY(*this, false, "SizeY", 0)
, myDock(*this, false, "Dock", 0, 0)
, myAnchor(*this, false, "Anchor", 0, 0)
, myPosition(*this, false, "Position", 0, 0)
, myAngle(*this, false, "RotationAngle", 0)
, myPreScaleX(*this, false, "PreScaleX", 1)
, myPreScaleY(*this, false, "PreScaleY", 1)
, myPostScaleX(*this, false, "PostScaleX", 1)
, myPostScaleY(*this, false, "PostScaleY", 1)
, mySizeModeX(*this, false, "SizeModeX", "Default", "Multiply", "Add")
, mySizeModeY(*this, false, "SizeModeY", "Default", "Multiply", "Add")
, myClipSons(*this, false, "ClipSons", false)
, myNeedUpdatePosition(true)
, mySonPriorityChanged(false)
{
	myLocalTransformMatrix.SetIdentity();
	myGlobalTransformMatrix.SetIdentity();
}

bool Node2D::IsInClip(v2f pos) const
{
	bool allow = true;
	if (myFlags & Node2D_Clipped)
	{
		auto father = getFather();
		while (father && allow)
		{
			bool clip = false;
			father->getValue("ClipSons", clip);
			if (clip && father->isSubType("UIItem"))
			{
				allow = static_cast<UIItem*>(father)->ContainsPoint(pos.x, pos.y);
			}
			father = father->getFather();
		}
	}
	return allow;
}



void Node2D::NotifyUpdate(const unsigned int labelid)
{
	if (labelid == myClipSons.getID())
	{
		PropagateNodeFlags();
	}
	
	bool sizechanged = (labelid == mySizeX.getLabelID()) ||
		(labelid == mySizeY.getLabelID()) || 
		(labelid == mySizeModeX.getLabelID()) ||
		(labelid == mySizeModeY.getLabelID());
	
	if (sizechanged || (labelid == myAnchor.getLabelID()) ||
		(labelid == myPosition.getLabelID()) ||
		(labelid == myDock.getLabelID()) ||
		(labelid == myPreScaleX.getLabelID()) ||
		(labelid == myPreScaleY.getLabelID()) ||
		(labelid == myPostScaleX.getLabelID()) ||
		(labelid == myPostScaleY.getLabelID()) ||
		(labelid == myAngle.getLabelID()))
	{
		myNeedUpdatePosition = true;

		//@TODO investigate if we really want this here
		if (mySizeModeX == 0 && labelid == mySizeX.getLabelID() && mySizeX >= 0)
			myRealSize.x = mySizeX;

		if (mySizeModeY == 0 && labelid == mySizeY.getLabelID() && mySizeY >= 0)
			myRealSize.y = mySizeY;

		if (sizechanged)
			myFlags |= Node2D_SizeChanged;

		//PropagateNeedUpdateToFather();
	}
	else if (labelid == myPriority.getLabelID())
	{
		if (myParent)
		{
			myParent->mySonPriorityChanged = true;
			if (myParent->isSubType(UILayout::myClassID))
				static_cast<UILayout*>(myParent)->NeedRecomputeLayout();
		}
	}
	CoreModifiable::NotifyUpdate(labelid);
}

void Node2D::SetParent(CoreModifiable* value)
{
	u32 old_flag = myFlags;
	myFlags &= ~(Node2D_PropagatedFlags);
	myParent = (Node2D*)value;
	if (myParent)
	{
		myFlags |= (myParent->myFlags & Node2D_PropagatedFlags);
		if (myParent->myClipSons) 
			myFlags |= Node2D_Clipped;

		bool hidden = false;
		if(myParent->getValue("IsHidden", hidden) && hidden) 
			myFlags |= Node2D_Hidden;
	}
	if (old_flag != myFlags)
		PropagateNodeFlags();
}

void Node2D::PropagateNodeFlags()
{
	for (auto son : mySons)
	{
		u32 old = son->myFlags; 

		son->myFlags &= ~(Node2D_PropagatedFlags);
		son->myFlags |= (myFlags & Node2D_PropagatedFlags);

		if (myClipSons)
			son->myFlags |= Node2D_Clipped;

		bool hidden = false;
		if (getValue("IsHidden", hidden) && hidden)
			son->myFlags |= Node2D_Hidden;

		if (old != son->myFlags)
			son->PropagateNodeFlags();
	}
}

bool Node2D::addItem(const CMSP& item, ItemPosition pos DECLARE_LINK_NAME)
{
	if (item->isSubType(Node2D::myClassID))
	{
		//if (item->IsInit())
		{
			((Node2D*)item.get())->SetParent(this);
			//((Node2D*)item)->PropagateNeedUpdateToFather();
		}
		ResortSons();
		mySons.insert((Node2D*)item.get());
	}
	return CoreModifiable::addItem(item,pos PASS_LINK_NAME(linkName));
}

bool Node2D::removeItem(const CMSP& item DECLARE_LINK_NAME)
{
	if (item->isSubType(Node2D::myClassID))
	{
		ResortSons();
		auto it = mySons.find(static_cast<Node2D*>(item.get()));
		
		if (it != mySons.end())
		{
			if ((*it)->myParent == this)
			{
				(*it)->SetParent(nullptr);
			}
			mySons.erase(it);
		}
	}
	return CoreModifiable::removeItem(item PASS_LINK_NAME(linkName));
}


bool Node2D::Draw(TravState* state)
{
	// TODO : can fast check bbox here ?
	return true;
}

void Node2D::ComputeRealSize()
{
	Point2D size(mySizeX, mySizeY);

	// size need to be computed
	if (mySizeModeX != DEFAULT ||
		mySizeModeY != DEFAULT ||
		size.x < 0 ||
		size.y < 0)
	{
		Node2D* father = getFather();
		Point2D fsize;
		if (father)
		{
			fsize = father->myRealSize;
		}
		else
		{
			float fX, fY;
			if (getLayerFather())
			{
				getLayerFather()->GetRenderingScreen()->GetDesignSize(fX, fY);

				fsize.x = (size.x < 0) ? fX : size.x;
				fsize.y = (size.y < 0) ? fY : size.y;
			}
		}


		switch ((int)mySizeModeX)
		{
		case DEFAULT:
			if (size.x < 0)
			{
				size.x = fsize.x;
				if (size.x >= 0)
					mySizeX = size.x;
			}
			break;
		case MULTIPLY:
			size.x = size.x*fsize.x;
			break;
		case ADD:
			size.x = size.x + fsize.x;
			break;
		}


		switch ((int)mySizeModeY)
		{
		case DEFAULT:
			if (size.y < 0)
			{
				size.y = fsize.y;
				if (size.y >= 0)
					mySizeY = size.y;
			}
			break;
		case MULTIPLY:
			size.y = size.y*fsize.y;
			break;
		case ADD:
			size.y = size.y + fsize.y;
			break;
		}
	}

	myRealSize = size;
}

void Node2D::ComputeMatrices()
{
	myLocalTransformMatrix.SetIdentity();
	myLocalTransformMatrix.SetScale((kfloat)myPreScaleX, (kfloat)myPreScaleY, 1.0);
	myLocalTransformMatrix.PreRotateZ((kfloat)myAngle);
	myLocalTransformMatrix.PostScale((kfloat)myPostScaleX,(kfloat)myPostScaleY, 1.0);
	
	ComputeRealSize();

	Point2D	Translate(myAnchor[0] * myRealSize.x, myAnchor[1] * myRealSize.y);

	myLocalTransformMatrix.TransformPoints(&Translate, 1);

	if (myParent) // Dock on parent UI
	{
		myLocalTransformMatrix.e[2][0] = myDock[0] * myParent->myRealSize.x;
		myLocalTransformMatrix.e[2][1] = myDock[1] * myParent->myRealSize.y;
	}
	else	// dock on layer if no parent UI
	{
		Abstract2DLayer* layerfather = getLayerFather();
		if (layerfather)
		{
			int sx, sy;
			layerfather->getSize(sx, sy);
			myLocalTransformMatrix.e[2][0] = myDock[0] * (float)sx;
			myLocalTransformMatrix.e[2][1] = myDock[1] * (float)sy;
		}
	}

	myLocalTransformMatrix.e[2][0] -= Translate.x;
	myLocalTransformMatrix.e[2][1] -= Translate.y;

	// add position
	myLocalTransformMatrix.e[2][0] += myPosition[0];
	myLocalTransformMatrix.e[2][1] += myPosition[1];

	myGlobalTransformMatrix = myLocalTransformMatrix;

	if (myParent)
	{
		myGlobalTransformMatrix.PostMultiply(myParent->myGlobalTransformMatrix);
		if (myParent && (myFlags & Node2D_SizeChanged) == Node2D_SizeChanged && myParent->isSubType(UILayout::myClassID))
			static_cast<UILayout*>(myParent)->NeedRecomputeLayout();
	}
}

void	Node2D::InitModifiable()
{
	// check if son rendering matrix is init and this is not init
	if (!IsInit())
	{
		myAnchor.changeNotificationLevel(Owner);
		myDock.changeNotificationLevel(Owner);
		myPosition.changeNotificationLevel(Owner);
		mySizeModeX.changeNotificationLevel(Owner);
		mySizeModeY.changeNotificationLevel(Owner);
		mySizeX.changeNotificationLevel(Owner);
		mySizeY.changeNotificationLevel(Owner);
		myPreScaleX.changeNotificationLevel(Owner);
		myPreScaleY.changeNotificationLevel(Owner);
		myPostScaleX.changeNotificationLevel(Owner);
		myPostScaleY.changeNotificationLevel(Owner);
		myPriority.changeNotificationLevel(Owner);
		myAngle.changeNotificationLevel(Owner);
		CoreModifiable::InitModifiable();
	}
}

void	Node2D::SetUpNodeIfNeeded()
{
	if (myNeedUpdatePosition)
	{
		// if not init, try to init
		if (!_isInit)
		{
			Init();
			if (!_isInit)
			{
				return;
			}
		}
		auto father = getFather();
		if (father)
			father->SetUpNodeIfNeeded();

		ComputeMatrices();

		// propagate to sons
		// propagate recalculation
		kstl::set<Node2D*, Node2D::PriorityCompare>::iterator it = mySons.begin();
		kstl::set<Node2D*, Node2D::PriorityCompare>::iterator end = mySons.end();
		for (; it != end; ++it)
		{
			(*it)->myNeedUpdatePosition = true;
			if ((myFlags & Node2D_SizeChanged) != 0)
				(*it)->myFlags |= Node2D_SizeChanged;
		}
		myFlags &= ~ Node2D_SizeChanged;
		myNeedUpdatePosition = false;
	}
}

void	Node2D::GetGlobalPosition(kfloat &X, kfloat &Y)
{
	// recompute matrix
	SetUpNodeIfNeeded();

	X = myGlobalTransformMatrix.e[2][0];
	Y = myGlobalTransformMatrix.e[2][1];

}

void Node2D::TravDraw(TravState* state)
{
	//! if this node was flagged as not visible, do nothing (return)
	if (!Draw(state))
	{
		return;
	}

	// recompute matrix
	SetUpNodeIfNeeded();

	ResortSons();

	PreTravDraw(state);
	ProtectedDraw(state);

	// draw sons
	kstl::set<Node2D*, Node2D::PriorityCompare>::iterator it = mySons.begin();
	kstl::set<Node2D*, Node2D::PriorityCompare>::iterator end = mySons.end();
	while (it != end)
	{
		(*it)->TravDraw(state);
		++it;
	}

	PostTravDraw(state);
}

void	Node2D::GetTransformedPoints(Point2D * pt)
{
	pt[0].Set(0.0f, 0.0f);
	pt[1].Set(0.0f, myRealSize.y);
	pt[2].Set(myRealSize.x, myRealSize.y);
	pt[3].Set(myRealSize.x,	0.0f);

	TransformPoints(pt, 4);
}


void	Node2D::ResortSons()
{
	if (mySonPriorityChanged)
	{
		kstl::set<Node2D*, Node2D::PriorityCompare> resortset = mySons;

		mySons.clear();

		kstl::set<Node2D*, Node2D::PriorityCompare>::iterator it = resortset.begin();
		kstl::set<Node2D*, Node2D::PriorityCompare>::iterator end = resortset.end();

		for (; it != end; ++it)
		{
			mySons.insert(*it);
		}
		mySonPriorityChanged = false;
	}
}

void	Node2D::PropagateNeedUpdateToFather()
{
	myNeedUpdatePosition = true;
	if (myParent)
	{
		if (!myParent->myNeedUpdatePosition)
		{
			myParent->PropagateNeedUpdateToFather();
		}
	}
}

Node2D* Node2D::getFather() const
{
	//! Node2D has only one Node2D father, so search in CoreModifiable parents, and found it 
	kstl::vector<CoreModifiable*>::const_iterator it;
	const kstl::vector<CoreModifiable*>& parents = GetParents();
	Node2D* father = 0;
	for (it = parents.begin(); it != parents.end(); ++it)
	{
		if ((*it)->isSubType(Node2D::myClassID))
		{
			father = (Node2D*)(*it);
			break;
		}
	}
	return father;
}

Abstract2DLayer* Node2D::getLayerFather() const
{
	kstl::vector<CoreModifiable*>::const_iterator it;
	const kstl::vector<CoreModifiable*>& parents = GetParents();
	Abstract2DLayer* father = 0;
	for (it = parents.begin(); it != parents.end(); ++it)
	{
		if ((*it)->isSubType(Abstract2DLayer::myClassID))
		{
			father = (Abstract2DLayer*)(*it);
			break;
		}
	}
	return father;
}

Abstract2DLayer*	Node2D::getRootLayerFather() const
{
	Node2D* father = getFather();
	if (father)
	{
		return father->getRootLayerFather();
	}
	
	return getLayerFather();
}

Node2D*	Node2D::getRootFather()
{
	Node2D* father = getFather();
	if (father)
	{
		return father->getRootFather();
	}

	return this;
}

int Node2D::GetFinalPriority() const 
{ 
	int prio = myPriority;
	if (prio != 0) return prio;

	auto father = getFather();
	if (father) return father->GetFinalPriority();
	return 0;
}