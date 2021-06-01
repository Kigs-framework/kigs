
#include "Platform/Core/PlatformCore.h"


// special case for cube map
// generate file names from base name
bool	OpenGLTexture::CubeMapGeneration()
{
	bool result = true;
	SP<FilePathManager>	pathManager = KigsCore::GetSingleton("FilePathManager");
	kstl::string fullfilename;
	char asciiCount[16];
	asciiCount[0] = 0;

	// get extension
	kstl::string	extension = mFileName;
	extension = extension.substr(extension.rfind("."));
	// remove extension
	kstl::string basefilename = mFileName;
	basefilename = basefilename.substr(0, basefilename.length() - extension.length());

	// check if all 6 textures are ok
	int index;
	for (index = 0; index<6; index++)
	{
		kstl::string filename = basefilename;
		filename += "_";
		sprintf(asciiCount, "%d", (index + 1));
		filename += asciiCount;
		filename += extension;

		SmartPointer<FileHandle> fullfilenamehandle = pathManager->FindFullName(filename);
		if (!fullfilenamehandle)
		{
			fullfilename = fullfilenamehandle->mFullFileName;
		}
		else
		{
			result = false;
			break;
		}
	}

	if (result) // ok, the 6 textures are here
	{
		// first load them all

		kfloat minwidth = 1024;
		kfloat minheight = 1024;

		// TODO

		return true;

	}

	return result;
}


bool	OpenGLTexture::UpdateBufferZone(unsigned char* bitmapbuffer, const BBox2DI& zone, const Point2DI& bitmapSize)
{
	return false;
}


bool			OpenGLTexture::CanUseDynamicTexture(TinyImage::ImageFormat format)
{
	return false;
}
bool			OpenGLTexture::UseDynamicTexture(unsigned char* buffer, unsigned int width, unsigned int height, TinyImage::ImageFormat format, bool needRealloc)
{
	return true;
}