
#include "RendererDX11.h"


#ifdef WUP
#include "Platform/Main/BaseApp.h"

#endif
void RendererDX11::PlatformInit(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params)
{
	auto space = App::GetApp()->GetHolographicSpace();
	auto dxinstance = getDXInstance();
	dxinstance->mHolographicSpace = space;
	dxinstance->mStationaryReferenceFrame = App::GetApp()->GetStationaryReferenceFrame();
	dxinstance->mWindow = App::GetApp()->GetWindow();
}

void RendererDX11::PlatformUpdate(const Timer& timer, void* addParam)
{
}

void RendererDX11::PlatformClose()
{
}
