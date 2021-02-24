#include "PrecompiledHeaders.h"
#include "ShrinkerWindows.h"
#include "Core.h"


IMPLEMENT_CLASS_INFO(ShrinkerWindows)

//! constructor
ShrinkerWindows::ShrinkerWindows(const kstl::string& name,CLASS_NAME_TREE_ARG) : GenericShrinker(name,PASS_CLASS_NAME_TREE_ARG)
{
   
}     

//! destructor
ShrinkerWindows::~ShrinkerWindows()
{
	if(mFrameBuffers[0])
	{
		FreeFrameBuffers();
	}
}    

void ShrinkerWindows::AllocateFrameBuffers()
{
	int buffersize=mResizeX*mResizeY;
	mFrameBuffers[0] = OwningRawPtrToSmartPtr(new AlignedCoreRawBuffer<16, unsigned char>(buffersize*mPixelSize, false));
	mFrameBuffers[1] = OwningRawPtrToSmartPtr(new AlignedCoreRawBuffer<16, unsigned char>(buffersize*mPixelSize, false));
	mFrameBuffers[2] = OwningRawPtrToSmartPtr(new AlignedCoreRawBuffer<16, unsigned char>(buffersize*mPixelSize, false));
}

void ShrinkerWindows::FreeFrameBuffers()
{
	mFrameBuffers[0] = nullptr;
	mFrameBuffers[1] = nullptr;
	mFrameBuffers[2] = nullptr;
}

void	ShrinkerWindows::RGBShrink()
{
	unsigned short* inputFrame=0;
	int	readyBuffer=-1;
	int	freebuffer=GetFreeBuffer();
	if(freebuffer == -1)
	{
		return;
	}
	if(mFrameBufferStream)
	{
		readyBuffer=mFrameBufferStream->GetReadyBufferIndex();

		if(readyBuffer!=-1)
		{
			inputFrame=(unsigned short*)mFrameBufferStream->GetBuffer(readyBuffer);
		}
	}

	if(!inputFrame)
	{
		return;
	}

	unsigned short* pInput=inputFrame;
	unsigned short* pOutput=(unsigned short*)GetBuffer(freebuffer);

	SetBufferState(freebuffer,ProcessingBuffer);
	int Width,Height;
	mFrameBufferStream->GetBufferSize(Width,Height);

	// basic shrink

	int i,j;

	kfloat	coordY=KFLOAT_CONST(0.0f);
	kfloat	coefY=(kfloat)Height/(kfloat)mResizeY;
	kfloat	coefX=(kfloat)Width/(kfloat)mResizeX;

	int		icoefx=(int)(coefX);
	int		icoefy=(int)(coefY);
	int		icoefprod = icoefx*icoefy;

	for(j=0;j<mResizeY;j++)
	{
		kfloat	coordX=KFLOAT_CONST(0.0f);
		unsigned short*	currentRow=pInput+Width*((int)coordY);
		for(i=0;i<mResizeX;i++)
		{
			int k,l;

			unsigned int lpixelR=0;
			unsigned int lpixelG=0;
			unsigned int lpixelB=0;

			for(k=0;k<icoefy;k++)
			{
				for(l=0;l<icoefx;l++)
				{
					unsigned int thispixel=currentRow[(int)coordX+l+k*Width];

					unsigned int thispixelR=(thispixel>>10)&31;
					unsigned int thispixelG=(thispixel>>5)&31;
					unsigned int thispixelB=(thispixel)&31;

					lpixelR+=thispixelR;
					lpixelG+=thispixelG;
					lpixelB+=thispixelB;
				}
			}

			if(icoefprod>1)
			{
				lpixelR/=(icoefprod);
				lpixelG/=(icoefprod);
				lpixelB/=(icoefprod);
			}
			// convert to 1555 (ABGR)
			unsigned short	pixel=(((lpixelR&31)<<10)|((lpixelG&31)<<5)|((lpixelB&31))|0x8000);

			*pOutput++=pixel;
			coordX+=coefX;
		}
		coordY+=coefY;
	}

	mFrameBufferStream->SetBufferState(readyBuffer,FreeBuffer);
	SetBufferState(freebuffer,ReadyBuffer);

}

void	ShrinkerWindows::YUVShrink()
{
	unsigned short* inputFrame=0;
	int	readyBuffer=-1;
	int	freebuffer=GetFreeBuffer();
	if(freebuffer == -1)
	{
		return;
	}
	if(mFrameBufferStream)
	{
		readyBuffer=mFrameBufferStream->GetReadyBufferIndex();

		if(readyBuffer!=-1)
		{
			inputFrame=(unsigned short*)mFrameBufferStream->GetBuffer(readyBuffer);
		}
	}

	if(!inputFrame)
	{
		return;
	}

	unsigned short* pInput=inputFrame;
	unsigned int* pOutput=(unsigned int*)GetBuffer(freebuffer);

	SetBufferState(freebuffer,ProcessingBuffer);
	int Width,Height;
	mFrameBufferStream->GetBufferSize(Width,Height);

	// basic shrink

	int i,j;

	kfloat	coordY=KFLOAT_CONST(0.0f);
	kfloat	coefY=(kfloat)Height/(kfloat)mResizeY;
	kfloat	coefX=(kfloat)Width/(kfloat)mResizeX;

	int		icoefx=(int)(coefX);
	int		icoefy=(int)(coefY);

	for(j=0;j<mResizeY;j++)
	{
		kfloat	coordX=KFLOAT_CONST(0.0f);
		unsigned int*	currentRow=(unsigned int*)(pInput+Width*((int)coordY));
		for(i=0;i<mResizeX;i++)
		{
			int k,l;

			unsigned int lpixelY=0;
			unsigned int lpixelU=0;
			unsigned int lpixelV=0;

			for(k=0;k<icoefy;k++)
			{
				int	PixelOffsetY = (k*Width)/2;

				for(l=0;l<icoefx;l++)
				{
					int	pixelCoordX = (int)coordX+l;
					
					unsigned int thispixel=currentRow[PixelOffsetY + pixelCoordX/2];

					unsigned int thispixelY;
					if(pixelCoordX&1)
					{
						thispixelY=(thispixel>>16)&255;
					}
					else
					{
						thispixelY=(thispixel)&255;
					}

					unsigned int thispixelU=(thispixel>>8)&255;
					unsigned int thispixelV=(thispixel>>24)&255;

					lpixelY+=thispixelY;
					lpixelU+=thispixelU;
					lpixelV+=thispixelV;
				}
			}

			lpixelY/=(icoefy*icoefx);
			lpixelU/=(icoefy*icoefx);
			lpixelV/=(icoefy*icoefx);

			if(i&1)
			{
				unsigned int	pixel=(lpixelY<<16);
				(*pOutput)|=pixel;
				pOutput++;
			}
			else
			{
				unsigned int	pixel=(lpixelY) | (lpixelU<<8) | (lpixelV<<24);
				*pOutput=pixel;
			}
			coordX+=coefX;
		}
		coordY+=coefY;
	}

	mFrameBufferStream->SetBufferState(readyBuffer,FreeBuffer);
	SetBufferState(freebuffer,ReadyBuffer);

}

void ShrinkerWindows::Process()
{
	if((int)mFormat==0) // RGB
	{
		RGBShrink();
	}
	else
	{
		YUVShrink();
	}
}

void ShrinkerWindows::InitModifiable()
{

	GenericShrinker::InitModifiable();

	AllocateFrameBuffers();


}