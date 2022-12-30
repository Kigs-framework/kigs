#include "UI/PassiveColoredMask.h"
#include "TextureFileManager.h"
#include "Texture.h"
#include <stdlib.h>

using namespace Kigs::Draw2D;
using namespace Kigs::Draw;

//IMPLEMENT_AND_REGISTER_CLASS_INFO(PassiveColoredMask, PassiveColoredMask, 2DLayers);
IMPLEMENT_CLASS_INFO(PassiveColoredMask)

//! constructor, init all parameters
PassiveColoredMask::PassiveColoredMask(const std::string& name,CLASS_NAME_TREE_ARG)
:UIItem(name,PASS_CLASS_NAME_TREE_ARG)
,mThreshold(*this,true,"Threshold",0)
,mTextureName(*this,false,"TextureName","")
,mMaskName(*this,false,"MaskName","")
,mRay(*this,true,"Ray",0)
{
	mIsDown = false;
	mIsMouseOver = false;
	mPixelArray = nullptr;
	mPixelMaskArray = nullptr;
	mOriginalPixelArray = nullptr;
}

PassiveColoredMask::~PassiveColoredMask()
{
	if(mTexture)
		mTexture = nullptr;

	if(mPixelArray)
	{
		free(mPixelArray);
		mPixelArray = nullptr;
	}
	if(mPixelMaskArray)
	{
		free(mPixelMaskArray);
		mPixelMaskArray = nullptr;
	}
	if(mOriginalPixelArray)
	{
		free(mOriginalPixelArray);
		mOriginalPixelArray = nullptr;
	}
}

void	PassiveColoredMask::ReloadTexture()
{
	if(mTexture)
	{
		mTexture->ReInit();
	}
}

void	PassiveColoredMask::InitModifiable()
{
	UIItem::InitModifiable();
	if(IsInit())
	{
		mIsDown      = false;
		mIsMouseOver = false;

		auto textureManager = KigsCore::Singleton<TextureFileManager>();
		mTexture = textureManager->GetTexture(mTextureName);

		if(mTexture)
		{
			mTexture->getValue("Width",mWidth);
			mTexture->getValue("Height",mHeight);

			mTexture->getValue("Pow2Width",mPow2Width);
			mTexture->getValue("Pow2Height",mPow2Height);

            int width, height, pow2width, pow2height;
            width = height = pow2width = pow2height = 0;
            
			SmartPointer<Texture>	MaskTexture;
			if(mMaskName.const_ref() != "")
			{
				MaskTexture = textureManager->GetTexture(mMaskName);
				MaskTexture->getValue("Width",width);
				MaskTexture->getValue("Height",height);

				MaskTexture->getValue("Pow2Width",pow2width);
				MaskTexture->getValue("Pow2Height",pow2height);

			}

			if(mWidth > 0 && mHeight > 0)
			{
				//Allow temp buffer to get pixel data
				mPixelArray  = nullptr;
				mPixelArray  = (unsigned int*) malloc((mPow2Width*mPow2Height)*sizeof(unsigned int));
				mOriginalPixelArray = (unsigned int*) malloc((mPow2Width*mPow2Height)*sizeof(unsigned int));
				

				mTexture->GetPixels(mPixelArray);
				memcpy(mOriginalPixelArray,mPixelArray,(mPow2Width*mPow2Height)*sizeof(unsigned int));
				if(MaskTexture)
				{
					mPixelMaskArray = (unsigned int*) malloc((pow2width*pow2height)*sizeof(unsigned int));
					MaskTexture->GetPixels(mPixelMaskArray);

				}

				// auto size button
				if( (((int)mSize[0])==0) && (((int)mSize[1])==0) )
				{
					if(mTexture)
					{
						float width,height;
						mTexture->GetSize(width,height);
						mSize=v2f(width,height);
					}
				}
			}
			else
			{
#ifdef _DEBUG
				printf("ALPHAMASK ERROR : you can't allow an alpha mask of size X or Y  = 0\n");
#endif
			}

			if(MaskTexture)
			{
				MaskTexture = nullptr;
			}
		}
#ifdef _DEBUG
		else
			printf("ALPHAMASK ERROR : texture pointer = nullptr\n");
#endif
	}
}

