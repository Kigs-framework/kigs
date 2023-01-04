#include "PrecompiledHeaders.h"

#include <dshow.h>
#include <stdio.h>
#include <Shlobj.h>
#include "CameraWindows.h"
#include "Core.h"
#include "FilepathManager.h"

#ifdef FFMPEG
extern "C"
{
#include "libavutil/opt.h"
#include "libavutil/imgutils.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
}
#endif

using namespace Kigs::Camera;

volatile bool   inCallback = false;

// missing in general header file ?

EXTERN_C const GUID DECLSPEC_SELECTANY MEDIASUBTYPE_I420 = { 0x30323449, 0x0000, 0x0010, { 0x80, 0x00, 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71 } };


void MyFreeMediaType(AM_MEDIA_TYPE& mt)
{
	if (mt.cbFormat != 0)
	{
		CoTaskMemFree((PVOID)mt.pbFormat);
		mt.cbFormat = 0;
		mt.pbFormat = NULL;
	}
	if (mt.pUnk != NULL)
	{
		// Unecessary because pUnk should not be used, but safest.
		mt.pUnk->Release();
		mt.pUnk = NULL;
	}
}


inline unsigned int SAFE_RELEASE(IUnknown** x)
{
	unsigned int result = 0;
	if (*x)
	{
		result = (*x)->Release();
		if (result == 0)
		{
			*x = NULL;
		}
	}
	return result;
}

IMPLEMENT_CLASS_INFO(CameraWindows)

//! constructor
CameraWindows::CameraWindows(const std::string& name, CLASS_NAME_TREE_ARG)
	: GenericCamera(name, PASS_CLASS_NAME_TREE_ARG)
	, myVW(0)
	, myMC(0)
	, myME(0)
	, myGraph(0)
	, myCapture(0)
	, mIsRecording(false)
	, mNeedCloseEncoder(false)
	, mySampleGrabberFilter(0)
	, mySampleGrabber(0)
	, myNullRenderer(0)
	, myYUVFormat(false)
	, myIsUpsideDown(false)
	, myConversionMethod(0)
	, myFrameCropMethod(0)
{
	myRejectFrames = 0;
}

//! destructor
CameraWindows::~CameraWindows()
{
	mySampleGrabber->SetCallback(0, 1);

	::Sleep(30);
	while (inCallback)
	{
		::Sleep(1);
	}

	if (mySrcFilter)
		SAFE_RELEASE((IUnknown**)&mySrcFilter);

	if (myCapture)
	{
		myCapture->Release();
		myCapture = 0;
	}

	if (myGraph)
	{
		myGraph->Release();
		myGraph = 0;
	}

	if (mySampleGrabberFilter)
	{
		mySampleGrabberFilter->Release();
		mySampleGrabberFilter = 0;
	}


	if (mySampleGrabber)
	{
		mySampleGrabber->Release();
		mySampleGrabber = 0;
	}

	if (myNullRenderer)
	{
		myNullRenderer->Release();
		myNullRenderer = 0;
	}

	if (myVW)
	{
		myVW->Release();
		myVW = 0;
	}

	if (myMC)
	{
		myMC->Release();
		myMC = 0;
	}


	if (myME)
	{
		myME->Release();
		myME = 0;
	}

	FreeFrameBuffers();
}

STDMETHODIMP SampleGrabberCallback::QueryInterface(REFIID riid, void **ppvObject)
{
	if (NULL == ppvObject) return E_POINTER;
	if (riid == __uuidof(IUnknown))
	{
		*ppvObject = static_cast<IUnknown*>(this);
		return S_OK;
	}
	if (riid == __uuidof(ISampleGrabberCB))
	{
		*ppvObject = static_cast<ISampleGrabberCB*>(this);
		return S_OK;
	}
	return E_NOTIMPL;
}


STDMETHODIMP SampleGrabberCallback::BufferCB(double Time, BYTE *pBuffer, long BufferLen)
{
	inCallback = true;
	if (myCamera->isConnected())
	{
		if (myCamera->NeedCrop())
		{
			// crop directly in input buffer
			myCamera->DoCrop(pBuffer, BufferLen);
		}
		myCamera->BufferReceived(Time, pBuffer, BufferLen);
	}
	inCallback = false;
	return S_OK;
}

// crop directly in input buffer
void	CameraWindows::DoCrop(BYTE *pBuffer, long &BufferLen)
{
	myFrameCropMethod(pBuffer, BufferLen, mInitResX, mInitResY, mWidth, mHeight, mOffsetX, mOffsetY);
}

