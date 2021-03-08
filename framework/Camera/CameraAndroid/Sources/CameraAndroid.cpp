#include "PrecompiledHeaders.h"

#include "CameraAndroid.h"
#include "Core.h"
#include "FilePathManager.h"
#include "Remotery.h"

// only those function are visibles from outside of the dll
#define KIGS_JNIEXPORT	__attribute__((visibility("default")))

CameraAndroid*	CurrentCamera = 0;

// Image format from android API
class ImageFormat {
public:
	const static int JPEG = 256;
	const static int NV16 = 16;
	const static int NV21 = 17;
	const static int RAW10 = 37;
	const static int RAW_SENSOR = 32;
	const static int RGB_565 = 4;
	const static int UNKNOWN = 0;
	const static int YUV_420_888 = 35;
	const static int YUY2 = 20;
	const static int YV12 = 842094169;
};

extern "C" {
	KIGS_JNIEXPORT void JNICALL Java_com_kigs_cameramanager_CameraManager_CameraCallback(JNIEnv * env, jobject obj, int width, int height, jbyteArray pixels);
	void Java_com_kigs_cameramanager_CameraManager_CameraCallback(JNIEnv*  env, jobject thiz, int width, int height, jbyteArray pixels)
	{
		if (CurrentCamera)
		{
			CurrentCamera->BufferReceived(env, thiz, width, height, pixels);
		}
	}

	KIGS_JNIEXPORT void JNICALL Java_com_kigs_cameramanager_CameraManager_nativeSetPictureToKigs(JNIEnv* env, jobject thiz, jbyteArray data, int SizeX, int SizeY);
	void Java_com_kigs_cameramanager_CameraManager_nativeSetPictureToKigs(JNIEnv* env, jobject thiz, jbyteArray data, int SizeX, int SizeY)
	{
		if (CurrentCamera)
		{
			CurrentCamera->PictureReceived(env, thiz, SizeX, SizeY, data);
		}
	}
}

IMPLEMENT_CLASS_INFO(CameraAndroid)

//! constructor
CameraAndroid::CameraAndroid(const kstl::string& name, CLASS_NAME_TREE_ARG) : GenericCamera(name, PASS_CLASS_NAME_TREE_ARG)
, myConversionMethod(0)
, myFrameCropMethod(0)
{
	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());

	jclass pMaClasse = g_env->FindClass("com/kigs/cameramanager/CameraManager");
	myJavaCameraManager = (jclass)g_env->NewGlobalRef(pMaClasse);

	StartMethod = g_env->GetStaticMethodID(myJavaCameraManager, "start", "()V");
	StopMethod = g_env->GetStaticMethodID(myJavaCameraManager, "stop", "()V");
	StartRecordMethod = g_env->GetStaticMethodID(myJavaCameraManager, "startRecording", "(Ljava/lang/String;)Z");
	StopRecordMethod = g_env->GetStaticMethodID(myJavaCameraManager, "stopRecording", "()V");
	GetFOVMethod = g_env->GetStaticMethodID(myJavaCameraManager, "getFOV", "()F");
	SetWhiteBalanceMethod = g_env->GetStaticMethodID(myJavaCameraManager, "setWhiteBalance", "(I)V");
	GetSupportedWhiteBalanceCountMethod = g_env->GetStaticMethodID(myJavaCameraManager, "getSupportedWhiteBalanceCount", "()I");

	CurrentCamera = this;
}

//! destructor
CameraAndroid::~CameraAndroid()
{
	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
	g_env->CallStaticVoidMethod(myJavaCameraManager, StopMethod);
	g_env->DeleteGlobalRef(myJavaCameraManager);
	myJavaCameraManager = 0;

	CurrentCamera = 0;

	FreeFrameBuffers();
}

void CameraAndroid::StartPreview()
{
	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
	g_env->CallStaticVoidMethod(myJavaCameraManager, StartMethod);
	mState = isRunning;
}

void CameraAndroid::StopPreview()
{
	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
	g_env->CallStaticVoidMethod(myJavaCameraManager, StopMethod);
	mState = isStopped;
}

void CameraAndroid::StopRecording()
{
	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
	g_env->CallStaticVoidMethod(myJavaCameraManager, StopRecordMethod);
}

