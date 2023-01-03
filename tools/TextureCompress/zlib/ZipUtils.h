#pragma once

namespace ZipUtils
{

	enum {
		CCZ_COMPRESSION_ZLIB,				// zlib format.
		CCZ_COMPRESSION_BZIP2,				// bzip2 format (not supported yet)
		CCZ_COMPRESSION_GZIP,				// gzip format (not supported yet)
		CCZ_COMPRESSION_NONE,				// plain (not supported yet)
	};

	int InflateMemory(unsigned char* in, unsigned int inLength, unsigned char** out);

	int InflateMemoryWithHint(unsigned char* in, unsigned int inLength, unsigned char** out, unsigned int outLenghtHint);

	void DeflateMemory(unsigned char* in, unsigned int inLength, unsigned char** out, unsigned int& outLength);

	int InflateMemoryWithHint(unsigned char* in, unsigned int inLength, unsigned char** out, unsigned int* outLength, unsigned int outLenghtHint);

}

