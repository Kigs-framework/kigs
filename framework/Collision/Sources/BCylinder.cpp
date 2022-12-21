#include "PrecompiledHeaders.h"
#include "BCylinder.h"

IMPLEMENT_CLASS_INFO(BCylinder)

IMPLEMENT_CONSTRUCTOR(BCylinder)
, mRadius(*this, false, "Radius", 0.0f)
,mHeight(*this, false, "Height", 0.0f)
, mAxle(*this, false, "Axle", "X", "Y", "Z")
{
}

void BCylinder::InitModifiable()
{
	CollisionBaseNode::InitModifiable();

	Point3D axle;
	GetAxle(axle);
	axle *= mHeight*0.5f;
	Point3D min = -axle;
	if (min.x == 0)min.x = -mRadius;
	if (min.y == 0)min.y = -mRadius;
	if (min.z == 0)min.z = -mRadius;

	Point3D max = axle;
	if (max.x == 0)max.x = mRadius;
	if (max.y == 0)max.y = mRadius;
	if (max.z == 0)max.z = mRadius;

	mBoundingBox.Update(min);
	mBoundingBox.Update(max);
}

void BCylinder::GetAxle(Point3D &Axle) 
{
	switch ((unsigned int)mAxle)
	{
	case 0: //X
		Axle.Set(1, 0, 0);
		break;
	case 1: //Y
		Axle.Set(0, 1, 0);
		break;
	case 2: //Z
		Axle.Set(0, 0, 1);
		break;
	default:
		break;
	}
}

#ifdef KIGS_TOOLS
#include <GLSLDebugDraw.h>
void BCylinder::DrawDebug(const Hit& h, const Matrix3x4& mat)
{
	Point3D p[2];
	p[0].Set(0, 0, 0);

	switch ((unsigned int)mAxle)
	{
	case 0: //X
		p[1].Set(1, 0, 0);
		break;
	case 1: //Y
		p[1].Set(0, 1, 0);
		break;
	case 2: //Z
		p[1].Set(0, 0, 1);
		break;
	default:
		break;
	}

	mat.TransformPoints(p, 2);

	p[1].Normalize();

	dd::circle(p[0] + (p[1] * (mHeight*0.5f)), p[1], mDebugColor, mRadius, 100);
	dd::circle(p[0] - (p[1] * (mHeight*0.5f)), p[1], mDebugColor, mRadius, 100);
}

void BCylinder::Update(const Timer&  timer, void* addParam)
{
	CollisionBaseNode::Update(timer, addParam);

	// draw bbox
	dd::aabb(mBoundingBox.m_Min, mBoundingBox.m_Max, Point3D(.5, 1, 0));
}
#endif

