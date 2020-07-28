#include "PrecompiledHeaders.h"
#include "ImGuiLayer.h"
#include "CoreBaseApplication.h"
#include "Platform/Input/KeyDefine.h"
#include "RenderingScreen.h"
#include "Timer.h"
#include "ModuleInput.h"
#include "KeyboardDevice.h"
#include "ImGuiConfig.h"
#include "ModuleFileManager.h"
#include "TextureFileManager.h"
#include "NotificationCenter.h"
#include "ModuleRenderer.h"

#if IMGUI_WEBBY_REMOTE 
#include "imgui_remote.h"
#endif

#include "IconsFontAwesome.h"
#include "fontawesome.h"

#include <algorithm>


#ifdef WUP
#include <utf8.h>
#include <winrt/Windows.Storage.h>
using namespace winrt::Windows::Storage;
#endif

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>

IMPLEMENT_CLASS_INFO(ImGuiLayer);

IMPLEMENT_CONSTRUCTOR(ImGuiLayer)
{
	myImGuiState = ImGui::CreateContext();
	ImGuiContext* old_state = ImGui::GetCurrentContext();
	ImGui::SetCurrentContext(myImGuiState);

	ImGui::GetIO().UserData = this;

#ifdef WUP
	ImGui::GetIO().IniFilename = nullptr;
	auto local_folder = ApplicationData::Current().LocalFolder().Path();
	std::string utf8_path;
	utf8::utf16to8(local_folder.begin(), local_folder.end(), std::back_inserter(utf8_path));
	u64 len;
	auto crb = OwningRawPtrToSmartPtr(ModuleFileManager::Get()->LoadFile((utf8_path + "\\imgui.ini").c_str(), len));
	if (crb)
	{
		ImGui::LoadIniSettingsFromMemory(crb->data(), crb->size());
	}
#endif
	ImGui::SetCurrentContext(old_state);
}

ImGuiLayer::~ImGuiLayer()
{
	ImGuiContext* old_state = SetActiveImGuiLayer();
	ImGui::GetIO().Fonts->TexID = 0;
	ImGui::SetCurrentContext(old_state);
	ImGui::DestroyContext(myImGuiState);
	mFontAtlas.Locked = false;
}

void ImGuiLayer::SetStyleMSFT()
{
	ImGuiContext* old_state = SetActiveImGuiLayer();
	auto style = &ImGui::GetStyle();
	int hspacing = 8;
	int vspacing = 6;
	style->DisplaySafeAreaPadding = ImVec2(0, 0);
	style->WindowPadding = ImVec2(hspacing / 2, vspacing);
	style->FramePadding = ImVec2(hspacing, vspacing);
	style->ItemSpacing = ImVec2(hspacing, vspacing);
	style->ItemInnerSpacing = ImVec2(hspacing, vspacing);
	style->IndentSpacing = 20.0f;

	style->WindowRounding = 0.0f;
	style->FrameRounding = 0.0f;

	//style->WindowBorderSize = 0.0f;
	//style->FrameBorderSize = 1.0f;
	//style->PopupBorderSize = 1.0f;

	style->ScrollbarSize = 20.0f;
	style->ScrollbarRounding = 0.0f;
	style->GrabMinSize = 5.0f;
	style->GrabRounding = 0.0f;

	ImVec4 white = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	ImVec4 transparent = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	ImVec4 dark = ImVec4(0.95f*0.8f, 0.95f*0.8f, 0.95f*0.8f, 1.00f);
	ImVec4 darker = ImVec4(0.95f*0.5f, 0.95f*0.5f, 0.95f*0.5f, 1.00f);

	ImVec4 background = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
	ImVec4 text = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	ImVec4 border = ImVec4(0.60f, 0.60f, 0.60f, 1.00f);
	ImVec4 grab = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
	ImVec4 header = ImVec4(0.86f, 0.86f, 0.86f, 1.00f);
	ImVec4 active = ImVec4(0.00f, 0.47f, 0.84f, 1.00f);
	ImVec4 hover = ImVec4(0.95f*0.2f, std::min(1.0f, 0.95f*0.2f + 0.47f), std::min(1.0f, 0.95f*0.2f + 0.84f), 1.00f);

	style->Colors[ImGuiCol_Text] = text;
	style->Colors[ImGuiCol_WindowBg] = background;
	style->Colors[ImGuiCol_ChildWindowBg] = background;
	style->Colors[ImGuiCol_PopupBg] = white;

	style->Colors[ImGuiCol_Border] = border;
	style->Colors[ImGuiCol_BorderShadow] = transparent;

	style->Colors[ImGuiCol_Button] = header;
	style->Colors[ImGuiCol_ButtonHovered] = hover;
	style->Colors[ImGuiCol_ButtonActive] = active;

	style->Colors[ImGuiCol_FrameBg] = white;
	style->Colors[ImGuiCol_FrameBgHovered] = hover;
	style->Colors[ImGuiCol_FrameBgActive] = active;

	style->Colors[ImGuiCol_MenuBarBg] = header;
	style->Colors[ImGuiCol_Header] = header;
	style->Colors[ImGuiCol_HeaderHovered] = hover;
	style->Colors[ImGuiCol_HeaderActive] = active;

	style->Colors[ImGuiCol_CheckMark] = text;
	style->Colors[ImGuiCol_SliderGrab] = grab;
	style->Colors[ImGuiCol_SliderGrabActive] = darker;

	style->Colors[ImGuiCol_ScrollbarBg] = header;
	style->Colors[ImGuiCol_ScrollbarGrab] = grab;
	style->Colors[ImGuiCol_ScrollbarGrabHovered] = dark;
	style->Colors[ImGuiCol_ScrollbarGrabActive] = darker;

	ImGui::SetCurrentContext(old_state);
}