Texture* PassiveColoredMask::GetTexture()
{
	if(mTexture)
	{
		return mTexture.get();
	}
	return nullptr;
}

void PassiveColoredMask::ApplyColorFrom(float X, float Y, int R, int G, int B, bool IsRGBA, unsigned int aExtendedRay)
{
	if(mPixelMaskArray != nullptr)
	{
		unsigned int extendedRay = aExtendedRay + mRay;
		float Sx = X - extendedRay;
		float Sy = Y - extendedRay;

		if(Sx < 0.0f)
			Sx = 0.0f;
		if(Sy < 0.0f)
			Sy = 0.0f;

		PLATFORM_COLOR *TheMaskCenterColor = (PLATFORM_COLOR*)mPixelMaskArray+((int)X+(int)Y*mPow2Width);

		unsigned int*	readPixel=mPixelArray+((int)Sx+(int)Sy*mPow2Width);

		int SqrtRay = extendedRay*extendedRay;

		int doubleRay= extendedRay + extendedRay;

		float dist = 0.0f;
		int tabPos = 0;

		float RemoteMitigation = (SqrtRay*60)*0.01f;
		float rest = SqrtRay - RemoteMitigation;
		int Size = mPow2Width*mPow2Height;

		for(int Line = 0; Line < doubleRay; Line ++)
		{
			for(int col = 0; col < doubleRay; col ++)
			{
				dist  = (float) ((Line - extendedRay)*(Line - extendedRay) + (col - extendedRay)*(col - extendedRay));
				if(dist <= SqrtRay)
				{
					//inside ray
					tabPos = (int) (((Sx + (col)) + (Sy+ Line)*mPow2Width));
					if(tabPos > 0 && tabPos < Size)
					{
						PLATFORM_COLOR *TheOriginalColor = (PLATFORM_COLOR*)mPixelArray+tabPos;
						PLATFORM_COLOR *TheMaskColor = (PLATFORM_COLOR*)mPixelMaskArray+tabPos;
						if(IsRGBA)
						{
							if(TheOriginalColor->A)
							{
								if(TheMaskColor->R == TheMaskCenterColor->R && TheMaskColor->G == TheMaskCenterColor->G && TheMaskColor->B == TheMaskCenterColor->B)
								{
									//pixel is not alpha
									//Change It
									if(dist > RemoteMitigation)
									{
										//Slide to original color
										float value = ((dist - RemoteMitigation) * 100/rest)*0.01f;
										float colordivided = 1.0f - value;
										TheOriginalColor->R = (unsigned char)(R * colordivided + TheOriginalColor->R * value);
										TheOriginalColor->G = (unsigned char)(G * colordivided + TheOriginalColor->G * value);
										TheOriginalColor->B = (unsigned char)(B * colordivided + TheOriginalColor->B * value);
									}
									else
									{
										TheOriginalColor->R = (unsigned char)R;
										TheOriginalColor->G = (unsigned char)G;
										TheOriginalColor->B = (unsigned char)B;
									}
								}
							}
						}
						else
						{
							if(TheOriginalColor->R)
							{
								if(TheMaskColor->G == TheMaskCenterColor->G && TheMaskColor->B == TheMaskCenterColor->B && TheMaskColor->A == TheMaskCenterColor->A)
								{
									//pixel is not alpha
									//Change It
									if(dist > RemoteMitigation)
									{
										//Slide to original color
										float value = ((dist - RemoteMitigation) * 100/rest)*0.01f;
										float colordivided = 1.0f - value;
										TheOriginalColor->G = (unsigned char)(R * colordivided + TheOriginalColor->G * value);
										TheOriginalColor->B = (unsigned char)(G * colordivided + TheOriginalColor->B * value);
										TheOriginalColor->A = (unsigned char)(B * colordivided + TheOriginalColor->A * value);
									}
									else
									{
										TheOriginalColor->G = (unsigned char)R;
										TheOriginalColor->B = (unsigned char)G;
										TheOriginalColor->A = (unsigned char)B;
									}
								}
							}
						}
					}
				}
			}
			readPixel = mPixelArray+((int)Sx+((int)Sy+ (Line+1) )*mPow2Width);
		}

		mTexture->SetPixels(mPixelArray,mPow2Width,mPow2Height);
	}
}

