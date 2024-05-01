#include "PrecompiledHeaders.h"

#include "Node3D.h"
#include "ModuleSceneGraph.h"
#include "TravState.h"
#include "TravPath.h"
#include "CullingObject.h"
#include "Camera.h"
#include "ModuleRenderer.h"
#include "DrawableSorter.h"

using namespace Kigs::Scene;

#ifdef KIGS_TOOLS
extern bool gCullingDrawBBox; 
#endif

//IMPLEMENT_AND_REGISTER_CLASS_INFO(Node3D, Node3D, SceneGraph);
IMPLEMENT_CLASS_INFO(Node3D)

IMPLEMENT_CONSTRUCTOR(Node3D)
, mFatherNode(nullptr)
{
	setUserFlag(UserFlagNode3D);
	//! init matrix to identity
	mTransform = mat4(1.0f);
	mLocalToGlobal = mat4(1.0f);
	mGlobalToLocal = mat4(1.0f);

	//! init visibility flag
	mIsVisible=0;
	mFirstVisibleNodeIndex=0;
	mVisibleNodeCount=0;
	mDrawPriority = -1;
}
void Node3D::PrepareExport(ExportSettings* settings)
{
	ParentClassType::PrepareExport(settings);
	if (!isIdentity(mTransform))
	{
		AddDynamicVectorAttribute("LocalMatrix", &mTransform[0][0], 4 * 4);
	}
}

void Node3D::EndExport(ExportSettings* settings)
{
	RemoveDynamicAttribute("LocalMatrix");
	ParentClassType::EndExport(settings);
}

bool Node3D::addItem(const CMSP& item,ItemPosition pos DECLARE_LINK_NAME)
{
	if (!item) return false;

	if (item->isSubType("SceneNode"))
	{
		if (item->isSubType("RendererMatrix"))
		{
			// Support for older xml files
			RendererMatrix* m = (RendererMatrix*)item.get();
			const float* values = m->GetMatrixValues();
			mat4 matrix = mat4(1.0f);
			matrix[0][0] = values[0];
			matrix[0][1] = values[1];
			matrix[0][2] = values[2];
			matrix[1][0] = values[4];
			matrix[1][1] = values[5];
			matrix[1][2] = values[6];
			matrix[2][0] = values[8];
			matrix[2][1] = values[9];
			matrix[2][2] = values[10];
			matrix[3][0] = values[12];
			matrix[3][1] = values[13];
			matrix[3][2] = values[14];
			ChangeMatrix(matrix);
			return false;
		}
		else if (item->isSubType("Camera"))
		{
			ModuleSceneGraph* scenegraph = static_cast<ModuleSceneGraph*>(KigsCore::Instance()->GetMainModuleInList(SceneGraphModuleCoreIndex));
			scenegraph->AddDefferedItem(item.get(), DefferedAction::ADD_CAMERA);
		}
		else if (item->isSubType("Light"))
		{
			ModuleSceneGraph* scenegraph = static_cast<ModuleSceneGraph*>(KigsCore::Instance()->GetMainModuleInList(SceneGraphModuleCoreIndex));
			scenegraph->AddDefferedItem(item.get(), DefferedAction::ADD_LIGHT);

		}

		if (item->isUserFlagSet(UserFlagNode3D))
		{
			auto node = static_cast<Node3D*>(item.get());
			node->setUserFlag(LocalToGlobalMatrixIsDirty);
			node->setUserFlag(GlobalToLocalMatrixIsDirty);
			node->PropagateDirtyFlagsToSons(node);
			PropagateNodePriorityDirtyToParents();
		}

		setUserFlag(BoundingBoxIsDirty);
		setUserFlag(GlobalBoundingBoxIsDirty);
		PropagateDirtyFlagsToParents((SceneNode*)item.get());
	}

	return SceneNode::addItem(item, pos PASS_LINK_NAME(linkName));
}

bool Node3D::removeItem(const CMSP& item DECLARE_LINK_NAME)
{
	//! if item is a SceneNode, then scenegraph will need update after node removing
	if(item->isSubType(SceneNode::mClassID))
	{
		if (item->isSubType("Camera"))
		{
			ModuleSceneGraph* scenegraph = static_cast<ModuleSceneGraph*>(KigsCore::Instance()->GetMainModuleInList(SceneGraphModuleCoreIndex));
			scenegraph->AddDefferedItem(item.get(), DefferedAction::REMOVE_CAMERA);
		}
		if (item->isSubType("API3DLight"))
		{
			ModuleSceneGraph* scenegraph = static_cast<ModuleSceneGraph*>(KigsCore::Instance()->GetMainModuleInList(SceneGraphModuleCoreIndex));
			scenegraph->AddDefferedItem(item.get(), DefferedAction::REMOVE_LIGHT);
		}		
		if (item->isUserFlagSet(UserFlagNode3D))
		{
			auto node = static_cast<Node3D*>(item.get());
			node->setUserFlag(LocalToGlobalMatrixIsDirty);
			node->setUserFlag(GlobalToLocalMatrixIsDirty);
			node->PropagateDirtyFlagsToSons(node);
			PropagateNodePriorityDirtyToParents();
		}
		setUserFlag(BoundingBoxIsDirty);
		setUserFlag(GlobalBoundingBoxIsDirty);
		PropagateDirtyFlagsToParents((SceneNode*)item.get());
	}

	return SceneNode::removeItem(item PASS_LINK_NAME(linkName));
}

