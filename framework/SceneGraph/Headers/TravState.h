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

#ifdef KIGS_TOOLS
	std::vector<std::string> debug_draw_path;
	bool record_pass = false;
#endif
};

/*! \defgroup InternalTravState TravState Internal
* \ingroup SceneGraph
*/

// ****************************************
// * TravState class
// * --------------------------------------
/**
* \file	TravState.h
* \class	TravState
* \ingroup SceneGraph
* \brief	this is the base class containing current traversal state
* \author	ukn
* \version ukn
* \date	ukn
* when culling or drawing scenegraph
* base members are the global LOD, and the global time
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
	void	SetTime(kdouble time){myTime=time;}

	/**
	* \brief	get the tavState time
	* \fn 		kdouble	GetTime()
	* \return	the tavState time
	*/
	kdouble	GetTime(){return myTime;}

	/**
	* \brief	set the camera
	* \fn 		void		SetCurrentCamera(Camera* cam)
	* \param	cam : camera to set
	*/
	void		SetCurrentCamera(Camera* cam){myCamera=cam;}

	/**
	* \brief	get the tavState camera
	* \fn 		Camera*		GetCurrentCamera()
	* \return	the tavState camera
	*/
	Camera*		GetCurrentCamera(){return myCamera;}

	/**
	* \brief	set the LOD
	* \fn 		void	SetLOD(kfloat lod)
	* \param	lod : LOD to set
	*/
	void	SetLOD(kfloat lod){myLOD=lod;}

	/**
	* \brief	get the tavState LOD
	* \fn 		kfloat	GetLOD()
	* \return	the tavState LOD
	*/
	kfloat	GetLOD(){return myLOD;}

	/**
	* \brief	get the culling object
	* \fn 		CullingObject* GetCullingObject() const
	* \return	the culling object
	*/
	SP<CullingObject>& GetCullingObject() {return myCullingObject;}

	/**
	* \brief	get the visible frame
	* \fn 		unsigned int GetVisibilityFrame()
	* \return	index of the visible frame
	*/
	unsigned int GetVisibilityFrame(){return myVisibilityFrame;}

	/**
	* \brief	set a frame visible
	* \fn 		void  SetVisibilityFrame(unsigned int frame)
	* \param	index of the frame to set visible
	*/
	void  SetVisibilityFrame(unsigned int frame){myVisibilityFrame=frame;}

	/**
	* \brief	update the current frame number
	* \fn 		unsigned int BeginNewFrame()
	* \return	the current frame number
	*/
	unsigned int BeginNewFrame() { return ++myFrame; }

	/**
	* \brief	get the current frame number
	* \fn 		unsigned int GetFrameNumber()
	* \return	the current frame number
	*/
	unsigned int GetFrameNumber(){return myFrame;}

	/**
	* \brief	set all frames visibles
	* \fn 		void  SetAllVisible(bool t)
	* \param	t : if TRUE, set all frames visibles
	*/
	void  SetAllVisible(bool t){myAllVisible=t;}

	/**
	* \brief	check if all frames are visibles
	* \fn 		bool  IsAllVisible()
	* \return	TRUE is all frames are visibles
	*/
	bool  IsAllVisible(){return myAllVisible;}

	/**
	* \brief	set the current local to global matrix
	* \fn 		void	SetCurrentLocalToGlobalMatrix(Matrix3x4* m)
	* \param	m : local to global matrix
	*/
	void	SetCurrentLocalToGlobalMatrix(const Matrix3x4& m){myCurrentLocalToGlobalMatrix=m;}

	/**
	* \brief	get the current local to global matrix
	* \fn 		Matrix3x4*  GetCurrentLocalToGlobalMatrix()
	* \return	the current local to global matrix
	*/
	Matrix3x4  GetCurrentLocalToGlobalMatrix(){return myCurrentLocalToGlobalMatrix;}

	/**
	* \brief	set the current global to local matrix
	* \fn 		void	SetCurrentGlobalToLocalMatrix(Matrix3x4* m)
	* \param	m : global to local matrix
	*/
	void	SetCurrentGlobalToLocalMatrix(const Matrix3x4& m){myCurrentGlobalToLocalMatrix=m;}

	/**
	* \brief	get the current global to local matrix
	* \fn 		Matrix3x4*  GetCurrentGlobalToLocalMatrix()
	* \return	the the current global to local matrix
	*/
	Matrix3x4  GetCurrentGlobalToLocalMatrix(){return myCurrentGlobalToLocalMatrix;}

	/**
	* \brief	set the current renderer
	* \fn 		void	SetRenderer(ModuleRenderer* renderer)
	* \param	renderer : the current renderer
	*/
	void	SetRenderer(ModuleSpecificRenderer* renderer){myRenderer=renderer;}

	/**
	* \brief	get the current renderer
	* \fn 		ModuleRenderer*	GetRenderer()
	* \return	the current renderer
	*/
	ModuleSpecificRenderer*	GetRenderer(){return myRenderer;}

	/**
	* \brief	set the current graph scene
	* \fn 		void	SetSceneGraph(ModuleSceneGraph* scenegraph)
	* \param	scenegraph : the current graph scene
	*/
	void	SetSceneGraph(ModuleSceneGraph* scenegraph){mySceneGraph=scenegraph;}

	/**
	* \brief	get the scene graph
	* \fn 		ModuleSceneGraph*	GetSceneGraph()
	* \return	the scene graph
	*/
	inline ModuleSceneGraph*	GetSceneGraph(){return mySceneGraph;}

	/**
	* \brief	set the current scene3D
	* \fn 		void	SetScene(Scene3D* scene)
	* \param	scene : the current scene3D
	*/
	void	SetScene(Scene3D* scene){myCurrentScene=scene;}

	unsigned int GetRenderDisableMask() const { return myRenderDisableMask; }
	void SetRenderDisableMask(unsigned int mask) { myRenderDisableMask = mask; }

	/**
	* \brief	get the scene3D
	* \fn 		Scene3D*	GetScene()
	* \return	the scene3D
	*/
	Scene3D*	GetScene(){return myCurrentScene;}

	//! link to the travPath
	TravPath*		myPath;

	//! change material only when needed
	Material*									myCurrentMaterial;

	//! list of current used light
	kstl::set<CoreModifiable*> *				myLights;

	void	SetHolographicMode(bool isHolo) { myIsHolographic = isHolo; }
	bool	GetHolographicMode() { return myIsHolographic; }

	bool mDrawingInstances = false;
	u32 mInstanceBufferIndex = 0xFFFFFFFF;
	u32 mInstanceCount = 0;
	std::unordered_map<ModernMeshItemGroup*, InstancingData> mInstancing;
	void DrawInstances();

	RenderPass* mCurrentPass = nullptr;

