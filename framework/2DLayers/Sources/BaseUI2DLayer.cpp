#include "BaseUI2DLayer.h"


#include "Timer.h"

#include "ModuleInput.h"
#include "MouseDevice.h"
#include "LayerMouseInfo.h"
#include "MultiTouchDevice.h"
#include "MultiTouchPinch.h"
#include "MouseVelocityComputer.h"

#include "NotificationCenter.h"
#include "RenderingScreen.h"
#include "ModuleRenderer.h"
#include "Module2DLayers.h"

#include "UI/UIItem.h"

#include "TouchInputEventManager.h"

#include <algorithm>

#include "Platform/2DLayers/BaseUI2DLayer.inl.h"

using namespace Kigs::Draw2D;
using namespace Kigs::Input;

IMPLEMENT_CLASS_INFO(BaseUI2DLayer);

///////////////////////////////////////////
//
//			BaseUI2DLayer
//
///////////////////////////////////////////
IMPLEMENT_CONSTRUCTOR(BaseUI2DLayer)
, mInput(0)
, mRootItem(0)
{

}

void BaseUI2DLayer::InitModifiable()
{
	Abstract2DLayer::InitModifiable();
	if (IsInit())
	{
		std::vector<CMSP>	instances;
		GetSonInstancesByType("UIItem", instances);
		KIGS_ASSERT(instances.size() < 2); // only one UIItem child
		if (instances.empty())
		{
			// add the root UIItem
			mRootItem = KigsCore::GetInstanceOf(getName(), "UIItem");
			// set the root size to the screen size
			float X, Y;
			GetRenderingScreen()->GetDesignSize(X, Y);
			mRootItem->setValue("Size", v2f(X,Y));
			addItem(mRootItem);
			mRootItem->Init();
		}
		else
		{
			mRootItem = instances[0];
		}
		instances.clear();

		instances = GetInstances("ModuleInput");
	//	KIGS_ASSERT(instances.size() == 1);
		if(instances.size()==1)
			mInput = (Input::ModuleInput*)(instances[0].get());

		ModuleSpecificRenderer* renderer = (ModuleSpecificRenderer*)((ModuleRenderer*)KigsCore::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
		if (renderer->getDefaultUiShader())
		{
			addItem(renderer->getDefaultUiShader());
		}
	}
	else
	{
		UninitModifiable();
	}
}

void BaseUI2DLayer::Update(const Timer& a_Timer, void* addParam)
{
	if (!IsInit())
	{
		Init();
	}
	if ((!IsRenderable()) || (!IsInit()))
	{
		return;
	}
	// if not interactive, don't do update (but still draw)
	if (!mIsInteractive)
	{
		return;
	}

	UpdateChildrens(a_Timer, mRootItem.get(), addParam);

}

void BaseUI2DLayer::UpdateChildrens(const Timer& a_timer, UIItem* current, void* addParam)
{
	current->CallUpdate(a_timer, addParam);

	// recursif Call
	const std::set<Node2D*, Node2D::PriorityCompare>& sons = current->GetSons();
	std::set<Node2D*, Node2D::PriorityCompare>::const_reverse_iterator it = sons.rbegin();
	std::set<Node2D*, Node2D::PriorityCompare>::const_reverse_iterator end = sons.rend();
	for (; it != end; ++it)
	{
		UpdateChildrens(a_timer, (UIItem*)(*it), addParam);
	}
}


///////////////////////////////////////////

bool NodeToDraw::Sorter::operator()(NodeToDraw& a, NodeToDraw& b) const
{
	auto orderA = std::make_tuple(a.prio, a.depth, a.parent, a.node);
	auto orderB = std::make_tuple(b.prio, b.depth, b.parent, b.node);
	return orderA < orderB;
}


void BaseUI2DLayer::SortItemsFrontToBack(SortItemsFrontToBackParam& param)
{
	std::vector<NodeToDraw> nodes; nodes.reserve(param.toSort.size());

	for (auto cm : param.toSort)
	{
		Node2D* node = aggregate_cast<Node2D>(cm);
		if (node)
		{
			int depth = 0;
			auto father = node->getFather();
			while (father)
			{
				++depth;
				father = father->getFather();
			}
			nodes.push_back(NodeToDraw{ node, node->GetFinalPriority(), node->getFather(), depth, 0, cm });
		}
		else
		{
			nodes.push_back(NodeToDraw{ 0, 0, cm, 0, 0, cm });
		}
		
	}
	std::sort(nodes.begin(), nodes.end(), NodeToDraw::Sorter{});

	param.sorted.resize(param.toSort.size());
	for (u32 i = 0; i < nodes.size(); ++i)
	{
		param.sorted[i] = std::make_tuple(nodes[nodes.size() - 1 - i].root, Maths::Hit{});
	}
}


void BaseUI2DLayer::AccumulateToDraw(TravState* state, std::vector<NodeToDraw>& todraw, CoreModifiable* current, int depth, u32 clip_count)
{
	if (current->isSubType(Node2D::mClassID))
	{
		if (static_cast<Node2D*>(current)->GetNodeFlag(Node2D::Node2D_ClipSons))
		{
			++clip_count;
		}
	}
	
	for (auto& item_struct : current->getItems())
	{
		bool continue_down = true;
		auto item = item_struct.mItem;

		if (item->isSubType(Node2D::mClassID))
		{
			auto node = static_cast<Node2D*>(item.get());
			if (node->Draw(state))
			{
				node->SetUpNodeIfNeeded();
				todraw.push_back(NodeToDraw{ node, node->GetFinalPriority(), current, depth, clip_count });
			}
			else
			{
				continue_down = false;
			}
		}
		
		if(continue_down)
			AccumulateToDraw(state, todraw, item.get(), depth+1, clip_count);
	}
}

//! Do drawing here if any
void BaseUI2DLayer::TravDraw(TravState* state)
{
	if ((!IsInit()))
	{
		return;
	}

	// first thing to do (activate rendering screen)
	if (!StartDrawing(state))
		return;

	auto holo_before = state->GetHolographicMode();
	state->SetHolographicMode(GetRenderingScreen()->IsHolographic());
	state->mHolographicUseStackMatrix = true;


	// call predraw (activate the shader)!
	PreDrawDrawable(state);

	ModuleSpecificRenderer* renderer = (ModuleSpecificRenderer*)state->GetRenderer();

	int lShaderMask = ModuleRenderer::VERTEX_ARRAY_MASK | ModuleRenderer::TEXCOORD_ARRAY_MASK | ModuleRenderer::NO_LIGHT_MASK;

	// create shader if none
	renderer->GetActiveShader()->ChooseShader(state, lShaderMask);
	renderer->ActiveTextureChannel(0);

	
	renderer->PushState();
	renderer->SetCullMode(RENDERER_CULL_NONE);
	renderer->SetLightMode(RENDERER_LIGHT_OFF);


	state->mCurrentMaterial = 0;

	state->SetAllVisible(false);

	renderer->SetDepthMaskMode(RENDERER_DEPTH_MASK_OFF);
	renderer->SetAlphaTestMode(RENDERER_ALPHA_TEST_ON);
	renderer->SetAlphaMode(RENDERER_ALPHA_GREATER, 0.01f);
	renderer->SetBlendMode(RENDERER_BLEND_ON);
	renderer->SetBlendFuncMode(RENDERER_BLEND_SRC_ALPHA, RENDERER_BLEND_ONE_MINUS_SRC_ALPHA);
	renderer->SetDepthTestMode(false);

	// get rendering screen size
	float rendersx, rendersy;
	GetRenderingScreen()->GetSize(rendersx, rendersy);
	float drendersx, drendersy;
	GetRenderingScreen()->GetDesignSize(drendersx, drendersy);

	// do drawing here
	if (Module2DLayers::getRotate180())
	{
		renderer->Ortho(MATRIX_MODE_PROJECTION,(float)drendersx, 0.0f, 0.0f, (float)drendersy, -1.0f, 1.0f);
	}
	else
	{
		renderer->Ortho(MATRIX_MODE_PROJECTION,0.0f, (float)drendersx, (float)drendersy, 0.0f, -1.0f, 1.0f);
	}

	renderer->Viewport(0, 0, (int)rendersx, (int)rendersy);
	renderer->SetScissorTestMode(RENDERER_SCISSOR_TEST_ON);
	renderer->SetScissorValue(0, 0, (int)rendersx, (int)rendersy);
	
	renderer->LoadIdentity(MATRIX_MODE_MODEL);
	renderer->LoadIdentity(MATRIX_MODE_VIEW);
	renderer->LoadIdentity(MATRIX_MODE_UV);

	//renderer->FlushMatrix();
	//renderer->FlushState();


	if (mClearColorBuffer)
	{
		renderer->SetClearColorValue(mClearColor[0], mClearColor[1], mClearColor[2], mClearColor[3]);
		renderer->ClearView(RENDERER_CLEAR_COLOR);
	}

	if (IsRenderable())
	{

		std::vector<NodeToDraw> todraw;
		todraw.push_back(NodeToDraw{ mRootItem.get(), 0 });

		if (mRootItem->Draw(state))
		{
			mRootItem->SetUpNodeIfNeeded();
			AccumulateToDraw(state, todraw, mRootItem.get());
		}

		std::sort(todraw.begin(), todraw.end(), NodeToDraw::Sorter{});
		renderer->SetStencilTest(true);

		std::vector<Node2D*> current_stencil_stack;
		std::vector<Node2D*> cache_stencil_stack;

		auto compare_stencil_stacks = [&]()
		{
			if (current_stencil_stack.size() != cache_stencil_stack.size())
				return false;

			for (u32 i = 0; i < current_stencil_stack.size(); ++i)
			{
				if (current_stencil_stack[i] != cache_stencil_stack[i])
					return false;
			}

			return true;
		};

		ShaderBase* current_custom_shader = nullptr;
		for (auto item : todraw)
		{
			if (item.clip_count > 0)
			{
				current_stencil_stack.clear();
				Node2D* father = item.node->getFather();

				while (father && current_stencil_stack.size() < item.clip_count)
				{
					if (father->GetNodeFlag(Node2D::Node2D_ClipSons))
					{
						current_stencil_stack.push_back(father);
					}
					father = father->getFather();
				}

					

				if (compare_stencil_stacks())
				{
					renderer->SetStencilMode(RendererCullMode::RENDERER_CULL_FRONT_AND_BACK, RENDERER_STENCIL_EQUAL, (int)cache_stencil_stack.size(), 0xFFFFFFFF);
				}
				else
				{
					renderer->SetColorMask(false, false, false, false);
					renderer->ClearView(RendererClearMode::RENDERER_CLEAR_STENCIL);
					renderer->SetStencilOp(RendererCullMode::RENDERER_CULL_FRONT_AND_BACK, RENDERER_STENCIL_OP_ZERO, RENDERER_STENCIL_OP_ZERO, RENDERER_STENCIL_OP_INCR);

					if (current_custom_shader)
					{
						current_custom_shader->DoPostDraw(state);
						//renderer->popShader(current_custom_shader, state);
					}

					int current_clip = 0;
					for (auto node : current_stencil_stack)
					{
						renderer->SetStencilMode(RendererCullMode::RENDERER_CULL_FRONT_AND_BACK, RENDERER_STENCIL_EQUAL, current_clip, 0xFFFFFFFF);
						float op = 0.0f;
						node->getValue("Opacity", op);
						node->setValue("Opacity", 1);
						node->ProtectedDraw(state);
						node->setValue("Opacity", op);
						++current_clip;
					}
					renderer->SetColorMask(true, true, true, true);
					renderer->SetStencilMode(RendererCullMode::RENDERER_CULL_FRONT_AND_BACK, RENDERER_STENCIL_EQUAL, current_clip, 0xFFFFFFFF);
					std::swap(current_stencil_stack, cache_stencil_stack);

					if (current_custom_shader)
					{
						current_custom_shader->DoPreDraw(state);
						//renderer->pushShader(current_custom_shader, state);
					}
				}
			}
			else
				renderer->SetStencilMode(RendererCullMode::RENDERER_CULL_FRONT_AND_BACK, RENDERER_STENCIL_ALWAYS, 0, 0xFFFFFFFF);

			renderer->SetStencilOp(RendererCullMode::RENDERER_CULL_FRONT_AND_BACK, RENDERER_STENCIL_OP_KEEP, RENDERER_STENCIL_OP_KEEP, RENDERER_STENCIL_OP_KEEP);

			ShaderBase* shader = nullptr;
			if (item.node->GetNodeFlag(Node2D::Node2D_UseCustomShader))
			{
				shader = item.node->getValue<CoreModifiable*>("CustomShader")->as<ShaderBase>();
			}
			if (shader != current_custom_shader)
			{
				if (current_custom_shader)
				{
					current_custom_shader->DoPostDraw(state);
					//renderer->popShader(current_custom_shader, state);
				}

				if (shader)
				{
					shader->DoPreDraw(state);
					//renderer->pushShader(shader, state);
				}

				current_custom_shader = shader;
			}
			item.node->ProtectedDraw(state);
		}
		if (current_custom_shader)
		{
			current_custom_shader->DoPostDraw(state);
			//renderer->popShader(current_custom_shader, state);
		}
		renderer->SetStencilTest(false);


	}
	state->mHolographicUseStackMatrix = false;
	state->SetHolographicMode(holo_before);
	renderer->PopState();
	

	// call postdraw (deactivate the shader)
	PostDrawDrawable(state);

	// last thing to do
	EndDrawing(state);

	//renderer->PopUIShader();

}