bool Node3D::Draw(TravState* state)
{
	return (IsRenderable() && (mIsVisible||state->IsAllVisible()||IsAllSonsVisible()));
}

void Node3D::InitModifiable()
{
	ParentClassType::InitModifiable();
	mat4 m(1);
	if (getArrayValue("LocalMatrix", &m[0][0], 4 * 4))
	{
		ChangeMatrix(m);
		RemoveDynamicAttribute("LocalMatrix");
	}
	setUserFlag(BoundingBoxIsDirty);

	int drawPriority;
	if (getValue("DrawPriority", drawPriority))
	{
		mDrawPriority = drawPriority;
		// we probably should remove dynamic attribute here ?
	}
}

void Node3D::ChangeMatrix(const mat4& newmatrix)
{
	if (!CompareMatrix(newmatrix, mTransform))
	{
		mTransform = newmatrix;

		setUserFlag(LocalToGlobalMatrixIsDirty | GlobalToLocalMatrixIsDirty | BoundingBoxIsDirty | GlobalBoundingBoxIsDirty);

		PropagateDirtyFlagsToSons(this);
		PropagateDirtyFlagsToParents(this);
	}
}

//! move local node position ( pos = move + pos )
void	Node3D::localMove(const v3f& move)
{
	v3f pos = column(mTransform,3);
	pos += move;
	mTransform = column(mTransform,3,v4f(pos,1.0f));
	setUserFlag(LocalToGlobalMatrixIsDirty | GlobalToLocalMatrixIsDirty | BoundingBoxIsDirty | GlobalBoundingBoxIsDirty);

	PropagateDirtyFlagsToSons(this);
	PropagateDirtyFlagsToParents(this);
}

//! move global node position ( gpos = gmove + gpos )
void	Node3D::globalMove(const v3f& move)
{
	v3f lmove;
	lmove = transformVector3(mGlobalToLocal,move);
	localMove(lmove);
}


void Node3D::PropagateDirtyFlagsToSons(SceneNode* source)
{
	for (auto& item : getItems())
	{
		if (item.mItem->isUserFlagSet(UserFlagNode3D))
		{
			auto node = item.mItem->as<Node3D>();
			node->setUserFlag(LocalToGlobalMatrixIsDirty| GlobalBoundingBoxIsDirty | GlobalToLocalMatrixIsDirty);
			node->PropagateDirtyFlagsToSons(source);
		}
	}
}

void Node3D::PropagateDirtyFlagsToParents(SceneNode* source)
{
	auto node = getFather();
	if (node)
	{
		node->setUserFlag(BoundingBoxIsDirty | GlobalBoundingBoxIsDirty);
		node->PropagateDirtyFlagsToParents(source);
	}
}

void Node3D::PropagateDirtyFlags(SceneNode* source)
{
	PropagateDirtyFlagsToSons(source);
	PropagateDirtyFlagsToParents(source);
}

bool Node3D::Cull(TravState* state, unsigned int cullingMask)
{
	auto tst = GetUpgradorMatchingFlag(1);
	if (tst)
	{
		return static_cast<CullUpgrador*>(tst)->Cull(this, state, cullingMask);
	}
	//! no culling if not shown
	if(!IsRenderable())
	{
		mIsVisible=0;
		return false;
	}

	mCullingMask=cullingMask;
	std::vector<ModifiableItemStruct>::const_iterator ittemp;

	//! compute visibility only if we are not in a all visible branch
	if(!state->IsAllVisible())
	{
		//! check if node visibility was not already computed during this frame
		if(mIsVisible!=state->GetVisibilityFrame())
		{
			UnsetAllSonsVisible();
			//! cull object against current culling object
			CullingObject::CULLING_RESULT result= state->GetCullingObject()->Cull(this,mCullingMask);

			//! if the object if totally out, just set mIsVisible to 0 (not visible) and return false
			if(result==CullingObject::all_out)
			{
				mIsVisible=0;
#ifdef KIGS_TOOLS
				if (!gCullingDrawBBox)
#endif
					return false;

			}

			//! if the object if totally in, set mIsVisible to current frame (visible), set all visible flag to true and return true
			if(result==CullingObject::all_in)
			{
				mIsVisible=state->GetVisibilityFrame();
#ifdef KIGS_TOOLS
				if (!gCullingDrawBBox)
				{
#endif
					SceneNode::SetAllSonsVisible();
					return true;
#ifdef KIGS_TOOLS
				}
#endif
			}
		}
	}

	//! else node is partially visible or visibility check was already true so return true 
	mIsVisible=state->GetVisibilityFrame();
	return true;
}

