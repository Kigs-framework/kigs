#include "UI/UINode3DLayer.h"
#include "CoreBaseApplication.h"
#include "BaseUI2DLayer.h"
#include "ModuleRenderer.h"

IMPLEMENT_CLASS_INFO(UINode3DLayer);

///////////////////////////////////////////
//
//			UINode3DLayer
//
///////////////////////////////////////////
IMPLEMENT_CONSTRUCTOR(UINode3DLayer)
, myRootItem(0)
{

}

void UINode3DLayer::InitModifiable()
{
	Node3D::InitModifiable();
	if (IsInit())
	{
		kstl::vector<CMSP>	instances;
		GetSonInstancesByType("UIItem", instances);
		KIGS_ASSERT(instances.size() < 2); // only one UIItem child
		if (instances.empty())
		{
			// add the root UIItem
			myRootItem = KigsCore::GetInstanceOf(getName(), "UIItem");

			myRootItem->setValue("SizeX", 1.0f);
			myRootItem->setValue("SizeY", 1.0f);
			addItem(myRootItem);
			myRootItem->Init();
		}
		else
		{
			myRootItem = instances[0];
		}
		instances.clear();

		CMSP shader=KigsCore::GetInstanceOf(getName()+"UIShader","API3DUINode3DShader");

		if (shader)
		{
			addItem(shader);
		}

		// set 3D object scale
		my3DAndDesignSizeUniform = KigsCore::GetInstanceOf(getName() + "3DSizeUniform", "API3DUniformFloat4");
		my3DAndDesignSizeUniform->setValue("Name", "SceneScaleAndDesignSize");
		my3DAndDesignSizeUniform->setArrayValue("Value", my3DSize[0] / myDesignSize[0] , my3DSize[1] / myDesignSize[1], myDesignSize[0], myDesignSize[1]);
		my3DAndDesignSizeUniform->Init();

		shader->addItem(my3DAndDesignSizeUniform);

		my3DSize.changeNotificationLevel(Owner);
		myDesignSize.changeNotificationLevel(Owner);

		CoreBaseApplication* L_currentApp = KigsCore::GetCoreApplication();
		if (L_currentApp)
		{
			L_currentApp->AddAutoUpdate(this);
		}
	}
	else
	{
		UninitModifiable();
	}
}

void UINode3DLayer::NotifyUpdate(const unsigned int labelid)
{
	ParentClassType::NotifyUpdate(labelid);
	if ((labelid == my3DSize.getLabelID()) || (labelid == myDesignSize.getLabelID()))
	{
		SetFlag(BoundingBoxIsDirty | GlobalBoundingBoxIsDirty);
		my3DAndDesignSizeUniform->setArrayValue("Value", my3DSize[0] / myDesignSize[0], my3DSize[1] / myDesignSize[1], myDesignSize[0], myDesignSize[1]);
		PropagateDirtyFlagsToSons(this);
		PropagateDirtyFlagsToParents(this);
	}
}

void UINode3DLayer::Update(const Timer& a_Timer, void* addParam)
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
	if (!myIsInteractive)
	{
		return;
	}

	UpdateChildrens(a_Timer, myRootItem.get(), addParam);

}

void UINode3DLayer::UpdateChildrens(const Timer& a_timer, UIItem* current, void* addParam)
{
	current->CallUpdate(a_timer, addParam);

	// recursif Call
	const kstl::set<Node2D*, Node2D::PriorityCompare>& sons = current->GetSons();
	kstl::set<Node2D*, Node2D::PriorityCompare>::const_reverse_iterator it = sons.rbegin();
	kstl::set<Node2D*, Node2D::PriorityCompare>::const_reverse_iterator end = sons.rend();
	for (; it != end; ++it)
	{
		UpdateChildrens(a_timer, (UIItem*)(*it), addParam);
	}
}

void UINode3DLayer::SortItemsFrontToBack(SortItemsFrontToBackParam& param)
{
	kstl::vector<NodeToDraw> nodes; nodes.reserve(param.toSort.size());

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
		param.sorted[i] = std::make_tuple(nodes[nodes.size() - 1 - i].root, Hit{});
	}
}

void UINode3DLayer::RecomputeBoundingBox()
{
	// classic Node3D BBox init
	ParentClassType::RecomputeBoundingBox();

	// approximate BBox with quad size
	float maxSize = std::max(my3DSize[0], my3DSize[1])*0.5f;

	BBox	uiBBox;
	uiBBox.m_Min.Set(-maxSize, -maxSize, -maxSize);
	uiBBox.m_Max.Set(maxSize, maxSize, maxSize);

	myLocalBBox.Update(uiBBox);
}



