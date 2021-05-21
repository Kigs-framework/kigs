#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "Node3D.h"
#include "TecLibs/Tec3D.h"
#include "RenderingScreen.h"
#include "maReference.h"
#include "ModuleInput.h"
#include "TouchInputEventManager.h"

class CullingObject;
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
		* \fn 		void	SetViewport(kfloat vpminx,kfloat vpminy,kfloat vpsx,kfloat vpsy)
		* \param	vpminx : position in x axis of the viewport
		* \param	vpminy : position in y axis of the viewport
		* \param	vpsx : width of the viewport
		* \param	vpsy : height of the viewport
		*/
	void	SetViewport(kfloat vpminx,kfloat vpminy,kfloat vpsx,kfloat vpsy)
	{
		mViewportMinX=vpminx;
		mViewportMinY=vpminy;
		mViewportSizeX=vpsx;
		mViewportSizeY=vpsy;
	}
		
	void	GetViewport(kfloat& vpminx, kfloat& vpminy, kfloat& vpsx, kfloat& vpsy)
	{
		vpminx = mViewportMinX;
		vpminy = mViewportMinY;
		vpsx = mViewportSizeX;
		vpsy = mViewportSizeY;
	}


	void ChangeMatrix(const Matrix3x4&) override;
		
	/**
		* \brief	set the position of the camera
		* \fn 		void	SetPosition(kfloat x,kfloat y,kfloat z)
		* \param	x : position in x axis of the camera
		* \param	y : position in y axis of the camera
		* \param	z : position in z axis of the camera
		*/
	void	SetPosition(kfloat x,kfloat y,kfloat z)
	{
		mPosition[0]=x;
		mPosition[1]=y;
		mPosition[2]=z;
		RecomputeMatrix();
	}
		
	void	SetPosition(const Point3D& pt)
	{
		SetPosition(pt.x, pt.y, pt.z);
	}
		
	/**
		* \brief	get the position of the camera
		* \fn 		void	GetPosition(kfloat& x,kfloat& y,kfloat& z)
		* \param	x : position in x axis of the camera (in/out param)
		* \param	y : position in y axis of the camera (in/out param)
		* \param	z : position in z axis of the camera (in/out param)
		*/
	void	GetPosition(kfloat& x,kfloat& y,kfloat& z)
	{
		x=mPosition[0];
		y=mPosition[1];
		z=mPosition[2];
	}
	Point3D GetPosition() const { return Point3D(mPosition[0], mPosition[1], mPosition[2]); }
		
	Point3D GetGlobalPosition() { return GetLocalToGlobal().Pos; }
	/**
		* \brief	set the view factor
		* \fn 		void	SetViewVector(kfloat x,kfloat y,kfloat z)
		* \param	x : position in x axis of the view factor
		* \param	y : position in y axis of the view factor
		* \param	z : position in z axis of the view factor
		*/
	void	SetViewVector(kfloat x,kfloat y,kfloat z)
	{
		mViewVector[0]=x;
		mViewVector[1]=y;
		mViewVector[2]=z;
		RecomputeMatrix();
	}
	void SetViewVector(const Point3D& v)
	{
		SetViewVector(v.x, v.y, v.z);
	}
		
	/**
		* \brief	get the view factor
		* \fn 		void	GetViewVector(kfloat& x,kfloat& y,kfloat& z )
		* \param	x : position in x axis of the view factor (in/out param)
		* \param	y : position in y axis of the view factor (in/out param)
		* \param	z : position in z axis of the view factor (in/out param)
		*/
	void	GetViewVector(kfloat& x,kfloat& y,kfloat& z )
	{
		x=mViewVector[0];
		y=mViewVector[1];
		z=mViewVector[2];
	}
	Point3D GetViewVector() const { return Point3D(mViewVector[0], mViewVector[1], mViewVector[2]); }
	Point3D GetGlobalViewVector() { return GetLocalToGlobal().XAxis.Normalized(); }
		

	std::pair<v3f, Vector3D> GetGlobalRay() { return { GetGlobalPosition(), GetGlobalViewVector() }; }
	std::pair<v3f, Vector3D> GetRay() { return { GetPosition(), GetViewVector() }; }


	/**
		* \brief	set the position of the up vector
		* \fn 		void	SetUpVector(kfloat x,kfloat y,kfloat z)
		* \param	x : position in x axis of the up vector
		* \param	y : position in y axis of the up vector
		* \param	z : position in z axis of the up vector
		*/
	void	SetUpVector(kfloat x,kfloat y,kfloat z)
	{
		mUpVector[0]=x;
		mUpVector[1]=y;
		mUpVector[2]=z;
		RecomputeMatrix();
	}
		
	void SetUpVector(const Point3D& v)
	{
		SetUpVector(v.x, v.y, v.z);
	}
		
	/**
		* \brief	get the up vector
		* \fn 		void	GetUpVector(kfloat& x,kfloat& y,kfloat& z )
		* \param	x : position in x axis of the up vector (in/out param)
		* \param	y : position in y axis of the up vector (in/out param)
		* \param	z : position in z axis of the up vector (in/out param)
		*/
	void	GetUpVector(kfloat& x,kfloat& y,kfloat& z )
	{
		x=mUpVector[0];
		y=mUpVector[1];
		z=mUpVector[2];
	}
	Point3D GetUpVector() const { return Point3D(mUpVector[0], mUpVector[1], mUpVector[2]); }
	Point3D GetGlobalUpVector() { return GetLocalToGlobal().ZAxis.Normalized(); }

	Point3D GetGlobalRightVector() { return GetLocalToGlobal().YAxis.Normalized(); }

	/**
	* \brief	special case for camera has we want it to be init even if have no bounding box	
	*/
	void	GetNodeBoundingBox(Point3D& pmin,Point3D& pmax) const override { pmin.Set(-0.5f,-0.5f,-0.5f); pmax.Set(0.5f,0.5f,0.5f);}
		
	/**
		* \brief	set the zNear and zFar values
		* \fn 		void	SetNearAndFar(kfloat znear,kfloat zfar)
		* \param	znear : zNear value
		* \param	zfar : zFar value
		*/
	void	SetNearAndFar(kfloat znear,kfloat zfar)
	{
		mNearPlane=znear;
		mFarPlane=zfar;
	}
		
	/**
		* \brief	set the clear color
		* \fn 		void	SetClearColor(kfloat r,kfloat g,kfloat b)
		* \param	r : red color
		* \param	g : green color
		* \param	b : blue color
		*/
	void	SetClearColor(kfloat r,kfloat g,kfloat b,kfloat a=1.0f)
	{
		mClearColor[0]=r;
		mClearColor[1]=g;
		mClearColor[2]=b;
		mClearColor[3] = a;
	}
		
	/**
		* \brief	check if the camera is active
		* \fn 		bool	IsActive()
		* \return TRUE if the camera is active, FALSE otherwise
		*/             
	bool	IsActive(){return mIsActive;}
		
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
		* \fn 		virtual void getRay(const kfloat &ScreenX, const kfloat &ScreenY, Point3D &RayOrigin, Vector3D &RayDirection);
		* \param	ScreenX : position in x axis of the pixel
		* \param	ScreenY : position in y axis of the pixel
		* \param	RayOrigin : origin of the ray (in/out param)
		* \param	RayDirection : direction of the ray (in/out param)
		*/ 
	virtual void getRay(const kfloat &ScreenX, const kfloat &ScreenY, Point3D &RayOrigin, Vector3D &RayDirection);
		
	//! project given point 3D using camera projection
	// return true if point is in front of camera, false if behind it
	virtual bool Project(kfloat &/* ScreenX */, kfloat & /* ScreenY */, Point3D /* Pt */);
		
	/**
		* \brief	do a Node3D cull but always return true
		* \fn 		virtual bool    Cull(TravState* state,unsigned int cullingMask);
		* \param	state : state of the node
		* \return	TRUE
		*/ 
	//virtual bool    Cull(TravState* state,unsigned int cullingMask);
	DECLARE_DECORABLE_DEFINITION(bool, Cull, TravState* state, unsigned int CullingMask);
		
	
		
	inline bool IsEnabled()const {return mCameraIsEnabled;}
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
	RenderingScreen*	getRenderingScreen()
	{
		return (RenderingScreen*)((CoreModifiable*)mRenderingScreen);
	}

	virtual bool	GetDataInTouchSupport(const touchPosInfos& posin, touchPosInfos& pout);

	// test
	//bool			ManageClickTouchEvent(v3f pos, int type, int state, int clickCount);

	virtual std::array<mat4, 2> GetStereoViewProjections() { KIGS_ASSERT(!"Camera doesn't support stereo rendering");  return {}; }

	bool ManageScrollTouchEvent(ScrollEvent& scroll_event);
	bool ManagePinchTouchEvent(PinchEvent& pinch_event);

	auto& GetRenderPasses() { return mRenderPasses; }

