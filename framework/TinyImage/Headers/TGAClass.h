#pragma once

#include "TinyImage.h"

namespace Kigs
{
	namespace Core
	{
		class CoreRawBuffer;

	}
	namespace Pict
	{

		// ****************************************
		// * TGAClass class
		// * --------------------------------------
		/**
		* \file	TGAClass.h
		* \class	TGAClass
		* \ingroup TinyImageModule
		* \brief TinyImage specialized for TGA management.
		*
		*/
		// ****************************************
		class TGAClass : public TinyImage
		{
		public:
			/// Destructor
			virtual ~TGAClass();

			/**
			* Constructor
			* \param fileName TGA file to load
			*/
			TGAClass(File::FileHandle* fileName);


			/**
			* Main loading method
			* \param fileName TGA file to load
			*/
			virtual bool Load(File::FileHandle* fileName);

			/**
			* \struct tgaheader
			* \brief TGA header
			*/
			struct tgaheader
			{
				u8  identsize;       // size of ID field that follows 18 byte header (0 usually)
				u8  colourmaptype;   // type of colour map 0=none, 1=has palette
				u8  imagetype;       // image format, see image formats enumeration below

				s16 colourmapstart;  // first colour map mEntry in palette
				s16 colourmaplength; // number of colours in palette
				u8  colourmapbits;   // number of bits per palette mEntry (16, 24, 32)

				s16 xstart;          // image x origin
				s16 ystart;          // image y origin
				s16 width;           // image width in pixels
				s16 height;          // image height in pixels
				u8  bits;            // image bits per pixel (8,16,24,32)
				u8  descriptor;      // image descriptor bits (vh flip bits)
			};

			/**
			* \struct tgafooter
			* \brief TGA footer (TGA 2.0 only)
			*/
			struct tgafooter
			{
				u32 extension;       // offset to the extension area
				u32 developperarea;  // offet to the developper area
				u8  signature[16];   // must contain "TRUEVISION-XFILE"
				u8  reservedchar;    // must contain '.'
				u8  endchar;         // must contain NULL
			};

			/// size of the TGA header (in bytes)
			static const u8 mHeaderSize = 18;
			/// size of the TGA footer (in bytes)
			static const u8 mFooterSize = 26;

			/**
			* \enum TGAFormats
			* \brief list of tga image formats
			*/
			enum TGAFormats
			{
				// standard formats
				NO_IMAGE = 0,                     /// no image data
				NO_COMPRESSION_COLORMAPPED = 1,  /// uncompressed, color-mapped image
				NO_COMPRESSION_TRUECOLOR = 2,  /// uncompressed, true-color image
				NO_COMPRESSION_BLACKWHITE = 3,  /// uncompressed, black&white image
				RLE_COMPRESSION_COLORMAPPED = 9,  /// run-length encoded, color-mapped image
				RLE_COMPRESSION_TRUECOLOR = 10, /// run-length encoded, true-color image
				RLE_COMPRESSION_BLACKWHITE = 11, /// run-length encoded, black&white image

				// custom nds formats
				NDS_A3I5_TRANSLUCENT = 128, /// 3 alpha bits 5 index bits, translucent texture
				NDS_A5I3_TRANSLUCENT = 129, /// 5 alpha bits 3 index bits, translucent texture
				NDS_4X4_COMPRESSED = 130  /// 4x4 texel compressed texture

			};

