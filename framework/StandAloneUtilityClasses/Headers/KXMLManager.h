#pragma once

#include "TecLibs/TecLibs.h"
#include "CoreRawBuffer.h"
#include "CoreModifiable.h"

#include "ModuleFileManager.h"

#include <AttributePacking.h>

#include "zlib.h"
#include "zstd.h"

#include <vector>


/*! CHECK
 * Check that the condition holds. If it doesn't print a message and die.
 */
#define KXMLMANAGER_CHECK(cond, ...)            \
    do {                                        \
        if (!(cond)) {                          \
            kigsprintf(							\
                    "%s:%d CHECK(%s) failed: ", \
                    __FILE__,                   \
                    __LINE__,                   \
                    #cond);                     \
            kigsprintf("" __VA_ARGS__);			\
            kigsprintf("\n");					\
            exit(1);                            \
        }                                       \
    } while (0)

 /*! CHECK_ZSTD
  * Check the zstd error code and die if an error occurred after printing a
  * message.
  */
#define KXMLMANAGER_CHECK_ZSTD(fn, ...)                                      \
    do {                                                         \
        size_t const err = (fn);                                 \
		KXMLMANAGER_CHECK(!ZSTD_isError(err), "%s", ZSTD_getErrorName(err)); \
    } while (0)



// KXMLManager manage compressed files
// a 64bit unsigned int is set at the start of the file :
// 4 bits flag | 60 bits for uncompressed size
// if flag&1 == 0 => zlib
// if flag&1 == 1 => zstandard
 
class KXMLManager : public CoreModifiable
{

public:

	DECLARE_CLASS_INFO(KXMLManager, CoreModifiable, FileManager)

	//! constructor
	KXMLManager(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	void UncompressKXML(CoreRawBuffer* compressedBuffer, std::vector<u8>& result)
	{
		u64 uncompress_size = 0;
		memcpy(&uncompress_size, compressedBuffer->buffer(), sizeof(u64));
		
		u64 flag=(uncompress_size>>60)&0xFL;
		uncompress_size = uncompress_size&0x0FFFFFFFFFFFFFFFL;

		result.resize(size_t(uncompress_size + 1));
		result.back() = 0;

		uLongf size = uLongf(uncompress_size);
		
		if(flag&1)// use zstd
		{
			size_t decompressedSize=ZSTD_decompress(result.data(),size,(const u8*)compressedBuffer->buffer() + sizeof(u64), compressedBuffer->length() - sizeof(u64));
			if(ZSTD_isError(decompressedSize))
			{
				result.clear();
			}
		}
		else
		{
			if (uncompress(result.data(), &size,(const u8*)compressedBuffer->buffer() + sizeof(u64), compressedBuffer->length() - sizeof(u64)) != Z_OK)
			{
				result.clear();
			}
		}
	}

	void UncompressZeroEndedData(CoreRawBuffer* compressedBuffer, CoreRawBuffer* result)
	{
		u64 uncompress_size = 0;
		memcpy(&uncompress_size, compressedBuffer->buffer(), sizeof(u64));

		u64 flag = (uncompress_size >> 60) & 0xFL;
		uncompress_size = uncompress_size & 0x0FFFFFFFFFFFFFFFL;

		result->SetBuffer(nullptr, (unsigned int)(uncompress_size + 1));
		(*result)[uncompress_size] = 0;

		uLongf size = uLongf(uncompress_size);

		if (flag & 1)// use zstd
		{
			size_t decompressedSize = ZSTD_decompress(result->data(), size, (const u8*)compressedBuffer->buffer() + sizeof(u64), compressedBuffer->length() - sizeof(u64));
			if (ZSTD_isError(decompressedSize))
			{
				result->clear();
			}
		}
		else
		{
			if (uncompress((Bytef*)result->data(), &size, (const u8*)compressedBuffer->buffer() + sizeof(u64), compressedBuffer->length() - sizeof(u64)) != Z_OK)
			{
				result->clear();
			}
		}
	}


	void CompressKXML(std::vector<std::string>& to_compress, std::vector<u8>& result)
	{
		size_t total_size = 0;
		for (auto& part : to_compress)
			total_size += part.size();
		
		std::vector<Bytef> uncompressed_data(total_size);

		u8* ptr = uncompressed_data.data();
		for (auto& part : to_compress)
		{
			memcpy(ptr, part.data(), part.size());
			ptr += part.size();
		}
		
		uLong 	len = (uLong)total_size;
		size_t	good_size=len;
		if(myUseZSTD)
		{
			good_size = ZSTD_compressBound(total_size);
		}
		
		result.resize(good_size + sizeof(u64));
		
		u64 s = (u64)total_size;
		
		if(myUseZSTD)
		{
			s|=((u64)1)<<60;	// flag zstd usage
		}
		
		memcpy(result.data(), &s, sizeof(u64));
		
		
		if(!myUseZSTD)
		{
			if (Z_OK != compress(result.data() + sizeof(u64), &len, (Bytef*)uncompressed_data.data(), (uLong)uncompressed_data.size()))
			{
				result.clear();
				return;
			}
			
			result.resize(len + sizeof(u64));
		}
		else
		{
			size_t compressedSize = ZSTD_compress(result.data() + sizeof(u64), good_size, (const Bytef *)uncompressed_data.data(), uncompressed_data.size(), 15);

			if(ZSTD_isError(compressedSize))
			{
				result.clear();
				return;
			}
			
			result.resize(compressedSize + sizeof(u64));
		}
		
	}

	void CompressData(CoreRawBuffer* to_compress, CoreRawBuffer* result)
	{
		size_t total_size = to_compress->size();
		uLong 	len = (uLong)total_size;
		size_t	good_size = len;
		if (myUseZSTD)
		{
			good_size = ZSTD_compressBound(total_size);
		}

		result->SetBuffer(nullptr,(unsigned int)( good_size + sizeof(u64)));
		u64 s = (u64)total_size;

		if (myUseZSTD)
		{
			s |= ((u64)1) << 60;	// flag zstd usage
		}

		memcpy(result->data(), &s, sizeof(u64));


		if (!myUseZSTD)
		{
			if (Z_OK != compress((Bytef*)result->data() + sizeof(u64), &len, (Bytef*)to_compress->data(), (uLong)to_compress->size()))
			{
				result->clear();
				return;
			}

			result->resize(len + sizeof(u64));
		}
		else
		{
			auto& cctx = mZSTDCompressContexts[std::this_thread::get_id()];
			if (!cctx)
			{
				cctx = ZSTD_createCCtx();
			}

			size_t compressedSize = ZSTD_compressCCtx(cctx, result->data() + sizeof(u64), good_size, (const Bytef *)to_compress->data(), to_compress->size(), 15);

			if (ZSTD_isError(compressedSize))
			{
				result->clear();
				return;
			}

			result->resize(compressedSize + sizeof(u64));
		}

	}
	void UncompressData(CoreRawBuffer* compressedBuffer, CoreRawBuffer* result)
	{
		u64 uncompress_size = 0;
		memcpy(&uncompress_size, compressedBuffer->buffer(), sizeof(u64));

		u64 flag = (uncompress_size >> 60) & 0xFL;
		uncompress_size = uncompress_size & 0x0FFFFFFFFFFFFFFFL;
		result->SetBuffer(nullptr, (unsigned int)(uncompress_size));
		uLongf size = uLongf(uncompress_size);

		if (flag & 1)// use zstd
		{
			auto& dctx = mZSTDDecompressContexts[std::this_thread::get_id()];
			if (!dctx)
			{
				dctx = ZSTD_createDCtx();
			}

			size_t decompressedSize = ZSTD_decompressDCtx(dctx, result->data(), size, (const u8*)compressedBuffer->buffer() + sizeof(u64), compressedBuffer->length() - sizeof(u64));
			if (ZSTD_isError(decompressedSize))
			{
				result->clear();
			}
		}
		else
		{
			if (uncompress((Bytef*)result->data(), &size, (const u8*)compressedBuffer->buffer() + sizeof(u64), compressedBuffer->length() - sizeof(u64)) != Z_OK)
			{
				result->clear();
			}
		}
	}
	
	void UncompressFileToFile(SmartPointer<FileHandle> fileIn, SmartPointer<FileHandle> fileOut)
	{
		Platform_fopen(fileIn.get(), "rb");
		Platform_fopen(fileOut.get(), "wb");

		u64 uncompress_size = 0;
		Platform_fread(&uncompress_size, sizeof(u64), 1, fileIn.get());

		u64 flag = (uncompress_size >> 60) & 0xFL;
		uncompress_size = uncompress_size & 0x0FFFFFFFFFFFFFFFL;

		uLongf size = uLongf(uncompress_size);

		if (flag & 1)// use zstd
		{
			auto& dctx = mZSTDDecompressContexts[std::this_thread::get_id()];

			if (!dctx)
			{
				dctx = ZSTD_createDCtx();
			}

			size_t const buffInSize = ZSTD_DStreamInSize();
			void* const buffIn = malloc(buffInSize);
			size_t const buffOutSize = ZSTD_DStreamOutSize();  /* Guarantee to successfully flush at least one complete compressed block in all circumstances. */
			void* const buffOut = malloc(buffOutSize);
			
			size_t const toRead = buffInSize;
			size_t read;
			while ((read = Platform_fread(buffIn, 1, toRead, fileIn.get())))
			{
				ZSTD_inBuffer input = { buffIn, read, 0 };
				/* Given a valid frame, zstd won't consume the last byte of the frame
				 * until it has flushed all of the decompressed data of the frame.
				 * Therefore, instead of checking if the return code is 0, we can
				 * decompress just check if input.pos < input.size.
				 */
				while (input.pos < input.size)
				{
					ZSTD_outBuffer output = { buffOut, buffOutSize, 0 };
					/* The return code is zero if the frame is complete, but there may
					 * be multiple frames concatenated together. Zstd will automatically
					 * reset the context when a frame is complete. Still, calling
					 * ZSTD_DCtx_reset() can be useful to reset the context to a clean
					 * state, for instance if the last decompression call returned an
					 * error.
					 */
					size_t const ret = ZSTD_decompressStream(dctx, &output, &input);
					KXMLMANAGER_CHECK_ZSTD(ret);
					Platform_fwrite(buffOut, 1, output.pos, fileOut.get());
				}
			}

			Platform_fclose(fileIn.get());
			Platform_fclose(fileOut.get());
			free(buffIn);
			free(buffOut);
		}
		else
		{
			KIGS_ASSERT(false);
		}
	}
	

	WRAP_METHODS(UncompressKXML, CompressKXML, CompressData, UncompressData, UncompressZeroEndedData);

protected:

	maBool	myUseZSTD;
	//! destructor
	virtual ~KXMLManager();

	std::unordered_map<std::thread::id, ZSTD_DCtx*> mZSTDDecompressContexts;
	std::unordered_map<std::thread::id, ZSTD_CCtx*> mZSTDCompressContexts;
};
