#include "PrecompiledHeaders.h"
#include "BasePixelBufferStruct.h"

//#define STB_IMAGE_RESIZE_IMPLEMENTATION

#ifdef STB_IMAGE_RESIZE_IMPLEMENTATION
#include <stb_image_resize.h>
#endif
void PixelBuffer::rescale(PixelBuffer& target)
{
#ifdef STB_IMAGE_RESIZE_IMPLEMENTATION
	auto inputSize = getSizeInPixel();
	auto outputSize = target.getSizeInPixel();
	
	stbir_resize_uint8(buffer(), inputSize.x, inputSize.y, getLineSizeInByte(), target.buffer(), outputSize.x, outputSize.y, target.getLineSizeInByte(), getComponentCount());
#endif
}
