#pragma once

#include "CollisionHit.h"
#include "CoreModifiable.h"
#include "TecLibs\Tec3D.h"
#include "maReference.h"
#include "AttributePacking.h"
#include "CoreItem.h"
#include "CoreRawBuffer.h"

#include <mutex>
#include <functional>
#include <atomic>
#include <map>
#include <variant>

#include "blockingconcurrentqueue.h"

namespace Kigs
{
	namespace Maths
	{
		class BBox;
	}
	namespace Scene
	{
		class Node3D;
	}
	namespace Collide
	{
		using namespace Kigs::Core;
		class Plane;
		class BSphere;
		class OctreeSubNode;
		class CollisionBaseObject;

		// ****************************************
		// * CollisionManager class
		// * --------------------------------------
		/**
		* \file	CollisionManager.h
		* \class	CollisionManager
		* \ingroup Collision
		* \brief  Manager class to manage collision.
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
				std::shared_ptr<CollisionBaseObject> mOwnedCollisionObject;
				std::atomic<CollisionBaseObject*> mNodeCollisionObject;

				u32 mUID = 0;
				unsigned int			mCollisionMask = 0;
				bool mNeedRebuild = false;
				CoreModifiable* mItem = nullptr;
			};

			struct SegmentList
			{
				Scene::Node3D* mParentNode;
				std::vector<Maths::Segment3D>	mSegmentList;
			};

			struct PlaneIntersectionInfoStruct
			{
				// per flag map
				std::map<unsigned int, std::vector < SegmentList > > mFoundIntersection;
			};

			DECLARE_CLASS_INFO(CollisionManager, CoreModifiable, Collision)
				DECLARE_CONSTRUCTOR(CollisionManager);
			~CollisionManager() override;

			WRAP_METHODS(OnAddItemCallback, OnRemoveItemCallback, OnDeleteCallBack, GetAllRayIntersection, SerializeAABBTree, DeserializeAABBTree, SetAABBTreeFromFile, ExportLeafAABBTrees, AddSimpleShapeFromDescription);

			bool SerializeAABBTree(CoreRawBuffer* buffer, const CMSP& node);
			bool DeserializeAABBTree(CoreRawBuffer* buffer, const CMSP& node);
			void SetAABBTreeFromFile(const std::string& filename, const CMSP& node);
			void ExportLeafAABBTrees(const CMSP& node, ExportSettings* settings);

			void AddSimpleShapeFromDescription(CoreItem* desc, const CMSP& node);

			enum Categories
			{
				NONE = 0,
				ALL = 0xffffffff
			};

			/*! \brief intersect a ray with all objects in the collision manager
			*/
			bool GetRayIntersection(Maths::Hit& hit, const v3f& start, const Vector3D& dir, unsigned int a_itemCategory = ALL, bool ignore_is_collidable = false);

			/*! \brief intersect a ray with all objects in the collision manager
			*/
			void GetAllRayIntersection(const v3f& start, const Vector3D& dir, std::vector<Maths::Hit>& hits, unsigned int a_itemCategory = ALL, bool ignore_is_collidable = false);

			/*! \brief intersect a plane with all (triangles) objects in the collision manager
			*/
			void GetPlaneIntersection(const v3f& o, const Vector3D& n, PlaneIntersectionInfoStruct& result, unsigned int a_itemCategory = ALL, const Maths::BBox* Zone = nullptr);

			/*! \brief intersect a sphere with all objects in the collision manager
			*/
			CoreModifiable* GetSphereIntersection(const v3f& start, const Vector3D& dir, const float Radius, double& Distance, Vector3D& normal, v3f& intersectP, unsigned int a_itemCategory = ALL);

			/*! \brief return altitude for the current point
			*/
			bool GetAltitude(float x, float y, float z, float& alt);

			/*! \brief set collision mask for a specific mItem
			*/
			void	setCollisionCategories(CoreModifiable* a_Item, unsigned int a_Category = 0);

			/*! \brief set collision mask for all the next items that will be added to the collision manager (should be reset to NONE(0) when we have finish).
			*/
			inline void	setActiveMask(unsigned int a_mask) { mActiveCollisionMask = a_mask; }

			// recursive check on coremodifiable
			void RecursiveCheck(CoreModifiable* item, u32 branchMask, bool maskWasTest);

			void SetCanProcessAddedObjects(bool can_process) { mCanProcessAddedObjects = can_process; }

			MeshCollisionInfo* GetCollisionInfoForObject(CoreModifiable* Object);

			// Will take ownership of collider if collider->mIsCoreModifiable is false
			void SetCollisionObject(const CMSP& item, CollisionBaseObject* collider);
			void SetCollisionObject(const CMSP& item, std::shared_ptr<CollisionBaseObject> collider);

