#include "PrecompiledHeaders.h"

#include "KigsBitmap.h"
#include "Texture.h"

IMPLEMENT_CLASS_INFO(KigsBitmap)

KigsBitmap::KigsBitmap(const kstl::string& name, CLASS_NAME_TREE_ARG) : Drawable(name, PASS_CLASS_NAME_TREE_ARG)
, mySize(*this, true, LABEL_AND_ID(Size),0,0)
, myRawPixels(0)
{
	myDirtyZone.SetEmpty();
}

bool KigsBitmap::PreDraw(TravState* state)
{
	if (Drawable::PreDraw(state))
	{
		if (IsInit())
		{
			Point2DI	bbsize = myDirtyZone.Size();
			if ((bbsize.x > 0) && (bbsize.y>0))
			{
				if ((myDirtyZone.m_Max.x >= 0) && (myDirtyZone.m_Max.y >= 0) && (myDirtyZone.m_Min.x < mySize[0]) && (myDirtyZone.m_Min.y < mySize[1]))
				{
					if (myDirtyZone.m_Min.x < 0)
					{
						myDirtyZone.m_Min.x = 0;
					}
					if (myDirtyZone.m_Min.y < 0)
					{
						myDirtyZone.m_Min.y = 0;
					}
					if (myDirtyZone.m_Max.x >= mySize[0])
					{
						myDirtyZone.m_Max.x = mySize[0]-1;
					}
					if (myDirtyZone.m_Max.y >= mySize[1])
					{
						myDirtyZone.m_Max.y = mySize[1]-1;
					}
					const kstl::vector<CoreModifiable*>& parents = GetParents();
					kstl::vector<CoreModifiable*>::const_iterator	parentsBegin = parents.begin();
					kstl::vector<CoreModifiable*>::const_iterator	parentsEnd = parents.end();

					while (parentsBegin != parentsEnd)
					{
						if ((*parentsBegin)->isSubType(Texture::myClassID))
						{
							Texture*	parentToUpdate = (Texture*)(*parentsBegin);

							unsigned char*	startPixel = GetPixelAddress(myDirtyZone.m_Min.x, myDirtyZone.m_Min.y);

							parentToUpdate->UpdateBufferZone(startPixel, myDirtyZone, Point2DI(mySize[0], mySize[1]));


							break;
						}
						++parentsBegin;
					}
				}
			}
			myDirtyZone.SetEmpty();
		}
		return true;
	}
	return false;
}


void	KigsBitmap::InitModifiable()
{
	if (!IsInit())
	{
		if ((mySize[0] > 0) && (mySize[1] > 0))
		{
			Drawable::InitModifiable();

			myRawPixels = new unsigned char[mySize[0] * mySize[1] * 4];

			// clear all
			memset(myRawPixels, 0, mySize[0] * mySize[1] * 4);

			myDirtyZone.m_Min.Set(0, 0);
			myDirtyZone.m_Max.Set(mySize[0]-1, mySize[1]-1);
		}
	}
}

KigsBitmap::~KigsBitmap()
{
	if (myRawPixels)
	{
		delete[] myRawPixels;
	}
}

void	KigsBitmap::Box(int x, int y, int sizex, int sizey, const KigsBitmapPixel& color)
{
	// check bounds
	
	if( ((x + sizex) < 0) || (x>=mySize[0]))
	{
		return;
	}
	if( ((y + sizey) < 0) || (y >= mySize[1]))
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

	if ( (x + sizex) >= mySize[0])
	{
		sizex = mySize[0] - x;
	}
	if ((y + sizey) >= mySize[1])
	{
		sizey = mySize[1] - y;
	}

	myDirtyZone.Update(x, y);
	myDirtyZone.Update((x + sizex)-1, (y + sizey)-1);

	// draw first box line
	KigsBitmapPixel* firstPixel = (KigsBitmapPixel*)myRawPixels;
	firstPixel += x;
	firstPixel += y*mySize[0];
	int i;
	for (i = 0; i < sizex; i++)
	{
		firstPixel[i] = color;
	}
	// then copy line
	for (i = 1; i < sizey; i++)
	{
		memcpy(firstPixel+i*mySize[0], firstPixel, sizex*sizeof(KigsBitmapPixel));
	}

}

void KigsBitmap::Line(int sx, int sy, int ex, int ey, const KigsBitmapPixel& color)
{
	Line(Point2DI(sx, sy), Point2DI(ex, ey), color);
}

void KigsBitmap::Line(Point2DI p1, Point2DI p2, const KigsBitmapPixel& color)
{
	Point2DI dv(p2);
	dv -= p1;

	KigsBitmapPixel* startPixel = (KigsBitmapPixel*)myRawPixels;
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
			if ((posX >= 0) && (posY >= 0) && (posX<mySize[0]) && (posY<mySize[1]))
			{
				int pos = posY*mySize[0]+posX;
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
				if ((posX >= 0) && (posY >= 0) && (posX < mySize[0]) && (posY < mySize[1]))
				{
					int pos = posY*mySize[0] + posX;
					startPixel[pos] = color;
				}
				startX += slope;
			}
		}
		else
		{
			int posY = p1.y;
			int posX = p1.x;
			if ((posX >= 0) && (posY >= 0) && (posX < mySize[0]) && (posY < mySize[1]))
			{
				int pos = posY*mySize[0] + posX;
				startPixel[pos] = color;
			}
		}
	}

	myDirtyZone.Update(p1);
	myDirtyZone.Update(p2);

}


void	KigsBitmap::ScrollX(int offset, const KigsBitmapPixel& color)
{
	// use memmove to copy with overlap
	if (abs(offset) >= mySize[0])
	{
		Clear(color);
		return;
	}

	if (offset == 0)
	{
		return;
	}
	KigsBitmapPixel* startPixel = (KigsBitmapPixel*)myRawPixels;
	if (offset > 0)
	{
		// scroll left to right
		memmove(startPixel + offset, startPixel, (mySize[0] * mySize[1] - offset) * sizeof(KigsBitmapPixel) );
		Box(0, 0, offset, mySize[1], color);
	}
	else
	{
		// scroll right to left
		memmove(startPixel, startPixel-offset, (mySize[0] * mySize[1] + offset ) * sizeof(KigsBitmapPixel) );
		Box(mySize[0]+offset, 0, -offset, mySize[1], color);
	}
	myDirtyZone.m_Min.Set(0, 0);
	myDirtyZone.m_Max.Set(mySize[0] - 1, mySize[1] - 1);

}
void	KigsBitmap::ScrollY(int offset, const KigsBitmapPixel& color)
{
	// use memmove to copy with overlap
	if (abs(offset) >= mySize[1])
	{
		Clear(color);
		return;
	}

	if (offset == 0)
	{
		return;
	}
	KigsBitmapPixel* startPixel = (KigsBitmapPixel*)myRawPixels;
	if (offset > 0)
	{
		// scroll top to bottom
		memmove(startPixel + offset*mySize[0], startPixel, (mySize[0] * (mySize[1] - offset)) * sizeof(KigsBitmapPixel));
		Box(0, 0, mySize[0],offset, color);
	}
	else
	{
		// scroll right to left
		memmove(startPixel, startPixel - offset*mySize[0], (mySize[0] * (mySize[1] + offset)) * sizeof(KigsBitmapPixel));
		Box(0,mySize[1] + offset, mySize[0], -offset, color);
	}
	myDirtyZone.m_Min.Set(0, 0);
	myDirtyZone.m_Max.Set(mySize[0] - 1, mySize[1] - 1);
}