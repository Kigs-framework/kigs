#include "PrecompiledHeaders.h"
#include "PNGClass.h"
#include "TGAClass.h"
#include "TinyImageLoaderContext.h"
#include "ModuleFileManager.h"


PNGClass::PNGClass(FileHandle* fileName) :TinyImage()
{
	myInitIsOK = Load(fileName);
}

PNGClass::PNGClass(CoreRawBuffer* rawbuffer) : TinyImage()
{
	myInitIsOK = Load(rawbuffer);
}

PNGClass::PNGClass(void* data, int sx, int sy, TinyImage::ImageFormat internalfmt) : TinyImage(data, sx, sy, internalfmt)
{
}

PNGClass::~PNGClass()
{
}



#ifdef USE_LIB_PNG
#include "png.h"

typedef struct
{
	unsigned char* data;
	int size;
	int offset;
}tImageSource;

static void pngReadCallback(png_structp png_ptr, png_bytep data, png_size_t length)
{
	tImageSource* isource = (tImageSource*)png_get_io_ptr(png_ptr);

	if ((int)(isource->offset + length) <= isource->size)
	{
		memcpy(data, isource->data + isource->offset, length);
		isource->offset += length;
	}
	else
	{
		png_error(png_ptr, "pngReaderCallback failed");
	}
}


bool PNGClass::Load(CoreRawBuffer* rawbuffer)
{
	bool result = false;
	if (rawbuffer)
	{
		u8* pData = (u8*)rawbuffer->buffer();
		u32 nDatalen = rawbuffer->size();

		png_byte        header[8] = { 0 };
		png_structp     png_ptr = 0;
		png_infop       info_ptr = 0;

		do
		{
			// png header len is 8 bytes
			if (nDatalen < 8)
				break;

			// check the data is png or not
			memcpy(header, pData, 8);
			if (png_sig_cmp(header, 0, 8))
				break;

			// init png_struct
			png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
			if (!png_ptr)
				break;

			// init png_info
			info_ptr = png_create_info_struct(png_ptr);
			if (!info_ptr || setjmp(png_jmpbuf(png_ptr)))
				break;

			// set the read call back function
			tImageSource imageSource;
			imageSource.data = (unsigned char*)pData;
			imageSource.size = (int)nDatalen;
			imageSource.offset = 0;
			png_set_read_fn(png_ptr, &imageSource, pngReadCallback);

			// read png
			// PNG_TRANSFORM_EXPAND: perform set_expand()
			// PNG_TRANSFORM_PACKING: expand 1, 2 and 4-bit samples to bytes
			// PNG_TRANSFORM_STRIP_16: strip 16-bit samples to 8 bits
			// PNG_TRANSFORM_GRAY_TO_RGB: expand grayscale samples to RGB (or GA to RGBA)
			png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND | PNG_TRANSFORM_PACKING
				/*| PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_GRAY_TO_RGB*/, 0);

			int         color_type = 0;
			png_uint_32 nWidth = 0;
			png_uint_32 nHeight = 0;
			int         nBitsPerComponent = 0;
			png_get_IHDR(png_ptr, info_ptr, &nWidth, &nHeight, &nBitsPerComponent, &color_type, 0, 0, 0);

			myWidth = nWidth;
			myHeight = nHeight;
			mPaletteDataSize = 0;
			// init image info

			int componentCount;
			switch (color_type)
			{
			case 0:
			case 4:
				componentCount = 1;
				break;
			case 2:
			case 6:
				componentCount = 3;
				break;
			default:
				return false;
			}

			// add alpha component
			if ((color_type & PNG_COLOR_MASK_ALPHA))
				componentCount++;

			// calc data size and alloc buffer
			mPixelLineSize = nWidth * componentCount;
			mPixelDataSize = mPixelLineSize * nHeight;
			mPixels = new unsigned char[mPixelDataSize];

			png_bytep * rowPointers = png_get_rows(png_ptr, info_ptr);
			for (unsigned int j = 0; j < nHeight; ++j)
			{
				memcpy(mPixels + j * mPixelLineSize, rowPointers[j], mPixelLineSize);
			}

			switch (componentCount)
			{
			case 4:
				mFormat = RGBA_32_8888;
				break;
			case 3:
				mFormat = RGB_24_888;
				break;
			case 2:
				mFormat = AI88;
				break;
			case 1:
				mFormat = GREYSCALE;
			default:
				break;
			}

			result = true;
		} while (0);

		if (png_ptr)
		{
			png_destroy_read_struct(&png_ptr, (info_ptr) ? &info_ptr : 0, 0);
		}
	}
	return result;
}

bool PNGClass::Load(FileHandle* fileName)
{
	bool result = false;

	// free previous image if any
	if (myInitIsOK)
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

	u64 nDatalen;
	CoreRawBuffer* rawbuffer = ModuleFileManager::LoadFile(fileName, nDatalen);
	if (rawbuffer)
	{
		result = Load(rawbuffer);
		rawbuffer->Destroy();
	}
	return result;
}


void	PNGClass::Export(const char* filename)
{
#ifdef WIN32
	FILE *fp = NULL;
	png_structp png_ptr = NULL;
	png_infop info_ptr = NULL;
	png_bytep row = NULL;
	
	SmartPointer<FileHandle> L_File = Platform_fopen(filename, "wb");
	fp = L_File->mFile;
	int mode, lineSize;
	// Initialize write structure
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (png_ptr == NULL) {
		fprintf(stderr, "Could not allocate write struct\n");
		goto finalise;
	}

	// Initialize info structure
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		fprintf(stderr, "Could not allocate info struct\n");
		goto finalise;
	}
	png_init_io(png_ptr, fp);

	mode = PNG_COLOR_TYPE_RGB;
	if (mFormat == TinyImage::RGBA_32_8888)
		mode = PNG_COLOR_TYPE_RGBA;
	else if (mFormat == TinyImage::GREYSCALE)
		mode = PNG_COLOR_TYPE_GRAY;

	// Write header (8 bit colour depth)
	png_set_IHDR(png_ptr, info_ptr, myWidth, myHeight,
		8, mode, PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);
	
	png_write_info(png_ptr, info_ptr);
	
	// Write image data
	// Allocate memory for one row (3 bytes per pixel - RGB)
	lineSize = (mPixelDataSize / myHeight) * sizeof(png_byte);
	row = (png_bytep)malloc(lineSize);

	// Write image data
	for (int y = 0; y<myHeight; y++) {
		//for (x = 0; x<myWidth; x++) {
			memcpy(row, &mPixels[y*lineSize], lineSize);
		//}
		png_write_row(png_ptr, row);
	}

	// End write
	png_write_end(png_ptr, NULL);

finalise:
	if (fp != NULL) fclose(fp);
	if (info_ptr != NULL) png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
	if (png_ptr != NULL) png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
#endif
}

#else

#include "Platform/TinyImage/PNGClassLoader.inl.h"

#endif // USE_LIB_PNG