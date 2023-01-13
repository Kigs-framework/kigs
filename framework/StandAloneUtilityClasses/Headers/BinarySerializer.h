#pragma once

#include "CoreSTL.h"
#include "TecLibs/Tec3D.h"
#include "AsciiParserUtils.h"

#include <limits>
#include <algorithm>
#include <vector>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <memory>

namespace Kigs
{
	using namespace Kigs::Core;

	template<class T, class T2>
	using has_member_serialize = decltype(std::declval<T&>().Serialize(std::declval<T2&>()));

	template<class T>
	using has_update = decltype(std::declval<T&>().Update());

	inline constexpr u32 constexpr_popcount(u32 x)
	{
		auto a = x - ((x >> 1) & 0x55555555);
		auto b = (((a >> 2) & 0x33333333) + (a & 0x33333333));
		auto c = (((b >> 4) + b) & 0x0f0f0f0f);
		auto d = c + (c >> 8);
		auto e = d + (d >> 16);
		return e & 0x0000003f;
	}

	inline constexpr u32 constexpr_log2(u32 x)
	{
		auto a = x | (x >> 1);
		auto b = a | (a >> 2);
		auto c = b | (b >> 4);
		auto d = c | (c >> 8);
		auto e = d | (d >> 16);
		auto f = e >> 1;
		return constexpr_popcount(f);
	}

	inline u32	GetFirstLeftBit(u32 n)
	{
		static u32 addlast16[16] = { 0,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3 };

		u32 result = 0;

		if (n & 0xffff0000)
		{
			result = 16;
			n >>= 16;
		}

		if (n & 0xff00)
		{
			result += 8;
			n >>= 8;
		}

		if (n & 0xf0)
		{
			result += 4;
			n >>= 4;
		}

		result += addlast16[n];

		return result;
	}

	template<typename T>
	inline constexpr u32 bits_required(T min_value, T max_value)
	{
		if (min_value == max_value) return 0u;
		u32 result = 0u;

		u64 diff = 0;
		if constexpr (std::is_same_v<T, u64>)
		{
			diff = max_value - min_value;
		}
		else
		{
			diff = (s64)max_value - (s64)min_value;
		}

		if (diff > std::numeric_limits<u32>::max())
		{
			diff >>= 32llu;
			result += 32u;
		}

		//result += constexpr_log2(u32(diff)) + 1;
		result += GetFirstLeftBit(u32(diff)) + 1;
		return result;
	}

	//#define CHECK_SERIALIZE(a) if(!a) { __debugbreak(); return false; };
#define CHECK_SERIALIZE(a) if(!a) return false;
#define CHECK_SERIALIZE_VER(ver, a) if(loaded_version >= ver) if(!a) return false;
#define CHECK_SERIALIZE_VERSION_RANGE(ver_start, ver_end, a) if(loaded_version >= ver_start && loaded_version <= ver_end) if(!a) return false;

#define CO_CHECK_SERIALIZE(a) if(!a) co_return false;
#define CO_CHECK_SERIALIZE_VER(ver, a) if(loaded_version >= ver) if(!a) co_return false;
#define CO_CHECK_SERIALIZE_VERSION_RANGE(ver_start, ver_end, a) if(loaded_version >= ver_start && loaded_version <= ver_end) if(!a) co_return false;


#define SERIALIZE_VERSION(stream, ver) const u16 current_version = ver;\
		u16 loaded_version = current_version;\
		CHECK_SERIALIZE(serialize_object(stream, loaded_version));

#define CO_SERIALIZE_VERSION(stream, ver) const u16 current_version = ver;\
		u16 loaded_version = current_version;\
		CO_CHECK_SERIALIZE(serialize_object(stream, loaded_version));

	namespace serializer_detail
	{
		template<typename T, typename PacketStream, REQUIRES(std::is_integral_v<T>)>
		bool serialize(PacketStream& stream, T& value, T min_value = std::numeric_limits<T>::min(), T max_value = std::numeric_limits<T>::max())
		{
			//KIGS_ASSERT(min_value < max_value);

			std::remove_cv_t<T> temp_value = {};
			if constexpr (PacketStream::IsWriting)
			{
				//KIGS_ASSERT(value <= max_value);
				//KIGS_ASSERT(value >= min_value);
				temp_value = value;
			}

			if (!stream.SerializeInt(temp_value, min_value, max_value))
				return false;

			if constexpr (!PacketStream::IsWriting)
			{
				if (temp_value < min_value || temp_value > max_value)
				{
					return false;
				}
				value = temp_value;
			}
			return true;
		}

