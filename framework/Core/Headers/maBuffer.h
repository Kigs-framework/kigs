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

	DECLARE_ATTRIBUTE_HERITAGE_NO_ASSIGN(maBufferHeritage, TemplateForPlacementNew, UnderlyingType, CoreModifiable::ATTRIBUTE_TYPE::COREBUFFER);


	void InitData()
	{
		if (!mValue)
		{
			mValue = MakeRefCounted<AlignedCoreRawBuffer<alignement, allocatedType>>();
		}
	}

	void SetBufferFromString(const std::string& value)
	{
		InitData();

		// detect if value is a filename
		if (value[0] == '#')
		{
			std::string filename = value.substr(1, value.length() - 1);

			auto ext = filename.substr(filename.find_last_of('.'));
			u64 filelength = 0;
			auto loaded = ModuleFileManager::LoadFile(filename.c_str(), filelength);
			if (loaded)
			{
				if (ext == ".kbin")
				{
					CMSP uncompressManager = KigsCore::GetSingleton("KXMLManager");
					if (uncompressManager)
					{
						uncompressManager->SimpleCall("UncompressData", loaded.get(), mValue.get());
					}
					else
					{
						KIGS_ERROR("trying to uncompress kxml, but KXMLManager was not declared", 1);
					}
				}
				else
					mValue->SetBuffer(std::move(*loaded.get()));
			}

		}
		else
		{
			unsigned int readSize;
			unsigned char* rawbuf = AsciiParserUtils::StringToBuffer(value, readSize);
			mValue->SetBuffer(rawbuf, readSize);
		}
	}


public:
	
	//! extra constructor with a string
	maBufferHeritage(CoreModifiable& owner, bool isInitAttribute, KigsID ID, const std::string& value) : BaseType(owner, isInitAttribute, ID)
	{
		SetBufferFromString(value);
	}

	/// setValue overloads
	virtual bool setValue(const char* value) override
	{
		std::string str(value); 
		return setValue(str);
	}
	virtual bool setValue(const std::string& value) override
	{
		if (this->isReadOnly())
			return false;
		SetBufferFromString(value);
		DO_NOTIFICATION(notificationLevel);
		return true;
	}

	virtual bool setValue(void* value) override
	{
		if (this->isReadOnly())
			return false;

		mValue = ((CoreRawBuffer*)value)->shared_from_this();
		DO_NOTIFICATION(notificationLevel);
		return true;
	}


	/// getValue overloads
	virtual bool getValue(std::string& value) const override
	{

		if (mValue)
		{
			value = AsciiParserUtils::BufferToString((unsigned char*)mValue->buffer(), mValue->length());
			return true;
		}
		value = "";
		return true;
	}
	
	virtual bool getValue(void*& value) const override { value = (CoreRawBuffer*)mValue.get(); return true; }

	auto& operator=(const std::string &value)
	{
		SetBufferFromString(value);
		return *this;
	}
	auto& operator=(CoreRawBuffer* value)
	{
		InitData();
		mValue->SetBuffer(*value);
		return *this;
	}

	char * buffer() const
	{
		return !mValue ? nullptr :  mValue->buffer();
	}
	unsigned int	length() const
	{
		return !mValue ? 0u :mValue->length();
	}
	void	SetBuffer(void* buffer, unsigned int length, bool manageMemory = true)
	{
		InitData();
		mValue->SetBuffer(buffer, length, manageMemory);
	}
	operator const CoreRawBuffer*() const { return mValue.get(); }

	void* getRawValue() final { return (void*)buffer(); }
	size_t MemorySize() const final { return length(); };

};


// ****************************************
// * maBuffer class
// * --------------------------------------
/**
* \class	maBuffer
* \ingroup	CoreModifiableAttibute
* \brief	CoreModifiable managing a CoreRawBuffer
*/
// ****************************************

using maBuffer = maBufferHeritage<16, char, 0>;



#endif //_MABUFFER_H