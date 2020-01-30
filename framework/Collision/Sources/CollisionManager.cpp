#include "CollisionManager.h"
#include "CollisionHit.h"
#include "CoreBaseApplication.h" // for auto update

#include "Intersection.h"
#include "Collision.h"
#include "Core.h"

#include "AABBTree.h"
#include "SpacialMeshBVH.h"
#include "BSphere.h"
#include "BCylinder.h"
#include "Plane.h"
#include "DrawVertice.h"
#include "ModernMesh.h"
#include "ModuleSceneGraph.h"

#include <atomic>

#ifdef KIGS_TOOLS
extern int gCollisionDrawLevel;
#include "GLSLDebugDraw.h"
#include "CoreBaseApplication.h"
const Matrix3x4 * currentNodeMatrix = nullptr;
#endif

IMPLEMENT_CLASS_INFO(CollisionManager);
IMPLEMENT_CONSTRUCTOR(CollisionManager)
, mScene(*this, false, "Scene")
, mActiveCollisionMask(NONE)
, mSceneWasSet(false)
{
	mCollisionObjectMap.clear();
}

CollisionManager::~CollisionManager()
{
	mContinueWork = false;
	mWorkThread.join();
	KigsCore::Instance()->GetInstanceFactory()->removeModifiableCallback("AddItem", this, "OnAddItemCallback");
}

void CollisionManager::InitModifiable()
{
	if (!IsInit())
	{
		KigsCore::GetCoreApplication()->AddAutoUpdate(this);
		CoreModifiable::InitModifiable();

		if (((CoreModifiable*)mScene) != nullptr)
		{
			DoFirstInit();
		}
		else
		{
			mScene.changeNotificationLevel(Owner);
		}

		// add a callback on Node3D addItem method
		KigsCore::Instance()->GetInstanceFactory()->addModifiableCallback("AddItem", this, "OnAddItemCallback","Node3D");

		mContinueWork = true;
		mWorkThread = std::thread{ [&]()
		{
			while (mContinueWork)
			{
				Work w;
				while (mHighPrioWork.try_dequeue(w))
				{
					if (w.item->getRefCount() != 1)
					{
						auto result = w.func();
						if (result)
						{
							if (result->IsCoreModifiable)
							{
								std::shared_ptr<CollisionBaseObject> ptr;
								std::atomic_store(&w.info->myOwnedCollisionObject, ptr);
								w.info->myNodeCollisionObject.exchange(result);
							}
							else
							{
								w.info->myNodeCollisionObject = nullptr;
								std::atomic_exchange(&w.info->myOwnedCollisionObject, std::shared_ptr<CollisionBaseObject>(result));
							}
						}
					}
				}

				if (mWork.try_dequeue(w))
				{
					if (w.item->getRefCount() != 1)
					{
						auto result = w.func();
						if (result)
						{
							if (result->IsCoreModifiable)
							{
								std::shared_ptr<CollisionBaseObject> ptr;
								std::atomic_store(&w.info->myOwnedCollisionObject, ptr);
								w.info->myNodeCollisionObject.exchange(result);
							}
							else
							{
								w.info->myNodeCollisionObject = nullptr;
								std::atomic_exchange(&w.info->myOwnedCollisionObject, std::shared_ptr<CollisionBaseObject>(result));
							}
						}
					}
				}
				else
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(100));
				}
			}
		} };
	}
}

void CollisionManager::DoFirstInit()
{
	if (!mSceneWasSet)
	{
		if (((CoreModifiable*)mScene) != nullptr)
		{
			KigsCore::Connect((CoreModifiable*)mScene, "AddItem", this, "OnAddItemCallback");
			RecursiveCheck(((CoreModifiable*)mScene), 0,false);

			mScene.setReadOnly(true); // the scene can not be changed after this
			mScene.changeNotificationLevel(None);
			mSceneWasSet = true;
		}
	}
}

void CollisionManager::OnRemoveItemCallback(CoreModifiable* localthis, CoreModifiable* item)
{
	UnRegisterObject(localthis, item);
}

void CollisionManager::OnDeleteCallBack(CoreModifiable* localthis)
{
	UnRegisterObject(nullptr, localthis);
}

void CollisionManager::OnAddItemCallback(CoreModifiable* localthis, CoreModifiable* item)
{
	// Only interested in Node3D father with Node3D or Drawable item 
	if (localthis->isUserFlagSet(UserFlagNode3D))
	{
		if (item->isUserFlagSet(UserFlagNode3D) || item->isUserFlagSet(UserFlagDrawable))
		{
			// now I want to be notified when removed
			//KigsCore::Connect(localthis, "RemoveItem", this, "OnRemoveItemCallback");
			RegisterNewObject(localthis, item);
		}
	}
}

void CollisionManager::RegisterNewObject(CoreModifiable* father, CoreModifiable* newitem)
{
	mNeedRecursiveCheck = true;
}

void CollisionManager::UnRegisterObject(CoreModifiable* father, CoreModifiable* removeditem)
{
	std::lock_guard<std::mutex> lock(mToDeleteMutex);
	//kigsprintf("delete callback mesh %u\n", removeditem->getUID());
	mToDelete.push_back({ removeditem, removeditem->getUID(), removeditem->getName() });
	//mHasDeadRefs = true;
}

void CollisionManager::NotifyUpdate(const unsigned int  labelid)
{
	if (labelid == mScene.getLabelID())
	{
		if (((CoreModifiable*)mScene) != nullptr) // the scene is now OK ?
		{
			DoFirstInit();
		}
	}
}

void CollisionManager::ProcessPendingItems()
{
	{
		std::lock_guard<std::mutex> lock(mToAddMutex);
		for (auto&[item, collider] : mToAdd)
		{
			//kigsprintf("adding uid %u to collision map\n", item->getUID());
			auto& info = mCollisionObjectMap[item->getUID()];
			KigsCore::Connect(item.get(), "Destroy", this, "OnDeleteCallBack");
			if (!info) info = std::make_shared<MeshCollisionInfo>();
			info->item = item.get();
			info->uid = item->getUID();
			info->need_rebuild = false;
			if (collider->IsCoreModifiable)
			{
				info->myNodeCollisionObject = collider;
			}
			else
			{
				std::shared_ptr<CollisionBaseObject> ptr(collider);
				std::atomic_store(&info->myOwnedCollisionObject, ptr);
			}
		}
		mToAdd.clear();
	}

	//bool exp = true;
	//if(mHasDeadRefs.compare_exchange_strong(exp, false))
	{
		std::lock_guard<std::mutex> lock(mToDeleteMutex);
		for (auto cm : mToDelete)
		{
			auto it = mCollisionObjectMap.find(cm.uid);
			if (it != mCollisionObjectMap.end())
			{
				if (it->second->uid == cm.uid)
				{
					//kigsprintf("erasing uid %u from collision map\n", cm.second);
					mCollisionObjectMap.erase(it);
				}
			}
		}
		mToDelete.clear();

		/*
		for (auto it = mCollisionObjectMap.begin(); it != mCollisionObjectMap.end();)
		{
			if (!it->first.IsValid())
			{
				it = mCollisionObjectMap.erase(it);
				continue;
			}
			++it;
		}
		*/
	}
}

