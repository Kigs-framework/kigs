#include "PrecompiledHeaders.h"
#include "AABBTree.h"
#include "Plane.h"
#include "CollisionHit.h"
#include "Intersection.h"
#include <algorithm>


namespace Intersection
{

bool IntersectRayPlane(const Point3D &rOrigin, const Vector3D &rVector,
	const Point3D &pOrigin, const Vector3D &pNormal,
	double &IntersectionDistance,
	CollisionBaseObject* pCollisionObject)
{
	float d = -Dot(pNormal, pOrigin);
	float numer = Dot(pNormal, rOrigin) + d;
	float denom = Dot(pNormal, rVector);

	if (denom == 0.0f)  // normal is orthogonal to vector, cant intersect
		return false;

	double dist = -(numer / denom);
	if (dist < 0 || dist >= IntersectionDistance)
		return false;

	IntersectionDistance = dist;
	return(pCollisionObject->ValidHit(rOrigin + (rVector*IntersectionDistance)));
}

bool IntersectRayPlane(const Point3D &rOrigin, const Vector3D &rVector,
	Plane * plane, double &IntersectionDistance)
{
	Point3D ori;
	Vector3D normal;
	plane->GetPlane(ori, normal);

	float d = -Dot(normal, ori);
	float numer = Dot(normal, rOrigin) + d;
	float denom = Dot(normal, rVector);

	if (denom == 0.0f)  // normal is orthogonal to vector, cant intersect
		return false;

	double dist = -(numer / denom);
	if (dist < 0 || dist >= IntersectionDistance)
		return false;

	IntersectionDistance = dist;
	return true;
}


bool IntersectionPlaneAABBTree(const Point3D* vertex_list, const Point3D& o, const Vector3D& n, const AABBTreeNode &pAABB, std::vector< Segment3D >& result, const std::vector< u8 >& side)
{
	bool bresult = false;


	int ix = signbit(n.x) ? 0 : 1;
	int iy = signbit(n.y) ? 0 : 1;
	int iz = signbit(n.z) ? 0 : 1;

	const BBox& currentBBox = pAABB.GetBBox();

	float d = o.x*n.x + o.y*n.y + o.z*n.z;

	float dot1 = currentBBox.m_MinMax[ix].x *n.x + currentBBox.m_MinMax[iy].y *n.y + currentBBox.m_MinMax[iz].z *n.z;

	if (dot1 <= d)
	{
		return false;
	}

	ix ^= 1; iy ^= 1; iz ^= 1;
	dot1 = currentBBox.m_MinMax[ix].x *n.x + currentBBox.m_MinMax[iy].y *n.y + currentBBox.m_MinMax[iz].z *n.z;

	if (dot1 >= d)
	{
		return false;
	}

	if (!pAABB.GetSon1()) // leaf
	{
		bool found = false;
		auto triangle_array = pAABB.GetTriangleArray();

		for (int i = 0; i < pAABB.GetTriangleCount(); i++)
		{

			unsigned int indexes[3] = { triangle_array[i].A(),triangle_array[i].B(),triangle_array[i].C() };

			unsigned int intersect = side[indexes[0]] | side[indexes[1]] | side[indexes[2]];

			if (intersect == 3)
			{
				int pointIndexInSegment = 0;
				int swapOffset = 0;

				if (side[indexes[0]] == 2)
				{
					swapOffset = 1;
				}

				result.push_back(Segment3D());

				Segment3D&	ToAdd = result.back();
				bool needSwap = false;
				// test each segment
				for (unsigned int j = 0; j < 3; j++)
				{
					unsigned int j1 = (j + 1) % 3;
					int pointIndexInSegmentMod2 = (pointIndexInSegment + swapOffset) & 1;

					if ((side[indexes[j]] | side[indexes[j1]]) == 3)
					{
						// always cut in same direction to avoid precision mismatch
						int cutj1 = indexes[j];
						int cutj2 = indexes[j1];
						if (side[indexes[j]] == 2)
						{
							int cutj1 = indexes[j1];
							int cutj2 = indexes[j];
						}


						float d1 = vertex_list[cutj2].x *n.x + vertex_list[cutj2].y *n.y + vertex_list[cutj2].z *n.z - d;
						float d2 = vertex_list[cutj1].x *n.x + vertex_list[cutj1].y *n.y + vertex_list[cutj1].z *n.z - d;

						float c = -d2 / (d1 - d2);

						ToAdd.pts[pointIndexInSegmentMod2] = vertex_list[cutj1] + (vertex_list[cutj2] - vertex_list[cutj1])*c;
						pointIndexInSegment++;
					}
					else if (side[indexes[j]] == 0)
					{
						ToAdd.pts[pointIndexInSegmentMod2] = vertex_list[indexes[j]];
						pointIndexInSegment++;
					}
					
				}

				// check for too small segment
				if (DistSquare(ToAdd.p1, ToAdd.p2) < 0.000001f)
				{
					result.pop_back();
				}
				else
				{
					found = true;
				}

#ifdef _DEBUG
				if (pointIndexInSegment != 2)
				{
					KIGS_ERROR("Bad Triangle plane intersection\n", 1);
				}
#endif


			}
		}
		return found;
	}

	bresult |= IntersectionPlaneAABBTree(vertex_list,o,n, *pAABB.GetSon1(), result,side);

	bresult |= IntersectionPlaneAABBTree(vertex_list, o, n, *pAABB.GetSon2(), result,side);


	return bresult;

}

bool IntersectionPlaneAABBTree(const Point3D& o, const Vector3D& n, const AABBTree &pAABB, std::vector< Segment3D >& result)
{
	// check if plane intersect aabb bbox
	
	int ix = signbit(n.x) ? 0 : 1;
	int iy = signbit(n.y) ? 0 : 1;
	int iz = signbit(n.z) ? 0 : 1;

	const BBox& currentBBox = pAABB.GetBBox();

	float d = o.x*n.x + o.y*n.y + o.z*n.z;

	float dot1 = currentBBox.m_MinMax[ix].x *n.x + currentBBox.m_MinMax[iy].y *n.y + currentBBox.m_MinMax[iz].z *n.z;

	if (dot1<=d)
	{
		return false;
	}

	ix ^= 1; iy ^= 1; iz ^= 1;
	dot1 = currentBBox.m_MinMax[ix].x *n.x + currentBBox.m_MinMax[iy].y *n.y + currentBBox.m_MinMax[iz].z *n.z;

	if (dot1 >= d)
	{
		return false;
	}

	// check each vertex side

	const Point3D* vertex_list = pAABB.GetVertexList();
	unsigned int vertex_count = pAABB.GetVertexCount();

	std::vector< u8 > sides;
	sides.resize(vertex_count, 0);

	for (unsigned int i = 0; i < vertex_count; i++)
	{
		const Point3D& coords = vertex_list[i];
		dot1 = coords.x *n.x + coords.y *n.y + coords.z *n.z;

		if (dot1 > d)
		{
			sides[i] = 1;
		}
		else if (dot1 < d)
		{
			sides[i] = 2;
		}
	}
	
	return IntersectionPlaneAABBTree(vertex_list, o,n, pAABB, result, sides);
}


/*!	test intersection between a ray (origin,direction) and a AABBTree
	intersection distance parameter have to be initialised to the maximum tested distance
	return the triangle, the intersection u and v in the triangle and the intersection distance
	this method is recursive
*/
bool IntersectionRayAABBTree(const Point3D* vertex_list, const Point3D &RayOrigin, const Vector3D &RayDirection,
	const AABBTreeNode &pAABB, double &outDistance, Vector3D &outNormal, float &outCoordU, float &outCoordV, s32* outTriangleIndices)
{
	bool found = false;
	double TempDist = outDistance;
	float Tempu, Tempv;

	
	// check hit on BBox
	if(IntersectionSegmentBBox(RayOrigin, RayOrigin + RayDirection * outDistance, pAABB.GetBBox().m_Min, pAABB.GetBBox().m_Max))
	//if (IntersectionRayBBox(RayOrigin, RayDirection, pAABB.GetBBox().m_Min, pAABB.GetBBox().m_Max))
	{
		// if leaf then check triangle collision
		if (!pAABB.GetSon1())
		{
			TempDist = outDistance;
		
			auto triangle_array = pAABB.GetTriangleArray();
			Vector3D triangle_normal;

			for (int i = 0; i < pAABB.GetTriangleCount(); i++)
			{
				if (IntersectionRayTriangle(RayOrigin, RayDirection,
					vertex_list[triangle_array[i].A()],
					vertex_list[triangle_array[i].B()],
					vertex_list[triangle_array[i].C()],
					TempDist, Tempu, Tempv, triangle_normal))
				{
					if (TempDist < outDistance /*&& pAABB.ValidHit(RayOrigin+(RayDirection*TempDist))*/)
					{
						outDistance = TempDist;
						outNormal = triangle_normal;
						outCoordU = Tempu;
						outCoordV = Tempv;

						if (outTriangleIndices)
						{
							outTriangleIndices[0] = triangle_array[i].A();
							outTriangleIndices[1] = triangle_array[i].B();
							outTriangleIndices[2] = triangle_array[i].C();
						}

						found = true;

#ifdef KIGS_TOOLS
						const_cast<AABBTreeNode&>(pAABB).mHit = true;
#endif
					}
				}
			}
			return found;
		}

		// check collision with first son (found if TempDist changed)
		IntersectionRayAABBTree(vertex_list,RayOrigin, RayDirection, *(pAABB.GetSon1()), TempDist, outNormal, outCoordU, outCoordV, outTriangleIndices);

		// check collision with second son (found if TempDist changed)
		IntersectionRayAABBTree(vertex_list,RayOrigin, RayDirection, *(pAABB.GetSon2()), TempDist, outNormal, outCoordU, outCoordV, outTriangleIndices);

		if (TempDist < outDistance)
		{
#ifdef KIGS_TOOLS
			const_cast<AABBTreeNode&>(pAABB).mHit = true;
#endif
			outDistance = TempDist;
			return true;
		}
	}

	return false;
}




bool IntersectionRayAABBTree(const Point3D* vertex_list, const Point3D &RayOrigin, const Vector3D &RayDirection,
	const AABBTreeNode &pAABB, std::vector<Hit>& hits)
{
	bool found = false;
	float Tempu, Tempv;
	double dist = DBL_MAX;
	// check hit on BBox
	if (IntersectionRayBBox(RayOrigin, RayDirection, pAABB.GetBBox().m_Min, pAABB.GetBBox().m_Max))
	{
		// if leaf then check triangle collision
		if (!pAABB.GetSon1())
		{
			auto triangle_array = pAABB.GetTriangleArray();
			Vector3D triangle_normal;

			for (int i = 0; i < pAABB.GetTriangleCount(); i++)
			{
				if (IntersectionRayTriangle(RayOrigin, RayDirection,
					vertex_list[triangle_array[i].A()],
					vertex_list[triangle_array[i].B()],
					vertex_list[triangle_array[i].C()],
					dist, Tempu, Tempv, triangle_normal))
				{
					hits.emplace_back();
					Hit *h = &hits.back();
					h->HitDistance = dist;
					h->HitNormal = triangle_normal;
					h->HitTriangleVertexIndices[0] = triangle_array[i].A();
					h->HitTriangleVertexIndices[1] = triangle_array[i].B();
					h->HitTriangleVertexIndices[2] = triangle_array[i].C();
					h->HitFaceIndex = triangle_array[i].FaceIndex();
					found = true;


#ifdef KIGS_TOOLS
					const_cast<AABBTreeNode&>(pAABB).mHit = true;
#endif					
				}
			}
			return found;
		}

		// check collision with first son (found if TempDist changed)
		IntersectionRayAABBTree(vertex_list,RayOrigin, RayDirection, *(pAABB.GetSon1()), hits);

		// check collision with second son (found if TempDist changed)
		IntersectionRayAABBTree(vertex_list,RayOrigin, RayDirection, *(pAABB.GetSon2()), hits);

#ifdef KIGS_TOOLS
		const_cast<AABBTreeNode&>(pAABB).mHit = true;
#endif
		return true;
	}

	return false;
	}

/*
void IntersectionAxisAABBTree(std::vector<Point2D>& coordinates, Point3D &RayOrigin, const int axisType, AABBTreeNode &pAABB, Point3D* VertexArray)
{
	switch (axisType)
	{
	case 0:
		IntersectionXAxisAABBTree(coordinates, RayOrigin, pAABB, VertexArray);
		break;
	case 1:
		IntersectionYAxisAABBTree(coordinates, RayOrigin, pAABB, VertexArray);
		break;
	case 2:
		IntersectionZAxisAABBTree(coordinates, RayOrigin, pAABB, VertexArray);
		break;
	}

	std::sort(coordinates.begin(), coordinates.end(), SortAxisIntersection);
}
*/
/*
void IntersectionXAxisAABBTree(std::vector<Point2D>& coordinates, Point3D &RayOrigin, AABBTreeNode &pAABB, Point3D* VertexArray)
{

	// check bbox first
	if ((RayOrigin.y >= pAABB.mBBox.m_Min.y) && (RayOrigin.z >= pAABB.mBBox.m_Min.z) &&
		(RayOrigin.y <= pAABB.mBBox.m_Max.y) && (RayOrigin.z <= pAABB.mBBox.m_Max.z))
	{
		// c'est une feuille
		if (pAABB.Son1 == NULL && pAABB.mSon2 == NULL)
		{
			int i;

			for (i = 0; i < pAABB.TriangleCount; i++)
			{
				float x;
				float dir;
				if (IntersectionXAxisTriangle(RayOrigin,
					VertexArray[pAABB.TriangleArray[i].m_Original->a],
					VertexArray[pAABB.TriangleArray[i].m_Original->b],
					VertexArray[pAABB.TriangleArray[i].m_Original->c], x, dir))
				{
					Point2D	toadd;
					toadd.x = x;
					toadd.y = dir;
					coordinates.push_back(toadd);
				}
			}

		}

		IntersectionXAxisAABBTree(coordinates, RayOrigin, *(pAABB.Son1), VertexArray);
		IntersectionXAxisAABBTree(coordinates, RayOrigin, *(pAABB.mSon2), VertexArray);
	}

	return;
}
*/

bool IntersectionXAxisTriangle(Point3D &RayOrigin, const Point3D &A, const Point3D &B, const Point3D &C, float& x, float& dir)
{
	Vector3D edge1(A, B, asVector{});
	Vector3D edge2(A, C, asVector{});

	Vector3D n;
	n.CrossProduct(edge1, edge2);
	if (n.x > 0.0001f)
	{
		dir = 1.0f;
	}
	else if (n.x < -0.0001f)
	{
		dir = -1.0f;
	}
	else
	{
		return false;
	}

	// to optimise (a lot)

	RayOrigin.x =-10000.0f;
	Vector3D	RayDirection;
	RayDirection.Set(1.0f, 0.0f, 0.0f);

	double	intersectionDistance;
	float	u, v;
	Vector3D triangle_normal;
	if (IntersectionRayTriangle(RayOrigin, RayDirection, A, B, C, intersectionDistance, u, v, triangle_normal))
	{
		x = intersectionDistance - 10000.0f;
		return true;
	}
	return false;
}

/*void IntersectionYAxisAABBTree(std::vector<Point2D>& coordinates, Point3D &RayOrigin, AABBTreeNode &pAABB, Point3D* VertexArray)
{
	// check bbox first
	if ((RayOrigin.x >= pAABB.mBBox.m_Min.x) && (RayOrigin.z >= pAABB.mBBox.m_Min.z) &&
		(RayOrigin.x <= pAABB.mBBox.m_Max.x) && (RayOrigin.z <= pAABB.mBBox.m_Max.z))
	{
		// c'est une feuille
		if (pAABB.Son1 == NULL && pAABB.mSon2 == NULL)
		{
			int i;

			for (i = 0; i < pAABB.TriangleCount; i++)
			{
				float y;
				float dir;
				if (IntersectionYAxisTriangle(RayOrigin,
					VertexArray[pAABB.TriangleArray[i].m_Original->a],
					VertexArray[pAABB.TriangleArray[i].m_Original->b],
					VertexArray[pAABB.TriangleArray[i].m_Original->c], y, dir))
				{
					Point2D	toadd;
					toadd.x = y;
					toadd.y = dir;
					coordinates.push_back(toadd);
				}
			}

		}

		IntersectionYAxisAABBTree(coordinates, RayOrigin, *(pAABB.Son1), VertexArray);
		IntersectionYAxisAABBTree(coordinates, RayOrigin, *(pAABB.mSon2), VertexArray);
	}

	return;
}*/

bool IntersectionYAxisTriangle(Point3D &RayOrigin, const Point3D &A, const Point3D &B, const Point3D &C, float& y, float& dir)
{
	Vector3D edge1(A, B, asVector{});
	Vector3D edge2(A, C, asVector{});

	Vector3D n;
	n.CrossProduct(edge1, edge2);
	if (n.y > 0.0001f)
	{
		dir = 1.0f;
	}
	else if (n.y <-0.0001f)
	{
		dir = -1.0f;
	}
	else
	{
		return false;
	}


	// to optimise (a lot)

	RayOrigin.y = -10000.0f;
	Vector3D	RayDirection;
	RayDirection.Set(0.0f, 1.0f, 0.0f);

	double	intersectionDistance;
	float	u, v;
	Vector3D triangle_normal;
	if (IntersectionRayTriangle(RayOrigin, RayDirection, A, B, C, intersectionDistance, u, v, triangle_normal))
	{
		y = intersectionDistance - 10000.0f;
		return true;
	}
	return false;

}

/*void IntersectionZAxisAABBTree(std::vector<Point2D>& coordinates, Point3D &RayOrigin, AABBTreeNode &pAABB, Point3D* VertexArray)
{
	// check bbox first
	if ((RayOrigin.x >= pAABB.mBBox.m_Min.x) && (RayOrigin.y >= pAABB.mBBox.m_Min.y) &&
		(RayOrigin.x <= pAABB.mBBox.m_Max.x) && (RayOrigin.y <= pAABB.mBBox.m_Max.y))
	{
		// c'est une feuille
		if (pAABB.Son1 == NULL && pAABB.mSon2 == NULL)
		{
			int i;

			for (i = 0; i < pAABB.TriangleCount; i++)
			{
				float z;
				float dir;
				if (IntersectionZAxisTriangle(RayOrigin,
					VertexArray[pAABB.TriangleArray[i].m_Original->a],
					VertexArray[pAABB.TriangleArray[i].m_Original->b],
					VertexArray[pAABB.TriangleArray[i].m_Original->c], z, dir))
				{
					Point2D	toadd;
					toadd.x = z;
					toadd.y = dir;
					coordinates.push_back(toadd);
				}
			}

		}

		if (pAABB.Son1)
		{
			IntersectionZAxisAABBTree(coordinates, RayOrigin, *(pAABB.Son1), VertexArray);
		}
		if (pAABB.mSon2)
		{
			IntersectionZAxisAABBTree(coordinates, RayOrigin, *(pAABB.mSon2), VertexArray);
		}
	}

	return;
}*/

bool IntersectionZAxisTriangle(Point3D &RayOrigin, const Point3D &A, const Point3D &B, const Point3D &C, float& z, float& dir)
{
	Vector3D edge1(A, B, asVector{});
	Vector3D edge2(A, C, asVector{});

	Vector3D n;
	n.CrossProduct(edge1, edge2);
	if (n.z > 0.0001f)
	{
		dir = 1.0f;
	}
	else if (n.z < -0.0001f)
	{
		dir = -1.0f;
	}
	else
	{
		return false;
	}


	// to optimise (a lot)

	RayOrigin.z =-10000.0f;
	Vector3D	RayDirection;
	RayDirection.Set(0.0f, 0.0f, 1.0f);

	double	intersectionDistance;
	float	u, v;
	Vector3D triangle_normal;
	if (IntersectionRayTriangle(RayOrigin, RayDirection, A, B, C, intersectionDistance, u, v, triangle_normal))
	{
		z = intersectionDistance - 10000.0f;
		return true;
	}
	return false;
}



//TODO
/*
bool	IntersectTriangleMovingSphere(const Point3D &spherePos1,const Point3D &spherePos2,const float& sphereRadius,const Interval& interval,const Triangle& triangle,float& result)
{
	// first get triangle normal
	Vector3D	normal;
	Vector3D	P1P2(triangle.P1,triangle.P2);
	Vector3D	P1P3(triangle.P1,triangle.P3);

	normal.CrossProduct(P1P2,P1P3);
	normal.Normalize();

	// find "oriented distance" at time interval.myStart and at time interval.myEnd

	float	odiststart=Dot(normal,Vector3D(triangle.P1,spherePos1));
	float	odistend=Dot(normal,Vector3D(triangle.P1,spherePos2));

	// if sphere never intersect plane during interval, return false directly

	if((odiststart>sphereRadius)&&(odistend>sphereRadius))
	{
		return false;
	}

	if((odiststart<-sphereRadius)&&(odistend<-sphereRadius))
	{
		return false;
	}

	// else find interval of intersection

	Interval	IntersectTrianglePlaneInterval;

	// 3 cases (in fact 4 but first case is when sphere doesn't intersect and was rejected before)

	bool	intersectatstart=(odiststart<=sphereRadius)&&(odiststart>=-sphereRadius);
	bool	intersectatend=(odistend<=sphereRadius)&&(odistend>=-sphereRadius);

	// sphere intersect at start and end, just copy interval
	if(intersectatstart&&intersectatend)
	{
		IntersectTrianglePlaneInterval.myStart=interval.myStart;
		IntersectTrianglePlaneInterval.myEnd=interval.myEnd;
	}
	else
	{
		// compute

	}




	 return true;
}
*/

}
