#include "PrecompiledHeaders.h"

#include "DrawVertice.h"
#include "TextureFileManager.h"
#include "Texture.h"
#include "RendererDefines.h"
#include "TecLibs/3D/3DObject/BBox.h"

#include "ModuleRenderer.h"
#include "ModuleSceneGraph.h"
#include "TravState.h"

IMPLEMENT_CLASS_INFO(DrawVertice);

IMPLEMENT_CONSTRUCTOR(DrawVertice)
{
	mSortable = true;
	mRenderPassMask = 1;
}

void DrawVertice::RecalculateBBox() const
{
	BBox bbox{ BBox::PreInit{} };
	if (mVertices)
	{
		// we want to jump to next vertex pos, so add all element to vSize if interleaved buffer
		int vSize = mVertexSize;
		if (mColorSize && (nullptr == mColors))
		{
			vSize += mColorSize;
		}

		if (mNormalSize && (nullptr == mNormals))
		{
			vSize += mNormalSize;
		}

		if (mTexCoordSize && (nullptr == mTexCoords))
		{
			vSize += mTexCoordSize;
		}

		u8* read = reinterpret_cast<u8*>(mVertices);
		read += mVertexDecal;
		for (int i = 0; i < mVertexCount; i++)
		{
			v3f* v = reinterpret_cast<v3f*>(read);
			bbox.Update(*v);
			read += vSize;
		}
	}
	mBBox = bbox;
	mNeedRecomputeBBox = false;
}

void DrawVertice::GetNodeBoundingBox(Point3D& pmin, Point3D& pmax) const
{
	if (mNeedRecomputeBBox)
	{
		RecalculateBBox();
	}
	pmin = mBBox.m_Min;
	pmax = mBBox.m_Max;
}

void DrawVertice::clearArrays()
{
	if (mVertices)
	{
		delete[](char*)mVertices;
		
	}
	if (mIndices)
	{
		delete[](char*)mIndices;
		
	}
	if (mNormals)
	{
		delete[](char*)mNormals;
	
	}
	if (mTexCoords)
	{
		delete[](char*)mTexCoords;
		
	
	}
	if (mColors)
	{
		delete[](char*)mColors;

	}

	mVertices = 0;
	mVertexCount = 0;
	mVertexSize = 0;
	mVertexDecal = 0;
	mIndices = 0;
	mIndiceCount = 0;
	mIndiceSize = 0;
	mNormals = 0;
	mNormalCount = 0;
	mNormalSize = 0;
	mNormalDecal = 0;
	mTexCoords = 0;
	mTexCoordCount = 0;
	mTexCoordSize = 0;
	mTexCoordDecal = 0;
	mColors = 0;
	mColorCount = 0;
	mColorSize = 0;
	mColorDecal = 0;
}

DrawVertice::~DrawVertice()
{
	clearArrays();
}

void DrawVertice::InitModifiable()
{
	mPreset.changeNotificationLevel(Owner);
	mPresetColor.changeNotificationLevel(Owner);
	mTextureFileName.changeNotificationLevel(Owner);
	mPresetOffset.changeNotificationLevel(Owner);
	mPresetSize.changeNotificationLevel(Owner);
	mPresetUVSize.changeNotificationLevel(Owner);

	clearArrays();
	Drawable::InitModifiable();
	if (IsInit())
	{
		if (mTextureFileName.const_ref().size())
		{
			auto textureManager = KigsCore::Singleton<TextureFileManager>();
			mTexture = textureManager->GetTexture(mTextureFileName);
		}
		// set data after texture so that can get correct uv ratio
		SetDataFromPreset();
	}
}

void DrawVertice::NotifyUpdate(const u32 labelid)
{
	ParentClassType::NotifyUpdate(labelid);
	if (labelid == mTextureFileName.getID())
	{
		auto old_tex = mTexture;
		mTexture = nullptr;
		if (mTextureFileName.const_ref().size())
		{
			auto textureManager = KigsCore::Singleton<TextureFileManager>();
			mTexture = textureManager->GetTexture(mTextureFileName);
			SetDataFromPreset();
		}
	}
	else if (labelid == mPreset.getID()
		|| labelid == mPresetColor.getID()
		|| labelid == mPresetOffset.getID()
		|| labelid == mPresetSize.getID()
		|| labelid == mPresetUVSize.getID()
		|| labelid == mPresetUVOffset.getID())
	{
		SetDataFromPreset();
	}
}

