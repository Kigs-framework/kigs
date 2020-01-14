#include "Platform/Main/BaseApp.h"

#include "DX11RenderingScreen.h"

#include <winrt/Windows.UI.Input.Spatial.h>
#include <winrt/Windows.Perception.People.h>

using namespace winrt::Windows::UI::Input::Spatial;

inline mat4 MatFromFloat4x4(const winrt::Windows::Foundation::Numerics::float4x4& transform)
{
	auto mat_row_major = mat4(
		v4f(transform.m11, transform.m12, transform.m13, transform.m14),
		v4f(transform.m21, transform.m22, transform.m23, transform.m24),
		v4f(transform.m31, transform.m32, transform.m33, transform.m34),
		v4f(transform.m41, transform.m42, transform.m43, transform.m44));

	return mat_row_major;
}

void DX11Camera::PlatformProtectedSetActive(TravState* state)
{
	auto rs = getRenderingScreen()->as<DX11RenderingScreen>();
	if (!rs->IsHolographic()) return;

	RendererDX11* renderer = reinterpret_cast<RendererDX11*>(ModuleRenderer::theGlobalRenderer);
	DXInstance* dxinstance = renderer->getDXInstance();
	auto frame = dxinstance->mCurrentFrame;

	frame.UpdateCurrentPrediction();
	auto prediction = frame.CurrentPrediction();
	auto poses = prediction.CameraPoses();
	auto coordinate_system = dxinstance->mStationaryReferenceFrame.CoordinateSystem();
	
	for (auto pose : poses)
	{
		auto camera = pose.HolographicCamera();
		//camera.ViewportScaleFactor(1.0f);
		camera.SetNearPlaneDistance(myNear);
		camera.SetFarPlaneDistance(myFar);

		auto size = camera.RenderTargetSize();
		myViewportMinX = pose.Viewport().X / size.Width;
		myViewportMinY = pose.Viewport().Y / size.Height;
		myViewportSizeX = pose.Viewport().Width / size.Width;
		myViewportSizeY = pose.Viewport().Height / size.Height;

#ifdef KIGS_HOLOLENS2
		myVerticalFOV = 29.0f;
#else
		myVerticalFOV = 17.5f;
#endif
		auto camera_projection_transform = pose.ProjectionTransform();
		auto view_transform_container = pose.TryGetViewTransform(coordinate_system);
		if (view_transform_container)
		{
			auto viewCoordinateSystemTransform = view_transform_container.Value();
			mCurrentStereoViewproj[0] = MatFromFloat4x4(camera_projection_transform.Left) * MatFromFloat4x4(viewCoordinateSystemTransform.Left);
			mCurrentStereoViewproj[1] = MatFromFloat4x4(camera_projection_transform.Right) * MatFromFloat4x4(viewCoordinateSystemTransform.Right);
		}
	}

	auto pointer_pose = SpatialPointerPose::TryGetAtTimestamp(coordinate_system, prediction.Timestamp());
	if (pointer_pose)
	{
		auto view = pointer_pose.Head().ForwardDirection();
		SetViewVector(v3f(view.x, view.y, view.z));
		auto up = pointer_pose.Head().UpDirection();
		SetUpVector(v3f(up.x, up.y, up.z));
		auto pos = pointer_pose.Head().Position();
		SetPosition(v3f(pos.x, pos.y, pos.z));
	}
}

void DX11Camera::PlatformProtectedRelease(TravState* state)
{
}