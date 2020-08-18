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
, mParent(nullptr)
, mPriority(*this, false, "Priority", 0)
, mSizeX(*this, false, "SizeX", 0)
, mSizeY(*this, false, "SizeY", 0)
, mDock(*this, false, "Dock", 0, 0)
, mAnchor(*this, false, "Anchor", 0, 0)
, mPosition(*this, false, "Position", 0, 0)
, mRotationAngle(*this, false, "RotationAngle", 0)
, mPreScaleX(*this, false, "PreScaleX", 1)
, mPreScaleY(*this, false, "PreScaleY", 1)
, mPostScaleX(*this, false, "PostScaleX", 1)
, mPostScaleY(*this, false, "PostScaleY", 1)
, mSizeModeX(*this, false, "SizeModeX", "Default", "Multiply", "Add")
, mSizeModeY(*this, false, "SizeModeY", "Default", "Multiply", "Add")
, mClipSons(*this, false, "ClipSons", false)
, mNeedUpdatePosition(true)
, mSonPriorityChanged(false)
{
	mLocalTransformMatrix.SetIdentity();
	mGlobalTransformMatrix.SetIdentity();
}

bool Node2D::IsInClip(v2f pos) const
{
	bool allow = true;
	if (mFlags & Node2D_Clipped)
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
	if (labelid == mClipSons.getID())
	{
		PropagateNodeFlags();
	}
	
	bool sizechanged = (labelid == mSizeX.getLabelID()) ||
		(labelid == mSizeY.getLabelID()) || 
		(labelid == mSizeModeX.getLabelID()) ||
		(labelid == mSizeModeY.getLabelID());
	
	if (sizechanged || (labelid == mAnchor.getLabelID()) ||
		(labelid == mPosition.getLabelID()) ||
		(labelid == mDock.getLabelID()) ||
		(labelid == mPreScaleX.getLabelID()) ||
		(labelid == mPreScaleY.getLabelID()) ||
		(labelid == mPostScaleX.getLabelID()) ||
		(labelid == mPostScaleY.getLabelID()) ||
		(labelid == mRotationAngle.getLabelID()))
	{
		mNeedUpdatePosition = true;

		//@TODO investigate if we really want this here
		if (mSizeModeX == 0 && labelid == mSizeX.getLabelID() && mSizeX >= 0)
			mRealSize.x = mSizeX;

		if (mSizeModeY == 0 && labelid == mSizeY.getLabelID() && mSizeY >= 0)
			mRealSize.y = mSizeY;

		if (sizechanged)
			mFlags |= Node2D_SizeChanged;

		//PropagateNeedUpdateToFather();
	}
	else if (labelid == mPriority.getLabelID())
	{
		if (mParent)
		{
			mParent->mSonPriorityChanged = true;
			if (mParent->isSubType(UILayout::mClassID))
				static_cast<UILayout*>(mParent)->NeedRecomputeLayout();
		}
	}
	CoreModifiable::NotifyUpdate(labelid);
}

void Node2D::SetParent(CoreModifiable* value)
{
	u32 old_flag = mFlags;
	mFlags &= ~(Node2D_PropagatedFlags);
	mParent = (Node2D*)value;
	if (mParent)
	{
		mFlags |= (mParent->mFlags & Node2D_PropagatedFlags);
		if (mParent->mClipSons) 
			mFlags |= Node2D_Clipped;

		bool hidden = false;
		if(mParent->getValue("IsHidden", hidden) && hidden) 
			mFlags |= Node2D_Hidden;
	}
	if (old_flag != mFlags)
		PropagateNodeFlags();
}

void Node2D::PropagateNodeFlags()
{
	for (auto son : mSons)
	{
		u32 old = son->mFlags; 

		son->mFlags &= ~(Node2D_PropagatedFlags);
		son->mFlags |= (mFlags & Node2D_PropagatedFlags);

		if (mClipSons)
			son->mFlags |= Node2D_Clipped;

		bool hidden = false;
		if (getValue("IsHidden", hidden) && hidden)
			son->mFlags |= Node2D_Hidden;

		if (old != son->mFlags)
			son->PropagateNodeFlags();
	}
}

bool Node2D::addItem(const CMSP& item, ItemPosition pos DECLARE_LINK_NAME)
{
	if (item->isSubType(Node2D::mClassID))
	{
		//if (item->IsInit())
		{
			((Node2D*)item.get())->SetParent(this);
			//((Node2D*)item)->PropagateNeedUpdateToFather();
		}
		ResortSons();
		mSons.insert((Node2D*)item.get());
	}
	return CoreModifiable::addItem(item,pos PASS_LINK_NAME(linkName));
}

