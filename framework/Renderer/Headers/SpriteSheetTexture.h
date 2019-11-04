#ifndef _SPRITESHEETTEXTURE_H_
#define _SPRITESHEETTEXTURE_H_

#include "CoreMap.h"
#include "CoreVector.h"
#include "CoreModifiableAttribute.h"

#include "Texture.h"

#include <memory>

struct SpriteSheetFrame
{
	int FramePos_X;
	int FramePos_Y;
	int Decal_X;
	int Decal_Y;
	int FrameSize_X;
	int FrameSize_Y;
	int SourceSize_X;
	int SourceSize_Y;
	bool Rotated;
	bool Trimmed;
};

class SpriteSheetTexture : public CoreModifiable
{
public:
	DECLARE_CLASS_INFO(SpriteSheetTexture,CoreModifiable,2DLayers);
	DECLARE_INLINE_CONSTRUCTOR(SpriteSheetTexture) {}

	Texture* Get_Texture() { return mTexture.get(); }

	void Get_SourceSize(const std::string& _Animation, int& _Ssx, int &Ssy);
	
	unsigned int Get_FrameNumber(const std::string& _Animation);

	std::set<std::string> Get_AnimationList();

	const SpriteSheetFrame*	Get_Frame(const std::string& _value);
	const SpriteSheetFrame*	Get_AnimFrame(const std::string& _value, unsigned int _frameNumber);

	virtual CoreModifiable*	getSharedInstance() override;

protected:
	virtual ~SpriteSheetTexture();
	void InitModifiable() override;

	maString	mFileName = BASE_ATTRIBUTE(FileName, "");
	SmartPointer<Texture> mTexture;

private:
	void SortAnimation(CoreVector& _FrameVector);

	//parser variables
	std::map<std::string, std::vector<SpriteSheetFrame*>>		mAnimationList;
	std::map<std::string, std::unique_ptr<SpriteSheetFrame>>	mAllFrameList;
};

#endif //_SPRITESHEETTEXTURE_H_