void DrawVertice::SetDataFromPreset()
{
	clearArrays();
	std::string type = mPreset;
	if (type != "None") mIsStaticBuffer = true;

	struct vertex
	{
		v3f v;
		std::array<u8, 4> c;
	};

	struct textured_vertex
	{
		v3f v;
		std::array<u8, 4> c;
		v2f uv;
	};
	std::array<u8, 4> color = { u8(mPresetColor[0] * 255.0f), u8(mPresetColor[1] * 255.0f),u8(mPresetColor[2] * 255.0f),u8(mPresetColor[3] * 255.0f) };

	v3f offset = mPresetOffset;
	v3f preset_size = mPresetSize;
	v2f uv_offset = mPresetUVOffset;
	v2f uv_size = mPresetUVSize;

	if (type == "TexturedQuad" || type == "Quad")
	{
		bool is_textured = type == "TexturedQuad";
		size_t buffer_stride = (is_textured ? sizeof(textured_vertex) : sizeof(vertex));
		size_t buffer_size = buffer_stride*6;
		char* buffer = new char[buffer_size];
		memset(buffer, 0, buffer_size);
		
		
		reinterpret_cast<vertex*>(buffer + 0 * buffer_stride)->v = v3f(-0.5f, -0.5f, 0.0f) * preset_size + offset;
		reinterpret_cast<vertex*>(buffer + 1 * buffer_stride)->v = v3f(0.5f, -0.5f, 0.0f) * preset_size + offset;
		reinterpret_cast<vertex*>(buffer + 2 * buffer_stride)->v = v3f(0.5f, 0.5f, 0.0f) * preset_size + offset;
		reinterpret_cast<vertex*>(buffer + 3 * buffer_stride)->v = v3f(-0.5f, -0.5f, 0.0f) * preset_size + offset;
		reinterpret_cast<vertex*>(buffer + 4 * buffer_stride)->v = v3f(0.5f, 0.5f, 0.0f) * preset_size + offset;
		reinterpret_cast<vertex*>(buffer + 5 * buffer_stride)->v = v3f(-0.5f, 0.5f, 0.0f) * preset_size + offset;
	
		reinterpret_cast<vertex*>(buffer + 0 * buffer_stride)->c = color;
		reinterpret_cast<vertex*>(buffer + 1 * buffer_stride)->c = color;
		reinterpret_cast<vertex*>(buffer + 2 * buffer_stride)->c = color;
		reinterpret_cast<vertex*>(buffer + 3 * buffer_stride)->c = color;
		reinterpret_cast<vertex*>(buffer + 4 * buffer_stride)->c = color;
		reinterpret_cast<vertex*>(buffer + 5 * buffer_stride)->c = color;

		if (is_textured)
		{
			float rx, ry;
			rx = ry = 1.0f;

			// if texture is valid, get ratio
			if (mTexture)
			{
				mTexture->GetRatio(rx, ry);
			}
			reinterpret_cast<textured_vertex*>(buffer + 0 * buffer_stride)->uv = v2f(0, ry);
			reinterpret_cast<textured_vertex*>(buffer + 1 * buffer_stride)->uv = v2f(rx, ry);
			reinterpret_cast<textured_vertex*>(buffer + 2 * buffer_stride)->uv = v2f(rx, 0);
			reinterpret_cast<textured_vertex*>(buffer + 3 * buffer_stride)->uv = v2f(0, ry);
			reinterpret_cast<textured_vertex*>(buffer + 4 * buffer_stride)->uv = v2f(rx, 0);
			reinterpret_cast<textured_vertex*>(buffer + 5 * buffer_stride)->uv = v2f(0, 0);

			for (int i = 0; i < 6; ++i)
			{
				auto& uv = reinterpret_cast<textured_vertex*>(buffer + i * buffer_stride)->uv;
				uv = (uv * uv_size) + uv_offset;
			}
				
		}
		SetVertexArray(buffer, 6, 12, 0);
		SetColorArray(nullptr, 6, 4, 12);

		if (is_textured)
			SetTexCoordArray(nullptr, 6, 8, 16);
	}
	else if (type == "Box" || type == "TexturedBox")
	{
		bool is_textured = type == "TexturedBox";
		size_t buffer_stride = (is_textured ? sizeof(textured_vertex) : sizeof(vertex));
		size_t buffer_size = buffer_stride * 6 * 6;
		char* buffer = new char[buffer_size];
		memset(buffer, 0, buffer_size);

		v3f offset = mPresetOffset;
		v3f preset_size = mPresetSize;

		mat3 m;

		for (auto face = 0; face < 6; ++face)
		{
			if (face < 4)
				m.SetRotationX(face * fPI/2);
			else
				m.SetRotationY(face == 4 ? fPI / 2 : -fPI / 2);
			
			reinterpret_cast<vertex*>(buffer + (0 + face * 6) * buffer_stride)->v = (m * v3f(-0.5f, -0.5f, 0.5f) * preset_size) + offset;
			reinterpret_cast<vertex*>(buffer + (1 + face * 6) * buffer_stride)->v = (m * v3f(0.5f, -0.5f, 0.5f) * preset_size) + offset;
			reinterpret_cast<vertex*>(buffer + (2 + face * 6) * buffer_stride)->v = (m * v3f(0.5f, 0.5f, 0.5f) * preset_size) + offset;
			reinterpret_cast<vertex*>(buffer + (3 + face * 6) * buffer_stride)->v = (m * v3f(-0.5f, -0.5f, 0.5f) * preset_size) + offset;
			reinterpret_cast<vertex*>(buffer + (4 + face * 6) * buffer_stride)->v = (m * v3f(0.5f, 0.5f, 0.5f) * preset_size) + offset;
			reinterpret_cast<vertex*>(buffer + (5 + face * 6) * buffer_stride)->v = (m * v3f(-0.5f, 0.5f, 0.5f) * preset_size) + offset;

			reinterpret_cast<vertex*>(buffer + (0 + face * 6) * buffer_stride)->c = color;
			reinterpret_cast<vertex*>(buffer + (1 + face * 6) * buffer_stride)->c = color;
			reinterpret_cast<vertex*>(buffer + (2 + face * 6) * buffer_stride)->c = color;
			reinterpret_cast<vertex*>(buffer + (3 + face * 6) * buffer_stride)->c = color;
			reinterpret_cast<vertex*>(buffer + (4 + face * 6) * buffer_stride)->c = color;
			reinterpret_cast<vertex*>(buffer + (5 + face * 6) * buffer_stride)->c = color;

			if (is_textured)
			{
				reinterpret_cast<textured_vertex*>(buffer + (0 + face * 6) * buffer_stride)->uv = v2f(0, 1);
				reinterpret_cast<textured_vertex*>(buffer + (1 + face * 6) * buffer_stride)->uv = v2f(1, 1);
				reinterpret_cast<textured_vertex*>(buffer + (2 + face * 6) * buffer_stride)->uv = v2f(1, 0);
				reinterpret_cast<textured_vertex*>(buffer + (3 + face * 6) * buffer_stride)->uv = v2f(0, 1);
				reinterpret_cast<textured_vertex*>(buffer + (4 + face * 6) * buffer_stride)->uv = v2f(1, 0);
				reinterpret_cast<textured_vertex*>(buffer + (5 + face * 6) * buffer_stride)->uv = v2f(0, 0);

				for (int i = 0; i < 6; ++i)
				{
					auto& uv = reinterpret_cast<textured_vertex*>(buffer + (i + face * 6) * buffer_stride)->uv;
					uv = (uv * uv_size) + uv_offset;
				}
			}

		}

		SetVertexArray(buffer, 6 * 6, 12, 0);
		SetColorArray(nullptr, 6 * 6, 4, 12);

		if (is_textured)
			SetTexCoordArray(nullptr, 6 * 6, 8, 16);
	}
}

