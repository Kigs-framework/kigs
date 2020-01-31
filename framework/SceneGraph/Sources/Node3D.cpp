#include "PrecompiledHeaders.h"

#include "Node3D.h"
#include "ModuleSceneGraph.h"
#include "TravState.h"
#include "TravPath.h"
#include "CullingObject.h"
#include "Camera.h"
#include "ModuleRenderer.h"
#include "DrawableSorter.h"

#ifdef KIGS_TOOLS
extern bool gCullingDrawBBox; 
#endif

//IMPLEMENT_AND_REGISTER_CLASS_INFO(Node3D, Node3D, SceneGraph);
IMPLEMENT_CLASS_INFO(Node3D)

IMPLEMENT_CONSTRUCTOR(Node3D)
, CONSTRUCT_DECORABLE(Cull, Node3D)
, myFatherNode(nullptr)
{
	setUserFlag(UserFlagNode3D);
	//! init matrix to identity
	myTransform.SetIdentity();
	myLocalToGlobal.SetIdentity();
	myGlobalToLocal.SetIdentity();

	//! init visibility flag
	myIsVisible=0;
	myFirstVisibleNodeIndex=0;
	myVisibleNodeCount=0;
	myDrawPriority = -1;
}
void Node3D::PrepareExport(ExportSettings* settings)
{
	ParentClassType::PrepareExport(settings);
	if (!myTransform.IsIdentity())
	{
		AddDynamicVectorAttribute("LocalMatrix", &myTransform.e[0][0], 3 * 4);
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
			const kfloat* values = m->GetMatrixValues();
			Matrix3x4 matrix = Matrix3x4::IdentityMatrix();
			matrix.e[0][0] = values[0];
			matrix.e[0][1] = values[1];
			matrix.e[0][2] = values[2];
			matrix.e[1][0] = values[4];
			matrix.e[1][1] = values[5];
			matrix.e[1][2] = values[6];
			matrix.e[2][0] = values[8];
			matrix.e[2][1] = values[9];
			matrix.e[2][2] = values[10];
			matrix.e[3][0] = values[12];
			matrix.e[3][1] = values[13];
			matrix.e[3][2] = values[14];
			ChangeMatrix(matrix);
			return false;
		}
		else if (item->isSubType("Camera"))
		{
			ModuleSceneGraph* scenegraph = static_cast<ModuleSceneGraph*>(KigsCore::Instance()->GetMainModuleInList(SceneGraphModuleCoreIndex));
			scenegraph->AddDefferedItem(item.get(), DefferedAction::ADD_CAMERA);
		}
		else if (item->isSubType("API3DLight"))
		{
			ModuleSceneGraph* scenegraph = static_cast<ModuleSceneGraph*>(KigsCore::Instance()->GetMainModuleInList(SceneGraphModuleCoreIndex));
			scenegraph->AddDefferedItem(item.get(), DefferedAction::ADD_LIGHT);

		}

		if (item->isUserFlagSet(UserFlagNode3D))
		{
			auto node = static_cast<Node3D*>(item.get());
			node->SetFlag(LocalToGlobalMatrixIsDirty);
			node->SetFlag(GlobalToLocalMatrixIsDirty);
			node->PropagateDirtyFlagsToSons(node);
			PropagateNodePriorityDirtyToParents();
		}

		SetFlag(BoundingBoxIsDirty);
		SetFlag(GlobalBoundingBoxIsDirty);
		PropagateDirtyFlagsToParents((SceneNode*)item.get());
	}

	return SceneNode::addItem(item, pos PASS_LINK_NAME(linkName));
}

bool Node3D::removeItem(const CMSP& item DECLARE_LINK_NAME)
{
	//! if item is a SceneNode, then scenegraph will need update after node removing
	if(item->isSubType(SceneNode::myClassID))
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
			node->SetFlag(LocalToGlobalMatrixIsDirty);
			node->SetFlag(GlobalToLocalMatrixIsDirty);
			node->PropagateDirtyFlagsToSons(node);
			PropagateNodePriorityDirtyToParents();
		}
		SetFlag(BoundingBoxIsDirty);
		SetFlag(GlobalBoundingBoxIsDirty);
		PropagateDirtyFlagsToParents((SceneNode*)item.get());
	}

	return SceneNode::removeItem(item PASS_LINK_NAME(linkName));
}

