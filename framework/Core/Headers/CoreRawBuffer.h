#ifndef _CORERAWBUFFER_H
#define _CORERAWBUFFER_H

#include "GenericRefCountedBaseClass.h"
#include "Core.h"

// utility method to manage a circular buffer
// TODO : set it in a specific header file

template<typename element>
class CircularBuffer
{
protected:

	element*	_elementBuffer;
	int			_elementCount;
	int			_currentElement;
	int			_currentSize;

public:

	CircularBuffer(int elemCount) : _elementCount(elemCount)
		, _elementBuffer(0)
		, _currentElement(0)
		, _currentSize(0)
	{
		_elementBuffer = new element[elemCount];
	}

	CircularBuffer() : _elementCount(0)
		, _elementBuffer(0)
		, _currentElement(0)
		, _currentSize(0)
	{

	}

	void Init(int elemCount)
	{
		if (_elementBuffer)
		{
			KIGS_ERROR("circular buffer already init", 1);
			return;
		}

		_elementCount = elemCount;
		_elementBuffer = new element[elemCount];
	}

	~CircularBuffer()
	{
		if (_elementBuffer)
			delete[] _elementBuffer;
	}

	void	push(const element& elem)
	{
		_elementBuffer[_currentElement] = elem;
		++_currentElement;
		if (_currentElement >= _elementCount)
		{
			_currentElement = 0;
		}
		if (_currentSize<_elementCount)
			++_currentSize;
	}

	void	pop() // remove last set value, but can not retreive lost value in circular buffer
	{
		if (_currentSize)
		{
			--_currentSize;
		}
		else
		{
			return;
		}

		--_currentElement;
		if (_currentElement < 0)
		{
			_currentElement = _elementCount - 1;
		}
	}

	virtual CircularBuffer& operator++()
	{
		++_currentElement;
		if (_currentElement >= _elementCount)
		{
			_currentElement = 0;
		}
		if (_currentSize<_elementCount)
			++_currentSize;
		return *this;
	}

	int getSize()
	{
		return _currentSize;
	}

	virtual element& operator[](int i) const
	{
		while ((_currentElement + i) < 0)
		{
			i += _elementCount;
		}
		return _elementBuffer[(_currentElement + i) % _elementCount];
	}
};

class CoreRawBuffer : public GenericRefCountedBaseClass
{
public:
	CoreRawBuffer() = default;
	CoreRawBuffer(void* buffer, unsigned int length, bool manageMemory=true) : myBuffer(buffer)
		, myBufferLen(length)
		, myBufferCapacity(length)
		, myManagedMemory(manageMemory)
	{
	};

	void SetBuffer(void* buffer, unsigned int length, bool manageMemory = true)
	{
		InternalSetBuffer(buffer, length, manageMemory);
	}

	void SetBuffer(const CoreRawBuffer& other)
	{
		InternalSetBuffer(other);
	}

	void SetBuffer(CoreRawBuffer&& other)
	{
		InternalSetBuffer(std::forward<CoreRawBuffer>(other));
	}

	operator char *() const
	{
		return data();
    };

	char * buffer() const
	{
		return data();
	}

	char * data() const
	{
		return ((char*)myBuffer + myOffset);
	}

	void clear()
	{
		if (!myManagedMemory) return;
		myBufferCapacity = 0;
	}

	void resize(size_t size)
	{
		if (!myManagedMemory) return;

		if (size <= myBufferCapacity)
		{
			myBufferLen = (u32)size;
			return;
		}

		auto buffer = myBuffer;
		auto old_size = myBufferLen;
		myBuffer = nullptr;
		SetBuffer(nullptr, (unsigned int)size);
		if (buffer) memcpy(data(), buffer, old_size);
		delete[](char*)buffer;
	}

	u32 size() const
	{
		return myBufferLen;
	}

	virtual char* CopyBuffer() const
	{
		char* ret = new char[myBufferLen];
		memcpy(ret, data(), myBufferLen);
		return ret;
	}

	u32	length() const
	{
		return myBufferLen;
	}
	u32	capacity() const
	{
		return myBufferCapacity;
	}

protected:

	void*			myBuffer = nullptr;
	uintptr_t		myOffset = 0;
	u32				myBufferLen = 0;
	u32				myBufferCapacity = 0;
	bool			myManagedMemory = true;


