#ifndef _MABUFFER_H
#define _MABUFFER_H

#include "CoreModifiableAttribute.h"
#include "AsciiParserUtils.h"
#include "AttributeModifier.h"
#include "ModuleFileManager.h"

// ****************************************
// * maBufferHeritage class
// * --------------------------------------
/**
* \class	maBufferHeritage
* \ingroup	CoreModifiableAttibute
* \brief	CoreModifiableAttributeData of reference with different notification level
*/
// ****************************************
template<unsigned int alignement, typename allocatedType, int notificationLevel>
class maBufferHeritage : public CoreModifiableAttributeData<SmartPointer<AlignedCoreRawBuffer<alignement, allocatedType>>>
{
	template<int notiflevel>
	using TemplateForPlacementNew = maBufferHeritage<alignement, allocatedType, notiflevel>;
	using UnderlyingType = SmartPointer<AlignedCoreRawBuffer<alignement, allocatedType>>;

	using BaseType = CoreModifiableAttributeData<SmartPointer<AlignedCoreRawBuffer<alignement, allocatedType>>>;

	DECLARE_ATTRIBUTE_HERITAGE_NO_ASSIGN(maBufferHeritage, TemplateForPlacementNew, UnderlyingType, CoreModifiable::COREBUFFER);


	void InitData()
	{
		if (_value.isNil())
		{
			_value = OwningRawPtrToSmartPtr(new AlignedCoreRawBuffer<alignement, allocatedType>());
		}
	}

	void SetBufferFromString(const kstl::string& value)
	{
		InitData();

		// detect if value is a filename
		if (value[0] == '#')
		{
			kstl::string filename = value.substr(1, value.length() - 1);

			auto ext = filename.substr(filename.find_last_of('.'));
			u64 filelength = 0;
			auto loaded = OwningRawPtrToSmartPtr(ModuleFileManager::LoadFile(filename.c_str(), filelength));
			if (loaded)
			{
				if (ext == ".kbin")
				{
					CoreModifiable* uncompressManager = KigsCore::GetSingleton("KXMLManager");
					if (uncompressManager)
					{
						uncompressManager->SimpleCall("UncompressData", loaded.get(), _value.get());
					}
					else
					{
						KIGS_ERROR("trying to uncompress kxml, but KXMLManager was not declared", 1);
					}
				}
				else
					_value->SetBuffer(std::move(*loaded.get()));
			}

		}
		else
		{
			unsigned int readSize;
			unsigned char* rawbuf = AsciiParserUtils::StringToBuffer(value, readSize);
			_value->SetBuffer(rawbuf, readSize);
		}
	}


public:
	
	//! extra constructor with a string
	maBufferHeritage(CoreModifiable& owner, bool isInitAttribute, KigsID ID, const kstl::string& value) : BaseType(owner, isInitAttribute, ID)
	{
		SetBufferFromString(value);
	}

	/// setValue overloads
	virtual bool setValue(const char* value) override
	{
		kstl::string str(value); 
		return setValue(str);
	}
	virtual bool setValue(const kstl::string& value) override
	{
		if (this->isReadOnly())
			return false;
		SetBufferFromString(value);
		DO_NOTIFICATION(notificationLevel);
		return true;
	}
	virtual bool setValue(const CheckUniqueObject& value) override
	{
		if (this->isReadOnly())
			return false;

		_value = NonOwningRawPtrToSmartPtr((typename CurrentAttributeType::ValueType*)((RefCountedClass*)value));
		DO_NOTIFICATION(notificationLevel);
		return true;
	}


	/// getValue overloads
	virtual bool getValue(kstl::string& value) const override
	{

		if (!_value.isNil())
		{
			value = AsciiParserUtils::BufferToString((unsigned char*)_value->buffer(), _value->length());
			return true;
		}
		value = "";
		return true;
	}
	virtual bool getValue(CheckUniqueObject& value) const override
	{
		value = (RefCountedClass*)_value.get();
		return true;
	}
	virtual bool getValue(void*& value) const override { value = (RefCountedClass*)_value.get(); return true; }

	auto& operator=(const kstl::string &value)
	{
		SetBufferFromString(value);
		return *this;
	}
	auto& operator=(CoreRawBuffer* value)
	{
		InitData();
		_value->SetBuffer(*value);
		return *this;
	}

	char * buffer() const
	{
		return _value.isNil() ? nullptr :  _value->buffer();
	}
	unsigned int	length() const
	{
		return _value.isNil() ? 0u :_value->length();
	}
	void	SetBuffer(void* buffer, unsigned int length, bool manageMemory = true)
	{
		InitData();
		_value->SetBuffer(buffer, length, manageMemory);
	}
	operator const CoreRawBuffer*() const { return _value.get(); }


};




using maBuffer = maBufferHeritage<16, char, 0>;



#endif //_MABUFFER_H