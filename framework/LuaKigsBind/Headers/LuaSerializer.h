#pragma once

#include "CoreSTL.h"
#include "CoreTypes.h"

#include "TecLibs/Tec3D.h"

#include "LuaIntf/LuaIntf.h"

#include <sstream>

#include <set>
#include <unordered_set>
#include <map>
#include <vector>
#include <iomanip>
#include <variant>

namespace Kigs
{
	namespace Lua
	{
		template <class Variant, std::size_t I = 0>
		Variant variant_from_index(std::size_t index)
		{
			if constexpr (I >= std::variant_size_v<Variant>)
				throw std::runtime_error{ "Variant index " + std::to_string(I + index) + " out of bounds" };
			else
				return index == 0
				? Variant{ std::in_place_index<I> }
			: variant_from_index<Variant, I + 1>(index - 1);
		}

		template<typename T>
		std::string ToLuaTable(T&& t, std::string prefix = "");

		inline std::string ToLua(const v3f& t, std::string prefix)
		{
			return "{" + ToLuaTable(t.x) + ", " + ToLuaTable(t.y) + ", " + ToLuaTable(t.z) + "}";
		}

		inline std::string ToLua(const v4f& t, std::string prefix)
		{
			return "{" + ToLuaTable(t.x) + ", " + ToLuaTable(t.y) + ", " + ToLuaTable(t.z) + ", " + ToLuaTable(t.w) + "}";
		}

		inline std::string ToLua(const bool& t, std::string prefix)
		{
			return t ? "true" : "false";
		}

		template<typename ... Types>
		std::string ToLua(const std::variant<Types...>& t, std::string prefix)
		{
			std::string result = "{";
			result += "\n" + prefix + "\tvariant_type = " + ToLuaTable(t.index(), prefix + "\t") + ",";
			auto visit_result = std::visit([&](auto&& r)
				{
					return ToLua(r, prefix);
				}, t);
			visit_result.erase(0, 1);
			return result + visit_result;
		}

		template<typename T>
		std::string ToLua(const std::vector<T>& t, std::string prefix)
		{
			if (t.empty()) return "{}";
			std::string result = "\n" + prefix + "{";
			for (auto&& el : t)
			{
				result += "\n" + prefix + "\t" + ToLuaTable(el, prefix + "\t") + ",";
			}
			if (t.size()) result.back() = '\n';
			result += prefix + "}";
			return result;
		}

		template<typename T>
		std::string ToLua(const std::set<T>& t, std::string prefix)
		{
			if (t.empty()) return "{}";
			std::string result = "\n" + prefix + "{";
			for (auto&& el : t)
			{
				result += "\n" + prefix + "\t" + ToLuaTable(el, prefix + "\t\t") + ",";
			}
			if (t.size()) result.back() = '\n';
			result += prefix + "}";
			return result;
		}

		template<typename T>
		std::string ToLua(const std::unordered_set<T>& t, std::string prefix)
		{
			if (t.empty()) return "{}";
			std::string result = "\n" + prefix + "{";
			for (auto&& el : t)
			{
				result += "\n" + prefix + "\t" + ToLuaTable(el, prefix + "\t\t") + ",";
			}
			if (t.size()) result.back() = '\n';
			result += prefix + "}";
			return result;
		}

		template<typename K, typename V>
		std::string ToLua(const std::map<K, V>& t, std::string prefix)
		{
			if (t.empty()) return "{}";
			std::string result = "\n" + prefix + "{";
			for (auto& [key, value] : t)
			{
				result += "\n" + prefix + "\t{";
				result += "\n" + prefix + "\t\t" + ToLuaTable(key, prefix + "\t\t") + ",";
				result += "\n" + prefix + "\t\t" + ToLuaTable(value, prefix + "\t\t");
				result += "\n" + prefix + "\t},";
			}
			if (t.size())
			{
				result.back() = '\n';
			}
			result += prefix + "}";
			return result;
		}

		template<typename K, typename V>
		std::string ToLua(const std::unordered_map<K, V>& t, std::string prefix)
		{
			if (t.empty()) return "{}";
			std::string result = "\n" + prefix + "{";
			for (auto& [key, value] : t)
			{
				result += "\n" + prefix + "\t{";
				result += "\n" + prefix + "\t\t" + ToLuaTable(key, prefix + "\t\t") + ",";
				result += "\n" + prefix + "\t\t" + ToLuaTable(value, prefix + "\t\t");
				result += "\n" + prefix + "\t},";
			}
			if (t.size())
			{
				result.back() = '\n';
			}
			result += prefix + "}";
			return result;
		}


