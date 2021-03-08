#ifndef _MPEG4BufferStreamFFMPEG_H_
#define _MPEG4BufferStreamFFMPEG_H_

#include "MPEG4BufferStream.h"

// ****************************************
// * MPEG4BufferStream class
// * --------------------------------------
/*!  \class MPEG4BufferStream
manage a mp4 video stream
*/
// ****************************************

#include "FilePathManager.h"
#include <memory>


struct ReadInfo;
struct AVPacket;

class MPEG4BufferStreamFFMPEG : public MPEG4BufferStream
{
public:

	DECLARE_CLASS_INFO(MPEG4BufferStreamFFMPEG,MPEG4BufferStream, CameraModule)

	//! constructor
	MPEG4BufferStreamFFMPEG(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	virtual void InitModifiable() override;
	virtual void SetTime(kdouble t) override;
	virtual void NextFrame() override;

	// Callbacks
	int ReadFunc(unsigned char* buf, int buf_size);
	int64_t SeekFunc(int64_t pos, int whence);

protected:

	std::unique_ptr<ReadInfo> RI;
	void Process() override;

	~MPEG4BufferStreamFFMPEG() override;

	bool ReadNextFrame(AVPacket* avpkt);
	void SyncStream();
	
	void SeekPts(int64_t seek_pts);
	
	

	int64_t _stream_pts;
	int64_t _timer_pts;
	int64_t _frame_pts;
	int64_t _stream_duration;

	kstl::vector<unsigned char> _internal_buffer;

	SmartPointer<FileHandle> _file_handle;


	struct StreamBuffer
	{
		CoreRawBuffer* buffer;
		int64_t pos;
		unsigned long size;
	} _stream_buffer;
};

#endif //_MPEG4BufferStreamFFMPEG_H_