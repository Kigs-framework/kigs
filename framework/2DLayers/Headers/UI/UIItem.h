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

// ****************************************
// * UIItem class
// * --------------------------------------
/**
* \file	UIItem.h
* \class	UIItem
* \ingroup 2DLayers
* \brief	Parent class for all UI types ( buttons, texts...)
*
*/
// ****************************************

class UIItem : public Node2D
{
public:
	DECLARE_CLASS_INFO(UIItem, Node2D, 2DLayers);
	DECLARE_CONSTRUCTOR(UIItem);

	/*Getter*/
	float												GetOpacity() override {
		if (mOpacity >= 0.0f) return mOpacity;
		if (mParent) return mParent->GetOpacity(); // if opacity is < 0.0 then check parent opacity
		return 0.0;
	}
	inline bool											Get_DisableBlend() const { return mDisableBlend; }
	inline bool                                         Get_IsHidden() const { return mIsHidden; }
	virtual SpriteSheetTexture*							GetSpriteSheetTexture() { return NULL; }

	/*Setter*/
	inline void											Set_RotationAngle(kfloat a_radAngle) { mRotationAngle = a_radAngle; mNeedUpdatePosition = true; }
	inline void											Set_Position(kfloat a_x, kfloat a_y) { mPosition[0] = a_x; mPosition[1] = a_y; mNeedUpdatePosition = true; }
	inline void											Set_Position(Point2D a_pos) { Set_Position(a_pos.x, a_pos.y); }
	inline void											IsTouchable(bool a_value) { mIsTouchable = a_value; }
	inline void											Set_Opacity(float a_value) { mOpacity = a_value; }
	inline void											Set_PreScale(kfloat a_valueX, kfloat a_valueY) { mPreScaleX = a_valueX; mPreScaleY = a_valueY; mNeedUpdatePosition = true; }
	inline void											Set_PostScale(kfloat a_valueX, kfloat a_valueY) { mPostScaleX = a_valueX; mPostScaleY = a_valueY; mNeedUpdatePosition = true; }
	inline void											Set_DisableBlend(bool a_value) { mDisableBlend = a_value; }
	
	bool												Draw(TravState* state) override;

	virtual bool										HasFocus() { return false; }
	virtual void										LoseFocus() {}
	virtual void										GetFocus() {}
	virtual bool										isAlpha(float X, float Y) { return false; };

	bool												ContainsPoint(kfloat X, kfloat Y);

	// utility method
	Point2D												GetCoordsInContainer(kfloat X, kfloat Y);

	inline void											TranslateWithOffSet(int a_offsetX, int a_offsetY) { mPosition[0] += a_offsetX; mPosition[1] += a_offsetY; mNeedUpdatePosition = true; }

	// call just before opengl drawelement
	virtual void PreDraw(TravState* state) {} // use for texture predraw if needed
	virtual void PostDraw(TravState* state) {} // use for texture postdraw if needed

	virtual int GetTransparencyType() { return (GetOpacity() == 1.0f) ? 0 : 2; }

	virtual void SetTexUV(UIVerticesInfo * aQI) {}
	virtual void SetVertexArray(UIVerticesInfo * aQI) {}
	virtual void SetColor(UIVerticesInfo * aQI) {}
	void GetColor(kfloat& R, kfloat& G, kfloat& B, kfloat& A) { R = mColor[0]; G = mColor[1]; B = mColor[2]; A = GetOpacity(); }

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
		if (mAlphaMask)
			mAlphaMask = nullptr;
	}

	maBoolHeritage<1>									mIsHidden;
	maBool												mIsTouchable;
	maBool												mDisableBlend;
	maBool												mIsEnabled;
	maVect3DF											mColor;  // USE [0,1] RANGE
	maFloat												mOpacity;
	SP<AlphaMask>										mAlphaMask;
	maBoolHeritage<1>									mSwallowInputs;
	bool												mFocus;

	WRAP_METHODS(ContainsPoint, ManageInputSwallowEvent);
};


#endif //_UIItem_H_
