#include "PrecompiledHeaders.h"
#include "TGAClass.h"
#include "TinyImageLoaderContext.h"
#include "ModuleFileManager.h"


TGAClass::TGAClass(FileHandle* fileName):TinyImage()
{
	myInitIsOK =Load(fileName);
}

TGAClass::~TGAClass()
{
}


// create images from data, to export them
TGAClass::TGAClass(void* data, int sx,int sy,TinyImage::ImageFormat internalfmt) : TinyImage(data,sx,sy,internalfmt)
{
}

void	TGAClass::Export(const char* filename)
{
	if(!myInitIsOK)
	{
		return;
	}

	if((mFormat != RGBA_32_8888)&&(mFormat != RGB_24_888)&&(mFormat != ABGR_16_1555_DIRECT_COLOR))
	{
		return;
	}
	
	tgaheader	towrite;
	towrite.identsize=0;
	towrite.colourmaptype=0;
	towrite.imagetype = NO_COMPRESSION_TRUECOLOR;
	towrite.colourmapstart=0;
	towrite.colourmaplength=0;
	towrite.colourmapbits=0;
	towrite.bits=GetPixelValueSize(mFormat)*8;
	towrite.xstart=towrite.ystart=0;
	towrite.width=myWidth;
	towrite.height=myHeight;
	towrite.descriptor=0;

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

	if(L_File->mFile)
	{
		Platform_fwrite(&towrite.identsize, 1, 1, L_File.get());
		Platform_fwrite(&towrite.colourmaptype, 1, 1, L_File.get());
		Platform_fwrite(&towrite.imagetype, 1, 1, L_File.get());
		Platform_fwrite(&towrite.colourmapstart, 2, 1, L_File.get());
		Platform_fwrite(&towrite.colourmaplength, 2, 1, L_File.get());
		Platform_fwrite(&towrite.colourmapbits, 1, 1, L_File.get());
		Platform_fwrite(&towrite.xstart, 2, 1, L_File.get());
		Platform_fwrite(&towrite.ystart, 2, 1, L_File.get());
		Platform_fwrite(&towrite.width, 2, 1, L_File.get());
		Platform_fwrite(&towrite.height, 2, 1, L_File.get());
		Platform_fwrite(&towrite.bits, 1, 1, L_File.get());
		Platform_fwrite(&towrite.descriptor, 1, 1, L_File.get());
		Platform_fwrite(mPixels,mPixelDataSize,1, L_File.get());
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


bool	TGAClass::Load(FileHandle* fileName)
{
	bool result = false;
	u64 filelength;
	
	if(myInitIsOK)
	{
		if(mPixels)
		{
			delete[] mPixels;
		}
		if(mPalette)
		{
			delete[] mPalette;
		}
		if(mPaletteIndex)
		{
			delete[] mPaletteIndex;
		}
	}

	CoreRawBuffer* rawbuffer=ModuleFileManager::LoadFile(fileName, filelength);
	
	if(rawbuffer)
	{
		u8* memfile=(u8*)rawbuffer->buffer();

		tgaheader header;
		loadHeader(header, memfile);
		
		int	sx=(int)header.width;
		int	sy=(int)header.height;
		myWidth = sx;
		myHeight = sy;
		
		// get screen origin bit
		mIsVFlipped= (header.descriptor&0x40)!=0;
		
		switch(header.imagetype)
		{
			case NO_COMPRESSION_COLORMAPPED:
			case RLE_COMPRESSION_COLORMAPPED:
			{
				ReadColorFunc currentReadPaletteFunc=0;
				ImageFormat	currentReadFormat=NO_TEXTURE;

				ReadColorFunc currentReadFunc=0;
				
				// color bit depth
				u32 colorBytes = (u32)header.colourmapbits>>3;

				switch(colorBytes)
				{
				case 2:
					// not supported for TGA
					currentReadFormat= NO_TEXTURE;
					break;

				case 3:
					currentReadFormat= PALETTE24_4_COLOR;
					break;

				case 4:
					currentReadFormat= PALETTE32_4_COLOR;
					break;
				}

				if(currentReadFormat != NO_TEXTURE)
				{
					switch(header.bits) 
					{
						case 2:
						{
							// stay on current read format for 4 colors
						} break;
						
						case 4:
						{
							// 16 colors
							currentReadFormat=(ImageFormat)(((int)currentReadFormat)+1);
						} break;
						
						case 8:
						{
							// 256 colors
							currentReadFormat=(ImageFormat)(((int)currentReadFormat)+2);
						} break;
					}
				}
				if(currentReadFormat == NO_TEXTURE)
				{
					// not supported
					rawbuffer->Destroy();
					return false;
				}

				// should be always 4 here 
				//int readpaletteValueSize=GetPaletteValueSize(currentReadFormat);

				mFormat=mLoaderContext->myFormatConvertion[(int)currentReadFormat];

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
					currentReadPaletteFunc=GetReadFunction(mFormat,4);
					// allocate palette
					mPalette=(unsigned char*)(new unsigned char[header.colourmaplength*paletteValueSize]);
					mPaletteDataSize=header.colourmaplength*paletteValueSize;
				}
				else
				{
					// read palette in 32 bits format and will be released before exit
					mPalette=(unsigned char*)(new unsigned char[header.colourmaplength*4]);
					mPaletteDataSize=header.colourmaplength*4;
					paletteValueSize=4;
					currentReadFunc=GetReadFunction(mFormat,paletteValueSize);
				}

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
				
				// pointer on file palette
				u8* palette= memfile;

				palette += headerSize;
				// skip image indentification info
				palette += header.identsize;

				// pointer on file pixel data 
				u8* pixels;
				
				// skip header
				palette += header.colourmapstart;
				// skip palette data
				pixels = palette + header.colourmaptype * header.colourmaplength * colorBytes;
				
				unsigned char* currentpalette=(unsigned char*)mPalette;
				if(keepPalette)
				{
					unsigned char copy[4];
					for (int i = 0; i < header.colourmaplength; ++i)
					{ 
						copy[0]=palette[0];
						copy[1]=palette[1];
						copy[2]=palette[2];
						if(i==0)
						{
							copy[3]=0;
						}
						else
						{
							copy[3]=0xff;
						}
						currentReadPaletteFunc(copy,currentpalette);
						palette += colorBytes;
						currentpalette+=paletteValueSize;
					}
				}
				else // just copy conversion will be done during pixel read
				{
					for (int i = 0; i < header.colourmaplength; ++i)
					{ 
						currentpalette[0]=palette[0];
						currentpalette[1]=palette[1];
						currentpalette[2]=palette[2];
						if(i==0)
						{
							currentpalette[3]=0;
						}
						else
						{
							currentpalette[3]=0xff;
						}
						palette += colorBytes;
						currentpalette+=paletteValueSize;
					}
				}
				
				if(header.imagetype==NO_COMPRESSION_COLORMAPPED)
				{
					if(keepPalette)
					{
						// fill pixel data
						memcpy(mPixels, pixels, (int)mPixelDataSize);
					}
					else
					{
						int x,y;
						for(y=0;y<sy;y++)
						{
							unsigned int offset=(unsigned int)(mPixelLineSize*y);
							for(x=0;x<sx;x++)
							{
								currentReadFunc(mPalette+((unsigned int)(*pixels)<<2),&(mPixels[offset]));
								pixels++;
								offset+=outFormatPixelSize;
							}
						}
					}
				}
				else
				{
					// uncompress
					// pointer on image pixel data
					u8* image = mPixels;
					// number of pixel to write
					u32 pixMax = (u32)sx*sy;
					// treated pixels count
					u32 pixCount = 0;

					while(pixCount<pixMax) 
					{
						// get the repetition count
						u8 count= (u8) ((*pixels)&0x7F);
						// read the first color
						if(keepPalette)
						{
							*image++=*(pixels+1);
						}
						else
						{
							currentReadFunc(mPalette+((unsigned int)(*(pixels+1))<<2),image);
							// next pixel in image data
							image+=outFormatPixelSize;
						}
						++pixCount;
						// get the ID bit (high-order bit)
						if((*pixels)&0x80) // Compressed packet
						{
							// skip packet header
							++pixels;
							
							// copy the first color <count> times
							for(u8 j=0;j<count;++j) 
							{
								if(keepPalette)
								{
									*image=*pixels;
								}
								else
								{
									currentReadFunc(mPalette+((unsigned int)(*pixels)<<2),image);
								}
								// next pixel in image data
								image+=outFormatPixelSize;
								++pixCount;
							}
							// skip first color
							pixels++;
						}
						else // Raw packet
						{
							// skip packet header and first color
							pixels+=2;
							
							// read the next <count> color
							for(u8 j=0;j<count;++j) 
							{
								if(keepPalette)
								{
									*image=*pixels;
								}
								else
								{
									currentReadFunc(mPalette+((unsigned int)(*pixels)<<2),image);
								}
								
								image+=outFormatPixelSize;
								++pixCount;
								pixels++;
							}
						} 
					}
				}
				// if not a palette output, remove palette data
				if(!keepPalette)
				{
					delete[] mPalette;
					mPalette=0;
					mPaletteDataSize=0;
				}

				result = true;
			} break;
			
			case NO_COMPRESSION_TRUECOLOR:
			{
				bool byteswap = false;
				ReadColorFunc currentReadFunc;
				// number of bytes to read
				u32 bytes = (u32)header.bits>>3;
				u32 size  = sx*sy*bytes;

				ImageFormat	currentReadFormat=NO_TEXTURE;
				if(bytes == 4)
				{
					currentReadFormat=RGBA_32_8888;
					// do I need to transform from BGRA here ?
				}
				else if(bytes == 3)
				{
					currentReadFormat=RGB_24_888;
					// use byte swapping as 24 bit TGA and BMP are store in BGR format and we want RGB output
					byteswap = true;
				}
				else if(bytes == 2)
				{
					currentReadFormat=ABGR_16_1555_DIRECT_COLOR;
				}
				
				mFormat=mLoaderContext->myFormatConvertion[(int)currentReadFormat];

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
				else
				{
					rawbuffer->Destroy();
					return false;
				}
				

				mPixelDataSize= mPixelLineSize*sy;
				mPixels= new u8[mPixelDataSize];
				
				// pointer on file pixel data 
				u8* pixels = memfile;
				// skip header
				pixels += headerSize;
				// skip image indentification info
				pixels += header.identsize;
				// skip palette data (useless here)
				pixels += header.colourmaptype * header.colourmaplength * (header.colourmapbits>>3);
				
				// pointer on image pixel data
				u8* image = mPixels;

				currentReadFunc = GetReadFunction(mFormat, bytes, byteswap);
				
				for(u32 i=0;i<size;i+=bytes,image+=outFormatPixelSize) 
				{
					currentReadFunc(&(pixels[i]), image);
				}
				
				result = true;
			} break;
			
			case RLE_COMPRESSION_TRUECOLOR:
			{
				ReadColorFunc currentReadFunc;
				u32 bytes = (u32)header.bits>>3;
			
				ImageFormat	currentReadFormat=NO_TEXTURE;
				if(bytes == 4)
				{
					currentReadFormat=RGBA_32_8888;
				}
				else if(bytes == 3)
				{
					currentReadFormat=RGB_24_888;
				}
				
				mFormat=mLoaderContext->myFormatConvertion[(int)currentReadFormat];

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
				else
				{
					rawbuffer->Destroy();
					return false;
				}

				mPixelDataSize= mPixelLineSize*sy;
				mPixels= new u8[mPixelDataSize];
				
				// pointer on file pixel data 
				u8* pixels = memfile;
				// skip header
				pixels += headerSize;
				// skip image indentification info
				pixels += header.identsize;
				// skip palette data (useless here)
				pixels += header.colourmaptype * header.colourmaplength * (header.colourmapbits>>3);
				
				// pointer on image pixel data
				u8* image = mPixels;
				// number of pixel to write
				u32 pixMax = (u32)sx*sy;
				// treated pixels count
				u32 pixCount = 0;
				
				currentReadFunc=GetReadFunction(mFormat,bytes);
				//int outPixelSize=0;

				while(pixCount<pixMax) 
				{
					// get the repetition count
					u8 count= (u8) ((*pixels)&0x7F);
					// read the first color
					currentReadFunc(pixels+1, image);
					// next pixel in image data
					image+=outFormatPixelSize;
					++pixCount;
					// get the ID bit (high-order bit)
					if((*pixels)&0x80) // Compressed packet
					{
						// skip packet header
						++pixels;
						
						// copy the first color <count> times
						for(u8 j=0;j<count;++j) 
						{
							currentReadFunc(pixels, image);
							// next pixel in image data
							image+=outFormatPixelSize;
							++pixCount;
						}
						// skip first color
						pixels+= bytes;
					}
					else // Raw packet
					{
						// skip packet header and first color
						pixels+= 1+bytes;
						
						// read the next <count> color
						for(u8 j=0;j<count;++j) 
						{
							currentReadFunc(pixels, image);
							image+=outFormatPixelSize;
							++pixCount;
							pixels+=bytes;
						}
					} 
				}
				result = true;
			} break;
			
			case NDS_A3I5_TRANSLUCENT:
			case NDS_A5I3_TRANSLUCENT:
			{
				if(header.imagetype==NDS_A3I5_TRANSLUCENT)
				     mFormat= A3I5_TRANSLUCENT;
				else mFormat= A5I3_TRANSLUCENT;
				
				// color bit depth should be 16 bits (no alpha saved in palette)
				u32 colorBytes = (u32)header.colourmapbits>>3;
				
				// each pixel is 1 byte long (AAAIIIII or AAAAAIII)
				mPixelLineSize= (unsigned int)sx;
				mPixelDataSize= mPixelLineSize*sy;
				mPixels= new u8[mPixelDataSize];
				
				// create palette
				mPaletteDataSize= header.colourmaplength*sizeof(unsigned short);
				mPalette=(unsigned char*)(new unsigned short[header.colourmaplength]);
				
				// pointer on file palette
				u8* palette= memfile;
				// pointer on file pixel data 
				u8* pixels;
				
				// skip header
				palette += header.colourmapstart;
				
				// skip palette data
				pixels = palette + header.colourmaptype * header.colourmaplength * colorBytes;
				
				// fill palette
				memcpy(mPalette, palette, (int)mPaletteDataSize);
				
				// fill pixel data
				memcpy(mPixels, pixels, (int)mPixelDataSize);
				
				result = true;
			} break;
			
			case NDS_4X4_COMPRESSED:
			{
				mFormat= COMPRESSED_4X4_TEXEL;
				
				// color bit depth should be 16 bits (nds standart color format)
				//u32 colorBytes = (u32)header.colourmapbits>>3;
				
				// pixel bit depth should be 32 bits (size of a 4x4 compressed texel block)
				//u32 pixelBytes = (u32)header.bits>>3;
				
				// width and height must be multiple of 4
				if(sx&0x03 || sy&0x03) break;
				
				// each 4x4 pixel block is 4 bytes long 
				mPixelLineSize= (unsigned int) (sx>>2);
				mPixelDataSize= mPixelLineSize*sy;
				mPixels= new u8[mPixelDataSize];
				
				// create palette index data
				mPaletteIndex= new unsigned short[(mPixelDataSize>>1)];
				
				// create palette
				mPaletteDataSize= header.colourmaplength*sizeof(unsigned short);
				mPalette=(unsigned char*)(new unsigned short[header.colourmaplength]);
				
				// pointer on file data
				u8* data= memfile;
				
				// skip header
				data+= header.colourmapstart;
				
				// fill palette data
				memcpy(mPalette, data, (int)mPaletteDataSize);
				
				// skip palette
				data+= mPaletteDataSize;
				
				// fill pixel data
				memcpy(mPixels, data, (int)mPixelDataSize);
				
				// skip pixel data
				data+= mPixelDataSize;
				
				// fill palette index data
				memcpy(mPaletteIndex, data, (int)(mPixelDataSize>>1));
				
				// mPalette and mPixels should be read from image color map and data as usual
				// mPaletteIndex should be read from developper field (after image data)
				
				result = true;
			} break;
			
			default:
				// todo: implement more TGA formats
				break;
		}
		
		rawbuffer->Destroy();
	}
	return result;
}


void TGAClass::loadHeader(tgaheader& header, u8* memfile)
{
	header.identsize       = memfile[0];
	header.colourmaptype   = memfile[1];
	header.imagetype       = memfile[2];
	header.colourmapstart  = (s16) (memfile[3]  | (memfile[4]<<8));
	header.colourmaplength = (s16) (memfile[5]  | (memfile[6]<<8));
	header.colourmapbits   = memfile[7];
	header.xstart          = (s16) (memfile[8]  | (memfile[9]<<8));
	header.ystart          = (s16) (memfile[10] | (memfile[11]<<8));
	header.width           = (s16) (memfile[12] | (memfile[13]<<8));
	header.height          = (s16) (memfile[14] | (memfile[15]<<8));
	header.bits            = memfile[16];
	header.descriptor      = memfile[17];
}


