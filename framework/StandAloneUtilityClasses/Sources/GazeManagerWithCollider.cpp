#include "GazeManagerWithCollider.h"
#include "Camera.h"
#include "Plane.h"
#include "Node3D.h"
#include "AttributePacking.h"

IMPLEMENT_CLASS_INFO(ColliderGazeManager);

IMPLEMENT_CONSTRUCTOR(ColliderGazeManager)
, myCollisionManager(*this, true, "CollisionManager")
{
}

bool ColliderGazeManager::GetHit(Hit &gaze_hit, double dist, u32 mask)
{
	if (myIsRecursive)
	{
		bool found=false;
		myHit.Clear();
		for (auto& hit : myHitList)
		{
			if (hit.HitDistance <= dist || (hit.HitFlag & mask)==0)
				continue;

			gaze_hit = hit;
			return true;
		}
	}
	else
	{
		gaze_hit = myHit;
		return true;
	}
	return false;
}

int ColliderGazeManager::GetHitsInRange(kstl::vector<Hit>& hits, double distBegin, double distEnd, u32 mask)
{
	if (myIsRecursive)
	{
		bool found = false;
		for (auto& hit : myHitList)
		{
			if((hit.HitFlag & mask) == 0 
			|| (hit.HitDistance <= distBegin)
			|| (hit.HitDistance >= distEnd))
				continue;

			hits.push_back(hit);
		}
		return hits.size();
	}
	return 0;
}

//#define SHOW_HIT
#ifdef SHOW_HIT
#include "GLSLDebugDraw.h"
#endif

void ColliderGazeManager::Update(const Timer&  timer, void* addParam)
{
	return;

	Point3D pos;
	Vector3D norm;

	// retreive ray from cam
	Node3D * lGazeFb = (Node3D*)(CoreModifiable*)myGazeFeedback;
	//Camera * lGazeCam = (Camera*)(CoreModifiable*)myGazeCamera;

	if ((CoreModifiable*)myGazeCamera || mUseMotionControllerAsGaze)
	{
		Point3D rayPos, result;
		Vector3D rayDir, normal, camUp;
		GetGazeRay(rayPos, rayDir, camUp);

		//lGazeCam->GetPosition(rayPos.x, rayPos.y, rayPos.z);
		//lGazeCam->GetViewVector(rayDir.x, rayDir.y, rayDir.z);
		//lGazeCam->GetUpVector(camUp.x, camUp.y, camUp.z);

		myHit.Clear();

		bool valid_hit_func = HasMethod("ValidHit");

		if (myIsRecursive)
		{
			myHitList.clear();
			static_cast<CollisionManager*>(myCollisionManager)->GetAllRayIntersection(rayPos, rayDir, myHitList);
			bool has_hit = false;
			if (myHitList.size() > 0)
			{
				if (lGazeFb)
				{
					Hit* hitp = nullptr;
					for (auto& hit : myHitList)
					{
						if (valid_hit_func && !SimpleCall<bool>("ValidHit", rayPos, rayDir, hit)) continue;
						hitp = &hit;
						has_hit = true;
						break;
					}
					
					if (hitp)
					{
						Matrix3x4 mat;

						Vector3D tmp, tmp2;
						auto normal = mFixedNormal ? -rayDir : hitp->HitNormal;
						tmp.CrossProduct(camUp, normal);
						tmp2.CrossProduct(normal, tmp);
						tmp.Normalize();
						tmp2.Normalize();
						mat.Set(tmp, tmp2, normal, hitp->HitPosition - rayDir*0.01f);
						lGazeFb->ChangeMatrix(mat);
						//lGazeFb->setValue("Show", true);
					}
				}
			}
			
			if (lGazeFb && !has_hit)
			{
				Matrix3x4 mat;
				Vector3D tmp;
				tmp.CrossProduct(camUp, -rayDir);
				mat.Set(tmp, camUp, -rayDir, rayPos + rayDir*1.5f);

				lGazeFb->ChangeMatrix(mat);
				//lGazeFb->SetVisible(false);
			}
		}
		else
		{
			Hit hit;
			bool has_hit = static_cast<CollisionManager*>(myCollisionManager)->GetRayIntersection(hit, rayPos, rayDir);
			if (has_hit && valid_hit_func && !SimpleCall<bool>("ValidHit", rayPos, rayDir, hit))
				has_hit = false;
			
			if (has_hit)
			{
				myHit = hit;

#ifdef SHOW_HIT
				dd::sphere(myHit->HitPosition, Vector3D(1, 1, 0), .1f);
				dd::line(myHit->HitPosition, myHit->HitPosition + myHit->HitNormal, Vector3D(1, 1, 0));
#endif
				if (lGazeFb)
				{
					Matrix3x4 mat;

					Vector3D tmp, tmp2;
					tmp.CrossProduct(camUp, myHit.HitNormal);
					tmp2.CrossProduct(myHit.HitNormal, tmp);
					tmp.Normalize();
					tmp2.Normalize();
					mat.Set(tmp, tmp2, myHit.HitNormal, myHit.HitPosition - rayDir*0.01f);

					lGazeFb->ChangeMatrix(mat);

					//lGazeFb->setValue("Show", true);
				}
			}
			else if (lGazeFb)
			{
				Matrix3x4 mat;
				Vector3D tmp;
				tmp.CrossProduct(camUp, -rayDir);
				mat.Set(tmp, camUp, -rayDir, rayPos + rayDir * 1.5f);
				lGazeFb->ChangeMatrix(mat);
				//lGazeFb->setValue("Show", false);
			}
		}
	}
}