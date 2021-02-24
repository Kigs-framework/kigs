#ifndef _CAMERAWINDOWS_H_
#define _CAMERAWINDOWS_H_


//class	IBaseFilter;
//class	IVideoWindow;
//class	IMediaControl;
//class	IMediaEventEx;
//class	IGraphBuilder;
//class	ICaptureGraphBuilder2;
//class	ISampleGrabber;
class	CameraWindows;

#include "CoreModifiable.h"
#include "GenericCamera.h"
#include <control.h> 
#include "qedit.h"


// ****************************************
// * CameraWindows class
// * --------------------------------------
/*!  \class CameraWindows
manage a camera
*/
// ****************************************
class SampleGrabberCallback : public ISampleGrabberCB
{
public:
	void	SetCamera(CameraWindows* cam)
	{
		myCamera = cam;
	}

	// Fake referance counting.
	STDMETHODIMP_(ULONG) AddRef() { return 1; }
	STDMETHODIMP_(ULONG) Release() { return 2; }

	STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject);
	STDMETHODIMP SampleCB(double Time, IMediaSample *pSample)
	{
		return E_NOTIMPL;
	}

	STDMETHODIMP BufferCB(double Time, BYTE *pBuffer, long BufferLen);

protected:

	CameraWindows*	myCamera;
};

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

class CameraWindows : public GenericCamera
{
public:
	friend class SampleGrabberCallback;

	DECLARE_CLASS_INFO(CameraWindows, GenericCamera, CameraModule)

	//! constructor
	CameraWindows(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	virtual void	Process()
	{
		// nothing to do, everithing is done in the callback
	}

	virtual void StartPreview();
	virtual void StopPreview();

#ifdef FFMPEG
	virtual bool StartRecording(kstl::string &);
	virtual void StopRecording();
#endif

	// crop directly in input buffer
	void	DoCrop(BYTE *pBuffer, long &BufferLen);

protected:

	bool			isConnected();

	virtual	void	InitModifiable();

	HRESULT GetInterfaces();
	HRESULT FindCaptureDevice(IBaseFilter ** ppSrcFilter);

	void	BufferReceived(double Time, BYTE *pBuffer, long BufferLen);

	virtual	void AllocateFrameBuffers();
	virtual	void FreeFrameBuffers();
#ifdef FFMPEG
	void InitEncoder(const char *filename);
	void EncodeFrame(double Time, void* data, int dataLen);
	void CloseEncoder();
#endif
	//! destructor
	virtual ~CameraWindows();

	IVideoWindow  *			myVW;
	IMediaControl *			myMC;
	IMediaEventEx *			myME;
	IGraphBuilder *			myGraph;
	IBaseFilter *			mySampleGrabberFilter;

	IBaseFilter *			mySrcFilter;
	ICaptureGraphBuilder2 * myCapture;
	ISampleGrabber *		mySampleGrabber;
	IBaseFilter *			myNullRenderer;
	IAMStreamConfig*		myConfigInterface;
#ifdef FFMPEG
	EncoderInfo				mEncoderInfo;
#endif
	SampleGrabberCallback	mySampleGrabberCB;

	AM_MEDIA_TYPE			myAmMediaType;

	unsigned int			myRejectFrames;


	typedef void(*FrameConvertionMethod)(void* bufferin, void* bufferout, int sx, int sy);

	bool	myYUVFormat;
	bool	mIsRecording;
	bool	mNeedCloseEncoder;

	bool	myIsUpsideDown;

	FrameConvertionMethod		myConversionMethod;

	typedef void(*FrameCropMethod)(void* bufferin, long& bufferlen, int sxin, int syin, int sxout, int syout, int offetx, int offsety);
	FrameCropMethod				myFrameCropMethod;
};
#endif //_CAMERAWINDOWS_H_