		template<typename PacketStream>
		bool serialize(PacketStream& stream, bool& value)
		{
			return stream.SerializeBool(value);
		}

		template<typename PacketStream, typename F, REQUIRES(std::is_enum_v<F>)>
		bool serialize(PacketStream& stream, F& value)
		{
			return serialize(stream, (std::underlying_type_t<F>&)value);
		}

		template<typename PacketStream, typename F, REQUIRES(std::is_floating_point_v<F>)>
		bool serialize(PacketStream& stream, F& value)
		{
			u64 value_u64 = 0;
			if constexpr (PacketStream::IsWriting)
				memcpy(&value_u64, &value, sizeof(F));

			if (!stream.SerializeInt(value_u64, 0llu, sizeof(F) == 4 ? std::numeric_limits<u32>::max() : std::numeric_limits<u64>::max()))
				return false;

			if constexpr (!PacketStream::IsWriting)
				memcpy(&value, &value_u64, sizeof(F));

			return true;
		}

		template<typename PacketStream>
		bool serialize(PacketStream& stream, v4f& value)
		{
			CHECK_SERIALIZE(serialize(stream, value.x));
			CHECK_SERIALIZE(serialize(stream, value.y));
			CHECK_SERIALIZE(serialize(stream, value.z));
			CHECK_SERIALIZE(serialize(stream, value.w));
			return true;
		}

		template<typename PacketStream>
		bool serialize(PacketStream& stream, v3f& value)
		{
			CHECK_SERIALIZE(serialize(stream, value.x));
			CHECK_SERIALIZE(serialize(stream, value.y));
			CHECK_SERIALIZE(serialize(stream, value.z));
			return true;
		}

		template<typename PacketStream>
		bool serialize(PacketStream& stream, v2f& value)
		{
			CHECK_SERIALIZE(serialize(stream, value.x));
			CHECK_SERIALIZE(serialize(stream, value.y));
			return true;
		}

		template<typename PacketStream>
		bool serialize(PacketStream& stream, v2i& value)
		{
			CHECK_SERIALIZE(serialize(stream, value.x));
			CHECK_SERIALIZE(serialize(stream, value.y));
			return true;
		}

		template<typename PacketStream>
		bool serialize(PacketStream& stream, mat3x4& value)
		{
			for (int i = 0; i < 4; ++i)
			{
				CHECK_SERIALIZE(serialize(stream, value.Axis[i]));
			}
			return true;
		}

		template<typename PacketStream>
		bool serialize(PacketStream& stream, SP<CoreRawBuffer>& value)
		{
			size_t size = 0;
			if constexpr (PacketStream::IsWriting)
			{
				if (value)
					size = value->size();
			}
			CHECK_SERIALIZE(serialize(stream, size));
			if (size == 0)
			{
				if (!PacketStream::IsWriting && value)
					value->clear();
				return true;
			}
			if constexpr (!PacketStream::IsWriting)
			{
				if (!value)
					value = MakeRefCounted<CoreRawBuffer>();
				value->resize(size);
			}
			CHECK_SERIALIZE(serialize_bytes(stream, (u8*)value->buffer(), (u64)value->size()));
			return true;
		}

		template<typename PacketStream, typename T>
		bool serialize_range(PacketStream& stream, T& range)
		{
			u64 count = 0;

			if constexpr (PacketStream::IsWriting)
			{
				count = std::distance(std::begin(range), std::end(range));
			}

			CHECK_SERIALIZE(serialize(stream, count));

			if constexpr (!PacketStream::IsWriting && is_detected_v<has_resize, T>)
			{
				range.resize((size_t)count);
			}

			for (auto& element : range)
			{
				/*CHECK_SERIALIZE*/(serialize_object(stream, element));
			}

			return true;
		}

