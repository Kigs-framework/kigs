#ifndef _MODULESCENEGRAPH_H_
#define _MODULESCENEGRAPH_H_

#include "Core.h"
#include "ModuleBase.h"

#include "kstlset.h"
#include "Scene3D.h"


/*! \defgroup SceneGraph 
 *  Manage 3D scenes.
*/




class TravState;

class		ModuleSpecificRenderer;

struct DefferedAction{
	enum ENUM{
		NONE,
		REMOVE_CAMERA,
		ADD_CAMERA,
		REMOVE_LIGHT,
		ADD_LIGHT,
		// destroy rendering state objects
		DESTROY_SHADER,
		DESTROY_PROGRAM,
		DESTROY_TEXTURE,
		DESTROY_BUFFER,
	};
};

// fast type check


	/**
	 * \brief		max visible node
	 * \def 		MAX_VISIBLE_NODES
	 */
#define		MAX_VISIBLE_NODES	(1<<16)

// ****************************************
// * Scene3DPriorityCompare class
// * --------------------------------------
/**
 * \file	ModuleSceneGraph.h
 * \class	Scene3DPriorityCompare
 * \ingroup SceneGraph
 * \brief	Struct to sort scenes.
 */
 // ****************************************
struct Scene3DPriorityCompare
{
	//! overload operator () for comparison
	bool operator()(Scene3D * a1, Scene3D * a2) const
	{
		if(a1->getPriority()==a2->getPriority())
			return (*a1).getUID()>(*a2).getUID();
		return a1->getPriority()>a2->getPriority();
	}
};

// ****************************************
// * ModuleSceneGraph class
// * --------------------------------------
/**
 * \file	ModuleSceneGraph.h
 * \class	ModuleSceneGraph
 * \ingroup SceneGraph
 * \ingroup Module
 * \brief	Manage all Scenes ( 3D and 2D ).
 */
 // ****************************************
class ModuleSceneGraph : public ModuleBase
{
public:
	//! declare module  
	DECLARE_CLASS_INFO(ModuleSceneGraph,ModuleBase,SceneGraph)
	DECLARE_CONSTRUCTOR(ModuleSceneGraph);
	
	void Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params) override;
	void Close() override;
	void Update(const Timer& timer, void* addParam) override;


	ModuleSpecificRenderer*	GetRenderer(){return mRenderer;}


	bool addItem(const CMSP& item, ItemPosition pos=Last DECLARE_DEFAULT_LINK_NAME) override;
	bool removeItem(const CMSP& item DECLARE_DEFAULT_LINK_NAME) override;

	/**
	 * \brief	retreive current visible Node Index
	 * \fn 		inline unsigned int	GetCurrentVisibleNodeIndex() const
	 * \return	the current visible node
	 */
	inline unsigned int	GetCurrentVisibleNodeIndex() const { return mCurrentVisibleNodeIndex; }

	/**
	 * \brief	add a visible node
	 * \fn 		inline void	AddVisibleNode(Node3D* node)
	 * \param	node : visible node to add
	 */
	inline void	AddVisibleNode(Node3D* node)
	{
		mVisiblesNodeList[mCurrentVisibleNodeIndex]=node;
		mCurrentVisibleNodeIndex++;
#ifdef	_DEBUG
		if(mCurrentVisibleNodeIndex>MAX_VISIBLE_NODES)
		{
			printf("Error current index : %u put by : %s \n",mCurrentVisibleNodeIndex,node->getName().c_str());
			KIGS_ASSERT(0);
		}
#endif
	}

	/**
	 * \brief	retreive a visible Node Index
	 * \fn 		inline Node3D* GetVisibleNode(unsigned int index) const
	 * \return	index of the visible node
	 */
	inline Node3D* GetVisibleNode(unsigned int index) const
	{
		return mVisiblesNodeList[index];
	}
	inline Node3D** GetVisibleNodeArray(unsigned int index)
	{
		return &mVisiblesNodeList[index];
	}

	/**
	 * \brief	reset visible node
	 * \fn 		inline void ResetVisibleNodeList()
	 */
	inline void ResetVisibleNodeList()
	{
		mCurrentVisibleNodeIndex=0;
	}

	void AddDefferedItem(void* item, DefferedAction::ENUM action);
	void NotifyDefferedItemDeath(CoreModifiable* item);
	void SignalLightChange(CoreModifiable* l);

	// Useless legacy stuff
	void AddPostManip(SceneNode* node, unsigned int type) {}
	void TreatNeedUpdateList() {}
	void AddNeedUpdate(Node3D* node) {}
	void RemoveNeedUpdate(Node3D* node) {}
	//

protected:
	void DoDefferedAction();
	bool FindParentScene(CoreModifiable* item, Scene3D** parent, bool checkSceneNode=false );
	bool RemoveFromParentScene(CoreModifiable* item);
	


	virtual ~ModuleSceneGraph();


	void	SortSceneList();

	//! current state for the culling or drawing
	SP<TravState>	mTravState = nullptr;

	//! renderer used by the module
	ModuleSpecificRenderer*	mRenderer;

	kigs::unordered_map<void*, DefferedAction::ENUM> mDefferedAction;

	//! list of scene
	kstl::set<Scene3D*,Scene3DPriorityCompare>	mScenes;
	//! visibles 3D nodes, the liste is setup during the travcull for each nodes
	Node3D*				mVisiblesNodeList[MAX_VISIBLE_NODES];

	//! index of the current visible node
	unsigned int		mCurrentVisibleNodeIndex;

	//! TRUE if scene list need to be sorted
	maBool				mSceneListNeedsSort;

	std::recursive_mutex mMutex;
}; 

#endif //_MODULESCENEGRAPH_H_