		template<typename T1, typename T2>
		std::string ToLua(const std::pair<T1, T2>& t, std::string prefix)
		{
			return "{" + ToLuaTable(t.first) + ", " + ToLuaTable(t.second) + "}";
		}

		inline std::string ToLua(const std::string& str, std::string prefix)
		{
			std::stringstream ss;
			ss << std::quoted(str);
			return ss.str();
		}

		template<typename T>
		std::string ToLua(const T& t, std::string prefix)
		{
			return std::to_string(t);
		}

		template<typename T>
		std::string ToLuaTable(T&& t, std::string prefix)
		{
			if constexpr (std::is_enum_v<std::remove_cvref_t<T>>)
			{
				auto val = (std::underlying_type_t<std::remove_cvref_t<T>>)t;
				return ToLua(val, prefix);
			}
			else
				return ToLua(FWD(t), prefix);
		}

		template<typename T>
		void FromLuaTable(T& t, LuaIntf::LuaRef ref);

		template<typename T>
		void FromLuaField(T& t, LuaIntf::LuaRef ref, const std::string field_name)
		{
			if (ref.has(field_name))
			{
				FromLuaTable(t, ref[field_name]);
			}
		}

		/*template<typename T>
		void FromLua(std::set<T>& t, LuaIntf::LuaRef ref)
		{
			t.clear();
			for (auto el : ref)
			{
				T value;
				FromLuaTable(value, el.value<LuaIntf::LuaRef>());
				t.insert(value);
			}
		}

		template<typename K, typename V>
		void FromLua(std::map<K, V>& t, LuaIntf::LuaRef ref)
		{
			t.clear();
			for (auto el : ref)
			{
				std::pair<K, V> pair;
				FromLuaTable(pair, el.value<LuaIntf::LuaRef>());
				t.insert(pair);
			}
		}

		template<typename T>
		void FromLua(std::vector<T>& t, LuaIntf::LuaRef ref)
		{
			t.clear();
			for (auto el : ref)
			{
				auto& value = t.emplace_back();
				FromLuaTable(value, el.value<LuaIntf::LuaRef>());
			}
		}*/

		template<typename T1, typename T2>
		void FromLua(std::tuple<T1, T2>& t, LuaIntf::LuaRef ref)
		{
			int i = 0;
			for (auto el : ref)
			{
				if (i == 0)
				{
					FromLuaTable(std::get<0>(t), el.value<LuaIntf::LuaRef>());
				}
				else if (i == 1)
				{
					FromLuaTable(std::get<1>(t), el.value<LuaIntf::LuaRef>());
				}
				++i;
			}
		}

		template<typename T1, typename T2, typename T3>
		void FromLua(std::tuple<T1, T2, T3>& t, LuaIntf::LuaRef ref)
		{
			int i = 0;
			for (auto el : ref)
			{
				if (i == 0)
				{
					FromLuaTable(std::get<0>(t), el.value<LuaIntf::LuaRef>());
				}
				else if (i == 1)
				{
					FromLuaTable(std::get<1>(t), el.value<LuaIntf::LuaRef>());
				}
				else if (i == 2)
				{
					FromLuaTable(std::get<2>(t), el.value<LuaIntf::LuaRef>());
				}
				++i;
			}
		}

		template<typename T1, typename T2, typename T3, typename T4>
		void FromLua(std::tuple<T1, T2, T3, T4>& t, LuaIntf::LuaRef ref)
		{
			int i = 0;
			for (auto el : ref)
			{
				if (i == 0)
				{
					FromLuaTable(std::get<0>(t), el.value<LuaIntf::LuaRef>());
				}
				else if (i == 1)
				{
					FromLuaTable(std::get<1>(t), el.value<LuaIntf::LuaRef>());
				}
				else if (i == 2)
				{
					FromLuaTable(std::get<2>(t), el.value<LuaIntf::LuaRef>());
				}
				else if (i == 4)
				{
					FromLuaTable(std::get<3>(t), el.value<LuaIntf::LuaRef>());
				}
				++i;
			}
		}

		template<typename T1, typename T2>
		void FromLua(std::pair<T1, T2>& t, LuaIntf::LuaRef ref)
		{
			int i = 0;
			for (auto el : ref)
			{
				if (i == 0)
				{
					FromLuaTable(t.first, el.value<LuaIntf::LuaRef>());
				}
				else if (i == 1)
				{
					FromLuaTable(t.second, el.value<LuaIntf::LuaRef>());
				}
				++i;
			}
		}