bool			CameraWindows::isConnected()
{
	mySampleGrabber->GetConnectedMediaType(&myAmMediaType);

	if ((myAmMediaType.majortype != MEDIATYPE_Video) ||
		(myAmMediaType.formattype != FORMAT_VideoInfo) ||
		(myAmMediaType.cbFormat < sizeof(VIDEOINFOHEADER)) ||
		(myAmMediaType.pbFormat == NULL))
	{
		return false;
	}
	return true;
}

void	CameraWindows::BufferReceived(double Time, BYTE *pBuffer, long BufferLen)
{
#ifdef FFMPEG
	if (mIsRecording)
	{
		EncodeFrame(Time, pBuffer, BufferLen);
	}
	else if (mNeedCloseEncoder)
	{
		mNeedCloseEncoder = false;
		EncodeFrame(Time, pBuffer, BufferLen);
		CloseEncoder();
	}
#endif
	myRejectFrames++;
	if (myRejectFrames < 2)
	{
		return;
	}
	mFrameCounter++;

	if ((mFrameCounter % ((int)mSkippedFrameCount+1)) != 0)
	{
		return;
	}
	mFrameCounter = 0;


	// copy buffer
	int buffertoset = GetFreeBuffer();
	if (buffertoset == -1)
	{
		buffertoset = GetReadyBufferIndex();
		if (buffertoset == -1)
			return;
	}
	SetBufferState(buffertoset, ProcessingBuffer);

	if (myConversionMethod)
	{
		myConversionMethod(pBuffer, mFrameBuffers[buffertoset]->buffer(), mWidth, mHeight);
	}
	else // bgr 555 to rgb 555 
	{
		unsigned short*	bufferOffset = ((unsigned short*)pBuffer);


		for (int i = 0; i < mHeight; i++)
		{
			unsigned short* writebuffer = (((unsigned short*)mFrameBuffers[buffertoset]->buffer()) + (mWidth*i));

			for (int j = 0; j < mWidth; j++)
			{
				unsigned short pix = bufferOffset[j];

				unsigned short R = pix & 0x7C00;
				unsigned short G = pix & 0x03E0;
				unsigned short B = pix & 0x001F;


				R = R >> 10;
				B = B << 10;
				*writebuffer++ = R | G | B | 0x8000;
			}

			// next line
			bufferOffset += mWidth;
		}
	}

	// upside down ?
	if (myIsUpsideDown) // flip image
	{
		char* invertB = mFrameBuffers[buffertoset]->buffer();
		
		int PixelLineSize = mWidth * mPixelSize;

		char* tmpbuffer = new char[PixelLineSize];

		char* l1, *l2;

		int i;
		for (i = 0; i < (mHeight / 2); i++)
		{
		l1 = invertB + i*PixelLineSize;
		l2 = invertB + (mHeight - i - 1)*PixelLineSize;

		// copy l1 to buffer
		memcpy(tmpbuffer, l1, (int)PixelLineSize);
		// copy l2 to l1
		memcpy(l1, l2, (int)PixelLineSize);
		// copy buffer to l2
		memcpy(l2, tmpbuffer, (int)PixelLineSize);
		}

		delete[] tmpbuffer;

	}

	SetBufferState(buffertoset, ReadyBuffer);
}