void CollisionManager::Update(const Timer&  timer, void* addParam)
{
	if (!mSceneWasSet)
	{
		if (((CoreModifiable*)mScene) != nullptr) // the scene is now OK ?
		{
			DoFirstInit();
		}
		return;
	}

	ProcessPendingItems();

	if (mCanProcessAddedObjects)
	{
		bool exp = true;
		if(mNeedRecursiveCheck.compare_exchange_strong(exp, false))
			RecursiveCheck(((CoreModifiable*)mScene), 0,false);
	}
}

void CollisionManager::RecursiveCheck(CoreModifiable* item, u32 branchMask, bool maskWasTest)
{
	u32 mask = 0;
	if (item->isUserFlagSet(UserFlagNode3D))
	{
		if(!maskWasTest)
			if (item->getValue("CollideMask", mask))
				branchMask = mask;

		u32 before = branchMask;

		for (auto& cm : item->getItems())
		{
			if (cm.myItem->getValue("CollideMask", mask))
				branchMask = mask;

			if (branchMask > 0)
				CreateCollisionObject((CoreModifiable*)cm.myItem.get(), branchMask);

			RecursiveCheck((CoreModifiable*)cm.myItem.get(), branchMask,true);
			branchMask = before;
		}
	}
}

CollisionManager::MeshCollisionInfo * CollisionManager::GetCollisionInfoForObject(CoreModifiable* Object)
{
	auto found = mCollisionObjectMap.find(Object->getUID());
	if (found != mCollisionObjectMap.end())
		return &(*found->second);
	return nullptr;
}

bool CollisionManager::CheckType(CoreModifiable* item)
{
	if (item->isUserFlagSet(UserFlagDrawable))
	{
		if ((item->isSubType(ModernMesh::myClassID)) ||
			(item->isSubType(DrawVertice::myClassID)) ||
			(item->isSubType(CollisionBaseNode::myClassID)))
			return true;
	}
	return false;
}

void CollisionManager::SetCollisionObject(const CMSP& item, CollisionBaseObject* collider)
{
	std::lock_guard<std::mutex> lock(mToAddMutex);
	mToAdd.push_back({ item, collider });
}

void CollisionManager::CreateCollisionObject(CoreModifiable* item, unsigned int ColMask)
{
	// check allowed type
	if (!CheckType(item))
		return;

	//! for each item in the list search if this item is already managed by the collision manager
	auto found = mCollisionObjectMap.find(item->getUID());
	if (found == mCollisionObjectMap.end() || found->second->need_rebuild)
	{
		CollisionBaseObject* col = nullptr;
		SmartPointer<CoreModifiable> ref{ item, GetRefTag{} };
		std::function<CollisionBaseObject*()> work;

		if (item->isSubType("ModernMesh"))
		{
			/*auto tree = item->getAttribute("AABBTree");
			if (tree)
			{
				std::string path;
				SmartPointer<CoreRawBuffer> crb;
				if (tree->getType() == STRING)
				{
					tree->getValue(path);
				}
				else
				{
					crb = ((maBuffer*)tree)->const_ref();
					RemoveDynamicAttribute("AABBTree");
				}
				work = [this, ref, tree, path, crb]() mutable -> AABBTreeNode*
				{
					if (path.size())
					{
						SetAABBTreeFromFile(path, ref.get());
					}
					else
					{
						DeserializeAABBTree(crb.get(), this);
					}
					ref.get()->as<ModernMesh>()->SetCanFree();
					return nullptr;
				};
			}
			else*/

			
			if (item->getAttribute("BVH"))
			{
				auto parent_node = item->getFirstParent("Node3D");
				auto mat = parent_node ? parent_node->as<Node3D>()->GetLocalToGlobal() : mat3x4::IdentityMatrix();
				work = [ref, mat]() -> CollisionBaseObject*
				{
					return static_cast<CollisionBaseObject*>(SpacialMeshBVH::BuildFromMesh((ModernMesh*)ref.get(), mat, true));
				};
			}
			else
			{
				work = [ref]() -> CollisionBaseObject*
				{
					return static_cast<CollisionBaseObject*>(AABBTree::BuildFromMesh((ModernMesh*)ref.get()));
				};
			}
		}
		else if (item->isSubType("DrawVertice"))
		{
			work = [ref]()
			{
				DrawVertice * mesh = (DrawVertice*)ref.get();
				
				// extract vertex position
				int vCount, stride, format;
				void *vTmpArray;
				mesh->GetVertexArray(&vTmpArray, vCount, stride, format);

				Point3D *vArray = new Point3D[vCount];
				char * read = (char *)vTmpArray;
				char * write = (char *)vArray;

				// manage interleaving
				if (3 * sizeof(float) < stride)
				{
					for (int i = 0; i < vCount; i++)
					{
						memcpy(write, read, 3 * sizeof(float));

						read += stride;
						write += 3 * sizeof(float);
					}
				}
				else
				{
					memcpy(write, read, 3 * sizeof(float)*vCount);
				}

				int iCount;
				void *iUntypedArray;
				mesh->GetIndexArray(&iUntypedArray, iCount, stride, format);

				AABBTree * node = new AABBTree(iCount / 3);
				node->BuildFromTriangleList(vArray, vCount, iUntypedArray, iCount / 3);

				return node;
			};
		}
		else // CollisionBaseNode
		{
			col = static_cast<CollisionBaseNode*>(item);
		}
		
		if (col || work)
		{
			//! else, construct its AABBTree if it is a mesh and add it to the collision map
			std::shared_ptr<MeshCollisionInfo> newinfo;
			
			if (found != mCollisionObjectMap.end())
			{
				newinfo = found->second;
			}
			else
			{
				newinfo = mCollisionObjectMap[item->getUID()] = std::make_shared<MeshCollisionInfo>();
			}

			newinfo->uid = item->getUID();
			newinfo->myCollisionMask = ColMask;
			newinfo->item = item;
			/*if (newinfo->myCollisionObject && col)
			{
				delete newinfo->myCollisionObject;
				newinfo->myCollisionObject = col;
			}*/
			if (col)
			{
				if (col->IsCoreModifiable)
				{
					newinfo->myNodeCollisionObject = col;
				}
				else
				{
					auto ref = std::shared_ptr<CollisionBaseObject>{ col };
					std::atomic_store(&newinfo->myOwnedCollisionObject, ref);
				}
			}
			newinfo->need_rebuild = false;

			//register on delete event
			KigsCore::Connect(item, "Destroy", this, "OnDeleteCallBack");
			
			if (work)
			{
				Work w{ item, item->getUID(), newinfo, work };


				if (false)
				{
					auto result = w.func();
					if (result)
					{
						if (result->IsCoreModifiable)
						{
							std::shared_ptr<CollisionBaseObject> ptr;
							std::atomic_store(&w.info->myOwnedCollisionObject, ptr);
							w.info->myNodeCollisionObject.exchange(result);
						}
						else
						{
							w.info->myNodeCollisionObject = nullptr;
							std::atomic_exchange(&w.info->myOwnedCollisionObject, std::shared_ptr<CollisionBaseObject>(result));
						}
					}
				}
				else
				{
					auto n = item->getFirstParent("Node3D");
					bool allow = false;
					if (n && n->getValue("HighPriorityCollider", allow) && allow)
						mHighPrioWork.enqueue(w);
					else
						mWork.enqueue(w);
				}
			}
		}
	}
	else
	{
	}
}

