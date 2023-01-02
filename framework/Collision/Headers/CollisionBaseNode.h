#pragma once

#include "Drawable.h"
#include "CollisionBaseObject.h"
#include "maReference.h" 
#include "TecLibs/3D/3DObject/BBox.h"

namespace Kigs
{

	namespace Collide
	{
		using namespace Kigs::Draw;
		// ****************************************
		// * CollisionBaseNode class
		// * --------------------------------------
		/**
		* \file	CollisionBaseNode.h
		* \class	CollisionBaseNode
		* \ingroup Collision
		* \brief  Base class for collidable objects.
		*/
		// ****************************************

		class CollisionBaseNode : public Drawable, public CollisionBaseObject
		{
		public:
			DECLARE_ABSTRACT_CLASS_INFO(CollisionBaseNode, Drawable, Collision)

				/*! \brief constructor
				*/
				DECLARE_CONSTRUCTOR(CollisionBaseNode);

			bool BBoxUpdate(double time) override { return true; }
			void	GetNodeBoundingBox(Point3D& pmin, Point3D& pmax) const override { pmin = mBoundingBox.m_Min; pmax = mBoundingBox.m_Max; }

		protected:
			void	InitModifiable() override;


			//! link to the bounding box
			BBox        mBoundingBox;

			maBool		mHit;
		};

	}
}