#ifndef _COLLISIONMANAGER_H_
#define _COLLISIONMANAGER_H_

#include "CollisionHit.h"
#include "CoreModifiable.h"
#include "TecLibs\Tec3D.h"
#include "maReference.h"
//#include "AABBTree.h"
#include "AttributePacking.h"

#include <mutex>
#include <functional>
#include <atomic>

#include "blockingconcurrentqueue.h"

class Plane;
class BBox;
class Node3D;
class BSphere;
class OctreeSubNode;
class CollisionBaseObject;

// ****************************************
// * CollisionManager class
// * --------------------------------------
/*!  \class CollisionManager
	 this class is the collision manager class, based on an Octree for optimisation
	 \ingroup Collision
*/
// ****************************************

class CollisionManager : public CoreModifiable
{
public:
	/*! \class MeshCollisionInfo
	utility class for the collision map
	*/
	struct MeshCollisionInfo
	{
		std::shared_ptr<CollisionBaseObject> myOwnedCollisionObject;
		std::atomic<CollisionBaseObject*> myNodeCollisionObject;

		u32 uid = 0;
		unsigned int			myCollisionMask = 0;
		bool need_rebuild = false;
		CoreModifiable* item = nullptr;
	};

	struct SegmentList
	{
		Node3D*					mParentNode;
		kstl::vector<Segment3D>	mSegmentList;
	};

	struct PlaneIntersectionInfoStruct
	{
		// per flag map
		kstl::map<unsigned int, kstl::vector < SegmentList > > mFoundIntersection;
	};

	DECLARE_CLASS_INFO(CollisionManager, CoreModifiable, Collision)
	DECLARE_CONSTRUCTOR(CollisionManager);
	WRAP_METHODS(OnAddItemCallback, OnRemoveItemCallback, OnDeleteCallBack, GetAllRayIntersection, SerializeAABBTree, DeserializeAABBTree, SetAABBTreeFromFile);

	bool SerializeAABBTree(CoreRawBuffer* buffer, const CMSP& node);
	bool DeserializeAABBTree(CoreRawBuffer* buffer, const CMSP& node);
	void SetAABBTreeFromFile(const std::string& filename, const CMSP& node);

	enum Categories
	{
		NONE = 0,
		ALL = 0xffffffff
	};

	/*! \brief intersect a ray with all objects in the collision manager
	*/
	bool GetRayIntersection(Hit &hit, const Point3D &start, const Vector3D &dir, unsigned int a_itemCategory = ALL, bool ignore_is_collidable = false);

	/*! \brief intersect a ray with all objects in the collision manager
	*/
	void GetAllRayIntersection(const Point3D &start, const Vector3D &dir, std::vector<Hit>& hits, unsigned int a_itemCategory = ALL, bool ignore_is_collidable = false);

	/*! \brief intersect a plane with all (triangles) objects in the collision manager
	*/
	void GetPlaneIntersection(const Point3D& o, const Vector3D& n, PlaneIntersectionInfoStruct& result, unsigned int a_itemCategory = ALL, const BBox* Zone = nullptr);

	/*! \brief intersect a sphere with all objects in the collision manager
	*/
	CoreModifiable* GetSphereIntersection(const Point3D& start, const Vector3D& dir, const kfloat Radius, kdouble &Distance, Vector3D& normal, Point3D& intersectP, unsigned int a_itemCategory = ALL);

	/*! \brief return altitude for the current point
	*/
	bool GetAltitude(kfloat x, kfloat y, kfloat z, kfloat &alt);

	/*! \brief set collision mask for a specific item
	*/
	void	setCollisionCategories(CoreModifiable* a_Item, unsigned int a_Category = 0);

	/*! \brief set collision mask for all the next items that will be added to the collision manager (should be reset to NONE(0) when we have finish).
	*/
	inline void	setActiveMask(unsigned int a_mask) { mActiveCollisionMask = a_mask; }

	// recursive check on coremodifiable
	void RecursiveCheck(CoreModifiable* item, u32 branchMask, bool maskWasTest);

	void SetCanProcessAddedObjects(bool can_process) { mCanProcessAddedObjects = can_process; }

	MeshCollisionInfo* GetCollisionInfoForObject(CoreModifiable* Object);

	// Will take ownership of collider if collider->IsCoreModifiable is false
	void SetCollisionObject(const CMSP& item, CollisionBaseObject* collider);

	//const std::unordered_map<WeakRef, std::shared_ptr<MeshCollisionInfo>>& GetCollisionMap() { ProcessPendingItems(); return mCollisionObjectMap; }
	const std::unordered_map<u32, std::shared_ptr<MeshCollisionInfo>>& GetCollisionMap() { ProcessPendingItems(); return mCollisionObjectMap; }

protected:
	~CollisionManager() override;
	void InitModifiable() override;
	void Update(const Timer&  timer, void* addParam) override;

	void ProcessPendingItems();

	bool CheckType(CoreModifiable* item);
	void CreateCollisionObject(CoreModifiable* item, unsigned int ColMask);


	std::atomic<bool> mCanProcessAddedObjects{ true };

	maReference mScene;

	void NotifyUpdate(const unsigned int /* labelid */) override;