/*!
search an intersection between a Ray (point and direction), and all the objects in the manager
call the recursive method : RecursiveSearchRayIntersection
*/
bool CollisionManager::GetRayIntersection(Hit &hit, const Point3D &start, const Vector3D &dir, unsigned int a_itemCategory, bool ignore_is_collidable)
{
	ProcessPendingItems();

	Node3D * node = (Node3D*)((CoreModifiable*)mScene);
	if (RecursiveSearchRayIntersection(nullptr, node, node, start, dir, hit, 0, a_itemCategory, ignore_is_collidable))
	{
#ifdef KIGS_TOOLS
		if (gCollisionDrawLevel > 0)
		{
			dd::sphere(hit.HitPosition, Point3D(255, 255, 0), 0.01f);
			dd::line(hit.HitPosition, hit.HitPosition + (hit.HitNormal*0.1f), Point3D(0, 0, 255));
			hit.HitCollisionObject->DrawDebug(hit.HitPosition, &hit.HitNode->GetLocalToGlobal(), KigsCore::GetCoreApplication()->GetApplicationTimer().get());
		}
#endif

		return true;
	}

	return false;
}

/*!
search an intersection between a Ray (point and direction), and all the objects in the manager
call the recursive method : RecursiveSearchRayIntersection
*/
void CollisionManager::GetAllRayIntersection(const Point3D &start, const Vector3D &dir, std::vector<Hit>& hits, unsigned int a_itemCategory, bool ignore_is_collidable)
{
	ProcessPendingItems();

	Node3D * node = (Node3D*)((CoreModifiable*)mScene);
	RecursiveSearchAllRayIntersection(nullptr, node, node, start, dir, hits, 0, a_itemCategory, ignore_is_collidable);

	for (auto & hit : hits)
	{
#ifdef KIGS_TOOLS
		if (gCollisionDrawLevel > 0)
		{
			dd::sphere(hit.HitPosition, Point3D(255, 255, 0), 0.01f);
			dd::line(hit.HitPosition, hit.HitPosition + (hit.HitNormal*0.1f), Point3D(0, 0, 255));
		}
#endif
	}
	std::sort(hits.begin(), hits.end(), [](const Hit& hita, const Hit& hitb) { return hita.HitDistance < hitb.HitDistance; });

#ifdef KIGS_TOOLS
	if (gCollisionDrawLevel > 0 && !hits.empty())
	{
		auto& hit = *hits.begin();
		dd::sphere(hit.HitPosition, Point3D(255, 255, 0), 0.01f);
		dd::line(hit.HitPosition, hit.HitPosition + (hit.HitNormal*0.1f), Point3D(0, 0, 255));

		hit.HitCollisionObject->DrawDebug(hit.HitPosition, &hit.HitNode->GetLocalToGlobal(), KigsCore::GetCoreApplication()->GetApplicationTimer().get());
	}
#endif
}

/*! intersect a plane with all (triangles) objects in the collision manager
*/
void CollisionManager::GetPlaneIntersection(const Point3D& o, const Vector3D& n, PlaneIntersectionInfoStruct& result, unsigned int a_itemCategory,const BBox* Zone)
{
	ProcessPendingItems();

	Node3D * node = (Node3D*)((CoreModifiable*)mScene);
	result.mFoundIntersection.clear();
	RecursiveSearchPlaneIntersection( node, node, o, n, Zone,result, 0, a_itemCategory);
}

void CollisionManager::setCollisionCategories(CoreModifiable* a_Item, unsigned int a_Category)
{
	if (mCollisionObjectMap.find(a_Item->getUID()) != mCollisionObjectMap.end())
		mCollisionObjectMap[a_Item->getUID()]->myCollisionMask = a_Category;
}

/*!
recurse in the Node3D hierarchy to find meshes and BSpheres.
this method is recursive

void CollisionManager::RescursiveSearchMesh(CoreModifiable *currentitem, std::vector<CoreModifiable*>& list)
{
	if ((currentitem->isSubType(Mesh::myClassID)) ||
		(currentitem->isSubType(ModernMeshItemGroup::myClassID)) ||
		(currentitem->isSubType(DrawVertice::myClassID)) ||
		(currentitem->isSubType(Plane::myClassID)) ||
		(currentitem->isSubType(BSphere::myClassID)))
	{
		list.push_back(currentitem);
	}
	else if (currentitem->isUserFlagSet(UserFlagNode3D) || currentitem->isSubType(ModernMesh::myClassID))
	{
		bool needCollision = true;

		//! recurse only if NeedCollision flag is true
		CoreModifiableAttribute *pNeedCollisionAttribute = getAttribute("NeedCollision");
		if (pNeedCollisionAttribute)
			pNeedCollisionAttribute->getValue(needCollision);
		if (needCollision)
		{
			std::vector<ModifiableItemStruct>::const_iterator it;
			for (it = currentitem->getItems().begin(); it != currentitem->getItems().end(); ++it)
			{
				RescursiveSearchMesh((*it).myItem, list);
			}
		}
	}
}*/