void CameraWindows::InitModifiable()
{
	GenericCamera::InitModifiable();

	mState = isStopped;

	HRESULT hr;
	mySrcFilter = NULL;

	//	ghNotification = hNotificationWindow;
	// Get DirectShow interfaces
	hr = GetInterfaces();
	if (FAILED(hr))
	{
		// error
		return;
	}

	// Attach the filter graph to the capture graph
	hr = myCapture->SetFiltergraph(myGraph);
	if (FAILED(hr))
	{
		// error
		return;
	}

	// Use the system device enumerator and class enumerator to find
	// a video capture/preview device, such as a desktop USB video camera.
	hr = FindCaptureDevice(&mySrcFilter);
	if (FAILED(hr))
	{
		// Don't display a message because FindCaptureDevice will handle it
		return;
	}

	hr = myCapture->FindInterface(
		&PIN_CATEGORY_CAPTURE,
		&MEDIATYPE_Video,
		mySrcFilter,
		IID_IAMStreamConfig,
		(void**)&myConfigInterface
		);

	if (FAILED(hr))
		return;

	int iCount, iSize;
	VIDEO_STREAM_CONFIG_CAPS caps;
	myConfigInterface->GetNumberOfCapabilities(&iCount, &iSize);

	if (sizeof(caps) != iSize) {
		// not what I was expecting
		return;
	}

	// first choice is init res
	int WantedResX = mInitResX;
	int WantedResY = mInitResY;

	// if init res was not set, use width / height
	if ((WantedResX == 0) || (WantedResY == 0))
	{
		WantedResX = mWidth;
		WantedResY = mHeight;
	}

	// else use default camera config
	if ((WantedResX == 0) || (WantedResY == 0))
	{
		AM_MEDIA_TYPE *pmt;
		// get default config and use it
		myConfigInterface->GetFormat(&pmt);
		VIDEOINFOHEADER *pVideoHeader = (VIDEOINFOHEADER*)pmt->pbFormat;
		mInitResX=mWidth = WantedResX = pVideoHeader->bmiHeader.biWidth;
		mInitResY=mHeight = WantedResY = abs((int)pVideoHeader->bmiHeader.biHeight);
		MyFreeMediaType(*pmt);
	}

	if ((mWidth == 0) || (mHeight == 0))
	{
		mWidth = mInitResX;
		mHeight = mInitResY;
	}
	// search config with good resolution
	float selectedResX = 0, selectedResY = 0;
	float errorRes = 99999;
	bool	configfound = false;
	for (int i = 0; i < iCount; i++)
	{
		AM_MEDIA_TYPE *pmt;
		if (myConfigInterface->GetStreamCaps(i, &pmt, reinterpret_cast<BYTE*>(&caps)) == S_OK)
		{
			if ((pmt->formattype != FORMAT_VideoInfo))
				continue;

			int cErrorRes = abs(WantedResX - caps.MaxOutputSize.cx)+ abs(WantedResY - caps.MaxOutputSize.cy);
			if (cErrorRes >= errorRes)
				continue;

			{
				bool prefered = false;
				if (myYUVFormat)
				{
					bool localConfigFound = false;
					if (((std::string)mFormat) == "YUV422")
					{
						if (pmt->subtype == MEDIASUBTYPE_I420)
						{
							myAmMediaType = *pmt;
							//myAmMediaType.subtype = MEDIASUBTYPE_I420;
							myConversionMethod = &ConvertFormat_I420ToYUV422;
							myFrameCropMethod = &CropBuffer_I420;
							localConfigFound = true;

						}
						else if (pmt->subtype == MEDIASUBTYPE_UYVY)
						{
							myAmMediaType = *pmt;
							//myAmMediaType.subtype = MEDIASUBTYPE_UYVY;
							myConversionMethod = &ConvertFormat_YUY2ToYUV422;
							myFrameCropMethod = &CropBuffer_YUY2;
							prefered = true;
							localConfigFound = true;

						}
						else if (pmt->subtype == MEDIASUBTYPE_YUY2)
						{
							myAmMediaType = *pmt;
							//myAmMediaType.subtype = MEDIASUBTYPE_YUY2;
							myConversionMethod = &ConvertFormat_YUY2ToYUV422;
							myFrameCropMethod = &CropBuffer_YUY2;
							prefered = true;
							localConfigFound = true;

						}
						else if (pmt->subtype == MEDIASUBTYPE_AYUV)
						{
							myAmMediaType = *pmt;
							//myAmMediaType.subtype = MEDIASUBTYPE_AYUV;
							myConversionMethod = &ConvertFormat_YUV24ToYUV422;
							myFrameCropMethod = &CropBuffer_YUV24;
							localConfigFound = true;
						}
					}
					else // YUV24
					{
						if (pmt->subtype == MEDIASUBTYPE_I420)
						{
							myAmMediaType = *pmt;
							//myAmMediaType.subtype = MEDIASUBTYPE_I420;
							myConversionMethod = &ConvertFormat_I420ToYUV24;
							myFrameCropMethod = &CropBuffer_I420;
							localConfigFound = true;

						}
						else if (pmt->subtype == MEDIASUBTYPE_UYVY)
						{
							myAmMediaType = *pmt;
							//myAmMediaType.subtype = MEDIASUBTYPE_UYVY;
							myConversionMethod = &ConvertFormat_YUY2ToYUV24;
							myFrameCropMethod = &CropBuffer_YUY2;
							localConfigFound = true;

						}
						else if (pmt->subtype == MEDIASUBTYPE_YUY2)
						{
							myAmMediaType = *pmt;
							//myAmMediaType.subtype = MEDIASUBTYPE_YUY2;
							myConversionMethod = &ConvertFormat_YUY2ToYUV24;
							myFrameCropMethod = &CropBuffer_YUY2;
							localConfigFound = true;

						}
						else if (pmt->subtype == MEDIASUBTYPE_AYUV)
						{
							myAmMediaType = *pmt;
							//myAmMediaType.subtype = MEDIASUBTYPE_AYUV;
							myConversionMethod = &CopyFormat_24; // no conversion, 24 bit per pixel
							myFrameCropMethod = &CropBuffer_RGB24;
							prefered = true;
							localConfigFound = true;
						}
					}

					if (localConfigFound)
					{
						selectedResX = caps.MaxOutputSize.cx;
						selectedResY = caps.MaxOutputSize.cy;
						errorRes = cErrorRes;
						myConfigInterface->SetFormat(pmt);
						myIsUpsideDown = false;
						configfound = true;
						if (prefered)
						{
							MyFreeMediaType(*pmt);
							break;
						}
					}
				}
				else // rgb 24 or 555
				{
					bool localConfigFound = false;
					if (((std::string)mFormat) == "RGB24")
					{
						if (pmt->subtype == MEDIASUBTYPE_RGB24)
						{
							myAmMediaType = *pmt;
							//myAmMediaType.subtype = MEDIASUBTYPE_RGB24;
							prefered = true;
							localConfigFound = true;
							myConversionMethod = &ConvertFormat_BGR24ToRGB24;
							myFrameCropMethod = &CropBuffer_RGB24;
						}
						else if (pmt->subtype == MEDIASUBTYPE_RGB555)
						{
							myAmMediaType = *pmt;
							//myAmMediaType.subtype = MEDIASUBTYPE_RGB555;
							localConfigFound = true;
							myConversionMethod = &ConvertFormat_BGR555ToRGB24;
							myFrameCropMethod = &CropBuffer_BGR555;
						}
					}
					else if (((std::string)mFormat) == "RGB555")
					{
						if (pmt->subtype == MEDIASUBTYPE_RGB24)
						{
							myAmMediaType = *pmt;
							//myAmMediaType.subtype = MEDIASUBTYPE_RGB24;
							localConfigFound = true;
							myConversionMethod = &ConvertFormat_BGR24ToRGB555; // same as rgb24
							myFrameCropMethod = &CropBuffer_RGB24;
						}
						else if (pmt->subtype == MEDIASUBTYPE_RGB555)
						{
							myAmMediaType = *pmt;
							//myAmMediaType.subtype = MEDIASUBTYPE_RGB555;
							localConfigFound = true;
							prefered = true;
							myConversionMethod = &CopyFormat_16;
							myFrameCropMethod = &CropBuffer_BGR555;
						}
					}
					else if (((std::string)mFormat) == "RGB32")
					{
						if (pmt->subtype == MEDIASUBTYPE_RGB32)
						{
							myAmMediaType = *pmt;
							//myAmMediaType.subtype = MEDIASUBTYPE_RGB32;
							localConfigFound = true;
							myConversionMethod = &ConvertFormat_BGR24ToRGB32; // same as rgb24
							myFrameCropMethod = &CropBuffer_RGB32;
						}
						else if (pmt->subtype == MEDIASUBTYPE_RGB555)
						{
							myAmMediaType = *pmt;
							//myAmMediaType.subtype = MEDIASUBTYPE_RGB555;
							localConfigFound = true;
							prefered = true;
							myConversionMethod = &CopyFormat_16;
							myFrameCropMethod = &CropBuffer_BGR555;
						}
					}


					if (localConfigFound)
					{
						selectedResX = caps.MaxOutputSize.cx;
						selectedResY = caps.MaxOutputSize.cy;
						myConfigInterface->SetFormat(pmt);
						myIsUpsideDown = true;
						configfound = true;
						if (prefered)
						{
							MyFreeMediaType(*pmt);
							break;
						}
					}
				}

			}
			MyFreeMediaType(*pmt);
		}
	}

	if (!configfound)
	{
		AM_MEDIA_TYPE *pmt;
		// get default config and use it
		myConfigInterface->GetFormat(&pmt);

		KIGS_ERROR("BAD CAMERA capture format", 1);

		VIDEOINFOHEADER *pVideoHeader = (VIDEOINFOHEADER*)pmt->pbFormat;

		mWidth = mInitResX = pVideoHeader->bmiHeader.biWidth;
		mHeight = mInitResY = abs((int)pVideoHeader->bmiHeader.biHeight);

		if ((pVideoHeader->bmiHeader.biBitCount == 24 || pVideoHeader->bmiHeader.biBitCount == 32) && pVideoHeader->bmiHeader.biHeight > 0)
		{
			myIsUpsideDown = true;
		}
		MyFreeMediaType(*pmt);
	}
	else
	{
		AM_MEDIA_TYPE *pmt;
		// get default config and use it
		myConfigInterface->GetFormat(&pmt);

		mWidth = mInitResX = selectedResX;
		mHeight = mInitResY = selectedResY;

		if ((mInitResX == 0) || (mInitResY == 0))
		{
			mInitResX = mWidth;
			mInitResY = mHeight;
		}

		hr = mySampleGrabber->SetMediaType(&myAmMediaType);
		if (FAILED(hr))
		{
			// error
			mySrcFilter->Release();
			return;
		}
	}

	myConfigInterface->Release();
	myConfigInterface = 0;


	// Add Capture filter to our graph.
	hr = myGraph->AddFilter(mySrcFilter, L"Video Capture");
	if (FAILED(hr))
	{
		// error
		mySrcFilter->Release();
		return;
	}

	hr = myGraph->AddFilter(mySampleGrabberFilter, LPCWSTR("Sample Grabber"));
	if (FAILED(hr))
	{
		// error
		return;
	}

	mySampleGrabber->SetBufferSamples(TRUE);

	mySampleGrabberCB.SetCamera(this);
	mySampleGrabber->SetCallback(&mySampleGrabberCB, 1);

	IPin *pSrcOut = 0, *pGrabberIn = 0, *pGrabberOut = 0, *pNullIn = 0;
	hr = myCapture->FindPin(mySrcFilter, PINDIR_OUTPUT, &PIN_CATEGORY_CAPTURE, 0, FALSE, 0, &pSrcOut);
	if (FAILED(hr))
	{
		// error
		return; ///LEAK
	}

	hr = myGraph->AddFilter(myNullRenderer, L"NullRender");

	// Render the preview pin on the video capture filter
	// Use this instead of myGraph->RenderFile

	hr = myCapture->RenderStream(
		&PIN_CATEGORY_CAPTURE, // Connect this pin ...
		&MEDIATYPE_Video,    // with this media type ...
		mySrcFilter,                // on this filter ...
		mySampleGrabberFilter,          // to the Sample Grabber ...
		myNullRenderer);              // ... and finally to the Null Renderer.


	if (FAILED(hr))
	{
		// error
		SAFE_RELEASE((IUnknown**)&mySrcFilter);
		return;
	}

	// Now that the filter has been added to the graph and we have
	// rendered its stream, we can release this reference to the filter.
	SAFE_RELEASE((IUnknown**)&mySrcFilter);

	RecomputeAllParams();

	mState = isStopped;
}

