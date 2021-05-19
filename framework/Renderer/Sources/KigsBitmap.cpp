#include "PrecompiledHeaders.h"

#include "KigsBitmap.h"
#include "Texture.h"
#include "FreeType_TextDrawer.h"
#include "ModuleRenderer.h"
#include "ModuleFileManager.h"
#include "FilePathManager.h"

#define FREETYPE_SIZE_COEFF	0.65f

IMPLEMENT_CLASS_INFO(KigsBitmap)

KigsBitmap::KigsBitmap(const kstl::string& name, CLASS_NAME_TREE_ARG) : Drawable(name, PASS_CLASS_NAME_TREE_ARG)
, mSize(*this, true, LABEL_AND_ID(Size),0,0)
, mRawPixels(0)
{
	mDirtyZone.SetEmpty();
}

bool KigsBitmap::PreDraw(TravState* state)
{
	if (Drawable::PreDraw(state))
	{
		if (IsInit())
		{
			Point2DI	bbsize = mDirtyZone.Size();
			if ((bbsize.x > 0) && (bbsize.y>0))
			{
				if ((mDirtyZone.m_Max.x >= 0) && (mDirtyZone.m_Max.y >= 0) && (mDirtyZone.m_Min.x < mSize[0]) && (mDirtyZone.m_Min.y < mSize[1]))
				{
					if (mDirtyZone.m_Min.x < 0)
					{
						mDirtyZone.m_Min.x = 0;
					}
					if (mDirtyZone.m_Min.y < 0)
					{
						mDirtyZone.m_Min.y = 0;
					}
					if (mDirtyZone.m_Max.x >= mSize[0])
					{
						mDirtyZone.m_Max.x = mSize[0]-1;
					}
					if (mDirtyZone.m_Max.y >= mSize[1])
					{
						mDirtyZone.m_Max.y = mSize[1]-1;
					}
					const kstl::vector<CoreModifiable*>& parents = GetParents();
					kstl::vector<CoreModifiable*>::const_iterator	parentsBegin = parents.begin();
					kstl::vector<CoreModifiable*>::const_iterator	parentsEnd = parents.end();

					while (parentsBegin != parentsEnd)
					{
						if ((*parentsBegin)->isSubType(Texture::mClassID))
						{
							Texture*	parentToUpdate = (Texture*)(*parentsBegin);

							unsigned char*	startPixel = GetPixelAddress(mDirtyZone.m_Min.x, mDirtyZone.m_Min.y);

							parentToUpdate->UpdateBufferZone(startPixel, mDirtyZone, Point2DI(mSize[0], mSize[1]));


							break;
						}
						++parentsBegin;
					}
				}
			}
			mDirtyZone.SetEmpty();
		}
		return true;
	}
	return false;
}


void	KigsBitmap::InitModifiable()
{
	if (!IsInit())
	{
		Texture* parentTexture = nullptr;
		for (auto p : GetParents())
		{
			if (p->isSubType(Texture::mClassID))
			{
				parentTexture = static_cast<Texture*>(p);
				break;
			}
		}
		if (!parentTexture) // don't init if no parent texture
		{
			return;
		}

		// if no size, check if parent texture has a size
		if ((mSize[0] <= 0) || (mSize[1] <= 0))
		{
			if (parentTexture)
			{
				mSize[0] = parentTexture->getValue<int>("Width");
				mSize[1] = parentTexture->getValue<int>("Height");
			}
		}

		// check if a size was given
		if ((mSize[0] > 0) && (mSize[1] > 0))
		{
			Drawable::InitModifiable();

			mRawPixels = new unsigned char[mSize[0] * mSize[1] * 4];

			// clear all
			memset(mRawPixels, 0, mSize[0] * mSize[1] * 4);

			mDirtyZone.m_Min.Set(0, 0);
			mDirtyZone.m_Max.Set(mSize[0]-1, mSize[1]-1);

			if (parentTexture)
			{
				if ((parentTexture->getValue<int>("Width") == 0) && (parentTexture->getValue<int>("Height") == 0))
				{
					SmartPointer<TinyImage>	img = OwningRawPtrToSmartPtr(TinyImage::CreateImage(mRawPixels, mSize[0], mSize[1], TinyImage::ImageFormat::RGBA_32_8888));
					parentTexture->CreateFromImage(img);
				}
			}

			ParentClassType::InitModifiable();
		}
	}
}

