#include "PrecompiledHeaders.h"
#include "BMPClass.h"
#include "TGAClass.h"
#include "ModuleFileManager.h"
#include "TinyImageLoaderContext.h"
#include "Core.h"

BMPClass::BMPClass(FileHandle* fileName):TinyImage()
{
	mIsVFlipped=false;
	mInitIsOK =Load(fileName);

}

BMPClass::~BMPClass()
{
}


//#ifdef WIN32
// create images from data, to export them
BMPClass::BMPClass(void* data, int sx,int sy,TinyImage::ImageFormat internalfmt) : TinyImage(data,sx,sy,internalfmt)
{
}

void	BMPClass::Export(const char* filename)
{
	if(!mInitIsOK)
	{
		return;
	}

	if((mFormat != RGBA_32_8888)&&(mFormat != RGB_24_888))
	{
		return;
	}
	
	bmpHeader	towrite;
	towrite.mFileSize=sizeof(bmpHeader) + 2 + mPixelDataSize;
	towrite.mReserved = 0;
	towrite.mBitmapDataSize=mPixelDataSize;
	towrite.mBitmapDataOffset=sizeof(bmpHeader)+2;
	towrite.mBitmapHeaderSize=40;
	towrite.mBitPerPixel=GetPixelValueSize(mFormat)*8;
	towrite.mBitplaneCount=1;
	towrite.mColorCount=0;
	towrite.mImportantColorCount=0;
	towrite.mCompression=0;
	towrite.mSizeX=mWidth;
	towrite.mSizeY=mHeight;
	towrite.mHRes=300;
	towrite.mVRes=300;

	unsigned char* convertbuffer = 0;

	// need to transform RGB to BGR before data export
	if (mFormat == RGB_24_888)
	{
		convertbuffer = new unsigned char[mPixelDataSize];
		unsigned char* swaptmp = mPixels;
		mPixels = convertbuffer;
		convertbuffer = swaptmp;

		ReadColorFunc currentReadFunc = TGAClass::GetReadFunction(RGB_24_888, 3, true);

		// pointer on file pixel data 
		u8* pixels = convertbuffer;

		// pointer on image pixel data
		u8* image = mPixels;

		for (u32 i = 0; i<mPixelDataSize; i += 3, image += 3)
		{
			currentReadFunc(&(pixels[i]), image);
		}
	}



	SmartPointer<FileHandle> L_File = Platform_fopen(filename, "wb");
	if (L_File->mFile)
	{
		Platform_fwrite("BM", 2, 1, L_File.get());
		Platform_fwrite(&towrite, sizeof(bmpHeader), 1, L_File.get());
		Platform_fwrite(mPixels, mPixelDataSize, 1, L_File.get());
		Platform_fclose(L_File.get());

	}

	if (convertbuffer)
	{
		unsigned char*	swaptmp = mPixels;
		mPixels = convertbuffer;
		convertbuffer = swaptmp;
		delete[] convertbuffer;
	}

}

//#endif

