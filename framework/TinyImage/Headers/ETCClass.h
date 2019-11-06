#ifndef _ETCCLASS_H_
#define _ETCCLASS_H_

// base class
#include "TinyImage.h"

/**
* \class ETCClass
* \brief specific DDS class
*/
class ETCClass : public TinyImage
{
public:

	friend class TinyImage;

	/// destructor
	virtual ~ETCClass();

protected:
	/**
	* constructor
	* \param filename  the name of the file to load
	*/
	ETCClass(FileHandle* fileName);


	virtual void	Export(const char* filename)
	{
		printf("can not export to ETC\n");
	}


	/**
	* load image from file
	* \param filename the name of the file to load
	*/
	virtual bool	Load(FileHandle* fileName);

	struct ETC_Header
	{
		unsigned int	sizex;	// if sizex&0xFFFF0000	=> content size
		unsigned int	sizey;  // if sizey&0xFFFF0000	=> content size
		unsigned int	format;
		unsigned int	datasize;
	};

};


#endif //_ETCCLASS_H_

