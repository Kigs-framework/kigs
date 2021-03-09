#pragma once 

#include "CoreModifiableAttribute.h"

#include <any>
#include <optional>

template<int notificationLevel>
class maAnyHeritage : public CoreModifiableAttributeData<std::any>
{
	DECLARE_ATTRIBUTE_HERITAGE_NO_ASSIGN(maAnyHeritage, maAnyHeritage, std::any, CoreModifiable::ATTRIBUTE_TYPE::ANY);
public:
	virtual operator CurrentAttributeType() const
	{
		return mValue; 
	}

	template<typename T>
	auto& operator=(const T& value)
	{
		mValue = value;
		return *this;
	}

	template<typename T> 
	std::optional<T> get()
	{
		T* value = std::any_cast<T>(&mValue);
		if (value) return *value;
		return {};
	}
	
	template<typename T>
	T* getAny()
	{
		return std::any_cast<T>(&mValue);
	}

};

using maAny = maAnyHeritage<0>;