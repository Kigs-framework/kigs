#include "PrecompiledHeaders.h"

#include "CameraAndroid.h"
#include "Core.h"
#include "FilePathManager.h"

// only those function are visibles from outside of the dll
#define KIGS_JNIEXPORT	__attribute__((visibility("default")))

CameraAndroid*	CurrentCamera=0;

// Image format from android API
class ImageFormat {
public:
   	const static int JPEG = 256;
	const static int NV16 = 16;
	const static int NV21 = 17;
	const static int RAW10 = 37;
	const static int RAW_SENSOR = 32;
	const static int RGB_888 = 41;
	const static int RGB_565 = 4;
	const static int UNKNOWN = 0;
	const static int YUV_420_888 = 35;
	const static int YUV_422_888 = 39;
	const static int YUY2 = 20;
	const static int YV12 = 842094169;
};

extern "C" {

	KIGS_JNIEXPORT void JNICALL Java_com_kigs_cameramanager_CameraManager_CameraCallback(JNIEnv * env, jobject obj, int width, int height, jbyteArray pixels);
	KIGS_JNIEXPORT void JNICALL Java_com_kigs_cameramanager_CameraManager_nativeSetPictureToKigs(JNIEnv* env, jobject thiz,jbyteArray data, int SizeX, int SizeY);

	void Java_com_kigs_cameramanager_CameraManager_nativeSetPictureToKigs(JNIEnv* env, jobject thiz,jbyteArray data, int SizeX, int SizeY)
	{
		if ( CurrentCamera )
		{
			CurrentCamera->PictureReceived(env, thiz, SizeX, SizeY, data);
		}
	}
}

IMPLEMENT_CLASS_INFO(CameraAndroid)

//! constructor
CameraAndroid::CameraAndroid(const kstl::string& name,CLASS_NAME_TREE_ARG) : GenericCamera(name,PASS_CLASS_NAME_TREE_ARG)
, myConversionMethod(0)
, myFrameCropMethod(0)
{
	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());

	jclass pMaClasse =g_env->FindClass("com/kigs/cameramanager/CameraManager");
	myJavaCameraManager = (jclass)g_env->NewGlobalRef(pMaClasse);
	
	StartMethod = g_env->GetStaticMethodID(myJavaCameraManager, "start", "([B)Z");
	StopMethod = g_env->GetStaticMethodID(myJavaCameraManager, "stop", "()V");
	/*StartRecordMethod = g_env->GetStaticMethodID(myJavaCameraManager, "startRecording", "(Ljava/lang/String;)Z");
	StopRecordMethod = g_env->GetStaticMethodID(myJavaCameraManager, "stopRecording", "()V");
	GetFOVMethod = g_env->GetStaticMethodID(myJavaCameraManager, "getFOV", "()F");
	SetWhiteBalanceMethod = g_env->GetStaticMethodID(myJavaCameraManager, "setWhiteBalance", "(I)V");
	GetSupportedWhiteBalanceCountMethod = g_env->GetStaticMethodID(myJavaCameraManager, "getSupportedWhiteBalanceCount", "()I");*/

	CurrentCamera=this;
}     

//! destructor
CameraAndroid::~CameraAndroid()
{
	if(mFrameBuffers[0])
	{
		FreeFrameBuffers();
	}	

	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());

	//g_env->CallStaticVoidMethod(myJavaCameraManager, StopRecordMethod);
	g_env->CallStaticVoidMethod(myJavaCameraManager,  StopMethod);
	g_env->DeleteGlobalRef(myJavaCameraManager);
	myJavaCameraManager=0;
	
	CurrentCamera=0;
	
}   

void CameraAndroid::StopRecording(){
	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
	g_env->CallStaticVoidMethod(myJavaCameraManager, StopRecordMethod);
} 

bool CameraAndroid::StartRecording(kstl::string& path){
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
	//JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
	//g_env->CallStaticVoidMethod(myJavaCameraManager, SetWhiteBalanceMethod, i);
}