/*!
search an intersection between a Ray (point and direction), and the current CoreModifiable
this method is recursive
*/
bool CollisionManager::RecursiveSearchRayIntersection(CoreModifiable* lastCollideNode, Node3D* lastNode, CoreModifiable *item, Point3D start, Vector3D dir, Hit &hit, unsigned int lastNodeCategory, unsigned int a_itemCategory, bool ignore_is_collidable)
{
	int mask = 0;
	bool hasCat = false;

	//! on Node3D, mark as last seen node au recurse to sons
	// check collision on BBox
	if (item->isUserFlagSet(UserFlagNode3D))
	{
		bool retVal = false;
		lastNode = (Node3D*)item;

		if (!ignore_is_collidable && !lastNode->IsCollidable())
			return false;

		BBox box;
		lastNode->GetGlobalBoundingBox(box.m_Min, box.m_Max);
		//printf("%s [%0.1f %0.1f %0.1f][%0.1f %0.1f %0.1f]\n", lastNode->getName().c_str(), min.x, min.y, min.z, max.x, max.y, max.z);

		if (!item->getItems().empty() && Intersection::IntersectionRayBBox(start, dir, box.m_Min, box.m_Max))
		{
			hasCat = item->getValue("CollideMask", mask);
			if (hasCat)
			{
				if (mask == 0)
					return false;
				lastCollideNode = item;
			}

			for (auto & son : item->getItems())
				retVal |= RecursiveSearchRayIntersection(lastCollideNode, lastNode,(CoreModifiable*) son.myItem.get(), start, dir, hit, (hasCat) ? mask : lastNodeCategory, a_itemCategory, ignore_is_collidable);
		}
		return retVal;
	}


	hasCat = item->getValue("CollideMask", mask);
	if (hasCat)
	{
		if (!(mask & a_itemCategory))
			return false;
	}
	else
	{
		mask = lastNodeCategory;
		if (!(lastNodeCategory & a_itemCategory))
			return false;
	}

	if (!CheckType(item))
		return false;

	double startDist = hit.HitDistance;

	// retreive the associate CollisionObject
	auto L_ItMap = mCollisionObjectMap.find(item->getUID());
	if (L_ItMap != mCollisionObjectMap.end()/* && L_ItMap->first.IsValid() && L_ItMap->second->uid == item->getUID()*/)
	{
		auto ref = std::atomic_load(&L_ItMap->second->myOwnedCollisionObject);
		auto collision_object = ref ? ref.get() : L_ItMap->second->myNodeCollisionObject.load();

		if (!collision_object || !collision_object->IsActive)
			return false;

		Vector3D ldir(dir);
		Point3D	 lstart(start);

		//! transform Ray in local mesh coordinate system if needed				
		const Matrix3x4& inverseMatrix = lastNode->GetGlobalToLocal();
		inverseMatrix.TransformVector(&ldir);
		inverseMatrix.TransformPoint(&lstart);

#ifdef KIGS_TOOLS
		currentNodeMatrix = &lastNode->GetLocalToGlobal();
#endif

		double lCurrentDist = hit.HitDistance;
		MeshCollisionInfo& colObj = *L_ItMap->second;

		auto setHit = [&](Hit &hit)
		{
			lastNode->GetLocalToGlobal().TransformVector(&hit.HitNormal);

			Vector3D v = ldir * hit.HitDistance;
			lastNode->GetLocalToGlobal().TransformVector(&v);
			hit.HitDistance = Norm(v);
			hit.HitPosition = start + dir * hit.HitDistance;

			hit.HitNormal.Normalize();
			hit.HitActor = item;
			hit.HitCollisionObject = collision_object;
			hit.HitNode = lastNode;
			hit.HitFlag = mask;
			hit.HitFlagNode = lastCollideNode;
		};

		//! for each son of the current item, if the son is a Mesh 
		if (item->isSubType("ModernMesh")
			|| item->isSubType("DrawVertice"))
		{
			//! then check intersection between ray (local) and mesh
			if (GetLocalRayIntersection(hit, lstart, ldir, collision_object))
			{
				//! if intersection found, go back to global coordinates and test if this intersection is the best one (nearest)
				/*lastNode->GetLocalToGlobal().TransformVector(&hit.HitNormal);
				hit.HitNormal.Normalize();
				hit.HitActor = item;
				hit.HitCollisionObject = colObj.myCollisionObject;
				hit.HitNode = lastNode;*/
				setHit(hit);
			}
		}
		else if (item->isSubType("CollisionBaseNode"))
		{
			if (item->as<CollisionBaseNode>()->TestHit(hit, lstart, ldir))
			{
				setHit(hit);
			}
		}
		else if (item->isSubType(BSphere::myClassID))
		{
			BSphere * sphere = static_cast<BSphere*>(item);
			kfloat L_Radius = sphere->GetRadius();

			if (GetLocalSphereIntersectionWithRay(hit, lstart, ldir, L_Radius, collision_object))
			{
				//! if intersection found, go back to global coordinates and test if this intersection is the best one (nearest)
				setHit(hit);
			}
		}
		else if (item->isSubType(BCylinder::myClassID))
		{
			BCylinder * sphere = static_cast<BCylinder*>(item);
			kfloat L_Radius = sphere->GetRadius();

			if (GetLocalSphereIntersectionWithRay(hit, lstart, ldir, L_Radius, collision_object))
			{
				//! if intersection found, go back to global coordinates and test if this intersection is the best one (nearest)
				setHit(hit);
			}
		}
		else if (item->isSubType(Plane::myClassID))
		{
			Plane * plane = (Plane*)item;

			Point3D		planePos;
			Vector3D	planeNorm;
			plane->GetPlane(planePos, planeNorm);

			if (GetLocalPlaneIntersectionWithRay(hit, lstart, ldir, planePos, planeNorm, collision_object))
			{
				//! if intersection found, go back to global coordinates and test if this intersection is the best one (nearest)
				hit.HitNormal = -planeNorm;
				setHit(hit);
			}
			else // unvalidate point
			{
				hit.HitDistance = lCurrentDist;
			}
		}
		// we hit something before ray end
		if (startDist > hit.HitDistance)
		{
			return true;
		}
	}

	return false;
}