			static	inline ReadColorFunc	GetReadFunction(ImageFormat fmt, int bytes, bool flipEndian = false)
			{
				switch ((int)fmt)
				{
				case ((int)NO_TEXTURE):
				{
					return 0;
				}

				case ((int)A3I5_TRANSLUCENT):
				{
					return 0;
				}
				case ((int)PALETTE16_4_COLOR):
				case ((int)PALETTE16_16_COLOR):
				case ((int)PALETTE16_256_COLOR):
				case ((int)ABGR_16_1555_DIRECT_COLOR):
				{

					if (bytes == 4)
					{
						return readColorData16_ABGR_1555<4>;
					}
					else if (bytes == 3)
					{
						return readColorData16_ABGR_1555<3>;
					}
					else if (bytes == 2)
					{
						return readColorData16_ABGR_1555<2>;
					}

					return 0;
				}

				case ((int)COMPRESSED_4X4_TEXEL):
				{
					return 0;
				}
				case ((int)A5I3_TRANSLUCENT):
				{
					return 0;
				}
				case ((int)RGBA_32_8888):
				{
					if (!flipEndian)
					{
						if (bytes == 4)
						{
							return readColorData32_RGBA<4>;
						}
						else if (bytes == 3)
						{
							return readColorData32_RGBA<3>;
						}
						else if (bytes == 2)
						{
							return readColorData32_RGBA<2>;
						}
					}
					else
					{
						if (bytes == 4)
						{
							return readColorData32_BGRA<4>;
						}
						else if (bytes == 3)
						{
							return readColorData32_BGRA<3>;
						}
						else if (bytes == 2)
						{
							return readColorData32_BGRA<2>;
						}
					}
					return 0;
				}
				case ((int)RGB_24_888):
				{
					if (!flipEndian)
					{
						if (bytes == 4)
						{
							return readColorData24_RGB<4>;
						}
						else if (bytes == 3)
						{
							return readColorData24_RGB<3>;
						}
						else if (bytes == 2)
						{
							return readColorData24_RGB<2>;
						}
					}
					else
					{
						if (bytes == 4)
						{
							return readColorData24_BGR<4>;
						}
						else if (bytes == 3)
						{
							return readColorData24_BGR<3>;
						}
						else if (bytes == 2)
						{
							return readColorData24_BGR<2>;
						}
					}
					return 0;
				}
				case ((int)RGBA_16_4444):
				{
					if (bytes == 4)
					{
						return readColorData16_RGBA_4444<4>;
					}
					else if (bytes == 3)
					{
						return readColorData16_RGBA_4444<3>;
					}
					else if (bytes == 2)
					{
						return readColorData16_RGBA_4444<2>;
					}
					return 0;
				}
				case ((int)BGRA_16_5551):
				{
					// TODO ?
					if (bytes == 4)
					{
						return readColorData16_BGRA_5551<4>;
					}
					else if (bytes == 3)
					{
						return readColorData16_BGRA_5551<3>;
					}
					else if (bytes == 2)
					{
						return readColorData16_BGRA_5551<2>;
					}
					return 0;
				}
				case ((int)RGBA_16_5551):
				{
					// TODO ?
					if (bytes == 4)
					{
						return readColorData16_RGBA_5551<4>;
					}
					else if (bytes == 3)
					{
						return readColorData16_RGBA_5551<3>;
					}
					else if (bytes == 2)
					{
						return readColorData16_RGBA_5551<2>;
					}
					return 0;
				}
				case ((int)RGB_16_565):
				{
					if (bytes == 4)
					{
						return readColorData16_RGB_565<4>;
					}
					else if (bytes == 3)
					{
						return readColorData16_RGB_565<3>;
					}
					else if (bytes == 2)
					{
						return readColorData16_RGB_565<2>;
					}
					return 0;
				}
				case ((int)COMPRESSED_PVRTC): // TODO ?
				{
					return 0;
				}
				case ((int)PALETTE32_4_COLOR):
				case ((int)PALETTE32_16_COLOR):
				case ((int)PALETTE32_256_COLOR):
				{
					if (bytes == 4)
					{
						return readColorData32_RGBA<4>;
					}
					else if (bytes == 3)
					{
						return readColorData32_RGBA<3>;
					}
					else if (bytes == 2)
					{
						return readColorData32_RGBA<2>;
					}
					return 0;
				}


				case ((int)PALETTE24_4_COLOR):
				case ((int)PALETTE24_16_COLOR):
				case ((int)PALETTE24_256_COLOR):
				{
					if (bytes == 4)
					{
						return readColorData24_RGB<4>;
					}
					else if (bytes == 3)
					{
						return readColorData24_RGB<3>;
					}
					else if (bytes == 2)
					{
						return readColorData24_RGB<2>;
					}
					return 0;
				}
				}
				return 0;
			}


			/**
			* read color from specified source, convert it and copy it to destination
			* \param src pointer on the color data to read
			* \param dst pointer to the color data to write
			* \param bytes number of bytes to read
			*/
			template<unsigned int bytes>
			static void readColorData16_RGBA_4444(unsigned char* src, unsigned char* dst);
			template<unsigned int bytes>
			static void readColorData16_RGBA_5551(unsigned char* src, unsigned char* dst);
			template<unsigned int bytes>
			static void readColorData16_BGRA_5551(unsigned char* src, unsigned char* dst);
			template<unsigned int bytes>
			static void readColorData16_ABGR_1555(unsigned char* src, unsigned char* dst);
			template<unsigned int bytes>
			static void readColorData16_RGB_565(unsigned char* src, unsigned char* dst);
			template<unsigned int bytes>
			static void readColorData24_RGB(unsigned char* src, unsigned char* dst);
			template<unsigned int bytes>
			static void readColorData32_RGBA(unsigned char* src, unsigned char* dst);
			template<unsigned int bytes>
			static void readColorData24_BGR(unsigned char* src, unsigned char* dst);
			template<unsigned int bytes>
			static void readColorData32_BGRA(unsigned char* src, unsigned char* dst);
			TGAClass(void* data, int sx, int sy, TinyImage::ImageFormat internalfmt);

