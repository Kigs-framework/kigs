#include "Holo3DPanel.h"
#include "Texture.h"
#include "ModuleRenderer.h"
#include "TravState.h"

IMPLEMENT_CLASS_INFO(Holo3DPanel);

///////////////////////////////////////////

IMPLEMENT_CONSTRUCTOR(Holo3DPanel)
{
	mSortable = true;
	mRenderPassMask = 2; // Default to transparent pass
}

void Holo3DPanel::InitModifiable()
{
	ParentClassType::InitModifiable();
}

void Holo3DPanel::SetTexture(Texture* t)
{
	mTexture = NonOwningRawPtrToSmartPtr(t);
}

bool Holo3DPanel::addItem(const CMSP& item, ItemPosition pos)
{
	if (item->isSubType("Texture"))
	{
		SetTexture((Texture*)item.get());
	}
	return ParentClassType::addItem(item, pos);
}

bool Holo3DPanel::removeItem(const CMSP& item)
{
	if (item == mTexture.get())
	{
		SetTexture(nullptr);
	}
	return ParentClassType::removeItem(item);
}


///////////////////////////////////////////
bool Holo3DPanel::Draw(TravState* travstate)
{
	if (ParentClassType::Draw(travstate))
	{
		// create shader if none
		int lShaderMask = ModuleRenderer::VERTEX_ARRAY_MASK | ModuleRenderer::TEXCOORD_ARRAY_MASK | ModuleRenderer::NO_LIGHT_MASK | ModuleRenderer::ALPHA_TEST_LOW;
		auto renderer = travstate->GetRenderer();
		renderer->GetActiveShader()->ChooseShader(travstate, lShaderMask);

		// ** Manage Transparency **
		renderer->PushState();
		// NOTE(antoine) SetAlphaTestMode & SetAlphaMode don't do anything, it's a shader flag now
		//renderer->SetAlphaTestMode(RENDERER_ALPHA_TEST_ON);
		//renderer->SetAlphaMode(RENDERER_ALPHA_GREATER, 0.01f);
		renderer->SetBlendMode(RENDERER_BLEND_ON);
		renderer->SetBlendFuncMode(RENDERER_BLEND_SRC_ALPHA, RENDERER_BLEND_ONE_MINUS_SRC_ALPHA);

		int depth_test = mDepthTest;
		switch (depth_test)
		{
		case 1:
			//renderer->SetDepthTestMode(true);
			renderer->SetDepthTestFunc(RENDERER_DEPTH_TEST_LESS);
			break;
		case 2:
			//renderer->SetDepthTestMode(false);
			renderer->SetDepthTestFunc(RENDERER_DEPTH_TEST_ALWAYS);
			break;
		default:
			break;
		}

		int depth_write = mDepthWrite;
		switch (depth_write)
		{
		case 1:
			renderer->SetDepthMaskMode(RENDERER_DEPTH_MASK_ON);
			break;
		case 2:
			renderer->SetDepthMaskMode(RENDERER_DEPTH_MASK_OFF);
			break;
		default:
			break;
		}


		renderer->SetCullMode((RendererCullMode)(int)mCullMode);


		renderer->ActiveTextureChannel(0);

		// active texture 
		if (mTexture)
			mTexture->DoPreDraw(travstate);
		else if (mTextureID > -1)
		{
			renderer->BindTexture(RENDERER_TEXTURE_2D, mTextureID);
		}

		// compute position

		Vector3D n = (v3f)mNormal;
		Vector3D up = (v3f)mUp;
		Vector3D left = up ^ n;

		up.Normalize();
		left.Normalize();

		VInfo3D vi;

		if (mShape == 0)
		{
			auto offset_left = mOffset[0] * left;
			auto offset_up = mOffset[1] * up;

			left *= mSize[0] * 0.5f;
			up *= mSize[1] * 0.5f;
			mVI = UIVerticesInfo{ &vi, mTwoSided ? 12u : 6u };
			VInfo3D::Data* buf = reinterpret_cast<VInfo3D::Data*>(mVI.Buffer());
			// triangle strip order

			v2f uv0 = mUVStart;
			v2f uv1 = mUVEnd;

			buf[0].setValue(-left.x - up.x + offset_left.x + offset_up.x, -left.y - up.y + offset_left.y + offset_up.y, -left.z - up.z + offset_left.z + offset_up.z, 255, 255, 255, 255, uv1.x, uv0.y);
			buf[1].setValue(-left.x + up.x + offset_left.x + offset_up.x, -left.y + up.y + offset_left.y + offset_up.y, -left.z + up.z + offset_left.z + offset_up.z, 255, 255, 255, 255, uv1.x, uv1.y);
			buf[2].setValue(left.x - up.x + offset_left.x + offset_up.x, left.y - up.y + offset_left.y + offset_up.y, left.z - up.z + offset_left.z + offset_up.z, 255, 255, 255, 255, uv0.x, uv0.y);

			buf[3].setValue(-left.x + up.x + offset_left.x + offset_up.x, -left.y + up.y + offset_left.y + offset_up.y, -left.z + up.z + offset_left.z + offset_up.z, 255, 255, 255, 255, uv1.x, uv1.y);
			buf[4].setValue(left.x + up.x + offset_left.x + offset_up.x, left.y + up.y + offset_left.y + offset_up.y, left.z + up.z + offset_left.z + offset_up.z, 255, 255, 255, 255, uv0.x, uv1.y);
			buf[5].setValue(left.x - up.x + offset_left.x + offset_up.x, left.y - up.y + offset_left.y + offset_up.y, left.z - up.z + offset_left.z + offset_up.z, 255, 255, 255, 255, uv0.x, uv0.y);
			if (mTwoSided)
			{
				buf[6].setValue(-left.x - up.x + offset_left.x + offset_up.x, -left.y - up.y + offset_left.y + offset_up.y, -left.z - up.z + offset_left.z + offset_up.z, 255, 255, 255, 255, uv0.x, uv0.y);
				buf[7].setValue(left.x - up.x + offset_left.x + offset_up.x, left.y - up.y + offset_left.y + offset_up.y, left.z - up.z + offset_left.z + offset_up.z, 255, 255, 255, 255, uv1.x, uv0.y);
				buf[8].setValue(-left.x + up.x + offset_left.x + offset_up.x, -left.y + up.y + offset_left.y + offset_up.y, -left.z + up.z + offset_left.z + offset_up.z, 255, 255, 255, 255, uv0.x, uv1.y);

				buf[9].setValue(-left.x + up.x + offset_left.x + offset_up.x, -left.y + up.y + offset_left.y + offset_up.y, -left.z + up.z + offset_left.z + offset_up.z, 255, 255, 255, 255, uv0.x, uv1.y);
				buf[10].setValue(left.x - up.x + offset_left.x + offset_up.x, left.y - up.y + offset_left.y + offset_up.y, left.z - up.z + offset_left.z + offset_up.z, 255, 255, 255, 255, uv1.x, uv0.y);
				buf[11].setValue(left.x + up.x + offset_left.x + offset_up.x, left.y + up.y + offset_left.y + offset_up.y, left.z + up.z + offset_left.z + offset_up.z, 255, 255, 255, 255, uv1.x, uv1.y);
			}
		}
		else if (mShape == 1)
		{
			left *= mSize[0];
			up *= mSize[0];
			u32 nb_triangles = (u32)mSize[1];
			u32 nb_vertices = nb_triangles * 3u;
			if (mTwoSided) nb_vertices *= 2u;
			mVI = UIVerticesInfo{ &vi, nb_vertices };
			VInfo3D::Data* buf = reinterpret_cast<VInfo3D::Data*>(mVI.Buffer());
			float radians_interval = 2.0f * fPI / nb_triangles;

			auto proj = [&](v2f pos2D)
			{
				return pos2D.x * left + pos2D.y * up;
			};

			auto center = proj({ 0,0 });

			for (u32 i = 0; i < nb_triangles; ++i)
			{
				buf[3 * i].setValue(V3F_EXP(center), 255, 255, 255, 255, 0.5f, 0.5f);
				v2f uv1, uv2;

				uv1.x = cosf(i * radians_interval);
				uv1.y = sinf(i * radians_interval);
				uv2.x = cosf((i + 1) * radians_interval);
				uv2.y = sinf((i + 1) * radians_interval);



				auto pos1 = proj(uv1);
				auto pos2 = proj(uv2);

				uv1 = (uv1 + v2f(1, 1)) * 0.5f;
				uv2 = (uv2 + v2f(1, 1)) * 0.5f;


				buf[3 * i + 1].setValue(V3F_EXP(pos1), 255, 255, 255, 255, uv1.x, uv1.y);
				buf[3 * i + 2].setValue(V3F_EXP(pos2), 255, 255, 255, 255, uv2.x, uv2.y);

				if (mTwoSided)
				{
					auto j = (i + nb_triangles);
					buf[3 * j].setValue(V3F_EXP(center), 255, 255, 255, 255, 0.5f, 0.5f);
					buf[3 * j + 1].setValue(V3F_EXP(pos2), 255, 255, 255, 255, uv2.x, uv2.y);
					buf[3 * j + 2].setValue(V3F_EXP(pos1), 255, 255, 255, 255, uv1.x, uv1.y);
				}
			}
		}

		mVI.SetFlag(UIVerticesInfo_Vertex | UIVerticesInfo_Texture);

		const Locations* locs = renderer->GetActiveShader()->GetLocation();

		unsigned int bufferName = renderer->getVBO();
		renderer->BufferData(bufferName, KIGS_BUFFER_TARGET_ARRAY, mVI.Offset * mVI.vertexCount, mVI.Buffer(), KIGS_BUFFER_USAGE_DYNAMIC);
		renderer->SetVertexAttrib(bufferName, KIGS_VERTEX_ATTRIB_VERTEX_ID, mVI.vertexComp, KIGS_FLOAT, false, mVI.Offset, (void*)mVI.vertexStride, locs);
		renderer->SetVertexAttrib(bufferName, KIGS_VERTEX_ATTRIB_COLOR_ID, mVI.colorComp, KIGS_UNSIGNED_BYTE, false, mVI.Offset, (void*)mVI.colorStride, locs);
		renderer->SetVertexAttrib(bufferName, KIGS_VERTEX_ATTRIB_TEXCOORD_ID, mVI.texComp, KIGS_FLOAT, false, mVI.Offset, (void*)mVI.texStride, locs);

		renderer->DrawArrays(travstate, KIGS_DRAW_MODE_TRIANGLES, 0, mVI.vertexCount);

		/*	unsigned short indice[6] = { 0,1,2,0,2,3 };
			bufferName = renderer->getVBO();
			renderer->BufferData(bufferName, KIGS_BUFFER_TARGET_ELEMENT, 12, indice, KIGS_BUFFER_USAGE_STATIC);

			// draw call
			renderer->DrawElements(travstate, KIGS_DRAW_MODE_TRIANGLES, 6, KIGS_UNSIGNED_SHORT, 0);*/

			// deactive texture 
		if (mTexture)
			mTexture->DoPostDraw(travstate);
		else if (mTextureID > -1)
		{
			renderer->BindTexture(RENDERER_TEXTURE_2D, 0);
		}

		renderer->PopState();
		return true;
	}
	return false;
}
