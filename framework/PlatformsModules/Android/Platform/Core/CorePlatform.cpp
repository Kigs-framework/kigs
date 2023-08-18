#include "PrecompiledHeaders.h"
#include "../../Core/Headers/Core.h"
#include "../../FileManager/Headers/FilePathManager.h"
#include "Platform/Core/PlatformCore.h"
#include "usString.h"
#include  <sys/stat.h>
#include <game-activity/native_app_glue/android_native_app_glue.h>


extern android_app* 			mAndroidApp;

namespace Kigs
{

	namespace File
	{
        struct ANDROIDFILE
        {
            void* myHandle;
        };

		void Android_CheckState(FileHandle* hndl)
		{
			
			switch(hndl->mDeviceID) {
                case FilePathManager::RESSOURCES: {
                    auto assetManager = mAndroidApp->activity->assetManager;

                    AAsset * asset=AAssetManager_open(assetManager,hndl->mFullFileName.c_str(),AASSET_MODE_BUFFER);
                    if(!asset)
                    {
                        // check for directory then ?
                        AAssetDir * assetdir=AAssetManager_openDir(assetManager,hndl->mFullFileName.c_str());

                        if(!assetdir) {
                            hndl->resetStatus();
                            return;
                        }
                        hndl->mStatus |= FileHandle::Exist;
                        hndl->mStatus |= FileHandle::IsDIr;

                        hndl->mFile = new ANDROIDFILE();
                        hndl->mFile->myHandle = assetdir;
                    }
                    else
                    {
                        hndl->mFile = new ANDROIDFILE();
                        hndl->mStatus |= FileHandle::Exist;
                        hndl->mStatus |= FileHandle::Open;
                        hndl->mFile->myHandle = asset;
						hndl->setOpeningFlags(FileHandle::Read | FileHandle::Binary);
                    }
                }
				break;
				default:
				// TODO
				break;
			}
			
			return;
		}

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

	                // TODO
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

			void*	fdHandle = NULL;

			switch (handle->mDeviceID)
			{
			case 0: // assets
			{
				auto assetManager = mAndroidApp->activity->assetManager;

				fdHandle=AAssetManager_open(assetManager,handle->mFullFileName.c_str(),AASSET_MODE_BUFFER);
			}
			break;

			case FilePathManager::DEVICE_STORAGE:
			case FilePathManager::APPLICATION_STORAGE:
			case FilePathManager::DOCUMENT_FOLDER:
				//
				// TODO
				break;
			}

			if (fdHandle)
			{
				handle->mFile = new ANDROIDFILE();
				handle->mFile->myHandle = fdHandle;


				handle->mSize = AAsset_getLength(static_cast<AAsset*>(handle->mFile->myHandle));

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
					// TODO : check if asset or other file
					return AAsset_read(static_cast<AAsset*>(handle->mFile->myHandle),ptr,size*count);
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
					// TODO
					return 0;
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
					return AAsset_seek(
							static_cast<AAsset*>(handle->mFile->myHandle),
							0,
							SEEK_CUR
					);
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
					return AAsset_seek(
							static_cast<AAsset*>(handle->mFile->myHandle),
							offset,
							origin
					);
				}
			}

			return 0;
		}
		int Android_fflush(FileHandle* handle)
		{
			// TODO ?
			return 0;
		}
		int Android_fclose(FileHandle* handle)
		{
			if (handle->mFile)
			{
				if (handle->mFile->myHandle)
				{
					if(handle->mStatus & FileHandle::IsDIr)
					{
						AAssetDir_close(static_cast<AAssetDir*>(handle->mFile->myHandle));
					}
					else
					{
						AAsset_close(static_cast<AAsset*>(handle->mFile->myHandle));
					}

					handle->mFile->myHandle = 0;
				}
				delete handle->mFile;
				handle->mFile = 0;
				handle->mStatus = 0;
			}

			return 0;
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