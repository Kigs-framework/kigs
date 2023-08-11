#include "PrecompiledHeaders.h"
#include "../../Core/Headers/Core.h"
#include "../../FileManager/Headers/FilePathManager.h"
#include "Platform/Core/PlatformCore.h"
#include "usString.h"
#include  <sys/stat.h>

namespace Kigs
{

	namespace File
	{

		void Android_CheckState(FileHandle* hndl)
		{
			JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());

			std::string stat;
			jstring fname = g_env->NewStringUTF(hndl->mFullFileName.c_str());
			jstring result = (jstring)g_env->CallStaticObjectMethod(KigsJavaIDManager::FileManager_class, KigsJavaIDManager::CheckFile, fname, hndl->mDeviceID);
			KigsJavaIDManager::convertJstringToKstlString(result, stat);
			g_env->DeleteLocalRef(fname);
			g_env->DeleteLocalRef(result);

			if (stat == "")
			{
				hndl->resetStatus();
				return;
			}

			std::vector<std::string> arr = SplitStringByCharacter(stat, '|');

			hndl->mFullFileName = arr[0];

			if (arr[1] == "1")
				hndl->mStatus |= FileHandle::Exist;

			if (arr[2] == "1")
				hndl->mStatus |= FileHandle::IsDIr;

			//printf("%s %d(%d)\n", hndl->mFullFileName.c_str(), 0, hndl->mStatus);
		}
	}

	namespace Core
	{
		// define KigsJavaIDManager methods here
		JavaVM* KigsJavaIDManager::pJavaVM = 0;

		jclass	 	KigsJavaIDManager::Renderer_class = 0;
		jclass	 	KigsJavaIDManager::FileManager_class = 0;
		jclass	 	KigsJavaIDManager::Main_class = 0;
		jclass	 	KigsJavaIDManager::imageLoader_class = 0;

		jmethodID 	KigsJavaIDManager::GetResolutionX = 0;
		jmethodID 	KigsJavaIDManager::GetResolutionY = 0;
		jmethodID 	KigsJavaIDManager::OpenFile = 0;
		jmethodID 	KigsJavaIDManager::CloseFile = 0;
		jmethodID 	KigsJavaIDManager::ReadFile = 0;
		jmethodID 	KigsJavaIDManager::WriteFile = 0;
		jmethodID 	KigsJavaIDManager::TellFile = 0;
		jmethodID 	KigsJavaIDManager::SeekFile = 0;
		jmethodID 	KigsJavaIDManager::CheckFile = 0;
		unsigned int KigsJavaIDManager::GetFileSizeID = 0;
		jmethodID 	KigsJavaIDManager::SendMessage = 0;
		jmethodID 	KigsJavaIDManager::OpenLink = 0;
		jmethodID 	KigsJavaIDManager::CheckConnexion = 0;
		jmethodID	KigsJavaIDManager::GetBackKeyState = 0;

		// bitmaps
		jmethodID 	KigsJavaIDManager::OpenBitmap;
		jmethodID 	KigsJavaIDManager::getBitmapWidth;
		jmethodID 	KigsJavaIDManager::getBitmapHeight;
		jmethodID 	KigsJavaIDManager::getBitmapPixels;
		jmethodID 	KigsJavaIDManager::closeBitmap;
		int KigsJavaIDManager::OsVersion = 0;

		std::map<pthread_t, JNIEnv*>		KigsJavaIDManager::pJNIEnvs;


		bool KigsJavaIDManager::init(JavaVM* vm)
		{
			pJavaVM = vm;

			JNIEnv* pEnv = getEnv(pthread_self());

			if ((vm == 0) || (pEnv == 0))
				return false;



			jclass pMaClasse = pEnv->FindClass("com/kigs/kigsmain/KigsRenderer");
			Renderer_class = (jclass)pEnv->NewGlobalRef(pMaClasse);
			if (Renderer_class == 0)
			{
				KIGS_ERROR("KigsRenderer not found", 2);
			}

			GetResolutionX = pEnv->GetStaticMethodID(Renderer_class, "GetResolutionX", "()I");
			GetResolutionY = pEnv->GetStaticMethodID(Renderer_class, "GetResolutionY", "()I");

			pMaClasse = pEnv->FindClass("com/kigs/filemanager/FileManager");
			FileManager_class = (jclass)pEnv->NewGlobalRef(pMaClasse);
			if (FileManager_class == 0)
			{
				KIGS_ERROR("FileManager not found", 2);
			}

			OpenFile = pEnv->GetStaticMethodID(FileManager_class, "openFile", "(Ljava/lang/String;IZZ)Lcom/kigs/filemanager/FILE_DESCRIPTOR;");
			ReadFile = pEnv->GetStaticMethodID(FileManager_class, "readFile", "(Lcom/kigs/filemanager/FILE_DESCRIPTOR;Ljava/nio/ByteBuffer;I)I");
			WriteFile = pEnv->GetStaticMethodID(FileManager_class, "writeFile", "(Lcom/kigs/filemanager/FILE_DESCRIPTOR;Ljava/nio/ByteBuffer;I)I");
			CloseFile = pEnv->GetStaticMethodID(FileManager_class, "closeFile", "(Lcom/kigs/filemanager/FILE_DESCRIPTOR;)V");
			SeekFile = pEnv->GetStaticMethodID(FileManager_class, "FileSeek", "(Lcom/kigs/filemanager/FILE_DESCRIPTOR;II)I");
			TellFile = pEnv->GetStaticMethodID(FileManager_class, "FileTell", "(Lcom/kigs/filemanager/FILE_DESCRIPTOR;)I");
			CheckFile = pEnv->GetStaticMethodID(FileManager_class, "CheckFile", "(Ljava/lang/String;I)Ljava/lang/String;");

			pMaClasse = pEnv->FindClass("com/kigs/kigsmain/kigsmainActivity");
			Main_class = (jclass)pEnv->NewGlobalRef(pMaClasse);
			if (Main_class == 0)
			{
				KIGS_ERROR("kigsmainActivity not found", 2);
			}




			SendMessage = pEnv->GetStaticMethodID(Main_class, "SendMessage", "(II)V");
			OpenLink = pEnv->GetStaticMethodID(Main_class, "OpenLink", "(Ljava/lang/String;)V");
			CheckConnexion = pEnv->GetStaticMethodID(Main_class, "CheckConnexion", "()Z");
			GetBackKeyState = pEnv->GetStaticMethodID(Main_class, "GetBackKeyState", "()Z");

			/*std::string str;
			jmethodID met = pEnv->GetStaticMethodID(Main_class, "osVersion", "()Ljava/lang/String;");
			if (met)
			{
				jstring result = (jstring)pEnv->CallStaticObjectMethod(Main_class, met);
				KigsJavaIDManager::convertJstringToKstlString(result, str);

				myPrintfLogger("%s\n", str.c_str());
			}*/

			jmethodID met = pEnv->GetStaticMethodID(Main_class, "osVersionCode", "()I");
			OsVersion = pEnv->CallStaticIntMethod(Main_class, met);

			pMaClasse = pEnv->FindClass("com/kigs/imageLoader/imageLoader");
			imageLoader_class = (jclass)pEnv->NewGlobalRef(pMaClasse);

			OpenBitmap = pEnv->GetStaticMethodID(imageLoader_class, "open", "(Ljava/lang/String;)Landroid/graphics/Bitmap;");
			getBitmapWidth = pEnv->GetStaticMethodID(imageLoader_class, "getWidth", "(Landroid/graphics/Bitmap;)I");
			getBitmapHeight = pEnv->GetStaticMethodID(imageLoader_class, "getHeight", "(Landroid/graphics/Bitmap;)I");
			getBitmapPixels = pEnv->GetStaticMethodID(imageLoader_class, "getPixels", "(Landroid/graphics/Bitmap;[I)V");
			closeBitmap = pEnv->GetStaticMethodID(imageLoader_class, "close", "(Landroid/graphics/Bitmap;)V");


#ifdef _DEBUG
			/*	KigsJavaIDManager::RegisterClass(pEnv, "dalvik/system/VMDebug");

				jclass vm_class = KigsJavaIDManager::findClass("dalvik/system/VMDebug");
				jmethodID dump_mid = pEnv->GetStaticMethodID( vm_class, "dumpReferenceTables", "()V" );
				pEnv->CallStaticVoidMethod( vm_class, dump_mid );*/
#endif

			GetFileSizeID = RegisterMethod(pEnv, "com/kigs/filemanager/FILE_DESCRIPTOR", "getSize", "()I"); // register in map


			return true;
		}

		void KigsJavaIDManager::close()
		{
			JNIEnv* pEnv = getEnv(pthread_self());
			pEnv->DeleteGlobalRef(Renderer_class);
			pEnv->DeleteGlobalRef(FileManager_class);
			pEnv->DeleteGlobalRef(Main_class);
			pEnv->DeleteGlobalRef(imageLoader_class);
		}

		void KigsJavaIDManager::convertJstringToUsString(jstring& js, usString& str)
		{
			JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
			const jchar* raw = g_env->GetStringChars(js, 0);
			jsize len = g_env->GetStringLength(js);
			const jchar* temp = raw;
			unsigned short* t = new unsigned short[len + 1];
			unsigned short* tt = t;
			while (len > 0)
			{
				*tt++ = *(temp++);
				len--;
			}
			*tt = 0;
			g_env->ReleaseStringChars(js, raw);
			str = usString(t);
			delete[] t;
		}

		void KigsJavaIDManager::convertJstringToKstlString(jstring& js, std::string& str)
		{
			JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
			const char* s = g_env->GetStringUTFChars(js, 0);
			str = std::string(s);
			g_env->ReleaseStringUTFChars(js, s);
		}

		JNIEnv* KigsJavaIDManager::getEnv(pthread_t thread)
		{
			JNIEnv* pEnv = 0;

			std::map<pthread_t, JNIEnv*>::iterator it = pJNIEnvs.find(thread);

			if (it != pJNIEnvs.end())
			{
				pEnv = it->second;
			}
			else
			{
				int ret = pJavaVM->GetEnv((void**)&pEnv, JNI_VERSION_1_4);
				if (ret < 0)
				{
					JavaVMAttachArgs args;
					args.version = JNI_VERSION_1_6;
					args.name = NULL;
					args.group = NULL;
					pJavaVM->AttachCurrentThread(&pEnv, &args);
				}

				pJNIEnvs.insert(std::pair<pthread_t, JNIEnv*>(thread, pEnv));
			}

			return pEnv;
		}

		void KigsJavaIDManager::clearEnvList()
		{
			printf("clear env list");
			pJNIEnvs.clear();
		}

		void KigsJavaIDManager::detachCurrentThread(pthread_t thread)
		{
			std::map<pthread_t, JNIEnv*>::iterator it = pJNIEnvs.find(thread);

			if (it != pJNIEnvs.end())
			{
				pJNIEnvs.erase(it);
			}

			pJavaVM->DetachCurrentThread();
		}
	}

	namespace File
	{
		struct ANDROIDFILE
		{
			void* myHandle;
		};


		SmartPointer<FileHandle> Android_FindFullName(const std::string& filename)
		{
			if (filename[0] != '#')
			{
				return 0;
			}
			SmartPointer<FileHandle> result = FilePathManager::CreateFileHandle((const char*)(&(filename.c_str()[3])));

			result->mFullFileName = (const char*)(&filename[3]);
			result->mDeviceID = (FilePathManager::DeviceID)(filename[1]);

			Android_CheckState(result.get());
			return result;
		}

		bool Android_CreateFolderTree(FileHandle* hndl)
		{

			if ((hndl->mStatus & FileHandle::Exist) == 0)
			{
				SmartPointer<FileHandle> parent = FilePathManager::CreateFileHandle(FilePathManager::GetParentDirectory(hndl->mFullFileName));
				if (parent->mFullFileName != "")
				{
					parent->mDeviceID = hndl->mDeviceID;
					Android_CheckState(parent.get());

					if ((parent->mStatus & FileHandle::Exist))
					{
						return true;
					}

					JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
					jmethodID jmethod = g_env->GetStaticMethodID(KigsJavaIDManager::FileManager_class, "createFolder", "(Ljava/lang/String;I)V");

					jstring fname = g_env->NewStringUTF(hndl->mFullFileName.c_str());
					g_env->CallStaticVoidMethod(KigsJavaIDManager::FileManager_class, jmethod, fname, hndl->mDeviceID);
					g_env->DeleteLocalRef(fname);

					//Android_CreateFolderTree(parent);

			//printf("mkdir %s", parent->mFullFileName.c_str());
					//CreateDirectoryA(parent->mFullFileName.c_str(), NULL);
					//return mkdir(parent->mFullFileName.c_str(), 0x0770);
				}
			}
			return false;
		}

		bool  Android_fopen(FileHandle* handle, const char* mode)
		{
			unsigned int flagmode = FileHandle::OpeningFlags(mode);

			if (handle->mStatus & FileHandle::Open) // already opened ? return true
			{
				// check if open mode is the same
				if (flagmode == handle->getOpeningFlags())
				{
					return true;
				}
				else
				{
					Android_fclose(handle);
				}
			}

			// create parent if in write mode
			if (flagmode & FileHandle::Write)
			{
				Android_CreateFolderTree(handle);
			}

			bool read = (flagmode & FileHandle::Read);
			bool append = (flagmode & FileHandle::Append);

			JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
			jstring fname = g_env->NewStringUTF(handle->mFullFileName.c_str());

			jobject	fdHandle = NULL;

			switch (handle->mDeviceID)
			{
			case 0: // assets
				fdHandle = (jobject)g_env->CallStaticObjectMethod(KigsJavaIDManager::FileManager_class, KigsJavaIDManager::OpenFile, fname, handle->mDeviceID, true, false);
				break;

			case FilePathManager::DEVICE_STORAGE:
			case FilePathManager::APPLICATION_STORAGE:
			case FilePathManager::DOCUMENT_FOLDER:
				fdHandle = (jobject)g_env->CallStaticObjectMethod(KigsJavaIDManager::FileManager_class, KigsJavaIDManager::OpenFile, fname, handle->mDeviceID, read, append);
				break;
			}



			g_env->DeleteLocalRef(fname);

			if (fdHandle)
			{
				handle->mFile = new ANDROIDFILE();

				jobject globalFD = (jobject)g_env->NewGlobalRef(fdHandle);
				/*
				jclass javaClassFILE_DESCRIPTOR = g_env->FindClass("com/kigs/filemanager/FILE_DESCRIPTOR");
				jmethodID getMethodGetSize = g_env->GetMethodID(javaClassFILE_DESCRIPTOR, "getSize", "()I");*/

				jmethodID getMethodGetSize = KigsJavaIDManager::GetMethod(g_env, KigsJavaIDManager::GetFileSizeID);

				handle->mSize = g_env->CallIntMethod(fdHandle, getMethodGetSize);

				handle->mFile->myHandle = ((void*)globalFD);
				g_env->DeleteLocalRef(fdHandle);

				handle->mStatus |= FileHandle::Open;
				handle->mStatus |= FileHandle::Exist;
				handle->setOpeningFlags(flagmode);

				return true;
			}
			handle->resetStatus();
			return false;
		}

		long int Android_fread(void* ptr, long size, long count, FileHandle* handle)
		{
			if (handle->mFile)
			{
				if (handle->mFile->myHandle)
				{
					JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
					jobject globalFD = (jobject)handle->mFile->myHandle;
					jobject	bytebuffer = g_env->NewDirectByteBuffer(ptr, size * count);
					int result = g_env->CallStaticIntMethod(KigsJavaIDManager::FileManager_class, KigsJavaIDManager::ReadFile, globalFD, bytebuffer, size * count);
					g_env->DeleteLocalRef(bytebuffer);
					return result;
				}
			}
			return 0;
		}

		long int Android_fwrite(const void* ptr, long size, long count, FileHandle* handle)
		{
			if (handle->mFile)
			{
				if (handle->mFile->myHandle)
				{
					JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
					jobject globalFD = (jobject)handle->mFile->myHandle;
					jobject	bytebuffer = g_env->NewDirectByteBuffer((void*)ptr, size * count);
					int result = g_env->CallStaticIntMethod(KigsJavaIDManager::FileManager_class, KigsJavaIDManager::WriteFile, globalFD, bytebuffer, size * count);
					g_env->DeleteLocalRef(bytebuffer);
					return result;
				}
			}
			return 0;
		}
		long int Android_ftell(FileHandle* handle)
		{
			if (handle->mFile)
			{
				if (handle->mFile->myHandle)
				{
					JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
					jobject globalFD = (jobject)handle->mFile->myHandle;
					int result = g_env->CallStaticIntMethod(KigsJavaIDManager::FileManager_class, KigsJavaIDManager::TellFile, globalFD);
					return result;
				}
			}

			return 0;
		}
		int Android_fseek(FileHandle* handle, long int offset, int origin)
		{
			if (handle->mFile)
			{
				if (handle->mFile->myHandle)
				{
					JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
					jobject globalFD = (jobject)handle->mFile->myHandle;
					int result = g_env->CallStaticIntMethod(KigsJavaIDManager::FileManager_class, KigsJavaIDManager::SeekFile, globalFD, offset, origin);
					return result;
				}
			}

			return 0;
		}
		int Android_fflush(FileHandle* handle)
		{
			// TODO
			return 0;
		}
		int Android_fclose(FileHandle* handle)
		{
			if (handle->mFile)
			{
				if (handle->mFile->myHandle)
				{
					JNIEnv* g_env = KigsJavaIDManager::getEnv(pthread_self());
					jobject globalFD = (jobject)handle->mFile->myHandle;
					g_env->CallStaticVoidMethod(KigsJavaIDManager::FileManager_class, KigsJavaIDManager::CloseFile, globalFD);
					g_env->DeleteGlobalRef(globalFD);

					handle->mFile->myHandle = 0;
				}
				delete handle->mFile;
				handle->mFile = 0;
				handle->mStatus = 0;
			}

			return 0;
		}
	}

	namespace Core
	{
		// METHOD MANAGMENT
		std::map<unsigned int, jmethodID> 	KigsJavaIDManager::pMethods;
		std::map<unsigned int, jclass> 	KigsJavaIDManager::pClass;

		jclass KigsJavaIDManager::RegisterClass(JNIEnv* env, const char* clsName)
		{
			unsigned int ID = CharToID::GetID(std::string_view(clsName, strlen(clsName)));

			// search in map
			std::map<unsigned int, jclass>::iterator found = pClass.find(ID);
			if (found != pClass.end())
				return found->second;

			jclass cls = (jclass)env->NewGlobalRef(env->FindClass(clsName));
			pClass[ID] = cls;

			return cls;
		}

		jclass KigsJavaIDManager::findClass(const char* clsName)
		{
			unsigned int ID = CharToID::GetID(std::string_view(clsName, strlen(clsName)));

			// search in map
			std::map<unsigned int, jclass>::iterator found = pClass.find(ID);
			if (found != pClass.end())
				return found->second;

			return NULL;
		}

		jmethodID KigsJavaIDManager::GetMethod(JNIEnv* env, unsigned int ID)
		{
			std::map<unsigned int, jmethodID>::iterator found = pMethods.find(ID);
			if (found != pMethods.end())
				return found->second;

			return nullptr;
		}


		jmethodID KigsJavaIDManager::GetMethod(JNIEnv* env, const char* clsName, const char* mtdName, const char* mtdSig)
		{
			char label[1024];
			sprintf(label, "%s%s%s", clsName, mtdName, mtdSig);

			unsigned int ID = CharToID::GetID(label);

			// search in map
			std::map<unsigned int, jmethodID>::iterator found = pMethods.find(ID);
			if (found != pMethods.end())
				return found->second;


			jmethodID mID = env->GetMethodID(RegisterClass(env, clsName), mtdName, mtdSig);
			pMethods[ID] = mID;

			return mID;
		}

		unsigned int KigsJavaIDManager::RegisterMethod(JNIEnv* env, const char* clsName, const char* mtdName, const char* mtdSig)
		{
			char label[1024];
			sprintf(label, "%s%s%s", clsName, mtdName, mtdSig);

			unsigned int ID = CharToID::GetID(label);

			jmethodID mID = env->GetMethodID(RegisterClass(env, clsName), mtdName, mtdSig);
			pMethods[ID] = mID;

			return ID;
		}


		jmethodID KigsJavaIDManager::GetStaticMethod(JNIEnv* env, const char* clsName, const char* mtdName, const char* mtdSig)
		{
			char label[1024];
			sprintf(label, "s_%s%s%s", clsName, mtdName, mtdSig);

			unsigned int ID = CharToID::GetID(label);

			// search in map
			std::map<unsigned int, jmethodID>::iterator found = pMethods.find(ID);
			if (found != pMethods.end())
				return found->second;


			jmethodID mID = env->GetStaticMethodID(RegisterClass(env, clsName), mtdName, mtdSig);
			pMethods[ID] = mID;

			return mID;
		}
	}
}

#include <iostream>
#include <codecvt>
#include <locale>

// utility wrapper to adapt locale-bound facets for wstring/wbuffer convert
template<class Facet>
struct deletable_facet : Facet
{
    template<class ...Args>
    deletable_facet(Args&& ...args) : Facet(std::forward<Args>(args)...) {}
    ~deletable_facet() {}
};

std::string Kigs::Core::to_utf8(const wchar_t* buffer, int len)
{
    std::wstring str(buffer,len);
    return to_utf8(str);
}

std::string Kigs::Core::to_utf8(const std::wstring& str)
{
	std::wstring_convert<deletable_facet<std::codecvt<wchar_t, char, std::mbstate_t>>> utf8_conv;
	return utf8_conv.to_bytes(str);
}

std::wstring Kigs::Core::to_wchar(const char* buffer, int len)
{
	std::string str(buffer,len);
    return to_wchar(str);
}

std::wstring Kigs::Core::to_wchar(const std::string& str)
{
	std::wstring_convert<deletable_facet<std::codecvt<wchar_t, char, std::mbstate_t>>>  wchar_conv;
	return wchar_conv.from_bytes(str);
}