/*!
search an intersection between a Ray (point and direction), and the current CoreModifiable
this method is recursive
*/
void CollisionManager::RecursiveSearchAllRayIntersection(CoreModifiable* lastCollideNode, Node3D* lastNode, CoreModifiable *item, const Point3D &start, const Vector3D &dir, std::vector<Hit> & hits, unsigned int lastNodeCategory, unsigned int a_itemCategory, bool ignore_is_collidable)
{
	int mask = 0;
	bool hasCat = false;

	//! on Node3D, mark as last seen node au recurse to sons
	// check collision on BBox
	if (item->isUserFlagSet(UserFlagNode3D))
		//if (item->isSubType("Node3D"))
	{
		lastNode = (Node3D*)item;
		if (!ignore_is_collidable && !lastNode->IsCollidable())
			return;

		BBox box;
		lastNode->GetGlobalBoundingBox(box.m_Min, box.m_Max);
		if (!item->getItems().empty() && Intersection::IntersectionRayBBox(start, dir, box.m_Min, box.m_Max))
		{
			hasCat = item->getValue("CollideMask", mask);
			if (hasCat)
				lastCollideNode = item;

			for (auto & son : item->getItems())
			{
				RecursiveSearchAllRayIntersection(lastCollideNode, lastNode, (CoreModifiable*)son.myItem.get(), start, dir, hits, (hasCat) ? mask : lastNodeCategory, a_itemCategory, ignore_is_collidable);
			}
			return;
		}
	}

	if (!CheckType(item))
		return;


	hasCat = item->getValue("CollideMask", mask);
	if (hasCat)
	{
		if (!(mask & a_itemCategory))
			return;
	}
	else
	{
		mask = lastNodeCategory;
		if (!(lastNodeCategory & a_itemCategory))
			return;
	}


	// retreive the associate CollisionObject
	auto L_ItMap = mCollisionObjectMap.find(item->getUID());
	if (L_ItMap != mCollisionObjectMap.end()/* && L_ItMap->first.IsValid() && L_ItMap->second->uid == item->getUID()*/)
	{
		auto ref = std::atomic_load(&L_ItMap->second->myOwnedCollisionObject);
		auto collision_object = ref ? ref.get() : L_ItMap->second->myNodeCollisionObject.load();

		if (!collision_object || !collision_object->IsActive)
			return;

		Vector3D ldir(dir);
		Point3D	 lstart(start);

		//! transform Ray in local mesh coordinate system if needed				
		const Matrix3x4& inverseMatrix = lastNode->GetGlobalToLocal();
		inverseMatrix.TransformVector(&ldir);
		inverseMatrix.TransformPoint(&lstart);
		ldir.Normalize();

#ifdef KIGS_TOOLS
		currentNodeMatrix = &lastNode->GetLocalToGlobal();
#endif

		MeshCollisionInfo& colObj = *L_ItMap->second;

		auto setHit = [&](Hit &hit)
		{
			lastNode->GetLocalToGlobal().TransformVector(&hit.HitNormal);
			hit.HitNormal.Normalize();

			Vector3D v = ldir*hit.HitDistance;
			lastNode->GetLocalToGlobal().TransformVector(&v);
			hit.HitDistance = Norm(v);
			hit.HitPosition = start + dir * hit.HitDistance;

			hit.HitActor = item;
			hit.HitCollisionObject = collision_object;
			hit.HitNode = lastNode;
			hit.HitFlagNode = lastCollideNode;
			hit.HitFlag = mask;
		};
		
		//! for each son of the current item, if the son is a Mesh 
		if (item->isSubType("ModernMesh")
			|| item->isSubType("DrawVertice"))
		{
			std::vector<Hit> tmp;
			if (GetAllLocalRayIntersection(tmp, lstart, ldir, collision_object))
			{
				for (auto h : tmp)
				{
					//! if intersection found, go back to global coordinates and test if this intersection is the best one (nearest)
					setHit(h);
					hits.push_back(h);
				}
			}
		}
		else if (item->isSubType("CollisionBaseNode"))
		{
			Hit hit;
			if (item->as<CollisionBaseNode>()->TestHit(hit, lstart, ldir))
			{
				setHit(hit);
				hits.push_back(hit);
			}
		}
		else if (item->isSubType("BCylinder"))
		{
			BCylinder * cylinder = static_cast<BCylinder*>(item);
			kfloat L_Radius = cylinder->GetRadius();
			kfloat L_Height = cylinder->GetHeight();
			Vector3D Cdir;
			cylinder->GetAxle(Cdir);

			Hit hit;
			//hit.HitFlag = lastNodeCategory;
			if (GetLocalCylinderIntersectionWithRay(hit, lstart, ldir, Cdir, L_Height, L_Radius, collision_object))
			{
				//! if intersection found, go back to global coordinates and test if this intersection is the best one (nearest)
				setHit(hit);
				hits.push_back(hit);
			}
		}
		else if (item->isSubType("BSphere"))
		{
			BSphere * sphere = static_cast<BSphere*>(item);
			kfloat L_Radius = sphere->GetRadius();

			Hit hit;
			//hit.HitFlag = lastNodeCategory;
			if (GetLocalSphereIntersectionWithRay(hit, lstart, ldir, L_Radius, collision_object))
			{
				//! if intersection found, go back to global coordinates and test if this intersection is the best one (nearest)
				setHit(hit);
				hits.push_back(hit);
			}
		}
		else if (item->isSubType("Plane"))
		{
			Plane * plane = (Plane*)item;

			Point3D		planePos;
			Vector3D	planeNorm;
			plane->GetPlane(planePos, planeNorm);

			Hit hit;
			//hit.HitFlag = lastNodeCategory;
			if (GetLocalPlaneIntersectionWithRay(hit, lstart, ldir, planePos, planeNorm, collision_object))
			{
				//! if intersection found, go back to global coordinates and test if this intersection is the best one (nearest)
				hit.HitNormal = -planeNorm;
				setHit(hit);
				hits.push_back(hit);
			}
		}
	}
	else
	{
		//kigsprintf("CollisionObject not found for %s\n", item->getName().c_str());
	}
}

