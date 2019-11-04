#ifndef _PASSIVECOLOREDMASK_H_
#define _PASSIVECOLOREDMASK_H_

#include "UIItem.h"
#include "SmartPointer.h"
#include "Texture.h"

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
	inline void				GetSize(unsigned int &X,unsigned int &Y) { X=myWidth; Y=myHeight; }
protected:
	/**
	 * \brief	destructor
	 */
	virtual					~PassiveColoredMask();

	/**
	 * \brief	init the modifiable and set the isInit flag to true if OK
	 */
	void			InitModifiable() override;

	bool					myIsDown;
	bool					myIsMouseOver;


	//! Alpha threshold
	maFloat					myThreshold;

	//! Texture Name
	maString				myTextureName;
	maString				myMaskName;
	maUInt					myRay;
	SmartPointer<Texture>	myTexture;

	int						myWidth;
	int						myHeight;
	int						myPow2Width;
	int						myPow2Height;
	unsigned int*			myPixelArray;
	unsigned int*			myOriginalPixelArray;
	unsigned int*			myPixelMaskArray;

	struct PLATFORM_COLOR
	{
		unsigned char R;
		unsigned char G;
		unsigned char B;
		unsigned char A;
	};

};

#endif //_PASSIVECOLOREDMASK_H_
