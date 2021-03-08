#pragma once

#include "GenericCamera.h"

#ifdef FFMPEG
#include <libavutil/pixfmt.h>
struct AVFormatContext;
struct AVFrame;
struct AVPacket;
struct EncoderInfo
{
	double lastTime = 0;
	AVPixelFormat PixFormat;
	AVFormatContext *oc;
	AVFrame *frame;
	AVPacket* pkt;
	int width;
	int height;
};
#endif


#include "TinyImage.h"

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Media.Capture.h>
#include <winrt/Windows.Media.Effects.h>
#include <winrt/Windows.Media.Capture.Frames.h>
#include <winrt/Windows.System.Display.h>
#include <winrt/Windows.Foundation.Collections.h>

#include <shared_mutex>

// ****************************************
// * CameraWUP class
// * --------------------------------------
/**
* \file	CameraWUP.h
* \class	CameraWUP
* \ingroup Camera
* \brief   UWP specific webcamera management.
*/
// ****************************************

class CameraWUP : public GenericCamera
{
public:
	DECLARE_CLASS_INFO(CameraWUP, GenericCamera, CameraModule)
	DECLARE_CONSTRUCTOR(CameraWUP);

	virtual void Process()
	{
		// nothing to do, everything is done in the callback
	}

	void StartPreview() override;
	void StopPreview() override;

	winrt::Windows::Foundation::IAsyncAction StartCaptureAsync();
	winrt::Windows::Foundation::IAsyncAction StopCaptureAsync();

#ifdef FFMPEG
	virtual bool StartRecording(kstl::string &);
	virtual void StopRecording();
#endif

	// crop directly in input buffer
	void DoCrop(u8* pBuffer, long &BufferLen);

    void ResetAllBuffers() override;

    void SetMinimumTimeFromNow(int additional_milliseconds=0);

protected:
	void InitModifiable() override;
	void Update(const Timer& timer, void* addParam) override;
	void AllocateFrameBuffers() override;
	void FreeFrameBuffers() override;

	void BufferReceived(double Time, u8* pBuffer, long BufferLen);

	winrt::Windows::Foundation::IAsyncAction InitMediaCaptureAsync();
	
	maBool mUseMRC = BASE_ATTRIBUTE(UseMRC, false);

#ifdef FFMPEG
	EncoderInfo myEI;
	void InitEncoder(const char *filename);
	void EncodeFrame(double Time, void* data, int dataLen);
	void CloseEncoder();
#endif

	unsigned int mRejectFrames = 0;

	bool mYUVFormat = false;
	bool mIsRecording = false;
	bool mNeedCloseEncoder = false;

	bool mIsUpsideDown = false;
	
	using FrameConversionMethod = void(void* bufferin, void* bufferout, int sx, int sy);
	FrameConversionMethod* mConversionMethod = nullptr;

	using FrameCropMethod = void(void* bufferin, long& bufferlen, int sxin, int syin, int sxout, int syout, int offetx, int offsety);
	FrameCropMethod* mFrameCropMethod = nullptr;

	winrt::Windows::Media::Capture::MediaCapture mMediaCapture = nullptr;
	winrt::Windows::Media::Capture::Frames::MediaFrameSource mFrameSource = nullptr;
	winrt::Windows::Media::Capture::Frames::MediaFrameReader mFrameReader = nullptr;
	winrt::Windows::Media::Capture::Frames::MediaFrameReference mLatestFrame = nullptr;

	winrt::Windows::Media::Capture::Frames::MediaFrameReader::FrameArrived_revoker mFrameReaderFrameArrivedRevokeToken;

	std::shared_mutex mMtx;

    LARGE_INTEGER mQPCFrequency;
    LARGE_INTEGER mMinTime = {};
};






#define RUNTIMECLASS_MIXEDREALITYCAPTURE_VIDEO_EFFECT L"Windows.Media.MixedRealityCapture.MixedRealityCaptureVideoEffect"

//
// StreamType: Describe which capture stream this effect is used for.
// Type: Windows::Media::Capture::MediaStreamType as UINT32
// Default: VideoRecord
//
#define PROPERTY_STREAMTYPE L"StreamType"

//
// HologramCompositionEnabled: Flag to enable or disable holograms in video capture.
// Type: bool
// Default: True
//
#define PROPERTY_HOLOGRAMCOMPOSITIONENABLED L"HologramCompositionEnabled"

//
// RecordingIndicatorEnabled: Flag to enable or disable recording indicator on screen during hologram capturing.
// Type: bool
// Default: True
//
#define PROPERTY_RECORDINGINDICATORENABLED L"RecordingIndicatorEnabled"


//
// VideoStabilizationEnabled: Flag to enable or disable video stabilization powered by the HoloLens tracker.
// Type : bool
// Default: False
//
#define PROPERTY_VIDEOSTABILIZATIONENABLED L"VideoStabilizationEnabled"

//
// VideoStabilizationBufferLength: Set how many historical frames are used for video stabilization.
// Type : UINT32 (Max num is 30)
// Default: 0
//
#define PROPERTY_VIDEOSTABILIZATIONBUFFERLENGTH L"VideoStabilizationBufferLength"