KigsBitmap::~KigsBitmap()
{
	if (mRawPixels)
	{
		delete[] mRawPixels;
	}
}

void	KigsBitmap::Box(int x, int y, int sizex, int sizey, const KigsBitmapPixel& color)
{
	// check bounds
	
	if( ((x + sizex) < 0) || (x>=mSize[0]))
	{
		return;
	}
	if( ((y + sizey) < 0) || (y >= mSize[1]))
	{
		return;
	}

	if (x < 0)
	{
		sizex += x;
		x = 0;
	}
	if (y < 0)
	{
		sizey += y;
		y = 0;
	}

	if ( (x + sizex) >= mSize[0])
	{
		sizex = mSize[0] - x;
	}
	if ((y + sizey) >= mSize[1])
	{
		sizey = mSize[1] - y;
	}

	mDirtyZone.Update(x, y);
	mDirtyZone.Update((x + sizex)-1, (y + sizey)-1);

	// draw first box line
	KigsBitmapPixel* firstPixel = (KigsBitmapPixel*)mRawPixels;
	firstPixel += x;
	firstPixel += y*mSize[0];
	int i;
	for (i = 0; i < sizex; i++)
	{
		firstPixel[i] = color;
	}
	// then copy line
	for (i = 1; i < sizey; i++)
	{
		memcpy(firstPixel+i*mSize[0], firstPixel, sizex*sizeof(KigsBitmapPixel));
	}

}

void KigsBitmap::Line(int sx, int sy, int ex, int ey, const KigsBitmapPixel& color)
{
	Line(Point2DI(sx, sy), Point2DI(ex, ey), color);
}

void	KigsBitmap::Print(const std::string& txt, int posx, int posy, unsigned int _maxLineNumber, unsigned int maxSize, unsigned int fontSize, const std::string& fontName, unsigned int a_Alignment, const KigsBitmapPixel& color)
{
	bool bRet = false;
	unsigned char* pImageData = 0;
	
	int L_Width = 0;
	int L_Height = 0;

	do
	{
		if (!txt.length())
			break;
		fontSize = (int)(((float)fontSize) * FREETYPE_SIZE_COEFF);

		int textSize = txt.length();

		if (!ModuleSpecificRenderer::mDrawer->IsInCache(fontName.c_str()))
		{
			auto pathManager = KigsCore::Singleton<FilePathManager>();
			SmartPointer<FileHandle> fullfilenamehandle;

			if (pathManager)
			{
				fullfilenamehandle = pathManager->FindFullName(fontName);
			}
			if ((fullfilenamehandle->mStatus & FileHandle::Exist) == 0)
				break;

			u64 size;
			CoreRawBuffer* L_Buffer = ModuleFileManager::LoadFile(fullfilenamehandle.get(), size);
			if (L_Buffer)
			{
				unsigned char* pBuffer = (unsigned char*)L_Buffer->CopyBuffer();
				ModuleSpecificRenderer::mDrawer->SetFont(fontName.c_str(), pBuffer, size, fontSize);
				L_Buffer->Destroy();
			}
			else
				break;
		}
		else
		{
			ModuleSpecificRenderer::mDrawer->SetFont(fontName.c_str(), 0, 0, fontSize);
		}

		pImageData = ModuleSpecificRenderer::mDrawer->DrawTextToImage(txt.c_str(), textSize, L_Width, L_Height, (TextAlignment)a_Alignment, false, _maxLineNumber, maxSize, -1, (unsigned char)color.R, (unsigned char)color.G, (unsigned char)color.B);

		if (!pImageData)
			break;

		bRet = true;
	} while (0);

	if (bRet)
	{
		KigsBitmapPixel* readbuffer = (KigsBitmapPixel * )pImageData;

		KigsBitmapPixel* writePixel = (KigsBitmapPixel*)mRawPixels;
		writePixel += posx;
		writePixel += posy * mSize[0];

		// copy bitmap
		for (u32 y = 0; y < L_Height; y++)
		{
			u32 writeposy = posy + y;
			if ((writeposy >= 0) && (writeposy < mSize[1]))
			{
				for (u32 x = 0; x < L_Width; x++)
				{
					u32 writeposx = posx + x;
					if ((writeposx >= 0) && (writeposx < mSize[0]))
					{
						if (readbuffer[x + y * L_Width].A != 0)
						{
							writePixel[x + y * mSize[0]] = readbuffer[x + y * L_Width];
						}
					}
				}
			}
		}

	}

	if (pImageData)
		delete[] pImageData;

	return;
}


