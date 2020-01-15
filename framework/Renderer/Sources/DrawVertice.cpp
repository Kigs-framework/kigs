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
	if (myVertex)
	{
		// we want to jump to next vertex pos, so add all element to vSize if interleaved buffer
		int vSize = myVertexSize;
		if (myColorSize && (nullptr == myColor))
		{
			vSize += myColorSize;
		}

		if (myNormalSize && (nullptr == myNormals))
		{
			vSize += myNormalSize;
		}

		if (myTexCoordSize && (nullptr == myTexCoords))
		{
			vSize += myTexCoordSize;
		}

		u8* read = reinterpret_cast<u8*>(myVertex);
		read += myVertexDecal;
		for (int i = 0; i < myVertexCount; i++)
		{
			v3f* v = reinterpret_cast<v3f*>(read);
			bbox.Update(*v);
			read += vSize;
		}
	}
	myBBox = bbox;
	needRecomputeBBox = false;
}

void DrawVertice::GetNodeBoundingBox(Point3D& pmin, Point3D& pmax) const
{
	if (needRecomputeBBox)
	{
		RecalculateBBox();
	}
	pmin = myBBox.m_Min;
	pmax = myBBox.m_Max;
}

void DrawVertice::clearArrays()
{
	if (myVertex)
	{
		delete[](char*)myVertex;
		
	}
	if (myIndex)
	{
		delete[](char*)myIndex;
		
	}
	if (myNormals)
	{
		delete[](char*)myNormals;
	
	}
	if (myTexCoords)
	{
		delete[](char*)myTexCoords;
		
	
	}
	if (myColor)
	{
		delete[](char*)myColor;

	}

	myVertex = 0;
	myVertexCount = 0;
	myVertexSize = 0;
	myVertexDecal = 0;
	myIndex = 0;
	myIndiceCount = 0;
	myIndiceSize = 0;
	myNormals = 0;
	myNormalCount = 0;
	myNormalSize = 0;
	myNormalDecal = 0;
	myTexCoords = 0;
	myTexCoordCount = 0;
	myTexCoordSize = 0;
	myTexCoordDecal = 0;
	myColor = 0;
	myColorCount = 0;
	myColorSize = 0;
	myColorDecal = 0;
}

DrawVertice::~DrawVertice()
{
	clearArrays();
}

void DrawVertice::InitModifiable()
{
	myPreset.changeNotificationLevel(Owner);
	myPresetColor.changeNotificationLevel(Owner);
	myTextureFileName.changeNotificationLevel(Owner);
	myPresetOffset.changeNotificationLevel(Owner);
	myPresetUVSize.changeNotificationLevel(Owner);

	clearArrays();
	Drawable::InitModifiable();
	if (IsInit())
	{
		SetDataFromPreset();
		if (myTextureFileName.const_ref().size())
		{
			SP<TextureFileManager> textureManager = KigsCore::GetSingleton("TextureFileManager");
			mypTexture = textureManager->GetTexture(myTextureFileName);
		}
	}
}

void DrawVertice::NotifyUpdate(const u32 labelid)
{
	ParentClassType::NotifyUpdate(labelid);
	if (labelid == myTextureFileName.getID())
	{
		auto old_tex = mypTexture;
		mypTexture = nullptr;
		if (myTextureFileName.const_ref().size())
		{
			SP<TextureFileManager> textureManager = KigsCore::GetSingleton("TextureFileManager");
			mypTexture = textureManager->GetTexture(myTextureFileName);
		}
	}
	else if (labelid == myPreset.getID()
		|| labelid == myPresetColor.getID()
		|| labelid == myPresetOffset.getID()
		|| labelid == myPresetSize.getID()
		|| labelid == myPresetUVSize.getID()
		|| labelid == myPresetUVOffset.getID())
	{
		SetDataFromPreset();
	}
}

void DrawVertice::SetDataFromPreset()
{
	clearArrays();
	std::string type = myPreset;
	if (type != "None") myIsStaticBuffer = true;

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
	std::array<u8, 4> color = { u8(myPresetColor[0] * 255.0f), u8(myPresetColor[1] * 255.0f),u8(myPresetColor[2] * 255.0f),u8(myPresetColor[3] * 255.0f) };

	v3f offset = myPresetOffset;
	v3f preset_size = myPresetSize;
	v2f uv_offset = myPresetUVOffset;
	v2f uv_size = myPresetUVSize;

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
			reinterpret_cast<textured_vertex*>(buffer + 0 * buffer_stride)->uv = v2f(0, 1);
			reinterpret_cast<textured_vertex*>(buffer + 1 * buffer_stride)->uv = v2f(1, 1);
			reinterpret_cast<textured_vertex*>(buffer + 2 * buffer_stride)->uv = v2f(1, 0);
			reinterpret_cast<textured_vertex*>(buffer + 3 * buffer_stride)->uv = v2f(0, 1);
			reinterpret_cast<textured_vertex*>(buffer + 4 * buffer_stride)->uv = v2f(1, 0);
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

		v3f offset = myPresetOffset;
		v3f preset_size = myPresetSize;

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
	if (myVertex && myVertex != aArray)
	{
		delete[](char*)myVertex;
		myVertex = nullptr;
		myVertexCount = 0;
	}

	if (aCount)
	{
		myVertex = aArray;
		myVertexCount = aCount;
		myVertexSize = aSize;
		myVertexFormat = aFormat;
		myVertexDecal = decal;
	}
	
	needReloadData = true;
	needRecomputeBBox = true;
}