void CameraWindows::StartPreview()
{
	// Start previewing video data
	HRESULT hr;
	hr = myMC->Run();
	if (FAILED(hr))
	{
		// error
		KIGS_WARNING("No free camera", 2);
		return;
	}

	mState = isRunning;
}

void CameraWindows::StopPreview()
{
	// Start previewing video data
	HRESULT hr;
	hr = myMC->Stop();
	if (FAILED(hr))
	{
		// error
		KIGS_WARNING("No free camera", 2);
		return;
	}

	mState = isStopped;
}

/////////////////////////////////////////////////////////////////////////////////
HRESULT CameraWindows::FindCaptureDevice(IBaseFilter ** ppSrcFilter)
{
	HRESULT hr = S_OK;
	IBaseFilter * pSrc = NULL;
	IMoniker* pMoniker = NULL;
	ICreateDevEnum *pDevEnum = NULL;
	IEnumMoniker *pClassEnum = NULL;

	if (!ppSrcFilter)
	{
		return E_POINTER;
	}

	// Create the system device enumerator
	hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC,
		IID_ICreateDevEnum, (void **)&pDevEnum);
	if (FAILED(hr))
	{
		//Msg(TEXT("Couldn't create system enumerator!  hr=0x%x"), hr);
	}

	// Create an enumerator for the video capture devices

	if (SUCCEEDED(hr))
	{
		hr = pDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, &pClassEnum, 0);
		if (FAILED(hr))
		{
			//	Msg(TEXT("Couldn't create class enumerator!  hr=0x%x"), hr);
		}
	}

	if (SUCCEEDED(hr))
	{
		// If there are no enumerators for the requested type, then 
		// CreateClassEnumerator will succeed, but pClassEnum will be NULL.
		if (pClassEnum == NULL)
		{
			/*	MessageBox(ghNotification,TEXT("No video capture device was detected.\r\n\r\n")
			TEXT("This sample requires a video capture device, such as a USB WebCam,\r\n")
			TEXT("to be installed and working properly.  The sample will now close."),
			TEXT("No Video Capture Hardware"), MB_OK | MB_ICONINFORMATION);*/
			hr = E_FAIL;
		}
	}

	// Use the first video capture device on the device list.
	// Note that if the Next() call succeeds but there are no monikers,
	// it will return S_FALSE (which is not a failure).  Therefore, we
	// check that the return code is S_OK instead of using SUCCEEDED() macro.

	if (SUCCEEDED(hr))
	{
		hr = pClassEnum->Next(1, &pMoniker, NULL);
		if (hr == S_FALSE)
		{
			//  Msg(TEXT("Unable to access video capture device!"));   
			hr = E_FAIL;
		}
	}

	if (SUCCEEDED(hr))
	{
		// Bind Moniker to a filter object
		hr = pMoniker->BindToObject(0, 0, IID_IBaseFilter, (void**)&pSrc);
		if (FAILED(hr))
		{

		}
	}

	// Copy the found filter pointer to the output parameter.
	if (SUCCEEDED(hr))
	{
		*ppSrcFilter = pSrc;
		(*ppSrcFilter)->AddRef();
	}

	SAFE_RELEASE((IUnknown**)&pDevEnum);
	SAFE_RELEASE((IUnknown**)&pClassEnum);
	SAFE_RELEASE((IUnknown**)&pMoniker);
	SAFE_RELEASE((IUnknown**)&pSrc);



	return hr;
}

