#ifndef _TRAVSTATE_H_
#define _TRAVSTATE_H_

#include "CoreModifiable.h"
#include "SceneGraphDefines.h"
#include "TecLibs/3D/Matrix3x4.h"

//#include "DrawableSorter.h"

class DrawableSorter;
class CullingObject;
struct Matrix3x4;
class Camera;
class ModuleSpecificRenderer;
class ModuleSceneGraph;
class TravPath;
class Zone;
class Material;
class Scene3D;
class Node3D;


class ShaderBase;
class ModernMeshItemGroup;

class ManageFrontToBackStruct;

struct InstancingData
{
	ShaderBase* shader = nullptr;
	u32 shader_variant = (u32)-1;
	s32 priority = 0;
	std::vector<Matrix3x4> transforms;
};

/*
TODO(antoine):
Possible things to move to RenderPass:

default DepthTest value (currently in camera, and always true)
default DepthMask value ?

*/

struct RenderPass
{
	u32 pass_mask = 0;
	bool use_front_to_back_sorter = false;
	bool clear_depth = false;
	bool write_depth = true;
	bool allow_instancing = true;
	DrawableSorter* sorter = nullptr;
	int depth_buffer_index = 0;

	KigsID name;

#ifdef KIGS_TOOLS
	struct DrawPathElement
	{
		CMSP Object;
		enum class Step
		{
			PreDraw,
			Draw,
			PostDraw
		} DrawStep = Step::PreDraw;
	};
	std::vector<DrawPathElement> debug_draw_path;
	bool record_pass = false;
#endif
};


// ****************************************
// * TravState class
// * --------------------------------------
/**
 * \file	TravState.h
 * \class	TravState
 * \ingroup SceneGraph
 * \brief  Base class used to store current scene traversal state.
 */
 // ****************************************
class TravState : public CoreModifiable
{
public:
	friend class Scene3D;
	DECLARE_CLASS_INFO(TravState,CoreModifiable,SceneGraph)   

