#include "UI/PassiveColoredMask.h"
#include "TextureFileManager.h"
#include "Texture.h"
#include <stdlib.h>

//IMPLEMENT_AND_REGISTER_CLASS_INFO(PassiveColoredMask, PassiveColoredMask, 2DLayers);
IMPLEMENT_CLASS_INFO(PassiveColoredMask)

//! constructor, init all parameters
PassiveColoredMask::PassiveColoredMask(const kstl::string& name,CLASS_NAME_TREE_ARG)
:UIItem(name,PASS_CLASS_NAME_TREE_ARG)
,myThreshold(*this,true,LABEL_AND_ID(Threshold),0)
,myTextureName(*this,false,LABEL_AND_ID(TextureName),"")
,myMaskName(*this,false,LABEL_AND_ID(MaskName),"")
,myRay(*this,true,LABEL_AND_ID(Ray),0)
{
	myIsDown = false;
	myIsMouseOver = false;
	myPixelArray = nullptr;
	myPixelMaskArray = nullptr;
}

PassiveColoredMask::~PassiveColoredMask()
{
	if(myTexture)
		myTexture = nullptr;

	if(myPixelArray)
	{
		free(myPixelArray);
		myPixelArray = nullptr;
	}
	if(myPixelMaskArray)
	{
		free(myPixelMaskArray);
		myPixelMaskArray = nullptr;
	}
	if(myOriginalPixelArray)
	{
		free(myOriginalPixelArray);
		myOriginalPixelArray = nullptr;
	}
}

void	PassiveColoredMask::ReloadTexture()
{
	if(myTexture)
	{
		myTexture->ReInit();
	}
}

