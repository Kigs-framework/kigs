#ifndef _HOLO3DPANEL_H_
#define _HOLO3DPANEL_H_

#include "Drawable.h"
#include "Texture.h"
#include "UIVerticesInfo.h"

class Texture;

// ****************************************
// * Holo3DPanel class
// * --------------------------------------
/**
 * \file	Holo3DPanel.h
 * \class	Holo3DPanel
 * \ingroup Renderer
 * \brief	Draw a textured quad or circular panel.
 *
 * Used by Hololens menus.
 * Do we need this specific class ?
 * 
 */
 // ****************************************

class Holo3DPanel : public Drawable
{
public:
	DECLARE_CLASS_INFO(Holo3DPanel, Drawable, Renderer);
	DECLARE_CONSTRUCTOR(Holo3DPanel);


	bool addItem(const CMSP& item, ItemPosition pos = Last) override;
	bool removeItem(const CMSP& item) override;

	void SetTextureID(unsigned int id) { mTextureID = id; }
	void SetTexture(Texture* t);

	bool Draw(TravState* travstate) override;

protected:	
	
	void InitModifiable() override;

	SmartPointer<Texture> mTexture;
	unsigned int mTextureID=-1;

	UIVerticesInfo mVI;


	maVect2DF mOffset = BASE_ATTRIBUTE(Offset, 0, 0);
	/*	
		Size:
			For a plane, SizeX, SizeY
			For a circle Radius, NumberOfPoints
	*/
	maVect2DF mSize = BASE_ATTRIBUTE(Size, 0, 0);

	maVect2DF mUVStart = BASE_ATTRIBUTE(UVStart, 0, 0);
	maVect2DF mUVEnd = BASE_ATTRIBUTE(UVEnd, 1, 1);

	maVect3DF mUp = BASE_ATTRIBUTE(Up, 0, 1, 0);
	maVect3DF mNormal= BASE_ATTRIBUTE(Normal, 0, 0, 1);

	maBool mTwoSided = BASE_ATTRIBUTE(TwoSided, false);
	maEnum<3> mDepthTest = BASE_ATTRIBUTE(DepthTest, "NoChange", "Enabled", "Disabled");
	maEnum<2> mShape = BASE_ATTRIBUTE(Shape, "Plane", "Circle");
};

#endif //_HOLO3DPANEL_H_
