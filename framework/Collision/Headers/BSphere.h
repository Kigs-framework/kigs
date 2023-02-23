#pragma once

#include "CollisionBaseNode.h"

namespace Kigs
{
	namespace Collide
	{
		// ****************************************
		// * BSphere class
		// * --------------------------------------
		/**
		* \file	BSphere.h
		* \class	BSphere
		* \ingroup Collision
		* \brief Sphere structure used for collision calculation.
		*/
		// ****************************************

		class BSphere : public CollisionBaseNode
		{
		public:
			DECLARE_CLASS_INFO(BSphere, CollisionBaseNode, Collision)

				/*! \brief constructor
				*/
			DECLARE_CONSTRUCTOR(BSphere)

			float GetRadius() const { return mRadius; }

			bool TestHit(Maths::Hit& hit, v3f local_origin, v3f local_direction) override;

		protected:

			virtual bool CallLocalRayIntersection(Maths::Hit& hit, const Point3D& start, const Vector3D& dir) const override
			{
				// TODO
				return false;
			}
			virtual bool CallLocalRayIntersection(std::vector<Maths::Hit>& hit, const Point3D& start, const Vector3D& dir) const override
			{
				// TODO
				return false;
			}
			void	InitModifiable() override;

			/*! \brief the sphere radius
			*/
			float	 mRadius = 0.0f;
			WRAP_ATTRIBUTES(mRadius);
#ifdef KIGS_TOOLS
			void DrawDebug(const Maths::Hit& h, const Matrix3x4& mat) override;
#endif
		};

	}
}