void	PassiveColoredMask::InitModifiable()
{
	UIItem::InitModifiable();
	if(IsInit())
	{
		myIsDown      = false;
		myIsMouseOver = false;

		auto& textureManager = KigsCore::Singleton<TextureFileManager>();
		myTexture = textureManager->GetTexture(myTextureName);

		if(myTexture)
		{
			myTexture->getValue("Width",myWidth);
			myTexture->getValue("Height",myHeight);

			myTexture->getValue("Pow2Width",myPow2Width);
			myTexture->getValue("Pow2Height",myPow2Height);

            int width, height, pow2width, pow2height;
            width = height = pow2width = pow2height = 0;
            
			SmartPointer<Texture>	MaskTexture;
			if(myMaskName.const_ref() != "")
			{
				MaskTexture = textureManager->GetTexture(myMaskName);
				MaskTexture->getValue("Width",width);
				MaskTexture->getValue("Height",height);

				MaskTexture->getValue("Pow2Width",pow2width);
				MaskTexture->getValue("Pow2Height",pow2height);

			}

			if(myWidth > 0 && myHeight > 0)
			{
				//Allow temp buffer to get pixel data
				myPixelArray  = nullptr;
				myPixelArray  = (unsigned int*) malloc((myPow2Width*myPow2Height)*sizeof(unsigned int));
				myOriginalPixelArray = (unsigned int*) malloc((myPow2Width*myPow2Height)*sizeof(unsigned int));
				

				myTexture->GetPixels(myPixelArray);
				memcpy(myOriginalPixelArray,myPixelArray,(myPow2Width*myPow2Height)*sizeof(unsigned int));
				if(MaskTexture)
				{
					myPixelMaskArray = (unsigned int*) malloc((pow2width*pow2height)*sizeof(unsigned int));
					MaskTexture->GetPixels(myPixelMaskArray);

				}

				// auto size button
				if( (((unsigned int)mySizeX)==0) && (((unsigned int)mySizeY)==0) )
				{
					if(myTexture)
					{
						float width,height;
						myTexture->GetSize(width,height);
						mySizeX=width;
						mySizeY=height;
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
	if(myTexture)
	{
		return myTexture.get();
	}
	return nullptr;
}

void PassiveColoredMask::ApplyColorFrom(float X, float Y, int R, int G, int B, bool IsRGBA, unsigned int aExtendedRay)
{
	if(myPixelMaskArray != nullptr)
	{
		unsigned int extendedRay = aExtendedRay + myRay;
		float Sx = X - extendedRay;
		float Sy = Y - extendedRay;

		if(Sx < 0.0f)
			Sx = 0.0f;
		if(Sy < 0.0f)
			Sy = 0.0f;

		PLATFORM_COLOR *TheMaskCenterColor = (PLATFORM_COLOR*)myPixelMaskArray+((int)X+(int)Y*myPow2Width);

		unsigned int*	readPixel=myPixelArray+((int)Sx+(int)Sy*myPow2Width);

		int SqrtRay = extendedRay*extendedRay;

		int doubleRay= extendedRay + extendedRay;

		float dist = 0.0f;
		int tabPos = 0;

		float RemoteMitigation = (SqrtRay*60)*0.01f;
		float rest = SqrtRay - RemoteMitigation;
		int Size = myPow2Width*myPow2Height;

		for(int Line = 0; Line < doubleRay; Line ++)
		{
			for(int col = 0; col < doubleRay; col ++)
			{
				dist  = (float) ((Line - extendedRay)*(Line - extendedRay) + (col - extendedRay)*(col - extendedRay));
				if(dist <= SqrtRay)
				{
					//inside ray
					tabPos = (int) (((Sx + (col)) + (Sy+ Line)*myPow2Width));
					if(tabPos > 0 && tabPos < Size)
					{
						PLATFORM_COLOR *TheOriginalColor = (PLATFORM_COLOR*)myPixelArray+tabPos;
						PLATFORM_COLOR *TheMaskColor = (PLATFORM_COLOR*)myPixelMaskArray+tabPos;
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
			readPixel = myPixelArray+((int)Sx+((int)Sy+ (Line+1) )*myPow2Width);
		}

		myTexture->SetPixels(myPixelArray,myPow2Width,myPow2Height);
	}
}

void PassiveColoredMask::ApplyColorFrom(float TopLeftCornerX, float TopLeftCornerY, int SizeX, int SizeY, int R, int G, int B, bool IsRGBA)
{
	if(myPixelMaskArray != nullptr)
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
				L_TabPos = (int) ((TopLeftCornerX+X)+(TopLeftCornerY+Y)*myPow2Width);
				

				PLATFORM_COLOR *TheOriginalColor = (PLATFORM_COLOR*)myPixelArray+L_TabPos;
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
		myTexture->SetPixels(myPixelArray,myPow2Width,myPow2Height);	
	}
}


void	PassiveColoredMask::RestoreOriginalColorAt(float TopLeftCornerX, float TopLeftCornerY, int SizeX, int SizeY, bool IsRGBA)
{
	if(myPixelMaskArray != nullptr)
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
				L_TabPos = (int) ((TopLeftCornerX+X)+(TopLeftCornerY+Y)*myPow2Width);
				

				PLATFORM_COLOR *TheOriginalColor = (PLATFORM_COLOR*)myOriginalPixelArray+L_TabPos;
				PLATFORM_COLOR *TheTextureColor = (PLATFORM_COLOR*)myPixelArray+L_TabPos;
				PLATFORM_COLOR *TheMaskColor = (PLATFORM_COLOR*)myPixelMaskArray+L_TabPos;
				*TheMaskColor = *TheOriginalColor;
				*TheTextureColor = *TheOriginalColor;
			}
		}
		myTexture->SetPixels(myPixelArray,myPow2Width,myPow2Height);	
	}
}

void	PassiveColoredMask::PixelCopyFromMask(int TopLeftCornerX, int TopLeftCornerY, int SizeX, int SizeY)
{
	if(myPixelMaskArray != nullptr)
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
				L_TabPos = (int) ((TopLeftCornerX+X)+(TopLeftCornerY+Y)*myPow2Width);
				PLATFORM_COLOR *TheTextureColor = (PLATFORM_COLOR*)myPixelArray+L_TabPos;
				PLATFORM_COLOR *TheMaskColor = (PLATFORM_COLOR*)myPixelMaskArray+L_TabPos;
				*TheTextureColor = *TheMaskColor;
			}
		}
		myTexture->SetPixels(myPixelArray,myPow2Width,myPow2Height);	
	}
}
