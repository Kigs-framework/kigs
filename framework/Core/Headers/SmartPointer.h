#pragma once

#include <memory>

namespace Kigs
{
	namespace Core
	{
		class CoreModifiable;

		// custom allocator for CoreModifiable
		// so shared pointers can be allocated in one block (using allocate_shared)
		// but custom destroy can call ProtectedDestroy (and send appropriate signals) when
		// the derived class still exists (so can call virtual methods)
		template <class T>
		class CMallocator
		{
		public:
			using value_type = T;

			CMallocator() noexcept {}
			template <class U> CMallocator(CMallocator<U> const&) noexcept {}

			value_type* allocate(std::size_t n)
			{
				return static_cast<value_type*>(::operator new (n * sizeof(value_type)));
			}

			void
				deallocate(value_type* p, std::size_t) noexcept
			{
				::operator delete(p);
			}

			template<class U>
			void destroy(U* p)
			{
				if constexpr (std::is_base_of_v<CoreModifiable, U>)
				{
					p->ProtectedDestroy();
				}
				p->~U();
			}
		};

		template <class T, class U>
		bool operator==(CMallocator<T> const&, CMallocator<U> const&) noexcept
		{
			return true;
		}

		template <class T, class U>
		bool operator!=(CMallocator<T> const& x, CMallocator<U> const& y) noexcept
		{
			return !(x == y);
		}


#ifdef _DEBUG
#ifdef WIN32
#define SmartPointerDebugCheck
#endif
#endif
#ifdef SmartPointerDebugCheck
		template<typename To, typename From>
		inline std::shared_ptr<To> debug_checked_pointer_cast(std::shared_ptr<From>&& ptr)
		{
			auto result = std::dynamic_pointer_cast<To>(ptr);
			if (result.get() != ptr.get())
			{
#ifdef WIN32			
				__debugbreak(); // Type mismatch
#endif		
			}
			return result;
		}
		template<typename To, typename From>
		inline std::shared_ptr<To> debug_checked_pointer_cast(const std::shared_ptr<From>& ptr)
		{
			auto result = std::dynamic_pointer_cast<To>(ptr);
			if (result.get() != ptr.get())
			{
#ifdef WIN32			
				__debugbreak(); // Type mismatch
#endif		
			}
			return result;
		}
#else
#define debug_checked_pointer_cast std::static_pointer_cast
#endif

		template<typename T>
		class SmartPointer : public std::shared_ptr<T>
		{
		public:
			using std::shared_ptr<T>::shared_ptr;

			// Auto casts
			template<typename U>
			SmartPointer(const std::shared_ptr<U>& other) : std::shared_ptr<T>(debug_checked_pointer_cast<T>(other))
			{
			}
			template<typename U>
			SmartPointer(std::shared_ptr<U>&& other) : std::shared_ptr<T>(debug_checked_pointer_cast<T>(other))
			{
			}
			template<typename U>
			SmartPointer<T>& operator=(const std::shared_ptr<U>& other)
			{
				std::shared_ptr<T>::operator=(debug_checked_pointer_cast<T>(other));
				return *this;
			}
			template<typename U>
			SmartPointer<T>& operator=(std::shared_ptr<U>&& other)
			{
				std::shared_ptr<T>::operator=(debug_checked_pointer_cast<T>(other));
				return *this;
			}
			template<typename U>
			operator SmartPointer<U>()
			{
				return debug_checked_pointer_cast<U>(*this);
			}
			template<typename U>
			bool operator==(const U* other) const
			{
				return this->get() == other;
			}
			template<typename U>
			bool operator!=(const U* other) const
			{
				return this->get() != other;
			}
		};

		template<typename smartPointOn, typename ... Args>
		SmartPointer<smartPointOn> MakeRefCounted(Args&& ... args)
		{
			if constexpr (std::is_base_of_v<CoreModifiable, smartPointOn>)
			{
				CMallocator<smartPointOn> cmalloc;
				return std::allocate_shared<smartPointOn>(cmalloc,std::forward<decltype(args)>(args)...);
			}
			else
				return std::make_shared<smartPointOn>(std::forward<decltype(args)>(args)...);
		}

		template<typename smartPointOn>
		using SP = SmartPointer<smartPointOn>;

	}
}