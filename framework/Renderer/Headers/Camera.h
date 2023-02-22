#pragma once

#include "Node3D.h"
#include "TecLibs/Tec3D.h"
#include "RenderingScreen.h"
#include "maReference.h"
#include "ModuleInput.h"
#include "TouchInputEventManager.h"

namespace Kigs
{
	namespace Scene
	{
		class CullingObject;
	}
	namespace Draw
	{
		// ****************************************
		// * Camera class
		// * --------------------------------------
		/**
		 * \file	Camera.h
		 * \class	Camera
		 * \ingroup Renderer
		 * \brief	Abstract class, special Node3D to manage a camera ( viewpoint in the scene )
		 */
		 // ****************************************
		class Camera : public Node3D
		{
		public:
			DECLARE_ABSTRACT_CLASS_INFO(Camera, Node3D, Renderer);
			DECLARE_CONSTRUCTOR(Camera);
			WRAP_METHODS(GetDataInTouchSupport, ManageScrollTouchEvent, ManagePinchTouchEvent);

			friend class Scene3D;

			/**
				* \brief	return true if camera was activated else do not render camera
				* \fn 		bool SetActive(TravState* state);
				* \return	TRUE if the camera is active, FALSE otherwise
				*/
			bool SetActive(TravState* state);

			/**
				* \brief	release the camera
				* \fn 		void Release()
				*/
			void Release(TravState* state);

			/**
				* \brief	set the viewport
				* \fn 		void	SetViewport(float vpminx,float vpminy,float vpsx,float vpsy)
				* \param	vpminx : position in x axis of the viewport
				* \param	vpminy : position in y axis of the viewport
				* \param	vpsx : width of the viewport
				* \param	vpsy : height of the viewport
				*/
			void	SetViewport(float vpminx, float vpminy, float vpsx, float vpsy)
			{
				mViewportMinX = vpminx;
				mViewportMinY = vpminy;
				mViewportSizeX = vpsx;
				mViewportSizeY = vpsy;
			}

			void	GetViewport(float& vpminx, float& vpminy, float& vpsx, float& vpsy)
			{
				vpminx = mViewportMinX;
				vpminy = mViewportMinY;
				vpsx = mViewportSizeX;
				vpsy = mViewportSizeY;
			}


			void ChangeMatrix(const Matrix3x4&) override;

			/**
				* \brief	set the position of the camera
				* \fn 		void	SetPosition(float x,float y,float z)
				* \param	x : position in x axis of the camera
				* \param	y : position in y axis of the camera
				* \param	z : position in z axis of the camera
				*/
			void	SetPosition(float x, float y, float z)
			{
				mPosition[0] = x;
				mPosition[1] = y;
				mPosition[2] = z;
				RecomputeMatrix();
			}

			void	SetPosition(const Point3D& pt)
			{
				SetPosition(pt.x, pt.y, pt.z);
			}

			/**
				* \brief	get the position of the camera
				* \fn 		void	GetPosition(float& x,float& y,float& z)
				* \param	x : position in x axis of the camera (in/out param)
				* \param	y : position in y axis of the camera (in/out param)
				* \param	z : position in z axis of the camera (in/out param)
				*/
			void	GetPosition(float& x, float& y, float& z)
			{
				x = mPosition[0];
				y = mPosition[1];
				z = mPosition[2];
			}
			Point3D GetPosition() const { return Point3D(mPosition[0], mPosition[1], mPosition[2]); }

			Point3D GetGlobalPosition() { return GetLocalToGlobal().Pos; }
			/**
				* \brief	set the view factor
				* \fn 		void	SetViewVector(float x,float y,float z)
				* \param	x : position in x axis of the view factor
				* \param	y : position in y axis of the view factor
				* \param	z : position in z axis of the view factor
				*/
			void	SetViewVector(float x, float y, float z)
			{
				mViewVector[0] = x;
				mViewVector[1] = y;
				mViewVector[2] = z;
				RecomputeMatrix();
			}
			void SetViewVector(const Point3D& v)
			{
				SetViewVector(v.x, v.y, v.z);
			}

			/**
				* \brief	get the view factor
				* \fn 		void	GetViewVector(float& x,float& y,float& z )
				* \param	x : position in x axis of the view factor (in/out param)
				* \param	y : position in y axis of the view factor (in/out param)
				* \param	z : position in z axis of the view factor (in/out param)
				*/
			void	GetViewVector(float& x, float& y, float& z)
			{
				x = mViewVector[0];
				y = mViewVector[1];
				z = mViewVector[2];
			}
			Point3D GetViewVector() const { return Point3D(mViewVector[0], mViewVector[1], mViewVector[2]); }
			Point3D GetGlobalViewVector() { return GetLocalToGlobal().XAxis.Normalized(); }


			std::pair<v3f, Vector3D> GetGlobalRay() { return { GetGlobalPosition(), GetGlobalViewVector() }; }
			std::pair<v3f, Vector3D> GetRay() { return { GetPosition(), GetViewVector() }; }


