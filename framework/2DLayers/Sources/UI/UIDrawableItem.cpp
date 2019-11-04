// #### 2DLayers Include 
#include "UI/UIDrawableItem.h"
// #### Renderer Include 
#include "UIVerticesInfo.h"
// #### Core Include
#include "Core.h"

#include "ModuleRenderer.h"

class TravState;

//IMPLEMENT_AND_REGISTER_CLASS_INFO(UIDrawableItem, UIDrawableItem, 2DLayers);
IMPLEMENT_CLASS_INFO(UIDrawableItem)

//#//////////////////////////////
//#		UIDrawableItem
//#//////////////////////////////
IMPLEMENT_CONSTRUCTOR(UIDrawableItem)
{
	// default UIDrawableItem use color Array
	setUserFlag(UserFlagUseColorArray);

	VInfo2D vi;
	mVI = UIVerticesInfo{ &vi };
}

void UIDrawableItem::SetVertexArray(UIVerticesInfo * aQI)
{
	// TODO NONO update only when changed
	aQI->Flag |= UIVerticesInfo_Vertex;

	auto slice_size = (v2f)mSliced;
	if (slice_size == v2f(0, 0))
	{
		aQI->Resize(4);
		VInfo2D::Data* buf = reinterpret_cast<VInfo2D::Data*>(aQI->Buffer());

		Point2D pt[4];
		GetTransformedPoints(pt);
		// triangle strip order
		buf[0].setVertex(pt[0].x, pt[0].y);
		buf[1].setVertex(pt[1].x, pt[1].y);
		buf[3].setVertex(pt[2].x, pt[2].y);
		buf[2].setVertex(pt[3].x, pt[3].y);
		return;
	}
	
	aQI->Resize(6*9); // 6 vertices per quad, 9 quads
	VInfo2D::Data* buf = reinterpret_cast<VInfo2D::Data*>(aQI->Buffer());

	v2f positions[6 * 9];

	auto make_quad = [&](v2f*& pts, v2f start_pos, v2f size)
	{
		pts[0] = start_pos;
		pts[1] = start_pos + v2f(0, size.y);
		pts[2] = start_pos + v2f(size.x, 0);

		pts[3] = start_pos + v2f(size.x, 0);;
		pts[4] = start_pos + v2f(0, size.y);
		pts[5] = start_pos + v2f(size.x, size.y);
		pts += 6;
	};

	if (myRealSize.x < slice_size.x * 2)
		slice_size.x = myRealSize.x / 2;
	if (myRealSize.y < slice_size.y * 2)
		slice_size.y = myRealSize.y / 2;

	auto current_pos = positions;
	// Top Left
	make_quad(current_pos, v2f(0, 0), slice_size);
	// Top Right
	make_quad(current_pos, v2f(myRealSize.x - slice_size.x, 0), slice_size);
	// Bottom Left
	make_quad(current_pos, v2f(0, myRealSize.y - slice_size.y), slice_size);
	// Bottom Right
	make_quad(current_pos, v2f(myRealSize.x - slice_size.x, myRealSize.y - slice_size.y), slice_size);
	// Center
	make_quad(current_pos, v2f(slice_size.x, slice_size.y), myRealSize - slice_size*2);
	// Top
	make_quad(current_pos, v2f(slice_size.x, 0), v2f((myRealSize - slice_size * 2).x, slice_size.y));
	// Bottom
	make_quad(current_pos, v2f(slice_size.x, myRealSize.y - slice_size.y), v2f((myRealSize - slice_size * 2).x, slice_size.y));
	// Left
	make_quad(current_pos, v2f(0, slice_size.y), v2f(slice_size.x, (myRealSize - slice_size * 2).y));
	// Right
	make_quad(current_pos, v2f(myRealSize.x - slice_size.x, slice_size.y), v2f(slice_size.x, (myRealSize - slice_size * 2).y));

	TransformPoints(positions, 6 * 9);
	for (int i = 0; i < 6 * 9; ++i)
		buf[i].setVertex(positions[i]);
}

void UIDrawableItem::SetColor(UIVerticesInfo * aQI)
{
	unsigned char r = (unsigned char)(myColor.getArrayBuffer()[0] * 255.0f);
	unsigned char g = (unsigned char)(myColor.getArrayBuffer()[1] * 255.0f);
	unsigned char b = (unsigned char)(myColor.getArrayBuffer()[2] * 255.0f);
	unsigned char a = (unsigned char)((myOpacity>=0.0f)?myOpacity*255.0f: myParent->GetOpacity()*255.f);

	VInfo2D::Data* buf = reinterpret_cast<VInfo2D::Data*>(aQI->Buffer());

	// TODO NONO update only when changed
	aQI->Flag |= UIVerticesInfo_Color;

	for (u32 i = 0; i < aQI->vertexCount; ++i)
	{
		buf[i].setColor(r, g, b, a);
	}
}