bool	BMPClass::Load(FileHandle* fileName)
{
	bool result=false;
	u64 filelength;

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

	// warning ! jump two first bytes of the file, so offset are wrongs
	auto rawbuffer = ModuleFileManager::LoadFile(fileName, filelength,2);
	if(rawbuffer)
	{
		u8* memfile=(u8*)rawbuffer->buffer();

		bmpHeader& header=*((bmpHeader*)memfile);

		int bpp= header.mBitPerPixel;
		int	sx=(int)header.mSizeX;
		int	sy=(int)header.mSizeY;

		unsigned int stride= 0;
		int extraBytes= 0;

		// bmp are stored upside down by default
		//mIsVFlipped=true;

		if(sy<0)	// but if sy is negative
		{
			mIsVFlipped=(!mIsVFlipped);	// image is store unflipped
			sy=-sy;
		}

		mWidth = sx;
		mHeight = sy;

		// 4 bits and 8 bits bmps use palette and can be compressed
		if(bpp==4 || bpp==8) 
		{
			ReadColorFunc currentReadPaletteFunc=0;
			ReadColorFunc currentReadFunc=0;
			ImageFormat	currentReadFormat=NO_TEXTURE;
			if(bpp==4) currentReadFormat= PALETTE32_16_COLOR;
			else if(bpp==8) currentReadFormat= PALETTE32_256_COLOR;

			int colorCount=(int)header.mColorCount;
			if(colorCount==0)
			{
				if(bpp==4) colorCount=16;
				else if(bpp==8) colorCount=256;
			}

			// should be always 4 here 
			int readpaletteValueSize=GetPaletteValueSize(currentReadFormat);

			mFormat=mLoaderContext->mFormatConvertion[(int)currentReadFormat];

			// check if we just convert palette or if output format is direct color
			int paletteValueSize=GetPaletteValueSize(mFormat);
			
			bool keepPalette=false;

			if(paletteValueSize>0)
			{
				keepPalette=true;
			}

			
			if(keepPalette)
			{
				// always read 4 bytes (even if alpha is not coded in palette value, we create an alpha for color 0)
				currentReadPaletteFunc=TGAClass::GetReadFunction(mFormat,4);
				// allocate palette
				mPalette=(unsigned char*)(new unsigned char[colorCount*paletteValueSize]);
				mPaletteDataSize=colorCount*paletteValueSize;
			}
			else
			{
				// read palette in 32 bits format and will be released before exit
				mPalette=(unsigned char*)(new unsigned char[colorCount*4]);
				mPaletteDataSize=colorCount*4;
				paletteValueSize=4;
				currentReadFunc=TGAClass::GetReadFunction(mFormat,readpaletteValueSize);
			}

			// allocate pixel data
			if(bpp==4) stride= (unsigned int)sx/2;
			else if (bpp==8) stride= (unsigned int)sx;

			int outFormatPixelSize=GetPixelValueSize(mFormat);

			if(outFormatPixelSize>0)
			{
				mPixelLineSize=sx*outFormatPixelSize;
			}
			else if(outFormatPixelSize==-1)
			{
				mPixelLineSize=(unsigned int)sx>>1;
			}
			else if(outFormatPixelSize==-2)
			{
				mPixelLineSize=(unsigned int)sx>>2;
			}

			mPixelDataSize= mPixelLineSize*sy;
			mPixels= new unsigned char[mPixelDataSize];

			extraBytes= (4-stride)&0x3;

			// fill data
			// set palette color 0 to transparent and other to opaque
			unsigned char* pal = memfile + 0x34;
			
			unsigned char* currentpalette=(unsigned char*)mPalette;
			if(keepPalette)
			{
				unsigned char copy[4];
				for (int i = 0; i < colorCount; ++i)
				{ 
					copy[0]=pal[0];
					copy[1]=pal[1];
					copy[2]=pal[2];
					if(i==0)
					{
						copy[3]=0;
					}
					else
					{
						copy[3]=0xff;
					}
					currentReadPaletteFunc(copy,currentpalette);
					pal += 4;
					currentpalette+=paletteValueSize;
				}
			}
			else // just copy conversion will be done during pixel read
			{
				for (int i = 0; i < colorCount; ++i)
				{ 
					currentpalette[0]=pal[0];
					currentpalette[1]=pal[1];
					currentpalette[2]=pal[2];
					if(i==0)
					{
						currentpalette[3]=0;
					}
					else
					{
						currentpalette[3]=0xff;
					}
					pal += 4;
					currentpalette+=paletteValueSize;
				}
			}
			// pixels
			unsigned char* pxl=memfile + header.mBitmapDataOffset - 2;
			int y;
			unsigned int x;

			// 8 bits RLE compression
			if(header.mCompression == RLE_8BITS)
			{
				// clear pixels
				u8*	pixels=(u8*)mPixels;
				for(y=sy-1;y>=0;y--)
				{
					unsigned int yoffset=(unsigned int)(mPixelLineSize*y);
					for(x=0;x<mPixelLineSize;x++)
					{
						pixels[yoffset+x]=0;
					}
				}
				UncompressRLE8bits(pxl,currentReadFunc,outFormatPixelSize);
				result = true;
			}
			// 4 bits RLE compression
			else if(header.mCompression == RLE_4BITS)
			{
				// clear pixels
				u8*	pixels=(u8*)mPixels;
				for(y=sy-1;y>=0;y--)
				{
					unsigned int yoffset=(unsigned int)(mPixelLineSize*y);
					for(x=0;x<mPixelLineSize;x++)
					{
						pixels[yoffset+x]=0;
					}
				}
				UncompressRLE4bits(pxl,currentReadFunc,outFormatPixelSize);
				result = true;
			}
			// no compression
			else if(header.mCompression == NO_COMPRESSION)
			{	
				u8*	pixels=(u8*)mPixels;
				if (bpp==8) 
				{	
					if(keepPalette)
					{
						for(y=sy-1;y>=0;y--)
						{
							unsigned int yoffset=(unsigned int)(stride*y);
							for(x=0;x<stride;x++)
							{
								pixels[yoffset+x]=*pxl;
								pxl++;
							}

							pxl+= extraBytes;
						}
					}
					else // convert to bitmap without palette
					{
						unsigned char* currentpalette=(unsigned char*)mPalette;
						for(y=sy-1;y>=0;y--)
						{
							unsigned int offset=(unsigned int)(mPixelLineSize*y);
							for(x=0;x<stride;x++)
							{
								currentReadFunc(currentpalette+((unsigned int)(*pxl)<<2),&(pixels[offset]));
								pxl++;
								offset+=outFormatPixelSize;
							}

							pxl+= extraBytes;
						}
					}
				}
				else if(bpp==4) 
				{
					if(keepPalette)
					{
						for(y=sy-1;y>=0;y--)
						{
							unsigned int yoffset=(unsigned int)(stride*y);
							for(x=0;x<stride;x++)
							{
								// switch pixels positions
								pixels[yoffset+x]= (u8)( ((*pxl)<<4) | ((*pxl)>>4) );
								pxl++;
							}

							pxl+= extraBytes;
						}
					}
					else
					{
						unsigned char* currentpalette=(unsigned char*)mPalette;
						for(y=sy-1;y>=0;y--)
						{
							unsigned int offset=(unsigned int)(mPixelLineSize*y);
							for(x=0;x<stride;x++)
							{
								unsigned int p2=((*pxl)&0xF);
								unsigned int p1=((*pxl)>>4)&0xF;

								currentReadFunc(currentpalette+((unsigned int)(p1)<<2),&(pixels[offset]));
								offset+=outFormatPixelSize;
								currentReadFunc(currentpalette+((unsigned int)(p2)<<2),&(pixels[offset]));
								offset+=outFormatPixelSize;

								pxl++;
							}

							pxl+= extraBytes;
						}
					}
				}
				result = true;
			}

			// if not a palette output, remove palette data
			if(!keepPalette)
			{
				delete[] mPalette;
				mPalette=0;
				mPaletteDataSize=0;
			}

		}
		else if(header.mCompression == NO_COMPRESSION)	// for 16, 24 or 32 bit don't use palette or compression
		{
			if((bpp==16) || (bpp==24) || (bpp==32))
			{
				ReadColorFunc currentReadFunc;
				bool byteswap=false;

				ImageFormat	currentReadFormat=NO_TEXTURE;
				if(bpp == 32)
				{
					currentReadFormat=RGBA_32_8888;
					// do I need to transform from BGRA here ?
				}
				else if(bpp == 24)
				{
					// use byte swapping as 24 bit TGA and BMP are store in BGR format and we want RGB output
					currentReadFormat=RGB_24_888;
					byteswap = true;
				}
				else 
				{
					currentReadFormat=RGB_16_565;
				}

				mFormat=mLoaderContext->mFormatConvertion[(int)currentReadFormat];

				int outFormatPixelSize=GetPixelValueSize(mFormat);

				if(outFormatPixelSize>0)
				{
					mPixelLineSize=sx*outFormatPixelSize;
				}
				else if(outFormatPixelSize==-1)
				{
					mPixelLineSize=(unsigned int)sx>>1;
				}
				else if(outFormatPixelSize==-2)
				{
					mPixelLineSize=(unsigned int)sx>>2;
				}
				
				mPixelDataSize= mPixelLineSize*sy;
				mPixels= new u8[mPixelDataSize];

				// pointer on file pixel data 
				u8* pixels = memfile + header.mBitmapDataOffset - 2;

				// pointer on image pixel data
				u8* image = mPixels;

				// number of bytes to read
				u32 bytes = (u32)bpp>>3;
				u32 size  = sx*sy*bytes;
				
				currentReadFunc = TGAClass::GetReadFunction(mFormat, bytes, byteswap);

				for(u32 i=0;i<size;i+=bytes,image+=outFormatPixelSize) 
				{
					currentReadFunc(&(pixels[i]), image);
				}
				
				mIsVFlipped=(!mIsVFlipped);

				result = true;
			}		
		}
	}

	if(result == false)
	{
		KIGS_ERROR("Unsupported BMP format",1);
	}

	return result;
}

