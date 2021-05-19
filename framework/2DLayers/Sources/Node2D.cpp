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
, mSize(*this, false, "Size", 0,0)
, mDock(*this, false, "Dock", 0, 0)
, mAnchor(*this, false, "Anchor", 0, 0)
, mPosition(*this, false, "Position", 0, 0)
, mRotationAngle(*this, false, "RotationAngle", 0)
, mPreScale(*this, false, "PreScale", 1,1)
, mPostScale(*this, false, "PostScale", 1,1)
, mClipSons(*this, false, "ClipSons", false)
{
	SetNodeFlag(Node2D_NeedUpdatePosition);
	SetNodeFlag(Node2D_SizeChanged);
	mLocalTransformMatrix.SetIdentity();
	mGlobalTransformMatrix.SetIdentity();
}

bool Node2D::IsInClip(v2f pos) const
{
	bool allow = true;
	if (isUserFlagSet(Node2D_Clipped))
	{
		auto father = getFather();
		while (father && allow)
		{
			if (father->GetNodeFlag(Node2D::Node2D_ClipSons) && father->isSubType("UIItem"))
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
		ChangeNodeFlag(Node2D_ClipSons,mClipSons);
		PropagateNodeFlags();
	}
	
	bool sizechanged = (labelid == mSize.getLabelID()) ||
		(labelid == mSizeModeX.getLabelID()) ||
		(labelid == mSizeModeY.getLabelID());
	
	if (sizechanged)
		SetNodeFlag(Node2D_SizeChanged);

	if (sizechanged || (labelid == mAnchor.getLabelID()) ||
		(labelid == mPosition.getLabelID()) ||
		(labelid == mDock.getLabelID()) ||
		(labelid == mPreScale.getLabelID()) ||
		(labelid == mPostScale.getLabelID()) ||
		(labelid == mRotationAngle.getLabelID()))
	{
		SetNodeFlag(Node2D_NeedUpdatePosition);
	}
	else if (labelid == mPriority.getLabelID())
	{
		if (mParent)
		{
			mParent->SetNodeFlag(Node2D_SonPriorityChanged);
			if (mParent->isSubType(UILayout::mClassID))
				static_cast<UILayout*>(mParent)->NeedRecomputeLayout();
		}
	}
	else if (labelid == mCustomShader.getLabelID())
	{
		ChangeNodeFlag(Node2D_UseCustomShader, ((std::string)mCustomShader != ""));
	}
	CoreModifiable::NotifyUpdate(labelid);
}

void Node2D::SetParent(CoreModifiable* value)
{
	u32 old_flag = getUserFlags(0xFFFFFFFF);
	unsetUserFlag(Node2D_PropagatedFlags);

	mParent = (Node2D*)value;
	if (mParent)
	{
		setUserFlag(mParent->getUserFlags(Node2D_PropagatedFlags));
		if (mParent->mClipSons) 
			setUserFlag (Node2D_Clipped);

		bool hidden = false;
		if(mParent->getValue("IsHidden", hidden) && hidden) 
			setUserFlag(Node2D_Hidden);
	}
	if (old_flag != getUserFlags(0xFFFFFFFF))
		PropagateNodeFlags();
}

void Node2D::PropagateNodeFlags()
{
	for (auto son : mSons)
	{
		u32 old = son->getUserFlags(0xFFFFFFFF);
		son->unsetUserFlag(Node2D_PropagatedFlags);
		son->setUserFlag(getUserFlags(Node2D_PropagatedFlags));
		
		if (mClipSons)
			son->setUserFlag(Node2D_Clipped);

		bool hidden = false;
		if (getValue("IsHidden", hidden) && hidden)
			son->setUserFlag(Node2D_Hidden);

		if (old != son->getUserFlags(0xFFFFFFFF))
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
	if (!GetNodeFlag(Node2D_SizeChanged))
	{
		return;
	}
	
	Point2D size(mSize);

	SizeMode	s_mode[2];
	s_mode[0] = (SizeMode)(int)mSizeModeX;
	s_mode[1] = (SizeMode)(int)mSizeModeY;

	// as Node2D needs a parent to be init, fsize shoult always be ok
	Node2D* father = getFather();
	Point2D fsize(-1.0f,-1.0f);
	if (father)
	{
		fsize = father->mRealSize;
	}
	else if (getLayerFather())
	{
		float fX, fY;
	
		getLayerFather()->GetRenderingScreen()->GetDesignSize(fX, fY);

		fsize.x = (size.x < 0) ? fX : size.x;
		fsize.y = (size.y < 0) ? fY : size.y;
	}

	v2f	contentSize = GetContentSize();

	v2f	referenceSize;
	int keepRatioPass = 0;
	for (int i = 0; i < 2; i++)
	{
		// special mode or mSize set to negative value
		if ((s_mode[i] != DEFAULT) || (size[i] < 0.0f))
		{
			switch ((int)s_mode[i])
			{
			case DEFAULT:
				if (size[i] < 0.0f)
				{
					size[i] = fsize[i];
					if (fsize[i] >= 0.0f)
					{
						mSize[i] = size[i];
					}
					referenceSize[i] = size[i];
				}
				break;
			case MULTIPLY:
				size[i] = size[i] * fsize[i];
				referenceSize[i] = fsize[i];
				break;
			case ADD:
				size[i] = size[i] + fsize[i];
				referenceSize[i] = fsize[i];
				break;
			case CONTENT:
				size[i] = contentSize[i];
				referenceSize[i] = contentSize[i];
				mSize[i] = size[i];
				break;
			case CONTENT_MULTIPLY:
				size[i] = size[i] * contentSize[i];
				referenceSize[i] = contentSize[i];
				break;
			case CONTENT_ADD:
				size[i] = size[i] + contentSize[i];
				referenceSize[i] = contentSize[i];
				break;
			case KEEP_RATIO:
				keepRatioPass++;
				break;
			}
		}
	}

	if (keepRatioPass)
	{
		if (keepRatioPass == 2)
		{
			KIGS_ERROR("Node2D KeepRatio set on both size mode", 2);
		}
		else
		{
			for (int i = 0; i < 2; i++)
			{
				// special mode or mSize set to negative value
				if (s_mode[i] == KEEP_RATIO)
				{
					size[i] = referenceSize[i] * size[1 - i] / referenceSize[1 - i];

					if ((s_mode[1 - i] == CONTENT) || (s_mode[1 - i] == DEFAULT))
					{
						mSize[i] = size[i];
					}
				}
			}
		}
	}

	mRealSize = size;

	ClearNodeFlag(Node2D_SizeChanged);
}

void Node2D::ComputeMatrices()
{
	mLocalTransformMatrix.SetIdentity();
	mLocalTransformMatrix.SetScale((kfloat)mPreScale[0], (kfloat)mPreScale[1], 1.0);
	mLocalTransformMatrix.PreRotateZ((kfloat)mRotationAngle);
	mLocalTransformMatrix.PostScale((kfloat)mPostScale[0],(kfloat)mPostScale[1], 1.0);
	
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
		if (mParent && GetNodeFlag(Node2D_SizeChanged) && mParent->isSubType(UILayout::mClassID))
			static_cast<UILayout*>(mParent)->NeedRecomputeLayout();
	}
}

void	Node2D::InitModifiable()
{
	// check if son rendering matrix is init and this is not init
	if (!IsInit() && !GetParents().empty()/* && ( getFather() || (getLayerFather()))*/)
	{
		mAnchor.changeNotificationLevel(Owner);
		mDock.changeNotificationLevel(Owner);
		mPosition.changeNotificationLevel(Owner);
		mSizeModeX.changeNotificationLevel(Owner);
		mSizeModeY.changeNotificationLevel(Owner);
		mSize.changeNotificationLevel(Owner);
		mPreScale.changeNotificationLevel(Owner);
		mPostScale.changeNotificationLevel(Owner);
		mPriority.changeNotificationLevel(Owner);
		mRotationAngle.changeNotificationLevel(Owner);
		mClipSons.changeNotificationLevel(Owner);

		ChangeNodeFlag(Node2D_ClipSons, mClipSons);

		// call me if custom shader is set
		mCustomShader.changeNotificationLevel(Owner);
		
		ChangeNodeFlag(Node2D_UseCustomShader, ((std::string)mCustomShader != ""));
		if (mParent)
			mParent->SetNodeFlag(Node2D_SonPriorityChanged);
		CoreModifiable::InitModifiable();
	}
}

void	Node2D::SetUpNodeIfNeeded()
{
	if (GetNodeFlag(Node2D_NeedUpdatePosition) || GetNodeFlag(Node2D_SizeChanged))
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


		// propagate to sons
		// propagate recalculation
		kstl::set<Node2D*, Node2D::PriorityCompare>::iterator it = mSons.begin();
		kstl::set<Node2D*, Node2D::PriorityCompare>::iterator end = mSons.end();
		for (; it != end; ++it)
		{
			(*it)->SetNodeFlag(Node2D_NeedUpdatePosition);
			if (GetNodeFlag(Node2D_SizeChanged))
				(*it)->SetNodeFlag(Node2D_SizeChanged);
		}

		ComputeMatrices();


		ClearNodeFlag(Node2D_SizeChanged);
		ClearNodeFlag(Node2D_NeedUpdatePosition);

		SetNodeFlag(Node2D_NeedVerticeInfoUpdate);
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
	if (GetNodeFlag(Node2D_SonPriorityChanged))
	{
		kstl::set<Node2D*, Node2D::PriorityCompare> resortset = mSons;

		mSons.clear();

		kstl::set<Node2D*, Node2D::PriorityCompare>::iterator it = resortset.begin();
		kstl::set<Node2D*, Node2D::PriorityCompare>::iterator end = resortset.end();

		for (; it != end; ++it)
		{
			mSons.insert(*it);
		}
		ClearNodeFlag(Node2D_SonPriorityChanged);
	}
}

void	Node2D::PropagateNeedUpdateToFather()
{
	SetNodeFlag(Node2D_NeedUpdatePosition);
	if (mParent)
	{
		if (!mParent->GetNodeFlag(Node2D_NeedUpdatePosition))
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