//! Do drawing here if any
void UINode3DLayer::TravDraw(TravState* state)
{
	if (!IsInit())
	{
		//kigsprintf("%p %s %s %s\n",this, getName().c_str(), (myShowNode == true) ? "true" : "false", (IsInit()) ? "true" : "false");
		return;
	}

	// call predraw (activate the shader)!
	PreDrawDrawable(state);

	ModuleSpecificRenderer* renderer = (ModuleSpecificRenderer*)state->GetRenderer();

	int lShaderMask = ModuleRenderer::VERTEX_ARRAY_MASK | ModuleRenderer::TEXCOORD_ARRAY_MASK | ModuleRenderer::NO_LIGHT_MASK;
	//if (myColors)lShaderMask |= ModuleRenderer::COLOR_ARRAY_MASK;

	// create shader if none
	renderer->GetActiveShader()->ChooseShader(state, lShaderMask);
	renderer->ActiveTextureChannel(0);


	renderer->PushState();
	renderer->SetCullMode(RENDERER_CULL_NONE);
	renderer->SetLightMode(RENDERER_LIGHT_OFF);


	state->myCurrentMaterial = 0;

	state->SetAllVisible(false);

	renderer->SetDepthMaskMode(RENDERER_DEPTH_MASK_OFF);
	renderer->SetAlphaTestMode(RENDERER_ALPHA_TEST_ON);
	renderer->SetAlphaMode(RENDERER_ALPHA_GREATER, 0.01f);
	renderer->SetBlendMode(RENDERER_BLEND_ON);
	renderer->SetBlendFuncMode(RENDERER_BLEND_SRC_ALPHA, RENDERER_BLEND_ONE_MINUS_SRC_ALPHA);
	renderer->SetDepthTestMode(false);

	if (IsRenderable())
	{

		kstl::vector<NodeToDraw> todraw;
		todraw.push_back(NodeToDraw{ myRootItem.get(), 0 });

		if (myRootItem->Draw(state))
		{
			myRootItem->SetUpNodeIfNeeded();
			BaseUI2DLayer::AccumulateToDraw(state, todraw, myRootItem.get());
		}

		std::sort(todraw.begin(), todraw.end(), NodeToDraw::Sorter{});
		renderer->SetStencilTest(true);

		kstl::vector<Node2D*> current_stencil_stack;
		kstl::vector<Node2D*> cache_stencil_stack;

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
					bool clip;
					if (father->getValue("ClipSons", clip) && clip)
					{
						current_stencil_stack.push_back(father);
					}
					father = father->getFather();
				}



				if (compare_stencil_stacks())
				{
					renderer->SetStencilMode(RendererCullMode::RENDERER_CULL_FRONT_AND_BACK, RENDERER_STENCIL_EQUAL, cache_stencil_stack.size(), 0xFFFFFFFF);
				}
				else
				{
					renderer->SetColorMask(false, false, false, false);
					renderer->ClearView(RendererClearMode::RENDERER_CLEAR_STENCIL);
					renderer->SetStencilOp(RendererCullMode::RENDERER_CULL_FRONT_AND_BACK, RENDERER_STENCIL_OP_ZERO, RENDERER_STENCIL_OP_ZERO, RENDERER_STENCIL_OP_INCR);

					if (current_custom_shader)
					{
						renderer->popShader(current_custom_shader, state);
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
						renderer->pushShader(current_custom_shader, state);
					}
				}
			}
			else
				renderer->SetStencilMode(RendererCullMode::RENDERER_CULL_FRONT_AND_BACK, RENDERER_STENCIL_ALWAYS, 0, 0xFFFFFFFF);

			renderer->SetStencilOp(RendererCullMode::RENDERER_CULL_FRONT_AND_BACK, RENDERER_STENCIL_OP_KEEP, RENDERER_STENCIL_OP_KEEP, RENDERER_STENCIL_OP_KEEP);

			auto shader = item.node->getValue<CoreModifiable*>("CustomShader")->as<ShaderBase>();
			if (shader != current_custom_shader)
			{
				if (current_custom_shader)
					renderer->popShader(current_custom_shader, state);

				if (shader)
					renderer->pushShader(shader, state);

				current_custom_shader = shader;
			}

			item.node->ProtectedDraw(state);
		}
		if (current_custom_shader)
			renderer->popShader(current_custom_shader, state);
		renderer->SetStencilTest(false);


	}

	renderer->PopState();

	// call postdraw (deactivate the shader)
	PostDrawDrawable(state);

}