void	BMPClass::UncompressRLE8bits(u8* bytes,ReadColorFunc func,unsigned int outPixelSize)
{
	u8*	pixels=(u8*)mPixels;
	u8* readval=bytes;

	int x=0;
	int y=0;

	bool	finished=false;

	while(!finished)
	{
		switch(*readval)
		{
		case 0:	// escape code
			{	
				++readval;
				switch(*readval)
				{
				case 0:	// line end
					{
						++y;
						x=0;
						if(y>=mHeight) finished=true;
					}
					break;

				case 1:	// bitmap end
					{
						finished=true;
					}
					break;

				case 2:	// delta shift
					{
						++readval;
						x+=(*readval);
						++readval;
						y+=(*readval);
						if(y>=mHeight) 	finished=true;
					}
					break;

				default:	// copy directly next bytes
					{
						int count=*readval;
						int i;
						for(i=0;i<count;++i)
						{
							++readval;
							if(func)
							{
								// convert
								unsigned int pixelvalOffset=(unsigned int)(*readval)<<2;
								unsigned int pixelOffset=outPixelSize*(x+((mHeight-1)-y)*mWidth);

								func(&(mPalette[pixelvalOffset]),&(pixels[pixelOffset]));
							}
							else
							{
								pixels[x+((mHeight-1)-y)*mWidth]= *readval;
							}
							++x;
							if(x>=mWidth) x=0;
						}
						// word aligned
						if(count&1) ++readval;
					}
					break;
				}
			}
			break;
		default: // no escape code, RLE decode
			{				
				int count=*readval;
				++readval;

				for(int i=0;i<count;++i)
				{
					if(func)
					{
						// convert
						unsigned int pixelvalOffset=(unsigned int)(*readval)<<2;
						unsigned int pixelOffset=outPixelSize*(x+((mHeight-1)-y)*mWidth);

						func(&(mPalette[pixelvalOffset]),&(pixels[pixelOffset]));
					}
					else
					{
						pixels[x+((mHeight-1)-y)*mWidth]= *readval;
					}
					++x;
					if(x>=mWidth) x=0;
				}
			}
			break;			
		}
		++readval;
	}
}


