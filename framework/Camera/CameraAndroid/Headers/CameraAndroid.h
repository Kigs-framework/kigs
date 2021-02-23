#ifndef _CAMERAANDROID_H_
#define _CAMERAANDROID_H_

#include "CoreModifiable.h"
#include "GenericCamera.h"

#include <mutex>

// ****************************************
// * CameraAndroid class
// * --------------------------------------
/*!  \class CameraAndroid
     manage a camera
*/
// ****************************************

#include <jni.h>

class CameraAndroid : public GenericCamera
{
public:
  	DECLARE_CLASS_INFO(CameraAndroid,GenericCamera,CameraModule)

	//! constructor
    CameraAndroid(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	
	void	Process() override
	{
		// nothing to do, everithing is done in the callback
	}
	
	void	BufferReceived(JNIEnv*  env, jobject thiz, int width, int height, jbyteArray pixels);
	void	PictureReceived(JNIEnv*  env, jobject thiz, int width, int height, jbyteArray pixels);

	void StopRecording() override;
	bool StartRecording(kstl::string & path) override;
	void SetWhiteBalance(int i) override;
	int GetSupportedWhiteBalanceCount() override;

	void StartPreview() override;
	void StopPreview() override;
	
protected:	
	void	InitModifiable() override;

	//! destructor
    virtual ~CameraAndroid();
    
	jclass		myJavaCameraManager;
	jmethodID 	StartMethod;
	jmethodID 	StopMethod;
	jmethodID   StartRecordMethod;
	jmethodID   StopRecordMethod;
	jmethodID	GetFOVMethod;
	jmethodID	SetWhiteBalanceMethod;
	jmethodID	GetSupportedWhiteBalanceCountMethod;
	
	void AllocateFrameBuffers() override;
	void FreeFrameBuffers() override;

	unsigned int 	myLumOffset;
	unsigned int	tmpGrabSize;
	void * 			myPictureBuffer;

	jbyteArray      myGlobalRefBufferPtr[BUFFER_COUNT];

	typedef void(*FrameConvertionMethod)(void* bufferin, void* bufferout, int sx, int sy);
	FrameConvertionMethod		myConversionMethod;
	typedef void(*FrameCropMethod)(void* bufferin, long& bufferlen, int sxin, int syin, int sxout, int syout, int offetx, int offsety);
	FrameCropMethod				myFrameCropMethod;
};

#endif //_CAMERAANDROID_H_
