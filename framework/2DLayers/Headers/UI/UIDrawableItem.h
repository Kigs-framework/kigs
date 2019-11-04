#ifndef _UIDrawableItem_H_
#define _UIDrawableItem_H_

#include "UI/UIItem.h"
#include "SmartPointer.h"
#include "UIVerticesInfo.h"

class Texture;
class UIDrawableItem;

#define		UserFlagUseColorArray	(8)

class UIDrawableItem : public UIItem
{
public:
	DECLARE_CLASS_INFO(UIDrawableItem, UIItem, 2DLayers);
	DECLARE_CONSTRUCTOR(UIDrawableItem);
	
	void ProtectedDraw(TravState* state) override;
	void SetVertexArray(UIVerticesInfo * aQI) override;
	void SetColor(UIVerticesInfo * aQI) override;
	UIVerticesInfo * GetVerticesInfo() override { return &mVI; }

	virtual void SetWhiteColor(UIVerticesInfo * aQI);

protected:
	UIVerticesInfo mVI;

	maVect2DI mSliced = BASE_ATTRIBUTE(Sliced, 0, 0);
};


#endif //_UIItem_H_
