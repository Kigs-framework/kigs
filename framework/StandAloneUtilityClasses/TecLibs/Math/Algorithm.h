#pragma once

#include <algorithm>

// Scalar to go from a (origin) to p' using b-a as unit vector
// Requires Dot(point_type, point_type) and operator-
template<typename point_type>
float ProjectOnLineScalar(point_type p, point_type a, point_type b)
{
	return Dot(p-a, b-a) / Dot(b-a, b-a);
}

// Projection of p on line ab
template<typename point_type>
point_type ProjectOnLine(point_type p, point_type a, point_type b)
{
	return a + (b-a) * ProjectOnLineScalar(p, a, b);
}


template<typename T>
inline T clamp(T v, T a, T b)
{
	return v<a ? a : (v>b?b:v); 
}

// Return -1, 0 or 1
template<typename T>
inline int sign(T v)
{
	return v<T(0) ? -1 : (v>T(0)?1:0);
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
	return (T)(a*(1 - t) + b*t);
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

inline bool angle_match(float a, float b, float epsilon = fPI / 12.0f)
{
	float diff = abs(a - b);
	return std::min(diff, 2 * fPI - diff) < epsilon;
}

inline bool angle_match_mod_pi(float a, float b, float epsilon = fPI / 12.0f)
{
	return (angle_match(a, b, epsilon) || angle_match(a > fPI ? a - fPI : a + fPI, b, epsilon));
}

inline auto distBetweenLines(v3f p0, v3f v0, v3f p1, v3f v1, v3f& out0, v3f& out1)
{
	//Vector   u = L1.P1 - L1.P0;
	//Vector   v = L2.P1 - L2.P0;
	v3f		 w = p0 - p1;
	float    a = Dot(v0, v0);         // always >= 0
	float    b = Dot(v0, v1);
	float    c = Dot(v1, v1);         // always >= 0
	float    d = Dot(v0, w);
	float    e = Dot(v1, w);
	float    D = a*c - b*b;        // always >= 0
	float    sc, tc;

	// compute the line parameters of the two closest points
	if (D < SMALL_NUM) {          // the lines are almost parallel
		sc = 0.0;
		tc = (b>c ? d / b : e / c);    // use the largest denominator
	}
	else {
		sc = (b*e - c*d) / D;
		tc = (a*e - b*d) / D;
	}

	out0 = sc * v0;
	out1 = tc * v1;
	out1 = -w + out1;

	// get the difference of the two closest points
	//Vector3D   dP = w + out0 - out1;  // =  L1(sc) - L2(tc)
	v3f   dP = out0 - out1;  // =  L1(sc) - L2(tc)

	return Norm(dP);   // return the closest distance
};

enum class orthonormalize_keep_axis
{
	x, y, z
};
inline mat3x4 orthonormalize_matrix(mat3x4 m, orthonormalize_keep_axis keep_axis = orthonormalize_keep_axis::y)
{
	m.XAxis.Normalize();
	m.YAxis.Normalize();
	m.ZAxis.Normalize();
	if (keep_axis == orthonormalize_keep_axis::x)
	{
		m.YAxis = m.ZAxis ^ m.XAxis;
		m.YAxis.Normalize();
		m.ZAxis = m.XAxis ^ m.YAxis;
		m.ZAxis.Normalize();
	}
	else if (keep_axis == orthonormalize_keep_axis::y)
	{
		m.ZAxis = m.XAxis ^ m.YAxis;
		m.ZAxis.Normalize();
		m.XAxis = m.YAxis ^ m.ZAxis;
		m.XAxis.Normalize();
	}
	else
	{
		m.XAxis = m.YAxis ^ m.ZAxis;
		m.XAxis.Normalize();
		m.YAxis = m.ZAxis ^ m.XAxis;
		m.YAxis.Normalize();
	}
	return m;
}
