#ifndef _UITexturedItem_H_
#define _UITexturedItem_H_

#include "UI/UIDrawableItem.h"
#include "UI/UIShapeDelegate.h"
#include "UIVerticesInfo.h"
#include "SmartPointer.h"
#include "TextureHandler.h"

// ****************************************
// * UITexturedItem class
// * --------------------------------------
/**
* \file	UITexturedItem.h
* \class	UITexturedItem
* \ingroup 2DLayers
* \brief	Textured UIDrawableItem
*
*/
// ****************************************

class UITexturedItem : public UIDrawableItem
{
public:
	DECLARE_CLASS_INFO(UITexturedItem, UIDrawableItem, 2DLayers);

	/**
	* \brief	constructor
	* \param	name : instance name
	* \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	*/
	UITexturedItem(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	bool	HasTexture()
	{
		if (!mTexturePointer.isNil())
			return mTexturePointer->HasTexture();

		return false;
	}

	inline void	TransformUV(Point2D* totransform, int count) const
	{
		if (mTexturePointer.isNil())
			return;

		const Matrix4x4& mat = mTexturePointer->getUVTexture();

		v2f result;
		for (size_t i = 0; i < count; i++)
		{
			result.x = totransform[i].x * mat.e[0][0] + totransform[i].y * mat.e[0][1] + mat.e[0][2];
			result.y = totransform[i].y * mat.e[1][0] + totransform[i].y * mat.e[1][1] + mat.e[1][2];

			totransform[i] = result;
		}
	}

	// TODO check if needed
	SP<TextureHandler> GetTexture() { return mTexturePointer; }
	//void     SetTexture(const SP<TextureHandler>& t);

	// manage texture directly added 
	bool	addItem(const CMSP& item, ItemPosition pos = Last DECLARE_DEFAULT_LINK_NAME) override;
	bool	removeItem(const CMSP& item DECLARE_DEFAULT_LINK_NAME) override;
	
protected:

	void	TextureNotifyUpdate(const unsigned int  labelid);

	virtual v2f	GetContentSize() override
	{
		if (HasTexture())
		{
			v2f textureSize;
			mTexturePointer->GetSize(textureSize.x, textureSize.y);
			return textureSize;
		}
		return ParentClassType::GetContentSize();
	}

	virtual v2f getDrawablePos(const v2f& pos)
	{
		if (mTexturePointer)
			return mTexturePointer->getDrawablePos(pos);

		return pos;
	}

	virtual ~UITexturedItem();

	void SetTexUV(UIVerticesInfo * aQI) override;

	int GetTransparencyType() override;
	void PreDraw(TravState* state) override;  // use for texture predraw if needed
	void PostDraw(TravState* state) override; // use for texture postdraw if needed

	INSERT_FORWARDSP(TextureHandler,mTexturePointer);

	WRAP_METHODS(TextureNotifyUpdate);
};

#endif //_UIItem_H_
