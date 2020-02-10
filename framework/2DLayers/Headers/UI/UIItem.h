#ifndef _UIItem_H_
#define _UIItem_H_

#include "CoreModifiable.h"
#include "../GlobalEnum.h"
#include "../Node2D.h"

#include "AttributePacking.h"
#include "TouchInputEventManager.h"
#include "../AlphaMask.h"

class TravState;
class Texture;
class SpriteSheetTexture;
class UIVerticesInfo;
class BaseUI2DLayer;

class UIItem : public Node2D
{
public:
	DECLARE_CLASS_INFO(UIItem, Node2D, 2DLayers);
	DECLARE_CONSTRUCTOR(UIItem);

	/*Getter*/
	float												GetOpacity() override {
		if (myOpacity >= 0.0f) return myOpacity;
		if (myParent) return myParent->GetOpacity(); // if opacity is < 0.0 then check parent opacity
		return 0.0;
	}
	inline bool											Get_DisableBlend() const { return mybDisableBlend; }
	inline bool                                         Get_IsHidden() const { return myIsHidden; }
	virtual SpriteSheetTexture*							GetSpriteSheetTexture() { return NULL; }

	/*Setter*/
	inline void											Set_RotationAngle(kfloat a_radAngle) { myAngle = a_radAngle; myNeedUpdatePosition = true; }
	inline void											Set_Position(kfloat a_x, kfloat a_y) { myPosition[0] = a_x; myPosition[1] = a_y; myNeedUpdatePosition = true; }
	inline void											Set_Position(Point2D a_pos) { Set_Position(a_pos.x, a_pos.y); }
	inline void											IsTouchable(bool a_value) { mybIsTouchable = a_value; }
	//inline void											Set_IsHidden(bool a_value) { myIsHidden = a_value; }
	inline void											Set_Opacity(float a_value) { myOpacity = a_value; }
	inline void											Set_PreScale(kfloat a_valueX, kfloat a_valueY) { myPreScaleX = a_valueX; myPreScaleY = a_valueY; myNeedUpdatePosition = true; }
	inline void											Set_PostScale(kfloat a_valueX, kfloat a_valueY) { myPostScaleX = a_valueX; myPostScaleY = a_valueY; myNeedUpdatePosition = true; }
	inline void											Set_DisableBlend(bool a_value) { mybDisableBlend = a_value; }
	
	bool												Draw(TravState* state) override;

	virtual bool										HasFocus() { return false; }
	virtual void										LoseFocus() {}
	virtual void										GetFocus() {}
	virtual bool										isAlpha(float X, float Y) { return false; };

	bool												ContainsPoint(kfloat X, kfloat Y);

	// utility method
	Point2D												GetCoordsInContainer(kfloat X, kfloat Y);

	inline void											TranslateWithOffSet(int a_offsetX, int a_offsetY) { myPosition[0] += a_offsetX; myPosition[1] += a_offsetY; myNeedUpdatePosition = true; }

	// call just before opengl drawelement
	virtual void PreDraw(TravState* state) {} // use for texture predraw if needed
	virtual void PostDraw(TravState* state) {} // use for texture postdraw if needed

	virtual int GetTransparencyType() { return (GetOpacity() == 1.0f) ? 0 : 2; }

	virtual void SetTexUV(UIVerticesInfo * aQI) {}
	virtual void SetVertexArray(UIVerticesInfo * aQI) {}
	virtual void SetColor(UIVerticesInfo * aQI) {}

	virtual UIVerticesInfo * GetVerticesInfo() { return NULL;/* mQI; */ }

	BaseUI2DLayer*	getParentLayer();

	bool CanInteract(v2f pos, bool must_contain=true);

protected:

	bool ManageInputSwallowEvent(InputEvent& ev);

	void NotifyUpdate(const unsigned int labelid) override;

	// real drawing
	void										ProtectedDraw(TravState* state)  override {}

	virtual												~UIItem()
	{
		if (myAlphaMask)
			myAlphaMask = nullptr;
	}

	maBoolHeritage<1>									myIsHidden;
	maBool												mybIsTouchable;
	maBool												mybDisableBlend;
	maBool												myIsEnabled;
	maVect3DF											myColor;  // USE [0,1] RANGE
	maFloat												myOpacity;
	SP<AlphaMask>										myAlphaMask;
	maBoolHeritage<1>									mySwallowInputs;
	bool												myFocus;

	//UIVerticesInfo *									mQI;
	WRAP_METHODS(ContainsPoint, ManageInputSwallowEvent);
};


#endif //_UIItem_H_