void Node3D::PreDrawDrawable(TravState* state)
{	
	mat4 m{ GetLocalToGlobal() };
	state->GetRenderer()->PushAndLoadMatrix(MATRIX_MODE_MODEL, m);

	//! when in "path" mode or when no drawablesorter is available, then no question, predraw all drawable
	if(state->mPath || !state->mCurrentPass || !state->mCurrentPass->sorter)
	{
		for (auto& it : getItems())
		{
			if (it.mItem->isUserFlagSet(UserFlagDrawable))
			{
				auto d = it.mItem->as<Drawable>();
				if (!state->mCurrentPass || d->IsUsedInRenderPass(state->mCurrentPass->pass_mask))
					d->CheckPreDraw(state);
			}
		}
	}
	//! else predraw only not transparent drawable
	else
	{
		for (auto& it : getItems())
		{
			if (it.mItem->isUserFlagSet(UserFlagDrawable))
			{
				auto d = it.mItem->as<Drawable>();
				if(d->IsUsedInRenderPass(state->mCurrentPass->pass_mask))
				{
					d->CheckPreDraw(state);
				}
			}
		}
	}
}

void Node3D::DrawDrawable(TravState* state)
{
	//! when in "path" mode, just get drawable in path, call DoDraw and update needed pass count
	if(state->mPath)
	{
		Drawable* drawable=state->mPath->GetDrawable();
		drawable->CheckDraw(state);
	}
	//! else when not in "path" mode and no drawable sorter is available, call DoDraw and update needed pass count on all drawable
	else if (!state->mCurrentPass || !state->mCurrentPass->sorter)
	{
		for (auto& it : getItems())
		{
			if (it.mItem->isUserFlagSet(UserFlagDrawable))
			{
				auto d = it.mItem->as<Drawable>();
				if (d->IsUsedInRenderPass(state->mCurrentPass->pass_mask))
					d->CheckDraw(state);
			}
		}
	}
	//! last case when not in "path" mode and drawable sorter is available, call DoDraw and update needed pass count on 
	//! not transparent drawable, or add transparent drawable to the path
	else
	{
		for(auto& it : getItems())
		{
			if(it.mItem->isUserFlagSet(UserFlagDrawable))
			{
				auto d = it.mItem->as<Drawable>();
				if (d->IsUsedInRenderPass(state->mCurrentPass->pass_mask))
				{
					if (d->IsRenderable() && d->IsSortable())
					{
						state->mCurrentPass->sorter->AddDrawable(d, state);
					}
				}
				/*else
				{
					d->CheckDraw(state);
				}*/
			}
		}
	}
}

void Node3D::PostDrawDrawable(TravState* state)
{
	//! when in "path" mode or when no drawablesorter is available, then no question, postdraw all drawable
	if(state->mPath || !state->mCurrentPass || !state->mCurrentPass->sorter)
	{
		for (auto& it : getItems())
		{
			if (it.mItem->isUserFlagSet(UserFlagDrawable))
			{
				auto d = it.mItem->as<Drawable>();
				if (!state->mCurrentPass || d->IsUsedInRenderPass(state->mCurrentPass->pass_mask))
					d->CheckPostDraw(state);
			}
		}
	}
	//! else postdraw only not transparent drawable 
	else
	{
		for (auto& it : getItems())
		{
			if (it.mItem->isUserFlagSet(UserFlagDrawable))
			{
				auto d = it.mItem->as<Drawable>();
				if (d->IsUsedInRenderPass(state->mCurrentPass->pass_mask))
				{
					d->CheckPostDraw(state);
				}
			}
		}
	}
	state->GetRenderer()->PopMatrix(MATRIX_MODE_MODEL);
}

