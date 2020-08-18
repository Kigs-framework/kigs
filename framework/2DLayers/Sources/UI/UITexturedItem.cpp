// #### 2DLayers Include 
#include "UI/UITexturedItem.h"
// #### Core Include
#include "Core.h"
// #### Renderer Include
#include "ModuleRenderer.h"
#include "Texture.h"

//#//////////////////////////////
//#		UITexturedItem
//#//////////////////////////////
//IMPLEMENT_AND_REGISTER_CLASS_INFO(UITexturedItem, UITexturedItem, 2DLayers);
IMPLEMENT_CLASS_INFO(UITexturedItem)

IMPLEMENT_CONSTRUCTOR(UITexturedItem)
{
	mTexturePointer = nullptr;
}

void UITexturedItem::SetTexUV(UIVerticesInfo * aQI)
{
	if (!mTexturePointer.isNil())
	{
		kfloat ratioX, ratioY, sx, sy;
		unsigned int p2sx, p2sy;
		mTexturePointer->GetSize(sx, sy);
		mTexturePointer->GetPow2Size(p2sx, p2sy);
		mTexturePointer->GetRatio(ratioX, ratioY);

		v2f uv_min = mUVMin;
		v2f uv_max = mUVMax;

		if (uv_min == v2f(FLT_MAX, FLT_MAX)) uv_min = { 0,0 };
		if (uv_max == v2f(FLT_MAX, FLT_MAX)) uv_max = { ratioX, ratioY };

		v2f image_size{ sx*ratioX, sy*ratioY };

		kfloat dx = 0.5f / ((float)p2sx);
		kfloat dy = 0.5f / ((float)p2sy);

		VInfo2D::Data* buf = reinterpret_cast<VInfo2D::Data*>(aQI->Buffer());

		aQI->Flag |= UIVerticesInfo_Texture;

		bool is_bgr = false;
		if (mTexturePointer->getValue("IsBGR", is_bgr) && is_bgr)
		{
			aQI->Flag |= UIVerticesInfo_BGRTexture;
		}

		auto slice_size = (v2f)mSliced;
		if (slice_size == v2f(0, 0))
		{
			// triangle strip order
			buf[0].setTexUV(uv_min.x + dx, uv_min.y + dy);
			buf[1].setTexUV(uv_min.x + dx, uv_max.y - dy);
			buf[3].setTexUV(uv_max.x - dx, uv_max.y - dy);
			buf[2].setTexUV(uv_max.x - dx, uv_min.y + dy);
		}
		else
		{
			v2f pixelRatio = (uv_max - uv_min) * (1.0f / image_size);

			auto set_quad_uv = [&](v2f*& pts, v2f start_pos, v2f size)
			{
				start_pos *= pixelRatio;
				size *= pixelRatio;
				pts[0] = uv_min + start_pos;
				pts[1] = uv_min + start_pos + v2f(0, size.y);
				pts[2] = uv_min + start_pos + v2f(size.x, 0);

				pts[3] = uv_min + start_pos + v2f(size.x, 0);;
				pts[4] = uv_min + start_pos + v2f(0, size.y);
				pts[5] = uv_min + start_pos + v2f(size.x, size.y);
				
				pts += 6;
			};

			v2f uvs[6 * 9];
			v2f* current_uv = uvs;
			
			// Top Left
			set_quad_uv(current_uv, v2f(0, 0), slice_size);
			// Top Right
			set_quad_uv(current_uv, v2f(image_size.x - slice_size.x, 0), slice_size);
			// Bottom Left
			set_quad_uv(current_uv, v2f(0, image_size.y - slice_size.y), slice_size);
			// Bottom Right
			set_quad_uv(current_uv, v2f(image_size.x - slice_size.x, image_size.y - slice_size.y), slice_size);
			// Center
			set_quad_uv(current_uv, v2f(slice_size.x, slice_size.y), image_size - slice_size * 2);
			// Top
			set_quad_uv(current_uv, v2f(slice_size.x, 0), v2f((image_size - slice_size * 2).x, slice_size.y));
			// Bottom
			set_quad_uv(current_uv, v2f(slice_size.x, image_size.y - slice_size.y), v2f((image_size - slice_size * 2).x, slice_size.y));
			// Left
			set_quad_uv(current_uv, v2f(0, slice_size.y), v2f(slice_size.x, (image_size - slice_size * 2).y));
			// Right
			set_quad_uv(current_uv, v2f(image_size.x - slice_size.x, slice_size.y), v2f(slice_size.x, (image_size - slice_size * 2).y));

			for (int i = 0; i < 6 * 9; ++i)
				buf[i].setTexUV(uvs[i]);

		}
	}
}

UITexturedItem::~UITexturedItem()
{
	mTexturePointer = NULL;
}

void UITexturedItem::NotifyUpdate(const unsigned int labelid)
{
	if (!mTexturePointer.isNil())
	{
		/*if (labelid == mySizeX.getLabelID())
		{
			if (mySizeX == 0)
			{
				unsigned int val;
				mTexturePointer->getValue(LABEL_TO_ID(Width), val);
				mySizeX = val;
			}
		}
		else if (labelid == mySizeY.getLabelID())
		{
			if (mySizeY == 0)
			{
				unsigned int val;
				mTexturePointer->getValue(LABEL_TO_ID(Height), val);
				mySizeY = val;
			}
		}*/
	}

	UIItem::NotifyUpdate(labelid);
}

void UITexturedItem::PreDraw(TravState* state)
{
	if (mTexturePointer)
		mTexturePointer->DoPreDraw(state);
}

void UITexturedItem::PostDraw(TravState* state)
{
	if (mTexturePointer)
		mTexturePointer->DoPostDraw(state);
}

int UITexturedItem::GetTransparencyType()
{
	if (mTexturePointer && GetOpacity() == 1.0f)
		return mTexturePointer->GetTransparency();
	else // overall transparency
		return 2;
}

void     UITexturedItem::SetTexture(Texture* t)
{
	mTexturePointer = NonOwningRawPtrToSmartPtr(t);

	if (mTexturePointer == nullptr)
		return;

	if (getAttribute("HasDynamicTexture"))
		mTexturePointer->setValue("IsDynamic", true);

	mTexturePointer->SetRepeatUV(false, false);
}




bool UITexturedItem::addItem(const CMSP& item, ItemPosition pos DECLARE_LINK_NAME)
{
	if (item->isSubType(Texture::mClassID))
	{
		mTexturePointer = item;

		if (mTexturePointer && getAttribute("HasDynamicTexture"))
			mTexturePointer->setValue("IsDynamic", true);
	}

	return UIDrawableItem::addItem(item, pos PASS_LINK_NAME(linkName));
}

bool UITexturedItem::removeItem(const CMSP& item DECLARE_LINK_NAME)
{
	if (item->isSubType(Texture::mClassID))
	{
		if (item == mTexturePointer.get())
		{
			mTexturePointer = 0;
		}
	}
	return UIDrawableItem::removeItem(item PASS_LINK_NAME(linkName));
}
