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

IMPLEMENT_CLASS_INFO(BaseUI2DLayer);

#include "Platform/2DLayers/BaseUI2DLayer.inl.h"

///////////////////////////////////////////
//
//			BaseUI2DLayer
//
///////////////////////////////////////////
IMPLEMENT_CONSTRUCTOR(BaseUI2DLayer)
, myInput(0)
, myRootItem(0)
, myFocusItem(0)
{
	myClicKept = false;
	KigsCore::GetNotificationCenter()->addObserver(this, "KeepClick", "KeepClick");
	KigsCore::GetNotificationCenter()->addObserver(this, "ReleaseClick", "ReleaseClick");
	KigsCore::GetNotificationCenter()->addObserver(this, "GetFocus", "GetFocus");
	KigsCore::GetNotificationCenter()->addObserver(this, "ReleaseFocus", "ReleaseFocus");
	KigsCore::GetNotificationCenter()->addObserver(this, "DeleteChild", "DeleteChild");
}

BaseUI2DLayer::~BaseUI2DLayer()
{
	myFocusItem = NULL;
	delete mMultiTouchPinch;
}

void BaseUI2DLayer::DeleteChild(CMSP& aChild)
{
	myWaitToDelete.push_back(aChild);

	// remove aChild and all it's children from mouseoverlist

	// first get the list of all UIItem children of aChild
	kstl::set<CoreModifiable*> instances;
	aChild->GetSonInstancesByType("UIItem", instances);
	instances.insert(aChild.get());

	kstl::set<CoreModifiable*>::const_iterator	itInstances = instances.begin();
	kstl::set<CoreModifiable*>::const_iterator	itInstancesEnd = instances.end();
	while (itInstances != itInstancesEnd)
	{
		std::vector<UIItem*>::iterator L_It = myMouseOverList.begin();
		while (L_It != myMouseOverList.end())
		{
			if ((*L_It) == (*itInstances))
			{
				myMouseOverList.erase(L_It);
				break;
			}
			++L_It;
		}
		++itInstances;
	}
}

void BaseUI2DLayer::AddChild(CMSP& aChild, UIItem * aParent)
{
	if (aParent)
		aParent->addItem(aChild);
	else
		myRootItem->addItem(aChild);
}

DEFINE_METHOD(BaseUI2DLayer, GetFocus)
{
	if (mGlobalPriority) return false;

	myFocus = true;
	//Check if another item has focus 
	kstl::set<Node2D*, Node2D::PriorityCompare> sons = myRootItem->GetSons();
	kstl::set<Node2D*, Node2D::PriorityCompare>::reverse_iterator it = sons.rbegin();
	kstl::set<Node2D*, Node2D::PriorityCompare>::reverse_iterator end = sons.rend();
	for (; it != end; ++it)
	{
		if (((UIItem*)(*it))->HasFocus())
		{
			((UIItem*)(*it))->LoseFocus();
		}
	}
	myFocusItem = (UIItem*)sender;
	return true;
}

DEFINE_METHOD(BaseUI2DLayer, ReleaseFocus)
{
	myFocus = false;
	myFocusItem = NULL;
	return true;
}

DEFINE_METHOD(BaseUI2DLayer, ReleaseClick)
{
	myClicKept = false;
	return true;
}

DEFINE_METHOD(BaseUI2DLayer, KeepClick)
{
	myClicKept = true;
	return true;
}

DEFINE_METHOD(BaseUI2DLayer, DeleteChild)
{
	CMSP toDelete = CMSP(sender, GetRefTag{});
	DeleteChild(toDelete);
	return false;
}

DEFINE_METHOD(BaseUI2DLayer, UIItemRemovedFromRootItem)
{
	//A child has been delete, Clear MouseOverList
	myMouseOverList.clear();
	return false;
}