void ImGuiLayer::SetStyleHoloLens()
{
	SetStyleMSFT();
	ImGuiContext* old_state = SetActiveImGuiLayer();
	auto style = &ImGui::GetStyle();

	int hspacing = 8;
	int vspacing = 6;
	style->DisplaySafeAreaPadding = ImVec2(0, 0);
	style->WindowPadding = ImVec2(hspacing / 2, vspacing);
	style->FramePadding = ImVec2(hspacing, vspacing);
	style->ItemSpacing = ImVec2(hspacing, vspacing);
	style->ItemInnerSpacing = ImVec2(hspacing, vspacing);
	style->IndentSpacing = 20.0f;

	style->WindowRounding = 0.0f;
	style->FrameRounding = 0.0f;

	//style->WindowBorderSize = 0.0f;
	//style->FrameBorderSize = 1.0f;
	//style->PopupBorderSize = 1.0f;

	style->ScrollbarSize = 60.0f;
	style->ScrollbarRounding = 0.0f;
	style->GrabMinSize = 5.0f;
	style->GrabRounding = 0.0f;

	ImGui::SetCurrentContext(old_state);
}

void ImGuiLayer::UpdateKeyboard(kstl::vector<KeyEvent>& keys)
{
	mKeyEvents.insert(mKeyEvents.end(), keys.begin(), keys.end());
}

void ImGuiLayer::RegisterTouch()
{
	//mInput->getTouchManager()->registerEvent(this, "ManageTouch", InputEventType::DirectAccess, InputEventManagementFlag::EmptyFlag, this);
	static_cast<TouchEventStateDirectTouch*>(mInput->getTouchManager()->registerEvent(this, "ManageTouch", InputEventType::DirectTouch, InputEventManagementFlag::EmptyFlag, this))->setAutoTouchDownDistance(0.05f);
}

