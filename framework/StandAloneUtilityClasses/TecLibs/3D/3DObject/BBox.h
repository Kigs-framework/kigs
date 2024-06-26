#pragma once
#include "TecLibs/Tec3D.h"
#include <limits>

namespace Kigs
{

	namespace Maths
	{

		class BBox
		{
		public:

			struct PreInit {};

			BBox() { ; }
			BBox(const v3f& P) : m_Min(P), m_Max(P) { ; }
			BBox(const v3f& PMin, const v3f& PMax) : m_Min(PMin), m_Max(PMax) { ; }
			BBox(const v3f*& Array, int Count)
			{
				Init(Array, Count);
			}

			BBox(const BBox& bb) : m_Min(bb.m_Min), m_Max(bb.m_Max) { ; }

			BBox(PreInit p) : m_Min{ v3f(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()) }
							, m_Max{ v3f(std::numeric_limits<float>::min(), std::numeric_limits<float>::min(), std::numeric_limits<float>::min()) } {}

			void Init(const v3f& P)
			{
				m_Min = P;
				m_Max = P;
			}
			void Init(const v3f* Array, int Count)
			{
				int i;
				Init(Array[0]);
				for (i = 1; i < Count; i++) Update(Array[i]);
			}
			void Init(const BBox& pBBox)
			{
				m_Min = pBBox.m_Min;
				m_Max = pBBox.m_Max;
			}

			inline float& operator[] (const int nIndex)
			{
				return *((&m_Min[0]) + nIndex);
			}

			void SetEmpty(void)
			{
				m_Min.x = m_Min.y = m_Min.z = std::numeric_limits<float>::max();
				m_Max.x = m_Max.y = m_Max.z = std::numeric_limits<float>::min();
			}

			bool IsEmpty(void) const
			{
				return m_Max.x < m_Min.x || m_Max.y < m_Min.y || m_Max.z < m_Min.z;
			}

			v3f Center() const
			{
				return (m_Min + m_Max) * 0.5f;
			}

			v3f Size() const
			{
				return m_Max - m_Min;
			}

			void Update(const v3f& P)
			{
				if (P.x < m_Min.x) m_Min.x = P.x;
				if (P.y < m_Min.y) m_Min.y = P.y;
				if (P.z < m_Min.z) m_Min.z = P.z;
				if (P.x > m_Max.x) m_Max.x = P.x;
				if (P.y > m_Max.y) m_Max.y = P.y;
				if (P.z > m_Max.z) m_Max.z = P.z;
			}
			void Update(const v3f* Array, int Count)
			{
				int i;
				for (i = 0; i < Count; i++) Update(Array[i]);
			}
			void Update(const BBox& pBBox)
			{
				if (pBBox.m_Min.x < m_Min.x) m_Min.x = pBBox.m_Min.x;
				if (pBBox.m_Min.y < m_Min.y) m_Min.y = pBBox.m_Min.y;
				if (pBBox.m_Min.z < m_Min.z) m_Min.z = pBBox.m_Min.z;
				if (pBBox.m_Max.x > m_Max.x) m_Max.x = pBBox.m_Max.x;
				if (pBBox.m_Max.y > m_Max.y) m_Max.y = pBBox.m_Max.y;
				if (pBBox.m_Max.z > m_Max.z) m_Max.z = pBBox.m_Max.z;
			}
			void ConvertToPoint(v3f* Array) const
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
			bool IsIn(const v3f& P) const
			{
				return (
					P.x >= m_Min.x && P.x <= m_Max.x
					&& P.y >= m_Min.y && P.y <= m_Max.y
					&& P.z >= m_Min.z && P.z <= m_Max.z);
			}
			bool IsIn(const BBox& pBBox) const
			{
				return (
					pBBox.m_Min.x >= m_Min.x && pBBox.m_Max.x <= m_Max.x &&
					pBBox.m_Min.y >= m_Min.y && pBBox.m_Max.y <= m_Max.y &&
					pBBox.m_Min.z >= m_Min.z && pBBox.m_Max.z <= m_Max.z);
			}
			bool IsInWithAxisMask(const BBox& pBBox, unsigned int axisMask) const
			{
				return (
					((pBBox.m_Min.x >= m_Min.x && pBBox.m_Max.x <= m_Max.x) || ((axisMask & 1))) &&
					((pBBox.m_Min.y >= m_Min.y && pBBox.m_Max.y <= m_Max.y) || ((axisMask & 2))) &&
					((pBBox.m_Min.z >= m_Min.z && pBBox.m_Max.z <= m_Max.z) || ((axisMask & 4))));
			}
			bool DoesIntersect(const BBox& Other) const
			{
				return (!(Other.m_Max.x < m_Min.x || Other.m_Min.x > m_Max.x)) &&
					(!(Other.m_Max.y < m_Min.y || Other.m_Min.y > m_Max.y)) &&
					(!(Other.m_Max.z < m_Min.z || Other.m_Min.z > m_Max.z));
			}

			bool operator==(const BBox& Other) const
			{
				if (IsEmpty()) return Other.IsEmpty();
				else return (m_Min == Other.m_Min && m_Max == Other.m_Max);
			}

			bool operator!=(const BBox& Other) const
			{
				return !operator==(Other);
			}


			BBox& operator=(const BBox& bb)
			{
				m_Min = bb.m_Min;
				m_Max = bb.m_Max;
				return *this;
			}

			float	SquaredDistance(const v3f& p) const
			{
				v3f	dmax(p);
				dmax -= m_Max;
				v3f	dmin(m_Min);
				dmin -= p;

				dmax.x = ((dmax.x) > 0.0f) ? (dmax.x) : 0.0f;
				dmax.x = ((dmin.x) > dmax.x) ? (dmin.x) : dmax.x;
				dmax.y = ((dmax.y) > 0.0f) ? (dmax.y) : 0.0f;
				dmax.y = ((dmin.y) > dmax.y) ? (dmin.y) : dmax.y;
				dmax.z = ((dmax.z) > 0.0f) ? (dmax.z) : 0.0f;
				dmax.z = ((dmin.z) > dmax.z) ? (dmin.z) : dmax.z;

				return dmax.x * dmax.x + dmax.y * dmax.y + dmax.z * dmax.z;
			}

			float	Distance(const v3f& p) const
			{
				return sqrtf(SquaredDistance(p));
			}

			union {
				struct
				{
					v3f m_Min, m_Max;
				};
				v3f	m_MinMax[2];
			};


		};
	}
}