//////////////////////////////////////////////////////////////////////////////
HRESULT CameraWindows::GetInterfaces()
{


	HRESULT hr;

	// Create the filter graph
	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC,
		IID_IGraphBuilder, (void **)&myGraph);
	if (FAILED(hr))
		return hr;

	// Create the capture graph builder
	hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, NULL, CLSCTX_INPROC,
		IID_ICaptureGraphBuilder2, (void **)&myCapture);
	if (FAILED(hr))
		return hr;


	// Obtain interfaces for media control and Video Window
	hr = myGraph->QueryInterface(IID_IMediaControl, (LPVOID *)&myMC);
	if (FAILED(hr))
		return hr;

	hr = myGraph->QueryInterface(IID_IVideoWindow, (LPVOID *)&myVW);
	if (FAILED(hr))
		return hr;

	hr = myGraph->QueryInterface(IID_IMediaEventEx, (LPVOID *)&myME);
	if (FAILED(hr))
		return hr;

	// Set the window handle used to process graph events
	//   hr = myME->SetNotifyWindow((OAHWND)ghNotification, WM_GRAPHNOTIFY, 0);


	hr = CoCreateInstance(CLSID_SampleGrabber, NULL, CLSCTX_INPROC, IID_IBaseFilter, (LPVOID *)&mySampleGrabberFilter);
	if (FAILED(hr))
		return hr;


	mySampleGrabberFilter->QueryInterface(IID_ISampleGrabber, (LPVOID *)&mySampleGrabber);

	ZeroMemory(&myAmMediaType, sizeof(myAmMediaType));
	myAmMediaType.majortype = MEDIATYPE_Video;

	if (((std::string)mFormat) == "RGB555")
	{
		myAmMediaType.subtype = MEDIASUBTYPE_RGB555;
	}
	else if (((std::string)mFormat) == "YUV422") //MEDIASUBTYPE_UYVY
	{
		myAmMediaType.subtype = MEDIASUBTYPE_I420;
		myYUVFormat = true; // check know yuv : MEDIASUBTYPE_UYVY , MEDIASUBTYPE_I420, MEDIASUBTYPE_YUY2
	}
	else if (((std::string)mFormat) == "YUV24") //MEDIASUBTYPE_AYUV
	{
		myAmMediaType.subtype = MEDIASUBTYPE_AYUV;
		myYUVFormat = true; // check know yuv : MEDIASUBTYPE_UYVY , MEDIASUBTYPE_I420, MEDIASUBTYPE_YUY2
	}
	else if (((std::string)mFormat) == "RGB24")
	{
		myAmMediaType.subtype = MEDIASUBTYPE_RGB24;
	}
	else if (((std::string)mFormat) == "RGB32")
	{
		myAmMediaType.subtype = MEDIASUBTYPE_RGB32;
	}
	else if (((std::string)mFormat) == "RGB565")
	{
		myAmMediaType.subtype = MEDIASUBTYPE_RGB565;
	}
	else
	{
		// TODO
		KIGS_ERROR("unsupported camera output format", 2);
	}

	myAmMediaType.formattype = FORMAT_VideoInfo;
	//mySampleGrabber->SetMediaType(&myAmMediaType);

	//
	hr = CoCreateInstance(CLSID_NullRenderer, NULL, CLSCTX_INPROC_SERVER, IID_IBaseFilter, (void**)&myNullRenderer);
	if (FAILED(hr))
		return hr;

	return hr;
}


