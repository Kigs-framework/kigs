#ifndef _SMARTPOINTER_H_
#define _SMARTPOINTER_H_
#include <memory>

#ifdef _DEBUG
#define SmartPointerDebugCheck
#endif
#ifdef SmartPointerDebugCheck
template<typename To, typename From>
inline std::shared_ptr<To> debug_checked_pointer_cast(std::shared_ptr<From>&& ptr)
{
	auto result = std::dynamic_pointer_cast<To>(ptr);
	if (result.get() != ptr.get())
	{
		__debugbreak(); // Type mismatch
	}
	return result;
}
template<typename To, typename From>
inline std::shared_ptr<To> debug_checked_pointer_cast(const std::shared_ptr<From>& ptr)
{
	auto result = std::dynamic_pointer_cast<To>(ptr);
	if (result.get() != ptr.get())
	{
		__debugbreak(); // Type mismatch
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
	return std::make_shared<smartPointOn>(std::forward<decltype(args)>(args)...);
}

template<typename smartPointOn>
using SP=SmartPointer<smartPointOn>;


#endif //_SMARTPOINTER_H_