void ImGuiLayer::InitModifiable()
{
	Abstract2DLayer::InitModifiable();
	if (IsInit())
	{
		KigsCore::GetNotificationCenter()->addObserver(this, "ResetContext", "ResetContext");


		if(mRemote)
			ImGui::RemoteInit(mRemoteBindAddress.c_str(), (int)mRemotePort);
		
		ImGuiContext* old_state = SetActiveImGuiLayer();
		ImGuiIO& io = ImGui::GetIO();
		io.KeyMap[ImGuiKey_Tab] = VK_TAB;                     // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
		io.KeyMap[ImGuiKey_LeftArrow] = VK_LEFT;
		io.KeyMap[ImGuiKey_RightArrow] = VK_RIGHT;
		io.KeyMap[ImGuiKey_UpArrow] = VK_UP;
		io.KeyMap[ImGuiKey_DownArrow] = VK_DOWN;
		io.KeyMap[ImGuiKey_PageUp] = VK_PRIOR;
		io.KeyMap[ImGuiKey_PageDown] = VK_NEXT;
		io.KeyMap[ImGuiKey_Home] = VK_HOME;
		io.KeyMap[ImGuiKey_End] = VK_END;
		io.KeyMap[ImGuiKey_Delete] = VK_DELETE;
		io.KeyMap[ImGuiKey_Backspace] = VK_BACK;
		io.KeyMap[ImGuiKey_Enter] = VK_RETURN;
		io.KeyMap[ImGuiKey_Escape] = VK_ESCAPE;
		io.KeyMap[ImGuiKey_A] = VK_A;
		io.KeyMap[ImGuiKey_C] = VK_C;
		io.KeyMap[ImGuiKey_V] = VK_V;
		io.KeyMap[ImGuiKey_X] = VK_X;
		io.KeyMap[ImGuiKey_Y] = VK_Y;
		io.KeyMap[ImGuiKey_Z] = VK_Z;

		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigDockingWithShift = false;
		io.ConfigWindowsResizeFromEdges = true;

		// TODO(antoine) actual cursor support
		io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;

		mApp = KigsCore::Instance()->GetCoreApplication();
		mInput = (ModuleInput*)KigsCore::Instance()->GetMainModuleInList(InputModuleCoreIndex);

		RegisterTouch();

		mLastTime = -1.0;

		if (mInput->GetKeyboard() && mInputsEnabled)
		{
			KigsCore::Connect(mInput->GetKeyboard(), "KeyboardEvent", this, "UpdateKeyboard");
		}


		ImFontConfig config;
		config.FontDataOwnedByAtlas = false;
		config.MergeMode = false;

		// before setting our font, release previous one
		if (io.Fonts && myImGuiState->FontAtlasOwnedByContext)
		{
			io.Fonts->Locked = false;
			IM_DELETE(io.Fonts);
		}

		io.Fonts = &mFontAtlas;
		myImGuiState->FontAtlasOwnedByContext = false;
		
		ImFont* font = nullptr;
		SmartPointer<CoreRawBuffer> crb;
		if (mFontName.const_ref().size())
		{
			u64 len;
			crb  = OwningRawPtrToSmartPtr(ModuleFileManager::Get()->LoadFile(mFontName.c_str(), len));
			if (crb)
			{
				font = io.Fonts->AddFontFromMemoryTTF(crb->buffer(), crb->size(), mFontSize, &config, io.Fonts->GetGlyphRangesDefault());
			}
		}
		if (font) io.FontDefault = font;
		else io.Fonts->AddFontDefault();

		config.MergeMode = true;
		const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
		io.Fonts->AddFontFromMemoryTTF((void*)g_fontawesome, sizeof(g_fontawesome) - 1, font ? mFontSize : 13.0f, &config, icon_ranges);
		
		io.Fonts->Build();

		auto& style = mStyle.const_ref();
		if (style == "MSFT")
			SetStyleMSFT();
		else if(style == "HoloLens")
			SetStyleHoloLens();

		if (mFontTexture) removeItem((CMSP&)mFontTexture);

		auto tex = KigsCore::GetInstanceOf(getName() + "_font_tex", "Texture");
		mFontTexture = tex;

		addItem(tex);

		// Build texture atlas
		int bpp;
		io.Fonts->GetTexDataAsRGBA32(&mPixelData, &mPixelDataWidth, &mPixelDataHeight, &bpp);

		SmartPointer<TinyImage>	img = OwningRawPtrToSmartPtr(TinyImage::CreateImage(mPixelData, mPixelDataWidth, mPixelDataHeight, TinyImage::RGBA_32_8888));
		mFontTexture->CreateFromImage(img);

		// Store our identifier
		io.Fonts->TexID = (void*)(intptr_t)mFontTexture.get();

		// Cleanup (don't clear the input data if you want to append new fonts later)
		io.Fonts->ClearInputData();

		// We need to keep that pixel data available in case of a reset context (only on android or when debugging with kigstools)
#if !defined(_M_ARM) && !defined(KIGS_TOOLS)
		io.Fonts->ClearTexData();
#endif

		ModuleSpecificRenderer* renderer = (ModuleSpecificRenderer*)((ModuleRenderer*)KigsCore::Instance()->GetMainModuleInList(RendererModuleCoreIndex))->GetSpecificRenderer();
		if (renderer->getDefaultUiShader())
		{
			addItem(renderer->getDefaultUiShader());
		}
		else
		{
			auto imgui_shader = GetFirstInstanceByName("API3DUIShader", "imguishader");
			if (imgui_shader)
			{
				addItem(imgui_shader);
			}
			else
			{
				CMSP newimgui_shader = KigsCore::GetInstanceOf("imguishader", "API3DUIShader");
				addItem(newimgui_shader);
			}
		}


		NewFrame(mApp->GetApplicationTimer().get());
		ImGui::SetCurrentContext(old_state);
	}
}