bool CameraAndroid::StartRecording(kstl::string & path)
{
	SmartPointer<FileHandle> file = Platform_FindFullName(path);
	const char * filename = file->mFullFileName.c_str();

	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
	jstring jId = g_env->NewStringUTF(filename);
	bool b = g_env->CallStaticBooleanMethod(myJavaCameraManager, StartRecordMethod, jId);
	g_env->DeleteLocalRef(jId);

	return b;
}


void CameraAndroid::SetWhiteBalance(int i)
{
	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
	g_env->CallStaticVoidMethod(myJavaCameraManager, SetWhiteBalanceMethod, i);
}

int CameraAndroid::GetSupportedWhiteBalanceCount()
{
	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
	return g_env->CallStaticIntMethod(myJavaCameraManager, GetSupportedWhiteBalanceCountMethod);
}

void CameraAndroid::InitModifiable()
{
	GenericCamera::InitModifiable();

	mState = isStopped;

	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());

	jmethodID  method = g_env->GetStaticMethodID(myJavaCameraManager, "initCameraManager", "()V");
	g_env->CallStaticVoidMethod(myJavaCameraManager, method);

	// first choice is init res
	int WantedResX = mInitResX;
	int WantedResY = mInitResY;

	// if init res was not set, use width / height
	if ((WantedResX == 0) || (WantedResY == 0))
	{
		WantedResX = mWidth;
		WantedResY = mHeight;
	}
	
	method = g_env->GetStaticMethodID(myJavaCameraManager, "SetCameraPreviewSize", "(IIZ)V");
	g_env->CallStaticVoidMethod(myJavaCameraManager, method, WantedResX, WantedResY, false);

	// retreive info from init camera
	method = g_env->GetStaticMethodID(myJavaCameraManager, "getCameraPreviewW", "()I");
	int w = g_env->CallStaticIntMethod(myJavaCameraManager, method);

	method = g_env->GetStaticMethodID(myJavaCameraManager, "getCameraPreviewH", "()I");
	int h = g_env->CallStaticIntMethod(myJavaCameraManager, method);

	WantedResX = mInitResX = w;
	WantedResY = mInitResY = h;

	if ((mWidth == 0) || (mHeight == 0))
	{
		mWidth = mInitResX;
		mHeight = mInitResY;
	}

	
	method = g_env->GetStaticMethodID(myJavaCameraManager, "askCameraPreviewFormat", "(I)I");
	int f = g_env->CallStaticIntMethod(myJavaCameraManager, method,4);
	
	printf("Camera base params are w:%d h:%d f:%d\n", w, h, f);

	method = g_env->GetStaticMethodID(myJavaCameraManager, "getCameraPreviewBytePerPix", "()I");
	float bpp = g_env->CallStaticIntMethod(myJavaCameraManager, method) / 8.0f;
	tmpGrabSize = bpp*WantedResX * WantedResY;

	//mFormat : "RGB555","RGB565","YUV422","RGB24","RGB32","YUV24"
	// check input format and set conversion method
	switch (f)
	{
	case ImageFormat::NV21:
		switch (mFormat)
		{
		case 0:
			//myConversionMethod = &ConvertFormat_420SPToRGB555;
			myFrameCropMethod = &CropBuffer_420SP;
			break;
		case 1:
			myConversionMethod = &ConvertFormat_420SPToRGB565;
			myFrameCropMethod = &CropBuffer_420SP;
			break;
		case 2:
			myConversionMethod = &ConvertFormat_420SPToYUV422;
			myFrameCropMethod = &CropBuffer_420SP;
			break;
		case 3:
			myConversionMethod = &ConvertFormat_420SPToRGB24;
			myFrameCropMethod = &CropBuffer_420SP;
			break;
		case 4:
			myConversionMethod = &ConvertFormat_420SPToRGB32;
			myFrameCropMethod = &CropBuffer_420SP;
			break;
		case 5:
			myConversionMethod = &ConvertFormat_420SPToYUV24;
			myFrameCropMethod = &CropBuffer_420SP;
			break;

		default:
			printf("error : UNKNOW output format");
		}

		break;
	case ImageFormat::RGB_565:
		switch (mFormat)
		{
		case 0:
			myConversionMethod = 0;
			break;
		case 1:
			myConversionMethod = 0;
			break;
		case 2:
			myConversionMethod = 0;
			break;
		case 3:
			myConversionMethod = 0;
			break;

		default:
			printf("error : UNKNOW output format");
		}

		break;

	default:
		printf("error : UNMANAGED input format");
	}

	if (myConversionMethod == NULL)
		printf("camera : no conversion method set");

	RecomputeAllParams();

	mFOV = (kfloat)g_env->CallStaticFloatMethod(myJavaCameraManager, GetFOVMethod);

	//g_env->CallStaticVoidMethod(myJavaCameraManager,  StartMethod);
	mState = isStopped;
}

