#ifndef IMGUILAYER_H_
#define IMGUILAYER_H_

#include "Abstract2DLayer.h"

#include "ModuleImGui.h"

#include "TouchInputEventManager.h"
#include "InputIncludes.h"
#include "AttributePacking.h"

#include "Texture.h"

// ****************************************
// * ImGuiLayer class
// * --------------------------------------
/**
* \file	ImGuiLayer.h
* \class	ImGuiLayer
* \ingroup ImGUIModule
* \brief Specialized 2D layer for ImGui display
*
*/
// ****************************************
class ImGuiLayer : public Abstract2DLayer
{
public:
	DECLARE_CLASS_INFO(ImGuiLayer, Abstract2DLayer, ImGui);
	DECLARE_CONSTRUCTOR(ImGuiLayer);
	SIGNALS(OnClickDown, OnClickUp);
	WRAP_METHODS(UpdateKeyboard, ManageTouch, PanelValidHit);
	
	virtual ~ImGuiLayer();
	
	void TravDraw(TravState* state) override;

	/*
	* Set this layer as active
	* returns previous active layer
	*/
	ImGuiContext* SetActiveImGuiLayer()
	{
		ImGuiContext* state = ImGui::GetCurrentContext();
		ImGui::SetCurrentContext(mImGuiState);
		return state;
	}
	void NewFrame(Timer* timer);

	void UpdateKeyboard(kstl::vector<KeyEvent>& keys);

	void SetStyleMSFT();
	void SetStyleHoloLens();

	void RegisterTouch();

	bool WantMouse() { return mWantMouse; }

	bool PanelValidHit(v2f hit_pos) { return WantMouse(); }

	Texture* GetTexture(const std::string& name);

protected:
	DECLARE_METHOD(ResetContext);
	COREMODIFIABLE_METHODS(ResetContext);

	void InitModifiable() override;

	//bool ManageTouch(DirectAccessEvent& ev);
	bool ManageTouch(DirectTouchEvent& ev);

	void SortItemsFrontToBack(SortItemsFrontToBackParam& param) override;

	ImGuiContext* mImGuiState = nullptr;
	CoreBaseApplication* mApp = nullptr;
	ModuleInput* mInput = nullptr;
	kdouble mLastTime = -1.0;

	bool mHasFrame = false;

	maBool mRemote = BASE_ATTRIBUTE(Remote, false);
	maString mRemoteBindAddress = BASE_ATTRIBUTE(RemoteBindAddress, "0.0.0.0");
	maInt mRemotePort = BASE_ATTRIBUTE(RemotePort, 7002);

#ifdef WUP
	maBool mUseOldWUPKeyboard = BASE_ATTRIBUTE(UseOldWUPKeyboard, false);
#endif

	maBool mInputsEnabled = BASE_ATTRIBUTE(InputsEnabled, true);


	maString mFontName = BASE_ATTRIBUTE(FontName, "");
	maFloat mFontSize = BASE_ATTRIBUTE(FontSize, 13.0f);

	maString mStyle = BASE_ATTRIBUTE(Style, "Default");

	kstl::vector<KeyEvent> mKeyEvents;

	std::vector<SmartPointer<Texture>> mUsedTexturesLastFrame;
	std::vector<SmartPointer<Texture>> mUsedTexturesThisFrame;

	ImFontAtlas mFontAtlas;

	TouchSourceID mPosSource = TouchSourceID::Invalid;
	TouchSourceID mClickSource = TouchSourceID::Invalid;
	v2f mCurrentPos = {0.0f,0.0f};

	v2f mStartTouchPos;
	v3f mStartGazePos;

	v3f mStartGazeUp;
	v3f mStartGazeRight;

	bool mIsDown = false;
	bool mHasKeyboardFocus = false;
	bool mWantMouse = false;



	SP<Texture> mFontTexture = nullptr;
	unsigned char* mPixelData = nullptr;
	int mPixelDataWidth = 0;
	int mPixelDataHeight = 0;

};

class ImGuiLayerScoped
{
public:
	ImGuiLayerScoped(ImGuiLayer* layer) : mOld{ layer->SetActiveImGuiLayer() } {}
	~ImGuiLayerScoped()
	{
		ImGui::SetCurrentContext(mOld);
	}
	ImGuiContext* mOld = nullptr;
};

#endif