#define DEBUG_DRAW_IMPLEMENTATION
#include "GLSLDebugDraw.h"

#include "Shader.h"
#include "TravState.h"
#include "ModuleRenderer.h"
#include "Camera.h"


#include "NotificationCenter.h"

using namespace Kigs::Utils;
using namespace Kigs::Scene;
using namespace Kigs::Draw;
using namespace Kigs::Maths;

//IMPLEMENT_AND_REGISTER_CLASS_INFO(DebugDraw, DebugDraw, StandAlone);
IMPLEMENT_CLASS_INFO(DebugDraw)


IMPLEMENT_CONSTRUCTOR(DebugDraw)
, _glyph_texture(nullptr)
{
	dd::initialize(this);
	mDrawingNeeds = Need_Draw;
	KigsCore::GetNotificationCenter()->addObserver(this, "ResetContext", "ResetContext");
}

DEFINE_METHOD(DebugDraw, ResetContext)
{
	dd::shutdown();
	dd::initialize(this);
	return false;
}

DebugDraw::~DebugDraw()
{
	dd::shutdown();
}


#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))



void DebugDraw::GetNodeBoundingBox(Point3D& pmin, Point3D& pmax) const
{
	pmin.Set(-FLT_MAX, -FLT_MAX, -FLT_MAX);
	pmax.Set(FLT_MAX, FLT_MAX, FLT_MAX);
}

TravState* DebugDraw::currentState = nullptr;
bool DebugDraw::Draw(TravState* state)
{
	//if (!dd::hasPendingDraws()) return false;
	dd::g_renderInterface = this; // HACK

	_renderer = state->GetRenderer();


	int lShaderMask = ModuleRenderer::VERTEX_ARRAY_MASK | ModuleRenderer::NO_LIGHT_MASK | ModuleRenderer::COLOR_ARRAY_MASK;

	// create shader if none
	_renderer->GetActiveShader()->ChooseShader(state, lShaderMask);

	_locs = _renderer->GetActiveShader()->GetLocation();
	_renderer->PushState();
	_renderer->PushMatrix(MATRIX_MODE_MODEL); 
	_renderer->LoadIdentity(MATRIX_MODE_MODEL);

	currentState = state;	
	
	_renderer->SetAlphaTestMode(RENDERER_ALPHA_TEST_OFF);
	_renderer->SetBlendMode(RENDERER_BLEND_OFF);

	_renderer->FlushState();

	auto now = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now());
	dd::flush(now.time_since_epoch().count());
	currentState = nullptr;

	_renderer->PopMatrix(MATRIX_MODE_MODEL);
	_renderer->PopState();
	return true;
}


dd::GlyphTextureHandle DebugDraw::createGlyphTexture(int width, int height, const void* pixels)
{
/*	_glyph_texture = KigsCore::GetInstanceOf("glyph_texture", "Texture");
	SmartPointer<TinyImage>	img = TinyImage::CreateImage((void*)pixels, width, height, TinyImage::ImageFormat::GREYSCALE);
	_glyph_texture->CreateFromImage(img);
	*/
	return (dd::OpaqueTextureType*)(Texture*)_glyph_texture.get();
}

void DebugDraw::destroyGlyphTexture(dd::GlyphTextureHandle glyphTex)
{
	if((Texture*)glyphTex == _glyph_texture.get())
	{
		_glyph_texture = nullptr;
	}
}

void DebugDraw::drawPointList(const dd::DrawVertex* points, int count, bool depthEnabled)
{

	auto vbo = _renderer->getVBO();
	_renderer->SetDepthTestMode(depthEnabled);
	_renderer->BufferData(vbo, KIGS_BUFFER_TARGET_ARRAY, count * sizeof(dd::DrawVertex), (void*)points, KIGS_BUFFER_USAGE_DYNAMIC);
	_renderer->SetVertexAttrib(vbo, KIGS_VERTEX_ATTRIB_VERTEX_ID, 3, KIGS_FLOAT, false, sizeof(dd::DrawVertex), (void*)OFFSETOF(dd::DrawVertex, point.x), _locs);
	_renderer->SetVertexAttrib(vbo, KIGS_VERTEX_ATTRIB_COLOR_ID, 3, KIGS_FLOAT, false, sizeof(dd::DrawVertex), (void*)OFFSETOF(dd::DrawVertex, point.r), _locs);
	_renderer->DrawArrays(currentState, KIGS_DRAW_MODE_POINTS, 0, count);
}

void DebugDraw::drawLineList(const dd::DrawVertex* lines, int count, bool depthEnabled)
{
	auto vbo = _renderer->getVBO();
	_renderer->SetDepthTestMode(depthEnabled);
	_renderer->BufferData(vbo, KIGS_BUFFER_TARGET_ARRAY, count * sizeof(dd::DrawVertex), (void*)lines, KIGS_BUFFER_USAGE_DYNAMIC);
	_renderer->SetVertexAttrib(vbo, KIGS_VERTEX_ATTRIB_VERTEX_ID, 3, KIGS_FLOAT, false, sizeof(dd::DrawVertex), (void*)OFFSETOF(dd::DrawVertex, line.x), _locs);
	_renderer->SetVertexAttrib(vbo, KIGS_VERTEX_ATTRIB_COLOR_ID, 3, KIGS_FLOAT, false, sizeof(dd::DrawVertex), (void*)OFFSETOF(dd::DrawVertex, line.r), _locs);
	_renderer->DrawArrays(currentState, KIGS_DRAW_MODE_LINES, 0, count);
}


