#pragma once

#include "Abstract2DLayer.h"

#include "ModuleImGui.h"

#include "TouchInputEventManager.h"
#include "InputIncludes.h"
#include "AttributePacking.h"

#include "Texture.h"

namespace Kigs
{
	namespace Gui
	{
		using namespace Draw2D;
		using namespace Draw;
		using namespace Input;
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
			WRAP_METHODS(UpdateKeyboard, ManageTouch, PanelValidHit, GetDataInTouchSupport);

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

			void UpdateKeyboard(std::vector<KeyEvent>& keys);

			void SetStyleMSFT();
			void SetStyleHoloLens();
			void SetStyleLightGreen();

			void RegisterTouch();

			bool WantMouse() { return mWantMouse; }

			bool PanelValidHit(v2f hit_pos) { return WantMouse(); }

			Draw::Texture* GetTexture(const std::string& name);

			ImFont* GetBoldFont() { return mBoldFont; }

		protected:
			DECLARE_METHOD(ResetContext);
			COREMODIFIABLE_METHODS(ResetContext);

			void InitModifiable() override;

			//bool ManageTouch(DirectAccessEvent& ev);
			bool ManageTouch(DirectTouchEvent& ev);

			void SortItemsFrontToBack(SortItemsFrontToBackParam& param) override;
			bool GetDataInTouchSupport(const touchPosInfos& posin, touchPosInfos& pout);

			void ApplyScaling();

			ImGuiContext* mImGuiState = nullptr;
			CoreBaseApplication* mApp = nullptr;
			ModuleInput* mInput = nullptr;
			double mLastTime = -1.0;

			bool mHasFrame = false;

			bool			mRemote = false;
			bool			mInputsEnabled = true;
			bool			mResetInputsWhenNoWindows = false;
			s32				mRemotePort = 7002;
			float			mFontSize = 13.0f;
			float			mScaling = 1.0f;
			std::string		mRemoteBindAddress = "0.0.0.0";
			std::string		mFontName = "";
			std::string		mBoldFontName = "";
			std::string		mStyle = "Default";

			WRAP_ATTRIBUTES(mRemote, mInputsEnabled, mResetInputsWhenNoWindows, mRemotePort, mFontSize, mScaling, mRemoteBindAddress, mFontName, mBoldFontName, mStyle);

#ifdef WUP
			maBool mUseOldWUPKeyboard = BASE_ATTRIBUTE(UseOldWUPKeyboard, false);
#endif
			std::vector<KeyEvent> mKeyEvents;

			std::vector<SmartPointer<Draw::Texture>> mUsedTexturesLastFrame;
			std::vector<SmartPointer<Draw::Texture>> mUsedTexturesThisFrame;

			ImFontAtlas mFontAtlas;
			ImFont* mBoldFont = nullptr;

			TouchSourceID mPosSource = TouchSourceID::Invalid;
			TouchSourceID mClickSource = TouchSourceID::Invalid;
			v2f mCurrentPos = { 0.0f,0.0f };

			v2f mStartTouchPos;
			v3f mStartGazePos;

			v3f mStartGazeUp;
			v3f mStartGazeRight;

			bool mIsDown = false;
			bool mHasKeyboardFocus = false;
			bool mWantMouse = false;
			bool mNeedClearClicks = false;

			SP<Draw::Texture> mFontTexture = nullptr;
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
	}

}