void CollisionManager::RecursiveSearchPlaneIntersection(Node3D* lastNode, CoreModifiable *item, const Point3D &o, const Vector3D &n, const BBox* Zone, PlaneIntersectionInfoStruct& result, unsigned int lastNodeCategory, unsigned int a_itemCategory)
{
	int mask = 0;
	bool hasCat = false;

	

	//! on Node3D, mark as last seen node au recurse to sons
	// check collision on BBox
	if (item->isUserFlagSet(UserFlagNode3D))
		//if (item->isSubType("Node3D"))
	{
		lastNode = (Node3D*)item;
		if (!lastNode->IsCollidable())
			return;

		const BBox& box = lastNode->GetGlobalBoundingBox();

		if (Zone)
		{
			if (!Zone->DoesIntersect(box))
			{
				return;
			}
		}
		
		if (!item->getItems().empty() && Intersection::IntersectionPlaneBBox(o, n, box))
		{
			hasCat = item->getValue("CollideMask", mask);

			for (auto & son : item->getItems())
			{
				RecursiveSearchPlaneIntersection(lastNode, (CoreModifiable*)son.myItem.get(), o, n, Zone,result, (hasCat) ? mask : lastNodeCategory, a_itemCategory);
			}
			return;
		}
	}

	if (!CheckType(item))
		return;


	hasCat = item->getValue("CollideMask", mask);
	if (hasCat)
	{
		if (!(mask & a_itemCategory))
			return;
	}
	else
	{
		mask = lastNodeCategory;
		if (!(lastNodeCategory & a_itemCategory))
			return;
	}

	

	// retreive the associate CollisionObject
	auto L_ItMap = mCollisionObjectMap.find(item->getUID());
	if (L_ItMap != mCollisionObjectMap.end()/* && L_ItMap->first.IsValid() && L_ItMap->second->uid == item->getUID()*/)
	{
		auto ref = std::atomic_load(&L_ItMap->second->myOwnedCollisionObject);
		auto collision_object = ref ? ref.get() : L_ItMap->second->myNodeCollisionObject.load();

		if (!collision_object || !collision_object->IsActive)
			return;

		Vector3D lnorm(n);
		Point3D	 lorig(o);

		//! transform Ray in local mesh coordinate system if needed				
		const Matrix3x4& inverseMatrix = lastNode->GetGlobalToLocal();
		inverseMatrix.TransformVector(&lnorm);
		inverseMatrix.TransformPoint(&lorig);
		lnorm.Normalize();

		int planeIndex = 4;
		// know check if lnorm is axis aligned
		if (fabsf(lnorm.x) > 0.99999f)
		{
			planeIndex = 0;
		}
		else if (fabsf(lnorm.y) > 0.99999f)
		{
			planeIndex = 1;
		}
		else if (fabsf(lnorm.z) > 0.99999f)
		{
			planeIndex = 2;
		}


#ifdef KIGS_TOOLS
		currentNodeMatrix = &lastNode->GetLocalToGlobal();
#endif

		MeshCollisionInfo& colObj = *L_ItMap->second;


		//! for each son of the current item, if the son is a Mesh 
		if (item->isSubType("ModernMesh")
			|| item->isSubType("DrawVertice"))
		{
			kstl::vector<Segment3D>	tmpResult;

			bool found = false;

			switch (planeIndex)
			{
			case 0:
			{
				found = Intersection::IntersectionAxisAlignedPlaneAABBTree<0>(lorig.x, *static_cast<const AABBTree*>(collision_object), tmpResult);
			}
			break;
			case 1:
			{
				found = Intersection::IntersectionAxisAlignedPlaneAABBTree<1>(lorig.y, *static_cast<const AABBTree*>(collision_object), tmpResult);
			}
			break;
			case 2:
			{
				found = Intersection::IntersectionAxisAlignedPlaneAABBTree<2>(lorig.z, *static_cast<const AABBTree*>(collision_object), tmpResult);
			}
			break;
			default:
				found = Intersection::IntersectionPlaneAABBTree(lorig,lnorm, *static_cast<const AABBTree*>(collision_object), tmpResult);
			}
			if(found && tmpResult.size())
			{
				
				Point3D*	toConvertBack = tmpResult[0].pts;
				int convertCount = tmpResult.size() * 2; // 2 point per segment

				lastNode->GetLocalToGlobal().TransformPoints(toConvertBack, convertCount);

				SegmentList	resultSL;
				resultSL.mParentNode = lastNode;

				result.mFoundIntersection[mask].push_back(resultSL);
				result.mFoundIntersection[mask].back().mSegmentList = std::move(tmpResult);
			}
		}
		else if (item->isSubType("CollisionBaseNode"))
		{
			// DO Nothing here
		}
		else if (item->isSubType("BCylinder"))
		{
			// DO Nothing here
		}
		else if (item->isSubType("BSphere"))
		{
			// Do Nothing here
		}
		else if (item->isSubType("Plane"))
		{
			// Do Nothing here
		}
	}
	else
	{
		//kigsprintf("CollisionObject not found for %s\n", item->getName().c_str());
	}
}




/*!
compute "altitude" = vertical distance from the given point to any object in the collision manager
*/
bool CollisionManager::GetAltitude(kfloat x, kfloat y, kfloat z, kfloat &alt)
{
	Point3D start(x, y, z);
	Vector3D dir(0, 0, -1.0f);

	Hit hit;
	if (GetRayIntersection(hit, start, dir))
	{
		alt = z - hit.HitPosition.z;

		return true;
	}

	return false;

}

/*!
special GetAltitude method, used as a maMethod (can be called using its name and a list of params)
compute "altitude" = vertical distance from the given point to any object in the collision manager
*/
DEFINE_METHOD(CollisionManager, GetAltitude)
{
	CoreModifiableAttribute*	xparam = 0;
	CoreModifiableAttribute*	yparam = 0;
	CoreModifiableAttribute*	zparam = 0;
	CoreModifiableAttribute*	altparam = 0;
	CoreModifiableAttribute*	normalparam = 0;

	std::vector<CoreModifiableAttribute*>::const_iterator ci;
	for (ci = params.begin(); ci != params.end(); ++ci)
	{
		CoreModifiableAttribute* current = (*ci);
		if (current->getLabel() == "x")
		{
			xparam = current;
		}
		else if (current->getLabel() == "y")
		{
			yparam = current;
		}
		else if (current->getLabel() == "z")
		{
			zparam = current;
		}
		else if (current->getLabel() == "alt")
		{
			altparam = current;
		}
		else if (current->getLabel() == "normal")
		{
			normalparam = current;
		}
	}

	//! the param list must at least have a "x", "y" and "alt" CoreModifiableParameter
	if ((xparam == 0) || (yparam == 0) || (altparam == 0))
	{
		return false;
	}

	kfloat x, y, z;

	xparam->getValue(x);
	yparam->getValue(y);

	//! if no "z" param is given, then set z to 1000
	if (zparam)
	{
		zparam->getValue(z);
	}
	else
	{
		z = 1000.0f;
	}

	Point3D start(x, y, z);
	Vector3D dir(0, 0, -1.0f);

	Hit hit;

	if (GetRayIntersection(hit, start, dir))
	{
		altparam->setValue(z - hit.HitPosition.z);

		//! if a "normal" param is given, then set normal values
		if (normalparam)
		{
			normalparam->setArrayElementValue(hit.HitNormal.x, 0, 0);
			normalparam->setArrayElementValue(hit.HitNormal.y, 0, 1);
			normalparam->setArrayElementValue(hit.HitNormal.z, 0, 2);
		}

		return true;
	}

	return false;
}

DEFINE_METHOD(CollisionManager, GetIntersection)
{
	//! <b>TODO NOT IMPLEMENTED</b>
	return false;
}