bool Node3D::Draw(TravState* state)
{
	return (IsRenderable() && (myIsVisible||state->IsAllVisible()||IsAllSonsVisible()));
}

void Node3D::InitModifiable()
{
	ParentClassType::InitModifiable();
	Matrix3x4 m;
	if (getArrayValue("LocalMatrix", &m.e[0][0], 3 * 4))
	{
		ChangeMatrix(m);
		RemoveDynamicAttribute("LocalMatrix");
	}
	SetFlag(BoundingBoxIsDirty);

	int drawPriority;
	if (getValue("DrawPriority", drawPriority))
	{
		myDrawPriority = drawPriority;
		// we probably should remove dynamic attribute here ?
	}
}

void Node3D::ChangeMatrix(const Matrix3x4& newmatrix)
{
	if (!CompareMatrix(newmatrix, myTransform))
	{
		myTransform = newmatrix;

		SetFlag(LocalToGlobalMatrixIsDirty | GlobalToLocalMatrixIsDirty | BoundingBoxIsDirty | GlobalBoundingBoxIsDirty);

		PropagateDirtyFlagsToSons(this);
		PropagateDirtyFlagsToParents(this);
	}
}

void Node3D::PropagateDirtyFlagsToSons(SceneNode* source)
{
	for (auto& item : getItems())
	{
		if (item.myItem->isUserFlagSet(UserFlagNode3D))
		{
			auto& node = (SP<Node3D>&)(item.myItem);
			node->SetFlag(LocalToGlobalMatrixIsDirty| GlobalBoundingBoxIsDirty | GlobalToLocalMatrixIsDirty);
			node->PropagateDirtyFlagsToSons(source);
		}
	}
}

void Node3D::PropagateDirtyFlagsToParents(SceneNode* source)
{
	auto node = getFather();
	if (node)
	{
		node->SetFlag(BoundingBoxIsDirty | GlobalBoundingBoxIsDirty);
		node->PropagateDirtyFlagsToParents(source);
	}
}

void Node3D::PropagateDirtyFlags(SceneNode* source)
{
	PropagateDirtyFlagsToSons(source);
	PropagateDirtyFlagsToParents(source);
}

