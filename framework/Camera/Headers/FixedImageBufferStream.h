#ifndef _FIXEDIMAGEBUFFERSTREAM_H_
#define _FIXEDIMAGEBUFFERSTREAM_H_

#include "FrameBufferStream.h"


// ****************************************
// * FixedImageBufferStream class
// * --------------------------------------
/**
* \file	FixedImageBufferStream.h
* \class	FixedImageBufferStream
* \ingroup Camera
* \brief Utility class to use a fixed image as a buffer stream 
*/
// ****************************************

class FixedImageBufferStream : public FrameBufferStream
{
public:

	DECLARE_CLASS_INFO(FixedImageBufferStream, FrameBufferStream, CameraModule)

	//! constructor
	FixedImageBufferStream(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);


	void	Process() override;

	void	InitModifiable() override;


protected:

	virtual ~FixedImageBufferStream();

	void AllocateFrameBuffers() override;
	void FreeFrameBuffers() override;

	maString	mFilename;
	maBool		mNoConvert;
	unsigned char*	mImageData;

};

#endif //_MPEG4BufferStream_H_
