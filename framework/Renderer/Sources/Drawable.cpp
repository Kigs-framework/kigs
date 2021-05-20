#include "PrecompiledHeaders.h"

#include "Drawable.h"
#include "Node3D.h"
#include "TravState.h"
#include "ModuleRenderer.h"
#include "ModuleSceneGraph.h"

//#define DRAW_DEBUG
#ifdef DRAW_DEBUG
#include "GLSLDebugDraw.h"
#endif
    
IMPLEMENT_CLASS_INFO(Drawable)

IMPLEMENT_CONSTRUCTOR(Drawable)
{
	AddDynamicAttribute(ATTRIBUTE_TYPE::BOOL, "TransarencyFlag", false);
	setUserFlag(UserFlagDrawable);
	mDrawingNeeds=(unsigned int)Not_Init;
}    

void Drawable::UpdateDrawingNeeds()
{
	if(mDrawingNeeds&((unsigned int)Not_Init))
	{

		mDrawingNeeds=GetSelfDrawingNeeds();
		
		// update my flag using son flag
		kstl::vector<ModifiableItemStruct>::const_iterator it;

		for (it=getItems().begin();it!=getItems().end();++it)
		{
			if((*it).mItem->isUserFlagSet(UserFlagDrawable))
			{
				SP<Drawable>& drawable=(SP<Drawable> & )(*it).mItem;
				drawable->UpdateDrawingNeeds();
				mDrawingNeeds|=drawable->GetDrawingNeeds();
			}
		}
		// then ask fathers to update
		kstl::vector<CoreModifiable*>::const_iterator itfather;
		
		for (itfather=GetParents().begin();itfather!=GetParents().end();++itfather)
		{
			if((*itfather)->isUserFlagSet(UserFlagDrawable))
			{
				Drawable* drawable=(Drawable*)(*itfather);
				drawable->mDrawingNeeds=(unsigned int)Not_Init;
				drawable->UpdateDrawingNeeds();
			}
		}
	}
}

void Drawable::DoPreDraw(TravState* state)
{	
	if(PreDraw(state)) // first ProtectedPreDraw for this
	{
#ifdef KIGS_TOOLS
		if (state && state->mCurrentPass && state->mCurrentPass->record_pass) 
		{ 
			RenderPass::DrawPathElement el;
			el.DrawStep = RenderPass::DrawPathElement::Step::PreDraw;
			el.Object = SharedFromThis();
			state->mCurrentPass->debug_draw_path.push_back(el);
		}
#endif
		// then PreDraw for sons
		kstl::vector<ModifiableItemStruct>::const_iterator it;

		for (it=getItems().begin();it!=getItems().end();++it)
		{
			if((*it).mItem->isUserFlagSet(UserFlagDrawable))
			{
				SP<Drawable>& drawable=(SP<Drawable>&)(*it).mItem;
				drawable->CheckPreDraw(state);
			}
		}        
	}
}

void Drawable::DoDraw(TravState* state)
{
	if(Draw(state)) // first ProtectedDraw for this
	{
#ifdef KIGS_TOOLS
		if (state && state->mCurrentPass && state->mCurrentPass->record_pass) 
		{ 
			RenderPass::DrawPathElement el;
			el.DrawStep = RenderPass::DrawPathElement::Step::Draw;
			el.Object = SharedFromThis();
			state->mCurrentPass->debug_draw_path.push_back(el);
		}
#endif
		// then Draw for sons
		kstl::vector<ModifiableItemStruct>::const_iterator it;

		for (it=getItems().begin();it!=getItems().end();++it)
		{
			if((*it).mItem->isUserFlagSet(UserFlagDrawable))
			{
				SP<Drawable>& drawable = (SP<Drawable>&)(*it).mItem;
				drawable->CheckDraw(state);
			}
		}      
	}
}

void Drawable::DoPostDraw(TravState* state)
{
	if(PostDraw(state)) // first PostDraw for this
	{
#ifdef KIGS_TOOLS
		if (state && state->mCurrentPass && state->mCurrentPass->record_pass) 
		{
			RenderPass::DrawPathElement el;
			el.DrawStep = RenderPass::DrawPathElement::Step::PostDraw;
			el.Object = SharedFromThis();
			state->mCurrentPass->debug_draw_path.push_back(el);
		}
#endif
		// then PostDraw for sons
		kstl::vector<ModifiableItemStruct>::const_iterator it;

		for (it=getItems().begin();it!=getItems().end();++it)
		{
			if((*it).mItem->isUserFlagSet(UserFlagDrawable))
			{
				SP<Drawable>& drawable = (SP<Drawable>&)(*it).mItem;
				drawable->CheckPostDraw(state);
			}
		}       
	}
}

void Drawable::InitModifiable()
{
	UpdateDrawingNeeds();
	SceneNode::InitModifiable();

	// Support for old TransarencyFlag attribute
	if (getValue<bool>("TransarencyFlag") && mRenderPassMask == 1)
	{
		mRenderPassMask = 2;
	}
	RemoveDynamicAttribute("TransarencyFlag");
}

bool Drawable::PreDraw(TravState* state)
{
	if(IsRenderable())
	{
		return true;
	}
	return false;
}

bool Drawable::Draw(TravState* state)
{
	if(IsRenderable())
	{
#ifdef DRAW_DEBUG
		if (BBoxUpdate(0))
		{
			Node3D* parent = (Node3D*)getFirstParent(Node3D::mClassID);
			if (parent)
			{
				Point3D min, max;
				GetBoundingBox(min, max);

				Point3D p[8];
				p[0].Set(max.x, min.y, min.z);
				p[1].Set(max.x, max.y, min.z);
				p[2].Set(min.x, max.y, min.z);
				p[3].Set(min.x, min.y, min.z);

				p[4].Set(max.x, min.y, max.z);
				p[5].Set(max.x, max.y, max.z);
				p[6].Set(min.x, max.y, max.z);
				p[7].Set(min.x, min.y, max.z);



				parent->GetLocalToGlobal().TransformPoints(p, 8);
				dd::box(p, Vector3D(255,0,0));
			}
		}
#endif

		return true;
	}
	
	return false;
}

bool Drawable::PostDraw(TravState* state)
{
	if(IsRenderable())
	{
		return true;
	}
	return false;
}

bool Drawable::addItem(const CMSP& item, ItemPosition pos DECLARE_LINK_NAME)
{

	mDrawingNeeds=(unsigned int)Not_Init;
	
	bool result=SceneNode::addItem(item,pos PASS_LINK_NAME(linkName));

	UpdateDrawingNeeds();

	return result;

}

bool Drawable::removeItem(const CMSP& item DECLARE_LINK_NAME)
{
	bool result=SceneNode::removeItem(item PASS_LINK_NAME(linkName));
	mDrawingNeeds=(unsigned int)Not_Init;
	UpdateDrawingNeeds();

	return result;
}

void Drawable::FatherNode3DNeedBoundingBoxUpdate()
{
	kstl::vector<CoreModifiable*>::const_iterator it;

	const kstl::vector<CoreModifiable*>& parents=GetParents();

	for(it=parents.begin();it!=parents.end();++it)
	{
		if((*it)->isSubType(Node3D::mClassID))
		{
			Node3D* father=(Node3D*)(*it);
      
			father->NeedBoundingBoxUpdate();
		}
	}
}
