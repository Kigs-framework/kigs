#ifndef _ETCCLASS_H_
#define _ETCCLASS_H_

// base class
#include "TinyImage.h"

// ****************************************
// * ETCClass class
// * --------------------------------------
/**
* \file	ETCClass.h
* \class	ETCClass
* \ingroup TinyImageModule
* \brief TinyImage specialized for ETC management.
*
*/
// ****************************************
class ETCClass : public TinyImage
{
public:
	/**
	* constructor
	* \param filename  the name of the file to load
	*/
	ETCClass(FileHandle* fileName);

	/// destructor
	virtual ~ETCClass();

protected:
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

