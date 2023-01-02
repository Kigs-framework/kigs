#include "PrecompiledHeaders.h"
#include "Distance.h"
#include "KMesh.h"

using namespace Kigs::Collide;
using namespace Kigs::Draw;
/*!
	compute distance from a point (P) to a triangle (A,B,C)
	seperate plane in parts
*/
float Distance::PointTriangle(const Point3D &P, const Point3D &A, const Point3D &B, const Point3D &C)
{
    Vector3D kDiff(P,A, asVector{});
    Vector3D kEdge0(A,B, asVector{});
    Vector3D kEdge1(A,C, asVector{});
    float fA00 = NormSquare(kEdge0);
    float fA01 = Dot(kEdge0,kEdge1);
    float fA11 = NormSquare(kEdge1);
    float fB0 = Dot(kDiff,kEdge0);
    float fB1 = Dot(kDiff,kEdge1);
    float fC = NormSquare(kDiff);
    float fDet = fabsf(fA00*fA11-fA01*fA01);
    float fS = fA01*fB1-fA11*fB0;
    float fT = fA01*fB0-fA00*fB1;
    float fSqrDistance;

    if (fS + fT <= fDet)
    {
        if (fS < 0.0f)
        {
            if (fT < 0.0f)  // region 4
            {
                if (fB0 < 0.0f)
                {
                    fT = 0.0f;
                    if (-fB0 >= fA00)
                    {
                        fS = 1.0f;
                        fSqrDistance = fA00+2.0f*fB0+fC;
                    }
                    else
                    {
                        fS = -fB0/fA00;
                        fSqrDistance = fB0*fS+fC;
                    }
                }
                else
                {
                    fS = 0.0f;
                    if (fB1 >= 0.0f)
                    {
                        fT = 0.0f;
                        fSqrDistance = fC;
                    }
                    else if (-fB1 >= fA11)
                    {
                        fT = 1.0f;
                        fSqrDistance = fA11+2.0f*fB1+fC;
                    }
                    else
                    {
                        fT = -fB1/fA11;
                        fSqrDistance = fB1*fT+fC;
                    }
                }
            }
            else  // region 3
            {
                fS = 0.0f;
                if (fB1 >= 0.0f)
                {
                    fT = 0.0f;
                    fSqrDistance = fC;
                }
                else if (-fB1 >= fA11)
                {
                    fT = 1.0f;
                    fSqrDistance = fA11+2.0f*fB1+fC;
                }
                else
                {
                    fT = -fB1/fA11;
                    fSqrDistance = fB1*fT+fC;
                }
            }
        }
        else if (fT < 0.0f)  // region 5
        {
            fT = 0.0f;
            if (fB0 >= 0.0f)
            {
                fS = 0.0f;
                fSqrDistance = fC;
            }
            else if (-fB0 >= fA00)
            {
                fS = 1.0f;
                fSqrDistance = fA00+2.0f*fB0+fC;
            }
            else
            {
                fS = -fB0/fA00;
                fSqrDistance = fB0*fS+fC;
            }
        }
        else  // region 0
        {
            float fInvDet = 1.0f/fDet;
            fS *= fInvDet;
            fT *= fInvDet;
            fSqrDistance = fS*(fA00*fS+fA01*fT+2.0f*fB0) +
                fT*(fA01*fS+fA11*fT+2.0f*fB1)+fC;
        }
    }
    else
    {
        float fTmp0, fTmp1, fNumer, fDenom;

        if (fS < 0.0f)  // region 2
        {
            fTmp0 = fA01 + fB0;
            fTmp1 = fA11 + fB1;
            if (fTmp1 > fTmp0)
            {
                fNumer = fTmp1 - fTmp0;
                fDenom = fA00-2.0f*fA01+fA11;
                if (fNumer >= fDenom)
                {
                    fS = 1.0f;
                    fT = 0.0f;
                    fSqrDistance = fA00+2.0f*fB0+fC;
                }
                else
                {
                    fS = fNumer/fDenom;
                    fT = 1.0f - fS;
                    fSqrDistance = fS*(fA00*fS+fA01*fT+2.0f*fB0) +
                        fT*(fA01*fS+fA11*fT+2.0f*fB1)+fC;
                }
            }
            else
            {
                fS = 0.0f;
                if (fTmp1 <= 0.0f)
                {
                    fT = 1.0f;
                    fSqrDistance = fA11+2.0f*fB1+fC;
                }
                else if (fB1 >= 0.0f)
                {
                    fT = 0.0f;
                    fSqrDistance = fC;
                }
                else
                {
                    fT = -fB1/fA11;
                    fSqrDistance = fB1*fT+fC;
                }
            }
        }
        else if (fT < 0.0f)  // region 6
        {
            fTmp0 = fA01 + fB1;
            fTmp1 = fA00 + fB0;
            if (fTmp1 > fTmp0)
            {
                fNumer = fTmp1 - fTmp0;
                fDenom = fA00-2.0f*fA01+fA11;
                if (fNumer >= fDenom)
                {
                    fT = 1.0f;
                    fS = 0.0f;
                    fSqrDistance = fA11+2.0f*fB1+fC;
                }
                else
                {
                    fT = fNumer/fDenom;
                    fS = 1.0f - fT;
                    fSqrDistance = fS*(fA00*fS+fA01*fT+2.0f*fB0) +
                        fT*(fA01*fS+fA11*fT+2.0f*fB1)+fC;
                }
            }
            else
            {
                fT = 0.0f;
                if (fTmp1 <= 0.0f)
                {
                    fS = 1.0f;
                    fSqrDistance = fA00+2.0f*fB0+fC;
                }
                else if (fB0 >= 0.0f)
                {
                    fS = 0.0f;
                    fSqrDistance = fC;
                }
                else
                {
                    fS = -fB0/fA00;
                    fSqrDistance = fB0*fS+fC;
                }
            }
        }
        else  // region 1
        {
            fNumer = fA11 + fB1 - fA01 - fB0;
            if (fNumer <= 0.0f)
            {
                fS = 0.0f;
                fT = 1.0f;
                fSqrDistance = fA11+2.0f*fB1+fC;
            }
            else
            {
                fDenom = fA00-2.0f*fA01+fA11;
                if (fNumer >= fDenom)
                {
                    fS = 1.0f;
                    fT = 0.0f;
                    fSqrDistance = fA00+2.0f*fB0+fC;
                }
                else
                {
                    fS = fNumer/fDenom;
                    fT = 1.0f - fS;
                    fSqrDistance = fS*(fA00*fS+fA01*fT+2.0f*fB0) +
                        fT*(fA01*fS+fA11*fT+2.0f*fB1)+fC;
                }
            }
        }
    }

    // account for numerical round-off error
    if (fSqrDistance < 0.0f)
    {
        fSqrDistance = 0.0f;
    }

    return sqrtf(fSqrDistance);
}

