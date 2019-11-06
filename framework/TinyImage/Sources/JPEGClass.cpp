#include "PrecompiledHeaders.h"
#include "JPEGClass.h"
#include "TGAClass.h"
#include "TinyImageLoaderContext.h"
#include "ModuleFileManager.h"

JPEGClass::JPEGClass(FileHandle* fileName) :TinyImage()
{
	myInitIsOK = Load(fileName);
}

JPEGClass::JPEGClass(CoreRawBuffer* rawbuffer) : TinyImage()
{
	myInitIsOK = Load(rawbuffer);
}

JPEGClass::~JPEGClass()
{
}


#ifdef USE_LIB_JPG


#include "jpeglib.h"
#include <setjmp.h>

/*
* ERROR HANDLING:
*
* The JPEG library's standard error handler (jerror.c) is divided into
* several "methods" which you can override individually. This lets you
* adjust the behavior without duplicating a lot of code, which you might
* have to update with each future release.
*
* Our example here shows how to override the "error_exit" method so that
* control is returned to the library's caller when a fatal error occurs,
* rather than calling exit() as the standard error_exit method does.
*
* We use C's setjmp/longjmp facility to return control. This means that the
* routine which calls the JPEG library must first execute a setjmp() call to
* establish the return point. We want the replacement error_exit to do a
* longjmp(). But we need to make the setjmp buffer accessible to the
* error_exit routine. To do this, we make a private extension of the
* standard JPEG error handler object. (If we were using C++, we'd say we
* were making a subclass of the regular error handler.)
*
* Here's the extended error handler struct:
*/
struct my_error_mgr {
	struct jpeg_error_mgr pub; /* "public" fields */
	jmp_buf setjmp_buffer; /* for return to caller */
};
typedef struct my_error_mgr * my_error_ptr;
/*
* Here's the routine that will replace the standard error_exit method:
*/
METHODDEF(void)
my_error_exit(j_common_ptr cinfo)
{
	/* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
	my_error_ptr myerr = (my_error_ptr)cinfo->err;
	/* Always display the message. */
	/* We could postpone this until after returning, if we chose. */
	(*cinfo->err->output_message) (cinfo);
	/* Return control to the setjmp point */
	longjmp(myerr->setjmp_buffer, 1);
}

#if !defined(UNITY_BUILD) || !UNITY_BUILD
typedef struct
{
	unsigned char* data;
	int size;
	int offset;
}tImageSource;
#endif

bool JPEGClass::Load(CoreRawBuffer* rawbuffer)
{
	bool result = false;

	if (rawbuffer)
	{
		u8* pData = (u8*)rawbuffer->buffer();
		u64 nDatalen = rawbuffer->size();


		/* This struct contains the JPEG decompression parameters and pointers to
		* working space (which is allocated as needed by the JPEG library).
		*/
		struct jpeg_decompress_struct cinfo;

		/* We use our private extension JPEG error handler.
		* Note that this struct must live as long as the main JPEG parameter
		* struct, to avoid dangling-pointer problems.
		*/
		struct my_error_mgr jerr;

		/* More stuff */
		/* Step 1: allocate and initialize JPEG decompression object */
		/* We set up the normal JPEG error routines, then override error_exit. */
		jpeg_create_decompress(&cinfo);
		cinfo.err = jpeg_std_error(&jerr.pub);
		jerr.pub.error_exit = my_error_exit;
		//cinfo.do_fancy_upsampling = FALSE;

		/* Establish the setjmp return context for my_error_exit to use. */
		if (setjmp(jerr.setjmp_buffer))
		{
			/* If we get here, the JPEG code has signaled an error.
			* We need to clean up the JPEG object, close the input file, and return.
			*/
			jpeg_destroy_decompress(&cinfo);
			//fclose(infile);
			return 0;
		}

		/* set source buffer */

		jpeg_mem_src(&cinfo, pData, (unsigned long)nDatalen);

		jpeg_read_header(&cinfo, TRUE);
		/* We can ignore the return value from jpeg_read_header since
		* (a) suspension is not possible with the stdio data source, and
		* (b) we passed TRUE to reject a tables-only JPEG file as an error.
		* See libjpeg.txt for more info.
		*/

		/* Step 4: set parameters for decompression */
		/* In this example, we don't need to change any of the defaults set by
		* jpeg_read_header(), so we do nothing here.
		*/

		/* Step 5: Start decompressor */
		jpeg_start_decompress(&cinfo);
		/* We can ignore the return value since suspension is not possible
		* with the stdio data source.
		*/
		/* We may need to do some setup of our own at this point before reading
		* the data. After jpeg_start_decompress() we have the correct scaled
		* output image dimensions available, as well as the output colormap
		* if we asked for color quantization.
		* In this example, we need to make an output work buffer of the right size.
		*/

		myWidth = cinfo.output_width;
		myHeight = cinfo.output_height;
		mPaletteDataSize = 0;
		mPixelLineSize = cinfo.output_width * cinfo.output_components;
		mPixelDataSize = mPixelLineSize * cinfo.output_height;
		mFormat = RGB_24_888;

		mPixels = new u8[mPixelDataSize];

		while (cinfo.output_scanline < cinfo.image_height)
		{
			u8* p = mPixels + cinfo.output_scanline*cinfo.image_width*cinfo.num_components;
			jpeg_read_scanlines(&cinfo, &p, 1);
		}

		/* Step 7: Finish decompression */
		jpeg_finish_decompress(&cinfo);
		/* We can ignore the return value since suspension is not possible
		* with the stdio data source.
		*/

		/* Step 8: Release JPEG decompression object */
		/* This is an important step since it will release a good deal of memory. */
		jpeg_destroy_decompress(&cinfo);

		result = true;

	}
	return result;
}

bool JPEGClass::Load(FileHandle* fileName)
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
	auto rawbuffer = OwningRawPtrToSmartPtr(ModuleFileManager::LoadFile(fileName, nDatalen));
	result = Load(rawbuffer.get());

	return result;
}



#else

#include "Platform/TinyImage/JPGClassLoader.inl.h"

#endif