void DrawVertice::SetIndexArray(void *aArray, int aCount, int aSize, int aFormat)
{
	if (myIndex && myIndex != aArray)
	{
		delete[](char*)myIndex;
		myIndex = nullptr;
		myIndiceCount = 0;
	}

	if (aCount)
	{
		myIndex = aArray;
		myIndiceCount = aCount;
		myIndiceSize = aSize;
		myIndiceFormat = aFormat;
	}
}

// aArray = nullptr for interleaved array with vertex
void DrawVertice::SetNormalArray(void *aArray, int aCount, int aSize, int decal, int aFormat)
{
	if (myNormals && myNormals != aArray)
	{
		delete[](char*)myNormals;
		myNormals = nullptr;
		myNormalCount = 0;
	}

	if (aCount)
	{
		myNormals = aArray;
		myNormalCount = aCount;
		myNormalSize = aSize;
		myNormalDecal = decal;
		myNormalFormat = aFormat;
	}

	needReloadData = true;
}

// aArray = nullptr for interleaved array with vertex
void DrawVertice::SetTexCoordArray(void *aArray, int aCount, int aSize, int decal, int aFormat)
{
	if (myTexCoords && myTexCoords != aArray)
	{
		delete[](char*)myTexCoords;
		myTexCoords = nullptr;
		myTexCoordCount = 0;
	}

	if (aCount)
	{
		myTexCoords = aArray;
		myTexCoordCount = aCount;
		myTexCoordSize = aSize;
		myTexCoordDecal = decal;
		myTexCoordFormat = aFormat;
	}
	needReloadData = true;
}

// aArray = nullptr for interleaved array with vertex
void DrawVertice::SetColorArray(void *aArray, int aCount, int aSize, int decal, int aFormat)
{
	if (myColor && myColor != aArray)
	{
		delete[](char*)myColor;
		myColor = nullptr;
		myColorCount = 0;
	}

	if (aCount)
	{
		myColor = aArray;
		myColorCount = aCount;
		myColorSize = aSize;
		myColorDecal = decal;
		myColorFormat = aFormat;
	}
	needReloadData = true;
}

