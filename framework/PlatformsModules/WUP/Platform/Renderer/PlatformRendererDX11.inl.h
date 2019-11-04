
#include "RendererDX11.h"


#ifdef WUP
#include "Platform/Main/BaseApp.h"

#endif
void RendererDX11::PlatformInit(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params)
{
	if (gIsHolographic)
	{
		auto space = App::GetApp()->GetHolographicSpace();
		auto frame = myDXInstance.mCurrentFrame = space.CreateNextFrame();
		auto prediction = frame.CurrentPrediction();
		auto poses = prediction.CameraPoses();
		auto coordinate_system = App::GetApp()->GetStationaryReferenceFrame().CoordinateSystem();
		for (auto pose : poses)
		{
			myDXInstance.mCurrentRenderingParameters = frame.GetRenderingParameters(pose);
		}
		KIGS_ASSERT(myDXInstance.mCurrentRenderingParameters);
	}
}

void RendererDX11::PlatformUpdate(const Timer& timer, void* addParam)
{
}

void RendererDX11::PlatformClose()
{
}
