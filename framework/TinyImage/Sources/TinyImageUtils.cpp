#include "PrecompiledHeaders.h"
#include "TinyImageUtils.h"

unsigned short TinyImageUtils::RGBtoNDS(u8 r, u8 g, u8 b)
{
	// _BBBBBGG GGGRRRRR
	return (unsigned short) ( ((r>>3)&0x1F) | (((g>>3)&0x1F)<<5) | (((b>>3)&0x1F)<<10) );
}