bool ImGuiLayer::ManageTouch(DirectTouchEvent& ev)
{
	auto old = SetActiveImGuiLayer();
	auto& io = ImGui::GetIO();
	
	//FIXME(android resize issue)
	mCurrentPos = ev.position.xy;// *(v2f)io.DisplayFramebufferScale;
	if(ev.state == GestureRecognizerState::StateBegan)
	{
		if (ev.touch_state == DirectTouchEvent::TouchState::TouchDown)
		{
			if (ev.interaction)
			{
				mStartTouchPos = mCurrentPos;
				mStartGazePos = ev.interaction->Position;
				mStartGazeUp = v3f(0,-1,0);
				mStartGazeRight = ev.direction ^ v3f(0, 1, 0);
			}

			io.MouseDown[0] = (ev.button_state & 1);
			io.MouseDown[1] = (ev.button_state & 2);
			io.MouseDown[2] = (ev.button_state & 4);
			mIsDown = true;
			EmitSignal(Signals::OnClickDown, this);
		}
		else if (ev.touch_state == DirectTouchEvent::TouchState::TouchUp)
		{
			io.MouseDown[0] = io.MouseDown[1] = io.MouseDown[2] = 0;
			mIsDown = false;
			EmitSignal(Signals::OnClickUp, this);
		}
	}

	
	if (ev.state == GestureRecognizerState::StatePossible)
	{
	//	ImGui::Text(V2F_FMT(2) "\n", V2F_EXP(mCurrentPos));
	}

	if (mIsDown && ev.interaction)
	{
		auto diff = ev.interaction->Position - mStartGazePos;
		v2f extra = { Dot(diff, mStartGazeRight), Dot(diff, mStartGazeUp) };

		mCurrentPos = mStartTouchPos + extra*2048.0f;
	}

	bool allow = (ImGui::GetIO().WantCaptureMouse || mIsDown) && mInput->getTouchManager()->isRegisteredOnCurrentState(this) && IsRenderable();

	if(allow)
	{
		*ev.swallow_mask = 0xFFFFFFFF;
		
		if(mIsDown)
			ev.capture_inputs = true;
	}

	ImGui::SetCurrentContext(old);
	return allow;
}

void ImGuiLayer::SortItemsFrontToBack(SortItemsFrontToBackParam& param)
{
	std::transform(param.toSort.begin(), param.toSort.end(), std::back_inserter(param.sorted), [](CoreModifiable* cm) -> std::pair<CoreModifiable*, Hit>
	{
		return { cm, Hit{} };
	});
}