void Node3D::TravDraw(TravState* state)
{
	if(!Draw(state)) return;
	
	bool LoopOnNode=true;
	while (LoopOnNode)
	{

		TravPath::PathNeed pathneed = TravPath::all; // normal draw all
		//! if in "path" mode, then check what to do (continue or pop this node) 
		if (state->mPath)
		{
			pathneed = state->mPath->ComputeNeeds(state->mCurrentPass->sorter->GetCurrentWay());
			if (pathneed == TravPath::pop)
			{
				// pop
				return;
			}
		}
		else
		{
			LoopOnNode = false;
		}

		//! init state current matrix with this node matrix 
		GetLocalToGlobal();
		GetGlobalToLocal();
		state->SetCurrentLocalToGlobalMatrix(mLocalToGlobal);
		state->SetCurrentGlobalToLocalMatrix(mGlobalToLocal);

		//! prepare drawing (save states)
		mat4 PrevL2GMatrix = state->GetCurrentLocalToGlobalMatrix();
		mat4 PrevG2LMatrix = state->GetCurrentGlobalToLocalMatrix();


		bool OldVisibility = state->IsAllVisible();
		if (IsAllSonsVisible())
		{
			state->SetAllVisible(true);
		}

		Node3D* last_node = state->mCurrentNode;
		state->mCurrentNode = this;


		PreDrawDrawable(state);

		if(((int)pathneed) & ((int)(TravPath::stay)))
		{
			DrawDrawable(state);
		}
		

		//! recurse to sons
		if(((int)pathneed) & ((int)(TravPath::push)))
		{

			//! doItAgain flag is used when "path" mode ask for another draw at same "level" in path
			bool doItAgain=true;

			while(doItAgain)
			{
				doItAgain=false;
				int i=0;

				// TODO : find a way to mutualise loop
				if(state->IsAllVisible())
				{
					std::vector<ModifiableItemStruct>::const_iterator it=getItems().begin();
					std::vector<ModifiableItemStruct>::const_iterator itend=getItems().end();
					while(it!=itend)
					{
						//! if "path" mode, just retreive needed son
						if(state->mPath)
						{
							i=state->mPath->GotoWay(state->mCurrentPass->sorter->GetCurrentWay());
							it+=i;
						}
						if((*it).mItem->isUserFlagSet(UserFlagNode3D))
						{
							if(state->mCurrentPass->sorter)
							{
								state->mCurrentPass->sorter->PushCurrentPath(i);
							}

							//! recurse to sons
							it->mItem->as<Node3D>()->TravDraw(state);

	
							if(state->mCurrentPass->sorter)
							{
								state->mCurrentPass->sorter->PopCurrentPath();
							}

						}
						++it;
						i++;

						//! if in "path" mode don't loop for other sons drawing
						if(state->mPath)
						{
							break;
						}
					}
				}
				else
				{
					unsigned int index;
					unsigned int lastNode=mFirstVisibleNodeIndex+mVisibleNodeCount;

					for(index=mFirstVisibleNodeIndex;index<lastNode;index++)
					{


						//! if "path" mode, just retreive needed son
						if(state->mPath)
						{
							i=state->mPath->GotoWay(state->mCurrentPass->sorter->GetCurrentWay());
							index+=i;
						}

						{
							if(state->mCurrentPass->sorter)
							{
								state->mCurrentPass->sorter->PushCurrentPath(i);
							}

							//! recurse to sons
							state->GetSceneGraph()->GetVisibleNode(index)->TravDraw(state);

							if(state->mCurrentPass->sorter)
							{
								state->mCurrentPass->sorter->PopCurrentPath();
							}

						}

						i++;

						//! if in "path" mode don't loop for other sons drawing
						if(state->mPath)
						{
							break;
						}
					}
				}

				//! if next son in path is at same level in graph tree, then just loop 
				if(state->mPath)
				{
					if(state->mPath->IsNeedSameLevel(state->mCurrentPass->sorter->GetCurrentWay()))
					{
						doItAgain=true;
					}
				}
			}

		}

		
		
		PostDrawDrawable(state);
		

		state->mCurrentNode = last_node;

		//! pop state to previous values
		state->SetAllVisible(OldVisibility);

		//! pop state matrices
		state->SetCurrentLocalToGlobalMatrix(PrevL2GMatrix);
		state->SetCurrentGlobalToLocalMatrix(PrevG2LMatrix);
		
		//! pop path if needed in path mode
		if(state->mPath)
		{
			if(((int)pathneed) & ((int)(TravPath::stay)))
			{
				// goto next drawable in path
				state->mPath->PopWay(state->mCurrentPass->sorter->GetCurrentWay());
			}
		}
	}
}

#define SORT_NODES_FROM_CAMERA
#ifdef SORT_NODES_FROM_CAMERA
struct nodeDistPair
{
	Node3D*	node;
	float	dist;
	bool operator < (const nodeDistPair& str) const
	{
		if (node->getDrawPriority() == str.node->getDrawPriority())
		{
			if (dist == str.dist)
			{
				return (node < str.node);
			}
			return (dist < str.dist);
		}
		
		return node->getDrawPriority() > str.node->getDrawPriority();
	}
};
#endif