		template<typename PacketStream, typename T>
		bool serialize_map_range(PacketStream& stream, T& range)
		{
			u64 count = 0;

			if constexpr (PacketStream::IsWriting)
			{
				count = std::size(range);
			}

			CHECK_SERIALIZE(serialize(stream, count));

			using key_type = std::remove_cv_t<typename T::key_type>;
			if constexpr (!PacketStream::IsWriting)
			{
				std::unordered_set<key_type> keys_to_erase;
				for (auto& [key, value] : range)
				{
					keys_to_erase.insert(key);
				}
				for (u64 i = 0; i < count; ++i)
				{
					key_type key;
					CHECK_SERIALIZE(serialize_object(stream, key));
					auto& value = range[key];
					CHECK_SERIALIZE(serialize_object(stream, value));
					keys_to_erase.erase(key);
				}
				for (auto& key : keys_to_erase)
				{
					range.erase(key);
				}
			}
			else
			{
				for (auto& it : range)
				{
					key_type key = it.first;
					CHECK_SERIALIZE(serialize_object(stream, key));
					CHECK_SERIALIZE(serialize_object(stream, it.second));
				}
			}
			return true;
		}

		template<typename PacketStream, typename T>
		bool serialize_set_range(PacketStream& stream, T& range)
		{
			u64 count = 0;

			if constexpr (PacketStream::IsWriting)
			{
				count = std::size(range);
			}

			CHECK_SERIALIZE(serialize(stream, count));
			using key_type = std::remove_cv_t<typename T::key_type>;
			if constexpr (!PacketStream::IsWriting)
			{
				range.clear();
				for (u64 i = 0; i < count; ++i)
				{
					key_type value;
					CHECK_SERIALIZE(serialize_object(stream, value));
					range.insert(std::move(value));
				}
			}
			else
			{
				for (auto& value : range)
				{
					CHECK_SERIALIZE(serialize_object(stream, value));
				}
			}
			return true;
		}

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

		template<typename PacketStream, typename ... Types>
		bool serialize(PacketStream& stream, std::variant<Types...>& t)
		{
			size_t index = 0;
			if constexpr (PacketStream::IsWriting)
				index = t.index();
			CHECK_SERIALIZE(serialize_object(stream, index));

			if constexpr (!PacketStream::IsWriting)
				t = variant_from_index<std::variant<Types...>>(index);

			return std::visit([&](auto&& r)
				{
					return serialize_object(stream, r);
				}, t);
		}

		template<typename PacketStream, typename T1, typename T2>
		bool serialize(PacketStream& stream, std::pair<T1, T2>& t)
		{
			CHECK_SERIALIZE(serialize_object(stream, t.first));
			CHECK_SERIALIZE(serialize_object(stream, t.second));
			return true;
		}

		template <typename Tuple, typename F, size_t... Is>
		bool tuple_for_each(Tuple&& t, F&& f, std::index_sequence<Is...>)
		{
			auto l = { f(std::get<Is>(t))... };
			return std::all_of(l.begin(), l.end(), [](auto a) { return a; });
		}

		template<typename PacketStream, typename ... Args>
		bool serialize(PacketStream& stream, std::tuple<Args...>& t)
		{
			return tuple_for_each(t, [&](auto&& el) { return serialize_object(stream, el); }, std::index_sequence_for<Args...>());
		}

		template<typename PacketStream, typename T>
		bool serialize(PacketStream& stream, std::unique_ptr<T>& t)
		{
			if constexpr (!PacketStream::IsWriting)
			{
				t = std::make_unique<T>();
			}
			return serialize_object(stream, *t.get());
		}
	}


	/*template<typename T, typename PacketStream, typename ... Args>
	bool serialize_object(PacketStream& stream, T&& value, Args&& ... args)
	{
		if constexpr (is_detected_v<has_member_serialize, T, PacketStream>)
		{
			return value.Serialize(stream, FWD(args)...);
		}
		else if constexpr (is_detected_v<has_begin, T> && is_detected_v<has_end, T>)
		{
			using namespace serializer_detail;
			if constexpr (is_detected_v<has_key_type, T>)
			{
				if constexpr (is_detected_v<has_bracket_operator, T>)
					return serialize_map_range(stream, value, FWD(args)...);
				else
					return serialize_set_range(stream, value, FWD(args)...);
			}
			else
			{
				return serialize_range(stream, value, FWD(args)...);
			}
		}
		else if constexpr (is_detected_v<has_first_type, T> && is_detected_v<has_second_type, T>)
		{
			using namespace serializer_detail;
			CHECK_SERIALIZE(serialize_object(stream, value.first));
			CHECK_SERIALIZE(serialize_object(stream, value.second));
			return true;
		}
		else
		{
			using namespace serializer_detail;
			return serialize(stream, FWD(value), FWD(args)...);
		}
		return false;
	}*/