/*!
	compute distance from a point (P) to a mesh
	also return the nearest triangle
*/
float Distance::PointMesh(const Point3D &P, const Mesh *pMesh, Mesh::Triangle* &TrianglePtr)
{
	int i=0,j;
	float dmin=1000.0f;
	float d;
	TrianglePtr = NULL;
	std::vector<ModifiableItemStruct>::const_iterator	it;

	for (it=pMesh->getItems().begin();it!=pMesh->getItems().end();++it)
	{
		if((*it).mItem->isSubType(MeshItemGroup::mClassID))
		{
			MeshItemGroup* current=(MeshItemGroup*)(*it).mItem.get();

			Mesh::Triangle *Tr = (Mesh::Triangle*)current->mFirstTriangle;
			int	sizeoftriangle=current->mTriangleSize;

			for (j=0;j<current->mTriangleCount;j++)
			{ 

/*
	for (i=0;i<pMesh->myGroupList.size();i++)
	{
		Mesh::Triangle *Tr = ((Mesh::GroupByMaterial*)pMesh->myGroupList[i])->mFirstTriangle;

		int	sizeoftriangle=((Mesh::GroupByMaterial*)pMesh->myGroupList[i])->mTriangleSize;

		//! no optimisation here, check each triangle in the mesh
		for (j=0;j<((Mesh::GroupByMaterial*)pMesh->myGroupList[i])->mTriangleCount;j++)
		{*/
				Mesh::Triangle* CurrentT=(Mesh::Triangle*)((unsigned char*)Tr+sizeoftriangle*j);

				d = PointTriangle(P,pMesh->mVertexArray[CurrentT->a],pMesh->mVertexArray[CurrentT->b],pMesh->mVertexArray[CurrentT->c]);
				
				//! select the minimal distance
				if (d<dmin)
				{
					dmin = d;
					TrianglePtr = CurrentT;
				}
			}
			i++;
		}
	}	
	return dmin;
}