void KigsBitmap::Line(Point2DI p1, Point2DI p2, const KigsBitmapPixel& color)
{
	Point2DI dv(p2);
	dv -= p1;

	KigsBitmapPixel* startPixel = (KigsBitmapPixel*)mRawPixels;
	if (abs(dv.x)>abs(dv.y)) // horizontal
	{
		int startX = (int)(p1.x + 0.5f);
		int endX = (int)(p2.x + 0.5f);
		float startY = p1.y;
		float slope = ((float)dv.y) / ((float)dv.x);
		if (dv.x<0.0f)
		{
			//slope=-slope;
			int swap = startX;
			startX = endX;
			endX = swap;
			startY = p2.y;
		}
		int i;
		
		for (i = startX; i <= endX; i++)
		{
			int posX = i;
			int posY = ((int)(startY + 0.5f));
			if ((posX >= 0) && (posY >= 0) && (posX<mSize[0]) && (posY<mSize[1]))
			{
				int pos = posY*mSize[0]+posX;
				startPixel[pos] = color;
			}
			startY += slope;
		}
	}
	else // vertical
	{
		if (dv.y != 0)
		{
			int startY = (int)(p1.y + 0.5f);
			int endY = (int)(p2.y + 0.5f);
			float startX = p1.x;
			float slope = ((float)dv.x) / ((float)dv.y);
			if (dv.y < 0.0f)
			{
				//slope=-slope;
				int swap = startY;
				startY = endY;
				endY = swap;
				startX = p2.x;
			}
			int i;
			for (i = startY; i <= endY; i++)
			{
				int posY = i;
				int posX = ((int)(startX + 0.5f));
				if ((posX >= 0) && (posY >= 0) && (posX < mSize[0]) && (posY < mSize[1]))
				{
					int pos = posY*mSize[0] + posX;
					startPixel[pos] = color;
				}
				startX += slope;
			}
		}
		else
		{
			int posY = p1.y;
			int posX = p1.x;
			if ((posX >= 0) && (posY >= 0) && (posX < mSize[0]) && (posY < mSize[1]))
			{
				int pos = posY*mSize[0] + posX;
				startPixel[pos] = color;
			}
		}
	}

	mDirtyZone.Update(p1);
	mDirtyZone.Update(p2);

}


void	KigsBitmap::ScrollX(int offset, const KigsBitmapPixel& color)
{
	// use memmove to copy with overlap
	if (abs(offset) >= mSize[0])
	{
		Clear(color);
		return;
	}

	if (offset == 0)
	{
		return;
	}
	KigsBitmapPixel* startPixel = (KigsBitmapPixel*)mRawPixels;
	if (offset > 0)
	{
		// scroll left to right
		memmove(startPixel + offset, startPixel, (mSize[0] * mSize[1] - offset) * sizeof(KigsBitmapPixel) );
		Box(0, 0, offset, mSize[1], color);
	}
	else
	{
		// scroll right to left
		memmove(startPixel, startPixel-offset, (mSize[0] * mSize[1] + offset ) * sizeof(KigsBitmapPixel) );
		Box(mSize[0]+offset, 0, -offset, mSize[1], color);
	}
	mDirtyZone.m_Min.Set(0, 0);
	mDirtyZone.m_Max.Set(mSize[0] - 1, mSize[1] - 1);

}
void	KigsBitmap::ScrollY(int offset, const KigsBitmapPixel& color)
{
	// use memmove to copy with overlap
	if (abs(offset) >= mSize[1])
	{
		Clear(color);
		return;
	}

	if (offset == 0)
	{
		return;
	}
	KigsBitmapPixel* startPixel = (KigsBitmapPixel*)mRawPixels;
	if (offset > 0)
	{
		// scroll top to bottom
		memmove(startPixel + offset*mSize[0], startPixel, (mSize[0] * (mSize[1] - offset)) * sizeof(KigsBitmapPixel));
		Box(0, 0, mSize[0],offset, color);
	}
	else
	{
		// scroll right to left
		memmove(startPixel, startPixel - offset*mSize[0], (mSize[0] * (mSize[1] + offset)) * sizeof(KigsBitmapPixel));
		Box(0,mSize[1] + offset, mSize[0], -offset, color);
	}
	mDirtyZone.m_Min.Set(0, 0);
	mDirtyZone.m_Max.Set(mSize[0] - 1, mSize[1] - 1);
}