	template<typename T, typename PacketStream, typename ... Args>
	bool serialize_object(PacketStream& stream, T& value, Args&& ... args)
	{
		if constexpr (is_detected_v<has_member_serialize, T, PacketStream>)
		{
			return value.Serialize(stream, FWD(args)...);
		}
		else if constexpr (is_detected_v<has_begin, T> && is_detected_v<has_end, T>)
		{
			using namespace serializer_detail;
			if constexpr (is_detected_v<has_key_type, T>)
			{
				if constexpr (is_detected_v<has_bracket_operator, T>)
					return serialize_map_range(stream, value, FWD(args)...);
				else
					return serialize_set_range(stream, value, FWD(args)...);
			}
			else
			{
				return serialize_range(stream, value, FWD(args)...);
			}
		}
		else if constexpr (is_detected_v<has_first_type, T> && is_detected_v<has_second_type, T>)
		{
			using namespace serializer_detail;
			CHECK_SERIALIZE(serialize_object(stream, value.first));
			CHECK_SERIALIZE(serialize_object(stream, value.second));
			return true;
		}
		else
		{
			using namespace serializer_detail;
			return serialize(stream, value, FWD(args)...);
		}
		return false;
	}


	template<typename PacketStream>
	bool serialize_bytes(PacketStream& stream, u8* data, u64 size)
	{
		return stream.SerializeBytes(data, size);
	}


	struct BitPacker
	{
		BitPacker(u32* data, size_t size)
		{
			buffer = data;
			max_size = size;
		}

		u64 scratch = 0;
		s32 scratch_bits = 0;
		s32 word_index = 0;
		u32* buffer = nullptr;
		size_t max_size = 0;

		void serialize_bits(u32 value, s32 bits)
		{
			auto mask = ((1llu << bits) - 1);
			scratch |= (u64(value) & mask) << scratch_bits;
			scratch_bits += bits;
			if (scratch_bits >= 32)
			{
				*buffer++ = u32(scratch & 0xFFFFFFFF);
				word_index++;
				scratch_bits -= 32;
				scratch = scratch >> 32llu;
			}
		}

		void flush()
		{
			if (scratch_bits)
			{
				*buffer++ = u32(scratch & 0xFFFFFFFF);
				word_index++;
				scratch_bits = 0;
				scratch = 0;
			}
		}

		void merge(const BitPacker& with)
		{
			if (word_index == with.word_index)
			{
				scratch |= with.scratch;
			}
		}
	};

	struct VectorBitPacker
	{
		VectorBitPacker() = default;

		VectorBitPacker(std::vector<u32>& data)
		{
			buffer = &data;
		}

		u64 scratch = 0;
		s32 scratch_bits = 0;
		s32 word_index = 0;

		std::vector<u32>* buffer = nullptr;

		void serialize_bits(u32 value, s32 bits)
		{
			scratch |= u64(value) << scratch_bits;
			scratch_bits += bits;
			if (scratch_bits >= 32)
			{
				if (word_index == buffer->size())
					buffer->push_back(u32(scratch & 0xFFFFFFFF));
				else
					(*buffer)[word_index] = u32(scratch & 0xFFFFFFFF);

				word_index++;
				scratch_bits -= 32;
				scratch = scratch >> 32llu;
			}
		}

		void flush()
		{
			if (scratch_bits)
			{
				if (word_index == buffer->size())
					buffer->push_back(u32(scratch & 0xFFFFFFFF));
				else
					(*buffer)[word_index] = u32(scratch & 0xFFFFFFFF);

				word_index++;
				scratch_bits = 0;
				scratch = 0;
			}
		}

		void merge_into(VectorBitPacker& into)
		{
			if (word_index == into.word_index)
			{
				into.scratch |= scratch;
			}
			else
			{
				(*buffer)[word_index] |= scratch;
			}
		}
	};

	struct BitUnpacker
	{
		u64 scratch = 0;
		s32 scratch_bits = 0;
		u64 total_bits = 0;
		u64 num_bits_read = 0;
		s32 word_index = 0;
		u32* buffer_start = nullptr;

		bool would_read_past_end(u64 bits)
		{
			return num_bits_read + bits > total_bits;
		}

		void serialize_bits(u32& value, s32 bits)
		{
			if (scratch_bits < bits)
			{
				scratch |= u64(buffer_start[word_index]) << scratch_bits;
				scratch_bits += 32;
				word_index++;
			}

			value = scratch & ((1llu << bits) - 1);
			scratch_bits -= bits;
			scratch = scratch >> bits;
			num_bits_read += bits;
		}

