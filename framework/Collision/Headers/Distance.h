#pragma once

#include "CoreModifiable.h"
#include "TecLibs/Tec3D.h"
#include "TecLibs/3D/3DObject/BBox.h"
#include "KMesh.h"

namespace Kigs
{
	namespace Collide
	{
		using namespace Kigs::Maths;
		// ****************************************
		// * Distance class
		// * --------------------------------------
		/**
		* \file	Distance.h
		* \class	Distance
		* \ingroup Collision
		* \brief  Encapsulate useful distance calculation methods
		*/
		// ****************************************
		class Distance
		{
		public:
			/*!   \brief return distance from point to triangle
				  triangle is given by points A,B,C
			*/
			static float PointTriangle(const v3f& P, const v3f& A, const v3f& B, const v3f& C);

			/*!   \brief return distance from point to mesh
				  the triangle from the mesh, where the distance is shortest is returned
			*/
			static float PointMesh(const v3f& P, const Draw::Mesh* pMesh, Draw::Mesh::Triangle*& TrianglePtr);

			/*!   \brief return distance from point to axis aligned box
				  the box is defined by a vector (diagonal), and centered on the origin point (0,0,0)
			*/
			static float PointAABBAtOrigin(const v3f& P, const Vector3D& BoxSize, v3f& MinDistPoint);

			/*!   \brief return distance from point to axis aligned box
				  the box is defined by min and max points
			*/
			static float PointAABB(const v3f& P, const v3f& BMin, const v3f& BMax, v3f& MinDistPoint);

			/*!   \brief return distance from point to oriented box
				  the box is defined a size (diagonal vector) and a transform matrix (rotation/scale and position)
			*/
			static float PointOBB(const v3f& P, const Matrix3x4& OrientationMatrix, Vector3D& BoxSize, v3f& MinDistPoint);

			/*!   \brief return distance from point to plane
				  plane is given by point PlaneOrigin and normal PlaneNormal
			*/
			static float PointPlane(const v3f& P, const Vector3D& PlaneNormal, const v3f& PlaneOrigin);
		};


	}
}