protected:

	std::vector<RenderPass> mRenderPasses;
	
	/**
		* \brief	set the ambient light
		* \fn 		virtual void SetAmbient(kfloat r,kfloat g,kfloat b)=0;
		* \param	r : reg color
		* \param	g : green color
		* \param	b : blue color
		*/ 
	//virtual void SetAmbient(kfloat r,kfloat g,kfloat b)=0;
		
	/**
		* \brief	enable the camera protection
		* \fn 		virtual bool ProtectedSetActive(TravState* state)=0;
		* \return	true
		*/ 
	virtual bool ProtectedSetActive(TravState* state)=0;
		
	/**
		* \brief	disable the camera protection
		* \fn 		virtual void ProtectedRelease(TravState* state)=0;
		*/ 
	virtual void ProtectedRelease(TravState* state)=0;
		
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
	//! coordinate on x axis of the min point of the viewport
	maFloat				mViewportMinX;
	//! coordinate on y axis of the min point of the viewport
	maFloat				mViewportMinY;
	//! width of the viewport
	maFloat				mViewportSizeX;
	//! height of the viewport
	maFloat				mViewportSizeY;
		
	//! zNear value
	maFloat				mNearPlane;
	//! zFar value
	maFloat				mFarPlane;
		
	//! position of the camera
	maVect3DF			mPosition;
	//! up vector of the camera
	maVect3DF			mUpVector;
	//! direction of the camera
	maVect3DF			mViewVector;
	//! clear color of the camera
	maVect4DF			mClearColor;
		
		
	//! reference on the rendering screen
	maReference			mRenderingScreen;
		
	//! vertical FOV of the camera
	maFloat				mVerticalFOV;
	//! aspect ration of the camera
	maFloat				mAspectRatio;
		
	//! should clear zBuffer
	maBool				mClearZBuffer;
	//! should clear color buffer
	maBool				mClearColorBuffer;
	//! should clear stencil buffer
	maBool				mClearStencilBuffer;
		
	//! Disable or Enable camera
	maBool				mCameraIsEnabled;
		
	//! Camera priority
	maUInt				mPriority;
		
	//! Camera brightness, change rendering screen brightness
	maFloat		mBrightness;
		
	//! Camera all visible(no cull)
	maBool				mAllVisible;

	maBool				mTouchControlled;
		
	//! TRUE when the camera is active
	bool				mIsActive;

	struct touchControlledDataStruct
	{
		// data
		Point2D		mStartPt; // used for rotation
		kfloat		mOneOnCoef; // used for rotation and zoom
		kfloat		mTargetPointDist; // used for rotation and zoom
		Vector3D	mStartV; // used for rotation and zoom

		// store starting camera data
		Matrix3x4	mStartMatrix;

		unsigned int mState;
	};

	// remove touchControlledDataStruct if needed
	virtual	void UninitModifiable() override;
}; 

#endif //_CAMERA_H_