		inline void FromLua(v2f& t, LuaIntf::LuaRef ref)
		{
			int i = 0;
			for (auto el : ref)
			{
				if (i == 0)
				{
					FromLuaTable(t.x, el.value<LuaIntf::LuaRef>());
				}
				else if (i == 1)
				{
					FromLuaTable(t.y, el.value<LuaIntf::LuaRef>());
				}
				++i;
			}
		}

		inline void FromLua(v3f& t, LuaIntf::LuaRef ref)
		{
			int i = 0;
			for (auto el : ref)
			{
				if (i == 0)
				{
					FromLuaTable(t.x, el.value<LuaIntf::LuaRef>());
				}
				else if (i == 1)
				{
					FromLuaTable(t.y, el.value<LuaIntf::LuaRef>());
				}
				else if (i == 2)
				{
					FromLuaTable(t.z, el.value<LuaIntf::LuaRef>());
				}
				++i;
			}
		}

		inline void FromLua(v4f& t, LuaIntf::LuaRef ref)
		{
			int i = 0;
			for (auto el : ref)
			{
				if (i == 0)
				{
					FromLuaTable(t.x, el.value<LuaIntf::LuaRef>());
				}
				else if (i == 1)
				{
					FromLuaTable(t.y, el.value<LuaIntf::LuaRef>());
				}
				else if (i == 2)
				{
					FromLuaTable(t.z, el.value<LuaIntf::LuaRef>());
				}
				else if (i == 3)
				{
					FromLuaTable(t.w, el.value<LuaIntf::LuaRef>());
				}
				++i;
			}
		}

		template<typename ... Types>
		void FromLua(std::variant<Types...>& t, LuaIntf::LuaRef ref)
		{
			size_t index = 0;
			FromLuaField(index, ref, "variant_type");
			t = variant_from_index<std::variant<Types...>>(index);
			std::visit([&](auto& r)
				{
					FromLua(r, ref);
				}, t);
		}

		template<typename T>
		void FromLuaTable(T& t, LuaIntf::LuaRef ref)
		{
			if constexpr (std::is_same_v<T, bool>)
			{
				t = ref.toValue<bool>();
			}
			else if constexpr (std::is_arithmetic_v<T>)
			{
				t = (T)ref.toValue<double>();
			}
			else if constexpr (std::is_same_v<T, std::string>)
			{
				t = ref.toValue<std::string>();
			}
			else if constexpr (std::is_enum_v<std::remove_cvref_t<T>>)
			{
				t = (T)ref.toValue<int>();
			}
			else if constexpr (is_detected_v<has_begin, T> && is_detected_v<has_end, T>)
			{
				if constexpr (is_detected_v<has_key_type, T>)
				{
					if constexpr (is_detected_v<has_bracket_operator, T>)
					{
						// map-like
						t.clear();
						for (auto el : ref)
						{
							std::pair<typename T::key_type, typename T::mapped_type> pair;
							FromLuaTable(pair, el.value<LuaIntf::LuaRef>());
							t.insert(pair);
						}
					}
					else
					{
						// set-like
						t.clear();
						for (auto el : ref)
						{
							typename T::key_type value;
							FromLuaTable(value, el.value<LuaIntf::LuaRef>());
							t.insert(value);
						}
					}
				}
				else
				{
					// vector-like
					t.clear();
					for (auto el : ref)
					{
						auto& value = t.emplace_back();
						FromLuaTable(value, el.value<LuaIntf::LuaRef>());
					}
				}
			}
			else
			{
				FromLua(t, ref);
			}
		}

#define TO_LUA_FIELD(field) result += "\n" + prefix + "\t" + ToLowerCase(#field) + " = " + ToLuaTable(t.field, prefix + "\t") + ",";
#define FROM_LUA_FIELD(field) FromLuaField(r.field, ref, ToLowerCase(#field));

#define SERIALIZE_LUA(type, ...) inline std::string ToLua(const type& t, std::string prefix)\
{\
	std::string result = "{";\
	FOR_EACH(TO_LUA_FIELD, __VA_ARGS__)\
	result.back() = '\n';\
	result += prefix + "}";\
	return result;\
}\
inline void FromLua(type& r, LuaIntf::LuaRef ref)\
{\
	FOR_EACH(FROM_LUA_FIELD, __VA_ARGS__)\
}

	}
}