		void align_u32()
		{
			if ((scratch_bits % 32) != 0)
			{
				if (would_read_past_end(scratch_bits))
				{
					num_bits_read = total_bits;
					word_index = total_bits / 32;
					scratch_bits = 0;
					scratch = 0;
				}
				else
				{
					u32 value;
					serialize_bits(value, scratch_bits);
				}
			}
		}

		template<typename PacketStream>
		bool Serialize(PacketStream& stream)
		{
			CHECK_SERIALIZE(serialize_object(stream, total_bits));

			u8* data_location = nullptr;
			if constexpr (PacketStream::IsWriting)
			{
				data_location = (u8*)buffer_start;
			}
			else
			{
				auto& vec = *(std::vector<u32>*)stream.user_data;
				vec.resize(total_bits / 32);
				data_location = (u8*)vec.data();
				buffer_start = (u32*)vec.data();
			}
			CHECK_SERIALIZE(serialize_bytes(stream, data_location, total_bits / 8));
			CHECK_SERIALIZE(serialize_object(stream, scratch));
			CHECK_SERIALIZE(serialize_object(stream, scratch_bits));
			CHECK_SERIALIZE(serialize_object(stream, num_bits_read));
			CHECK_SERIALIZE(serialize_object(stream, word_index));
			return true;
		}

	};

	template<typename UnPackerTypeTemplate>
	struct BasePacketReadStream
	{
		constexpr static bool IsWriting = false;

		using UnPackerType = UnPackerTypeTemplate;

		BasePacketReadStream() = default;
		BasePacketReadStream(const BasePacketReadStream& other)
		{
			*this = other;
		}
		BasePacketReadStream(BasePacketReadStream&&) = default;
		BasePacketReadStream& operator=(const BasePacketReadStream& other)
		{
			if (this == &other) return *this;
			packer = other.packer;
			backing_buffer = other.backing_buffer;
			if (!backing_buffer.empty())
			{
				packer.buffer_start = backing_buffer.data();
			}
			return *this;
		}
		BasePacketReadStream& operator=(BasePacketReadStream&&) = default;

		BasePacketReadStream(std::vector<u32> buffer) : backing_buffer{ std::move(buffer) }
		{
			packer.buffer_start = backing_buffer.data();
			packer.total_bits = u64(backing_buffer.size()) * sizeof(u32) * 8llu;
		}

		BasePacketReadStream(void* b, size_t size, bool make_copy = false)
		{
			if (make_copy)
			{
				backing_buffer.resize(size / 4);
				memcpy(backing_buffer.data(), b, size);
				packer.buffer_start = backing_buffer.data();
			}
			else packer.buffer_start = (u32*)b;
			packer.total_bits = u64(size) * 8llu;
		}

		template<typename T>
		bool SerializeInt(T& value, T min_value, T max_value)
		{
			const s32 bits = bits_required(min_value, max_value);

			if (packer.would_read_past_end(bits)) { return false; }

			if constexpr (sizeof(T) > 4)
			{
				s32 remaining_bits = bits - 32;
				u32 low_bits = 0;
				packer.serialize_bits(low_bits, std::min(bits, 32));
				u64 value_u64 = low_bits;
				if (remaining_bits > 0)
				{
					u32 high_bits = 0;
					packer.serialize_bits(high_bits, remaining_bits);
					value_u64 |= u64(high_bits) << 32llu;
				}
				if (bits == sizeof(T) * 8)
				{
					memcpy(&value, &value_u64, sizeof(T));
				}
				else
				{
					// When not using the full 64 bits, value_u32 always fit in a s64
					value = (T)((s64)value_u64 + min_value);
				}
			}
			else
			{
				u32 value_u32;
				packer.serialize_bits(value_u32, bits);


				if (bits == sizeof(T) * 8)
				{
					memcpy(&value, &value_u32, sizeof(T));
				}
				else
				{
					// When not using the full 32 bits, value_u32 always fit in a s32
					value = (T)((s32)value_u32 + min_value);
				}
			}
			return true;
		}

		bool SerializeBool(bool& value)
		{
			if (packer.would_read_past_end(1)) { return false; }
			u32 value_u32 = 0;
			packer.serialize_bits(value_u32, 1);
			value = value_u32;
			return true;
		}

