#include "StencilMask.h"
#include "TravState.h"
#include "ModuleRenderer.h"

using namespace Kigs::Draw;

IMPLEMENT_CLASS_INFO(StencilMask);

bool StencilMask::PreDraw(TravState* state)
{
	if (!ParentClassType::PreDraw(state)) return false;

	auto renderer = state->GetRenderer();
	state->GetRenderer()->PushState();
	
	renderer->SetStencilTest(true);
	switch ((int)MaskUsage)
	{
	case 0: // Build
		if (MaskMode == 0)
		{
			renderer->SetStencilMode(RendererCullMode::RENDERER_CULL_FRONT_AND_BACK, RendererStencilMode::RENDERER_STENCIL_ALWAYS, (int)ReferenceValue, (u32)Mask);
			renderer->SetStencilOp(RendererCullMode::RENDERER_CULL_FRONT_AND_BACK, RendererStencilOp::RENDERER_STENCIL_OP_KEEP, 
				IgnoreDepthTest ? RendererStencilOp::RENDERER_STENCIL_OP_INCR_WRAP : RendererStencilOp::RENDERER_STENCIL_OP_KEEP,
				RendererStencilOp::RENDERER_STENCIL_OP_INCR_WRAP);
		}
		else if (MaskMode == 1)
		{
			renderer->SetStencilMode(RendererCullMode::RENDERER_CULL_FRONT_AND_BACK, RendererStencilMode::RENDERER_STENCIL_ALWAYS, (int)ReferenceValue, (u32)Mask);
			renderer->SetStencilOp(RendererCullMode::RENDERER_CULL_FRONT_AND_BACK, RendererStencilOp::RENDERER_STENCIL_OP_KEEP, 
				IgnoreDepthTest ? RendererStencilOp::RENDERER_STENCIL_OP_REPLACE : RendererStencilOp::RENDERER_STENCIL_OP_KEEP,
				RendererStencilOp::RENDERER_STENCIL_OP_REPLACE);
		}
		break;
	case 1: // Use
		renderer->SetStencilMode(RendererCullMode::RENDERER_CULL_FRONT_AND_BACK, RendererStencilMode::RENDERER_STENCIL_EQUAL, (int)ReferenceValue, (u32)Mask);
		renderer->SetStencilOp(RendererCullMode::RENDERER_CULL_FRONT_AND_BACK, RendererStencilOp::RENDERER_STENCIL_OP_KEEP, RendererStencilOp::RENDERER_STENCIL_OP_KEEP, RendererStencilOp::RENDERER_STENCIL_OP_KEEP);
		break;
	case 2: // Ignore
		renderer->SetStencilMode(RendererCullMode::RENDERER_CULL_FRONT_AND_BACK, RendererStencilMode::RENDERER_STENCIL_ALWAYS, 0, 0xFFFFFFFF);
		renderer->SetStencilOp(RendererCullMode::RENDERER_CULL_FRONT_AND_BACK, RendererStencilOp::RENDERER_STENCIL_OP_KEEP, RendererStencilOp::RENDERER_STENCIL_OP_KEEP, RendererStencilOp::RENDERER_STENCIL_OP_KEEP);
		break;
	}
	return true;
}

bool StencilMask::PostDraw(TravState* state)
{
	if (!ParentClassType::PostDraw(state)) return false;

	auto renderer = state->GetRenderer();
	//renderer->SetStencilTest(false);
	state->GetRenderer()->PopState();
	return true;
}