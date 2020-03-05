#ifndef __KIGSJAVAIDMANAGER_H__
#define __KIGSJAVAIDMANAGER_H__

#include <jni.h>
#include <pthread.h>

#include "../../Core/Headers/kstlstring.h"
#include "kstlmap.h"

class usString;

class KigsJavaIDManager {
public:
	static JavaVM *pJavaVM;

	static int OsVersion;

	// Display
	static jclass	 Renderer_class;
	static jmethodID GetResolutionX;
	static jmethodID GetResolutionY;
	
	// Files
	static jclass	 FileManager_class;
	static jmethodID OpenFile;
	static jmethodID ReadFile;
	static jmethodID WriteFile;
	static jmethodID TellFile;
	static jmethodID SeekFile;
	static jmethodID CloseFile;
	static jmethodID CheckFile;
	static unsigned int GetFileSizeID;

	// send message to java main
	static jclass	 Main_class;
	static jmethodID SendMessage;
	static jmethodID GetBackKeyState;
	static jmethodID OpenLink;
	static jmethodID CheckConnexion;

	// bitmaps
	static jclass	 imageLoader_class;
	static jmethodID OpenBitmap;
	static jmethodID getBitmapWidth;
	static jmethodID getBitmapHeight;
	static jmethodID getBitmapPixels;
	static jmethodID closeBitmap;
		
	static jobject gClassLoader;
	static jmethodID gFindClassMethod;

	static bool init(JavaVM* vm);
	static void close();

	static void convertJstringToUsString(jstring &js, usString &str);
	static void convertJstringToKstlString(jstring &js, kstl::string &str);

	static JNIEnv* getEnv(pthread_t thread);
	static void clearEnvList();
	static void detachCurrentThread(pthread_t thread);
	
	static kstl::map<pthread_t, JNIEnv*> pJNIEnvs;
	
	// methodID managment
	static kstl::map<unsigned int, jclass> pClass;
	static jclass RegisterClass(JNIEnv* env, const char * clsName);
	static jclass findClass(const char * clsName);
	
	
	static kstl::map<unsigned int, jmethodID> pMethods;

	static unsigned int RegisterMethod(JNIEnv* env, const char * clsName, const char * mtdName, const char * mtdSig);
	static jmethodID GetMethod(JNIEnv* env, const char * clsName, const char * mtdName, const char * mtdSig);
	static jmethodID GetMethod(JNIEnv* env, unsigned int ID);

	static jmethodID GetStaticMethod(JNIEnv* env, const char * clsName, const char * mtdName, const char * mtdSig);
};

#endif //__KIGSJAVAIDMANAGER_H__