/*!
compute intersection between a moving sphere and all the objects in the collision manager
return the nearest intersection if at least one is found
*/
CoreModifiable* CollisionManager::GetSphereIntersection(const Point3D& start, const Vector3D& dir, const kfloat Radius, kdouble &Distance, Vector3D& normal, Point3D& intersectP, unsigned int a_itemCategory)
{
	Distance = 1000.0f;

	//! first compute moving sphere BBox in local space
	BBox	MovingSphereBBox;

	Vector3D ldir(dir);
	Point3D	 lstart(start);
	Vector3D radiusV(Radius, 0, 0);
	kfloat lradius;

	//! ensure the node is up to date (matrix, bbox...)
#ifdef REWORKING
	SetupNodeIfNeeded();
	const Matrix3x4& inverseMatrix = GetGlobalToLocal();
	const Matrix3x4& LToGMatrix = GetLocalToGlobal();

	inverseMatrix.TransformVector(&ldir);
	inverseMatrix.TransformPoint(&lstart);
	inverseMatrix.TransformVector(&radiusV);
#endif
	lradius = Norm(radiusV);

	if (ldir.x >= 0.0f)
	{
		MovingSphereBBox.m_Max.x = lstart.x + ldir.x + lradius;
		MovingSphereBBox.m_Min.x = lstart.x - lradius;
	}
	else
	{
		MovingSphereBBox.m_Max.x = lstart.x + lradius;
		MovingSphereBBox.m_Min.x = lstart.x + ldir.x - lradius;
}
	if (ldir.y >= 0.0f)
	{
		MovingSphereBBox.m_Max.y = lstart.y + ldir.y + lradius;
		MovingSphereBBox.m_Min.y = lstart.y - lradius;
	}
	else
	{
		MovingSphereBBox.m_Max.y = lstart.y + lradius;
		MovingSphereBBox.m_Min.y = lstart.y + ldir.y - lradius;
	}
	if (ldir.z >= 0.0f)
	{
		MovingSphereBBox.m_Max.z = lstart.z + ldir.z + lradius;
		MovingSphereBBox.m_Min.z = lstart.z - lradius;
	}
	else
	{
		MovingSphereBBox.m_Max.z = lstart.z + lradius;
		MovingSphereBBox.m_Min.z = lstart.z + ldir.z - lradius;
	}

#ifdef REWORKING
	//! search octree node where to start our search (a node containing our moving sphere BBox)  
	bool found = true;
	OctreeSubNode* search = myRootSubNode;

	kdouble ldist = 1000.0;
	Vector3D lnormal;
	Point3D lpoint;
	CoreModifiable* foundone = NULL;

	while (found)
	{
		//! if one of the octree node son contains the box 
		int result = search->FindSubNode(&MovingSphereBBox);
		if (result == -1)
		{
			found = false;
		}
		else
		{
			//! call the recursive method : RecursiveSearchSphereIntersection 
			CoreModifiable* L_TmpIntersect = RecursiveSearchSphereIntersection(search, start, dir, Radius, lstart, ldir, lradius, ldist, lnormal, lpoint, false, a_itemCategory);
			if (L_TmpIntersect)
			{
				if (ldist < Distance)
				{
					normal = lnormal;
					Distance = ldist;
					intersectP = lpoint;
					foundone = L_TmpIntersect;
				}
			}
			search = search->GetSubNode(result);
		}
	}

	//! else search in current (father) son
	CoreModifiable* L_TmpIntersect = RecursiveSearchSphereIntersection(search, start, dir, Radius, lstart, ldir, lradius, ldist, lnormal, lpoint, true, a_itemCategory);
	if (L_TmpIntersect)
	{
		if (ldist < Distance)
		{
			normal = lnormal;
			Distance = ldist;
			intersectP = lpoint;
			foundone = L_TmpIntersect;
		}
	}
	return foundone;
#else 
	return nullptr;
#endif
}

/*!
Recursive method to search for intersection of a moving sphere in an octree
*/
CoreModifiable*	CollisionManager::RecursiveSearchSphereIntersection(OctreeSubNode* currentNode, const Point3D& start, const Vector3D& dir, const kfloat &Radius, const Point3D& lstart, const Vector3D& ldir, const kfloat &lRadius, kdouble &Distance, Vector3D& normal, Point3D& intersectP, bool recurse, unsigned int a_itemCategory)
{
#ifdef REWORKING
	std::vector<SceneNode*>::const_iterator it;

	CoreModifiable* foundone = NULL;
	for (it = currentNode->GetObjectList().begin(); it != currentNode->GetObjectList().end(); ++it)
	{
		//! for each object in this OctreeSubNode
		if ((*it)->isSubType(Node3D::myClassID))
		{
			Hit hit;
			hit.HitDistance = 1000;
			//! call recursive search for Node3D
			if (RecursiveSearchSphereIntersection(hit, *it, start, dir, Radius, a_itemCategory))
			{
				if (hit.HitDistance < Distance)
				{
					normal = hit.HitNormal;
					Distance = hit.HitDistance;
					intersectP = hit.HitPosition;
					foundone = *it;
				}
			}
		}
		else if ((*it)->isSubType(Mesh::myClassID))
		{
			std::map<CoreModifiable*, MeshCollisionInfo>::iterator L_ItMap = mCollisionObjectMap.find((Mesh*)(*it));
			if (L_ItMap != mCollisionObjectMap.end())
			{
				unsigned int L_ItemCat = L_ItMap->second.myCollisionMask;
				if (a_itemCategory != ALL && L_ItemCat != ALL)
				{
					if (!(L_ItemCat & a_itemCategory))
					{
						/*Distance = Bestdist;*/
						//return foundone;
						continue;
					}
				}

				Hit hit;
				hit.HitDistance = 1000;
				//! or call intersection search for Mesh
				if (GetLocalSphereIntersection(hit, lstart, ldir, lRadius, mCollisionObjectMap[(*it)].myCollisionObject))
				{
					if (hit.HitDistance < Distance)
					{
#ifdef REWORKING
						const Matrix3x4& LToGMatrix = GetLocalToGlobal();
						LToGMatrix.TransformVector(&hit.HitNormal);
						LToGMatrix.TransformPoint(&hit.HitPosition);
#endif
						Distance = hit.HitDistance;
						intersectP = hit.HitPosition;
						foundone = (*it);
					}
				}
			}
		}
	}

	//! then recurse to son nodes			
	if (recurse)
		if (currentNode->HasSons())
		{
			int i;
			for (i = 0; i < 8; i++)
			{
				kdouble ldist = 1000.0;
				Vector3D lnormal;
				Point3D lpoint;

				CoreModifiable* L_TmpIntersect = RecursiveSearchSphereIntersection(currentNode->GetSubNode(i), start, dir, Radius, lstart, ldir, lRadius, ldist, lnormal, lpoint, true, a_itemCategory);
				if (L_TmpIntersect)
				{
					if (ldist < Distance)
					{
						normal = lnormal;
						Distance = ldist;
						intersectP = lpoint;
						foundone = L_TmpIntersect;
					}
				}
			}
		}
	return foundone;
#endif
	return NULL;
}

