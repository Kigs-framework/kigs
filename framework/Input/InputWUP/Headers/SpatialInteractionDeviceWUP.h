#pragma once

#include "SpatialInteractionDevice.h"
#include "ModuleInputWUP.h"

#include <mutex>

#include <winrt/Windows.UI.Input.Spatial.h>


// ****************************************
// * SpatialInteractionDeviceWUP class
// * --------------------------------------
/**
 * \class	SpatialInteractionDeviceWUP
 * \file	SpatialInteractionDeviceWUP.h
 * \ingroup Input
 * \brief	UWP Spacial interaction management ( Hololens ).
 */
 // ****************************************

class SpatialInteractionDeviceWUP : public SpatialInteractionDevice
{
public:
	DECLARE_CLASS_INFO(SpatialInteractionDeviceWUP, SpatialInteractionDevice, Input);
	DECLARE_CONSTRUCTOR(SpatialInteractionDeviceWUP);
	virtual ~SpatialInteractionDeviceWUP();

	virtual bool	Aquire();
	virtual bool	Release();

	void	UpdateDevice();
	void	DoInputDeviceDescription();

	void StartListening();
	void StopListening();

protected:
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