void Node3D::TravCull(TravState* state)
{
	ModuleSceneGraph*	scenegraph=state->GetSceneGraph();

	//! TravCull method is called on all sons wich are visible as some processings can be done 
	//! inside overloaded TravCull mMethods
	mFirstVisibleNodeIndex=scenegraph->GetCurrentVisibleNodeIndex();
	mVisibleNodeCount=0;

#ifndef KIGS_TOOLS
	if(IsAllSonsVisible())
		return;
#else
	if (IsAllSonsVisible() && !gCullingDrawBBox)
		return;
#endif

	bool front_to_back_sort = false;
#ifdef SORT_NODES_FROM_CAMERA
	if (isUserFlagSet(UserFlagCameraSort))
	{
		Camera* cam = state->GetCurrentCamera();
		v3f	camPos;
		cam->GetPosition(camPos.x, camPos.y, camPos.z);
		v3f	viewVector;
		cam->GetViewVector(viewVector.x, viewVector.y, viewVector.z);

		// insert nodes in sorted list using dist from camera
		std::set<nodeDistPair>	visibleSet;

		std::vector<ModifiableItemStruct>::const_iterator it;
		for (it = getItems().begin(); it != getItems().end(); ++it)
		{
			// if node3D, then full process
			if ((*it).mItem->isUserFlagSet(UserFlagNode3D))
			{
				auto node = it->mItem->as<Node3D>();
				if (node->Cull(state, mCullingMask))
				{
					nodeDistPair toAdd;
					toAdd.node = node;
					const mat4& nodemat = node->GetLocalToGlobal();
					v3f	nodepos(column(nodemat,3));
					nodepos -= (v3f&)camPos;

					toAdd.dist = dot(viewVector, nodepos);

					visibleSet.insert(toAdd);
				}
			}
		}

		if (visibleSet.size())
		{
			std::set<nodeDistPair>::iterator	currentN = visibleSet.begin();
			std::set<nodeDistPair>::iterator	endN = visibleSet.end();

			while (currentN != endN)
			{
				scenegraph->AddVisibleNode((*currentN).node);
				mVisibleNodeCount++;
				++currentN;
			}
		}
	}
	else // classic way
#endif
	if((front_to_back_sort = (state->mManageFrontToBackStruct && (isUserFlagSet(UserFlagFrontToBackSort) || state->mManageFrontToBackStruct->flag_set_in_parents))))
	{
		// insert nodes in sorted list using dist from camera
		std::set<nodeDistPair>	visibleSet;

		std::vector<ModifiableItemStruct>::const_iterator it;
		for (it = getItems().begin(); it != getItems().end(); ++it)
		{
			// if node3D, then full process
			if ((*it).mItem->isUserFlagSet(UserFlagNode3D))
			{
				auto node = it->mItem->as<Node3D>();
				if (node->Cull(state, mCullingMask))
				{
					nodeDistPair toAdd;
					toAdd.node = node;
					
					BBox nodeBBox;
					node->GetGlobalBoundingBox(nodeBBox.m_Min, nodeBBox.m_Max);
					v3f testPos(nodeBBox[state->mManageFrontToBackStruct->BBoxPointToTestIndexes[0]],
						nodeBBox[state->mManageFrontToBackStruct->BBoxPointToTestIndexes[1]],
						nodeBBox[state->mManageFrontToBackStruct->BBoxPointToTestIndexes[2]]);

					
					testPos -= state->mManageFrontToBackStruct->camPos;

					toAdd.dist = dot(state->mManageFrontToBackStruct->camViewVector, testPos);
					visibleSet.insert(toAdd);
				}
			}
		}

		if (visibleSet.size())
		{
			std::set<nodeDistPair>::iterator	currentN = visibleSet.begin();
			std::set<nodeDistPair>::iterator	endN = visibleSet.end();

			while (currentN != endN)
			{
				scenegraph->AddVisibleNode((*currentN).node);
				mVisibleNodeCount++;
				++currentN;
			}
		}
	}
	else
	{
		std::vector<ModifiableItemStruct>::const_iterator it;
		for (it = getItems().begin(); it != getItems().end(); ++it)
		{
			// if node3D, then full process
			if ((*it).mItem->isUserFlagSet(UserFlagNode3D))
			{
				auto node = it->mItem->as<Node3D>();
				if (node->Cull(state, mCullingMask))
				{
					scenegraph->AddVisibleNode(node);
					mVisibleNodeCount++;
				}
			}
		}
	}

	bool need_reset = front_to_back_sort && !state->mManageFrontToBackStruct->flag_set_in_parents;
	if (state->mManageFrontToBackStruct && front_to_back_sort)
		state->mManageFrontToBackStruct->flag_set_in_parents = true;

	// Travcull found nodes
	unsigned int i;
	unsigned int index = mFirstVisibleNodeIndex;
	for (i = 0; i < mVisibleNodeCount; i++)
	{
		((Node3D*)scenegraph->GetVisibleNode(index))->TravCull(state);
		index++;
	}

	if(need_reset)
		state->mManageFrontToBackStruct->flag_set_in_parents = false;
}