		bool SerializeBytes(u8* data, u64 size)
		{
			if (packer.would_read_past_end(size * 8)) { return false; }
			auto write = data;
			while (size != 0)
			{
				auto count = s32(std::min(size, 4llu));
				u32 value = 0;
				packer.serialize_bits(value, count * 8);
				memcpy(write, &value, count);
				size -= count;
				write += count;
			}
			return true;
		}

		bool SkipBits(u64 bits)
		{
			if (packer.would_read_past_end(bits)) { return false; }
			while (bits != 0)
			{
				auto count = s32(std::min(bits, 32llu));
				u32 value = 0;
				packer.serialize_bits(value, count);
				bits -= count;
			}
			return true;
		}

		UnPackerType GetState()
		{
			return packer;
		}

		void SetState(const UnPackerType& p)
		{
			packer = p;
		}

		UnPackerType packer;
		std::vector<u32> backing_buffer;

		void* user_data = nullptr;
	};

	using PacketReadStream = BasePacketReadStream<BitUnpacker>;

	template<typename PackerType>
	struct BasePacketWriteStream
	{
		constexpr static bool IsWriting = true;

		BasePacketWriteStream() = default;


		template<typename ... Args>
		BasePacketWriteStream(Args&& ... args) : packer{ FWD(args)... }
		{
		}

		~BasePacketWriteStream()
		{
			Flush();
		}

		void Flush()
		{
			packer.flush();
		}

		template<typename T>
		bool SerializeInt(T value, T min_value, T max_value)
		{
			const s32 bits = bits_required(min_value, max_value);

			u64 value_u64 = 0;
			if (bits == sizeof(T) * 8)
			{
				memcpy(&value_u64, &value, sizeof(T));
			}
			else
			{
				value_u64 = value - min_value;
			}
			if constexpr (sizeof(T) > 4)
			{
				s32 remaining_bits = bits - 32;
				u32 low_bits = u32(value_u64 & 0x00000000FFFFFFFF);
				packer.serialize_bits(low_bits, std::min(bits, 32));

				if (remaining_bits > 0)
				{
					u32 high_bits = u32((value_u64 >> 32) & 0x00000000FFFFFFFF);
					packer.serialize_bits(high_bits, remaining_bits);
				}
			}
			else
			{
				packer.serialize_bits(u32(value_u64 & 0x00000000FFFFFFFF), bits);
			}
			return true;
		}

		bool SerializeBool(bool value)
		{
			u32 value_u32 = value ? 1u : 0u;
			packer.serialize_bits(value_u32, 1);
			return true;
		}

		bool SerializeBytes(u8* data, u64 size)
		{
			auto read = data;
			while (size != 0)
			{
				auto count = s32(std::min(size, 4llu));
				u32 value = 0;
				memcpy(&value, read, count);
				packer.serialize_bits(value, count * 8);
				size -= count;
				read += count;
			}

			return true;
		}


		template<typename T>
		bool SerializeAtPos(T& thing, const PackerType& at_pos)
		{
			auto before = packer;
			packer = at_pos;
			CHECK_SERIALIZE(serialize_object(*this, thing));
			packer.merge_into(before);
			packer = before;
			return true;
		}

		PackerType packer;

		void* user_data = nullptr;
	};

	using PacketWriteStream = BasePacketWriteStream<BitPacker>;
	using VectorWriteStream = BasePacketWriteStream<VectorBitPacker>;

	template<typename T>
	std::string SaveToString(T&& thing, void* user_data = nullptr)
	{
		std::vector<u32> data;
		VectorWriteStream stream{ data };
		stream.user_data = user_data;
		if (!serialize_object(stream, thing)) return "";
		stream.Flush();
		return AsciiParserUtils::BufferToString((unsigned char*)data.data(), data.size() * sizeof(u32));
	}

	template<typename T>
	bool LoadFromString(T&& thing, const std::string& str, void* user_data = nullptr)
	{
		size_t size = 0;
		auto data = AsciiParserUtils::StringToBuffer(str, size);
		kigs_defer{ delete[] data; };
		PacketReadStream stream{ data, size };
		stream.user_data = user_data;
		return serialize_object(stream, thing);
	}

	template<typename T>
	bool LoadFromBuffer(T&& thing, CoreRawBuffer* buffer, void* user_data = nullptr)
	{
		PacketReadStream stream{ buffer->data(), buffer->size() };
		stream.user_data = user_data;
		return serialize_object(stream, thing);
	}
}