// #### 2DLayers Include 
#include "UI/UIDrawableItem.h"
// #### Renderer Include 
#include "UIVerticesInfo.h"
// #### Core Include
#include "Core.h"

#include "ModuleRenderer.h"
#include "TravState.h"
#include "UI/UIShapeDelegate.h"

using namespace Kigs::Draw2D;
using namespace Kigs::Scene;


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

	if (mShape)
	{
		return mShape->SetVertexArray(this, aQI);
	}
	auto slice_size = (v2f)mSliced;
	if (slice_size == v2f(0, 0))
	{
		aQI->Resize(4);
		VInfo2D::Data* buf = reinterpret_cast<VInfo2D::Data*>(aQI->Buffer());
		v2f pt[4];

		pt[0] = getDrawablePos({ 0.0f,0.0f });
		pt[1] = getDrawablePos({ 0.0f,1.0f });
		pt[2] = getDrawablePos({ 1.0f,1.0f });
		pt[3] = getDrawablePos({ 1.0f,0.0f });

		for (auto& p : pt) p *= mRealSize;

		TransformPoints(pt, 4);
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

	if (mRealSize.x < slice_size.x * 2)
		slice_size.x = mRealSize.x / 2;
	if (mRealSize.y < slice_size.y * 2)
		slice_size.y = mRealSize.y / 2;

	auto current_pos = positions;
	// Top Left
	make_quad(current_pos, v2f(0, 0), slice_size);
	// Top
	make_quad(current_pos, v2f(slice_size.x, 0), v2f((mRealSize.x - slice_size.x * 2.0f), slice_size.y));
	// Top Right
	make_quad(current_pos, v2f(mRealSize.x - slice_size.x, 0), slice_size);
	// Left
	make_quad(current_pos, v2f(0, slice_size.y), v2f(slice_size.x, (mRealSize.y - slice_size.y * 2.0f)));
	// Center
	make_quad(current_pos, v2f(slice_size.x, slice_size.y), mRealSize - slice_size * 2.0f);
	// Right
	make_quad(current_pos, v2f(mRealSize.x - slice_size.x, slice_size.y), v2f(slice_size.x, (mRealSize.y - slice_size.y * 2.0f)));
	// Bottom Left
	make_quad(current_pos, v2f(0, mRealSize.y - slice_size.y), slice_size);
	// Bottom
	make_quad(current_pos, v2f(slice_size.x, mRealSize.y - slice_size.y), v2f((mRealSize.x - slice_size.x * 2.0f), slice_size.y));
	// Bottom Right
	make_quad(current_pos, v2f(mRealSize.x - slice_size.x, mRealSize.y - slice_size.y), slice_size);

	TransformPoints(positions, 6 * 9);
	for (int i = 0; i < 6 * 9; ++i)
		buf[i].setVertex(positions[i]);
}

void UIDrawableItem::SetColor(UIVerticesInfo * aQI)
{
	unsigned char r = (unsigned char)(mColor[0] * 255.0f);
	unsigned char g = (unsigned char)(mColor[1] * 255.0f);
	unsigned char b = (unsigned char)(mColor[2] * 255.0f);
	unsigned char a = (unsigned char)(GetOpacity() *255.0f);

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
	unsigned char a = (unsigned char)((mOpacity >= 0.0f) ? mOpacity*255.0f : mParent->GetOpacity()*255.f);

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

	Draw::ModuleSpecificRenderer* renderer = (Draw::ModuleSpecificRenderer*)state->GetRenderer();
	unsigned int shader_flag = Draw::ModuleRenderer::VERTEX_ARRAY_MASK | Draw::ModuleRenderer::NO_LIGHT_MASK;

	PreDraw(state); // activate texture if any
	//lQI->resetFlag();

	if (GetNodeFlag(Node2D_NeedVerticeInfoUpdate))
	{
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

		ClearNodeFlag(Node2D_NeedVerticeInfoUpdate);
	}


	if ((lQI->Flag & UIVerticesInfo_Color) != 0)
		shader_flag |= Draw::ModuleRenderer::COLOR_ARRAY_MASK;
	if ((lQI->Flag & UIVerticesInfo_Texture) != 0)
		shader_flag |= Draw::ModuleRenderer::TEXCOORD_ARRAY_MASK;
	if ((lQI->Flag & UIVerticesInfo_BGRTexture) != 0)
		shader_flag |= Draw::ModuleRenderer::SHADER_FLAGS_USER1;

	if ((lQI->Flag & UIVerticesInfo_UseModelMatrix) != 0)
	{
		mat3 m = GetGlobalTransform();
		mat4 m4 = (mat4)m;
		//renderer->PushMatrix(MATRIX_MODE_MODEL);
		renderer->PushAndMultMatrix(Draw::MATRIX_MODE_MODEL, &m4.e[0][0]);
	}

	// ** Manage Transparency (after texture init) **
	int lTransparencyType = GetTransparencyType();
	switch (lTransparencyType)
	{
	case 2:
		//renderer->SetAlphaTestMode(RENDERER_ALPHA_TEST_ON);
		//renderer->SetAlphaMode(RENDERER_ALPHA_GREATER, 0.01f);
		shader_flag |= Draw::ModuleRenderer::ALPHA_TEST_LOW;
		renderer->SetBlendMode(Draw::RENDERER_BLEND_ON);
		renderer->SetBlendFuncMode(Draw::RENDERER_BLEND_SRC_ALPHA, Draw::RENDERER_BLEND_ONE_MINUS_SRC_ALPHA);
		break;
	case 1:
		//renderer->SetAlphaTestMode(RENDERER_ALPHA_TEST_ON);
		//renderer->SetAlphaMode(RENDERER_ALPHA_GREATER, 0.95f);

		shader_flag |= Draw::ModuleRenderer::ALPHA_TEST_HIGH;
		renderer->SetBlendMode(Draw::RENDERER_BLEND_OFF);

		break;
	default:
		//renderer->SetAlphaTestMode(RENDERER_ALPHA_TEST_OFF);
		renderer->SetBlendMode(Draw::RENDERER_BLEND_OFF);
		break;
	}

	renderer->GetActiveShader()->ChooseShader(state, shader_flag);

	if (lQI->vertexCount == 4 || (lQI->Flag & UIVerticesInfo_TriangleStrip))
		renderer->DrawUIQuad(state, lQI);
	else
		renderer->DrawUITriangles(state, lQI);

	PostDraw(state); // deactivate texture if any

	if ((lQI->Flag & UIVerticesInfo_UseModelMatrix) != 0)
		renderer->PopMatrix(Draw::MATRIX_MODE_MODEL);

}

bool UIDrawableItem::addItem(const CMSP& item, ItemPosition pos DECLARE_LINK_NAME)
{
	if (item->isSubType(UIShapeDelegate::mClassID))
	{
		mShape = item;
		SetNodeFlag(Node2D_NeedVerticeInfoUpdate);
	}

	return UIItem::addItem(item, pos PASS_LINK_NAME(linkName));
}

bool UIDrawableItem::removeItem(const CMSP& item DECLARE_LINK_NAME)
{
	if (item->isSubType(UIShapeDelegate::mClassID))
	{
		if (item == mShape)
		{
			mShape = nullptr;
			SetNodeFlag(Node2D_NeedVerticeInfoUpdate);
		}
	}
	return UIItem::removeItem(item PASS_LINK_NAME(linkName));
}
