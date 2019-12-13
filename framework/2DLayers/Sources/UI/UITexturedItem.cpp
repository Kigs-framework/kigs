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
	myTexture = nullptr;
}

void UITexturedItem::SetTexUV(UIVerticesInfo * aQI)
{
	if (!myTexture.isNil())
	{
		kfloat ratioX, ratioY, sx, sy;
		myTexture->GetSize(sx, sy);
		myTexture->GetRatio(ratioX, ratioY);

		v2f uv_min = mUVMin;
		v2f uv_max = mUVMax;

		if (uv_min == v2f(FLT_MAX, FLT_MAX)) uv_min = { 0,0 };
		if (uv_max == v2f(FLT_MAX, FLT_MAX)) uv_max = { ratioX, ratioY };

		v2f image_size{ sx*ratioX, sy*ratioY };

		kfloat dx = 0.5f / sx;
		kfloat dy = 0.5f / sy;

		VInfo2D::Data* buf = reinterpret_cast<VInfo2D::Data*>(aQI->Buffer());

		aQI->Flag |= UIVerticesInfo_Texture;

		bool is_bgr = false;
		if (myTexture->getValue("IsBGR", is_bgr) && is_bgr)
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
	myTexture = NULL;
}

void UITexturedItem::NotifyUpdate(const unsigned int labelid)
{
	if (!myTexture.isNil())
	{
		/*if (labelid == mySizeX.getLabelID())
		{
			if (mySizeX == 0)
			{
				unsigned int val;
				myTexture->getValue(LABEL_TO_ID(Width), val);
				mySizeX = val;
			}
		}
		else if (labelid == mySizeY.getLabelID())
		{
			if (mySizeY == 0)
			{
				unsigned int val;
				myTexture->getValue(LABEL_TO_ID(Height), val);
				mySizeY = val;
			}
		}*/
	}

	UIItem::NotifyUpdate(labelid);
}

void UITexturedItem::PreDraw(TravState* state)
{
	if (myTexture)
		myTexture->DoPreDraw(state);
}

void UITexturedItem::PostDraw(TravState* state)
{
	if (myTexture)
		myTexture->DoPostDraw(state);
}

int UITexturedItem::GetTransparencyType()
{
	if (myTexture && GetOpacity() == 1.0f)
		return myTexture->GetTransparency();
	else // overall transparency
		return 2;
}

void     UITexturedItem::SetTexture(Texture* t)
{
	myTexture = NonOwningRawPtrToSmartPtr(t);

	if (myTexture == nullptr)
		return;

	if (getAttribute("HasDynamicTexture"))
		myTexture->setValue("IsDynamic", true);

	myTexture->SetRepeatUV(false, false);
}




bool UITexturedItem::addItem(CoreModifiable *item, ItemPosition pos DECLARE_LINK_NAME)
{
	if (item->isSubType(Texture::myClassID))
	{
		myTexture = NonOwningRawPtrToSmartPtr(item);

		if (myTexture && getAttribute("HasDynamicTexture"))
			myTexture->setValue("IsDynamic", true);
	}

	return UIDrawableItem::addItem(item, pos PASS_LINK_NAME(linkName));
}

bool UITexturedItem::removeItem(CoreModifiable* item DECLARE_LINK_NAME)
{
	if (item->isSubType(Texture::myClassID))
	{
		if (item == myTexture.get())
		{
			myTexture = 0;
		}
	}
	return UIDrawableItem::removeItem(item PASS_LINK_NAME(linkName));
}
