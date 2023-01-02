#pragma once

#include "CollisionBaseNode.h"

namespace Kigs
{

	namespace Collide
	{
		// ****************************************
		// * BCylinder class
		// * --------------------------------------
		/**
		* \file	BCylinder.h
		* \class	BCylinder
		* \ingroup Collision
		* \brief Bounding cylinder, for collision calculation purpose.
		*/
		// ****************************************
		class BCylinder : public CollisionBaseNode
		{
		public:
			DECLARE_CLASS_INFO(BCylinder, CollisionBaseNode, Collision)

				/*! \brief constructor
				*/
				DECLARE_CONSTRUCTOR(BCylinder)

				float GetRadius() const { return mRadius.const_ref(); }
			float GetHeight() const { return mHeight.const_ref(); }
			void GetAxle(Point3D& Axle);

		protected:
			void	InitModifiable() override;

			void	GetNodeBoundingBox(Point3D& pmin, Point3D& pmax) const override { pmin.Set(mBoundingBox.m_Min); pmax.Set(mBoundingBox.m_Max); }

			virtual bool CallLocalRayIntersection(Maths::Hit& hit, const Point3D& start, const Vector3D& dir)  const override
			{
				// TODO
				return false;
			}
			virtual bool CallLocalRayIntersection(std::vector<Maths::Hit>& hit, const Point3D& start, const Vector3D& dir)  const override
			{
				// TODO
				return false;
			}

			//! \brief cylinder radius
			maFloat mRadius;
			//! \brief cylinder height
			maFloat mHeight;
			//! \brief cylinder alignement axis
			maEnum<3> mAxle;

#ifdef KIGS_TOOLS
			void Update(const Time::Timer& timer, void* addParam) override;
			void DrawDebug(const Maths::Hit& h, const Matrix3x4& mat) override;
#endif
		};

	}
}