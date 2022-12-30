#pragma once

#include "LuaIntf/LuaIntf.h"

namespace Kigs
{
	namespace Lua
	{
		inline LuaIntf::LuaRef V3F_TO_TABLE(LuaIntf::LuaState L, v3f v)
		{
			L.newTable();
			auto ref = L.popValue<LuaIntf::LuaRef>();
			ref[1] = v.x;
			ref[2] = v.y;
			ref[3] = v.z;
			return ref;
		}

		inline v3f TABLE_TO_V3F(LuaIntf::LuaRef ref)
		{
			v3f result;
			result.x = ref[1].value<float>();
			result.y = ref[2].value<float>();
			result.z = ref[3].value<float>();
			return result;
		}

	}
}