bool Node2D::removeItem(const CMSP& item DECLARE_LINK_NAME)
{
	if (item->isSubType(Node2D::mClassID))
	{
		ResortSons();
		auto it = mSons.find(static_cast<Node2D*>(item.get()));
		
		if (it != mSons.end())
		{
			if ((*it)->mParent == this)
			{
				(*it)->SetParent(nullptr);
			}
			mSons.erase(it);
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
	Point2D size(mSizeX, mSizeY);

	// size need to be computed
	if (mSizeModeX != DEFAULT ||
		mSizeModeY != DEFAULT ||
		size.x < 0 ||
		size.y < 0)
	{
		Node2D* father = getFather();
		Point2D fsize;
		if (father)
		{
			fsize = father->mRealSize;
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


		switch ((int)mSizeModeX)
		{
		case DEFAULT:
			if (size.x < 0)
			{
				size.x = fsize.x;
				if (size.x >= 0)
					mSizeX = size.x;
			}
			break;
		case MULTIPLY:
			size.x = size.x*fsize.x;
			break;
		case ADD:
			size.x = size.x + fsize.x;
			break;
		}


		switch ((int)mSizeModeY)
		{
		case DEFAULT:
			if (size.y < 0)
			{
				size.y = fsize.y;
				if (size.y >= 0)
					mSizeY = size.y;
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

	mRealSize = size;
}

void Node2D::ComputeMatrices()
{
	mLocalTransformMatrix.SetIdentity();
	mLocalTransformMatrix.SetScale((kfloat)mPreScaleX, (kfloat)mPreScaleY, 1.0);
	mLocalTransformMatrix.PreRotateZ((kfloat)mRotationAngle);
	mLocalTransformMatrix.PostScale((kfloat)mPostScaleX,(kfloat)mPostScaleY, 1.0);
	
	ComputeRealSize();

	Point2D	Translate(mAnchor[0] * mRealSize.x, mAnchor[1] * mRealSize.y);

	mLocalTransformMatrix.TransformPoints(&Translate, 1);

	if (mParent) // Dock on parent UI
	{
		mLocalTransformMatrix.e[2][0] = mDock[0] * mParent->mRealSize.x;
		mLocalTransformMatrix.e[2][1] = mDock[1] * mParent->mRealSize.y;
	}
	else	// dock on layer if no parent UI
	{
		Abstract2DLayer* layerfather = getLayerFather();
		if (layerfather)
		{
			int sx, sy;
			layerfather->getSize(sx, sy);
			mLocalTransformMatrix.e[2][0] = mDock[0] * (float)sx;
			mLocalTransformMatrix.e[2][1] = mDock[1] * (float)sy;
		}
	}

	mLocalTransformMatrix.e[2][0] -= Translate.x;
	mLocalTransformMatrix.e[2][1] -= Translate.y;

	// add position
	mLocalTransformMatrix.e[2][0] += mPosition[0];
	mLocalTransformMatrix.e[2][1] += mPosition[1];

	mGlobalTransformMatrix = mLocalTransformMatrix;

	if (mParent)
	{
		mGlobalTransformMatrix.PostMultiply(mParent->mGlobalTransformMatrix);
		if (mParent && (mFlags & Node2D_SizeChanged) == Node2D_SizeChanged && mParent->isSubType(UILayout::mClassID))
			static_cast<UILayout*>(mParent)->NeedRecomputeLayout();
	}
}

void	Node2D::InitModifiable()
{
	// check if son rendering matrix is init and this is not init
	if (!IsInit())
	{
		mAnchor.changeNotificationLevel(Owner);
		mDock.changeNotificationLevel(Owner);
		mPosition.changeNotificationLevel(Owner);
		mSizeModeX.changeNotificationLevel(Owner);
		mSizeModeY.changeNotificationLevel(Owner);
		mSizeX.changeNotificationLevel(Owner);
		mSizeY.changeNotificationLevel(Owner);
		mPreScaleX.changeNotificationLevel(Owner);
		mPreScaleY.changeNotificationLevel(Owner);
		mPostScaleX.changeNotificationLevel(Owner);
		mPostScaleY.changeNotificationLevel(Owner);
		mPriority.changeNotificationLevel(Owner);
		mRotationAngle.changeNotificationLevel(Owner);
		CoreModifiable::InitModifiable();
	}
}

void	Node2D::SetUpNodeIfNeeded()
{
	if (mNeedUpdatePosition)
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
		kstl::set<Node2D*, Node2D::PriorityCompare>::iterator it = mSons.begin();
		kstl::set<Node2D*, Node2D::PriorityCompare>::iterator end = mSons.end();
		for (; it != end; ++it)
		{
			(*it)->mNeedUpdatePosition = true;
			if ((mFlags & Node2D_SizeChanged) != 0)
				(*it)->mFlags |= Node2D_SizeChanged;
		}
		mFlags &= ~ Node2D_SizeChanged;
		mNeedUpdatePosition = false;
	}
}

void	Node2D::GetGlobalPosition(kfloat &X, kfloat &Y)
{
	// recompute matrix
	SetUpNodeIfNeeded();

	X = mGlobalTransformMatrix.e[2][0];
	Y = mGlobalTransformMatrix.e[2][1];

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
	kstl::set<Node2D*, Node2D::PriorityCompare>::iterator it = mSons.begin();
	kstl::set<Node2D*, Node2D::PriorityCompare>::iterator end = mSons.end();
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
	pt[1].Set(0.0f, mRealSize.y);
	pt[2].Set(mRealSize.x, mRealSize.y);
	pt[3].Set(mRealSize.x,	0.0f);

	TransformPoints(pt, 4);
}


void	Node2D::ResortSons()
{
	if (mSonPriorityChanged)
	{
		kstl::set<Node2D*, Node2D::PriorityCompare> resortset = mSons;

		mSons.clear();

		kstl::set<Node2D*, Node2D::PriorityCompare>::iterator it = resortset.begin();
		kstl::set<Node2D*, Node2D::PriorityCompare>::iterator end = resortset.end();

		for (; it != end; ++it)
		{
			mSons.insert(*it);
		}
		mSonPriorityChanged = false;
	}
}

void	Node2D::PropagateNeedUpdateToFather()
{
	mNeedUpdatePosition = true;
	if (mParent)
	{
		if (!mParent->mNeedUpdatePosition)
		{
			mParent->PropagateNeedUpdateToFather();
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
		if ((*it)->isSubType(Node2D::mClassID))
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
		if ((*it)->isSubType(Abstract2DLayer::mClassID))
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
	int prio = mPriority;
	if (prio != 0) return prio;

	auto father = getFather();
	if (father) return father->GetFinalPriority();
	return 0;
}