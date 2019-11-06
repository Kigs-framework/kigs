#ifndef _HOLO3DPANEL_H_
#define _HOLO3DPANEL_H_

#include "Drawable.h"
#include "Texture.h"
#include "UIVerticesInfo.h"

class Texture;

class Holo3DPanel : public Drawable
{
public:
	DECLARE_CLASS_INFO(Holo3DPanel, Drawable, Renderer);
	DECLARE_CONSTRUCTOR(Holo3DPanel);


	bool addItem(CoreModifiable* item, ItemPosition pos = Last) override;
	bool removeItem(CoreModifiable* item) override;

	void SetTextureID(unsigned int id) { myTextureID = id; }
	void SetTexture(Texture* t);

	bool Draw(TravState* travstate) override;

protected:	
	
	void InitModifiable() override;

	SmartPointer<Texture> myTexture;
	unsigned int myTextureID=-1;

	UIVerticesInfo myVI;


	maVect2DF myOffset = BASE_ATTRIBUTE(Offset, 0, 0);
	/*	
		Size:
			For a plane, SizeX, SizeY
			For a circle Radius, NumberOfPoints
	*/
	maVect2DF mySize = BASE_ATTRIBUTE(Size, 0, 0);

	maVect2DF myUVStart = BASE_ATTRIBUTE(UVStart, 0, 0);
	maVect2DF myUVEnd = BASE_ATTRIBUTE(UVEnd, 1, 1);

	maVect3DF myUp = BASE_ATTRIBUTE(Up, 0, 1, 0);
	maVect3DF myNormal= BASE_ATTRIBUTE(Normal, 0, 0, 1);

	maBool mTwoSided = BASE_ATTRIBUTE(TwoSided, false);
	maEnum<3> mDepthTest = BASE_ATTRIBUTE(DepthTest, "NoChange", "Enabled", "Disabled");
	maEnum<2> mShape = BASE_ATTRIBUTE(Shape, "Plane", "Circle");
};

#endif //_HOLO3DPANEL_H_