			/**
				* \brief	set the position of the up vector
				* \fn 		void	SetUpVector(float x,float y,float z)
				* \param	x : position in x axis of the up vector
				* \param	y : position in y axis of the up vector
				* \param	z : position in z axis of the up vector
				*/
			void	SetUpVector(float x, float y, float z)
			{
				mUpVector[0] = x;
				mUpVector[1] = y;
				mUpVector[2] = z;
				RecomputeMatrix();
			}

			void SetUpVector(const Point3D& v)
			{
				SetUpVector(v.x, v.y, v.z);
			}

			/**
				* \brief	get the up vector
				* \fn 		void	GetUpVector(float& x,float& y,float& z )
				* \param	x : position in x axis of the up vector (in/out param)
				* \param	y : position in y axis of the up vector (in/out param)
				* \param	z : position in z axis of the up vector (in/out param)
				*/
			void	GetUpVector(float& x, float& y, float& z)
			{
				x = mUpVector[0];
				y = mUpVector[1];
				z = mUpVector[2];
			}
			Point3D GetUpVector() const { return Point3D(mUpVector[0], mUpVector[1], mUpVector[2]); }
			Point3D GetGlobalUpVector() { return GetLocalToGlobal().ZAxis.Normalized(); }

			Point3D GetGlobalRightVector() { return GetLocalToGlobal().YAxis.Normalized(); }

			/**
			* \brief	special case for camera has we want it to be init even if have no bounding box
			*/
			void	GetNodeBoundingBox(Point3D& pmin, Point3D& pmax) const override { pmin.Set(-0.5f, -0.5f, -0.5f); pmax.Set(0.5f, 0.5f, 0.5f); }

			/**
				* \brief	set the zNear and zFar values
				* \fn 		void	SetNearAndFar(float znear,float zfar)
				* \param	znear : zNear value
				* \param	zfar : zFar value
				*/
			void	SetNearAndFar(float znear, float zfar)
			{
				mNearPlane = znear;
				mFarPlane = zfar;
			}

			/**
				* \brief	set the clear color
				* \fn 		void	SetClearColor(float r,float g,float b)
				* \param	r : red color
				* \param	g : green color
				* \param	b : blue color
				*/
			void	SetClearColor(float r, float g, float b, float a = 1.0f)
			{
				mClearColor[0] = r;
				mClearColor[1] = g;
				mClearColor[2] = b;
				mClearColor[3] = a;
			}

			/**
				* \brief	check if the camera is active
				* \fn 		bool	IsActive()
				* \return TRUE if the camera is active, FALSE otherwise
				*/
			bool	IsActive() { return mIsActive; }

			/**
			* \brief	check if the camera is in All Visible mode
			* \fn 		bool	AllVisible()
			* \return TRUE if the camera is in All Visible mode, FALSE otherwise
			*/
			bool	AllVisible() { return mAllVisible; }

			/**
				* \brief	initialize the culling on an object
				* \fn 		virtual void  InitCullingObject(CullingObject* mObj);
				* \param	mObj : object to cull
				*/
			virtual void  InitCullingObject(CullingObject* obj);

			struct FrustumPlanes
			{
				struct Plane { v3f o; Vector3D n; };
				Plane Near;
				Plane Far;
				Plane Down;
				Plane Up;
				Plane Left;
				Plane Right;
			};
			FrustumPlanes GetFrustum();

			/**
				* \brief	get the ray from a pixel
				* \fn 		virtual void getRay(const float &ScreenX, const float &ScreenY, Point3D &RayOrigin, Vector3D &RayDirection);
				* \param	ScreenX : position in x axis of the pixel
				* \param	ScreenY : position in y axis of the pixel
				* \param	RayOrigin : origin of the ray (in/out param)
				* \param	RayDirection : direction of the ray (in/out param)
				*/
			virtual void getRay(const float& ScreenX, const float& ScreenY, Point3D& RayOrigin, Vector3D& RayDirection);

			//! project given point 3D using camera projection
			// return true if point is in front of camera, false if behind it
			virtual bool Project(float&/* ScreenX */, float& /* ScreenY */, Point3D /* Pt */);

			/**
				* \brief	do a Node3D cull but always return true
				* \fn 		virtual bool    Cull(TravState* state,unsigned int cullingMask);
				* \param	state : state of the node
				* \return	TRUE
				*/
		bool    Cull(TravState* state,unsigned int cullingMask) override;

			inline bool IsEnabled()const { return mCameraIsEnabled; }
			inline void setIsEnabled(bool a_value)
			{
				mCameraIsEnabled = a_value;
			}

			inline unsigned int getPriority()const { return mPriority; }
			inline void setPriority(unsigned int a_value)
			{
				mPriority = a_value;
			}

			bool	Draw(TravState* state) override;

			void NotifyUpdate(const unsigned int /* labelid */) override;

