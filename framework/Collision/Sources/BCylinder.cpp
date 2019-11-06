#include "PrecompiledHeaders.h"
#include "BCylinder.h"

IMPLEMENT_CLASS_INFO(BCylinder)

IMPLEMENT_CONSTRUCTOR(BCylinder)
, myRadius(*this, false, LABEL_AND_ID(Radius), KFLOAT_CONST(0.0))
,myHeight(*this, false, LABEL_AND_ID(Height), KFLOAT_CONST(0.0))
, myAxle(*this, false, LABEL_AND_ID(Axle), "X", "Y", "Z")
{
}

void BCylinder::InitModifiable()
{
	CollisionBaseNode::InitModifiable();

	Point3D axle;
	GetAxle(axle);
	axle *= myHeight*0.5f;
	Point3D min = -axle;
	if (min.x == 0)min.x = -myRadius;
	if (min.y == 0)min.y = -myRadius;
	if (min.z == 0)min.z = -myRadius;

	Point3D max = axle;
	if (max.x == 0)max.x = myRadius;
	if (max.y == 0)max.y = myRadius;
	if (max.z == 0)max.z = myRadius;

	myBoundingBox.Update(min);
	myBoundingBox.Update(max);
}

void BCylinder::GetAxle(Point3D &Axle) 
{
	switch ((unsigned int)myAxle)
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
void BCylinder::DrawDebug(const Point3D& pos, const  Matrix3x4* mat, Timer *timer)
{
	Point3D p[2];
	p[0].Set(0, 0, 0);

	switch ((unsigned int)myAxle)
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

	mat->TransformPoints(p, 2);

	p[1].Normalize();

	dd::circle(p[0] + (p[1] * (myHeight*0.5f)), p[1], debugColor, myRadius, 100);
	dd::circle(p[0] - (p[1] * (myHeight*0.5f)), p[1], debugColor, myRadius, 100);
}

void BCylinder::Update(const Timer&  timer, void* addParam)
{
	CollisionBaseNode::Update(timer, addParam);

	// draw bbox
	dd::aabb(myBoundingBox.m_Min, myBoundingBox.m_Max, Point3D(.5, 1, 0));
}
#endif

