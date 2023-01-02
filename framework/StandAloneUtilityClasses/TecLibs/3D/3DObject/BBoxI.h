#pragma once

#include "TecLibs/Tec3D.h"

namespace Kigs
{
	namespace Maths
	{
		template<typename Point3DType, typename coordType>
		class BBoxIBase
		{
		public:
			BBoxIBase() { ; }
			BBoxIBase(coordType minx, coordType miny, coordType minz, coordType maxx, coordType maxy, coordType maxz) : m_Min(minx, miny, minz), m_Max(maxx, maxy, maxz) { ; }
			BBoxIBase(const Point3DType& P) : m_Min(P), m_Max(P) { ; }
			BBoxIBase(const Point3DType& PMin, const Point3DType& PMax) : m_Min(PMin), m_Max(PMax) { ; }
			BBoxIBase(const Point3DType*& Array, coordType Count)
			{
				Init(Array, Count);
			}
			void Init(const Point3DType& P)
			{
				m_Min = P;
				m_Max = P;
			}
			void Init(const Point3DType* Array, coordType Count)
			{
				coordType i;
				Init(Array[0]);
				for (i = 1; i < Count; i++) Update(Array[i]);
			}
			void Init(const BBoxIBase& pBBoxI)
			{
				m_Min = pBBoxI.m_Min;
				m_Max = pBBoxI.m_Max;
			}

			void SetEmpty(void)
			{
				m_Min.x = m_Min.y = m_Min.z = 1;
				m_Max.x = m_Max.y = m_Max.z = -1;
			}

			bool IsEmpty(void) const
			{
				return m_Max.x < m_Min.x || m_Max.y < m_Min.y || m_Max.z < m_Min.z;
			}

			void Update(const Point3DType& P)
			{
				if (IsEmpty())
				{
					Init(P);
				}
				else
				{
					if (P.x < m_Min.x) m_Min.x = P.x;
					if (P.y < m_Min.y) m_Min.y = P.y;
					if (P.z < m_Min.z) m_Min.z = P.z;
					if (P.x > m_Max.x) m_Max.x = P.x;
					if (P.y > m_Max.y) m_Max.y = P.y;
					if (P.z > m_Max.z) m_Max.z = P.z;
				}
			}

			void Update(coordType mx, coordType my, coordType mz)
			{
				if (IsEmpty())
				{
					m_Min.Set(mx, my, mz);
					m_Max = m_Min;
				}
				else
				{
					if (mx < m_Min.x) m_Min.x = mx;
					if (my < m_Min.y) m_Min.y = my;
					if (mz < m_Min.z) m_Min.z = mz;
					if (mx > m_Max.x) m_Max.x = mx;
					if (my > m_Max.y) m_Max.y = my;
					if (mz > m_Max.z) m_Max.z = mz;
				}
			}

			void Update(const Point2DI& P)
			{
				if (IsEmpty())
				{
					//KIGS_ERROR("BBoxIBase can not be init with Point2DI",2);
				}
				else
				{
					if (P.x < m_Min.x) m_Min.x = P.x;
					if (P.y < m_Min.y) m_Min.y = P.y;

					if (P.x > m_Max.x) m_Max.x = P.x;
					if (P.y > m_Max.y) m_Max.y = P.y;
				}
			}

			void Update(const Point3DType* Array, coordType Count)
			{
				coordType i;
				for (i = 0; i < Count; i++) Update(Array[i]);
			}
			void Update(const BBoxIBase& pBBoxI)
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
					if (pBBoxI.m_Min.z < m_Min.z) m_Min.z = pBBoxI.m_Min.z;
					if (pBBoxI.m_Max.x > m_Max.x) m_Max.x = pBBoxI.m_Max.x;
					if (pBBoxI.m_Max.y > m_Max.y) m_Max.y = pBBoxI.m_Max.y;
					if (pBBoxI.m_Max.z > m_Max.z) m_Max.z = pBBoxI.m_Max.z;
				}
			}
			void ConvertToPoint(Point3DType* Array) const
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
			bool IsIn(const Point3DType& P) const
			{
				return (
					P.x >= m_Min.x && P.x <= m_Max.x
					&& P.y >= m_Min.y && P.y <= m_Max.y
					&& P.z >= m_Min.z && P.z <= m_Max.z);
			}
			bool IsIn(const BBoxIBase& pBBoxI) const
			{
				return (
					pBBoxI.m_Min.x >= m_Min.x && pBBoxI.m_Max.x <= m_Max.x &&
					pBBoxI.m_Min.y >= m_Min.y && pBBoxI.m_Max.y <= m_Max.y &&
					pBBoxI.m_Min.z >= m_Min.z && pBBoxI.m_Max.z <= m_Max.z);
			}
			bool DoesIntersect(const BBoxIBase& Other) const
			{
				return (!(Other.m_Max.x < m_Min.x || Other.m_Min.x > m_Max.x)) &&
					(!(Other.m_Max.y < m_Min.y || Other.m_Min.y > m_Max.y)) &&
					(!(Other.m_Max.z < m_Min.z || Other.m_Min.z > m_Max.z));
			}

			bool operator==(const BBoxIBase& Other) const
			{
				if (IsEmpty()) return Other.IsEmpty();
				else return (m_Min == Other.m_Min && m_Max == Other.m_Max);
			}

			bool operator!=(const BBoxIBase& Other) const
			{
				return !operator==(Other);
			}

			Point3DType m_Min, m_Max;
		};

		typedef BBoxIBase<Point3DI, int> BBoxI;
		typedef BBoxIBase<Point3DSI, short> BBoxSI;

	}
}
