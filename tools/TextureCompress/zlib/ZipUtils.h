
#ifndef __ZIPUTILS_H__
#define __ZIPUTILS_H__


enum {
	CCZ_COMPRESSION_ZLIB,				// zlib format.
	CCZ_COMPRESSION_BZIP2,				// bzip2 format (not supported yet)
	CCZ_COMPRESSION_GZIP,				// gzip format (not supported yet)
	CCZ_COMPRESSION_NONE,				// plain (not supported yet)
};

class ZipUtils
{
public:
	
	static int InflateMemory(unsigned char *in, unsigned int inLength, unsigned char **out);

	static int InflateMemoryWithHint(unsigned char *in, unsigned int inLength, unsigned char **out, unsigned int outLenghtHint);

	static void DeflateMemory(unsigned char *in, unsigned int inLength, unsigned char **out, unsigned int & outLength);

	static int InflateMemoryWithHint(unsigned char *in, unsigned int inLength, unsigned char **out, unsigned int *outLength,unsigned int outLenghtHint);
};

#endif // __ZIPUTILS_H__