void ImGuiLayer::NewFrame(Timer* timer)
{
	ImGuiContext* old_state = SetActiveImGuiLayer();
	
	ImGuiIO& io = ImGui::GetIO();

#ifdef WUP
	if (io.WantSaveIniSettings)
	{
		auto local_folder = ApplicationData::Current().LocalFolder().Path();
		std::string utf8_path;
		utf8::utf16to8(local_folder.begin(), local_folder.end(), std::back_inserter(utf8_path));

		size_t data_size = 0;
		auto data = ImGui::SaveIniSettingsToMemory(&data_size);
		if (data && data_size)
		{
			if(ModuleFileManager::Get()->SaveFile((utf8_path + "\\imgui.ini").c_str(), (u8*)data, data_size))
				io.WantSaveIniSettings = false;
		}
	}
#endif

	// Setup display size (every frame to accommodate for window resizing)
	kfloat w, h;
	kfloat display_w, display_h;

	RenderingScreen* screen = GetRenderingScreen();

	ImVec2 coef(1, 1);
	if (screen)
	{
		screen->GetSize(w, h);
		screen->GetDesignSize(display_w, display_h);

		io.DisplaySize = v2f(w, h);
		
		coef.x = w / display_w;
		coef.y = h / display_h;

		io.DisplayFramebufferScale = coef;
	}

	// Setup time step
	kdouble current_time = timer->GetTime();
	kfloat dt = mLastTime > 0.0 ? (float)(current_time - mLastTime) : (float)(1.0f / 60.0f);
	if (dt <= 0.0f) dt = 0.0166667f;
	io.DeltaTime = dt;
	mLastTime = current_time;

	bool do_regular_input = true;

	if (gImGuiRemoteAvailable && mInputsEnabled)
	{
		if (mRemote)
		{
			ImGui::RemoteUpdate();
			ImGui::RemoteInput input;
			if (ImGui::RemoteGetInput(input))
			{
				ImGuiIO& io = ImGui::GetIO();
				for (int i = 0; i < 256; i++)
					io.KeysDown[i] = input.KeysDown[i];
				io.KeyCtrl = input.KeyCtrl;
				io.KeyShift = input.KeyShift;
				io.MousePos = input.MousePos;
				io.MouseDown[0] = (input.MouseButtons & 1);
				io.MouseDown[1] = (input.MouseButtons & 2) != 0;
				io.MouseWheel += input.MouseWheelDelta;
				// Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
				io.KeyMap[ImGuiKey_Tab] = ImGuiKey_Tab;
				io.KeyMap[ImGuiKey_LeftArrow] = ImGuiKey_LeftArrow;
				io.KeyMap[ImGuiKey_RightArrow] = ImGuiKey_RightArrow;
				io.KeyMap[ImGuiKey_UpArrow] = ImGuiKey_UpArrow;
				io.KeyMap[ImGuiKey_DownArrow] = ImGuiKey_DownArrow;
				io.KeyMap[ImGuiKey_Home] = ImGuiKey_Home;
				io.KeyMap[ImGuiKey_End] = ImGuiKey_End;
				io.KeyMap[ImGuiKey_Delete] = ImGuiKey_Delete;
				io.KeyMap[ImGuiKey_Backspace] = ImGuiKey_Backspace;
				io.KeyMap[ImGuiKey_Enter] = 13;
				io.KeyMap[ImGuiKey_Escape] = 27;
				io.KeyMap[ImGuiKey_A] = 'a';
				io.KeyMap[ImGuiKey_C] = 'c';
				io.KeyMap[ImGuiKey_V] = 'v';
				io.KeyMap[ImGuiKey_X] = 'x';
				io.KeyMap[ImGuiKey_Y] = 'y';
				io.KeyMap[ImGuiKey_Z] = 'z';
				do_regular_input = false;
			}
		}
	}
	
	if (!mInputsEnabled)
	{
		io.MousePos = ImVec2(-1, -1);
		io.KeyCtrl = false;
		io.KeyAlt = false;
		io.KeyShift = false;
		for (auto& k : io.KeysDown)
			k = false;
	}
	if (do_regular_input && mInputsEnabled)
	{
		// Setup inputs
		// (we already got mouse wheel, keyboard keys & characters from glfw callbacks polled in glfwPollEvents())
		
		if (mApp->HasFocus())
		{
			io.MousePos = mCurrentPos * v2f(coef);
		}
		else
		{
			io.MousePos = ImVec2(-1, -1);
		}
		auto mouse = mInput->GetMouse();
		if(mouse)
			io.MouseWheel += mouse->getWheelRollDelta()/120.0f;

		KeyboardDevice* kb = mInput->GetKeyboard();

		auto test = KeyboardDevice::Get();
		if (kb)
		{
#ifdef WUP
			if (io.WantTextInput && mInput->getTouchManager()->isRegisteredOnCurrentState(this))
			{
				//SetInternalFocus();
				mHasKeyboardFocus = true;
				std::u16string typed = kb->ConsumeTypedCharacters();
				if (mUseOldWUPKeyboard)
				{
					for (auto c : typed)
					{
						io.AddInputCharacter(c);
					}
				}
			}
			else if(mHasKeyboardFocus)
			{
				mHasKeyboardFocus = false;
				//RemoveInternalFocus();
			}
#endif

			std::array<bool, 256> state_changed = {};
			std::vector<KeyEvent> toReAdd;
			for (auto& key : mKeyEvents)
			{
				if (state_changed[key.KeyCode])
				{
					toReAdd.push_back(key);
					continue;
				}

				if (key.Action == key.ACTION_DOWN)
				{
					state_changed[key.KeyCode] = true;

					io.KeysDown[key.KeyCode] = true;
					if ((key.KeyCode == VK_CONTROL) || (key.KeyCode == VK_LCONTROL) || (key.KeyCode == VK_RCONTROL))
						io.KeyCtrl = true;
					if ((key.KeyCode == VK_SHIFT) || (key.KeyCode == VK_LSHIFT) || (key.KeyCode == VK_RSHIFT))
						io.KeyShift = true;
					if ((key.KeyCode == VK_MENU) || (key.KeyCode == VK_LMENU) || (key.KeyCode == VK_RMENU))
						io.KeyAlt = true;
				}
				else if (key.Action == key.ACTION_UP)
				{
					state_changed[key.KeyCode] = true;

					io.KeysDown[key.KeyCode] = false;
					if ((key.KeyCode == VK_CONTROL) || (key.KeyCode == VK_LCONTROL) || (key.KeyCode == VK_RCONTROL))
						io.KeyCtrl = false;
					if ((key.KeyCode == VK_SHIFT) || (key.KeyCode == VK_LSHIFT) || (key.KeyCode == VK_RSHIFT))
						io.KeyShift = false;
					if ((key.KeyCode == VK_MENU) || (key.KeyCode == VK_LMENU) || (key.KeyCode == VK_RMENU))
						io.KeyAlt = false;

#ifndef WUP
					if (key.Unicode > 0)
					{
						if (io.WantTextInput && mInput->getTouchManager()->isRegisteredOnCurrentState(this))
						{

							io.AddInputCharacter(key.Unicode);

						}
					}
#endif
				}
			}
			mKeyEvents.clear();
			for (auto& key : toReAdd)
			{
				mKeyEvents.push_back(key);
			}
		}
	}


	mWantMouse = ImGui::GetIO().WantCaptureMouse;
	ImGui::NewFrame();
	ImGui::SetCurrentContext(old_state);
}