		protected:


			// create images from data, to export them
			virtual void	Export(const char* filename);



			/**
			* load the tga header from the file
			* \param header the header to fill
			* \param memfile the loaded file
			*/
			static void loadHeader(tgaheader& header, u8* memfile);
		};

		template<unsigned int bytes>
		void TGAClass::readColorData16_RGBA_4444(unsigned char* src, unsigned char* dst)
		{
			switch (bytes)
			{
			case 4:
			{
				// BBBB AAAA
				dst[0] = (src[0] & 0xF0) | ((src[3] >> 4) & 0x0F);
				// RRRR GGGG
				dst[1] = ((src[2] & 0xF0) | ((src[1] >> 4) & 0x0F));
			} break;

			case 3:
			{
				// BBBB AAAA
				dst[0] = (src[0] & 0xF0) | 0x0F;
				// RRRR GGGG
				dst[1] = ((src[2] & 0xF0) | ((src[1] >> 4) & 0x0F));
			} break;


			case 2:
			{
				// TODO
			} break;

			// what else?
			default: break;
			}
		}
		template<unsigned int bytes>
		void TGAClass::readColorData16_RGBA_5551(unsigned char* src, unsigned char* dst)
		{
			switch (bytes)
			{
			case 4:
			{
				// gg bbbbb a
				dst[0] = ((src[1] << 3) & 0xC0) | ((src[0] >> 2) & 0x3E) | (src[3] >> 7);
				// rrrrr ggg
				dst[1] = ((src[2]) & 0xF8) | ((src[1] >> 5) & 0x07);
			} break;

			case 3:
			{
				// gg bbbbb a
				dst[0] = ((src[1] << 3) & 0xC0) | ((src[0] >> 2) & 0x3E) | 0x01;
				// rrrrr ggg
				dst[1] = ((src[2]) & 0xF8) | ((src[1] >> 5) & 0x07);
			} break;

			case 2:
			{
				// TODO
			} break;

			// what else?
			default: break;
			}
		}

		template<unsigned int bytes>
		void TGAClass::readColorData16_BGRA_5551(unsigned char* src, unsigned char* dst)
		{
			switch (bytes)
			{
				// 32 bits color : BBBBBBBB GGGGGGGG RRRRRRRR AAAAAAAA -> GGRRRRRA BBBBBGGG
			case 4:
			{
				dst[0] = (u8)(((src[1] << 3) & 0xC0) | ((src[2] >> 2) & 0x3E)) | (src[3] >> 7);
				dst[1] = (u8)((src[0] & 0xF8) | ((src[1] >> 5) & 0x07));
			} break;

			// 24 bits color : BBBBBBBB GGGGGGGG RRRRRRRR -> GGRRRRR1  BBBBBGGG
			case 3:
			{
				dst[0] = (u8)(((src[1] << 3) & 0xC0) | ((src[2] >> 2) & 0x3E)) | 0x01;
				dst[1] = (u8)((src[0] & 0xF8) | ((src[1] >> 5) & 0x07));
			} break;

			// 16 bits color : GGGBBBBB ARRRRRGG -> GGRRRRR1 BBBBBGGG
			case 2:
			{
				// TODO
				/*dst[0]= (u8) ((src[0]&0xE0) | ((src[1]>>2)&0x1F));
				dst[1]= (u8) ((src[1]&0x83) | ((src[0]&0x1F)<<2));*/
			} break;

			// what else?
			default: break;
			}
		}

		template<unsigned int bytes>
		void TGAClass::readColorData16_ABGR_1555(unsigned char* src, unsigned char* dst)
		{
			switch (bytes)
			{
				// 32 bits color : BBBBBBBB GGGGGGGG RRRRRRRR AAAAAAAA -> GGGRRRRR ABBBBBGG
			case 4:
			{
				dst[0] = (u8)(((src[1] << 2) & 0xE0) | ((src[2] >> 3) & 0x1F));
				dst[1] = (u8)((src[3] & 0x80) | ((src[0] >> 1) & 0x7C) | ((src[1] >> 6) & 0x03));
			} break;

			// 24 bits color : BBBBBBBB GGGGGGGG RRRRRRRR -> GGGRRRRR 1BBBBBGG
			case 3:
			{
				dst[0] = (u8)(((src[1] << 2) & 0xE0) | ((src[2] >> 3) & 0x1F));
				dst[1] = (u8)(0x80 | ((src[0] >> 1) & 0x7C) | ((src[1] >> 6) & 0x03));
			} break;

			// 16 bits color : GGGBBBBB ARRRRRGG -> GGGRRRRR ABBBBBGG
			case 2:
			{
				dst[0] = (u8)((src[0] & 0xE0) | ((src[1] >> 2) & 0x1F));
				dst[1] = (u8)((src[1] & 0x83) | ((src[0] & 0x1F) << 2));
			} break;

			// what else?
			default: break;
			}
		}