void CameraAndroid::AllocateFrameBuffers()
{
	std::unique_lock<std::recursive_mutex> lk(mBuffersMutex);
	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
	int buffersize = mWidth*mHeight;
	jmethodID method = g_env->GetStaticMethodID(myJavaCameraManager, "setCameraBuffer", "([B)V");

	for (int i = 0; i < BUFFER_COUNT; i++)
	{
		mFrameBuffers[i] = OwningRawPtrToSmartPtr(new AlignedCoreRawBuffer<16, unsigned char>(buffersize*mPixelSize, false));

		myGlobalRefBufferPtr[i] = (jbyteArray)g_env->NewGlobalRef(g_env->NewByteArray(tmpGrabSize));
		
		g_env->CallStaticVoidMethod(myJavaCameraManager, method, myGlobalRefBufferPtr[i]);
	}
}

void CameraAndroid::FreeFrameBuffers()
{
	std::unique_lock<std::recursive_mutex> lk(mBuffersMutex);
	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
	for (int i = 0; i < BUFFER_COUNT; i++)
	{

		mFrameBuffers[i] = nullptr;

		g_env->DeleteGlobalRef(myGlobalRefBufferPtr[i]);
		myGlobalRefBufferPtr[i] = 0;
	}

}

void	CameraAndroid::BufferReceived(JNIEnv*  env, jobject thiz, int width, int height, jbyteArray pixels)
{
	rmt_ScopedCPUSample(CameraAndroidBufferReceived, 0);
	
	
	if (tmpGrabSize == 0)
		KIGS_ERROR("Camera bad init", 0);


	mFrameCounter++;

	if ((mFrameCounter % ((int)mSkippedFrameCount+1)) != 0)
	{
		return;
	}
	
	mFrameCounter = 0;
	int buffertoset;
	{
		std::unique_lock<std::recursive_mutex> lk(mBuffersMutex);
		buffertoset = GetFreeBuffer();
		if (buffertoset == -1)
		{
			buffertoset = GetReadyBufferIndex();
			if (buffertoset == -1)
				return;
		}

		SetBufferState(buffertoset, ProcessingBuffer);
	}
	//env->GetByteArrayRegion(pixels, 0, tmpGrabSize, (jbyte*)tmpGrabBuffer);
	unsigned char * tmpGrabBuffer = (unsigned char *)env->GetByteArrayElements(pixels, 0);

	if (NeedCrop())
	{
		if (myFrameCropMethod != NULL)
		{
			long BufferLen = tmpGrabSize;
			myFrameCropMethod(tmpGrabBuffer, BufferLen, mInitResX, mInitResY, mWidth, mHeight, mOffsetX, mOffsetY);
		}
	}

	if (myConversionMethod != NULL)
		myConversionMethod(tmpGrabBuffer, mFrameBuffers[buffertoset]->buffer(), mWidth, mHeight);

	env->ReleaseByteArrayElements(pixels, (jbyte*)tmpGrabBuffer, JNI_ABORT);
	
	{
		std::unique_lock<std::recursive_mutex> lk(mBuffersMutex);
		SetBufferState(buffertoset, ReadyBuffer);
	}
}

void	CameraAndroid::PictureReceived(JNIEnv*  env, jobject thiz, int width, int height, jbyteArray pixels)
{
	/*this->setValue(_S_2_ID("PictureBufferSizeX"),width/2);
	this->setValue(_S_2_ID("PictureBufferSizeY"),height/2);

	int size = width * height * 4;

	//Allocation of my picture buffer
	unsigned char *myPictureBuffer2 = new unsigned char[(width/2)*(height/2)*mPixelSize];

	unsigned char*	tmpPictureBuffer = new unsigned char[size];

	env->GetByteArrayRegion(pixels, 0, size, (jbyte*)tmpPictureBuffer);

	//convertRGBA8888_To_RGBA5551((unsigned short*)myPictureBuffer2,tmpPictureBuffer,width,height);

	myPictureBuffer = myPictureBuffer2;
	delete[] tmpPictureBuffer;*/
}