#include "PrecompiledHeaders.h"
#include "DrawableCallback.h"
#include "NotificationCenter.h"

IMPLEMENT_CLASS_INFO(DrawableCallback)

DrawableCallback::DrawableCallback(const kstl::string& name,CLASS_NAME_TREE_ARG) : Drawable(name,PASS_CLASS_NAME_TREE_ARG)
{

}

void	DrawableCallback::DoPreDraw(TravState* travstate)
{
	KigsCore::GetNotificationCenter()->postNotificationName("PreDraw",this,travstate);
}

void	DrawableCallback::DoDraw(TravState* travstate)
{
	KigsCore::GetNotificationCenter()->postNotificationName("Draw",this,travstate);
}

void	DrawableCallback::DoPostDraw(TravState* travstate)
{
	KigsCore::GetNotificationCenter()->postNotificationName("PostDraw",this,travstate);
}