/*!
Recursive method to search for intersection of a moving sphere in an Node3D
*/
bool CollisionManager::RecursiveSearchSphereIntersection(Hit &hit, CoreModifiable *currentitem, const Point3D& start, const Vector3D& dir, const kfloat &Radius, unsigned int a_itemCategory)
{
	assert(0);
	return false;
#if 0
	CoreModifiable*	foundone = NULL;

	if (currentitem->isUserFlagSet(UserFlagNode3D))
	{
		//! first ensure node is up to date
		((Node3D*)currentitem)->SetupNodeIfNeeded();

		bool localTransformHasBeenDone = false;

		Vector3D ldir(dir);
		Point3D	 lstart(start);
		Vector3D radiusV(Radius, 0, 0);
		kfloat lradius;

		std::vector<ModifiableItemStruct>::const_iterator it;
		for (it = currentitem->getItems().begin(); it != currentitem->getItems().end(); ++it)
		{
			CoreModifiable* item = (*it).myItem;
			//! then for each son 
			if (item->isSubType(Mesh::myClassID))
			{
				//! if son is a Mesh, call GetLocalSphereIntersection
				const Matrix3x4& LToGMatrix = ((Node3D*)currentitem)->GetLocalToGlobal();

				auto L_ItMap = mCollisionObjectMap.find(item);

				if (L_ItMap != mCollisionObjectMap.end())
				{
					if (!localTransformHasBeenDone)
					{
						const Matrix3x4& inverseMatrix = ((Node3D*)currentitem)->GetGlobalToLocal();

						inverseMatrix.TransformVector(&ldir);
						inverseMatrix.TransformPoint(&lstart);
						inverseMatrix.TransformVector(&radiusV);
						lradius = Norm(radiusV);
						localTransformHasBeenDone = true;
					}

					unsigned int L_ItemCat = L_ItMap->second.myCollisionMask;
					if (a_itemCategory != ALL && L_ItemCat != ALL)
					{
						if (!(L_ItemCat & a_itemCategory))
						{
							/*Distance = Bestdist;*/
							//return foundone;
							continue;
						}
					}

					if (GetLocalSphereIntersection(hit, lstart, ldir, lradius, mCollisionObjectMap[item].myCollisionObject))
					{
						LToGMatrix.TransformVector(&hit.HitNormal);
						LToGMatrix.TransformPoint(&hit.HitPosition);
						foundone = item;
					}
				}
			}
			else if (item->isUserFlagSet(UserFlagNode3D))
			{
				//! else if son is a Node3D, recurse
				if (RecursiveSearchSphereIntersection(hit, item, start, dir, Radius, a_itemCategory))
				{
					foundone = item;
				}
			}
		}
	}

	return foundone != nullptr;
#endif
}

/*!
Utility method used by recursive intersection search
*/
bool CollisionManager::GetLocalSphereIntersection(Hit &hit, const Point3D& start, const Vector3D& dir, const kfloat &Radius, CollisionBaseObject* pCollisionObject)
{
	assert(0); // kmesh is deprecated
	//return Collision::CollideSphereAABBTreeNode(start, dir, Radius, *(AABBTreeNode*)pCollisionObject, (Mesh*)pObject, lDist, lNormal, lpoint);
	return false;
}


/*!
compute intersection between a ray and a mesh in the local mesh coordinate system
*/
bool CollisionManager::GetLocalRayIntersection(Hit &hit, const Point3D& start, const Vector3D& dir, const CollisionBaseObject* pCollisionObject)
{
	return pCollisionObject->CallLocalRayIntersection(hit, start, dir);
}

int CollisionManager::GetAllLocalRayIntersection(std::vector<Hit> &hit, const Point3D& start, const Vector3D& dir, const CollisionBaseObject* pCollisionObject)
{
	pCollisionObject->CallLocalRayIntersection(hit, start, dir);
	return hit.size();
}

bool CollisionManager::GetLocalSphereIntersectionWithRay(Hit &hit, const Point3D& start, const Vector3D& dir, const kfloat &Radius, CollisionBaseObject* pCollisionObject, unsigned int a_itemCategory)
{
	// call the Intersection class test on Sphere
	return Collision::CollideRaySphere(start, dir, Radius, hit.HitDistance, hit.HitNormal);
}

bool CollisionManager::GetLocalCylinderIntersectionWithRay(Hit &hit, const Point3D& start, const Vector3D& dir, const Vector3D& CDir, const kfloat &CHeight, const kfloat& CRadius, CollisionBaseObject* pCollisionObject, unsigned int a_itemCategory)
{
	// call the Intersection class test on Cylinder
	return Collision::CollideRayCylinder(start, dir, CDir, CHeight, CRadius, hit.HitDistance, hit.HitNormal);
}

bool CollisionManager::GetLocalPlaneIntersectionWithRay(Hit &hit, const Point3D& start, const Vector3D& dir, const Point3D& planePos, const Vector3D& planeNorm, CollisionBaseObject* pCollisionObject)
{
	// call the Intersection class test on Sphere
	return Intersection::IntersectRayPlane(start, dir, planePos, planeNorm, hit.HitDistance, pCollisionObject);
}

static void compare_tree(AABBTreeNode* a, AABBTreeNode* b)
{
	if (a->GetTriangleCount() != b->GetTriangleCount()
		|| a->GetBBox() != b->GetBBox()

		)
	{
		int zae = 0;
	}

	if (a->Son1)
	{
		if (!b->Son1)
		{
			int aze = 0;
		}
		compare_tree(a->Son1, b->Son1);
		compare_tree(a->Son2, b->Son2);
	}
	else
	{
		if (memcmp(a->TriangleArray2, b->TriangleArray2, a->GetTriangleCount() * 3 * 4) != 0)
		{
			int aze = 0;
		}
	}
}

bool CollisionManager::SerializeAABBTree(CoreRawBuffer* buffer, const CMSP& node)
{
	auto it = mCollisionObjectMap.find(node->getUID());
	if (it == mCollisionObjectMap.end()) return false;

	//printf("PACK\n");
	std::shared_ptr<CollisionBaseObject> aabbtree_ref = std::atomic_load(&it->second->myOwnedCollisionObject);
	auto aabbtree = (AABBTree*)aabbtree_ref.get();
	if (!aabbtree) return false;

	std::vector<u32> data;
	VectorWriteStream stream{ data };
	bool b1 = serialize_object(stream, *aabbtree);
	stream.Flush();
	buffer->resize(data.size() * sizeof(u32));
	memcpy(buffer->data(), data.data(), data.size() * sizeof(u32));
	
	/*
	printf("UNPACK\n");
	AABBTree* aabbtree_read = new AABBTree;
	PacketReadStream stream_reader{ (u32*)buffer->data(), buffer->size() };
	bool b2 = serialize_object(stream_reader, *aabbtree_read);
	printf("\n");


	std::vector<u32> data2;
	VectorWriteStream stream2{ data2 };
	bool b3 = serialize_object(stream2, *aabbtree_read);
	stream2.Flush();
	
	int aze = data.size() != data2.size() ? 1 : memcmp(data.data(), data2.data(), data.size()*sizeof(u32));
	if (aze != 0)
	{
		int azee = 0;
	}
	compare_tree(aabbtree, aabbtree_read);*/
	return true;
}

bool CollisionManager::DeserializeAABBTree(CoreRawBuffer* buffer, const CMSP& node)
{
	AABBTree* aabbtree = new AABBTree;
	PacketReadStream stream_reader{ (u32*)buffer->data(), buffer->size() };
	serialize_object(stream_reader, *aabbtree);
	SetCollisionObject(node, aabbtree);
	return true;
}

void CollisionManager::SetAABBTreeFromFile(const std::string& filename, const CMSP& node)
{
	AABBTree* aabbtree = new AABBTree;
	aabbtree->LoadFromFile(filename);
	SetCollisionObject(node, aabbtree);
}