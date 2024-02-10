#include "PrecompiledHeaders.h"

#include "CullingObject.h"
#include "Node3D.h"

#include <cmath>

#ifdef KIGS_TOOLS
#include "Headers/GLSLDebugDraw.h"
#include <functional>
bool gCullingDrawBBox;
std::function<bool(Kigs::Scene::Node3D*)> gDrawBBoxForNode;
#endif
int gCullingTests;

using namespace Kigs::Scene;

//IMPLEMENT_AND_REGISTER_CLASS_INFO(CullingObject, CullingObject, SceneGraph);
IMPLEMENT_CLASS_INFO(CullingObject)

void  CullingObject::InitPlane(int i, const v3f& n, const v3f& o)
{
	mCullPlaneList[(unsigned int)i].mNormal = n;
	mCullPlaneList[(unsigned int)i].mOrigin = o;

	float d = o.x*n.x + o.y*n.y + o.z*n.z;
	mCullPlaneList[(unsigned int)i].mD = d;
}

void  CullingObject::AddPlane(const v3f& n, const v3f& o)
{
	CullPlane  nplane;
	nplane.mNormal = n;
	nplane.mOrigin = o;

	float d = o.x*n.x + o.y*n.y + o.z*n.z;
	nplane.mD = d;
	mCullPlaneList.push_back(nplane);

}

void  CullingObject::RemovePlane(int i)
{
	mCullPlaneList.erase(mCullPlaneList.begin() + i);
}

template<bool isScaled>
CullingObject::CULLING_RESULT CullingObject::SubCull(Node3D* node, unsigned int& cullingMask)
{

	int result = all_in;
	std::vector< CullingObject::CullPlane>::const_iterator it = mCullPlaneList.begin();
	std::vector< CullingObject::CullPlane>::const_iterator itend = mCullPlaneList.end();

	int index = 1;
	v3f  normal;
	v3f   origin;
	
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
	float dot1;

	while (it != itend)
	{
		if ((cullingMask&index) == 0)
		{
			float d;
			if (isScaled)
			{
				normal = it->mNormal;
				origin = it->mOrigin;
				d = it->mD;
			}
			else
			{
				normal = transformVector(g2l,it->mNormal);
				origin = transformPoint(g2l,it->mOrigin);

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

				for (auto i = 0; i < 8; i++)
				{
					pts[i] = node->GetLocalToGlobal() * v4f(pts[i].x, pts[i].y,pts[i].z,1.0f);
				}
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
v3f	CullingObject::getIntersection(const CullPlane& p1, const  CullPlane& p2, const  CullPlane& p3)
{
	v3f	result(0.0f, 0.0f, 0.0f);
	// first get line of intersection between p1 & p2

	v3f dnormal;
	dnormal=cross(p1.mNormal, p2.mNormal);

	if (length2(dnormal) > 0.0001)
	{
		dnormal=normalize(dnormal); // this is the direction of the line
		// project p1.myOrigin on p2 to have a point on the line

		v3f distVector(p1.mOrigin);
		distVector -= (v3f&)p2.mOrigin;

		float dist = dot(distVector, p2.mNormal);

		v3f dpoint(p1.mOrigin);
		dpoint -= p2.mNormal*dist;

		// then compute intersection of this line with p3

		// check parallel
		float denom = dot(p3.mNormal, dnormal);

		if (fabsf(denom) > 0.0001)
		{
			float t = dot((p3.mOrigin - dpoint), p3.mNormal) / denom;
			result = dpoint + t*dnormal;
		}
	}

	return result;
}