void Node3D::removeUser(CoreModifiable* user)
{
	if (user == mFatherNode)
		mFatherNode = nullptr;

	ParentClassType::removeUser(user);
}

void Node3D::addUser(CoreModifiable* user)
{
	if (user->isSubType(Node3D::mClassID))
	{
		mFatherNode = (Node3D*)user;
	}
	ParentClassType::addUser(user);
}

void Node3D::RecomputeLocalToGlobal()
{
	Node3D* nodes[512];
	int current_index = 1;
	nodes[0] = this;
	auto father = getFather();
	while (father && father->isUserFlagSet(LocalToGlobalMatrixIsDirty))
	{
		KIGS_ASSERT(current_index < 512);
		nodes[current_index++] = father;
		father = father->getFather();
	}

	mat4 father_local_to_global = mat4(1.0f);
	if (father)
	{
		father_local_to_global = father->mLocalToGlobal;
	}

	for (int i = current_index - 1; i >= 0; --i)
	{
		nodes[i]->mLocalToGlobal = nodes[i]->mTransform;
		nodes[i]->mLocalToGlobal = father_local_to_global * nodes[i]->mLocalToGlobal;

		const auto& l2g = nodes[i]->mLocalToGlobal;
		float sx = 1.0f / (l2g[0][0] * l2g[0][0] + l2g[0][1] * l2g[0][1] + l2g[0][2] * l2g[0][2]);
		float sy = 1.0f / (l2g[1][0] * l2g[1][0] + l2g[1][1] * l2g[1][1] + l2g[1][2] * l2g[1][2]);
		float sz = 1.0f / (l2g[2][0] * l2g[2][0] + l2g[2][1] * l2g[2][1] + l2g[2][2] * l2g[2][2]);

		if (length2(v3f{ sx - 1.0f, sy - 1.0f, sz - 1.0f }) > 0.001f)
			nodes[i]->setUserFlag(IsScaledFlag);
		else
			nodes[i]->unsetUserFlag(IsScaledFlag);

		nodes[i]->unsetUserFlag(LocalToGlobalMatrixIsDirty);
		father_local_to_global = nodes[i]->mLocalToGlobal;
	}
}

void Node3D::RecomputeGlobalToLocal()
{
	if (isUserFlagSet(LocalToGlobalMatrixIsDirty))
		RecomputeLocalToGlobal();

	float sx = 1.0f / (mLocalToGlobal[0][0] * mLocalToGlobal[0][0] + mLocalToGlobal[0][1] * mLocalToGlobal[0][1] + mLocalToGlobal[0][2] * mLocalToGlobal[0][2]);
	float sy = 1.0f / (mLocalToGlobal[1][0] * mLocalToGlobal[1][0] + mLocalToGlobal[1][1] * mLocalToGlobal[1][1] + mLocalToGlobal[1][2] * mLocalToGlobal[1][2]);
	float sz = 1.0f / (mLocalToGlobal[2][0] * mLocalToGlobal[2][0] + mLocalToGlobal[2][1] * mLocalToGlobal[2][1] + mLocalToGlobal[2][2] * mLocalToGlobal[2][2]);

	//! use 3x3 transpose matrix * invscale
	mGlobalToLocal[0][0] = mLocalToGlobal[0][0] * sx;
	mGlobalToLocal[0][1] = mLocalToGlobal[1][0] * sy;
	mGlobalToLocal[0][2] = mLocalToGlobal[2][0] * sz;

	mGlobalToLocal[1][0] = mLocalToGlobal[0][1] * sx;
	mGlobalToLocal[1][1] = mLocalToGlobal[1][1] * sy;
	mGlobalToLocal[1][2] = mLocalToGlobal[2][1] * sz;

	mGlobalToLocal[2][0] = mLocalToGlobal[0][2] * sx;
	mGlobalToLocal[2][1] = mLocalToGlobal[1][2] * sy;
	mGlobalToLocal[2][2] = mLocalToGlobal[2][2] * sz;

	//! then compute inverse translation
	v3f  invtrans(mLocalToGlobal[3][0], mLocalToGlobal[3][1], mLocalToGlobal[3][2]);
	invtrans = transformVector3(mGlobalToLocal ,invtrans);

	//! then set global to local translation
	mGlobalToLocal[3][0] = -invtrans[0];
	mGlobalToLocal[3][1] = -invtrans[1];
	mGlobalToLocal[3][2] = -invtrans[2];

	unsetUserFlag(GlobalToLocalMatrixIsDirty);
}

