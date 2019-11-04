#pragma once

#include "TecLibs/Tec3D.h"

class BBoxD
{
public:

	struct PreInit {};

	BBoxD() { ; }
	BBoxD(const v3d &P) : m_Min(P), m_Max(P) { ; }
	BBoxD(const v3d &PMin, const v3d &PMax) : m_Min(PMin), m_Max(PMax) { ; }
	BBoxD(const v3d* &Array, int Count)
	{
		Init(Array, Count);
	}

	BBoxD(PreInit p) : m_Min{ v3d(DBL_MAX, DBL_MAX, DBL_MAX) }, m_Max{ v3d(-DBL_MAX, -DBL_MAX, -DBL_MAX) } {}

	void Init(const v3d &P)
	{
		m_Min = P;
		m_Max = P;
	}
	void Init(const v3d* Array, int Count)
	{
		int i;
		Init(Array[0]);
		for (i = 1; i < Count; i++) Update(Array[i]);
	}
	void Init(const BBoxD &pBBoxD)
	{
		m_Min = pBBoxD.m_Min;
		m_Max = pBBoxD.m_Max;
	}

	inline double& operator[] (const int nIndex)
	{
		return *((&m_Min[0]) + nIndex);
	}

	void SetEmpty(void)
	{
		m_Min.x = m_Min.y = m_Min.z = DBL_MAX;
		m_Max.x = m_Max.y = m_Max.z = -DBL_MAX;
	}

	bool IsEmpty(void) const
	{
		return m_Max.x < m_Min.x || m_Max.y < m_Min.y || m_Max.z < m_Min.z;
	}

	v3d Center() const
	{
		return (m_Min + m_Max)*0.5;
	}

	v3d Size() const
	{
		return m_Max - m_Min;
	}

	void Update(const v3d &P)
	{
		if (P.x < m_Min.x) m_Min.x = P.x;
		if (P.y < m_Min.y) m_Min.y = P.y;
		if (P.z < m_Min.z) m_Min.z = P.z;
		if (P.x > m_Max.x) m_Max.x = P.x;
		if (P.y > m_Max.y) m_Max.y = P.y;
		if (P.z > m_Max.z) m_Max.z = P.z;
	}
	void Update(const v3d* Array, int Count)
	{
		int i;
		for (i = 0; i < Count; i++) Update(Array[i]);
	}
	void Update(const BBoxD &pBBoxD)
	{
		if (pBBoxD.m_Min.x < m_Min.x) m_Min.x = pBBoxD.m_Min.x;
		if (pBBoxD.m_Min.y < m_Min.y) m_Min.y = pBBoxD.m_Min.y;
		if (pBBoxD.m_Min.z < m_Min.z) m_Min.z = pBBoxD.m_Min.z;
		if (pBBoxD.m_Max.x > m_Max.x) m_Max.x = pBBoxD.m_Max.x;
		if (pBBoxD.m_Max.y > m_Max.y) m_Max.y = pBBoxD.m_Max.y;
		if (pBBoxD.m_Max.z > m_Max.z) m_Max.z = pBBoxD.m_Max.z;
	}
	void ConvertToPoint(v3d* Array) const
	{
		Array[0].x = m_Min.x;
		Array[0].y = m_Min.y;
		Array[0].z = m_Min.z;
		Array[1].x = m_Max.x;
		Array[1].y = m_Min.y;
		Array[1].z = m_Min.z;
		Array[2].x = m_Max.x;
		Array[2].y = m_Max.y;
		Array[2].z = m_Min.z;
		Array[3].x = m_Min.x;
		Array[3].y = m_Max.y;
		Array[3].z = m_Min.z;
		Array[4].x = m_Min.x;
		Array[4].y = m_Min.y;
		Array[4].z = m_Max.z;
		Array[5].x = m_Max.x;
		Array[5].y = m_Min.y;
		Array[5].z = m_Max.z;
		Array[6].x = m_Max.x;
		Array[6].y = m_Max.y;
		Array[6].z = m_Max.z;
		Array[7].x = m_Min.x;
		Array[7].y = m_Max.y;
		Array[7].z = m_Max.z;
	}
	bool IsIn(const v3d &P) const
	{
		return (
			P.x >= m_Min.x && P.x <= m_Max.x
			&& P.y >= m_Min.y && P.y <= m_Max.y
			&& P.z >= m_Min.z && P.z <= m_Max.z);
	}
	bool IsIn(const BBoxD &pBBoxD) const
	{
		return (
			pBBoxD.m_Min.x >= m_Min.x && pBBoxD.m_Max.x <= m_Max.x &&
			pBBoxD.m_Min.y >= m_Min.y && pBBoxD.m_Max.y <= m_Max.y &&
			pBBoxD.m_Min.z >= m_Min.z && pBBoxD.m_Max.z <= m_Max.z);
	}
	bool IsInWithAxisMask(const BBoxD &pBBoxD, unsigned int axisMask) const
	{
		return (
			((pBBoxD.m_Min.x >= m_Min.x && pBBoxD.m_Max.x <= m_Max.x) || ((axisMask & 1))) &&
			((pBBoxD.m_Min.y >= m_Min.y && pBBoxD.m_Max.y <= m_Max.y) || ((axisMask & 2))) &&
			((pBBoxD.m_Min.z >= m_Min.z && pBBoxD.m_Max.z <= m_Max.z) || ((axisMask & 4))));
	}
	bool DoesIntersect(const BBoxD &Other) const
	{
		return (!(Other.m_Max.x < m_Min.x || Other.m_Min.x > m_Max.x)) &&
			(!(Other.m_Max.y < m_Min.y || Other.m_Min.y > m_Max.y)) &&
			(!(Other.m_Max.z < m_Min.z || Other.m_Min.z > m_Max.z));
	}

	bool operator==(const BBoxD& Other) const
	{
		if (IsEmpty()) return Other.IsEmpty();
		else return (m_Min == Other.m_Min && m_Max == Other.m_Max);
	}

	bool operator!=(const BBoxD& Other) const
	{
		return !operator==(Other);
	}


	double	SquaredDistance(const v3d& p) const
	{
		v3d	dmax(p);
		dmax -= m_Max;
		v3d	dmin(m_Min);
		dmin -= p;

		dmax.x = ((dmax.x) > 0.0) ? (dmax.x) : 0.0;
		dmax.x = ((dmin.x) > dmax.x) ? (dmin.x) : dmax.x;
		dmax.y = ((dmax.y) > 0.0) ? (dmax.y) : 0.0;
		dmax.y = ((dmin.y) > dmax.y) ? (dmin.y) : dmax.y;
		dmax.z = ((dmax.z) > 0.0) ? (dmax.z) : 0.0;
		dmax.z = ((dmin.z) > dmax.z) ? (dmin.z) : dmax.z;

		return dmax.x*dmax.x + dmax.y*dmax.y + dmax.z*dmax.z;
	}

	double	Distance(const v3d& p) const
	{
		return sqrt(SquaredDistance(p));
	}

	v3d m_Min, m_Max;
};

