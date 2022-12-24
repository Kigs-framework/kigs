#pragma once

#include <type_traits>
#include <iterator>

#define FWD(a) std::forward<decltype(a)>(a)
#define REQUIRES(...) typename std::enable_if<(__VA_ARGS__), int>::type = 0

namespace Kigs
{
	namespace Core
	{
		template<typename T>
		using stripped_type = std::remove_pointer_t<std::decay_t<T>>;


		// Detection idiom

		template< class... >
		using void_t = void;

		struct nonesuch
		{
			nonesuch() = delete;
			~nonesuch() = delete;
			nonesuch(nonesuch const&) = delete;
			void operator=(nonesuch const&) = delete;
		};


		namespace detail
		{
			template <class Default, class AlwaysVoid,
				template<class...> class Op, class... Args>
			struct detector
			{
				using value_t = std::false_type;
				using type = Default;
			};

			template <class Default, template<class...> class Op, class... Args>
			struct detector<Default, void_t<Op<Args...>>, Op, Args...>
			{
				using value_t = std::true_type;
				using type = Op<Args...>;
			};

		} // namespace detail

		template <template<class...> class Op, class... Args>
		using is_detected = typename detail::detector<nonesuch, void, Op, Args...>::value_t;

		template <template<class...> class Op, class... Args>
		constexpr bool is_detected_v = is_detected<Op, Args...>::value;

		template <template<class...> class Op, class... Args>
		using detected_t = typename detail::detector<nonesuch, void, Op, Args...>::type;

		template <class Default, template<class...> class Op, class... Args>
		using detected_or = detail::detector<Default, void, Op, Args...>;

		template <class Expected, template<class...> class Op, class... Args>
		using is_detected_exact = std::is_same<Expected, detected_t<Op, Args...>>;

		template<class T>
		using has_begin = decltype(std::begin(std::declval<T&>()));

		template<class T>
		using has_end = decltype(std::end(std::declval<T&>()));

		template<class T>
		using has_resize = decltype(std::declval<T&>().resize((size_t)0u));

		template<class T>
		using has_bracket_operator = decltype(std::declval<T&>()[std::declval<typename T::key_type&>()]);

		template<class T>
		using has_key_type = decltype(std::declval<typename T::key_type&>());

		template<class T>
		using has_first_type = decltype(std::declval<typename T::first_type&>());

		template<class T>
		using has_second_type = decltype(std::declval<typename T::second_type&>());

		template<class T>
		using has_allocator_type = decltype(std::declval<typename T::allocator_type&>());

		template<class T>
		using is_smart_pointer = decltype(std::declval<T&>().use_count());

		template <class T1, class ... T>
		inline void hash_combine(std::size_t& seed, const T1& value, const T& ... args) noexcept
		{
			std::hash<T1> hasher;
			seed ^= hasher(value) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
			//#ifdef ANDROID
			//	if (sizeof...(args) != 0)
			//#else
			if constexpr (sizeof...(args) != 0)
				//#endif
			{
				hash_combine(seed, FWD(args)...);
			}
		}

		// Overload for std::visit

		template <class... Fs>
		struct overload;

		template <class F0, class... Frest>
		struct overload<F0, Frest...> : F0, overload<Frest...>
		{
			constexpr overload(F0&& f0, Frest&&... rest) : F0(FWD(f0)), overload<Frest...>(FWD(rest)...) {}

			using F0::operator();
			using overload<Frest...>::operator();
		};

		template <class F0>
		struct overload<F0> : F0
		{
			constexpr overload(F0&& f0) : F0(FWD(f0)) {}

			using F0::operator();
		};

		template <class... Fs>
		constexpr auto make_overload(Fs&&... fs)
		{
			return overload<Fs...>(FWD(fs)...);
		}

#ifndef kigs_defer
		struct kigs_defer_dummy {};
		template <class F> struct kigs_deferrer { F f; ~kigs_deferrer() { f(); } };
		template <class F> kigs_deferrer<F> operator*(kigs_defer_dummy, F f) { return { f }; }
#define KIGS_DEFER_(LINE) zz_kigs_defer##LINE
#define KIGS_DEFER(LINE) KIGS_DEFER_(LINE)
#define kigs_defer auto KIGS_DEFER(__LINE__) = kigs_defer_dummy {} *[&]()
#endif // kigs_defer

		template<typename Container>
		typename Container::mapped_type* FindOrNull(Container& container, const typename Container::key_type& key)
		{
			auto it = container.find(key);
			if (it != container.end()) return &it->second;
			return nullptr;
		}
	}
}