void DrawVertice::SetVertexArray(void *aArray, int aCount, int aSize, int decal, int aFormat)
{
	if (mVertices && mVertices != aArray)
	{
		delete[](char*)mVertices;
		mVertices = nullptr;
		mVertexCount = 0;
	}

	if (aCount)
	{
		mVertices = aArray;
		mVertexCount = aCount;
		mVertexSize = aSize;
		mVertexFormat = aFormat;
		mVertexDecal = decal;
	}
	
	needReloadData = true;
	mNeedRecomputeBBox = true;
}

void DrawVertice::SetIndexArray(void *aArray, int aCount, int aSize, int aFormat)
{
	if (mIndices && mIndices != aArray)
	{
		delete[](char*)mIndices;
		mIndices = nullptr;
		mIndiceCount = 0;
	}

	if (aCount)
	{
		mIndices = aArray;
		mIndiceCount = aCount;
		mIndiceSize = aSize;
		mIndiceFormat = aFormat;
	}
}

// aArray = nullptr for interleaved array with vertex
void DrawVertice::SetNormalArray(void *aArray, int aCount, int aSize, int decal, int aFormat)
{
	if (mNormals && mNormals != aArray)
	{
		delete[](char*)mNormals;
		mNormals = nullptr;
		mNormalCount = 0;
	}

	if (aCount)
	{
		mNormals = aArray;
		mNormalCount = aCount;
		mNormalSize = aSize;
		mNormalDecal = decal;
		mNormalFormat = aFormat;
	}

	needReloadData = true;
}