int CameraAndroid::GetSupportedWhiteBalanceCount()
{
	//JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
	//return g_env->CallStaticIntMethod(myJavaCameraManager, GetSupportedWhiteBalanceCountMethod);
	return 0;
}

void CameraAndroid::InitModifiable()
{
	GenericCamera::InitModifiable();

	mState = isStopped;
	
	// format
	int format = -1;
	if (((kstl::string)mFormat) == "RGB555")
	{
		format = ImageFormat::YUV_420_888;
	}
	else if(((kstl::string)mFormat) == "YUV422")
	{
		format = ImageFormat::YUV_422_888;
	}
	else if(((kstl::string)mFormat) == "RGB24")
	{
		format = ImageFormat::RGB_888;
	}
	else if(((kstl::string)mFormat) == "YUV24")
	{
		format = ImageFormat::YUV_420_888;
	}
	
	if(format == -1)
	{
		printf("ERROR Unmanaged format (%s)\n", ((kstl::string)mFormat).c_str());
		return;
	}
	
	// Init android camera
	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
	jmethodID  method= g_env->GetStaticMethodID(myJavaCameraManager, "initCameraManager", "()V");
	g_env->CallStaticVoidMethod(myJavaCameraManager,  method);

	// first choice is init res
	int WantedResX = mInitResX;
	int WantedResY = mInitResY;

	// if init res was not set, use width / height
	if ((WantedResX == 0) || (WantedResY == 0))
	{
		WantedResX = mWidth;
		WantedResY = mHeight;
	}

	// set the wanted resolution
	jmethodID  getinfo= g_env->GetStaticMethodID(myJavaCameraManager, "SelectFormatAndSize", "(III)V");
	g_env->CallStaticVoidMethod(myJavaCameraManager, getinfo, format, WantedResX, WantedResY);

	// get the used resolution (may change from wanted)
	getinfo= g_env->GetStaticMethodID(myJavaCameraManager, "getCameraPreviewW", "()I");
	int w=g_env->CallStaticIntMethod(myJavaCameraManager,  getinfo);
	
	getinfo= g_env->GetStaticMethodID(myJavaCameraManager, "getCameraPreviewH", "()I");
	int h=g_env->CallStaticIntMethod(myJavaCameraManager,  getinfo);

	WantedResX = mInitResX = w;
	WantedResY = mInitResY = h;

	if ((mWidth == 0) || (mHeight == 0))
	{
		mWidth = mInitResX;
		mHeight = mInitResY;
	}


	getinfo= g_env->GetStaticMethodID(myJavaCameraManager, "getCameraPreviewFormat", "()I");
	int f=g_env->CallStaticIntMethod(myJavaCameraManager,  getinfo);

	char	tmp[1024];
	sprintf(tmp,"Camera base params are w:%d h:%d f:%d\n",w,h,f);	
	printf("%s\n", tmp);
	KIGS_MESSAGE(tmp);

	//mFormat : "RGB555","YUV422","RGB24","YUV24"	
	// check input format and set conversion method
	switch(f)
	{
		case ImageFormat::NV21:
		switch(mFormat)
		{
			case 0:
			myConversionMethod = 0;
			break;
			case 1:
			myConversionMethod = &ConvertFormat_420SPToYUV422;
			myFrameCropMethod = &CropBuffer_420SP;
			break;
			case 2:
			myConversionMethod = &ConvertFormat_420SPToRGB24;
			myFrameCropMethod = &CropBuffer_420SP;
			break;
			case 3:
			myConversionMethod = &ConvertFormat_420SPToYUV24;
			myFrameCropMethod = &CropBuffer_420SP;
			break;
			
			default:
			printf("error : UNKNOW output format");
		}		
		// alloc tmp buffer
		tmpGrabSize = (WantedResX * WantedResY * 12) / 8;
		break;
		
		case ImageFormat::YUV_420_888:
		switch(mFormat)
		{
			case 0:
			myConversionMethod = NULL;
			break;
			case 1:
			myConversionMethod = NULL;
			break;
			case 2:
			myConversionMethod = NULL;
			break;
			case 3:
			myConversionMethod = NULL;
			break;
			
			default:
			printf("error : UNKNOW output format");
		}		
		// alloc tmp buffer
		tmpGrabSize = (WantedResX * WantedResY * 12) / 8;
		break;
		
		case ImageFormat::RGB_565:
		switch(mFormat)
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
		
		// alloc tmp buffer
		tmpGrabSize = WantedResX * WantedResY * 2;
		break;
		
		default:
			printf("error : UNMANAGED input format");		
	}
	
	if(myConversionMethod==NULL)
		printf("camera : no conversion method set");
		
	RecomputeAllParams();
	
	jbyteArray jBuff = g_env->NewByteArray(mWidth*mHeight);
	
	bool hasStarted=false;
	while(hasStarted==false)	
		hasStarted = g_env->CallStaticBooleanMethod(myJavaCameraManager,  StartMethod, jBuff);
	
	mState = isRunning;
}