			//const std::unordered_map<WeakRef, std::shared_ptr<MeshCollisionInfo>>& GetCollisionMap() { ProcessPendingItems(); return mCollisionObjectMap; }
			const std::unordered_map<u32, std::shared_ptr<MeshCollisionInfo>>& GetCollisionMap() { ProcessPendingItems(); return mCollisionObjectMap; }

		protected:
			void InitModifiable() override;
			void Update(const Time::Timer& timer, void* addParam) override;

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
				std::weak_ptr<CoreModifiable> mItem;
				u32 mUID;
				std::shared_ptr<MeshCollisionInfo> mInfo;
				std::function<CollisionBaseObject* ()> mFunc;
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
			bool GetLocalRayIntersection(Maths::Hit& hit, const v3f& start, const Vector3D& dir, const CollisionBaseObject* pCollisionObject);
			int  GetAllLocalRayIntersection(std::vector<Maths::Hit>& hit, const v3f& start, const Vector3D& dir, const CollisionBaseObject* pCollisionObject);

			/*! \brief recursive internal method
			*/
			bool GetLocalSphereIntersection(Maths::Hit& hit, const v3f& start, const Vector3D& dir, const float& Radius, CollisionBaseObject* pCollisionObject);

			bool GetLocalCylinderIntersectionWithRay(Maths::Hit& hit, const v3f& start, const Vector3D& dir, const Vector3D& CDir, const float& CHeight, const float& CRadius, CollisionBaseObject* pCollisionObject, unsigned int a_itemCategory = ALL);

			/*! \brief recursive internal method
			*/
			// void	RescursiveSearchMesh(CoreModifiable *currentitem, std::vector<CoreModifiable*>& list);

			/*! \brief recursive internal method
			*/
			bool	RecursiveSearchRayIntersection(CoreModifiable* lastCollideNode, Scene::Node3D* lastNode, CoreModifiable* currentitem, v3f start, Vector3D dir, Maths::Hit& hit, unsigned int lastNodeCategory, unsigned int a_itemCategory = ALL, bool ignore_is_collidable = false);
			void	RecursiveSearchAllRayIntersection(CoreModifiable* lastCollideNode, Scene::Node3D* lastNode, CoreModifiable* currentitem, const v3f& start, const Vector3D& dir, std::vector<Maths::Hit>& hits, unsigned int lastNodeCategory, unsigned int a_itemCategory = ALL, bool ignore_is_collidable = false);
			void	RecursiveSearchPlaneIntersection(Scene::Node3D* lastNode, CoreModifiable* currentitem, const v3f& o, const Vector3D& n, const Maths::BBox* Zone, PlaneIntersectionInfoStruct& result, unsigned int lastNodeCategory, unsigned int a_itemCategory = ALL);


			/*! \brief recursive internal method
			*/
			bool RecursiveSearchSphereIntersection(Maths::Hit& hit, CoreModifiable* currentitem, const v3f& start, const Vector3D& dir, const float& Radius, unsigned int a_itemCategory = ALL);

			/*! \brief recursive internal method
			*/
			CoreModifiable* RecursiveSearchSphereIntersection(OctreeSubNode* currentNode, const v3f& start, const Vector3D& dir, const float& Radius, const v3f& lstart, const Vector3D& ldir, const float& lRadius, double& Distance, Vector3D& normal, v3f& intersectP, bool recurse = true, unsigned int a_itemCategory = ALL);

			/*! \brief recursive internal method
			*/
			bool GetLocalSphereIntersectionWithRay(Maths::Hit& hit, const v3f& start, const Vector3D& dir, const float& Radius, CollisionBaseObject* pCollisionObject, unsigned int a_itemCategory = ALL);

			bool GetLocalPlaneIntersectionWithRay(Maths::Hit& hit, const v3f& start, const Vector3D& dir, const v3f& planePos, const Vector3D& planeNorm, CollisionBaseObject* pCollisionObject);

			//std::unordered_map<WeakRef, std::shared_ptr<MeshCollisionInfo>> mCollisionObjectMap;
			std::unordered_map<u32, std::shared_ptr<MeshCollisionInfo>> mCollisionObjectMap;

			std::mutex mToDeleteMutex;
			std::mutex mToAddMutex;

			using RawOrSharedPtr = std::variant<CollisionBaseObject*, std::shared_ptr<CollisionBaseObject>>;
			std::vector<std::pair<SmartPointer<CoreModifiable>, RawOrSharedPtr>> mToAdd;

			struct ToDelete
			{
				CoreModifiable* mPtr = nullptr;
				u32 mUID = 0;
				std::string mName;
			};
			std::vector<ToDelete> mToDelete;
			//std::atomic_bool mHasDeadRefs{ false };

			unsigned int				mActiveCollisionMask;
		};

	}
}