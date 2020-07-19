#pragma once

#include "SpatialInteractionDevice.h"
#include "ModuleInputWUP.h"

#include <mutex>

#include <winrt/Windows.UI.Input.Spatial.h>

class SpatialInteractionDeviceWUP : public SpatialInteractionDevice
{
public:
	DECLARE_CLASS_INFO(SpatialInteractionDeviceWUP, SpatialInteractionDevice, Input);
	DECLARE_CONSTRUCTOR(SpatialInteractionDeviceWUP);

	virtual bool	Aquire();
	virtual bool	Release();

	void	UpdateDevice();

	bool GetInteractionPosition(u32 ID, v3f& pos) const override;
	bool GetInteractionState(u32 ID, SourceState& state) const override;
	const Interaction* GetInteraction(u32 ID) const override;

	void	DoInputDeviceDescription();

	void StartListening();
	void StopListening();

protected:
	virtual ~SpatialInteractionDeviceWUP();
	
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

