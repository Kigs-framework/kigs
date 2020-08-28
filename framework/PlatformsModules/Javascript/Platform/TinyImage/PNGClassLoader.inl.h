
extern "C" void			JSLoadTextureInCanvas(const char* filename, int& width, int& height);

extern "C" void			JSInitImageData(void* pixels, int width, int height);

bool	PNGClass::Load(FileHandle* fileName)
{

	// free previous image if any
	if(mInitIsOK)
	{
		if (mPixels)
		{
			delete[] mPixels;
			mPixels = 0;
		}
		if (mPalette)
		{
			delete[] mPalette;
			mPalette = 0;
		}
	}

	int  width, height;
	JSLoadTextureInCanvas(fileName->mFullFileName.c_str(), width,height);
	if ((width > 0) && (height > 0))
	{
		mWidth = width;
		mHeight = height;
		mPaletteDataSize = 0;
		mPixelLineSize = width * 4;
		mPixelDataSize = mPixelLineSize*height;

		mPixels = new unsigned char[mPixelDataSize];

		JSInitImageData(mPixels, width, height);

		mFormat = RGBA_32_8888;

		return true;
	}
	return false;
}

void	PNGClass::Export(const char* filename) // Not in JS
{

}