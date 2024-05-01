#include "OpenGLHolo3DPanel.h"
#include "RendererOpenGL.h"
#include "Texture.h"
#include "GLSLShader.h"

#include "TravState.h"

using namespace Kigs::Draw;
using namespace Kigs::Draw2D;

IMPLEMENT_CLASS_INFO(OpenGLHolo3DPanel);

///////////////////////////////////////////
bool OpenGLHolo3DPanel::Draw(TravState* travstate)
{
	if (Drawable::Draw(travstate))
	{
		// create shader if none
		int lShaderMask = ModuleRenderer::VERTEX_ARRAY_MASK | ModuleRenderer::TEXCOORD_ARRAY_MASK | ModuleRenderer::NO_LIGHT_MASK;
		RendererOpenGL* renderer = (RendererOpenGL*)travstate->GetRenderer();
		renderer->GetActiveShader()->ChooseShader(travstate, lShaderMask);

		// ** Manage Transparency **
		renderer->PushState();
		renderer->SetAlphaTestMode(RENDERER_ALPHA_TEST_ON);
		renderer->SetAlphaMode(RENDERER_ALPHA_GREATER, 0.01f);
		renderer->SetBlendMode(RENDERER_BLEND_ON);
		renderer->SetBlendFuncMode(RENDERER_BLEND_SRC_ALPHA, RENDERER_BLEND_ONE_MINUS_SRC_ALPHA);
		
		int depth_test = mDepthTest;
		switch (depth_test)
		{
		case 1:
			renderer->SetDepthTestMode(true);
			break;
		case 2:
			renderer->SetDepthTestMode(false);
			break;
		default:
			break;
		}
		
		renderer->SetCullMode(RendererCullMode::RENDERER_CULL_BACK);


		renderer->ActiveTextureChannel(0);

		// active texture 
		if (mTexture)
			mTexture->DoPreDraw(travstate);
		else if (mTextureID > -1)
		{
			renderer->BindTexture(RENDERER_TEXTURE_2D, mTextureID);
		}

		// compute position

		v3f n = (v3f)mNormal;
		v3f up = normalize((v3f)mUp);
		v3f left = normalize(cross(up,n));

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
			buf[2].setValue( left.x - up.x + offset_left.x + offset_up.x,  left.y - up.y + offset_left.y + offset_up.y,  left.z - up.z + offset_left.z + offset_up.z, 255, 255, 255, 255, uv0.x, uv0.y);

			buf[3].setValue(-left.x + up.x + offset_left.x + offset_up.x, -left.y + up.y + offset_left.y + offset_up.y, -left.z + up.z + offset_left.z + offset_up.z, 255, 255, 255, 255, uv1.x, uv1.y);
			buf[4].setValue( left.x + up.x + offset_left.x + offset_up.x,  left.y + up.y + offset_left.y + offset_up.y,  left.z + up.z + offset_left.z + offset_up.z, 255, 255, 255, 255, uv0.x, uv1.y);
			buf[5].setValue( left.x - up.x + offset_left.x + offset_up.x,  left.y - up.y + offset_left.y + offset_up.y,  left.z - up.z + offset_left.z + offset_up.z, 255, 255, 255, 255, uv0.x, uv0.y);
			if (mTwoSided)
			{
				buf[6].setValue(-left.x - up.x + offset_left.x + offset_up.x, -left.y - up.y + offset_left.y + offset_up.y, -left.z - up.z + offset_left.z + offset_up.z, 255, 255, 255, 255, uv0.x, uv0.y);
				buf[7].setValue( left.x - up.x + offset_left.x + offset_up.x,  left.y - up.y + offset_left.y + offset_up.y,  left.z - up.z + offset_left.z + offset_up.z, 255, 255, 255, 255, uv1.x, uv0.y);
				buf[8].setValue(-left.x + up.x + offset_left.x + offset_up.x, -left.y + up.y + offset_left.y + offset_up.y, -left.z + up.z + offset_left.z + offset_up.z, 255, 255, 255, 255, uv0.x, uv1.y);

				buf[9].setValue( -left.x + up.x + offset_left.x + offset_up.x, -left.y + up.y + offset_left.y + offset_up.y, -left.z + up.z + offset_left.z + offset_up.z, 255, 255, 255, 255, uv0.x, uv1.y);
				buf[10].setValue( left.x - up.x + offset_left.x + offset_up.x,  left.y - up.y + offset_left.y + offset_up.y,  left.z - up.z + offset_left.z + offset_up.z, 255, 255, 255, 255, uv1.x, uv0.y);
				buf[11].setValue( left.x + up.x + offset_left.x + offset_up.x,  left.y + up.y + offset_left.y + offset_up.y,  left.z + up.z + offset_left.z + offset_up.z, 255, 255, 255, 255, uv1.x, uv1.y);
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
			float radians_interval = 2.0f * glm::pi<float>() / nb_triangles;
			
			auto proj = [&](v2f pos2D)
			{
				return pos2D.x * left + pos2D.y*up;
			};

			auto center = proj({ 0,0 });
			
			for (u32 i = 0; i < nb_triangles; ++i)
			{
				buf[3 * i].setValue(V3F_EXP(center), 255, 255, 255, 255, 0.5f, 0.5f);
				v2f uv1, uv2;

				uv1.x = cosf(i*radians_interval);
				uv1.y = sinf(i*radians_interval);
				uv2.x = cosf((i + 1)*radians_interval);
				uv2.y = sinf((i + 1)*radians_interval);

				

				auto pos1 = proj(uv1);
				auto pos2 = proj(uv2);

				uv1 = (uv1 + v2f(1, 1))*0.5f;
				uv2 = (uv2 + v2f(1, 1))*0.5f;
				

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

		const Locations * locs = renderer->GetActiveShader()->GetLocation();

		unsigned int bufferName = renderer->getVBO();
		renderer->BufferData(bufferName, KIGS_BUFFER_TARGET_ARRAY, mVI.Offset* mVI.vertexCount, mVI.Buffer(), KIGS_BUFFER_USAGE_STATIC);
		renderer->SetVertexAttrib(bufferName, KIGS_VERTEX_ATTRIB_VERTEX_ID, mVI.vertexComp, KIGS_FLOAT, false, mVI.Offset, (void*)(uintptr_t)mVI.vertexStride,locs);
		renderer->SetVertexAttrib(bufferName, KIGS_VERTEX_ATTRIB_COLOR_ID, mVI.colorComp, KIGS_UNSIGNED_BYTE, false, mVI.Offset, (void*)(uintptr_t)mVI.colorStride, locs);
		renderer->SetVertexAttrib(bufferName, KIGS_VERTEX_ATTRIB_TEXCOORD_ID, mVI.texComp, KIGS_FLOAT, false, mVI.Offset, (void*)(uintptr_t)mVI.texStride, locs);

		renderer->DrawArrays(travstate, KIGS_DRAW_MODE_TRIANGLES, 0, mVI.vertexCount);

	/*	unsigned short indice[6] = { 0,1,2,0,2,3 };
		bufferName = renderer->getVBO();
		renderer->BufferData(bufferName, KIGS_BUFFER_TARGET_ELEMENT, 12, indice, KIGS_BUFFER_USAGE_STATIC);

		// draw call
		renderer->DrawElements(travstate, KIGS_DRAW_MODE_TRIANGLES, 6, KIGS_UNSIGNED_SHORT, 0);*/

		// deactive texture 
		if (mTexture)
			mTexture->DoPostDraw(travstate);
		else if (mTextureID>-1)
		{
			renderer->BindTexture(RENDERER_TEXTURE_2D, 0);
		}

		renderer->PopState();
		return true;
	}
	return false;
}
