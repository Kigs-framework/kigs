#include "PrecompiledHeaders.h"

#include "CullingObject.h"
#include "Node3D.h"

#include <cmath>

#ifdef KIGS_TOOLS
#include "Headers/GLSLDebugDraw.h"
#include <functional>
bool gCullingDrawBBox;
std::function<bool(Node3D*)> gDrawBBoxForNode;
#endif
int gCullingTests;

//IMPLEMENT_AND_REGISTER_CLASS_INFO(CullingObject, CullingObject, SceneGraph);
IMPLEMENT_CLASS_INFO(CullingObject)

void  CullingObject::InitPlane(int i, const Vector3D& n, const Point3D& o)
{
	myCullPlaneList[(unsigned int)i].myNormal = n;
	myCullPlaneList[(unsigned int)i].myOrigin = o;

	float d = o.x*n.x + o.y*n.y + o.z*n.z;
	myCullPlaneList[(unsigned int)i].myD = d;
}

void  CullingObject::AddPlane(const Vector3D& n, const Point3D& o)
{
	CullPlane  nplane;
	nplane.myNormal = n;
	nplane.myOrigin = o;

	float d = o.x*n.x + o.y*n.y + o.z*n.z;
	nplane.myD = d;
	myCullPlaneList.push_back(nplane);

}

void  CullingObject::RemovePlane(int i)
{
	myCullPlaneList.erase(myCullPlaneList.begin() + i);
}

kigs::unordered_map<Node3D*, bool> show_bbox;

template<bool isScaled>
CullingObject::CULLING_RESULT CullingObject::SubCull(Node3D* node, unsigned int& cullingMask)
{

	int result = all_in;
	std::vector< CullingObject::CullPlane>::const_iterator it = myCullPlaneList.begin();
	std::vector< CullingObject::CullPlane>::const_iterator itend = myCullPlaneList.end();

	int index = 1;
	Vector3D  normal;
	Point3D   origin;
	
	const BBox* currentBBoxP;
	if (isScaled)
	{
		currentBBoxP = &node->GetGlobalBoundingBox();
		//node->GetGlobalBoundingBox(currentBBox.m_Min, currentBBox.m_Max);
	}
	else
	{
		currentBBoxP = &node->GetLocalBoundingBox();
		//node->GetLocalBoundingBox(currentBBox.m_Min, currentBBox.m_Max);
	}

	const BBox& currentBBox = *currentBBoxP;

	if (currentBBox.m_Max.x < currentBBox.m_Min.x) // bbox not init ?
		return all_out;

	auto& g2l = node->GetGlobalToLocal();
	kfloat dot1;

	while (it != itend)
	{
		if ((cullingMask&index) == 0)
		{
			float d;
			if (isScaled)
			{
				normal = it->myNormal;
				origin = it->myOrigin;
				d = it->myD;
			}
			else
			{
				g2l.TransformVector(&it->myNormal, &normal);
				g2l.TransformPoint(&it->myOrigin, &origin);

				d = origin.x*normal.x + origin.y*normal.y + origin.z*normal.z;
			}

			int ix = std::signbit(normal.x) ? 0 : 1;
			int iy = std::signbit(normal.y) ? 0 : 1;
			int iz = std::signbit(normal.z) ? 0 : 1;

			dot1 = currentBBox.m_MinMax[ix].x *normal.x + currentBBox.m_MinMax[iy].y *normal.y + currentBBox.m_MinMax[iz].z *normal.z;
			if (dot1 < d)
			{
				result = all_out;
				break;
			}
			ix ^= 1; iy ^= 1; iz ^= 1;
			dot1 = currentBBox.m_MinMax[ix].x *normal.x + currentBBox.m_MinMax[iy].y *normal.y + currentBBox.m_MinMax[iz].z *normal.z;

			if (dot1 < d)
			{
				result  |= partially_in;
			}
			else
			{
				cullingMask |= index; // fully inside this plane
			}
		}
		++it;
		index = index << 1;
	}
	return (CULLING_RESULT)result;
}

CullingObject::CULLING_RESULT CullingObject::Cull(Node3D* node, unsigned int& cullingMask)
{
	CullingObject::CULLING_RESULT result;


	if (node->IsScaled())
	{
		result = SubCull<true>(node, cullingMask);
	}
	else
	{
		result = SubCull<false>(node, cullingMask);
	}

#ifdef KIGS_TOOLS
	if (gCullingDrawBBox && (!gDrawBBoxForNode || gDrawBBoxForNode(node)))
	{	
		BBox bb;
		v3f pts[8];
		bool valid = false;
		if (node->IsScaled())
		{
			bb = node->GetGlobalBoundingBox();
			valid = bb.m_Max.x >= bb.m_Min.x;
			if(valid) bb.ConvertToPoint(pts);
		}
		else
		{
			bb = node->GetLocalBoundingBox();
			valid = bb.m_Max.x >= bb.m_Min.x;
			if (valid)
			{
				bb.ConvertToPoint(pts);
				node->GetLocalToGlobal().TransformPoints(pts, 8);
			}
			
		}
		if (valid)
		{
			if (result == partially_in)
			{
				dd::box(pts, { 0,0,255 });
			}
			else if (result == all_out)
			{
				dd::box(pts, { 255,0,0 });
			}
			else if (result == all_in)
			{
				dd::box(pts, { 0,255,0 });
			}
			else
			{
				dd::box(pts, { 255,255,255 });
			}
		}
	}
#endif
	return result;
}

// return intersection point of p1,p2,p3
Point3D	CullingObject::getIntersection(const CullPlane& p1, const  CullPlane& p2, const  CullPlane& p3)
{
	Point3D	result(0.0f, 0.0f, 0.0f);
	// first get line of intersection between p1 & p2

	Vector3D dnormal;
	dnormal.CrossProduct(p1.myNormal, p2.myNormal);

	if (NormSquare(dnormal) > 0.0001)
	{
		dnormal.Normalize(); // this is the direction of the line
		// project p1.myOrigin on p2 to have a point on the line

		Vector3D distVector(p1.myOrigin);
		distVector -= p2.myOrigin;

		kfloat dist = Dot(distVector, p2.myNormal);

		Point3D dpoint(p1.myOrigin);
		dpoint -= p2.myNormal*dist;

		// then compute intersection of this line with p3

		// check parallel
		kfloat denom = Dot(p3.myNormal, dnormal);

		if (fabsf(denom) > 0.0001)
		{
			kfloat t = Dot((p3.myOrigin - dpoint), p3.myNormal) / denom;
			result = dpoint + t*dnormal;
		}
	}

	return result;
}