protected:
	/**
	* \brief	destructor
	* \fn 		~TravState();
	*/
	virtual ~TravState();

	//! time
	kdouble	myTime;
	//! LOD value
	kfloat	myLOD;
	//! link to the current camera
	Camera*	myCamera;
	//! link to the current renderer
	ModuleSpecificRenderer*		myRenderer;
	//! link to the current scene graph
	ModuleSceneGraph*	mySceneGraph;
	//! link to the current scene3D
	Scene3D*			myCurrentScene;
	//! culling object
	SP<CullingObject>  myCullingObject;

	//! TRUE if all frames are visibles
	bool  myAllVisible;

	bool myIsHolographic;
	

	//! index of the visible frame
	unsigned int myVisibilityFrame;
	//! index of the current frame
	unsigned int myFrame;
	//! current scale
	kfloat myCurrentScale;

	unsigned int myRenderDisableMask =0;


	Matrix3x4	myCurrentLocalToGlobalMatrix = Matrix3x4::IdentityMatrix();
	Matrix3x4	myCurrentGlobalToLocalMatrix = Matrix3x4::IdentityMatrix();

public:
	//! drawable sorter
	//DrawableSorter *pDrawableSorter;
	float TangentSpaceLOD=-1.0f;
	int OverrideCullMode = -1;
	Node3D* CurrentNode = nullptr;
	bool HolographicUseStackMatrix = false;

	ManageFrontToBackStruct*	pManageFrontToBackStruct;

	u64 mFramesNeededForOcclusion = 1;
};

#endif //_TRAVSTATE_H_
