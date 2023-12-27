#pragma once

#include "TecLibs/Tec3D.h"
#include "TecLibs/2D/BBox2DI.h"
#include "Drawable.h"
#include "CoreModifiableAttribute.h"

namespace Kigs
{
	namespace Draw
	{

		// ****************************************
		// * KigsBitmap class
		// * --------------------------------------
		/**
		 * \file	KigsBitmap.h
		 * \class	KigsBitmap
		 * \ingroup Renderer
		 * \brief	Pixel buffer used as a Texture.
		 *
		 */
		 // ****************************************
		class KigsBitmap : public Drawable
		{
		public:

			class KigsBitmapPixel
			{
			public:
				KigsBitmapPixel(unsigned char r, unsigned char g, unsigned char b, unsigned char a) :R(r), G(g), B(b), A(a) {};
				unsigned char R, G, B, A;
				void set(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
				{
					R = r;
					G = g;
					B = b;
					A = a;
				}
			};


			DECLARE_CLASS_INFO(KigsBitmap, Drawable, Renderer)

				/**
				* \brief	constructor
				* \fn 		KigsBitmap(const std::string& name,DECLARE_CLASS_NAME_TREE_ARG);
				* \param	name : instance name
				* \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
				*/
				KigsBitmap(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);

			virtual ~KigsBitmap();

			unsigned int	GetSelfDrawingNeeds() override
			{
				return ((unsigned int)Need_Predraw);
			}

			// no check for out of bound
			inline KigsBitmapPixel	GetPixel(int x, int y) const
			{
				KigsBitmapPixel* readPixel = (KigsBitmapPixel*)mRawPixels;
				readPixel += x;
				readPixel += y * mSize[0];
				return *readPixel;
			}

			void	Box(int x, int y, int sizex, int sizey, const KigsBitmapPixel& color);

			inline void Clear(const KigsBitmapPixel& clearColor)
			{
				Box(0, 0, mSize[0], mSize[1], clearColor);
			}

			inline void PutPixel(int x, int y, const KigsBitmapPixel& write)
			{
				mDirtyZone.Update(x, y);

				KigsBitmapPixel* readPixel = (KigsBitmapPixel*)mRawPixels;
				readPixel += x;
				readPixel += y * mSize[0];
				*readPixel = write;
			}

			void	Line(int sx, int sy, int ex, int ey, const KigsBitmapPixel& color);
			void	Line(v2i p1, v2i p2, const KigsBitmapPixel& color);

			void	ScrollX(int offset, const KigsBitmapPixel& color);
			void	ScrollY(int offset, const KigsBitmapPixel& color);


			void	Print(const std::string& txt, int posx, int posy, unsigned int _maxLineNumber, unsigned int maxSize, unsigned int fontSize, const std::string& fontName, unsigned int a_Alignment, const KigsBitmapPixel& color);

			inline unsigned char* GetPixelBuffer()
			{
				// reset dirty zone
				mDirtyZone.Init(0, 0);
				mDirtyZone.Update(mSize[0] - 1, mSize[1] - 1);
				return mRawPixels;
			}

		protected:

			inline unsigned char* GetPixelAddress(int x, int y)
			{
				KigsBitmapPixel* readPixel = (KigsBitmapPixel*)mRawPixels;
				readPixel += x;
				readPixel += y * mSize[0];
				return (unsigned char*)readPixel;
			}

			//! init the modifiable and set the _isInit flag to true if OK
			void	InitModifiable() override;

			/**
			* \brief	initialise pre draw method
			* \fn 		virtual bool PreDraw(TravState*);
			* \param	TravState : camera state
			* \return	TRUE if a could PreDraw
			*/
			bool PreDraw(TravState*)  override;

			v2i					mSize = {0,0};
			WRAP_ATTRIBUTES(mSize);

			// manage a RGBA8888 array of pixel and update it's father texture if needed 
			unsigned char* mRawPixels;

			BBox2DI	mDirtyZone;

		};
	}
}