void PassiveColoredMask::ApplyColorFrom(float TopLeftCornerX, float TopLeftCornerY, int SizeX, int SizeY, int R, int G, int B, bool IsRGBA)
{
	if(mPixelMaskArray != nullptr)
	{
		if(TopLeftCornerX < 0.0f)
			TopLeftCornerX = 0.0f;
		if(TopLeftCornerY < 0.0f)
			TopLeftCornerY = 0.0f;

		int X, Y = 0;
		int L_TabPos = 0;
		for(X = 0; X < SizeX; X++)
		{
			for(Y = 0; Y < SizeY; Y++)
			{
				L_TabPos = (int) ((TopLeftCornerX+X)+(TopLeftCornerY+Y)*mPow2Width);
				

				PLATFORM_COLOR *TheOriginalColor = (PLATFORM_COLOR*)mPixelArray+L_TabPos;
				if(IsRGBA)
				{
					if(TheOriginalColor->A)
					{
						//pixel is not alpha
						//Change It
						TheOriginalColor->R = (unsigned char)R;
						TheOriginalColor->G = (unsigned char)G;
						TheOriginalColor->B = (unsigned char)B;
					}
				}
				else
				{
					if(TheOriginalColor->R)
					{
						TheOriginalColor->G = (unsigned char)R;
						TheOriginalColor->B = (unsigned char)G;
						TheOriginalColor->A = (unsigned char)B;
					}
				}
			}
		}
		mTexture->SetPixels(mPixelArray,mPow2Width,mPow2Height);	
	}
}


void	PassiveColoredMask::RestoreOriginalColorAt(float TopLeftCornerX, float TopLeftCornerY, int SizeX, int SizeY, bool IsRGBA)
{
	if(mPixelMaskArray != nullptr)
	{
		if(TopLeftCornerX < 0.0f)
			TopLeftCornerX = 0.0f;
		if(TopLeftCornerY < 0.0f)
			TopLeftCornerY = 0.0f;

		int X, Y = 0;
		int L_TabPos = 0;
		for(X = 0; X < SizeX; X++)
		{
			for(Y = 0; Y < SizeY; Y++)
			{
				L_TabPos = (int) ((TopLeftCornerX+X)+(TopLeftCornerY+Y)*mPow2Width);
				

				PLATFORM_COLOR *TheOriginalColor = (PLATFORM_COLOR*)mOriginalPixelArray+L_TabPos;
				PLATFORM_COLOR *TheTextureColor = (PLATFORM_COLOR*)mPixelArray+L_TabPos;
				PLATFORM_COLOR *TheMaskColor = (PLATFORM_COLOR*)mPixelMaskArray+L_TabPos;
				*TheMaskColor = *TheOriginalColor;
				*TheTextureColor = *TheOriginalColor;
			}
		}
		mTexture->SetPixels(mPixelArray,mPow2Width,mPow2Height);	
	}
}

void	PassiveColoredMask::PixelCopyFromMask(int TopLeftCornerX, int TopLeftCornerY, int SizeX, int SizeY)
{
	if(mPixelMaskArray != nullptr)
	{
		if(TopLeftCornerX < 0)
			TopLeftCornerX = 0;
		if(TopLeftCornerY < 0)
			TopLeftCornerY = 0;

		int X, Y = 0;
		int L_TabPos = 0;
		for(X = 0; X < SizeX; X++)
		{
			for(Y = 0; Y < SizeY; Y++)
			{
				L_TabPos = (int) ((TopLeftCornerX+X)+(TopLeftCornerY+Y)*mPow2Width);
				PLATFORM_COLOR *TheTextureColor = (PLATFORM_COLOR*)mPixelArray+L_TabPos;
				PLATFORM_COLOR *TheMaskColor = (PLATFORM_COLOR*)mPixelMaskArray+L_TabPos;
				*TheTextureColor = *TheMaskColor;
			}
		}
		mTexture->SetPixels(mPixelArray,mPow2Width,mPow2Height);	
	}
}
