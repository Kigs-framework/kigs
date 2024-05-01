#pragma once

#include <algorithm>

namespace Kigs
{
	namespace Maths
	{

		// Scalar to go from a (origin) to p' using b-a as unit vector
		// Requires dot(point_type, point_type) and operator-
		template<typename point_type>
		float ProjectOnLineScalar(point_type p, point_type a, point_type b)
		{
			return dot(p - a, b - a) / dot(b - a, b - a);
		}

		// Projection of p on line ab
		template<typename point_type>
		point_type ProjectOnLine(point_type p, point_type a, point_type b)
		{
			return a + (b - a) * ProjectOnLineScalar(p, a, b);
		}

		// Keep main vector the same, return orthogonalized side_vector
		inline v3f Orthogonalize(v3f main_vector, v3f side_vector)
		{
			auto other_side_vector = normalize(cross(main_vector,side_vector));
			side_vector = normalize(cross(other_side_vector,main_vector));
			return side_vector;
		}


		template<typename T>
		inline T clamp(T v, T a, T b)
		{
			return v < a ? a : (v > b ? b : v);
		}

		// Return -1, 0 or 1
		template<typename T>
		inline int sign(T v)
		{
			return v < T(0) ? -1 : (v > T(0) ? 1 : 0);
		}

		// Return -1, 0 or 1 (1 is left of vector going from a to b)
		template<typename point_type>
		inline int SideOfLine(point_type p, point_type a, point_type b)
		{
			auto area = (b.x - a.x) * (p.y - a.y) - (b.y - a.y) * (p.x - a.x);
			if (area > 0)
				return -1;
			else if (area < 0)
				return 1;
			return 0;
		}


		template<typename T, typename scalar_type>
		T lerp(const T& a, const T& b, scalar_type t)
		{
			return (T)(a * (1 - t) + b * t);
		}

		template<typename T>
		inline T clampV2(T v, T minv, T maxv)
		{
			v.x = clamp(v.x, minv.x, maxv.x);
			v.y = clamp(v.y, minv.y, maxv.y);
			return v;
		}


		template<typename iterator_type, typename FGetter, typename FComp>
		auto minimize(iterator_type itbegin, iterator_type itend, FGetter DistanceGetter, FComp comparator) -> std::pair<iterator_type, decltype(DistanceGetter(*itbegin))>
		{
			if (itbegin == itend) return{ itend, {} };
			auto it = itbegin;
			auto best_distance = DistanceGetter(*itbegin);
			auto itbest = itbegin;
			++it;
			for (; it != itend; ++it)
			{
				auto distance = DistanceGetter(*it);
				if (comparator(distance, best_distance))
				{
					best_distance = distance;
					itbest = it;
				}
			}

			return{ itbest, best_distance };
		}

		inline bool angle_match(float a, float b, float epsilon = glm::pi<float>() / 12.0f)
		{
			float diff = abs(a - b);
			return std::min(diff, 2.0f * glm::pi<float>() - diff) < epsilon;
		}

		inline bool angle_match_mod_pi(float a, float b, float epsilon = glm::pi<float>() / 12.0f)
		{
			return (angle_match(a, b, epsilon) || angle_match(a > glm::pi<float>() ? a - glm::pi<float>() : a + glm::pi<float>(), b, epsilon));
		}

		inline auto distBetweenLines(v3f p0, v3f v0, v3f p1, v3f v1, v3f& out0, v3f& out1)
		{
			//Vector   u = L1.P1 - L1.P0;
			//Vector   v = L2.P1 - L2.P0;
			v3f		 w = p0 - p1;
			float    a = dot(v0, v0);         // always >= 0
			float    b = dot(v0, v1);
			float    c = dot(v1, v1);         // always >= 0
			float    d = dot(v0, w);
			float    e = dot(v1, w);
			float    D = a * c - b * b;        // always >= 0
			float    sc, tc;

			// compute the line parameters of the two closest points
			if (D < 0.000001) {          // the lines are almost parallel
				sc = 0.0;
				tc = (b > c ? d / b : e / c);    // use the largest denominator
			}
			else {
				sc = (b * e - c * d) / D;
				tc = (a * e - b * d) / D;
			}

			out0 = sc * v0;
			out1 = tc * v1;
			out1 = -w + out1;

			// get the difference of the two closest points
			//v3f   dP = w + out0 - out1;  // =  L1(sc) - L2(tc)
			v3f   dP = out0 - out1;  // =  L1(sc) - L2(tc)

			return length(dP);   // return the closest distance
		};

		enum class orthonormalize_keep_axis
		{
			x, y, z
		};
		inline mat4 orthonormalize_matrix(mat4 m, orthonormalize_keep_axis keep_axis = orthonormalize_keep_axis::y)
		{

			m = column(m, 0, v4f(normalize(v3f(column(m ,0))), 0.0f));
			m = column(m, 1, v4f(normalize(v3f(column(m, 1))), 0.0f));
			m = column(m, 2, v4f(normalize(v3f(column(m, 2))), 0.0f));

			if (keep_axis == orthonormalize_keep_axis::x)
			{
				m = column(m, 1, v4f(normalize(cross(v3f(column(m, 2)) , v3f(column(m, 0)))), 0.0f));
				m = column(m, 2, v4f(normalize(cross(v3f(column(m, 0)) , v3f(column(m, 1)))), 0.0f));
			}
			else if (keep_axis == orthonormalize_keep_axis::y)
			{
				m = column(m, 2, v4f(normalize(cross(v3f(column(m, 0)) , v3f(column(m, 1)))), 0.0f));
				m = column(m, 0, v4f(normalize(cross(v3f(column(m, 1)) , v3f(column(m, 2)))), 0.0f));
			}
			else
			{
				m = column(m, 0, v4f(normalize(cross(v3f(column(m, 1)) , v3f(column(m, 2)))), 0.0f));
				m = column(m, 1, v4f(normalize(cross(v3f(column(m, 2)) , v3f(column(m, 0)))), 0.0f));
			}
			return m;
		}
	}
}