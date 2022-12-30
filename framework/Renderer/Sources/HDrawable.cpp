#include "PrecompiledHeaders.h"

#include "HDrawable.h"
#include "Node3D.h"
#include "TravState.h"
#include "ModuleRenderer.h"
#include "ModuleSceneGraph.h"
    
using namespace Kigs::Draw;

IMPLEMENT_CLASS_INFO(HDrawable)

HDrawable::HDrawable(const std::string& name,CLASS_NAME_TREE_ARG) : Drawable(name,PASS_CLASS_NAME_TREE_ARG)
{
}    
    
HDrawable::~HDrawable()
{
}    

void HDrawable::DoPreDraw(TravState* state)
{	
	// for HDrawable, predraw is done just before sons draw
}

void HDrawable::DoDraw(TravState* state)
{
	if(Draw(state)) // first ProtectedDraw for this
	{
		PreDraw(state); //PreDraw for this
		// then Draw for sons
		std::vector<ModifiableItemStruct>::const_iterator it;

		for (it=getItems().begin();it!=getItems().end();++it)
		{
			if((*it).mItem->isUserFlagSet(UserFlagDrawable))
			{
				Drawable* drawable = (*it).mItem->as<Drawable>();
				drawable->CheckPreDraw(state);
				drawable->CheckDraw(state);
				drawable->CheckPostDraw(state);
			}
		}   
		PostDraw(state); // PostDraw for this
	}
}

void HDrawable::DoPostDraw(TravState* state)
{
	// for HDrawable, postdraw is done just after sons draw
}

// special case as HDrawable don't need predraw / postdraw even if sons need it

void	HDrawable::UpdateDrawingNeeds()
{
	if(mDrawingNeeds&((unsigned int)Not_Init))
	{

		mDrawingNeeds=GetSelfDrawingNeeds();
		
		// then ask fathers to update
		std::vector<CoreModifiable*>::const_iterator itfather;
		
		for (itfather=GetParents().begin();itfather!=GetParents().end();++itfather)
		{
			if((*itfather)->isUserFlagSet(UserFlagDrawable))
			{
				Drawable* drawable=(Drawable*)(*itfather);
				drawable->SetDrawingNeeds((unsigned int)Not_Init);
				drawable->UpdateDrawingNeeds();
			}
		}

	}
}
