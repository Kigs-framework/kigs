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
	CoreRawBuffer(void* buffer, unsigned int length, bool manageMemory=true) : mBuffer(buffer)
		, mBufferLen(length)
		, mBufferCapacity(length)
		, mManagedMemory(manageMemory)
	{
	};

	virtual ~CoreRawBuffer()
	{
		ProtectedDestroy();
	}

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
		return ((char*)mBuffer + mOffset);
	}

	void clear()
	{
		if (!mManagedMemory) return;
		mBufferCapacity = 0;
	}

	void resize(size_t size)
	{
		if (!mManagedMemory) return;

		if (size <= mBufferCapacity)
		{
			mBufferLen = (u32)size;
			return;
		}

		auto buffer = mBuffer;
		auto old_size = mBufferLen;
		mBuffer = nullptr;
		SetBuffer(nullptr, (unsigned int)size);
		if (buffer) memcpy(data(), buffer, old_size);
		delete[](char*)buffer;
	}

	u32 size() const
	{
		return mBufferLen;
	}

	virtual char* CopyBuffer() const
	{
		char* ret = new char[mBufferLen];
		memcpy(ret, data(), mBufferLen);
		return ret;
	}

	u32	length() const
	{
		return mBufferLen;
	}
	u32	capacity() const
	{
		return mBufferCapacity;
	}

protected:

	void*			mBuffer = nullptr;
	uintptr_t		mOffset = 0;
	u32				mBufferLen = 0;
	u32				mBufferCapacity = 0;
	bool			mManagedMemory = true;


	virtual void InternalSetBuffer(void* buffer, unsigned int length, bool manageMemory = true)
	{
		ProtectedDestroy();
		if (!buffer)
			mBuffer = new char[length];
		else
			mBuffer = buffer;
		mOffset = 0;
		mBufferLen = length;
		mBufferCapacity = length;
		mManagedMemory = !buffer || manageMemory;
	}

	virtual void InternalSetBuffer(const CoreRawBuffer& other)
	{
		if (this == &other) return;
		ProtectedDestroy();
		mBuffer = other.CopyBuffer();
		mOffset = 0;
		mBufferCapacity = mBufferLen = other.size();
		mManagedMemory = true;
	}

	virtual void InternalSetBuffer(CoreRawBuffer&& other)
	{
		if (this == &other) return;
		ProtectedDestroy();
		mBuffer = other.mBuffer;
		mBufferLen = other.mBufferLen;
		mBufferCapacity = other.mBufferCapacity;
		mOffset = other.mOffset;
		mManagedMemory = other.mManagedMemory;
		other.mBuffer = nullptr;
		other.mBufferLen = 0;
		other.mManagedMemory = true;
		other.mOffset = 0;
		other.mBufferCapacity = 0;
	}

	// manage buffer destruction when no more refcount
	virtual void ProtectedDestroy() final
	{
		if (mBuffer)
		{
			// only if this manage the buffer
			if(mManagedMemory)
				delete[](char*)mBuffer;
			mBuffer = 0;
		}
		mBufferLen = 0;
		mBufferCapacity = 0;
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
		return (allocatedType*)((u8*)mBuffer + mOffset);
	}

protected:
	void InternalSetBufferAligned(void* buffer, u32 size)
	{
		mBufferCapacity = mBufferLen = size;
		mBuffer = new char[size + alignement];
		uintptr_t address = (uintptr_t)((uintptr_t)mBuffer + (alignement - 1))& (~(alignement - 1));
		mOffset = address - (uintptr_t)mBuffer;
		if(buffer)
			memcpy(data(), buffer, size);
		mManagedMemory = true;
	}

	virtual void InternalSetBuffer(void* buffer, unsigned int length, bool manageMemory = true) override
	{
		ProtectedDestroy();
		mManagedMemory = manageMemory;
		mBufferLen = length;
		mBufferCapacity = length;

		if (!manageMemory) // just keep reference
		{
			if (((uintptr_t)buffer & (alignement - 1)) != 0)
			{
				KIGS_WARNING("setting unaligned data to AlignedCoreRawBuffer", 1);
			}
			mBuffer = buffer;
			mOffset = 0;
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