bool DrawVertice::Draw(TravState* travstate)
{
	if (ParentClassType::Draw(travstate))
	{
		if (myVertexCount > 0)
		{
			auto renderer = /*(RendererOpenGL*)*/travstate->GetRenderer();
			renderer->SetCullMode((RendererCullMode)(int)myCullMode);

			int lShaderMask = ModuleRenderer::VERTEX_ARRAY_MASK | ModuleRenderer::NO_LIGHT_MASK;
			if (myColorCount)lShaderMask |= ModuleRenderer::COLOR_ARRAY_MASK;
			if (myNormalCount)lShaderMask |= ModuleRenderer::NORMAL_ARRAY_MASK;
			if (myTexCoordCount)lShaderMask |= ModuleRenderer::TEXCOORD_ARRAY_MASK;

#ifdef KIGS_TOOLS
			if (myDrawNormals)
				lShaderMask |= ModuleRenderer::DRAW_NORMALS;
			if (myDrawUVs)
				lShaderMask |= ModuleRenderer::DRAW_UVS;

			renderer->SetPolygonMode(myWireMode ? RENDERER_LINE : RENDERER_FILL);
#endif


			bool blend = false;
			if (mypTexture)
			{
				switch (mypTexture->GetTransparency())
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
			}

			// create shader if none
			renderer->GetActiveShader()->ChooseShader(travstate, lShaderMask);

			if (mypTexture)
			{
				renderer->ActiveTextureChannel(0);
				mypTexture->DoPreDraw(travstate);
			}

			if (blend)
			{
				renderer->SetBlendMode(RENDERER_BLEND_ON);
				renderer->SetBlendFuncMode(RENDERER_BLEND_SRC_ALPHA, RENDERER_BLEND_ONE_MINUS_SRC_ALPHA);
			}

			const Locations* locs = renderer->GetActiveShader()->GetLocation();

			if (myIsStaticBuffer)
			{
				int vSize = myVertexSize;
				if (myColorSize && (nullptr == myColor))
				{
					vSize += myColorSize;
				}

				if (myNormalSize && (nullptr == myNormals))
				{
					vSize += myNormalSize;
				}

				if (myTexCoordSize && (nullptr == myTexCoords))
				{
					vSize += myTexCoordSize;
				}

				// try to init VBO
				if (myVertexBuffer == -1 || needReloadData)
				{
					if (myVertexBuffer == -1)
						renderer->CreateBuffer(1, &myVertexBuffer);
					renderer->BufferData(myVertexBuffer, KIGS_BUFFER_TARGET_ARRAY, vSize * myVertexCount, myVertex, KIGS_BUFFER_USAGE_DYNAMIC);
				}

				renderer->SetVertexAttrib(myVertexBuffer, KIGS_VERTEX_ATTRIB_VERTEX_ID, 3, myVertexFormat, false, vSize, (void*)myVertexDecal, locs);
				if (myColorCount > 0)
					renderer->SetVertexAttrib(myVertexBuffer, KIGS_VERTEX_ATTRIB_COLOR_ID, 4, myColorFormat, true, vSize, (void*)myColorDecal, locs);
				if (myNormalCount > 0)
					renderer->SetVertexAttrib(myVertexBuffer, KIGS_VERTEX_ATTRIB_NORMAL_ID, 3, myNormalFormat, false, vSize, (void*)myNormalDecal, locs);
				if (myTexCoordCount > 0)
					renderer->SetVertexAttrib(myVertexBuffer, KIGS_VERTEX_ATTRIB_TEXCOORD_ID, 2, myTexCoordFormat, false, vSize, (void*)myTexCoordDecal, locs);

				renderer->SetElementBuffer(0);
				renderer->DrawArrays(travstate, myDrawMode, 0, myVertexCount);

			}
			// with dynamic buffer use preallocated VBO (buffer may change)
			else
			{
				int VBO = renderer->getVBO();
				renderer->BufferData(VBO, KIGS_BUFFER_TARGET_ARRAY, myVertexCount * sizeof(Point3D), myVertex, KIGS_BUFFER_USAGE_DYNAMIC);
				renderer->SetVertexAttrib(VBO, KIGS_VERTEX_ATTRIB_VERTEX_ID, 3, KIGS_FLOAT, false, sizeof(Point3D), (void*)0, locs);

				if (myNormals)
				{
					VBO = renderer->getVBO();
					renderer->BufferData(VBO, KIGS_BUFFER_TARGET_ARRAY, myNormalCount * myNormalSize, myNormals, KIGS_BUFFER_USAGE_DYNAMIC);
					renderer->SetVertexAttrib(VBO, KIGS_VERTEX_ATTRIB_NORMAL_ID, 3, myNormalFormat, false, myNormalSize, (void*)0, locs);
				}

				if (myColor)
				{

					VBO = renderer->getVBO();
					renderer->BufferData(VBO, KIGS_BUFFER_TARGET_ARRAY, myColorCount * sizeof(unsigned char) * 4, myColor, KIGS_BUFFER_USAGE_DYNAMIC);
					renderer->SetVertexAttrib(VBO, KIGS_VERTEX_ATTRIB_COLOR_ID, 4, KIGS_UNSIGNED_BYTE, false, sizeof(unsigned char) * 4, (void*)0, locs);
				}

				if (myTexCoords && mypTexture)
				{
					VBO = renderer->getVBO();
					renderer->BufferData(VBO, KIGS_BUFFER_TARGET_ARRAY, myTexCoordCount * sizeof(Point2D), myTexCoords, KIGS_BUFFER_USAGE_DYNAMIC);
					renderer->SetVertexAttrib(VBO, KIGS_VERTEX_ATTRIB_TEXCOORD_ID, 2, KIGS_FLOAT, false, sizeof(Point2D), (void*)0, locs);
				}

#ifdef WIN32
				VBO = renderer->getVBO();
				renderer->BufferData(VBO, KIGS_BUFFER_TARGET_ELEMENT, myIndiceCount * myIndiceSize, myIndex, KIGS_BUFFER_USAGE_DYNAMIC);
				renderer->SetElementBuffer(VBO);
				renderer->DrawElements(travstate, myDrawMode, myIndiceCount, myIndiceFormat, 0);
#else
				// TODO : no GL_UNSIGNED_INT on GL ES
				renderer->DrawElements(travstate, myDrawMode, myIndiceCount, KIGS_UNSIGNED_SHORT, myIndex);
				//glDrawElements(GL_TRIANGLES, myIndiceCount, GL_UNSIGNED_SHORT, myIndex);
#endif
			}


			if (mypTexture)
				mypTexture->DoPostDraw(travstate);

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
	if (myVertexBuffer != -1)
	{
		scenegraph->AddDefferedItem((void*)myVertexBuffer, DefferedAction::DESTROY_BUFFER);
		//scenegraph->AddDefferedItem((void*)myIndexBuffer, DefferedAction::DESTROY_BUFFER);
		//((RendererOpenGL*)ModuleRenderer::theGlobalRenderer)->DeleteBuffer(1, &myVertexBuffer);
		myVertexBuffer = -1;
		// myIndexBuffer = -1;
	}
}