void DebugDraw::addUser(CoreModifiable* parent)
{
	if (parent->isSubType("UICustomDraw"))
	{
		auto cd = static_cast<Draw2D::UICustomDraw*>(parent);
		cd->SetDelegate(this);
	}
	ParentClassType::addUser(parent);
}
void DebugDraw::removeUser(CoreModifiable* parent)
{
	if (parent->isSubType("UICustomDraw"))
	{
		auto cd = static_cast<Draw2D::UICustomDraw*>(parent);
		if(cd->GetDelegate() == this)
			cd->SetDelegate(nullptr);
	}
	ParentClassType::removeUser(parent);
}

namespace
{
	Point2D current_size(1,1);
};

void dd::set_rendering_size(Point2D size)
{
	current_size = size;
}

void dd::line2D(Point2D p1, Point2D p2, ddVec3Param color, int duration)
{
	dd::line({ p1.x*current_size.x, p1.y*current_size.y, 0 }, { p2.x*current_size.x, p2.y*current_size.y, 0 }, color, duration, false);
}


void dd::rect2D(Point2D p1, Point2D p2, ddVec3Param color, int duration)
{
	dd::line2D(p1, { p2.x, p1.y }, color, duration);
	dd::line2D(p2, { p2.x, p1.y }, color, duration);
	dd::line2D(p2, { p1.x, p2.y }, color, duration);
	dd::line2D(p1, { p1.x, p2.y }, color, duration);
}


dd::CameraPoints dd::camera_points(Camera* cam)
{
	dd::CameraPoints result;
	v2f rs;
	cam->getRenderingScreen()->GetSize(rs.x, rs.y);

	float farZ = cam->getValue<float>("FarPlane");
	float nearZ = cam->getValue<float>("NearPlane");

	float aspect = cam->getValue<float>("AspectRatio");
	if (aspect == 0) aspect = (rs.x*cam->getValue<float>("ViewportSizeX")) / (rs.y*cam->getValue<float>("ViewportSizeY"));

	float frustumHeight = (float)tanf(cam->getValue<float>("VerticalFOV") * fPI / 360.0f) * nearZ;

	float farHeight = (float)tanf(cam->getValue<float>("VerticalFOV") * fPI / 360.0f) * farZ;
	float farWidth = farHeight * aspect;

	float frustumWidth = frustumHeight * aspect;

	v3f pos = cam->GetGlobalPosition();
	v3f view = cam->GetGlobalViewVector();
	v3f up = cam->GetGlobalUpVector();
	v3f right = up^view;

	result.ptnear[0] = pos + view*nearZ + right*frustumWidth + up*frustumHeight;
	result.ptnear[1] = pos + view*nearZ + right*frustumWidth - up*frustumHeight;
	result.ptnear[2] = pos + view*nearZ - right*frustumWidth - up*frustumHeight;
	result.ptnear[3] = pos + view*nearZ - right*frustumWidth + up*frustumHeight;

	result.ptfar[0] = pos + view*farZ + right*farWidth + up*farHeight;
	result.ptfar[1] = pos + view*farZ + right*farWidth - up*farHeight;
	result.ptfar[2] = pos + view*farZ - right*farWidth - up*farHeight;
	result.ptfar[3] = pos + view*farZ - right*farWidth + up*farHeight;
	return result;
}

void dd::camera(Camera* cam, ddVec3Param color, u32 flags, int duration)
{
	auto pts = dd::camera_points(cam);
	dd::camera(pts, color, flags, duration);
}

void dd::camera(const dd::CameraPoints& pts, ddVec3Param color, u32 flags, int duration)
{
	dd::line(pts.ptnear[0], pts.ptnear[1], color, duration);
	dd::line(pts.ptnear[1], pts.ptnear[2], color, duration);
	dd::line(pts.ptnear[2], pts.ptnear[3], color, duration);
	dd::line(pts.ptnear[3], pts.ptnear[0], color, duration);

	dd::line(pts.ptfar[0], pts.ptfar[1], color, duration);
	dd::line(pts.ptfar[1], pts.ptfar[2], color, duration);
	dd::line(pts.ptfar[2], pts.ptfar[3], color, duration);
	dd::line(pts.ptfar[3], pts.ptfar[0], color, duration);

	if ((flags & CameraFlag_NoFarCross) == 0)
	{
		dd::line(pts.ptfar[0], pts.ptfar[2], color, duration);
		dd::line(pts.ptfar[1], pts.ptfar[3], color, duration);
	}
	
	dd::line(pts.ptnear[0], pts.ptfar[0], color, duration);
	dd::line(pts.ptnear[1], pts.ptfar[1], color, duration);
	dd::line(pts.ptnear[2], pts.ptfar[2], color, duration);
	dd::line(pts.ptnear[3], pts.ptfar[3], color, duration);

	dd::line((pts.ptnear[0] + pts.ptnear[1] + pts.ptnear[2] + pts.ptnear[3]) / 4, (pts.ptnear[0] + pts.ptnear[3]) / 2, color, duration);
	dd::line((pts.ptnear[0] + pts.ptnear[1] + pts.ptnear[2] + pts.ptnear[3]) / 4, (pts.ptfar[0] + pts.ptfar[1] + pts.ptfar[2] + pts.ptfar[3]) / 4, color, duration);
}

void dd::local_bbox(const Matrix3x4& local_to_global, BBox bbox, ddVec3Param color, int duration, bool depth_enabled)
{
	v3f pts[8];
	bbox.ConvertToPoint(pts);
	local_to_global.TransformPoints(pts, 8);
	dd::box(pts, color, duration, depth_enabled);
}