Texture* ImGuiLayer::GetTexture(const std::string& name)
{
	auto& tfm = KigsCore::Singleton<TextureFileManager>();
	auto tex = tfm->GetTexture(name);
	mUsedTexturesThisFrame.push_back(tex);
	return tex.get();
}

// Provided by ImGui.lib
namespace ImGui
{
	bool RemoteDraw(ImDrawList** const cmd_lists, int cmd_lists_count);
}

#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))

void ImGuiLayer::TravDraw(TravState* state)
{
	if (!IsInit())
	{
		InitModifiable();
		if (!IsInit()) return;
	}

	if (!IsRenderable())
		return;

	ImGuiContext* old_state = SetActiveImGuiLayer();

	mUsedTexturesLastFrame = std::move(mUsedTexturesThisFrame);
	mUsedTexturesThisFrame = {};

	ImGui::Render();
	ImDrawData* draw_data = ImGui::GetDrawData();
	if (!draw_data)
	{
		ImGui::SetCurrentContext(old_state);
		return;
	}

	if (gImGuiRemoteAvailable)
	{
		if (mRemote && ImGui::RemoteDraw(draw_data->CmdLists, draw_data->CmdListsCount))
		{
			NewFrame(mApp->GetApplicationTimer().get());
			ImGui::SetCurrentContext(old_state);
			return;
		}
	}

	ImGuiIO& io = ImGui::GetIO();

	ModuleSpecificRenderer* renderer = (ModuleSpecificRenderer*)state->GetRenderer();

	if (!StartDrawing(state))
	{
		NewFrame(mApp->GetApplicationTimer().get());
		ImGui::SetCurrentContext(old_state);
		return;
	}

	PreDrawDrawable(state);


	renderer->PushState();

	auto holo_before = state->GetHolographicMode();

	state->SetHolographicMode(myRenderingScreen->as<RenderingScreen>()->IsHolographic());
	state->HolographicUseStackMatrix = true;

	//state->SetCurrentDrawablePass(0);
	state->SetAllVisible(false);

	ImVec2 design_size;
	design_size.x = io.DisplaySize.x; // *io.DisplayFramebufferScale.x;
	design_size.y = io.DisplaySize.y; // *io.DisplayFramebufferScale.y;

	// FIXME(antoine) zero size (when minimizing) causes renderer to crash
	if (design_size.x == 0) design_size.x = 1;
	if (design_size.y == 0) design_size.y = 1;

	renderer->Viewport(0, 0, (unsigned int)io.DisplaySize.x, (unsigned int)io.DisplaySize.y);
	//renderer->Viewport(0, 0, (unsigned int)design_size.x, (unsigned int)design_size.y);

	// create shader if none
	int lShaderMask = ModuleRenderer::VERTEX_ARRAY_MASK | ModuleRenderer::COLOR_ARRAY_MASK | ModuleRenderer::TEXCOORD_ARRAY_MASK | ModuleRenderer::NO_LIGHT_MASK;
	renderer->GetActiveShader()->ChooseShader(state, lShaderMask);

	renderer->SetLightMode(RENDERER_LIGHT_OFF);
	renderer->SetBlendMode(RENDERER_BLEND_ON);
	renderer->SetBlendFuncMode(RENDERER_BLEND_SRC_ALPHA, RENDERER_BLEND_ONE_MINUS_SRC_ALPHA);
	renderer->SetDepthMaskMode(RENDERER_DEPTH_MASK_OFF);
	renderer->SetDepthTestMode(false);
	renderer->SetCullMode(RENDERER_CULL_NONE);

	renderer->EnableTexture();
	renderer->ActiveTextureChannel(0);

	renderer->SetScissorTestMode(RENDERER_SCISSOR_TEST_ON);
	renderer->SetScissorValue(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);

	float fb_height = io.DisplaySize.y;// *io.DisplayFramebufferScale.y;
	//auto scale = io.DisplayFramebufferScale;
	//draw_data->ScaleClipRects(scale);

	// Setup orthographic projection matrix
	renderer->Ortho(MATRIX_MODE_PROJECTION, 0.0f, io.DisplaySize.x, io.DisplaySize.y, 0.0f, -1.0f, 1.0f);
	renderer->LoadIdentity(MATRIX_MODE_MODEL);
	renderer->LoadIdentity(MATRIX_MODE_VIEW);

	renderer->FlushState(true);

	if (myClearColorBuffer)
	{
		renderer->SetClearColorValue(myClearColor[0], myClearColor[1], myClearColor[2], myClearColor[3]);
		renderer->ClearView(RENDERER_CLEAR_COLOR);
	}

	auto locs = renderer->GetActiveShader()->GetLocation();

	//renderer->SetElementBuffer(0);

	auto clip_offset = draw_data->DisplayPos;
	auto clip_scale = draw_data->FramebufferScale;

	// Render command lists
	for (int n = 0; n < draw_data->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[n];
		unsigned char* vtx_buffer = (unsigned char*)& cmd_list->VtxBuffer.front();
		const ImDrawIdx* idx_buffer = &cmd_list->IdxBuffer.front();
		int offset = 0;

		unsigned int vbo_array = renderer->getVBO();
		unsigned int vbo_index = renderer->getVBO();

		renderer->SetArrayBuffer(vbo_array);
		renderer->GetVertexBufferManager()->FlushBindBuffer();
		renderer->BufferData(vbo_array, KIGS_BUFFER_TARGET_ARRAY, cmd_list->VtxBuffer.size() * sizeof(ImDrawVert), (void*)vtx_buffer, KIGS_BUFFER_USAGE_DYNAMIC);

		renderer->SetElementBuffer(vbo_index);
		renderer->GetVertexBufferManager()->FlushBindBuffer();
		renderer->BufferData(vbo_index, KIGS_BUFFER_TARGET_ELEMENT, cmd_list->IdxBuffer.size() * sizeof(u16), (void*)idx_buffer, KIGS_BUFFER_USAGE_DYNAMIC);

		idx_buffer = nullptr;

		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.size(); cmd_i++)
		{
			const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
			if (pcmd->UserCallback)
			{
				pcmd->UserCallback(cmd_list, pcmd);
			}
			else
			{
				renderer->SetArrayBuffer(vbo_array);
				renderer->SetVertexAttrib(vbo_array, KIGS_VERTEX_ATTRIB_VERTEX_ID, 2, KIGS_FLOAT, false, sizeof(ImDrawVert), (void*)OFFSETOF(ImDrawVert, pos), locs);
				renderer->SetVertexAttrib(vbo_array, KIGS_VERTEX_ATTRIB_TEXCOORD_ID, 2, KIGS_FLOAT, false, sizeof(ImDrawVert), (void*)OFFSETOF(ImDrawVert, uv), locs);
				renderer->SetVertexAttrib(vbo_array, KIGS_VERTEX_ATTRIB_COLOR_ID, 4, KIGS_UNSIGNED_BYTE, false, sizeof(ImDrawVert), (void*)OFFSETOF(ImDrawVert, col), locs);

				//unsigned int id = pcmd->TextureId != 0 ? ((OpenGLTexture*)(Texture*)pcmd->TextureId)->GetGLID() : 0;
				if (pcmd->TextureId != 0)
				{
					((Texture*)pcmd->TextureId)->DoPreDraw(state);
				}

				ImVec4 clip_rect;
				clip_rect.x = (pcmd->ClipRect.x - clip_offset.x) * clip_scale.x;
				clip_rect.y = (pcmd->ClipRect.y - clip_offset.y) * clip_scale.y;
				clip_rect.z = (pcmd->ClipRect.z - clip_offset.x) * clip_scale.x;
				clip_rect.w = (pcmd->ClipRect.w - clip_offset.y) * clip_scale.y;

				renderer->SetScissorValue((int)clip_rect.x, (int)(fb_height - clip_rect.w), (int)(clip_rect.z - clip_rect.x), (int)(clip_rect.w - clip_rect.y));
				renderer->DrawElements(state, KIGS_DRAW_MODE_TRIANGLES, pcmd->ElemCount, KIGS_UNSIGNED_SHORT, (void*)idx_buffer);

				if (pcmd->TextureId != 0)
				{
					((Texture*)pcmd->TextureId)->DoPostDraw(state);
				}
			}
			idx_buffer += pcmd->ElemCount;
			offset += pcmd->ElemCount;
		}
	}

	renderer->SetArrayBuffer(-1);
	renderer->SetElementBuffer(-1);
	renderer->DisableTexture();

	state->HolographicUseStackMatrix = false;
	state->SetHolographicMode(holo_before);
	renderer->PopState();

	PostDrawDrawable(state);
	EndDrawing(state);


	NewFrame(mApp->GetApplicationTimer().get());
	ImGui::SetCurrentContext(old_state);
}


DEFINE_METHOD(ImGuiLayer, ResetContext)
{
	ImGuiContext* old_state = SetActiveImGuiLayer();

	ImGuiIO& io = ImGui::GetIO();

	if (mFontTexture) removeItem((CMSP&)mFontTexture);
	auto tex = KigsCore::GetInstanceOf(getName() + "_font_tex", "Texture");
	mFontTexture = tex;

	// Build texture atlas
	//unsigned char* pixels;
	//int width, height;
	//io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

	mFontTexture->Init();

	SmartPointer<TinyImage>	img = OwningRawPtrToSmartPtr(TinyImage::CreateImage(mPixelData, mPixelDataWidth, mPixelDataHeight, TinyImage::RGBA_32_8888));

	mFontTexture->CreateFromImage(img);

	addItem(tex);

	// Store our identifier
	io.Fonts->TexID = (void*)(intptr_t)mFontTexture.get();

	// Cleanup (don't clear the input data if you want to append new fonts later)
	//io.Fonts->ClearInputData();
	//io.Fonts->ClearTexData();

	// Flush current frame data
	ImGui::Render();
	NewFrame(KigsCore::Instance()->GetCoreApplication()->GetApplicationTimer().get());

	ImGui::SetCurrentContext(old_state);
	return false;
}