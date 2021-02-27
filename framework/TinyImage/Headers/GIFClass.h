#ifndef _GIFCLASS_H_
#define _GIFCLASS_H_

#include "Core.h"

#include "TinyImage.h"
class CoreRawBuffer;


// ****************************************
// * GIFClass class
// * --------------------------------------
/**
* \file	GIFClass.h
* \class	GIFClass
* \ingroup TinyImageModule
* \brief TinyImage specialized for PNG management.
*
*/
// ****************************************
class GIFClass : public TinyImage
{
friend class TinyImage;
public:
	/// Destructor
	virtual ~GIFClass();
	
	/**
	* Constructor
	* \param fileName TGA file to load
	*/
	GIFClass(FileHandle* fileName);
	GIFClass(void* data, int sx, int sy, TinyImage::ImageFormat internalfmt);
	GIFClass(CoreRawBuffer* rawbuffer);

	/**
	* Main loading method
	* \param fileName TGA file to load
	*/
	virtual bool Load(FileHandle* fileName);

	bool Load(CoreRawBuffer* buffer);

	virtual void	Export(const char* filename);


protected:

};

#endif //_GIFCLASS_H_
