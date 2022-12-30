#include "PrecompiledHeaders.h"

#include "MaterialStage.h"
#include "Material.h"
#include "Texture.h"
#include "ModuleRenderer.h"
#include "TravState.h"
#include "NotificationCenter.h"

using namespace Kigs::Draw;

#ifndef NO_MULTISTAGE_RENDERING

IMPLEMENT_CLASS_INFO(MaterialStage);

IMPLEMENT_CONSTRUCTOR(MaterialStage)
{
	//KigsCore::GetNotificationCenter()->addObserver(this, "ResetContext", "ResetContext");
}

bool MaterialStage::PreDraw(TravState* state)
{
	if (Drawable::PreDraw(state))
	{
		state->GetRenderer()->ActiveTextureChannel(mChannel);
		return true;
	}
	return false;

}

bool MaterialStage::PostDraw(TravState* state)
{
	if (Drawable::PostDraw(state))
	{
		state->GetRenderer()->ActiveTextureChannel(mChannel); 
		return true;
	}
	return false;
}

DEFINE_METHOD(MaterialStage, ResetContext)
{
	std::vector<CMSP> inst;
	GetSonInstancesByType("Texture", inst);
	for (auto it : inst)
	{
		it->as<Texture>()->setUserFlag(Texture::isDirtyContext);
		it->ReInit();	
	}
	return false;
}


#endif
