#include <jni.h>

bool	SQLiteDB::ExistDatabase(const char* dbName)
{
	JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
	jclass myCls = KigsJavaIDManager::findClass("com/assoria/sqlite/KigsDB");	
	jmethodID exist_database = g_env->GetStaticMethodID(myCls, "ExistDatabase", "(Ljava/lang/String;)Z");	
	jstring jpath = g_env->NewStringUTF(dbName);
	bool ret = g_env->CallStaticBooleanMethod(myCls, exist_database, jpath);
	g_env->DeleteLocalRef(jpath);
	
	return ret;
}