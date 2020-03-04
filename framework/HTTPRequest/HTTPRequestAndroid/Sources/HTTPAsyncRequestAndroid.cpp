#include "HTTPAsyncRequestAndroid.h"
#include "HTTPConnectAndroid.h"
#include "Core.h"
#include "NotificationCenter.h"
#include "CoreRawBuffer.h"

IMPLEMENT_CLASS_INFO(HTTPAsyncRequestAndroid)

int HTTPAsyncRequestAndroid::requestIndex = 0;
kstl::map<int, HTTPAsyncRequestAndroid*> HTTPAsyncRequestAndroid::requestMap;

//! constructor
HTTPAsyncRequestAndroid::HTTPAsyncRequestAndroid(const kstl::string& name,CLASS_NAME_TREE_ARG) : HTTPAsyncRequest(name,PASS_CLASS_NAME_TREE_ARG)
{
}

//! destructor
HTTPAsyncRequestAndroid::~HTTPAsyncRequestAndroid()
{
	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
	
	g_env->DeleteGlobalRef( myRequest );
	myRequest = 0;
}    

void HTTPAsyncRequestAndroid::InitModifiable()
{	
	HTTPAsyncRequest::InitModifiable();
	
	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());

	myCls = KigsJavaIDManager::findClass("com/assoria/httprequest/AsyncHTTPRequest");	
	query_method = g_env->GetStaticMethodID(myCls, "Query", "(Ljava/lang/String;ZI)Lcom/assoria/httprequest/AsyncHTTPRequest;");
	getsize_method = g_env->GetMethodID(myCls, "GetSize",  "()I");	
	getcontent_enc_method = g_env->GetMethodID(myCls, "GetContentEncoding", "()Ljava/lang/String;");
	getcontent_dec_method = g_env->GetMethodID(myCls, "GetCharsetEncoding", "()Ljava/lang/String;");
	getresult_method = g_env->GetMethodID(myCls, "GetResult", "()[B");

	char url[1024];


	sprintf(url,"%s%s", ((HTTPConnect*)((CoreModifiable*)myConnection))->getHostNameWithProtocol().c_str(),myRequestURL.c_str());
	
	bool isSync = false;
	((CoreModifiable*)myConnection)->getValue(LABEL_TO_ID(IsSynchronous),isSync);
	
		
	int hdl = 0;
	if(!isSync)
		hdl = HTTPAsyncRequestAndroid::requestIndex++;

	jstring jFilename = g_env->NewStringUTF(url);
	myRequest = g_env->CallStaticObjectMethod(myCls, query_method, jFilename, !isSync, hdl);
	myRequest = g_env->NewGlobalRef(myRequest);
	g_env->DeleteLocalRef(jFilename);
	
	if(isSync)
	{
		kstl::string content;
		jstring jcontent = (jstring)g_env->CallObjectMethod(myRequest, getcontent_enc_method);
		KigsJavaIDManager::convertJstringToKstlString(jcontent,content);
		g_env->DeleteLocalRef(jcontent);
		
		if(content == "utf-8")
		{
			myContentEncoding = UTF8;
		}
		else if(content == "utf-16")
		{
			myContentEncoding = UTF16;
		}
		else
		{
			myContentEncoding = ANSI;
		}
		
		kstl::string charset;
		jstring jcharset = (jstring)g_env->CallObjectMethod(myRequest, getcontent_dec_method);
		KigsJavaIDManager::convertJstringToKstlString(jcharset,charset);
		g_env->DeleteLocalRef(jcharset);
		
		if(charset == "utf-8")
		{
			myFoundCharset = UTF8;
		}
		else if(charset == "utf-16")
		{
			myFoundCharset = UTF16;
		}
		else
		{
			myFoundCharset = ANSI;
		}
		
		//printf("content :%d; charset:%d\n", myContentEncoding, myFoundCharset);

		GetData();
	}
	else
	{
		requestMap[hdl] = this;
	}
}

void HTTPAsyncRequestAndroid::GetData()
{
	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
	unsigned int ReceivedRawBufferSize = g_env->CallIntMethod(myRequest, getsize_method);
	jbyteArray tmp = (jbyteArray) g_env->CallObjectMethod(myRequest, getresult_method);


	unsigned char* ReceivedRawBuffer = new unsigned char[ReceivedRawBufferSize*2 + 2];// add two zero
	ReceivedRawBuffer[ReceivedRawBufferSize] = 0;
	ReceivedRawBuffer[ReceivedRawBufferSize + 1] = 0;

	g_env->GetByteArrayRegion (tmp, 0, ReceivedRawBufferSize, reinterpret_cast<jbyte*>(ReceivedRawBuffer));
	g_env->DeleteLocalRef(tmp);

	myReceivedBuffer = new CoreRawBuffer(ReceivedRawBuffer, ReceivedRawBufferSize);


	setDone();
}

void HTTPAsyncRequestAndroid::protectedProcess()
{
	HTTPAsyncRequest::protectedProcess();
}


// only those function are visibles from outside of the dll
#define KIGS_JNIEXPORT	__attribute__((visibility("default")))

extern "C" {
	KIGS_JNIEXPORT void JNICALL Java_com_assoria_httprequest_AsyncHTTPRequest_RequestEnded(JNIEnv * env, jobject obj, jint handle, jboolean result);
}

KIGS_JNIEXPORT void JNICALL Java_com_assoria_httprequest_AsyncHTTPRequest_RequestEnded(JNIEnv * env, jobject obj, jint handle, jboolean result)
{
	HTTPAsyncRequestAndroid * ptr = HTTPAsyncRequestAndroid::requestMap[handle];
	
	if(ptr!=NULL)
	{		
		HTTPAsyncRequestAndroid::requestMap.erase(HTTPAsyncRequestAndroid::requestMap.find(handle));

		if(result == JNI_TRUE)
			ptr->GetData();
	}
}