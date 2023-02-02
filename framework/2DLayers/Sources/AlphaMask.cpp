#include "PrecompiledHeaders.h"
#include "AlphaMask.h"
#include "TinyImage.h"

using namespace Kigs::Draw2D;

//IMPLEMENT_AND_REGISTER_CLASS_INFO(AlphaMask, AlphaMask, 2DLayers);
IMPLEMENT_CLASS_INFO(AlphaMask)


void AlphaMask::InitModifiable()
{
	ParentClassType::InitModifiable();
	if(IsInit())
	{
		auto img = Pict::TinyImage::CreateImage(((std::string)mTextureName).c_str());
		auto pixel_size = img->GetPixelValueSize(img->GetFormat());
		
		mSize.x = img->GetUsedWidth();
		mSize.y = img->GetUsedHeight();
		
		if (pixel_size <= 0 || pixel_size > 4)
		{
			UninitModifiable();
			return;
		}

		auto stride = img->GetPixelLineSize();

		if (stride <= 0)
			stride = mSize.x*pixel_size;

		auto data = img->GetPixelData();
		CreateMask(data, stride, pixel_size);

	}
}

void AlphaMask::CreateMask(u8* pixelData, int stride, int pixel_size)
{
	int tempthreshold = (int) (mThreshold*255);
	mTab.resize(mSize.x*mSize.y);

	unsigned char*	writeTab = mTab.data();
	unsigned char*	readPixel = pixelData;

	for(int j = 0; j < mSize.y; j++)
	{
		for(int i = 0; i < mSize.x; i++)
		{
			u8 value = pixel_size == 4 ? readPixel[pixel_size*i + 3] : readPixel[pixel_size*i + 0];
			if(value < tempthreshold)
			{
				*writeTab = 0;
			}
			else
			{
				*writeTab = 1;
			}
			++writeTab;
		}
		readPixel += stride;
	}
}

bool AlphaMask::CheckTo(float X, float Y)
{
	int lX = (int)(X * mSize.x);
	int lY = (int)(Y * mSize.y);
	return mTab[lX+lY* mSize.x];
}