// aArray = nullptr for interleaved array with vertex
void DrawVertice::SetTexCoordArray(void *aArray, int aCount, int aSize, int decal, int aFormat)
{
	if (mTexCoords && mTexCoords != aArray)
	{
		delete[](char*)mTexCoords;
		mTexCoords = nullptr;
		mTexCoordCount = 0;
	}

	if (aCount)
	{
		mTexCoords = aArray;
		mTexCoordCount = aCount;
		mTexCoordSize = aSize;
		mTexCoordDecal = decal;
		mTexCoordFormat = aFormat;
	}
	needReloadData = true;
}

// aArray = nullptr for interleaved array with vertex
void DrawVertice::SetColorArray(void *aArray, int aCount, int aSize, int decal, int aFormat)
{
	if (mColors && mColors != aArray)
	{
		delete[](char*)mColors;
		mColors = nullptr;
		mColorCount = 0;
	}

	if (aCount)
	{
		mColors = aArray;
		mColorCount = aCount;
		mColorSize = aSize;
		mColorDecal = decal;
		mColorFormat = aFormat;
	}
	needReloadData = true;
}

bool DrawVertice::Draw(TravState* travstate)
{
	if (ParentClassType::Draw(travstate))
	{
		if (mVertexCount > 0)
		{
			auto renderer = /*(RendererOpenGL*)*/travstate->GetRenderer();
			renderer->SetCullMode((RendererCullMode)(int)mCullMode);

			int lShaderMask = ModuleRenderer::VERTEX_ARRAY_MASK | ModuleRenderer::NO_LIGHT_MASK;
			if (mColorCount)lShaderMask |= ModuleRenderer::COLOR_ARRAY_MASK;
			if (mNormalCount)lShaderMask |= ModuleRenderer::NORMAL_ARRAY_MASK;
			if (mTexCoordCount)lShaderMask |= ModuleRenderer::TEXCOORD_ARRAY_MASK;

#ifdef KIGS_TOOLS
			if (mDrawNormals)
				lShaderMask |= ModuleRenderer::DRAW_NORMALS;
			if (mDrawUVs)
				lShaderMask |= ModuleRenderer::DRAW_UVS;

			renderer->SetPolygonMode(mWireMode ? RENDERER_LINE : RENDERER_FILL);
#endif


			bool blend = false;

			int transparency = mTransparencyType;
			if (transparency == -1)
			{
				transparency = mTexture ? mTexture->GetTransparency() : 0;
			}

			switch (transparency)
			{
			case 2:
				lShaderMask |= ModuleRenderer::ALPHA_TEST_LOW;
				blend = true;
				break;
			case 1:
				lShaderMask |= ModuleRenderer::ALPHA_TEST_HIGH;
				renderer->SetBlendMode(RENDERER_BLEND_OFF);
				break;
			default:
				renderer->SetBlendMode(RENDERER_BLEND_OFF);
				break;
			}
			

			// create shader if none
			renderer->GetActiveShader()->ChooseShader(travstate, lShaderMask);

			if (mTexture)
			{
				renderer->ActiveTextureChannel(0);
				mTexture->DoPreDraw(travstate);
			}

			if (blend)
			{
				renderer->SetBlendMode(RENDERER_BLEND_ON);
				renderer->SetBlendFuncMode(RENDERER_BLEND_SRC_ALPHA, RENDERER_BLEND_ONE_MINUS_SRC_ALPHA);
			}

			const Locations* locs = renderer->GetActiveShader()->GetLocation();

			if (mIsStaticBuffer)
			{
				int vSize = mVertexSize;
				if (mColorSize && (nullptr == mColors))
				{
					vSize += mColorSize;
				}

				if (mNormalSize && (nullptr == mNormals))
				{
					vSize += mNormalSize;
				}

				if (mTexCoordSize && (nullptr == mTexCoords))
				{
					vSize += mTexCoordSize;
				}

				// try to init VBO
				if (mVertexBuffer == -1 || needReloadData)
				{
					if (mVertexBuffer == -1)
						renderer->CreateBuffer(1, &mVertexBuffer);
					renderer->BufferData(mVertexBuffer, KIGS_BUFFER_TARGET_ARRAY, vSize * mVertexCount, mVertices, KIGS_BUFFER_USAGE_DYNAMIC);
				}

				renderer->SetVertexAttrib(mVertexBuffer, KIGS_VERTEX_ATTRIB_VERTEX_ID, 3, mVertexFormat, false, vSize, (void*)mVertexDecal, locs);
				if (mColorCount > 0)
					renderer->SetVertexAttrib(mVertexBuffer, KIGS_VERTEX_ATTRIB_COLOR_ID, 4, mColorFormat, true, vSize, (void*)mColorDecal, locs);
				if (mNormalCount > 0)
					renderer->SetVertexAttrib(mVertexBuffer, KIGS_VERTEX_ATTRIB_NORMAL_ID, 3, mNormalFormat, false, vSize, (void*)mNormalDecal, locs);
				if (mTexCoordCount > 0)
					renderer->SetVertexAttrib(mVertexBuffer, KIGS_VERTEX_ATTRIB_TEXCOORD_ID, 2, mTexCoordFormat, false, vSize, (void*)mTexCoordDecal, locs);

				renderer->SetElementBuffer(0);
				renderer->DrawArrays(travstate, mDrawMode, 0, mVertexCount);

			}
			// with dynamic buffer use preallocated VBO (buffer may change)
			else
			{
				int VBO = renderer->getVBO();
				renderer->BufferData(VBO, KIGS_BUFFER_TARGET_ARRAY, mVertexCount * sizeof(Point3D), mVertices, KIGS_BUFFER_USAGE_DYNAMIC);
				renderer->SetVertexAttrib(VBO, KIGS_VERTEX_ATTRIB_VERTEX_ID, 3, KIGS_FLOAT, false, sizeof(Point3D), (void*)0, locs);

				if (mNormals)
				{
					VBO = renderer->getVBO();
					renderer->BufferData(VBO, KIGS_BUFFER_TARGET_ARRAY, mNormalCount * mNormalSize, mNormals, KIGS_BUFFER_USAGE_DYNAMIC);
					renderer->SetVertexAttrib(VBO, KIGS_VERTEX_ATTRIB_NORMAL_ID, 3, mNormalFormat, false, mNormalSize, (void*)0, locs);
				}

				if (mColors)
				{

					VBO = renderer->getVBO();
					renderer->BufferData(VBO, KIGS_BUFFER_TARGET_ARRAY, mColorCount * sizeof(unsigned char) * 4, mColors, KIGS_BUFFER_USAGE_DYNAMIC);
					renderer->SetVertexAttrib(VBO, KIGS_VERTEX_ATTRIB_COLOR_ID, 4, KIGS_UNSIGNED_BYTE, false, sizeof(unsigned char) * 4, (void*)0, locs);
				}

				if (mTexCoords && mTexture)
				{
					VBO = renderer->getVBO();
					renderer->BufferData(VBO, KIGS_BUFFER_TARGET_ARRAY, mTexCoordCount * sizeof(Point2D), mTexCoords, KIGS_BUFFER_USAGE_DYNAMIC);
					renderer->SetVertexAttrib(VBO, KIGS_VERTEX_ATTRIB_TEXCOORD_ID, 2, KIGS_FLOAT, false, sizeof(Point2D), (void*)0, locs);
				}

#ifdef WIN32
				VBO = renderer->getVBO();
				renderer->BufferData(VBO, KIGS_BUFFER_TARGET_ELEMENT, mIndiceCount * mIndiceSize, mIndices, KIGS_BUFFER_USAGE_DYNAMIC);
				renderer->SetElementBuffer(VBO);
				renderer->DrawElements(travstate, mDrawMode, mIndiceCount, mIndiceFormat, 0);
#else
				// TODO : no GL_UNSIGNED_INT on GL ES
				renderer->DrawElements(travstate, mDrawMode, mIndiceCount, KIGS_UNSIGNED_SHORT, mIndices);
				//glDrawElements(GL_TRIANGLES, mIndiceCount, GL_UNSIGNED_SHORT, mIndices);
#endif
			}


			if (mTexture)
				mTexture->DoPostDraw(travstate);

#ifdef KIGS_TOOLS
			renderer->SetPolygonMode(RENDERER_FILL);
#endif
			return true;
		}
	}
	return false;
}

void DrawVertice::ProtectedDestroy()
{
	ParentClassType::ProtectedDestroy();
	ModuleSceneGraph* scenegraph = static_cast<ModuleSceneGraph*>(KigsCore::Instance()->GetMainModuleInList(SceneGraphModuleCoreIndex));
	if (mVertexBuffer != -1)
	{
		scenegraph->AddDefferedItem((void*)mVertexBuffer, DefferedAction::DESTROY_BUFFER);
		//scenegraph->AddDefferedItem((void*)myIndexBuffer, DefferedAction::DESTROY_BUFFER);
		//((RendererOpenGL*)ModuleRenderer::mTheGlobalRenderer)->DeleteBuffer(1, &mVertexBuffer);
		mVertexBuffer = -1;
		// myIndexBuffer = -1;
	}
}