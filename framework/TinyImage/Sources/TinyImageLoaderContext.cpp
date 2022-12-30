#include "PrecompiledHeaders.h"
#include "TinyImageLoaderContext.h"



namespace Kigs
{

	namespace Pict
	{

		// TODO : set this somewhere else ?

#ifdef WIN32

		TinyImageLoaderContext	gDefaultContext = {
			{
													TinyImage::NO_TEXTURE,								//NO_TEXTURE,
													TinyImage::A3I5_TRANSLUCENT,						//A3I5_TRANSLUCENT	
													TinyImage::PALETTE16_4_COLOR,						//PALETTE16_4_COLOR			(ABGR 1555)
													TinyImage::PALETTE16_16_COLOR,						//PALETTE16_16_COLOR		(ABGR 1555)
													TinyImage::PALETTE16_256_COLOR,						//PALETTE16_256_COLOR		(ABGR 1555)
													TinyImage::COMPRESSED_4X4_TEXEL,					//COMPRESSED_4X4_TEXEL
													TinyImage::A5I3_TRANSLUCENT,						//A5I3_TRANSLUCENT	
													TinyImage::RGBA_32_8888,							//ABGR_16_1555_DIRECT_COLOR
													TinyImage::RGBA_32_8888,							//RGBA_32_8888
													TinyImage::RGB_24_888,								//RGB_24_888
													TinyImage::RGBA_32_8888,							//RGBA_16_4444
													TinyImage::RGBA_32_8888,							//BGRA_16_5551
													TinyImage::RGBA_32_8888,							//RGBA_16_5551
													TinyImage::RGB_24_888,								//RGB_16_565
													TinyImage::COMPRESSED_PVRTC,						//COMPRESSED_PVRTC	
													TinyImage::PALETTE16_4_COLOR,						//PALETTE32_4_COLOR
													TinyImage::PALETTE16_16_COLOR,						//PALETTE32_16_COLOR
													TinyImage::PALETTE16_256_COLOR,						//PALETTE32_256_COLOR
													TinyImage::PALETTE16_4_COLOR,						//PALETTE24_4_COLOR	
													TinyImage::PALETTE16_16_COLOR,						//PALETTE24_16_COLOR
													TinyImage::PALETTE16_256_COLOR,						//PALETTE24_256_COLOR
													TinyImage::GREYSCALE									//GREYSCALE
			}
		};



#elif ANDROID
		TinyImageLoaderContext	gDefaultContext = {
			{
													TinyImage::NO_TEXTURE,								//NO_TEXTURE,
													TinyImage::A3I5_TRANSLUCENT,						//A3I5_TRANSLUCENT	
													TinyImage::RGBA_16_5551,							//PALETTE16_4_COLOR			(ABGR 1555)
													TinyImage::RGBA_16_5551,							//PALETTE16_16_COLOR		(ABGR 1555)
													TinyImage::RGBA_16_5551,							//PALETTE16_256_COLOR		(ABGR 1555)
													TinyImage::COMPRESSED_4X4_TEXEL,					//COMPRESSED_4X4_TEXEL
													TinyImage::A5I3_TRANSLUCENT,						//A5I3_TRANSLUCENT	
													TinyImage::RGBA_16_5551,							//ABGR_16_1555_DIRECT_COLOR
													TinyImage::RGBA_32_8888,							//RGBA_32_8888
													TinyImage::RGB_16_565,								//RGB_24_888
													TinyImage::RGBA_16_4444,							//RGBA_16_4444
													TinyImage::RGBA_16_5551,							//BGRA_16_5551
													TinyImage::RGBA_16_5551,							//RGBA_16_5551
													TinyImage::RGB_16_565,								//RGB_16_565
													TinyImage::COMPRESSED_PVRTC,						//COMPRESSED_PVRTC	
													TinyImage::RGBA_16_5551,							//PALETTE32_4_COLOR
													TinyImage::RGBA_16_5551,							//PALETTE32_16_COLOR
													TinyImage::RGBA_16_5551,							//PALETTE32_256_COLOR
													TinyImage::RGBA_16_5551,							//PALETTE24_4_COLOR	
													TinyImage::RGBA_16_5551,							//PALETTE24_16_COLOR
													TinyImage::RGBA_16_5551								//PALETTE24_256_COLOR
		}
		};

#elif RIM
		TinyImageLoaderContext	gDefaultContext = {
			{
													TinyImage::NO_TEXTURE,								//NO_TEXTURE,
													TinyImage::A3I5_TRANSLUCENT,						//A3I5_TRANSLUCENT	
													TinyImage::RGBA_16_5551,						//PALETTE16_4_COLOR			(ABGR 1555)
													TinyImage::RGBA_16_5551,						//PALETTE16_16_COLOR		(ABGR 1555)
													TinyImage::RGBA_16_5551,						//PALETTE16_256_COLOR		(ABGR 1555)
													TinyImage::COMPRESSED_4X4_TEXEL,					//COMPRESSED_4X4_TEXEL
													TinyImage::A5I3_TRANSLUCENT,						//A5I3_TRANSLUCENT	
													TinyImage::RGBA_16_5551,							//ABGR_16_1555_DIRECT_COLOR
													TinyImage::RGBA_16_4444,							//RGBA_32_8888
													TinyImage::RGB_16_565,								//RGB_24_888
													TinyImage::RGBA_16_4444,							//RGBA_16_4444
													TinyImage::RGBA_16_5551,							//BGRA_16_5551
													TinyImage::RGBA_16_5551,							//RGBA_16_5551
													TinyImage::RGB_16_565,								//RGB_16_565
													TinyImage::COMPRESSED_PVRTC,						//COMPRESSED_PVRTC	
													TinyImage::RGBA_16_5551,						//PALETTE32_4_COLOR
													TinyImage::RGBA_16_5551,						//PALETTE32_16_COLOR
													TinyImage::RGBA_16_5551,						//PALETTE32_256_COLOR
													TinyImage::RGBA_16_5551,						//PALETTE24_4_COLOR	
													TinyImage::RGBA_16_5551,						//PALETTE24_16_COLOR
													TinyImage::RGBA_16_5551						//PALETTE24_256_COLOR
		}
		};

#elif __APPLE__