/*!
	compute distance from a point (P) to a BBOX centered at origin (half diagonal)
	also return the nearest point on the BBox
*/
float Distance::PointAABBAtOrigin(const Point3D &P, const Vector3D &BoxSize, Point3D &MinDistPoint)
{
	float Dist = 0.0f;
	float d;
	//! first check on x axis
	if (P.x<-BoxSize.x)
	{
		d = BoxSize.x+P.x;
		Dist = d*d;
		MinDistPoint.x = -BoxSize.x;
	}
	else
	{
		if (P.x>BoxSize.x)
		{
			d = P.x-BoxSize.x;
			Dist = d*d;
			MinDistPoint.x = BoxSize.x;
		}
		else
		{
			MinDistPoint.x = P.x;
		}
	}

	//! then check on y axis
	if (P.y<-BoxSize.y)
	{
		d = BoxSize.y+P.y;
		Dist += d*d;
		MinDistPoint.y = -BoxSize.y;
	}
	else
	{
		if (P.y>BoxSize.y)
		{
			d = P.y-BoxSize.y;
			Dist += d*d;
			MinDistPoint.y = BoxSize.y;
		}
		else
		{
			MinDistPoint.y = P.y;
		}
	}

	//! then check on z axis
	if (P.z<-BoxSize.z)
	{
		d = BoxSize.z+P.z;
		Dist += d*d;
		MinDistPoint.z = -BoxSize.z;
	}
	else
	{
		if (P.z>BoxSize.z)
		{
			d = P.z-BoxSize.z;
			Dist += d*d;
			MinDistPoint.z = BoxSize.z;
		}
		else
		{
			MinDistPoint.z = P.z;
		}
	}
	return sqrtf(Dist);
}

/*!
	compute distance from a point (P) to a BBOX (min and max poins)
	also return the nearest point on the BBox
*/
float Distance::PointAABB(const Point3D &P, const Point3D &BMin,const Point3D &BMax, Point3D &MinDistPoint)
{
	float Dist = 0.0f;
	float d;
	//! first check on x axis
	if (P.x<BMin.x)
	{
		d = BMin.x-P.x;
		Dist = d*d;
		MinDistPoint.x = BMin.x;
	}
	else
	{
		if (P.x>BMax.x)
		{
			d = P.x-BMax.x;
			Dist = d*d;
			MinDistPoint.x = BMax.x;
		}
		else
		{
			MinDistPoint.x = P.x;
		}
	}

	//! then check on y axis
	if (P.y<BMin.y)
	{
		d = BMin.y-P.y;
		Dist += d*d;
		MinDistPoint.y = BMin.y;
	}
	else
	{
		if (P.y>BMax.y)
		{
			d = P.y-BMax.y;
			Dist += d*d;
			MinDistPoint.y = BMax.y;
		}
		else
		{
			MinDistPoint.y = P.y;
		}
	}

	//! then check on z axis
	if (P.z<BMin.z)
	{
		d = BMin.z-P.z;
		Dist += d*d;
		MinDistPoint.z = BMin.z;
	}
	else
	{
		if (P.z>BMax.z)
		{
			d = P.z-BMax.z;
			Dist += d*d;
			MinDistPoint.z = BMax.z;
		}
		else
		{
			MinDistPoint.z = P.z;
		}
	}

	return sqrtf(Dist);
}

/*!
	compute distance from a point (P) to an oriented BOX (half diagonal in local coordinate system, and transform matrix)
	also return the nearest point on the BBox
*/
float Distance::PointOBB(const Point3D &P, const Matrix3x4 &OrientationMatrix, Vector3D &BoxSize, Point3D &MinDistPoint)
{
	//! first compute point coordinate in local Box coordinate system
	Point3D Q;
	Q.x = OrientationMatrix.e[0][0]*P.x + OrientationMatrix.e[0][1]*P.y + OrientationMatrix.e[0][2]*P.z - OrientationMatrix.e[3][0];
	Q.y = OrientationMatrix.e[1][0]*P.x + OrientationMatrix.e[1][1]*P.y + OrientationMatrix.e[1][2]*P.z - OrientationMatrix.e[3][1];
	Q.z = OrientationMatrix.e[2][0]*P.x + OrientationMatrix.e[2][1]*P.y + OrientationMatrix.e[2][2]*P.z - OrientationMatrix.e[3][2];
	
	//! then use PointAABBAtOrigin method
	float d = PointAABBAtOrigin(Q,BoxSize,MinDistPoint);
	//! and transform intersection point back again in global space
	OrientationMatrix.TransformPoints(&MinDistPoint,1);

	//! probably a bug to check here : returned distance is computed in local space so no scale is applied even if transform matrix has a scale 
	return d;
}

/*!
	compute distance from a point (P) to an plane (Origin and Normal)
*/
float Distance::PointPlane(const Point3D &P, const Vector3D &PlaneNormal, const Point3D &PlaneOrigin)
{
	Vector3D u(PlaneOrigin,P, asVector{});
	return fabsf(Dot(PlaneNormal,u));
}