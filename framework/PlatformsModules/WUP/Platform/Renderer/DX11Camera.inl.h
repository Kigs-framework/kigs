#include "Platform/Main/BaseApp.h"

#include "DX11RenderingScreen.h"

#include <winrt/Windows.UI.Input.Spatial.h>
#include <winrt/Windows.Perception.People.h>
#include <winrt/Windows.Foundation.Collections.h>

#include "winrt_helpers.h"

#include <DirectXMath.h>

using namespace winrt::Windows::UI::Input::Spatial;


void DX11Camera::PlatformProtectedSetActive(TravState* state)
{
	auto rs = getRenderingScreen()->as<DX11RenderingScreen>();
	if (!rs->IsHolographic()) return;

	RendererDX11* renderer = reinterpret_cast<RendererDX11*>(ModuleRenderer::mTheGlobalRenderer);
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
		camera.SetNearPlaneDistance(mNearPlane);
		camera.SetFarPlaneDistance(mFarPlane);

		auto size = camera.RenderTargetSize();
		mViewportMinX = pose.Viewport().X / size.Width;
		mViewportMinY = pose.Viewport().Y / size.Height;
		mViewportSizeX = pose.Viewport().Width / size.Width;
		mViewportSizeY = pose.Viewport().Height / size.Height;

#ifdef KIGS_HOLOLENS2
		mVerticalFOV = 29.0f;
#else
		mVerticalFOV = 17.5f;
#endif
		auto camera_projection_transform = pose.ProjectionTransform();
		auto view_transform_container = pose.TryGetViewTransform(coordinate_system);
		if (view_transform_container)
		{
			auto viewCoordinateSystemTransform = view_transform_container.Value();

			auto view_left = Mat4FromFloat4x4(viewCoordinateSystemTransform.Left);
			auto view_right = Mat4FromFloat4x4(viewCoordinateSystemTransform.Right);

			mCurrentStereoViewproj[0] = Mat4FromFloat4x4(camera_projection_transform.Left) * view_left;
			mCurrentStereoViewproj[1] = Mat4FromFloat4x4(camera_projection_transform.Right) * view_right;

			auto p1 = Inv(view_left);
			auto p2 = Inv(view_right);
			
			auto pos = (p1.Pos.xyz + p2.Pos.xyz)*0.5f;
			auto up = ((p1.YAxis.xyz + p2.YAxis.xyz) * 0.5f).Normalized();
			auto view = (-(p1.ZAxis.xyz + p2.ZAxis.xyz) * 0.5f).Normalized();

			SetViewVector(view);
			SetUpVector(up);
			SetPosition(pos);
		}
	}
	/*
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
	*/
}

void DX11Camera::PlatformProtectedRelease(TravState* state)
{
}