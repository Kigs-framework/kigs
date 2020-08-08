#include "UI/UINode3DLayer.h"
#include "CoreBaseApplication.h"
#include "BaseUI2DLayer.h"
#include "ModuleRenderer.h"

#include "TouchInputEventManager.h"


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

// behavior inspired by BaseUI2DLayer
void UINode3DLayer::InitModifiable()
{
	Node3D::InitModifiable();
	if (IsInit())
	{
		kstl::vector<CMSP>	instances;
		GetSonInstancesByType("UIItem", instances);
		KIGS_ASSERT(instances.size() < 2); // only one UIItem child
		// Should we just uninit and wait for a root item before real init ?
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

		// specific shader for UINode3DLayer
		CMSP shader=KigsCore::GetInstanceOf(getName()+"UIShader","API3DUINode3DShader");
		if (shader)
		{
			addItem(shader);
		}

		// set 3D object size and design size in a single uniform
		my3DAndDesignSizeUniform = KigsCore::GetInstanceOf(getName() + "3DSizeUniform", "API3DUniformFloat4");
		my3DAndDesignSizeUniform->setValue("Name", "SceneScaleAndDesignSize");
		my3DAndDesignSizeUniform->setArrayValue("Value", my3DSize[0] / myDesignSize[0] , my3DSize[1] / myDesignSize[1], myDesignSize[0], myDesignSize[1]);
		my3DAndDesignSizeUniform->Init();

		// and add the uniform directly to the shader
		shader->addItem(my3DAndDesignSizeUniform);

		// notify me when 3D size or design size change
		// ( recompute BBox and update uniform)
		my3DSize.changeNotificationLevel(Owner);
		myDesignSize.changeNotificationLevel(Owner);

		auto input = KigsCore::GetModule<ModuleInput>();
		input->getTouchManager()->addTouchSupport(this, myCamera);

		std::vector<CMSP> colliders;
		GetSonInstancesByType("CollisionBaseNode", colliders);
		if (colliders.empty())
		{
			myCollider = KigsCore::GetInstanceOf(getName()+"_panel", "Panel");
			myCollider->setValue("Size", (v2f)my3DSize);
			myCollider->Init();
			addItem(myCollider);
		}

		// add myself to auto update
		// unlike BaseUI2DLayer, the scenegraph will not call Update on me
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

// update BBox and Uniform when parameters are touched
void UINode3DLayer::NotifyUpdate(const unsigned int labelid)
{
	ParentClassType::NotifyUpdate(labelid);
	if ((labelid == my3DSize.getLabelID()) || (labelid == myDesignSize.getLabelID()))
	{
		SetFlag(BoundingBoxIsDirty | GlobalBoundingBoxIsDirty);
		my3DAndDesignSizeUniform->setArrayValue("Value", my3DSize[0] / myDesignSize[0], my3DSize[1] / myDesignSize[1], myDesignSize[0], myDesignSize[1]);
		if(myCollider) myCollider->setValue("Size", (v2f)my3DSize);
		PropagateDirtyFlagsToSons(this);
		PropagateDirtyFlagsToParents(this);
	}
}

// Update childrens
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

	BaseUI2DLayer::UpdateChildrens(a_Timer, myRootItem.get(), addParam);

}

// not sure how to connect this with TouchManager
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


bool UINode3DLayer::GetDataInTouchSupport(const touchPosInfos& posin, touchPosInfos& pout)
{
	if (!(IsRenderable() && IsCollidable()))
	{
		pout.pos.x = -1;
		pout.pos.y = -1;
		return false;
	}
	double dist = DBL_MAX;
	
	Point3D		planePos = v3f(0,0,0);
	Vector3D	planeNorm = v3f(0,0,1);

	Vector3D pos = posin.origin;
	Vector3D dir = posin.dir;

	//! transform Ray in local mesh coordinate system if needed				
	const Matrix3x4& inverseMatrix = GetGlobalToLocal();
	inverseMatrix.TransformPoint(&pos);
	inverseMatrix.TransformVector(&dir);

	if (Intersection::IntersectRayPlane(pos, dir, planePos, planeNorm, dist))
	{
		auto hit_pos = pos + ((float)dist * dir);

		Vector3D up(0,1,0);
		Vector3D left = up ^ Vector3D(0,0,1);

		pout.pos.xy = v2f((Dot(left, hit_pos) / my3DSize[0]) + 0.5f, (Dot(up, hit_pos) / my3DSize[1]) + 0.5f);
		
		pout.pos.x *= myDesignSize[0];
		pout.pos.y *= myDesignSize[1];


		bool is_in = true;

		if (pout.pos.x > 1.0f ||
			pout.pos.y > 1.0f ||
			pout.pos.x < 0.0f ||
			pout.pos.y < 0.0f)
			is_in = false;


		auto& l2g = GetLocalToGlobal();
		pout.hit.HitPosition = pos + dir * dist;
		pout.hit.HitNormal = planeNorm;
		l2g.TransformPoint(&pout.hit.HitPosition);
		l2g.TransformVector(&pout.hit.HitNormal);
		pout.hit.HitNode = this;
		pout.hit.HitActor = myCollider.get();
		//pout.hit.HitCollisionObject = (CollisionBaseObject*)myCollider.get();

		return is_in;
	}
	return false;
}


// BBox is computed using 2D quad size
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



// Do drawing here if any
// behavior inspired by BaseUI2DLayer
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

