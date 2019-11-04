#ifndef _TINYIMAGEUTILS_H_
#define _TINYIMAGEUTILS_H_

//#include "TecLibs/Tec3D.h"
#include"CoreTypes.h"

/**
* \class TinyImageUtils
* \brief contains image manipulation tools
*/
class TinyImageUtils 
{
	public:
		/**
		* \return the NDS color based on specified RGB components
		* \param r red component
		* \param g green component
		* \param b blue component
		*/
		static unsigned short RGBtoNDS(u8 r, u8 g, u8 b);
		
};


#endif //_TINYIMAGEUTILS_H_
