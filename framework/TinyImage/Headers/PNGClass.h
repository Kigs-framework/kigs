#ifndef _PNGCLASS_H_
#define _PNGCLASS_H_

#include "Core.h"

#include "TinyImage.h"

/**
* \class PNGClass
* \brief holds PNG loading methods 
*/

class CoreRawBuffer;

class PNGClass : public TinyImage
{
friend class TinyImage;
public:
	/// Destructor
	virtual ~PNGClass();
	
	/**
	* Constructor
	* \param fileName TGA file to load
	*/
	PNGClass(FileHandle* fileName);
	PNGClass(void* data, int sx, int sy, TinyImage::ImageFormat internalfmt);
	PNGClass(CoreRawBuffer* rawbuffer);

	/**
	* Main loading method
	* \param fileName TGA file to load
	*/
	virtual bool Load(FileHandle* fileName);

	bool Load(CoreRawBuffer* buffer);

	virtual void	Export(const char* filename);


protected:

};

#endif //_PNGCLASS_H_