DECLARE_DECORABLE_IMPLEMENT(bool, Cull, Node3D, TravState* state, unsigned int cullingMask)
{
	//! no culling if not shown
	if(!IsRenderable())
	{
		myIsVisible=0;
		return false;
	}

	myCullingMask=cullingMask;
	kstl::vector<ModifiableItemStruct>::const_iterator ittemp;

	//! compute visibility only if we are not in a all visible branch
	if(!state->IsAllVisible())
	{
		//! check if node visibility was not already computed during this frame
		if(myIsVisible!=state->GetVisibilityFrame())
		{
			UnsetAllSonsVisible();
			//! cull object against current culling object
			CullingObject::CULLING_RESULT result= state->GetCullingObject()->Cull(this,myCullingMask);

			//! if the object if totally out, just set myIsVisible to 0 (not visible) and return false
			if(result==CullingObject::all_out)
			{
				myIsVisible=0;
#ifdef KIGS_TOOLS
				if (!gCullingDrawBBox)
#endif
					return false;

			}

			//! if the object if totally in, set myIsVisible to current frame (visible), set all visible flag to true and return true
			if(result==CullingObject::all_in)
			{
				myIsVisible=state->GetVisibilityFrame();
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
	myIsVisible=state->GetVisibilityFrame();
	return true;
}

void Node3D::PreDrawDrawable(TravState* state)
{	
	Matrix4x4 m{ GetLocalToGlobal() };
	state->GetRenderer()->PushAndLoadMatrix(MATRIX_MODE_MODEL, m);

	//! when in "path" mode or when no drawablesorter is available, then no question, predraw all drawable
	if(state->myPath || !state->mCurrentPass || !state->mCurrentPass->sorter)
	{
		for (auto& it : getItems())
		{
			if (it.myItem->isUserFlagSet(UserFlagDrawable))
			{
				auto& d = (SP<Drawable>&)it.myItem;
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
			if (it.myItem->isUserFlagSet(UserFlagDrawable))
			{
				auto& d = (SP<Drawable>&)it.myItem;
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
	if(state->myPath)
	{
		Drawable* drawable=state->myPath->GetDrawable();
		drawable->CheckDraw(state);
	}
	//! else when not in "path" mode and no drawable sorter is available, call DoDraw and update needed pass count on all drawable
	else if (!state->mCurrentPass || !state->mCurrentPass->sorter)
	{
		for (auto& it : getItems())
		{
			if (it.myItem->isUserFlagSet(UserFlagDrawable))
			{
				auto& d = (SP<Drawable>&)it.myItem;
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
			if(it.myItem->isUserFlagSet(UserFlagDrawable))
			{
				auto& d = (SP<Drawable>&)it.myItem;
				if (d->IsUsedInRenderPass(state->mCurrentPass->pass_mask))
				{
					if (d->IsRenderable() && d->IsSortable())
					{
						state->mCurrentPass->sorter->AddDrawable(d.get(), state);
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
	if(state->myPath || !state->mCurrentPass || !state->mCurrentPass->sorter)
	{
		for (auto& it : getItems())
		{
			if (it.myItem->isUserFlagSet(UserFlagDrawable))
			{
				auto& d = (SP<Drawable>&)it.myItem;
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
			if (it.myItem->isUserFlagSet(UserFlagDrawable))
			{
				auto& d = (SP<Drawable>&)it.myItem;
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
		if (state->myPath)
		{
			pathneed = state->myPath->ComputeNeeds(state->mCurrentPass->sorter->GetCurrentWay());
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
		state->SetCurrentLocalToGlobalMatrix(myLocalToGlobal);
		state->SetCurrentGlobalToLocalMatrix(myGlobalToLocal);

		//! prepare drawing (save states)
		Matrix3x4 PrevL2GMatrix = state->GetCurrentLocalToGlobalMatrix();
		Matrix3x4 PrevG2LMatrix = state->GetCurrentGlobalToLocalMatrix();


		bool OldVisibility = state->IsAllVisible();
		if (IsAllSonsVisible())
		{
			state->SetAllVisible(true);
		}

		Node3D* last_node = state->CurrentNode;
		state->CurrentNode = this;


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
					kstl::vector<ModifiableItemStruct>::const_iterator it=getItems().begin();
					kstl::vector<ModifiableItemStruct>::const_iterator itend=getItems().end();
					while(it!=itend)
					{
						//! if "path" mode, just retreive needed son
						if(state->myPath)
						{
							i=state->myPath->GotoWay(state->mCurrentPass->sorter->GetCurrentWay());
							it+=i;
						}
						if((*it).myItem->isUserFlagSet(UserFlagNode3D))
						{
							if(state->mCurrentPass->sorter)
							{
								state->mCurrentPass->sorter->PushCurrentPath(i);
							}

							//! recurse to sons
							((SP<Node3D>&)(*it).myItem)->TravDraw(state);

	
							if(state->mCurrentPass->sorter)
							{
								state->mCurrentPass->sorter->PopCurrentPath();
							}

						}
						++it;
						i++;

						//! if in "path" mode don't loop for other sons drawing
						if(state->myPath)
						{
							break;
						}
					}
				}
				else
				{
					unsigned int index;
					unsigned int lastNode=myFirstVisibleNodeIndex+myVisibleNodeCount;

					for(index=myFirstVisibleNodeIndex;index<lastNode;index++)
					{


						//! if "path" mode, just retreive needed son
						if(state->myPath)
						{
							i=state->myPath->GotoWay(state->mCurrentPass->sorter->GetCurrentWay());
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
						if(state->myPath)
						{
							break;
						}
					}
				}

				//! if next son in path is at same level in graph tree, then just loop 
				if(state->myPath)
				{
					if(state->myPath->IsNeedSameLevel(state->mCurrentPass->sorter->GetCurrentWay()))
					{
						doItAgain=true;
					}
				}
			}

		}

		
		
		PostDrawDrawable(state);
		

		state->CurrentNode = last_node;

		//! pop state to previous values
		state->SetAllVisible(OldVisibility);

		//! pop state matrices
		state->SetCurrentLocalToGlobalMatrix(PrevL2GMatrix);
		state->SetCurrentGlobalToLocalMatrix(PrevG2LMatrix);
		
		//! pop path if needed in path mode
		if(state->myPath)
		{
			if(((int)pathneed) & ((int)(TravPath::stay)))
			{
				// goto next drawable in path
				state->myPath->PopWay(state->mCurrentPass->sorter->GetCurrentWay());
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
	//! inside overloaded TravCull Methods
	myFirstVisibleNodeIndex=scenegraph->GetCurrentVisibleNodeIndex();
	myVisibleNodeCount=0;

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
		Point3D	camPos;
		cam->GetPosition(camPos.x, camPos.y, camPos.z);
		Vector3D	viewVector;
		cam->GetViewVector(viewVector.x, viewVector.y, viewVector.z);

		// insert nodes in sorted list using dist from camera
		kstl::set<nodeDistPair>	visibleSet;

		kstl::vector<ModifiableItemStruct>::const_iterator it;
		for (it = getItems().begin(); it != getItems().end(); ++it)
		{
			// if node3D, then full process
			if ((*it).myItem->isUserFlagSet(UserFlagNode3D))
			{
				SP<Node3D>& node = (SP<Node3D>&)(*it).myItem;
				if (node->Cull(state, myCullingMask))
				{
					nodeDistPair toAdd;
					toAdd.node = node.get();
					const Matrix3x4& nodemat = node->GetLocalToGlobal();
					Point3D	nodepos(nodemat.GetTranslation());
					nodepos -= camPos;

					toAdd.dist = Dot(viewVector, nodepos);

					visibleSet.insert(toAdd);
				}
			}
		}

		if (visibleSet.size())
		{
			kstl::set<nodeDistPair>::iterator	currentN = visibleSet.begin();
			kstl::set<nodeDistPair>::iterator	endN = visibleSet.end();

			while (currentN != endN)
			{
				scenegraph->AddVisibleNode((*currentN).node);
				myVisibleNodeCount++;
				++currentN;
			}
		}
	}
	else // classic way
#endif
	if((front_to_back_sort = (state->pManageFrontToBackStruct && (isUserFlagSet(UserFlagFrontToBackSort) || state->pManageFrontToBackStruct->flag_set_in_parents))))
	{
		// insert nodes in sorted list using dist from camera
		kstl::set<nodeDistPair>	visibleSet;

		kstl::vector<ModifiableItemStruct>::const_iterator it;
		for (it = getItems().begin(); it != getItems().end(); ++it)
		{
			// if node3D, then full process
			if ((*it).myItem->isUserFlagSet(UserFlagNode3D))
			{
				SP<Node3D>& node = (SP<Node3D>&)(*it).myItem;
				if (node->Cull(state, myCullingMask))
				{
					nodeDistPair toAdd;
					toAdd.node = node.get();
					
					BBox nodeBBox;
					node->GetGlobalBoundingBox(nodeBBox.m_Min, nodeBBox.m_Max);
					Point3D testPos(nodeBBox[state->pManageFrontToBackStruct->BBoxPointToTestIndexes[0]],
						nodeBBox[state->pManageFrontToBackStruct->BBoxPointToTestIndexes[1]],
						nodeBBox[state->pManageFrontToBackStruct->BBoxPointToTestIndexes[2]]);

					
					testPos -= state->pManageFrontToBackStruct->camPos;

					toAdd.dist = Dot(state->pManageFrontToBackStruct->camViewVector, testPos);
					visibleSet.insert(toAdd);
				}
			}
		}

		if (visibleSet.size())
		{
			kstl::set<nodeDistPair>::iterator	currentN = visibleSet.begin();
			kstl::set<nodeDistPair>::iterator	endN = visibleSet.end();

			while (currentN != endN)
			{
				scenegraph->AddVisibleNode((*currentN).node);
				myVisibleNodeCount++;
				++currentN;
			}
		}
	}
	else
	{
		kstl::vector<ModifiableItemStruct>::const_iterator it;
		for (it = getItems().begin(); it != getItems().end(); ++it)
		{
			// if node3D, then full process
			if ((*it).myItem->isUserFlagSet(UserFlagNode3D))
			{
				SP<Node3D>& node = (SP<Node3D>&)(*it).myItem;
				if (node->Cull(state, myCullingMask))
				{
					scenegraph->AddVisibleNode(node.get());
					myVisibleNodeCount++;
				}
			}
		}
	}

	bool need_reset = front_to_back_sort && !state->pManageFrontToBackStruct->flag_set_in_parents;
	if (state->pManageFrontToBackStruct && front_to_back_sort)
		state->pManageFrontToBackStruct->flag_set_in_parents = true;

	// Travcull found nodes
	unsigned int i;
	unsigned int index = myFirstVisibleNodeIndex;
	for (i = 0; i < myVisibleNodeCount; i++)
	{
		((Node3D*)scenegraph->GetVisibleNode(index))->TravCull(state);
		index++;
	}

	if(need_reset)
		state->pManageFrontToBackStruct->flag_set_in_parents = false;
}

void Node3D::removeUser(CoreModifiable* user)
{
	if (user == myFatherNode)
		myFatherNode = nullptr;

	ParentClassType::removeUser(user);
}

void Node3D::addUser(CoreModifiable* user)
{
	if (user->isSubType(Node3D::myClassID))
	{
		myFatherNode = (Node3D*)user;
	}
	ParentClassType::addUser(user);
}

void Node3D::RecomputeLocalToGlobal()
{
	Node3D* nodes[512];
	int current_index = 1;
	nodes[0] = this;
	auto father = getFather();
	while (father && father->HasFlag(LocalToGlobalMatrixIsDirty))
	{
		KIGS_ASSERT(current_index < 512);
		nodes[current_index++] = father;
		father = father->getFather();
	}

	Matrix3x4 father_local_to_global = Matrix3x4::IdentityMatrix();
	if (father)
	{
		father_local_to_global = father->myLocalToGlobal;
	}

	for (int i = current_index - 1; i >= 0; --i)
	{
		nodes[i]->myLocalToGlobal = nodes[i]->myTransform;
		nodes[i]->myLocalToGlobal.PostMultiply(father_local_to_global);

		const auto& l2g = nodes[i]->myLocalToGlobal;
		kfloat sx = 1.0f / (l2g.e[0][0] * l2g.e[0][0] + l2g.e[0][1] * l2g.e[0][1] + l2g.e[0][2] * l2g.e[0][2]);
		kfloat sy = 1.0f / (l2g.e[1][0] * l2g.e[1][0] + l2g.e[1][1] * l2g.e[1][1] + l2g.e[1][2] * l2g.e[1][2]);
		kfloat sz = 1.0f / (l2g.e[2][0] * l2g.e[2][0] + l2g.e[2][1] * l2g.e[2][1] + l2g.e[2][2] * l2g.e[2][2]);

		if (NormSquare(v3f{ sx - 1.0f, sy - 1.0f, sz - 1.0f }) > 0.001f)
			nodes[i]->SetFlag(IsScaledFlag);
		else
			nodes[i]->UnsetFlag(IsScaledFlag);

		nodes[i]->UnsetFlag(LocalToGlobalMatrixIsDirty);
		father_local_to_global = nodes[i]->myLocalToGlobal;
	}
}

void Node3D::RecomputeGlobalToLocal()
{
	if (HasFlag(LocalToGlobalMatrixIsDirty))
		RecomputeLocalToGlobal();

	kfloat sx = 1.0f / (myLocalToGlobal.e[0][0] * myLocalToGlobal.e[0][0] + myLocalToGlobal.e[0][1] * myLocalToGlobal.e[0][1] + myLocalToGlobal.e[0][2] * myLocalToGlobal.e[0][2]);
	kfloat sy = 1.0f / (myLocalToGlobal.e[1][0] * myLocalToGlobal.e[1][0] + myLocalToGlobal.e[1][1] * myLocalToGlobal.e[1][1] + myLocalToGlobal.e[1][2] * myLocalToGlobal.e[1][2]);
	kfloat sz = 1.0f / (myLocalToGlobal.e[2][0] * myLocalToGlobal.e[2][0] + myLocalToGlobal.e[2][1] * myLocalToGlobal.e[2][1] + myLocalToGlobal.e[2][2] * myLocalToGlobal.e[2][2]);

	//! use 3x3 transpose matrix * invscale
	myGlobalToLocal.e[0][0] = myLocalToGlobal.e[0][0] * sx;
	myGlobalToLocal.e[0][1] = myLocalToGlobal.e[1][0] * sy;
	myGlobalToLocal.e[0][2] = myLocalToGlobal.e[2][0] * sz;

	myGlobalToLocal.e[1][0] = myLocalToGlobal.e[0][1] * sx;
	myGlobalToLocal.e[1][1] = myLocalToGlobal.e[1][1] * sy;
	myGlobalToLocal.e[1][2] = myLocalToGlobal.e[2][1] * sz;

	myGlobalToLocal.e[2][0] = myLocalToGlobal.e[0][2] * sx;
	myGlobalToLocal.e[2][1] = myLocalToGlobal.e[1][2] * sy;
	myGlobalToLocal.e[2][2] = myLocalToGlobal.e[2][2] * sz;

	//! then compute inverse translation
	Vector3D  invtrans(myLocalToGlobal.e[3][0], myLocalToGlobal.e[3][1], myLocalToGlobal.e[3][2]);
	myGlobalToLocal.TransformVector(&invtrans);

	//! then set global to local translation
	myGlobalToLocal.e[3][0] = -invtrans[0];
	myGlobalToLocal.e[3][1] = -invtrans[1];
	myGlobalToLocal.e[3][2] = -invtrans[2];

	UnsetFlag(GlobalToLocalMatrixIsDirty);
}

void Node3D::RecomputeBoundingBox()
{
	if (HasFlag(LocalToGlobalMatrixIsDirty)) 
		RecomputeLocalToGlobal();
	
	bool isInit = false;

	//! init local and "local in father coordinate system" bounding box to "invalid" 
	myLocalBBox.m_Min.Set((0.0f), (0.0f), (0.0f));
	myLocalBBox.m_Max.Set((-1.0f), (-1.0f), (-1.0f));

	myBBox.m_Min.Set((0.0f), (0.0f), (0.0f));
	myBBox.m_Max.Set((-1.0f), (-1.0f), (-1.0f));

	Point3D boundingp[2];
	bool	hasDrawable = false;
	//! first check drawable sons bounding box 
	kstl::vector<ModifiableItemStruct>::const_iterator it;
	for (it = getItems().begin(); it != getItems().end(); ++it)
	{
		if ((*it).myItem->isSubType(Drawable::myClassID))
		{
			SP<Drawable>& drawable = (SP<Drawable>&)(*it).myItem;
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
						myLocalBBox.Update((Point3D *)boundingp, 2);
					}
					else
					{
						myLocalBBox.Init((Point3D *)boundingp, 2);
						isInit = true;
					}
				}
			}
		}
	}

	//! then recurse to sons 
	for (it = getItems().begin(); it != getItems().end(); ++it)
	{
		if ((*it).myItem->isUserFlagSet(UserFlagNode3D))
		{
			SP<Node3D>& node = (SP<Node3D>&)(*it).myItem;

			if (node->mIgnoreBBox) continue;
			
			//! this is the son bounding box in the coordinate system of this node
			node->GetBoundingBox(boundingp[0], boundingp[1]);

			//! check if the son Bbox is ok
			if (boundingp[0].x <= boundingp[1].x)
			{
				if (isInit)
				{
					myLocalBBox.Update((Point3D *)boundingp, 2);
				}
				else
				{
					myLocalBBox.Init((Point3D *)boundingp, 2);
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
			myLocalBBox.Update((Point3D *)boundingp, 2);
		}
		else
		{
			myLocalBBox.Init((Point3D *)boundingp, 2);
			isInit = true;
		}
	}

	//! ok still not init but has drawable ... Init bbox at our position
	if ((!isInit) && (hasDrawable))
	{
		Point3D	zeroPos(0.0f, 0.0f, 0.0f);
		myLocalBBox.Init(zeroPos);
		isInit = true;
	}


	//! if Node3D has a valid local BBox, then we can compute BBox in father coordinate system  
	if (isInit)
	{
		Matrix3x4		BBoxTransformMatrix(myTransform);

		BBoxTransformMatrix.e[0][0] = fabsf(BBoxTransformMatrix.e[0][0]);
		BBoxTransformMatrix.e[1][0] = fabsf(BBoxTransformMatrix.e[1][0]);
		BBoxTransformMatrix.e[2][0] = fabsf(BBoxTransformMatrix.e[2][0]);

		BBoxTransformMatrix.e[0][1] = fabsf(BBoxTransformMatrix.e[0][1]);
		BBoxTransformMatrix.e[1][1] = fabsf(BBoxTransformMatrix.e[1][1]);
		BBoxTransformMatrix.e[2][1] = fabsf(BBoxTransformMatrix.e[2][1]);

		BBoxTransformMatrix.e[0][2] = fabsf(BBoxTransformMatrix.e[0][2]);
		BBoxTransformMatrix.e[1][2] = fabsf(BBoxTransformMatrix.e[1][2]);
		BBoxTransformMatrix.e[2][2] = fabsf(BBoxTransformMatrix.e[2][2]);

		//! compute center of the bbox so we can move it around origin
		Vector3D translation(myLocalBBox.m_Min);
		translation += myLocalBBox.m_Max;
		translation *= (0.5f);

		myBBox.m_Max = myLocalBBox.m_Max;
		myBBox.m_Max -= translation;

		//! transform diagonal to compute diagonal in father coordinate system
		BBoxTransformMatrix.TransformVector((Vector3D*)&myBBox.m_Max);
		Vector3D finaltranslate;
		myTransform.TransformPoint((Point3D*)&translation, (Point3D*)&finaltranslate);

		//! and translate computed bbox at its final position in father coordinate system
		//translation+=myTransform.GetTranslation();

		myBBox.m_Min = -myBBox.m_Max;
		myBBox.m_Max += finaltranslate;
		myBBox.m_Min += finaltranslate;
	}
	UnsetFlag(BoundingBoxIsDirty);
	SetFlag(GlobalBoundingBoxIsDirty);
}

void Node3D::RecomputeGlobalBoundingBox()
{
	if (HasFlag(BoundingBoxIsDirty) || HasFlag(LocalToGlobalMatrixIsDirty)) 
		RecomputeBoundingBox();

	//! use abs(3x3 local to global matrix) to transform bbox
	Matrix3x4 BBoxTransformMatrix(myLocalToGlobal);

	BBoxTransformMatrix.e[0][0] = fabsf(BBoxTransformMatrix.e[0][0]);
	BBoxTransformMatrix.e[1][0] = fabsf(BBoxTransformMatrix.e[1][0]);
	BBoxTransformMatrix.e[2][0] = fabsf(BBoxTransformMatrix.e[2][0]);

	BBoxTransformMatrix.e[0][1] = fabsf(BBoxTransformMatrix.e[0][1]);
	BBoxTransformMatrix.e[1][1] = fabsf(BBoxTransformMatrix.e[1][1]);
	BBoxTransformMatrix.e[2][1] = fabsf(BBoxTransformMatrix.e[2][1]);

	BBoxTransformMatrix.e[0][2] = fabsf(BBoxTransformMatrix.e[0][2]);
	BBoxTransformMatrix.e[1][2] = fabsf(BBoxTransformMatrix.e[1][2]);
	BBoxTransformMatrix.e[2][2] = fabsf(BBoxTransformMatrix.e[2][2]);

	//! compute origin to bbox center so we can "move" the bbox at the origin
	Vector3D translation(myLocalBBox.m_Min);
	translation += myLocalBBox.m_Max;
	translation *= (0.5f);

	//! move the BBox max at origin (so that BBox min is -(BBox max))
	myGlobalBBox.m_Max = myLocalBBox.m_Max;
	myGlobalBBox.m_Max -= translation;

	//! transform BBox diagonal vector by BBoxTransformMatrix
	BBoxTransformMatrix.TransformVector((Vector3D*)&myGlobalBBox.m_Max);
	//! transform local translation in global coordinates
	Point3D	finalTranslate;
	myLocalToGlobal.TransformPoint((Point3D*)&translation, (Point3D*)&finalTranslate);

	//! and add local to global translation 
	//translation+=myLocalToGlobal.GetTranslation();

	//! min is -max in local coordinates
	myGlobalBBox.m_Min = -myGlobalBBox.m_Max;
	//! move the global BBox at its final destination
	myGlobalBBox.m_Max += finalTranslate;
	myGlobalBBox.m_Min += finalTranslate;

	UnsetFlag(GlobalBoundingBoxIsDirty);
}

void Node3D::PropagateNodePriorityDirtyToParents()
{
	auto node = getFather();
	if (node)
	{
		node->myDrawPriority =-1;
		node->PropagateNodePriorityDirtyToParents();
	}
}

int	Node3D::ComputeNodePriority()
{
	if (myDrawPriority == -1)
	{
		int newPriority = 0;
		int count = 0;
		int maxPriority = 0;
	
		for (auto& item : getItems())
		{
			if (item.myItem->isUserFlagSet(UserFlagNode3D))
			{
				auto& node = (SP<Node3D>&)item.myItem;
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
			myDrawPriority = (3*maxPriority + newPriority / count)/4;
		}
		else
		{
			myDrawPriority = 1;
		}

	}
	
	return myDrawPriority;
}

mat3x4 GetLocalLookAtPoint(Node3D* node, v3f global_point, bool force_up, v3f up_axis)
{
	Vector3D view = global_point - node->GetLocalToGlobal().Pos;
	view.Normalize();

	auto g2l = node->getFather()->GetGlobalToLocal();
	g2l.TransformVector(&view);
	view.Normalize();

	auto up = up_axis;
	auto right = view ^ up; right.Normalize();
	if (force_up)
	{
		view = up ^ right;
		view.Normalize();
	}
	else
	{
		up = right ^ view;
		up.Normalize();
	}
	Matrix3x4 m = Matrix3x4::LookAt(node->GetLocal().Pos, node->GetLocal().Pos + view, up);
	return m;
}

void Node3D::ApplyLocalTransform(const Matrix3x4& transform)
{
	auto m = GetLocal();
	ChangeMatrix(transform * m);
}

void Node3D::ApplyGlobalTransform(const Matrix3x4& transform)
{
	auto f = getFather();
	if (!f) 
		ApplyLocalTransform(transform);
	else
		ChangeMatrix(f->GetGlobalToLocal() * transform * f->GetLocalToGlobal() * GetLocal());
}

void Node3D::ChangeMatrixGlobal(const Matrix3x4& new_global_matrix)
{
	auto f = getFather();
	if (!f) ChangeMatrix(new_global_matrix);
	else ChangeMatrix(f->GetGlobalToLocal() * new_global_matrix);
}