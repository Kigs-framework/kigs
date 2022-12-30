#pragma once

#include "CoreModifiable.h"
#include "TecLibs/Tec3D.h"

namespace Kigs
{

	namespace Scene
	{
		using namespace Kigs::Core;
		class Node3D;


		// ****************************************
		// * CullingObject class
		// * --------------------------------------
		/**
		 * \file	CullingObject.h
		 * \class	CullingObject
		 * \ingroup SceneGraph
		 * \brief	Base class for culling objects.
		 */
		 // ****************************************
		class CullingObject : public CoreModifiable
		{
		public:
			//! a culling object is a CoreModifiable
			DECLARE_CLASS_INFO(CullingObject, CoreModifiable, SceneGraph)
				DECLARE_INLINE_CONSTRUCTOR(CullingObject) {}

			//! an object can be totally outside, totally inside or partially inside
			enum  CULLING_RESULT
			{
				all_out = 1,
				all_in = 2,
				partially_in = 3
			};

			//! "classic" culling planes when culling object is created by a camera  
			enum CULLING_PLANE
			{
				plane_near = 0,
				plane_far,
				plane_bottom,
				plane_top,
				plane_left,
				plane_right
			};

			//! test the given node 
			virtual CULLING_RESULT Cull(Node3D*, unsigned int& cullingMask);

			template<bool isScaled>
			CULLING_RESULT SubCull(Node3D*, unsigned int& cullingMask);

			// ****************************************
			// * CullPlane class
			// * --------------------------------------
			/**
			 * \file	CullingObject.h
			 * \class	CullPlane
			 * \ingroup SceneGraph
			 * \brief structure to store a plane definition : a Origin Point3D and a Normal Vector3D
			 */
			 // ****************************************
			class CullPlane
			{
			public:
				//! normal
				Vector3D  mNormal;
				//! position
				Point3D   mOrigin;

				//! fourth term of plane equation
				float	mD;
			};

			/**
			 * \brief	return current plane count in this object
			 * \fn 		int GetPlaneCount()
			 * \return	name : the current plane count in this object
			 */
			int GetPlaneCount() { return (int)mCullPlaneList.size(); }

			/**
			 * \brief	initialise plane with index i in the list with given origin and normal
			 * \fn 		void  InitPlane(int i,const Vector3D&, const Point3D&);
			 * \param	i : index
			 * \param	Vector3D : normal
			 * \param	Point3D : position
			 * \return	the current plane count in this object
			 */
			void  InitPlane(int i, const Vector3D&, const Point3D&);

			/**
			 * \brief	insert a new plane in the liste defined with given origin and normal
			 * \fn 		void  AddPlane(const Vector3D&, const Point3D&);
			 * \param	Vector3D : normal
			 * \param	Point3D : position
			 */
			void  AddPlane(const Vector3D&, const Point3D&);

			/**
			 * \brief	remove plane with index i from the list
			 * \fn 		void  RemovePlane(int i);
			 * \param	i : index
			 */
			void  RemovePlane(int i);

			/**
			 * \brief	return the plane list
			 * \fn 		const std::vector<CullPlane*>& GetCullPlaneList()
			 * \return	the plane list
			 */
			std::vector<CullPlane>& GetCullPlaneList() { return mCullPlaneList; }

			// return intersection point of p1,p2,p3
			static Point3D	getIntersection(const CullPlane& p1, const CullPlane& p2, const CullPlane& p3);

		protected:

			//! the vector used to store the list of plane
			std::vector<CullPlane>   mCullPlaneList;
		};

	}
}
