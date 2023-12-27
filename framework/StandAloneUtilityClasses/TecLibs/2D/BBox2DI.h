#pragma once
#include "TecLibs/Tec3D.h"
#include "TecLibs/3D/3DObject/BBoxI.h"

namespace Kigs
{
	namespace Maths
	{
		class BBox2DI
		{
		public:
			BBox2DI() { ; }
			BBox2DI(const BBoxI& from3DBox)
			{
				m_Min.x = from3DBox.m_Min.x;
				m_Min.y = from3DBox.m_Min.y;
				m_Max.x = from3DBox.m_Max.x;
				m_Max.y = from3DBox.m_Max.y;
			}
			BBox2DI(int px, int py) : m_Min(px, py), m_Max(px, py) { ; }
			BBox2DI(const v2i& P) : m_Min(P), m_Max(P) { ; }
			//BBox2DI(const Point3DI &P)  : m_Min(P.xy), m_Max(P.xy){;}
			BBox2DI(const v2i& PMin, const v2i& PMax) : m_Min(PMin), m_Max(PMax) { ; }
			BBox2DI(const v2i*& Array, int Count)
			{
				Init(Array, Count);
			}
			void Init(const v2i& P)
			{
				m_Min = P;
				m_Max = P;
			}
			void Init(int px, int py)
			{
				m_Min = { px, py };
				m_Max = { px, py };
			}
			/*void Init(const Point3DI &P)
			{
				m_Min = P.xy;
				m_Max = P.xy;
			}*/
			void Init(const v2i* Array, int Count)
			{
				int i;
				Init(Array[0]);
				for (i = 1; i < Count; i++) Update(Array[i]);
			}
			void Init(const BBox2DI& pBBoxI)
			{
				m_Min = pBBoxI.m_Min;
				m_Max = pBBoxI.m_Max;
			}

			void SetEmpty(void)
			{
				m_Min.x = m_Min.y = 1;
				m_Max.x = m_Max.y = -1;
			}

			bool IsEmpty(void) const
			{
				return m_Max.x < m_Min.x || m_Max.y < m_Min.y;
			}

			void Update(const v2i& P)
			{
				if (IsEmpty())
				{
					Init(P);
				}
				else
				{
					if (P.x < m_Min.x) m_Min.x = P.x;
					if (P.y < m_Min.y) m_Min.y = P.y;

					if (P.x > m_Max.x) m_Max.x = P.x;
					if (P.y > m_Max.y) m_Max.y = P.y;
				}
			}

			void Update(int px, int py)
			{
				if (IsEmpty())
				{
					Init(px, py);
				}
				else
				{
					if (px < m_Min.x) m_Min.x = px;
					if (py < m_Min.y) m_Min.y = py;

					if (px > m_Max.x) m_Max.x = px;
					if (py > m_Max.y) m_Max.y = py;
				}
			}
			/*
			void Update(const Point3DI &P)
			{
				if(IsEmpty())
				{
					Init(P);
				}
				else
				{
					if (P.x<m_Min.x) m_Min.x = P.x;
					if (P.y<m_Min.y) m_Min.y = P.y;

					if (P.x>m_Max.x) m_Max.x = P.x;
					if (P.y>m_Max.y) m_Max.y = P.y;
				}
			}*/

			void Update(const v2i* Array, int Count)
			{
				int i;
				for (i = 0; i < Count; i++) Update(Array[i]);
			}
			void Update(const BBox2DI& pBBoxI)
			{
				if (IsEmpty())
				{
					Init(pBBoxI.m_Min);
					Update(pBBoxI.m_Max);
				}
				else
				{
					if (pBBoxI.m_Min.x < m_Min.x) m_Min.x = pBBoxI.m_Min.x;
					if (pBBoxI.m_Min.y < m_Min.y) m_Min.y = pBBoxI.m_Min.y;

					if (pBBoxI.m_Max.x > m_Max.x) m_Max.x = pBBoxI.m_Max.x;
					if (pBBoxI.m_Max.y > m_Max.y) m_Max.y = pBBoxI.m_Max.y;

				}
			}
			void ConvertToPoint(v2i* Array) const
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
			bool IsIn(const v2i& P) const
			{
				return (
					P.x >= m_Min.x && P.x <= m_Max.x
					&& P.y >= m_Min.y && P.y <= m_Max.y);
			}
			bool IsIn(const BBox2DI& pBBoxI) const
			{
				return (
					pBBoxI.m_Min.x >= m_Min.x && pBBoxI.m_Max.x <= m_Max.x &&
					pBBoxI.m_Min.y >= m_Min.y && pBBoxI.m_Max.y <= m_Max.y);
			}
			bool DoesIntersect(const BBox2DI& Other) const
			{
				return (!(Other.m_Max.x < m_Min.x || Other.m_Min.x > m_Max.x)) &&
					(!(Other.m_Max.y < m_Min.y || Other.m_Min.y > m_Max.y));
			}

			bool operator==(const BBox2DI& Other) const
			{
				if (IsEmpty()) return Other.IsEmpty();
				else return (m_Min == Other.m_Min && m_Max == Other.m_Max);
			}

			bool operator!=(const BBox2DI& Other) const
			{
				return !operator==(Other);
			}

			v2i Size() const
			{
				if (IsEmpty())
				{
					return v2i(0, 0);
				}

				return v2i(m_Max.x - m_Min.x + 1, m_Max.y - m_Min.y + 1);
			}

			v2i m_Min, m_Max;
		};

	}
}