		template<unsigned int bytes>
		void TGAClass::readColorData16_RGB_565(unsigned char* src, unsigned char* dst)
		{
			switch (bytes)
			{
			case 4:
			{
				// ggg bbbbb
				dst[0] = ((src[1] << 3) & 0xE0) | ((src[0] >> 3) & 0x1F);
				// rrrrr ggg 
				dst[1] = (src[2] & 0xF8) | ((src[1] >> 5) & 0x07);

			} break;

			case 3:
			{
				// ggg bbbbb
				dst[0] = ((src[1] << 3) & 0xE0) | ((src[0] >> 3) & 0x1F);
				// rrrrr ggg 
				dst[1] = (src[2] & 0xF8) | ((src[1] >> 5) & 0x07);
			} break;


			case 2:
			{
				// TODO
			} break;

			// what else?
			default: break;
			}
		}



		template<unsigned int bytes>
		void TGAClass::readColorData32_RGBA(unsigned char* src, unsigned char* dst)
		{

			switch (bytes)
			{
				// 32 bits color 
			case 4:
			{
				dst[0] = src[0];
				dst[1] = src[1];
				dst[2] = src[2];
				dst[3] = src[3];
			} break;

			// 24 bits color
			case 3:
			{
				dst[0] = src[0];
				dst[1] = src[1];
				dst[2] = src[2];
				dst[3] = 0xFF;

			} break;

			// 16 bits color 
			case 2:
			{
				// TODO : not TESTED
				dst[0] = (src[1] & 0x7c) << 1;
				dst[1] = ((src[1] & 0x03) << 6) | ((src[0] & 0xe0) >> 2);
				dst[2] = (src[0] & 0x1f) << 3;
				dst[3] = (src[1] & 0x80);
			} break;

			// what else?
			default: break;
			}
		}


		template<unsigned int bytes>
		void TGAClass::readColorData32_BGRA(unsigned char* src, unsigned char* dst)
		{
			switch (bytes)
			{
				// 32 bits color 
			case 4:
			{
				dst[0] = src[2];
				dst[1] = src[1];
				dst[2] = src[0];
				dst[3] = src[3];
			} break;

			// 24 bits color
			case 3:
			{
				dst[0] = src[2];
				dst[1] = src[1];
				dst[2] = src[0];
				dst[3] = 0xFF;

			} break;

			// 16 bits color 
			case 2:
			{
				// TODO : not TESTED
				dst[0] = (src[1] & 0x7c) << 1;
				dst[1] = ((src[1] & 0x03) << 6) | ((src[0] & 0xe0) >> 2);
				dst[2] = (src[0] & 0x1f) << 3;
				dst[3] = (src[1] & 0x80);
			} break;

			// what else?
			default: break;
			}
		}

		template<unsigned int bytes>
		void TGAClass::readColorData24_RGB(unsigned char* src, unsigned char* dst)
		{
			switch (bytes)
			{
				// 32 bits color 
			case 4:
			{
				// straight
				dst[0] = src[0];
				dst[1] = src[1];
				dst[2] = src[2];
			} break;

			// 24 bits color
			case 3:
			{
				dst[0] = src[0];
				dst[1] = src[1];
				dst[2] = src[2];

			} break;

			// 16 bits color 
			case 2:
			{
				// TODO : not TESTED
				dst[0] = (src[1] & 0x7c) << 1;
				dst[1] = ((src[1] & 0x03) << 6) | ((src[0] & 0xe0) >> 2);
				dst[2] = (src[0] & 0x1f) << 3;
			} break;

			// what else?
			default: break;
			}
		}


		template<unsigned int bytes>
		void TGAClass::readColorData24_BGR(unsigned char* src, unsigned char* dst)
		{
			switch (bytes)
			{
				// 32 bits color 
			case 4:
			{
				// straight
				dst[0] = src[2];
				dst[1] = src[1];
				dst[2] = src[0];
			} break;

			// 24 bits color
			case 3:
			{
				dst[0] = src[2];
				dst[1] = src[1];
				dst[2] = src[0];

			} break;

			// 16 bits color 
			case 2:
			{
				// TODO : not TESTED
				dst[0] = (src[1] & 0x7c) << 1;
				dst[1] = ((src[1] & 0x03) << 6) | ((src[0] & 0xe0) >> 2);
				dst[2] = (src[0] & 0x1f) << 3;
			} break;

			// what else?
			default: break;
			}
		}

	}
}