			//! link to the rendering screen
			RenderingScreen* getRenderingScreen()
			{
				return mRenderingScreen.lock().get();
			}

			virtual bool	GetDataInTouchSupport(const Input::touchPosInfos& posin, Input::touchPosInfos& pout);

			// test
			//bool			ManageClickTouchEvent(v3f pos, int type, int state, int clickCount);

			virtual std::array<mat4, 2> GetStereoViewProjections() { KIGS_ASSERT(!"Camera doesn't support stereo rendering");  return {}; }

			bool ManageScrollTouchEvent(Input::ScrollEvent& scroll_event);
			bool ManagePinchTouchEvent(Input::PinchEvent& pinch_event);

			auto& GetRenderPasses() { return mRenderPasses; }

			bool UsesFog() { return mUseFog; }
			float GetFogScale() { return mFogScale; }
			v4f GetFogColor() { return mFogColor; }
			float GetNearPlane() { return mNearPlane; }
			float GetFarPlane() { return mFarPlane; }

		protected:

			std::vector<RenderPass> mRenderPasses;

			/**
				* \brief	set the ambient light
				* \fn 		virtual void SetAmbient(float r,float g,float b)=0;
				* \param	r : reg color
				* \param	g : green color
				* \param	b : blue color
				*/
				//virtual void SetAmbient(float r,float g,float b)=0;

				/**
					* \brief	enable the camera protection
					* \fn 		virtual bool ProtectedSetActive(TravState* state)=0;
					* \return	true
					*/
			virtual bool ProtectedSetActive(TravState* state) = 0;

			/**
				* \brief	disable the camera protection
				* \fn 		virtual void ProtectedRelease(TravState* state)=0;
				*/
			virtual void ProtectedRelease(TravState* state) = 0;

			/**
				* \brief	initialize modifiable
				* \fn 		void InitModifiable() override
				*/
			void	InitModifiable() override;

			/**
				* \brief	destructor
				* \fn 		~Camera();
				*/
			virtual ~Camera();

			//! recompute matrix
			void	RecomputeMatrix();

			void	activeTouchControlledCamera(bool active);

			//! should clear zBuffer
			bool				mClearZBuffer = true;
			//! should clear color buffer
			bool				mClearColorBuffer = true;
			//! should clear stencil buffer
			bool				mClearStencilBuffer = false;
			//! Disable or Enable camera
			bool				mCameraIsEnabled = true;

			//! Camera all visible(no cull)
			bool				mAllVisible = false;

			bool				mTouchControlled = false;

			bool				mUseFog = false;

			//! coordinate on x axis of the min point of the viewport
			float				mViewportMinX = 0.0f;
			//! coordinate on y axis of the min point of the viewport
			float				mViewportMinY = 0.0f;
			//! width of the viewport
			float				mViewportSizeX = 1.0f;
			//! height of the viewport
			float				mViewportSizeY = 1.0f;

			//! zNear value
			float				mNearPlane = 0.1f;
			//! zFar value
			float				mFarPlane = 40.0f;

			//! vertical FOV of the camera
			float				mVerticalFOV = 45.0f;
			//! aspect ration of the camera
			float				mAspectRatio = 0.0f;
			//! Camera brightness, change rendering screen brightness
			float				mBrightness = 0.0f;
			//! fog scale
			float				mFogScale = 10.0f;

			//! Camera priority
			u32					mPriority = 0.0f;

			//! position of the camera
			v3f					mPosition;
			//! up vector of the camera
			v3f					mUpVector;
			//! direction of the camera
			v3f					mViewVector;
			//! clear color of the camera
			v4f					mClearColor;
			//! fog color
			v4f					mFogColor = { 0.0f, 0.0f, 0.0f, 1.0f };

			//! reference on the rendering screen
			std::weak_ptr<RenderingScreen>	mRenderingScreen;
			//maReferenceInit		mRenderingScreen;

			WRAP_ATTRIBUTES(mClearZBuffer,mClearColorBuffer, mClearStencilBuffer, mCameraIsEnabled, mAllVisible, mTouchControlled, mUseFog,
							mViewportMinX,mViewportMinY,mViewportSizeX,mViewportSizeY,mNearPlane,mFarPlane,
							mVerticalFOV, mAspectRatio, mBrightness, mFogScale, mPriority, mPosition,mUpVector,mViewVector,mClearColor, mFogColor,
							mRenderingScreen);

			//! TRUE when the camera is active
			bool				mIsActive;

			struct touchControlledDataStruct
			{
				// data
				Point2D		mStartPt; // used for rotation
				float		mOneOnCoef; // used for rotation and zoom
				float		mTargetPointDist; // used for rotation and zoom
				Vector3D	mStartV; // used for rotation and zoom

				// store starting camera data
				Matrix3x4	mStartMatrix;

				unsigned int mState;
			};

			// remove touchControlledDataStruct if needed
			virtual	void UninitModifiable() override;
		};

	}
}