	// Object add / remove management
	void	DoFirstInit();
	void	OnAddItemCallback(CoreModifiable* localthis, CoreModifiable* item);
	void	OnRemoveItemCallback(CoreModifiable* localthis, CoreModifiable* item);
	void	OnDeleteCallBack(CoreModifiable* item);

	void	RegisterNewObject(CoreModifiable* father, CoreModifiable* newitem);
	void	UnRegisterObject(CoreModifiable* father, CoreModifiable* removeditem);

	std::atomic_bool mNeedRecursiveCheck;

	std::atomic_bool mContinueWork;
	std::thread mWorkThread;

	struct Work
	{
		CoreModifiable* item;
		u32 UID;
		std::shared_ptr<MeshCollisionInfo> info;
		std::function<CollisionBaseObject*()> func;
	};

	moodycamel::BlockingConcurrentQueue<Work> mWork;
	moodycamel::BlockingConcurrentQueue<Work> mHighPrioWork;

	bool		mSceneWasSet;

	DECLARE_METHOD(GetIntersection);
	DECLARE_METHOD(GetAltitude);
	COREMODIFIABLE_METHODS(GetIntersection, GetAltitude);

	/*! \brief recursive internal method
	* return true if itersection found
	*/
	bool GetLocalRayIntersection(Hit &hit, const Point3D& start, const Vector3D& dir, const CollisionBaseObject* pCollisionObject);
	int  GetAllLocalRayIntersection(std::vector<Hit> &hit, const Point3D& start, const Vector3D& dir, const CollisionBaseObject* pCollisionObject);

	/*! \brief recursive internal method
	*/
	bool GetLocalSphereIntersection(Hit &hit, const Point3D& start, const Vector3D& dir, const kfloat &Radius, CollisionBaseObject* pCollisionObject);

	bool GetLocalCylinderIntersectionWithRay(Hit &hit, const Point3D& start, const Vector3D& dir, const Vector3D& CDir, const kfloat &CHeight, const kfloat& CRadius, CollisionBaseObject* pCollisionObject, unsigned int a_itemCategory = ALL);

	/*! \brief recursive internal method
	*/
	// void	RescursiveSearchMesh(CoreModifiable *currentitem, std::vector<CoreModifiable*>& list);

	/*! \brief recursive internal method
	*/
	bool	RecursiveSearchRayIntersection(CoreModifiable* lastCollideNode, Node3D* lastNode, CoreModifiable *currentitem, Point3D start, Vector3D dir, Hit &hit, unsigned int lastNodeCategory, unsigned int a_itemCategory = ALL, bool ignore_is_collidable = false);
	void	RecursiveSearchAllRayIntersection(CoreModifiable* lastCollideNode, Node3D* lastNode, CoreModifiable *currentitem, const Point3D &start, const Vector3D &dir, std::vector<Hit> & hits, unsigned int lastNodeCategory, unsigned int a_itemCategory = ALL, bool ignore_is_collidable = false);
	void	RecursiveSearchPlaneIntersection( Node3D* lastNode, CoreModifiable *currentitem, const Point3D &o, const Vector3D &n, const BBox* Zone, PlaneIntersectionInfoStruct& result, unsigned int lastNodeCategory, unsigned int a_itemCategory = ALL);


	/*! \brief recursive internal method
	*/
	bool RecursiveSearchSphereIntersection(Hit &hit, CoreModifiable *currentitem, const Point3D& start, const Vector3D& dir, const kfloat &Radius, unsigned int a_itemCategory = ALL);

	/*! \brief recursive internal method
	*/
	CoreModifiable*	RecursiveSearchSphereIntersection(OctreeSubNode* currentNode, const Point3D& start, const Vector3D& dir, const kfloat &Radius, const Point3D& lstart, const Vector3D& ldir, const kfloat &lRadius, kdouble &Distance, Vector3D& normal, Point3D& intersectP, bool recurse = true, unsigned int a_itemCategory = ALL);

	/*! \brief recursive internal method
	*/
	bool GetLocalSphereIntersectionWithRay(Hit &hit, const Point3D& start, const Vector3D& dir, const kfloat &Radius, CollisionBaseObject* pCollisionObject, unsigned int a_itemCategory = ALL);

	bool GetLocalPlaneIntersectionWithRay(Hit &hit, const Point3D& start, const Vector3D& dir, const Point3D& planePos, const Vector3D& planeNorm, CollisionBaseObject* pCollisionObject);

	//std::unordered_map<WeakRef, std::shared_ptr<MeshCollisionInfo>> mCollisionObjectMap;
	std::unordered_map<u32, std::shared_ptr<MeshCollisionInfo>> mCollisionObjectMap;

	std::mutex mToDeleteMutex;
	std::mutex mToAddMutex;

	std::vector<std::pair<SmartPointer<CoreModifiable>, CollisionBaseObject*>> mToAdd;
	
	struct ToDelete
	{
		CoreModifiable* ptr = nullptr;
		u32 uid = 0;
		std::string name;
	};
	std::vector<ToDelete> mToDelete;
	//std::atomic_bool mHasDeadRefs{ false };

	unsigned int				mActiveCollisionMask;
};

#endif //_COLLISIONMANAGER_H_