	virtual void InternalSetBuffer(void* buffer, unsigned int length, bool manageMemory = true)
	{
		ProtectedDestroy();
		if (!buffer)
			myBuffer = new char[length];
		else
			myBuffer = buffer;
		myOffset = 0;
		myBufferLen = length;
		myBufferCapacity = length;
		myManagedMemory = !buffer || manageMemory;
	}

	virtual void InternalSetBuffer(const CoreRawBuffer& other)
	{
		if (this == &other) return;
		ProtectedDestroy();
		myBuffer = other.CopyBuffer();
		myOffset = 0;
		myBufferCapacity = myBufferLen = other.size();
		myManagedMemory = true;
	}

	virtual void InternalSetBuffer(CoreRawBuffer&& other)
	{
		if (this == &other) return;
		ProtectedDestroy();
		myBuffer = other.myBuffer;
		myBufferLen = other.myBufferLen;
		myBufferCapacity = other.myBufferCapacity;
		myOffset = other.myOffset;
		myManagedMemory = other.myManagedMemory;
		other.myBuffer = nullptr;
		other.myBufferLen = 0;
		other.myManagedMemory = true;
		other.myOffset = 0;
		other.myBufferCapacity = 0;
	}

	// manage buffer destruction when no more refcount
	virtual void ProtectedDestroy()
	{
		if (myBuffer)
		{
			// only if this manage the buffer
			if(myManagedMemory)
				delete[](char*)myBuffer;
			myBuffer = 0;
		}
		myBufferLen = 0;
		myBufferCapacity = 0;
	}
};

// aligned on power of two address buffer
// alignement must be a power of two
template<uintptr_t alignement,typename allocatedType>
class AlignedCoreRawBuffer : public CoreRawBuffer
{
public:

	AlignedCoreRawBuffer() = default;

	AlignedCoreRawBuffer(unsigned int elemCount, bool construct_items = false) : CoreRawBuffer(nullptr, 0)
	{
		InternalSetBufferAligned(nullptr, elemCount * sizeof(allocatedType));
		if (construct_items)
		{
			new (data())allocatedType[elemCount];
		}
	}

	AlignedCoreRawBuffer(void* buffer, unsigned int length, bool manageMemory = true) : CoreRawBuffer(nullptr, 0, manageMemory)
	{
		SetBuffer(buffer, length, manageMemory);
	}

	virtual char * CopyBuffer() const override
	{
		KIGS_WARNING("making an potentially unaligned copy of aligned buffer", 1);
		return CoreRawBuffer::CopyBuffer();
	}

	operator allocatedType*() const
	{
		return (allocatedType*)((u8*)myBuffer + myOffset);
	}

protected:
	void InternalSetBufferAligned(void* buffer, u32 size)
	{
		myBufferCapacity = myBufferLen = size;
		myBuffer = new char[size + alignement];
		uintptr_t address = (uintptr_t)((uintptr_t)myBuffer + (alignement - 1))& (~(alignement - 1));
		myOffset = address - (uintptr_t)myBuffer;
		if(buffer)
			memcpy(data(), buffer, size);
		myManagedMemory = true;
	}

	virtual void InternalSetBuffer(void* buffer, unsigned int length, bool manageMemory = true) override
	{
		ProtectedDestroy();
		myManagedMemory = manageMemory;
		myBufferLen = length;
		myBufferCapacity = length;

		if (!manageMemory) // just keep reference
		{
			if (((uintptr_t)buffer & (alignement - 1)) != 0)
			{
				KIGS_WARNING("setting unaligned data to AlignedCoreRawBuffer", 1);
			}
			myBuffer = buffer;
			myOffset = 0;
		}
		else
		{
			InternalSetBufferAligned(buffer, length);
			delete[](char*)buffer;
		}
	}

	virtual void InternalSetBuffer(const CoreRawBuffer& other) override
	{
		if (this == &other) return;
		ProtectedDestroy();
		InternalSetBufferAligned(other.data(), other.size());
	}

	virtual void InternalSetBuffer(CoreRawBuffer&& other) override
	{
		if (this == &other) return;
		ProtectedDestroy();
		if (((uintptr_t)other.data() & (alignement - 1)) != 0)
		{
			InternalSetBuffer(other); // Not aligned can't move, do a copy instead
			other.clear();
		}
		else
		{
			// other is properly aligned, do a move
			CoreRawBuffer::InternalSetBuffer(std::forward<CoreRawBuffer>(other));
		}
	}
};

#endif // _CORERAWBUFFER_H