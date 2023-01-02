#pragma once

#include <TecLibs\Tec3D.h>
#include <TecLibs\Math\IntersectionAlgorithms.h>
#include <vector>

namespace Kigs
{
	namespace Time
	{
		class Timer;
	}
	namespace Core
	{
		class CoreModifiable;
	}
	namespace Collide
	{
		class AABBTree;
		class SpacialMeshBVH;


		// ****************************************
		// * CollisionBaseObject class
		// * --------------------------------------
		/**
		* \file	CollisionBaseObject.h
		* \class	CollisionBaseObject
		* \ingroup Collision
		* \brief  Base class for collision object (sphere, box, mesh...)
		*/
		// ****************************************

		class CollisionBaseObject
		{
		public:
			CollisionBaseObject();

			virtual ~CollisionBaseObject() {}

			virtual bool ValidHit(const v3f& pos) { return true; }

			virtual bool TestHit(Maths::Hit& hit, v3f local_origin, v3f local_direction) { return false; }

			virtual bool CallLocalRayIntersection(Maths::Hit& hit, const v3f& start, const Maths::Vector3D& dir)  const = 0;
			virtual bool CallLocalRayIntersection(std::vector<Maths::Hit>& hit, const v3f& start, const Maths::Vector3D& dir)  const = 0;

#ifdef KIGS_TOOLS
			// debug color used for this node
			v3f mDebugColor;

			// draw debug mInfo using GLSLDrawDebug
			virtual void DrawDebug(const Maths::Hit& h, const Maths::Matrix3x4& mat) = 0;
#endif

			bool mIsCoreModifiable = false;
			bool mIsActive = true;

			// false static cast for two main collision type
			virtual AABBTree* getAABBTree()
			{
				return nullptr;
			}
			virtual SpacialMeshBVH* getSpacialMeshBVH()
			{
				return nullptr;
			}
		};
	}
}