void Node3D::RecomputeBoundingBox()
{
	if (isUserFlagSet(LocalToGlobalMatrixIsDirty))
		RecomputeLocalToGlobal();
	
	bool isInit = false;

	//! init local and "local in father coordinate system" bounding box to "invalid" 
	mLocalBBox.m_Min = { (0.0f), (0.0f), (0.0f) };
	mLocalBBox.m_Max = { (-1.0f), (-1.0f), (-1.0f) };

	mBBox.m_Min = { (0.0f), (0.0f), (0.0f) };
	mBBox.m_Max = { (-1.0f), (-1.0f), (-1.0f) };

	v3f boundingp[2];
	bool	hasDrawable = false;
	//! first check drawable sons bounding box 
	std::vector<ModifiableItemStruct>::const_iterator it;
	for (it = getItems().begin(); it != getItems().end(); ++it)
	{
		if ((*it).mItem->isSubType(Drawable::mClassID))
		{
			auto drawable = it->mItem->as<Drawable>();
			//hasDrawable = true;
			//! if object has a valid BBox update return true
			if (drawable->BBoxUpdate(0))
			{
				drawable->GetNodeBoundingBox(boundingp[0], boundingp[1]);

				//! check if the box is ok
				if (boundingp[0].x <= boundingp[1].x)
				{
					if (isInit)
					{
						mLocalBBox.Update((v3f *)boundingp, 2);
					}
					else
					{
						mLocalBBox.Init((v3f *)boundingp, 2);
						isInit = true;
					}
				}
			}
		}
	}

	//! then recurse to sons 
	for (it = getItems().begin(); it != getItems().end(); ++it)
	{
		if ((*it).mItem->isUserFlagSet(UserFlagNode3D))
		{
			auto node = it->mItem->as<Node3D>();
			if (node->mIgnoreBBox) continue;
			
			//! this is the son bounding box in the coordinate system of this node
			node->GetBoundingBox(boundingp[0], boundingp[1]);

			//! check if the son Bbox is ok
			if (boundingp[0].x <= boundingp[1].x)
			{
				if (isInit)
				{
					mLocalBBox.Update((v3f *)boundingp, 2);
				}
				else
				{
					mLocalBBox.Init((v3f *)boundingp, 2);
					isInit = true;
				}
			}
		}
	}

	//! if inherited class overload GetBoundingBox method, then use the result bbox
	//! to update local bounding box
	GetNodeBoundingBox(boundingp[0], boundingp[1]);
	//! check if the box is ok
	if (boundingp[0].x <= boundingp[1].x)
	{
		if (isInit)
		{
			mLocalBBox.Update((v3f *)boundingp, 2);
		}
		else
		{
			mLocalBBox.Init((v3f *)boundingp, 2);
			isInit = true;
		}
	}

	//! ok still not init but has drawable ... Init bbox at our position
	if ((!isInit) && (hasDrawable))
	{
		v3f	zeroPos(0.0f, 0.0f, 0.0f);
		mLocalBBox.Init(zeroPos);
		isInit = true;
	}


	//! if Node3D has a valid local BBox, then we can compute BBox in father coordinate system  
	if (isInit)
	{
		mat4		BBoxTransformMatrix(mTransform);

		BBoxTransformMatrix[0][0] = fabsf(BBoxTransformMatrix[0][0]);
		BBoxTransformMatrix[1][0] = fabsf(BBoxTransformMatrix[1][0]);
		BBoxTransformMatrix[2][0] = fabsf(BBoxTransformMatrix[2][0]);

		BBoxTransformMatrix[0][1] = fabsf(BBoxTransformMatrix[0][1]);
		BBoxTransformMatrix[1][1] = fabsf(BBoxTransformMatrix[1][1]);
		BBoxTransformMatrix[2][1] = fabsf(BBoxTransformMatrix[2][1]);

		BBoxTransformMatrix[0][2] = fabsf(BBoxTransformMatrix[0][2]);
		BBoxTransformMatrix[1][2] = fabsf(BBoxTransformMatrix[1][2]);
		BBoxTransformMatrix[2][2] = fabsf(BBoxTransformMatrix[2][2]);

		//! compute center of the bbox so we can move it around origin
		v3f translation(mLocalBBox.m_Min);
		translation += mLocalBBox.m_Max;
		translation *= (0.5f);

		mBBox.m_Max = mLocalBBox.m_Max;
		mBBox.m_Max -= translation;

		//! transform diagonal to compute diagonal in father coordinate system
		mBBox.m_Max=transformVector3(BBoxTransformMatrix,mBBox.m_Max);
		translation=transformPoint3(mTransform,translation);

		//! and translate computed bbox at its final position in father coordinate system
		//translation+=mTransform.GetTranslation();

		mBBox.m_Min = -mBBox.m_Max;
		mBBox.m_Max += translation;
		mBBox.m_Min += translation;
	}
	unsetUserFlag(BoundingBoxIsDirty);
	setUserFlag(GlobalBoundingBoxIsDirty);
}

