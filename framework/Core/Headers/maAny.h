#include "CoreModifiableAttribute.h"

#include <any>
#include <optional>

template<int notificationLevel>
class maAnyHeritage : public CoreModifiableAttributeData<std::any>
{
	DECLARE_ATTRIBUTE_HERITAGE_NO_ASSIGN(maAnyHeritage, maAnyHeritage, std::any, CoreModifiable::ANY);
public:
	virtual operator CurrentAttributeType() const
	{
		return _value; 
	}

	template<typename T>
	auto& operator=(const T& value)
	{
		_value = value;
		return *this;
	}

	template<typename T> 
	std::optional<T> get()
	{
		try
		{
			return std::any_cast<T>(_value);
		}
		catch (const std::bad_any_cast& e)
		{
			return {};
		}
	}
	


};

using maAny = maAnyHeritage<0>;