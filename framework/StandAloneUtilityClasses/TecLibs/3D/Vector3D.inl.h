// **********************************************************************
// * FILE  : Vector3D.cpp
// * GROUP : TecLibs, section Math
// *---------------------------------------------------------------------
// * PURPOSE : Implementation of Vector3D
// **********************************************************************
namespace Kigs
{
	namespace Maths
	{
		Vector3D::Vector3D(const Point3D& pt)
		{
			x = pt.x;
			y = pt.y;
			z = pt.z;
		}
	}
}



