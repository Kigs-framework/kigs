
#include <zlib.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>

#include "ZipUtils.h"


#define BUFFER_INC_FACTOR (2)

int ZipUtils::InflateMemoryWithHint(unsigned char *in, unsigned int inLength, unsigned char **out, unsigned int *outLength, unsigned int outLenghtHint)
{
	/* ret value */
	int err = Z_OK;

	int bufferSize = outLenghtHint;
	*out = new unsigned char[bufferSize];

	z_stream d_stream; /* decompression stream */
	d_stream.zalloc = (alloc_func)0;
	d_stream.zfree = (free_func)0;
	d_stream.opaque = (voidpf)0;

	d_stream.next_in  = in;
	d_stream.avail_in = inLength;
	d_stream.next_out = *out;
	d_stream.avail_out = bufferSize;

	/* window size to hold 256k */
	if( (err = inflateInit2(&d_stream, 15 + 32)) != Z_OK )
		return err;

	for (;;)
	{
		err = inflate(&d_stream, Z_NO_FLUSH);

		if (err == Z_STREAM_END)
		{
			break;
		}

		switch (err)
		{
		case Z_NEED_DICT:
			err = Z_DATA_ERROR;
		case Z_DATA_ERROR:
		case Z_MEM_ERROR:
			inflateEnd(&d_stream);
			return err;
		}

		// not enough memory ?
		if (err != Z_STREAM_END)
		{
            delete [] *out;
            *out = new unsigned char[bufferSize * BUFFER_INC_FACTOR];

			/* not enough memory, ouch */
			if (! *out )
			{
				inflateEnd(&d_stream);
				return Z_MEM_ERROR;
			}

			d_stream.next_out = *out + bufferSize;
			d_stream.avail_out = bufferSize;
			bufferSize *= BUFFER_INC_FACTOR;
		}
	}

	*outLength = bufferSize - d_stream.avail_out;
	err = inflateEnd(&d_stream);
	return err;
}

int ZipUtils::InflateMemoryWithHint(unsigned char *in, unsigned int inLength, unsigned char **out, unsigned int outLengthHint)
{
	unsigned int outLength = 0;
	int err = InflateMemoryWithHint(in, inLength, out, &outLength, outLengthHint);

	if (err != Z_OK || *out == NULL) {
		if (err == Z_MEM_ERROR)
		{
			printf("cocos2d: ZipUtils: Out of memory while decompressing map data!");
		} else
		if (err == Z_VERSION_ERROR)
		{
			printf("cocos2d: ZipUtils: Incompatible zlib version!");
		} else
		if (err == Z_DATA_ERROR)
		{
			printf("cocos2d: ZipUtils: Incorrect zlib compressed data!");
		}
		else
		{
			printf("cocos2d: ZipUtils: Unknown error while decompressing map data!");
		}

		delete[] *out;
		*out = NULL;
		outLength = 0;
	}

	return outLength;
}

int ZipUtils::InflateMemory(unsigned char *in, unsigned int inLength, unsigned char **out)
{
	// 256k for hint
	return InflateMemoryWithHint(in, inLength, out, 256 * 1024);
}

void ZipUtils::DeflateMemory(unsigned char *in, unsigned int inLength, unsigned char **out, unsigned int & outLength)
{
	std::vector<Bytef> buffer;

	const size_t BUFSIZE = inLength;

	*out = new unsigned char[BUFSIZE];

	z_stream strm;
	strm.zalloc = 0;
	strm.zfree = 0;
	strm.next_in = reinterpret_cast<Bytef *>(in);
	strm.avail_in = inLength;
	strm.next_out = *out;
	strm.avail_out = BUFSIZE;

	deflateInit(&strm, Z_BEST_COMPRESSION);

	while (strm.avail_in != 0)
	{
		int res = deflate(&strm, Z_NO_FLUSH);
		assert(res == Z_OK);
		if (strm.avail_out == 0)
		{
			buffer.insert(buffer.end(), *out, *out + BUFSIZE);
			strm.next_out = *out;
			strm.avail_out = BUFSIZE;
		}
	}

	int deflate_res = Z_OK;
	while (deflate_res == Z_OK)
	{
		if (strm.avail_out == 0)
		{
			buffer.insert(buffer.end(), *out, *out + BUFSIZE);
			strm.next_out = *out;
			strm.avail_out = BUFSIZE;
		}
		deflate_res = deflate(&strm, Z_FINISH);
	}

	assert(deflate_res == Z_STREAM_END);
	buffer.insert(buffer.end(), *out, *out + BUFSIZE - strm.avail_out);
	deflateEnd(&strm);
	outLength = strm.total_out;
}