void	BMPClass::UncompressRLE4bits(u8* bytes,ReadColorFunc func,unsigned int outPixelSize)
{
	u8*	pixels=(u8*)mPixels;
	u8* readval=bytes;

	int x=0;
	int y=0;

	bool	finished=false;

	while(!finished)
	{
		switch(*readval)
		{
		case 0:	// escape code
			{	
				++readval;
				switch(*readval)
				{
				case 0:	// line end
					{
						++y;
						x=0;
						if(y>=mHeight) finished=true;
					}
					break;

				case 1:	// bitmap end
					{
						finished=true;
					}
					break;

				case 2:	// delta shift
					{
						++readval;
						x+=(*readval);
						++readval;
						y+=(*readval);
						if(y>=mHeight) 	finished=true;
					}
					break;

				default:	// copy directly next bytes
					{
						int count=*readval;
						int i;
						for(i=0;i<count;++i)
						{
							if(!(i&1))
							{
								++readval;
								if(func)
								{
									unsigned int p2=((*readval)&0xF);
									unsigned int p1=((*readval)>>4)&0xF;

									// convert
									unsigned int pixelvalOffset=(unsigned int)(p1)<<2;
									unsigned int pixelOffset=outPixelSize*(x+((mHeight-1)-y)*mWidth);

									func(&(mPalette[pixelvalOffset]),&(pixels[pixelOffset]));

									pixelvalOffset=(unsigned int)(p2)<<2;
									pixelOffset=outPixelSize*(x+1+((mHeight-1)-y)*mWidth);

									func(&(mPalette[pixelvalOffset]),&(pixels[pixelOffset]));
								}
								else
								{
									pixels[(x+((mHeight-1)-y)*mWidth)>>1]= (u8) ( (((*readval)>>4)&0x0F) | ((*readval)<<4) );
								}
							}
							
							++x;
							if(x>=mWidth) x=0;
						}
						// word aligned
						if(count&1) ++readval;
					}
					break;
				}
			}
			break;
		default: // no escape code, RLE decode
			{				
				int count=*readval;
				++readval;

				for(int i=0;i<count;++i)
				{
					if(!(i&1))
					{
						if(func)
						{
							unsigned int p2=((*readval)&0xF);
							unsigned int p1=((*readval)>>4)&0xF;

							// convert
							unsigned int pixelvalOffset=(unsigned int)(p1)<<2;
							unsigned int pixelOffset=outPixelSize*(x+((mHeight-1)-y)*mWidth);

							func(&(mPalette[pixelvalOffset]),&(pixels[pixelOffset]));

							pixelvalOffset=(unsigned int)(p2)<<2;
							pixelOffset=outPixelSize*(x+1+((mHeight-1)-y)*mWidth);

							func(&(mPalette[pixelvalOffset]),&(pixels[pixelOffset]));
						}
						else
						{
							pixels[(x+((mHeight-1)-y)*mWidth)>>1]= (u8) ( (((*readval)>>4)&0x0F) | ((*readval)<<4) );
						}
					}
					++x;
					if(x>=mWidth) x=0;
				}
			}
			break;			
		}
		++readval;
	}
}