	/**
	* \brief	constructor
	* \fn 		TravState(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	* \param	name : instance name
	* \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	*/
	TravState(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	/**
	* \brief	set the tavState time
	* \fn 		void	SetTime(kdouble time)
	* \param	time : time to set
	*/
	void	SetTime(kdouble time){mTime=time;}

	/**
	* \brief	get the tavState time
	* \fn 		kdouble	GetTime()
	* \return	the tavState time
	*/
	kdouble	GetTime(){return mTime;}

	/**
	* \brief	set the camera
	* \fn 		void		SetCurrentCamera(Camera* cam)
	* \param	cam : camera to set
	*/
	void		SetCurrentCamera(Camera* cam){mCamera=cam;}

	/**
	* \brief	get the tavState camera
	* \fn 		Camera*		GetCurrentCamera()
	* \return	the tavState camera
	*/
	Camera*		GetCurrentCamera(){return mCamera;}

	/**
	* \brief	set the LOD
	* \fn 		void	SetLOD(kfloat lod)
	* \param	lod : LOD to set
	*/
	void	SetLOD(kfloat lod){mLOD=lod;}

	/**
	* \brief	get the tavState LOD
	* \fn 		kfloat	GetLOD()
	* \return	the tavState LOD
	*/
	kfloat	GetLOD(){return mLOD;}

	/**
	* \brief	get the culling object
	* \fn 		CullingObject* GetCullingObject() const
	* \return	the culling object
	*/
	SP<CullingObject> GetCullingObject() {return mCullingObject;}

	/**
	* \brief	get the visible frame
	* \fn 		unsigned int GetVisibilityFrame()
	* \return	index of the visible frame
	*/
	unsigned int GetVisibilityFrame(){return mVisibilityFrame;}

	/**
	* \brief	set a frame visible
	* \fn 		void  SetVisibilityFrame(unsigned int frame)
	* \param	index of the frame to set visible
	*/
	void  SetVisibilityFrame(unsigned int frame){mVisibilityFrame=frame;}

	/**
	* \brief	update the current frame number
	* \fn 		unsigned int BeginNewFrame()
	* \return	the current frame number
	*/
	unsigned int BeginNewFrame() { return ++mFrame; }

	/**
	* \brief	get the current frame number
	* \fn 		unsigned int GetFrameNumber()
	* \return	the current frame number
	*/
	unsigned int GetFrameNumber(){return mFrame;}

	/**
	* \brief	set all frames visibles
	* \fn 		void  SetAllVisible(bool t)
	* \param	t : if TRUE, set all frames visibles
	*/
	void  SetAllVisible(bool t){mAllVisible=t;}

	/**
	* \brief	check if all frames are visibles
	* \fn 		bool  IsAllVisible()
	* \return	TRUE is all frames are visibles
	*/
	bool  IsAllVisible(){return mAllVisible;}

	/**
	* \brief	set the current local to global matrix
	* \fn 		void	SetCurrentLocalToGlobalMatrix(Matrix3x4* m)
	* \param	m : local to global matrix
	*/
	void	SetCurrentLocalToGlobalMatrix(const Matrix3x4& m){mCurrentLocalToGlobalMatrix=m;}

	/**
	* \brief	get the current local to global matrix
	* \fn 		Matrix3x4*  GetCurrentLocalToGlobalMatrix()
	* \return	the current local to global matrix
	*/
	Matrix3x4  GetCurrentLocalToGlobalMatrix(){return mCurrentLocalToGlobalMatrix;}

	/**
	* \brief	set the current global to local matrix
	* \fn 		void	SetCurrentGlobalToLocalMatrix(Matrix3x4* m)
	* \param	m : global to local matrix
	*/
	void	SetCurrentGlobalToLocalMatrix(const Matrix3x4& m){mCurrentGlobalToLocalMatrix=m;}

	/**
	* \brief	get the current global to local matrix
	* \fn 		Matrix3x4*  GetCurrentGlobalToLocalMatrix()
	* \return	the the current global to local matrix
	*/
	Matrix3x4  GetCurrentGlobalToLocalMatrix(){return mCurrentGlobalToLocalMatrix;}

	/**
	* \brief	set the current renderer
	* \fn 		void	SetRenderer(ModuleRenderer* renderer)
	* \param	renderer : the current renderer
	*/
	void	SetRenderer(ModuleSpecificRenderer* renderer){mRenderer=renderer;}

	/**
	* \brief	get the current renderer
	* \fn 		ModuleRenderer*	GetRenderer()
	* \return	the current renderer
	*/
	ModuleSpecificRenderer*	GetRenderer(){return mRenderer;}

	/**
	* \brief	set the current graph scene
	* \fn 		void	SetSceneGraph(ModuleSceneGraph* scenegraph)
	* \param	scenegraph : the current graph scene
	*/
	void	SetSceneGraph(ModuleSceneGraph* scenegraph){mSceneGraph=scenegraph;}

	/**
	* \brief	get the scene graph
	* \fn 		ModuleSceneGraph*	GetSceneGraph()
	* \return	the scene graph
	*/
	inline ModuleSceneGraph*	GetSceneGraph(){return mSceneGraph;}

	/**
	* \brief	set the current scene3D
	* \fn 		void	SetScene(Scene3D* scene)
	* \param	scene : the current scene3D
	*/
	void	SetScene(Scene3D* scene){mCurrentScene=scene;}

	unsigned int GetRenderDisableMask() const { return mRenderDisableMask; }
	void SetRenderDisableMask(unsigned int mask) { mRenderDisableMask = mask; }

	/**
	* \brief	get the scene3D
	* \fn 		Scene3D*	GetScene()
	* \return	the scene3D
	*/
	Scene3D*	GetScene(){return mCurrentScene;}

	//! link to the travPath
	TravPath*		mPath;

	//! change material only when needed
	Material*									mCurrentMaterial;

	//! list of current used light
	kstl::set<CoreModifiable*>*				mLights;

	void	SetHolographicMode(bool isHolo) { mIsHolographic = isHolo; }
	bool	GetHolographicMode() { return mIsHolographic; }

	bool mDrawingInstances = false;
	u32 mInstanceBufferIndex = 0xFFFFFFFF;
	u32 mInstanceCount = 0;
	std::unordered_map<ModernMeshItemGroup*, InstancingData> mInstancing;
	void DrawInstances();

	RenderPass* mCurrentPass = nullptr;

	/**
	* \brief	destructor
	* \fn 		~TravState();
	*/
	virtual ~TravState();

protected:
	//! time
	kdouble	mTime;
	//! LOD value
	kfloat	mLOD;
	//! link to the current camera
	Camera*	mCamera;
	//! link to the current renderer
	ModuleSpecificRenderer*		mRenderer;
	//! link to the current scene graph
	ModuleSceneGraph*	mSceneGraph;
	//! link to the current scene3D
	Scene3D*			mCurrentScene;
	//! culling object
	SP<CullingObject>  mCullingObject;

	//! TRUE if all frames are visibles
	bool  mAllVisible;

	bool mIsHolographic;
	

	//! index of the visible frame
	unsigned int mVisibilityFrame;
	//! index of the current frame
	unsigned int mFrame;
	//! current scale
	kfloat mCurrentScale;

	unsigned int mRenderDisableMask =0;


	Matrix3x4	mCurrentLocalToGlobalMatrix = Matrix3x4::IdentityMatrix();
	Matrix3x4	mCurrentGlobalToLocalMatrix = Matrix3x4::IdentityMatrix();

public:
	//! drawable sorter
	//DrawableSorter *pDrawableSorter;
	float mTangentSpaceLOD=-1.0f;
	int mOverrideCullMode = -1;
	Node3D* mCurrentNode = nullptr;
	bool mHolographicUseStackMatrix = false;

	ManageFrontToBackStruct*	mManageFrontToBackStruct;

	u64 mFramesNeededForOcclusion = 1;
};

#endif //_TRAVSTATE_H_
