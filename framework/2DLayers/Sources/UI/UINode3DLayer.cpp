#include "UI/UINode3DLayer.h"
#include "CoreBaseApplication.h"
#include "BaseUI2DLayer.h"
#include "ModuleRenderer.h"
#include "ModuleInput.h"

#include "TouchInputEventManager.h"

using namespace Kigs::Draw2D;

IMPLEMENT_CLASS_INFO(UINode3DLayer);

///////////////////////////////////////////
//
//			UINode3DLayer
//
///////////////////////////////////////////
IMPLEMENT_CONSTRUCTOR(UINode3DLayer)
, mRootItem(0)
{
	setValue("Camera","Camera:camera");
}

// behavior inspired by BaseUI2DLayer
void UINode3DLayer::InitModifiable()
{
	Node3D::InitModifiable();
	if (IsInit())
	{
		std::vector<CMSP>	instances;
		GetSonInstancesByType("UIItem", instances);
		KIGS_ASSERT(instances.size() < 2); // only one UIItem child
		// Should we just uninit and wait for a root item before real init ?
		if (instances.empty())
		{
			// add the root UIItem
			mRootItem = KigsCore::GetInstanceOf(getName(), "UIItem");

			mRootItem->setValue("Size", v2f(1.0f,1.0f));
			addItem(mRootItem);
			mRootItem->Init();
		}
		else
		{
			mRootItem = instances[0];
		}
		instances.clear();

		ModuleSpecificRenderer* renderer = (ModuleSpecificRenderer*)((ModuleRenderer*)KigsCore::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
		if (renderer->getDefaultUiShader())
		{
			addItem(renderer->getDefaultUiShader());
		}

		// ( recompute BBox and update collider size)
		setOwnerNotification("Size", true);
		setOwnerNotification("DesignSize", true);

		auto input = KigsCore::GetModule<Input::ModuleInput>();

		CMSP	currentCamera;
		getValue("Camera", currentCamera);

		input->getTouchManager()->addTouchSupport(this, currentCamera.get());

		std::vector<CMSP> colliders;
		GetSonInstancesByType("CollisionBaseNode", colliders);
		if (colliders.empty())
		{
			mCollider = KigsCore::GetInstanceOf(getName() + "_panel", "Panel");
			mCollider->setValue("Size", (v2f)mSize);
			mCollider->Init();
			addItem(mCollider);
		}
		else if (colliders.front()->isSubType("Panel")) 
		{
			mCollider = colliders.front();
			mCollider->setValue("Size", (v2f)mSize);
		}

		// add mself to auto update
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
	if ((labelid == KigsID("Size")._id) || (labelid == KigsID("DesignSize")._id))
	{
		setUserFlag(BoundingBoxIsDirty | GlobalBoundingBoxIsDirty);
		if(mCollider) mCollider->setValue("Size", (v2f)mSize);
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
	if (!mIsInteractive)
	{
		return;
	}

	BaseUI2DLayer::UpdateChildrens(a_Timer, mRootItem.get(), addParam);

}

// not sure how to connect this with TouchManager
void UINode3DLayer::SortItemsFrontToBack(Input::SortItemsFrontToBackParam& param)
{
	std::vector<NodeToDraw> nodes; nodes.reserve(param.toSort.size());
	int aze = 0;
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


bool UINode3DLayer::GetDataInTouchSupport(const Input::touchPosInfos& posin, Input::touchPosInfos& pout)
{
	if (!(IsRenderable() && IsCollidable()))
	{
		pout.pos.x = -1;
		pout.pos.y = -1;
		return false;
	}
	double dist = DBL_MAX;
	
	v3f		planePos = v3f(0,0,0);
	v3f	planeNorm = v3f(0,0,1);

	v3f pos = posin.origin;
	v3f dir = posin.dir;

	//! transform Ray in local mesh coordinate system if needed				
	const mat4& inverseMatrix = GetGlobalToLocal();
	transformPoint(inverseMatrix ,pos);
	transformVector(inverseMatrix ,dir);

	if (Maths::IntersectRayPlane(pos, dir, planePos, planeNorm, dist))
	{
		auto hit_pos = pos + ((float)dist * dir);

		v3f up(0,1,0);
		v3f left = cross(up , v3f(0,0,1));

		pout.pos = v3f((dot(left, hit_pos) / mSize[0]) + 0.5f, (dot(up, hit_pos) / mSize[1]) + 0.5f, pout.pos.z);
		
		bool is_in = true;

		if (pout.pos.x > 1.0f ||
			pout.pos.y > 1.0f ||
			pout.pos.x < 0.0f ||
			pout.pos.y < 0.0f)
			is_in = false;

		pout.pos.x *= mDesignSize[0];
		pout.pos.y *= mDesignSize[1];

		auto& l2g = GetLocalToGlobal();
		pout.hit.HitPosition = pos + dir * dist;
		pout.hit.HitNormal = planeNorm;
		transformPoint(l2g ,pout.hit.HitPosition);
		transformVector(l2g ,pout.hit.HitNormal);
		pout.hit.HitNode = this;
		pout.hit.HitActor = mCollider.get();
		//pout.hit.HitCollisionObject = (CollisionBaseObject*)mCollider.get();

		return is_in;
	}
	return false;
}

void UINode3DLayer::GetDistanceForInputSort(Input::GetDistanceForInputSortParam& params)
{
	params.inout_sorting_layer = mInputSortingLayer;
}

// BBox is computed using 2D quad size
void UINode3DLayer::RecomputeBoundingBox()
{
	// classic Node3D BBox init
	ParentClassType::RecomputeBoundingBox();

	// approximate BBox with quad size
	float maxSize = std::max(mSize[0], mSize[1])*0.5f;

	BBox	uiBBox;
	uiBBox.m_Min = v3f(-maxSize, -maxSize, -maxSize);
	uiBBox.m_Max = v3f(maxSize, maxSize, maxSize);

	mLocalBBox.Update(uiBBox);
}



// Do drawing here if any
// behavior inspired by BaseUI2DLayer
void UINode3DLayer::TravDraw(TravState* state)
{
	if (!IsInit())
	{
		//kigsprintf("%p %s %s %s\n",this, getName().c_str(), (mShowNode == true) ? "true" : "false", (IsInit()) ? "true" : "false");
		return;
	}

	if (state->mCurrentPass && !IsUsedInRenderPass(state->mCurrentPass->pass_mask))
		return;

	// call predraw (activate the shader)!
	PreDrawDrawable(state);

	ModuleSpecificRenderer* renderer = (ModuleSpecificRenderer*)state->GetRenderer();

	int lShaderMask = ModuleRenderer::VERTEX_ARRAY_MASK | ModuleRenderer::TEXCOORD_ARRAY_MASK | ModuleRenderer::NO_LIGHT_MASK;
	//if (mColors)lShaderMask |= ModuleRenderer::COLOR_ARRAY_MASK;

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

	mat4 m(1.0f);
	m[0][0] = mSize[0] / mDesignSize[0];
	m[1][1] = mSize[1] / mDesignSize[1];
	m[2][2] = 0.0f;
	m[3][0] = -mDesignSize[0] * 0.5f * m[0][0];
	m[3][1] = -mDesignSize[1] * 0.5f * m[1][1];
	renderer->PushAndMultMatrix(MATRIX_MODE_MODEL, &m[0][0]);

	if (IsRenderable())
	{

		std::vector<NodeToDraw> todraw;
		todraw.push_back(NodeToDraw{ mRootItem.get(), 0 });

		if (mRootItem->Draw(state))
		{
			mRootItem->SetUpNodeIfNeeded();
			BaseUI2DLayer::AccumulateToDraw(state, todraw, mRootItem.get());
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

			ShaderBase* shader = nullptr;
			if (item.node->GetNodeFlag(Node2D::Node2D_UseCustomShader))
			{
				shader = item.node->getValue<CMSP>("CustomShader")->as<ShaderBase>();
			}
			if (shader != current_custom_shader)
			{
				if (current_custom_shader)
					renderer->popShader(current_custom_shader, state);

				if (shader)
					renderer->pushShader(shader, state);

				current_custom_shader = shader;
			}

			bool draw_depth = false;
			draw_depth = item.node->getValue("DrawDepth", draw_depth) && draw_depth;

			if (draw_depth)
			{
				renderer->SetDepthTestMode(true);
				renderer->SetDepthMaskMode(RENDERER_DEPTH_MASK_ON);
				renderer->SetDepthTestFunc(RENDERER_DEPTH_TEST_ALWAYS);
			}
				

			item.node->ProtectedDraw(state);

			if (draw_depth)
			{
				renderer->SetDepthTestMode(false);
				//renderer->SetDepthMaskMode(RENDERER_DEPTH_MASK_OFF);
			}
				
		}
		if (current_custom_shader)
			renderer->popShader(current_custom_shader, state);
		renderer->SetStencilTest(false);


	}

	renderer->PopMatrix(MATRIX_MODE_MODEL);
	renderer->PopState();

	// call postdraw (deactivate the shader)
	PostDrawDrawable(state);

}