void Node3D::RecomputeGlobalBoundingBox()
{
	if (isUserFlagSet(BoundingBoxIsDirty) || isUserFlagSet(LocalToGlobalMatrixIsDirty))
		RecomputeBoundingBox();

	//! use abs(3x3 local to global matrix) to transform bbox
	mat4 BBoxTransformMatrix(mLocalToGlobal);

	BBoxTransformMatrix[0][0] = fabsf(BBoxTransformMatrix[0][0]);
	BBoxTransformMatrix[1][0] = fabsf(BBoxTransformMatrix[1][0]);
	BBoxTransformMatrix[2][0] = fabsf(BBoxTransformMatrix[2][0]);

	BBoxTransformMatrix[0][1] = fabsf(BBoxTransformMatrix[0][1]);
	BBoxTransformMatrix[1][1] = fabsf(BBoxTransformMatrix[1][1]);
	BBoxTransformMatrix[2][1] = fabsf(BBoxTransformMatrix[2][1]);

	BBoxTransformMatrix[0][2] = fabsf(BBoxTransformMatrix[0][2]);
	BBoxTransformMatrix[1][2] = fabsf(BBoxTransformMatrix[1][2]);
	BBoxTransformMatrix[2][2] = fabsf(BBoxTransformMatrix[2][2]);

	//! compute origin to bbox center so we can "move" the bbox at the origin
	v3f translation(mLocalBBox.m_Min);
	translation += mLocalBBox.m_Max;
	translation *= (0.5f);

	//! move the BBox max at origin (so that BBox min is -(BBox max))
	mGlobalBBox.m_Max = mLocalBBox.m_Max;
	mGlobalBBox.m_Max -= translation;

	//! transform BBox diagonal vector by BBoxTransformMatrix
	mGlobalBBox.m_Max=transformVector3(BBoxTransformMatrix,mGlobalBBox.m_Max);
	//! transform local translation in global coordinates
	translation=transformPoint3(mLocalToGlobal,translation);


	//! min is -max in local coordinates
	mGlobalBBox.m_Min = -mGlobalBBox.m_Max;
	//! move the global BBox at its final destination
	mGlobalBBox.m_Max += translation;
	mGlobalBBox.m_Min += translation;

	unsetUserFlag(GlobalBoundingBoxIsDirty);
}

void Node3D::PropagateNodePriorityDirtyToParents()
{
	auto node = getFather();
	if (node)
	{
		node->mDrawPriority =-1;
		node->PropagateNodePriorityDirtyToParents();
	}
}

int	Node3D::ComputeNodePriority()
{
	if (mDrawPriority == -1)
	{
		int newPriority = 0;
		int count = 0;
		int maxPriority = 0;
	
		for (auto& item : getItems())
		{
			if (item.mItem->isUserFlagSet(UserFlagNode3D))
			{
				auto node = item.mItem->as<Node3D>();
				int currentP = node->ComputeNodePriority();
				if (currentP > maxPriority)
				{
					maxPriority = currentP;
				}
				newPriority += currentP;
				count++;
				
			}
		}
		if (count)
		{
			mDrawPriority = (3*maxPriority + newPriority / count)/4;
		}
		else
		{
			mDrawPriority = 1;
		}

	}
	
	return mDrawPriority;
}

mat4 GetLocalLookAtPoint(Node3D* node, v3f global_point, bool force_up, v3f up_axis)
{
	v3f view = normalize(global_point - v3f(column(node->GetLocalToGlobal(),3)));

	auto g2l = node->getFather()->GetGlobalToLocal();
	view = transformVector3(g2l ,view);
	view=normalize(view);

	auto up = up_axis;
	auto right = cross(view,up); 
	right=normalize(right);
	if (force_up)
	{
		view = cross(up, right);
		view=normalize(view);
	}
	else
	{
		up = cross(right, view);
		up = normalize(up);
	}
	mat4 m = lookAt(v3f(column(node->GetLocal(),3)), v3f(column(node->GetLocal(),3)) + view, up);
	return m;
}

void Node3D::ApplyLocalTransform(const mat4& transform)
{
	auto m = GetLocal();
	ChangeMatrix(transform * m);
}

void Node3D::ApplyGlobalTransform(const mat4& transform)
{
	auto f = getFather();
	if (!f) 
		ApplyLocalTransform(transform);
	else
		ChangeMatrix(f->GetGlobalToLocal() * transform * f->GetLocalToGlobal() * GetLocal());
}

void Node3D::ChangeMatrixGlobal(const mat4& new_global_matrix)
{
	auto f = getFather();
	if (!f) ChangeMatrix(new_global_matrix);
	else ChangeMatrix(f->GetGlobalToLocal() * new_global_matrix);
}