void CameraAndroid::AllocateFrameBuffers()
{	
	int buffersize = mWidth*mHeight;


	mFrameBuffers[0] = OwningRawPtrToSmartPtr(new AlignedCoreRawBuffer<16, unsigned char>(buffersize*mPixelSize, false));
	mFrameBuffers[1] = OwningRawPtrToSmartPtr(new AlignedCoreRawBuffer<16, unsigned char>(buffersize*mPixelSize, false));
	mFrameBuffers[2] = OwningRawPtrToSmartPtr(new AlignedCoreRawBuffer<16, unsigned char>(buffersize*mPixelSize, false));
}

void CameraAndroid::FreeFrameBuffers()
{
	mFrameBuffers[0] = nullptr;
	mFrameBuffers[1] = nullptr;
	mFrameBuffers[2] = nullptr;
}

void	CameraAndroid::BufferReceived(JNIEnv*  env, jobject thiz, int width, int height, jbyteArray pixels)
{
	if(tmpGrabSize == 0)
		KIGS_ERROR("Camera bad init", 0);
	
	int buffertoset=GetFreeBuffer();
	if (buffertoset == -1)
	{
		return;
	}

	SetBufferState(buffertoset,ProcessingBuffer);

	//env->GetByteArrayRegion(pixels, 0, tmpGrabSize, (jbyte*)tmpGrabBuffer);

	if (NeedCrop())
	{
		if (myFrameCropMethod != NULL)
		{
			//long BufferLen = tmpGrabSize;
			//myFrameCropMethod(tmpGrabBuffer, BufferLen, mInitResX, mInitResY, myCaptureX, myCaptureY, myOffsetX, myOffsetY);
		}
	}

	//if(myConversionMethod!=NULL)
	//	myConversionMethod(tmpGrabBuffer, mFrameBuffers[buffertoset]->buffer(), myCaptureX, myCaptureY);
	
	
	SetBufferState(buffertoset,ReadyBuffer);
}

void	CameraAndroid::PictureReceived(JNIEnv*  env, jobject thiz, int width, int height, jbyteArray pixels)
{
	/*this->setValue(_S_2_ID("PictureBufferSizeX"),width/2);
	this->setValue(_S_2_ID("PictureBufferSizeY"),height/2);

	int size = width * height * 4;

	//Allocation of my picture buffer
	unsigned char *myPictureBuffer2 = new unsigned char[(width/2)*(height/2)*myPixelSize];

	unsigned char*	tmpPictureBuffer = new unsigned char[size];

	env->GetByteArrayRegion(pixels, 0, size, (jbyte*)tmpPictureBuffer);

	//convertRGBA8888_To_RGBA5551((unsigned short*)myPictureBuffer2,tmpPictureBuffer,width,height);

	myPictureBuffer = myPictureBuffer2;
	delete[] tmpPictureBuffer;*/
}

extern "C"
{
	/**
	* this method is called by java code to init width, height and pixels data
	*/
	void Java_com_kigs_cameramanager_CameraManager_CameraCallback(JNIEnv*  env, jobject thiz, int width, int height, jbyteArray pixels)
	{
		if(CurrentCamera)
		{
			CurrentCamera->BufferReceived(env, thiz, width, height, pixels);
		}
	}
}