void CameraWindows::AllocateFrameBuffers()
{
	int buffersize = mWidth*mHeight;

	for (int i = 0; i < BUFFER_COUNT; i++)
		mFrameBuffers[i] = std::make_shared<AlignedCoreRawBuffer<16, unsigned char>>(buffersize * mPixelSize, false);
}

void CameraWindows::FreeFrameBuffers()
{
	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		mFrameBuffers[i] = nullptr;
	}
}



#ifdef FFMPEG

/* add a video output stream */
static AVStream * add_video_stream(EncoderInfo& EI, int width, int height)
{
	AVCodecContext *c;
	AVCodec *codec;
	AVStream *st;

	st = avformat_new_stream(EI.oc, 0);
	if (!st) {
		fprintf(stderr, "Could not alloc stream\n");
		return 0;
	}

	c = st->codec;
	c->codec_id = EI.oc->oformat->video_codec;
	c->codec_type = AVMEDIA_TYPE_VIDEO;

	/* put sample parameters */
	c->bit_rate = 400000;
	/* resolution must be a multiple of two */
	//c->width = 352;
	//c->height = 288;

	c->width = width;
	c->height = height;
	/* frames per second */
	c->time_base.num = 1;
	c->time_base.den = 25;
	/* emit one intra frame every ten frames
	* check frame pict_type before passing frame
	* to encoder, if frame->pict_type is AV_PICTURE_TYPE_I
	* then gop_size is ignored and the output of encoder
	* will always be I frame irrespective to gop_size*/

	c->me_range = 16;
	c->qmin = 10;
	c->qmax = 51;

	c->gop_size = 10;
	c->max_b_frames = 1;
	c->pix_fmt = EI.PixFormat;

	// some formats want stream headers to be separate
	if (EI.oc->oformat->flags & AVFMT_GLOBALHEADER)
		c->flags |= CODEC_FLAG_GLOBAL_HEADER;


	/* find the video encoder */
	codec = avcodec_find_encoder(EI.oc->oformat->video_codec);
	if (!codec) {
		fprintf(stderr, "Codec not found\n");
		return 0;
	}


	/* open it */
	if (avcodec_open2(c, codec, NULL) < 0) {
		fprintf(stderr, "Could not open codec\n");
		return 0;
	}

	return st;
}