		TinyImageLoaderContext	gDefaultContext = {
			{
													TinyImage::NO_TEXTURE,								//NO_TEXTURE,
													TinyImage::A3I5_TRANSLUCENT,						//A3I5_TRANSLUCENT	
													TinyImage::PALETTE16_4_COLOR,						//PALETTE16_4_COLOR			(ABGR 1555)
													TinyImage::PALETTE16_16_COLOR,						//PALETTE16_16_COLOR		(ABGR 1555)
													TinyImage::PALETTE16_256_COLOR,						//PALETTE16_256_COLOR		(ABGR 1555)
													TinyImage::COMPRESSED_4X4_TEXEL,					//COMPRESSED_4X4_TEXEL
													TinyImage::A5I3_TRANSLUCENT,						//A5I3_TRANSLUCENT	

													/*TinyImage::RGBA_16_4444,							//ABGR_16_1555_DIRECT_COLOR
													TinyImage::RGBA_16_4444,							//RGBA_32_8888
													TinyImage::RGB_16_565,								//RGB_24_888
													TinyImage::RGBA_16_4444,							//RGBA_16_4444
													TinyImage::RGBA_16_4444,							//BGRA_16_5551
													TinyImage::RGBA_16_4444,							//RGBA_16_5551
													TinyImage::RGB_16_565,	*/							//RGB_16_565

													TinyImage::RGBA_16_5551,							//ABGR_16_1555_DIRECT_COLOR
													TinyImage::RGBA_16_4444,							//RGBA_32_8888
													TinyImage::RGB_16_565,								//RGB_24_888
													TinyImage::RGBA_16_4444,							//RGBA_16_4444
													TinyImage::RGBA_16_5551,							//BGRA_16_5551
													TinyImage::RGBA_16_5551,							//RGBA_16_5551
													TinyImage::RGB_16_565,								//RGB_16_565

													TinyImage::COMPRESSED_PVRTC,						//COMPRESSED_PVRTC	
													TinyImage::PALETTE16_4_COLOR,						//PALETTE32_4_COLOR
													TinyImage::PALETTE16_16_COLOR,						//PALETTE32_16_COLOR
													TinyImage::PALETTE16_256_COLOR,						//PALETTE32_256_COLOR
													TinyImage::PALETTE16_4_COLOR,						//PALETTE24_4_COLOR	
													TinyImage::PALETTE16_16_COLOR,						//PALETTE24_16_COLOR
													TinyImage::PALETTE16_256_COLOR						//PALETTE24_256_COLOR
		}
		};

#elif JAVASCRIPT
		TinyImageLoaderContext	gDefaultContext = {
												{
													TinyImage::NO_TEXTURE,								//NO_TEXTURE,
													TinyImage::RGBA_32_8888,							//A3I5_TRANSLUCENT	
													TinyImage::RGBA_32_8888,							//PALETTE16_4_COLOR			(ABGR 1555)
													TinyImage::RGBA_32_8888,							//PALETTE16_16_COLOR		(ABGR 1555)
													TinyImage::RGBA_32_8888,							//PALETTE16_256_COLOR		(ABGR 1555)
													TinyImage::RGBA_32_8888,							//COMPRESSED_4X4_TEXEL
													TinyImage::RGBA_32_8888,							//A5I3_TRANSLUCENT	
													TinyImage::RGBA_32_8888,							//ABGR_16_1555_DIRECT_COLOR
													TinyImage::RGBA_32_8888,							//RGBA_32_8888
													TinyImage::RGBA_32_8888,							//RGB_24_888
													TinyImage::RGBA_32_8888,							//RGBA_16_4444
													TinyImage::RGBA_32_8888,							//BGRA_16_5551
													TinyImage::RGBA_32_8888,							//RGBA_16_5551
													TinyImage::RGBA_32_8888,							//RGB_16_565
													TinyImage::RGBA_32_8888,							//COMPRESSED_PVRTC	
													TinyImage::RGBA_32_8888,							//PALETTE32_4_COLOR
													TinyImage::RGBA_32_8888,							//PALETTE32_16_COLOR
													TinyImage::RGBA_32_8888,							//PALETTE32_256_COLOR
													TinyImage::RGBA_32_8888,							//PALETTE24_4_COLOR	
													TinyImage::RGBA_32_8888,							//PALETTE24_16_COLOR
													TinyImage::RGBA_32_8888								//PALETTE24_256_COLOR
		}
		};
#else // default no convertion
		TinyImageLoaderContext	gDefaultContext =
		{
												{
													TinyImage::NO_TEXTURE,								//NO_TEXTURE,
													TinyImage::A3I5_TRANSLUCENT,						//A3I5_TRANSLUCENT	
													TinyImage::PALETTE16_4_COLOR,						//PALETTE16_4_COLOR			(ABGR 1555)
													TinyImage::PALETTE16_16_COLOR,						//PALETTE16_16_COLOR		(ABGR 1555)
													TinyImage::PALETTE16_256_COLOR,						//PALETTE16_256_COLOR		(ABGR 1555)
													TinyImage::COMPRESSED_4X4_TEXEL,					//COMPRESSED_4X4_TEXEL
													TinyImage::A5I3_TRANSLUCENT,						//A5I3_TRANSLUCENT	
													TinyImage::ABGR_16_1555_DIRECT_COLOR,				//ABGR_16_1555_DIRECT_COLOR
													TinyImage::RGBA_32_8888,							//RGBA_32_8888
													TinyImage::RGB_24_888,								//RGB_24_888
													TinyImage::RGBA_16_4444,							//RGBA_16_4444
													TinyImage::BGRA_16_5551,							//BGRA_16_5551
													TinyImage::RGBA_16_5551,							//RGBA_16_5551
													TinyImage::RGB_16_565,								//RGB_16_565
													TinyImage::COMPRESSED_PVRTC,						//COMPRESSED_PVRTC	
													TinyImage::PALETTE32_4_COLOR,						//PALETTE32_4_COLOR
													TinyImage::PALETTE32_16_COLOR,						//PALETTE32_16_COLOR
													TinyImage::PALETTE32_256_COLOR,						//PALETTE32_256_COLOR
													TinyImage::PALETTE24_4_COLOR,						//PALETTE24_4_COLOR	
													TinyImage::PALETTE24_16_COLOR,						//PALETTE24_16_COLOR
													TinyImage::PALETTE24_256_COLOR						//PALETTE24_256_COLOR
												}
		};

#endif
	}
}