//
// GlobalOpacityCoefficient: Set global opacity coefficient of hologram.
// Type : float (0.0 to 1.0)
// Default: 0.9
//
#define PROPERTY_GLOBALOPACITYCOEFFICIENT L"GlobalOpacityCoefficient"

//
// Maximum value of VideoStabilizationBufferLength
// This number is defined and used in MixedRealityCaptureVideoEffect
//
#define PROPERTY_MAX_VSBUFFER 30U


template<typename T, typename U>
U GetValueFromPropertySet(winrt::Windows::Foundation::Collections::IPropertySet const& propertySet, winrt::hstring const& key, U defaultValue)
{
    try
    {
        return static_cast<U>(static_cast<T>(winrt::unbox_value<T>(propertySet.Lookup(key))));
    }
    catch (winrt::hresult_out_of_bounds const& /*e*/)
    {
        // The key is not present in the PropertySet. Return the default value.
        return defaultValue;
    }
}

class MrcVideoEffectDefinition : public winrt::implements<MrcVideoEffectDefinition, winrt::Windows::Media::Effects::IVideoEffectDefinition>
{
public:
    MrcVideoEffectDefinition();

    //
    // IVideoEffectDefinition
    //
    winrt::hstring ActivatableClassId()
    {
        return m_activatableClassId;
    }

    winrt::Windows::Foundation::Collections::IPropertySet Properties()
    {
        return m_propertySet;
    }

    //
    // Mixed Reality Capture effect properties
    //
    winrt::Windows::Media::Capture::MediaStreamType StreamType()
    {
        return GetValueFromPropertySet<uint32_t>(m_propertySet, PROPERTY_STREAMTYPE, DefaultStreamType);
    }

    void StreamType(winrt::Windows::Media::Capture::MediaStreamType newValue)
    {
        m_propertySet.Insert(PROPERTY_STREAMTYPE, winrt::box_value(static_cast<uint32_t>(newValue)));
    }

    bool HologramCompositionEnabled()
    {
        return GetValueFromPropertySet<bool>(m_propertySet, PROPERTY_HOLOGRAMCOMPOSITIONENABLED, DefaultHologramCompositionEnabled);
    }

    void HologramCompositionEnabled(bool newValue)
    {
        m_propertySet.Insert(PROPERTY_HOLOGRAMCOMPOSITIONENABLED, winrt::box_value(newValue));
    }

    bool RecordingIndicatorEnabled()
    {
        return GetValueFromPropertySet<bool>(m_propertySet, PROPERTY_RECORDINGINDICATORENABLED, DefaultRecordingIndicatorEnabled);
    }

    void RecordingIndicatorEnabled(bool newValue)
    {
        m_propertySet.Insert(PROPERTY_RECORDINGINDICATORENABLED, winrt::box_value(newValue));
    }

    bool VideoStabilizationEnabled()
    {
        return GetValueFromPropertySet<bool>(m_propertySet, PROPERTY_VIDEOSTABILIZATIONENABLED, DefaultVideoStabilizationEnabled);
    }

    void VideoStabilizationEnabled(bool newValue)
    {
        m_propertySet.Insert(PROPERTY_VIDEOSTABILIZATIONENABLED, winrt::box_value(newValue));
    }

    uint32_t VideoStabilizationBufferLength()
    {
        return GetValueFromPropertySet<uint32_t>(m_propertySet, PROPERTY_VIDEOSTABILIZATIONBUFFERLENGTH, DefaultVideoStabilizationBufferLength);
    }

    void VideoStabilizationBufferLength(uint32_t newValue)
    {
        m_propertySet.Insert(PROPERTY_VIDEOSTABILIZATIONBUFFERLENGTH, winrt::box_value((std::min)(newValue, PROPERTY_MAX_VSBUFFER)));
    }

    float GlobalOpacityCoefficient()
    {
        return GetValueFromPropertySet<float>(m_propertySet, PROPERTY_GLOBALOPACITYCOEFFICIENT, DefaultGlobalOpacityCoefficient);
    }

    void GlobalOpacityCoefficient(float newValue)
    {
        m_propertySet.Insert(PROPERTY_GLOBALOPACITYCOEFFICIENT, winrt::box_value(newValue));
    }

    uint32_t VideoStabilizationMaximumBufferLength()
    {
        return PROPERTY_MAX_VSBUFFER;
    }

private:
    static constexpr winrt::Windows::Media::Capture::MediaStreamType DefaultStreamType = winrt::Windows::Media::Capture::MediaStreamType::VideoRecord;
    static constexpr bool DefaultHologramCompositionEnabled = true;
    static constexpr bool DefaultRecordingIndicatorEnabled = true;
    static constexpr bool DefaultVideoStabilizationEnabled = false;
    static constexpr uint32_t DefaultVideoStabilizationBufferLength = 0U;
    static constexpr float DefaultGlobalOpacityCoefficient = 0.9f;
private:
    winrt::hstring m_activatableClassId{ RUNTIMECLASS_MIXEDREALITYCAPTURE_VIDEO_EFFECT };
    winrt::Windows::Foundation::Collections::PropertySet m_propertySet;
};