void BaseUI2DLayer::InitModifiable()
{
	Abstract2DLayer::InitModifiable();
	if (IsInit())
	{
		if (!mGlobalPriority)
		{
			mMouseVelocityComputer = KigsCore::GetInstanceOf("velocityComputer", "MouseVelocityComputer");
			addItem((CMSP&)mMouseVelocityComputer);
			mMultiTouchPinch = new MultiTouchPinch();
		}

		KigsCore::GetNotificationCenter()->addObserver(this, "UIItemRemovedFromRootItem", "UIItemRemovedFromRootItem");

		myMouseOverList.clear();
		myMouseOverChanged = false;

		kstl::set<CoreModifiable*>	instances;
		GetSonInstancesByType("UIItem", instances);
		KIGS_ASSERT(instances.size() < 2); // only one UIItem child
		if (instances.empty())
		{
			// add the root UIItem
			myRootItem = KigsCore::GetInstanceOf(getName(), "UIItem");
			// set the root size to the screen size
			kfloat X, Y;
			GetRenderingScreen()->GetDesignSize(X, Y);
			myRootItem->setValue("SizeX", X);
			myRootItem->setValue("SizeY", Y);
			addItem((CMSP&)myRootItem);
			myRootItem->Init();
		}
		else
		{
			myRootItem = CMSP(*instances.begin(), GetRefTag{});
		}
		instances.clear();

		GetInstances("ModuleInput", instances);
	//	KIGS_ASSERT(instances.size() == 1);
		if(instances.size()==1)
			myInput = (ModuleInput*)(*instances.begin());

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
	if (!myIsInteractive)
	{
		return;
	}

	UpdateChildrens(a_Timer, myRootItem.get(), addParam);

	if (!mGlobalPriority)
	{
		// update mouse position
		kfloat X, Y, sX, sY, dRx, dRy; // X and Y are unused
		/*if (theMouseInfo)
		{
			X = 0.0f; Y = 0.0f;
			sX = theMouseInfo->position[0];
			sY = theMouseInfo->position[1];
			dRx = theMouseInfo->delta[0];
			dRy = theMouseInfo->delta[1];
		}
		else*/
			Platform_GetMousePosition(X, Y, sX, sY, dRx, dRy);

		//if (myMouseStateL) mMultiTouchPinch->set(0, sX, sY);

		if (!myClicKept)
		{
			UpdateMouseOverItem(sX, sY, dRx, dRy, myRootItem.get(), 0);
		}
		else if (!myMouseOverList.empty())
		{
			if (mMultiTouchPinch->update())
			{
				myMouseOverList[myMouseOverList.size() - 1]->TriggerPinch(mMultiTouchPinch->mDx, mMultiTouchPinch->mDy, mMultiTouchPinch->mDZ);
			}
			mMouseVelocityComputer->StoreDisplacement(dRx, dRy, a_Timer.GetTime());

		}
		else
		{
			myClicKept = false;
			UpdateMouseOverItem(sX, sY, dRx, dRy, myRootItem.get(), 0);
		}

		if ((dRx != 0.0f) || (dRy != 0.0f))
		{
			std::vector<UIItem*>::reverse_iterator it = myMouseOverList.rbegin();
			std::vector<UIItem*>::reverse_iterator end = myMouseOverList.rend();

			for (; it != end; ++it)
			{
				if ((*it)->TriggerMouseMove(true, dRx, dRy))
					break;
			}
		}

		// DEBUG PRINT
		/*{
			if (myMouseOverChanged)
			{
				//printf("new MouseOver (%d): \n",myMouseOverList.size());
				std::vector<UIItem*>::iterator IT = myMouseOverList.begin();
				std::vector<UIItem*>::iterator END = myMouseOverList.end();
				while (IT != END)
				{

					if (IT != myMouseOverList.begin())
						kigsprintf(" > ");
	#ifdef KEEP_NAME_AS_STRING
					kigsprintf("%s(%s)", (*IT)->getName().c_str(), (*IT)->getExactType().c_str());
	#else
					kigsprintf("%s(%d)", (*IT)->getName().c_str(), (*IT)->getExactType());
	#endif

					IT++;
				}
				kigsprintf("\n");
				myMouseOverChanged = false;
			}
		}*/

		// update mouse click
		int inputFlag = Platform_GetInputFlag();
		//bool left = myInput->GetMouse()->getButtonState(MouseDevice::LEFT) != 0;
		//if (myMouseStateL != left)

		// trigger event if inputFlag changed
		int inputEvent = inputFlag^myInputFlag;
		if (inputEvent)
		{
			//myMouseStateL = left;
			auto it = myMouseOverList.rbegin();
			auto end = myMouseOverList.rend();

			// loose focus if clicked outside
			bool FocusItemIsinside = false;
			for (; it != end; ++it)
			{
				if ((*it) == myFocusItem)
				{
					FocusItemIsinside = true;
					break;
				}
			}
			if (!FocusItemIsinside && myFocusItem)
			{
				myFocusItem->LoseFocus();
				myFocusItem = NULL;
				myFocus = false;
			}

			kfloat vx, vy;
			if (inputFlag&inputEvent&UIInputEvent::LEFT)
				mMouseVelocityComputer->Init(a_Timer.GetTime());
			mMouseVelocityComputer->ComputeVelocity(vx, vy);

			// MultiTouch 0 (== souris)
			/*if (inputFlag&UIInputEvent::PINCH)
				mMultiTouchPinch->set(0, sX, sY);
			else
				mMultiTouchPinch->release(0);*/

			it = myMouseOverList.rbegin();
			for (; it != end; ++it)
			{
				(*it)->TriggerMouseSwipe(inputFlag, vx, vy);
				if ((*it)->TriggerMouseClick(inputFlag, inputEvent, (int)sX, (int)sY, myClicKept))
					break;
			}
		}
		myInputFlag = inputFlag;
	}

	/*bool right = myInput->GetMouse()->getButtonState(MouseDevice::RIGHT) != 0;
	if (myMouseStateR != right)
	{
		myMouseStateR = right;
		std::vector<UIItem*>::reverse_iterator it = myMouseOverList.rbegin();
		std::vector<UIItem*>::reverse_iterator end = myMouseOverList.rend();
		for (; it != end; ++it)
		{
			if ((*it)->TriggerMouseClick(2, myMouseStateR, (int)sX, (int)sY, myClicKept))
				break;
		}
	}*/

	if (!myWaitToDelete.empty())
	{

		while (!myWaitToDelete.empty())
		{
			CMSP item = myWaitToDelete.back();
			myWaitToDelete.pop_back();

			if (item->GetParents().size() == 0)
			{
				KIGS_ERROR("item already deleted ?", 2);
			}
			item->GetParents()[0]->removeItem(item);
		}
	}
}

void BaseUI2DLayer::UpdateChildrens(const Timer& a_timer, UIItem* current, void* addParam)
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

bool BaseUI2DLayer::UpdateMouseOverItem(kfloat X, kfloat Y, kfloat dX, kfloat dY, UIItem* current, unsigned int depth)
{
	bool isOver = false;

	if (current->ContainsPoint(X, Y))
	{
		isOver = true;
		if (myMouseOverList.size() > depth)
		{
			if (myMouseOverList.at(depth) != current)
			{
				myMouseOverChanged = true;

				myMouseOverList[depth]->TriggerMouseMove(false, dX, dY); // mouse leave event
				myMouseOverList[depth] = current;

				while (myMouseOverList.size() > depth + 1)
				{
					myMouseOverList.back()->TriggerMouseMove(false, dX, dY); // mouse leave event
					myMouseOverList.pop_back();
				}
			}
		}
		else
		{
			myMouseOverChanged = true;
			myMouseOverList.push_back(current);
		}

		// recursif Call
		auto sons = current->GetSons();
		auto it = sons.rbegin();
		auto end = sons.rend();
		for (; it != end; ++it)
		{
			UIItem* item = (UIItem*)(*it);
			if (UpdateMouseOverItem(X, Y, dX, dY, item, depth + 1))
				return true;
		}
	}
	else
	{
		// remove current if mouve is not over anymore
		if (myMouseOverList.size() > depth && myMouseOverList[depth] == current)
		{
			myMouseOverChanged = true;
			myMouseOverList.back()->TriggerMouseMove(false, dX, dY);
			myMouseOverList.pop_back();
		}
	}

	return isOver;
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


static void AccumulateToDraw(TravState* state, kstl::vector<NodeToDraw>& todraw, CoreModifiable* current, int depth=0, u32 clip_count = 0)
{
	bool clipper;
	if (current->getValue("ClipSons", clipper))
	{
		if (clipper)
			++clip_count;
	}
	
	for (auto& item_struct : current->getItems())
	{
		bool continue_down = true;
		auto item = item_struct.myItem;

		if (item->isSubType(Node2D::myClassID))
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
		//kigsprintf("%p %s %s %s\n",this, getName().c_str(), (myShowNode == true) ? "true" : "false", (IsInit()) ? "true" : "false");
		return;
	}

	// first thing to do (activate rendering screen)
	if (!StartDrawing(state))
		return;

	auto holo_before = state->GetHolographicMode();
	state->SetHolographicMode(GetRenderingScreen()->IsHolographic());
	state->HolographicUseStackMatrix = true;


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

	// get rendering screen size
	kfloat rendersx, rendersy;
	GetRenderingScreen()->GetSize(rendersx, rendersy);
	kfloat drendersx, drendersy;
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
	//renderer->FlushMatrix();
	//renderer->FlushState();


	if (myClearColorBuffer)
	{
		renderer->SetClearColorValue(myClearColor[0], myClearColor[1], myClearColor[2], myClearColor[3]);
		renderer->ClearView(RENDERER_CLEAR_COLOR);
	}

	if (IsRenderable())
	{
		if (mGlobalPriority)
		{
			kstl::vector<NodeToDraw> todraw;
			todraw.push_back(NodeToDraw{ myRootItem.get(), 0 });

			if (myRootItem->Draw(state))
			{
				myRootItem->SetUpNodeIfNeeded();
				AccumulateToDraw(state, todraw, myRootItem.get());
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
		else
		{
			myRootItem->TravDraw(state);
		}
	}
	state->HolographicUseStackMatrix = false;
	state->SetHolographicMode(holo_before);
	renderer->PopState();
	

	// call postdraw (deactivate the shader)
	PostDrawDrawable(state);

	// last thing to do
	EndDrawing(state);

	//renderer->PopUIShader();

}