/*
* Video encoding example
*/
void CameraWindows::InitEncoder(const char *filename)
{
	//AVFormatContext *oc;
	AVOutputFormat *fmt;
	AVStream *audio_st = NULL, *vStream = NULL;

	/*int i, ret, x, y, got_output;
	AVFrame *frame;
	AVPacket pkt;*/

	printf("Encode video file %s\n", filename);

	/* auto detect the output format from the name. default is
	mpeg. */
	fmt = av_guess_format(NULL, filename, NULL);
	if (!fmt) {
		fprintf(stderr, "Could not find suitable output format\n");
		return;
	}

	/* allocate the output media context */
	mEncoderInfo.oc = avformat_alloc_context();
	if (!mEncoderInfo.oc) {
		fprintf(stderr, "Memory error : could not allocate the output media context\n");
		return;
	}

	/* set the media context used format */
	mEncoderInfo.oc->oformat = fmt;

	/* add the audio and video streams using the default format codecs and initialize the codecs */
	if (fmt->video_codec != CODEC_ID_NONE)
		vStream = add_video_stream(mEncoderInfo, mEncoderInfo.width, mEncoderInfo.height);

	// no audio stream for now
	fmt->audio_codec = CODEC_ID_NONE;

	// allocate the video frame
	mEncoderInfo.frame = av_frame_alloc();
	if (!mEncoderInfo.frame) {
		fprintf(stderr, "Could not allocate video frame\n");
		return;
	}

	// set frame parameters
	mEncoderInfo.frame->pts = 0;
	mEncoderInfo.frame->format = vStream->codec->pix_fmt;
	mEncoderInfo.frame->width = vStream->codec->width;
	mEncoderInfo.frame->height = vStream->codec->height;

	// allocate the video frame data
	int ret = av_image_alloc(mEncoderInfo.frame->data, mEncoderInfo.frame->linesize, mEncoderInfo.width, mEncoderInfo.height, vStream->codec->pix_fmt, 32);
	if (ret < 0) {
		fprintf(stderr, "Could not allocate raw picture buffer\n");
		return;
	}

	// dump codec info
	av_dump_format(mEncoderInfo.oc, 0, filename, 1);

	// open the output file
	if (!(fmt->flags & AVFMT_NOFILE)) {
		if (avio_open(&mEncoderInfo.oc->pb, filename, AVIO_FLAG_WRITE) < 0) {
			KIGS_ERROR("Could not open mp4 file output\n", 0);
		}
	}

	/* write the stream header, if any */
	avformat_write_header(mEncoderInfo.oc, NULL);

	mEncoderInfo.pkt = new AVPacket();
}