void UIDrawableItem::SetWhiteColor(UIVerticesInfo * aQI)
{
	unsigned char a = (unsigned char)((myOpacity >= 0.0f) ? myOpacity*255.0f : myParent->GetOpacity()*255.f);

	VInfo2D::Data* buf = reinterpret_cast<VInfo2D::Data*>(aQI->Buffer());

	aQI->Flag |= UIVerticesInfo_Color;
	for (u32 i = 0; i < aQI->vertexCount; ++i)
	{
		buf[i].setColor(255, 255, 255, a);
	}
}

void UIDrawableItem::ProtectedDraw(TravState* state)
{
	UIVerticesInfo* lQI = GetVerticesInfo();
	if (lQI == nullptr)
		return;

	ModuleSpecificRenderer* renderer = (ModuleSpecificRenderer*)state->GetRenderer();
	unsigned int shader_flag = ModuleRenderer::VERTEX_ARRAY_MASK | ModuleRenderer::NO_LIGHT_MASK;

	PreDraw(state); // activate texture if any
	//lQI->resetFlag();

	// ** set vertex array **
	SetVertexArray(lQI);

	if (lQI->vertexCount == 0) return;

	if (isUserFlagSet(UserFlagUseColorArray))
	{
		SetColor(lQI);
	}
	else
	{
		SetWhiteColor(lQI); // only for alpha
	}

	// ** set UV array **
	SetTexUV(lQI);


	if ((lQI->Flag & UIVerticesInfo_Color) != 0)
		shader_flag |= ModuleRenderer::COLOR_ARRAY_MASK;
	if ((lQI->Flag & UIVerticesInfo_Texture) != 0)
		shader_flag |= ModuleRenderer::TEXCOORD_ARRAY_MASK;
	if ((lQI->Flag & UIVerticesInfo_BGRTexture) != 0)
		shader_flag |= ModuleRenderer::SHADER_FLAGS_USER1;

	if ((lQI->Flag & UIVerticesInfo_UseModelMatrix) != 0)
	{
		Matrix3x3 m = GetGlobalTransform();
		Matrix4x4 m4 = (Matrix4x4)m;
		renderer->PushMatrix(MATRIX_MODE_MODEL);
		renderer->LoadMatrix(MATRIX_MODE_MODEL, &m4.e[0][0]);
	}

	// ** Manage Transparency (after texture init) **
	int lTransparencyType = GetTransparencyType();
	switch (lTransparencyType)
	{
	case 2:
		//renderer->SetAlphaTestMode(RENDERER_ALPHA_TEST_ON);
		//renderer->SetAlphaMode(RENDERER_ALPHA_GREATER, 0.01f);
		shader_flag |= ModuleRenderer::ALPHA_TEST_LOW;
		renderer->SetBlendMode(RENDERER_BLEND_ON);
		renderer->SetBlendFuncMode(RENDERER_BLEND_SRC_ALPHA, RENDERER_BLEND_ONE_MINUS_SRC_ALPHA);
		break;
	case 1:
		//renderer->SetAlphaTestMode(RENDERER_ALPHA_TEST_ON);
		//renderer->SetAlphaMode(RENDERER_ALPHA_GREATER, 0.95f);

		shader_flag |= ModuleRenderer::ALPHA_TEST_HIGH;
		renderer->SetBlendMode(RENDERER_BLEND_OFF);

		break;
	default:
		//renderer->SetAlphaTestMode(RENDERER_ALPHA_TEST_OFF);
		renderer->SetBlendMode(RENDERER_BLEND_OFF);
		break;
	}

	renderer->GetActiveShader()->ChooseShader(state, shader_flag);

	if (lQI->vertexCount == 4 || (lQI->Flag & UIVerticesInfo_TriangleStrip))
		renderer->DrawUIQuad(state, lQI);
	else
		renderer->DrawUITriangles(state, lQI);

	PostDraw(state); // deactivate texture if any

	if ((lQI->Flag & UIVerticesInfo_UseModelMatrix) != 0)
		renderer->PopMatrix(MATRIX_MODE_MODEL);

}
