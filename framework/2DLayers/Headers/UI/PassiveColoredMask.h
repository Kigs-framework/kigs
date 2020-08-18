#ifndef _PASSIVECOLOREDMASK_H_
#define _PASSIVECOLOREDMASK_H_

#include "UIItem.h"
#include "SmartPointer.h"
#include "Texture.h"

// ****************************************
// * PassiveColoredMask class
// * --------------------------------------
/**
* \file	PassiveColoredMask.h
* \class	PassiveColoredMask
* \ingroup 2DLayers
* \brief	Obsolete ?
*
*/
// ****************************************
class PassiveColoredMask : public UIItem
{
public:
	DECLARE_CLASS_INFO(PassiveColoredMask,UIItem,2DLayers)

	PassiveColoredMask(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	void					ApplyColorFrom(float X, float Y, int R, int G, int B, bool IsRGBA=true, unsigned int extendedRay=0);
	void					ApplyColorFrom(float TopLeftCornerX, float TopLeftCornerY, int SizeX, int SizeY, int R, int G, int B, bool IsRGBA);
	void					RestoreOriginalColorAt(float TopLeftCornerX, float TopLeftCornerY, int SizeX, int SizeY, bool IsRGBA);
	void					PixelCopyFromMask(int TopLeftCornerX, int TopLeftCornerY, int SizeX, int SizeY);
	Texture*				GetTexture();
	virtual void			ReloadTexture();
	inline void				GetSize(unsigned int &X,unsigned int &Y) { X=mWidth; Y=mHeight; }
protected:
	/**
	 * \brief	destructor
	 */
	virtual					~PassiveColoredMask();

	/**
	 * \brief	init the modifiable and set the isInit flag to true if OK
	 */
	void			InitModifiable() override;

	bool					mIsDown;
	bool					mIsMouseOver;


	//! Alpha threshold
	maFloat					mThreshold;

	//! Texture Name
	maString				mTextureName;
	maString				mMaskName;
	maUInt					mRay;
	SmartPointer<Texture>	mTexture;

	int						mWidth;
	int						mHeight;
	int						mPow2Width;
	int						mPow2Height;
	unsigned int*			mPixelArray;
	unsigned int*			mOriginalPixelArray;
	unsigned int*			mPixelMaskArray;

	struct PLATFORM_COLOR
	{
		unsigned char R;
		unsigned char G;
		unsigned char B;
		unsigned char A;
	};

};

#endif //_PASSIVECOLOREDMASK_H_
