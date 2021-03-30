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
	// create empty Textured Item
	mTexturePointer = KigsCore::GetInstanceOf(getName()+"_TextureHandler", "TextureHandler");
	mTexturePointer->Init();

	KigsCore::Connect(mTexturePointer.get(), "NotifyUpdate", this , "TextureNotifyUpdate");
}

void UITexturedItem::SetTexUV(UIVerticesInfo * aQI)
{
	
	if (!mTexturePointer.isNil())
	{
		if (mTexturePointer->getTexture().isNil())
		{
			return;
		}
		aQI->Flag |= UIVerticesInfo_Texture;

		bool is_bgr = false;
		if (mTexturePointer->getValue("IsBGR", is_bgr) && is_bgr)
		{
			aQI->Flag |= UIVerticesInfo_BGRTexture;
		}

		if (mShape)
		{
			return mShape->SetTexUV(this, aQI);
		}
		
		v2f isize;
		mTexturePointer->GetSize(isize.x, isize.y);

	
		VInfo2D::Data* buf = reinterpret_cast<VInfo2D::Data*>(aQI->Buffer());

		auto slice_size = (v2f)mSliced;
		if (slice_size == v2f(0, 0))
		{
			// triangle strip order
			v2f uvpos = mTexturePointer->getUVforPosInPixels({ 0.f,0.f });
			buf[0].setTexUV(uvpos.x, uvpos.y);
			uvpos = mTexturePointer->getUVforPosInPixels({ 0.f,isize.y - 1.0f });
			buf[1].setTexUV(uvpos.x, uvpos.y);
			uvpos = mTexturePointer->getUVforPosInPixels({ isize.x-1.0f,isize.y - 1.0f });
			buf[3].setTexUV(uvpos.x, uvpos.y);
			uvpos = mTexturePointer->getUVforPosInPixels({ isize.x - 1.0f,0.0f });
			buf[2].setTexUV(uvpos.x, uvpos.y);
		}
		else
		{
			kfloat ratioX, ratioY;
			mTexturePointer->GetRatio(ratioX, ratioY);

			v2f image_size{ isize.x * ratioX, isize.y * ratioY };

			auto set_quad_uv = [&](v2f*& pts, v2f start_pos, v2f size)
			{
			
				v2f uvpos = mTexturePointer->getUVforPosInPixels(start_pos);
				pts[0] = uvpos;
				uvpos = mTexturePointer->getUVforPosInPixels({ start_pos.x,start_pos.y + size.y });
				pts[1] = uvpos;
				pts[4] = uvpos;
				uvpos = mTexturePointer->getUVforPosInPixels({ start_pos.x+size.x ,start_pos.y });
				pts[2] = uvpos;
				pts[3] = uvpos;
				uvpos = mTexturePointer->getUVforPosInPixels({ start_pos.x + size.x ,start_pos.y+size.y });
				pts[5] = uvpos;
				
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
	if(!mTexturePointer.isNil())
		KigsCore::Disconnect(mTexturePointer.get(), "NotifyUpdate", this, "TextureNotifyUpdate");
	mTexturePointer = NULL;
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

bool UITexturedItem::addItem(const CMSP& item, ItemPosition pos DECLARE_LINK_NAME)
{
	if (item->isSubType(Texture::mClassID))
	{
		mTexturePointer->setTexture((SP<Texture>&)item);

		SetNodeFlag(Node2D_SizeChanged);

		if (mTexturePointer && getAttribute("HasDynamicTexture"))
			mTexturePointer->setValue("IsDynamic", true);
	}

	return UIDrawableItem::addItem(item, pos PASS_LINK_NAME(linkName));
}

bool UITexturedItem::removeItem(const CMSP& item DECLARE_LINK_NAME)
{
	if (item->isSubType(Texture::mClassID))
	{
		if (item.get() == mTexturePointer->getTexture().get())
		{
			mTexturePointer->setTexture(nullptr);
			SetNodeFlag(Node2D_SizeChanged);
		}
	}
	return UIDrawableItem::removeItem(item PASS_LINK_NAME(linkName));
}

void	UITexturedItem::TextureNotifyUpdate(const unsigned int  labelid)
{
	if (labelid == KigsID("TextureName"))
	{
		SetNodeFlag(Node2D_SizeChanged);
	}
}