void CameraWindows::EncodeFrame(double Time, void* data, int dataLen)
{
	AVStream * vStream = mEncoderInfo.oc->streams[0];

	// init packet
	av_init_packet(mEncoderInfo.pkt);
	memcpy(mEncoderInfo.frame->data[0], data, dataLen);
	mEncoderInfo.pkt->size = dataLen;

	// encode the packet
	int got_output;
	int ret = avcodec_encode_video2(vStream->codec, mEncoderInfo.pkt, mEncoderInfo.frame, &got_output);
	mEncoderInfo.frame->pts += av_rescale_q(1, vStream->codec->time_base, vStream->time_base);
	if (ret < 0) {
		fprintf(stderr, "Error encoding frame\n");
		return;
	}

	if (got_output) {
		printf(".");
		//av_interleaved_write_frame(mEncoderInfo.oc, mEncoderInfo.pkt);
		av_write_frame(mEncoderInfo.oc, mEncoderInfo.pkt);
	}

	av_free_packet(mEncoderInfo.pkt);
}

void CameraWindows::CloseEncoder()
{
	AVStream * vStream = mEncoderInfo.oc->streams[0];

	/* get the delayed frames */
	int got_output = 1;
	if ((vStream->codec->codec->capabilities&AV_CODEC_CAP_DELAY) != 0)
	{
		for (; got_output > 0;)
		{
			av_init_packet(mEncoderInfo.pkt);
			mEncoderInfo.pkt->data = NULL;    // packet data will be allocated by the encoder
			mEncoderInfo.pkt->size = 0;

			int ret = avcodec_encode_video2(vStream->codec, mEncoderInfo.pkt, NULL, &got_output);
			mEncoderInfo.frame->pts += av_rescale_q(1, vStream->codec->time_base, vStream->time_base);
			if (ret < 0) {
				fprintf(stderr, "Error encoding frame\n");
				return;
			}

			if (got_output) {
				printf(".");
				//av_interleaved_write_frame(mEncoderInfo.oc, mEncoderInfo.pkt);
				av_write_frame(mEncoderInfo.oc, mEncoderInfo.pkt);
			}
		}
	}

	printf("\nwrite %d images\n", (int)vStream->nb_frames);

	av_free_packet(mEncoderInfo.pkt);

	av_write_trailer(mEncoderInfo.oc);

	if (!(mEncoderInfo.oc->oformat->flags & AVFMT_NOFILE)) {
		/* close the output file */
		avio_close(mEncoderInfo.oc->pb);
	}


	/* free the streams */
	for (unsigned int i = 0; i < mEncoderInfo.oc->nb_streams; i++) {
		av_freep(mEncoderInfo.oc->streams[i]->codec);
		av_freep(mEncoderInfo.oc->streams[i]);
	}

	/* free the stream */
	av_free(mEncoderInfo.oc);

	delete mEncoderInfo.pkt;
}



bool CameraWindows::StartRecording(std::string & name)
{

	SmartPointer<FileHandle> file = Platform_FindFullName(name);
	const char * filename = file != 0 ? file->myFullFileName.c_str() : name.c_str();


	/* initialize libavcodec, and register all codecs and formats */
	av_register_all();


	mEncoderInfo.width = myCaptureX;
	mEncoderInfo.height = myCaptureY;
	switch (mFormat)
	{
	case 0: // RGB555
		mEncoderInfo.PixFormat = AV_PIX_FMT_RGB555;
		break;
	case 1: // YUV422
		mEncoderInfo.PixFormat = AV_PIX_FMT_YUV420P;
		break;
	case 2: // RGB24
		mEncoderInfo.PixFormat = AV_PIX_FMT_RGB24;
		break;
	case 3: // YUV24
		mEncoderInfo.PixFormat = AV_PIX_FMT_YUV420P;
		break;
	default:
		printf("Recording failed : BAD FORMAT\n");
		mIsRecording = false;
		return false;
	}

	InitEncoder(filename);

	mIsRecording = true;

	return true;
}

void CameraWindows::StopRecording()
{
	mIsRecording = false;
	mNeedCloseEncoder = true;
}
#endif