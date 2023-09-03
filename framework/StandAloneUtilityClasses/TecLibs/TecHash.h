#pragma once

#include "CoreSTL.h"
#include "Tec3D.h"

namespace std
{
	template<>
	struct hash<Kigs::v2f>
	{
		size_t operator()(const Kigs::v2f& pt)
		{
			size_t hash = 0;
			Kigs::Core::hash_combine(hash, pt.x(), pt.y());
			return hash;
		}
	};

	template<>
	struct hash<Kigs::v3f>
	{
		size_t operator()(const Kigs::v3f& pt)
		{
			size_t hash = 0;
			Kigs::Core::hash_combine(hash, pt.x(), pt.y(), pt.z());
			return hash;
		}
	};

	template<>
	struct hash<Kigs::v4f>
	{
		size_t operator()(const Kigs::v4f& pt)
		{
			size_t hash = 0;
			Kigs::Core::hash_combine(hash, pt.x(), pt.y(), pt.z(), pt.w());
			return hash;
		}
	};
}