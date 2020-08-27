#ifndef _AFFINETRANSFORM2D_H_
#define _AFFINETRANSFORM2D_H_

#include "TecLibs/Tec3D.h"

// utility class to maintain a 2D transform
class AffineTransform2D
{
public:
	inline AffineTransform2D()
	{
	}
	inline ~AffineTransform2D()
	{}

protected:

	Point2D	mTranslate;
	Point2D	mScale;
	Float	mRotation;
};


#endif //_AFFINETRANSFORM2D_H_
