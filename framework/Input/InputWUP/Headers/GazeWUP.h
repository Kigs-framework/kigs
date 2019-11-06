#pragma once

#include "GazeDevice.h"
#include "ModuleInputWUP.h"

#include <mutex>

#include <winrt/Windows.UI.Input.Spatial.h>

class GazeDeviceWUP : public GazeDevice
{
public:
	DECLARE_CLASS_INFO(GazeDeviceWUP, GazeDevice, Input);
	DECLARE_CONSTRUCTOR(GazeDeviceWUP);

	virtual bool	Aquire();
	virtual bool	Release();

	void	UpdateDevice();

	virtual bool GetTouchPosition(u32 ID, v3f& pos) const override;
	virtual bool GetTouchState(u32 ID, SourceState& state) const override;
	virtual const GazeTouch* GetTouchEvent(u32 ID) const override;
	void	DoInputDeviceDescription();

	void StartListening();
	void StopListening();

protected:
	virtual ~GazeDeviceWUP();
	
	winrt::Windows::UI::Input::Spatial::SpatialInteractionManager mSpatialInteractionManager = nullptr;
	bool mIsListening = false;
	std::mutex mMutex;

	struct Source
	{
		winrt::Windows::UI::Input::Spatial::SpatialInteractionSourceEventArgs args = nullptr;
		SourceState state;
	};

	std::unordered_map<u32, Source> mUpdateList;

	winrt::event_token mInteractionDetectedToken;
	winrt::event_token mSourcePressedToken;
	winrt::event_token mSourceReleasedToken;
	winrt::event_token mSourceLostToken;
	winrt::event_token mSourceUpdatedToken;
	winrt::event_token mSourceDetectedToken;


	bool mVoiceSelect = false;
};

