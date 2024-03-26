#pragma once
#include "TecLibs/Tec3D.h"

namespace Kigs
{
	namespace Maths
	{
		class BBox2D
		{
		public:
			BBox2D() { ; }
			BBox2D(const v2f& P) : m_Min(P), m_Max(P) { ; }
			BBox2D(const v2f& PMin, const v2f& PMax) : m_Min(PMin), m_Max(PMax) { ; }
			BBox2D(const v2f*& Array, int Count)
			{
				Init(Array, Count);
			}
			void Init(const v2f& P)
			{
				m_Min = P;
				m_Max = P;
			}
			void Init(const v2f* Array, int Count)
			{
				int i;
				Init(Array[0]);
				for (i = 1; i < Count; i++) Update(Array[i]);
			}
			void Init(const BBox2D& pBBox)
			{
				m_Min = pBBox.m_Min;
				m_Max = pBBox.m_Max;
			}

			void SetEmpty(void)
			{
				m_Min.x = m_Min.y = std::numeric_limits<float>::max();
				m_Max.x = m_Max.y = -std::numeric_limits<float>::max();
			}

			bool IsEmpty(void) const
			{
				return m_Max.x < m_Min.x || m_Max.y < m_Min.y;
			}

			void Update(const v2f& P)
			{
				if (P.x < m_Min.x) m_Min.x = P.x;
				if (P.y < m_Min.y) m_Min.y = P.y;
				if (P.x > m_Max.x) m_Max.x = P.x;
				if (P.y > m_Max.y) m_Max.y = P.y;
			}
			void Update(const v2f* Array, int Count)
			{
				int i;
				for (i = 0; i < Count; i++) Update(Array[i]);
			}
			void Update(const BBox2D& pBBox)
			{
				if (pBBox.m_Min.x < m_Min.x) m_Min.x = pBBox.m_Min.x;
				if (pBBox.m_Min.y < m_Min.y) m_Min.y = pBBox.m_Min.y;
				if (pBBox.m_Max.x > m_Max.x) m_Max.x = pBBox.m_Max.x;
				if (pBBox.m_Max.y > m_Max.y) m_Max.y = pBBox.m_Max.y;
			}
			void ConvertToPoint(v2f* Array) const
			{
				Array[0].x = m_Min.x;
				Array[0].y = m_Min.y;

				Array[1].x = m_Max.x;
				Array[1].y = m_Min.y;

				Array[2].x = m_Max.x;
				Array[2].y = m_Max.y;

				Array[3].x = m_Min.x;
				Array[3].y = m_Max.y;
			}

			void Center(v2f& aCenter) const
			{
				aCenter = (m_Min + m_Max) / 2.0f;
			}

			bool IsIn(const v2f& P) const
			{
				return (
					P.x >= m_Min.x && P.x <= m_Max.x
					&& P.y >= m_Min.y && P.y <= m_Max.y);
			}
			bool IsIn(const BBox2D& pBBox) const
			{
				return (
					pBBox.m_Min.x >= m_Min.x && pBBox.m_Max.x <= m_Max.x &&
					pBBox.m_Min.y >= m_Min.y && pBBox.m_Max.y <= m_Max.y);
			}
			bool DoesIntersect(const BBox2D& Other) const
			{
				return ((!(Other.m_Max.x < m_Min.x || Other.m_Min.x > m_Max.x)) &&
					(!(Other.m_Max.y < m_Min.y || Other.m_Min.y > m_Max.y)));
			}

			v2f Size() const
			{
				return m_Max - m_Min;
			}

			bool operator==(const BBox2D& Other) const
			{
				if (IsEmpty()) return Other.IsEmpty();
				else return (m_Min == Other.m_Min && m_Max == Other.m_Max);
			}

			bool operator!=(const BBox2D& Other) const
			{
				return !operator==(Other);
			}

			v2f m_Min, m_Max;
		};

	}
}
