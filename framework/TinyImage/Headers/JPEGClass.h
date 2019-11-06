#ifndef _JPEGCLASS_H_
#define _JPEGCLASS_H_

#include "Core.h"

#include "TinyImage.h"

class CoreRawBuffer;
/**
* \class JPEG
* \brief holds TGA loading methods for standard and custom formats
*/
class JPEGClass : public TinyImage
{
friend class TinyImage;
public:
	/// Destructor
	virtual ~JPEGClass();
	
	/**
	* Constructor
	* \param fileName TGA file to load
	*/
	JPEGClass(FileHandle* fileName);
	JPEGClass(CoreRawBuffer* rawbuffer);

	/**
	* Main loading method
	* \param fileName TGA file to load
	*/
	virtual bool Load(FileHandle* fileName);
	bool Load(CoreRawBuffer* fileName);


	virtual void	Export(const char* filename)
	{
		// TODO
	};


protected:

};


#endif //_JPEGCLASS_H_
