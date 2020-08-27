


bool	PNGClass::Load(const char* fileName)
{
	bool result=false;

	// free previous image if any
	if(mInitIsOK)
	{
		if(mPixels)
		{
			delete[] mPixels;
		}
		if(mPalette)
		{
			delete[] mPalette;
		}	
	}

	static bool isInit=false;
	
	if(!isInit)
	{
		ilInit();
		isInit=true;
	}
	ILuint Img;
	BITMAPINFO Info;

	ilGenImages(1,&Img);
	ilBindImage(Img);
	
	//load the image
	ilLoadImage(fileName);
	ilutGetBmpInfo(&Info);
	myWidth = (int)Info.bmiHeader.biWidth;
	myHeight = (int)Info.bmiHeader.biHeight;
	
	ReadColorFunc currentReadFunc;

	ImageFormat	currentReadFormat=NO_TEXTURE;
	if(Info.bmiHeader.biBitCount == 32)
	{
		currentReadFormat=RGBA_32_8888;
	}
	else if(Info.bmiHeader.biBitCount == 24)
	{
		currentReadFormat=RGB_24_888;
	}
	
	if(currentReadFormat != NO_TEXTURE)
	{

		mFormat=mLoaderContext->myFormatConvertion[(int)currentReadFormat];

		int outFormatPixelSize=GetPixelValueSize(mFormat);

		if(outFormatPixelSize>0)
		{
			mPixelLineSize=myWidth*outFormatPixelSize;
		}
		else if(outFormatPixelSize==-1)
		{
			mPixelLineSize=(unsigned int)myWidth>>1;
		}
		else if(outFormatPixelSize==-2)
		{
			mPixelLineSize=(unsigned int)myWidth>>2;
		}
				
		mPixelDataSize= mPixelLineSize*myHeight;
		mPixels= new u8[mPixelDataSize];

		// pointer on file pixel data 
		u8* pixels = ilGetData();

		// pointer on image pixel data
		u8* image = mPixels;

		// number of bytes to read
		u32 bytes = (u32)Info.bmiHeader.biBitCount>>3;
		u32 size  = myWidth*myHeight*bytes;

		currentReadFunc=TGAClass::GetReadFunction(mFormat,bytes,true);

		for(u32 i=0;i<size;i+=bytes,image+=outFormatPixelSize) 
		{
			currentReadFunc(&(pixels[i]), image);
		}

		result = true;	
	}

	ilDeleteImages(1,&Img);

	return result;
}

