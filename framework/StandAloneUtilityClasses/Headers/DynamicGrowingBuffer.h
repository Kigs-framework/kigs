#ifndef _DYNAMICGROWINGBUFFER_H_
#define _DYNAMICGROWINGBUFFER_H_


#include "Core.h"
#include "CoreModifiable.h"
#include "CoreModifiableAttribute.h"


template<typename datatype>
class RawDataArrayAccess
{
protected:
	datatype*		mArray;
	unsigned int	mSizeX, mSizeY;
public:
	RawDataArrayAccess(datatype* darray, unsigned int arraysizeX, unsigned int arraysizeY) : mArray(darray), mSizeX(arraysizeX), mSizeY(arraysizeY) {};

	inline datatype*	getVertex(unsigned int x, unsigned int y)
	{
		return &mArray[y*mSizeX + x];
	}

	inline datatype*	getVertexLinear(unsigned int x)
	{
		return &mArray[x];
	}

	inline datatype&	getVertexRef(unsigned int x, unsigned int y)
	{
		return mArray[y*mSizeX + x];
	}

	inline unsigned int getIndex(unsigned int x, unsigned int y)
	{
		return y*mSizeX + x;
	}

	inline unsigned int Count()
	{
		return mSizeX*mSizeY;
	}
};



template<class bufferType>
class	DynamicGrowingBuffer
{
protected:

	class bufferStruct
	{
	public:
		bufferStruct(unsigned int size) :mItemCount(size)
		{
			mBuffer = new bufferType[size];
		}
		~bufferStruct()
		{
			delete[] mBuffer;
		}

		bufferType& operator[] (const unsigned int index) const
		{
			return mBuffer[index];
		}

		unsigned int size()
		{
			return mItemCount;
		}

		bufferType*	buffer()
		{
			return mBuffer;
		}

	protected:
		bufferType*		mBuffer;
		unsigned int	mItemCount;

	};

public:
	DynamicGrowingBuffer(unsigned int initSize) : mTotalSize(initSize), mGrowSize(initSize), mCurrentSize(-1)
	{
		// cannot be null
		if (initSize == 0)
			initSize = 1;

		mBufferList.clear();
		bufferStruct* first = new bufferStruct(initSize);
		mBufferList.push_back(first);
		recomputeCache(0);
	}

	DynamicGrowingBuffer(unsigned int initSize,unsigned int growingSize) : mTotalSize(initSize), mGrowSize(growingSize), mCurrentSize(-1)
	{
		// cannot be null
		if (initSize == 0)
			initSize = 1;

		mBufferList.clear();
		bufferStruct* first = new bufferStruct(initSize);
		mBufferList.push_back(first);
		recomputeCache(0);
	}

	~DynamicGrowingBuffer()
	{
		clearBuffers();
	};

	void Clear()
	{
		recomputeCache(0);
	}

	bufferType& operator[] (const unsigned int index) 
	{
		if (((int)index) > mCurrentSize)
		{
			mCurrentSize = index;
		}
		if ((index >= mCurrentCachedPosMin) && (index < mCurrentCachedPosMax))
		{
			return (*mCachedStruct)[index - mCurrentCachedPosMin];
		}

		recomputeCache(index);

		return (*mCachedStruct)[index - mCurrentCachedPosMin];

	}

	bufferType* at(const unsigned int index)
	{
		if (((int)index) > mCurrentSize)
		{
			mCurrentSize = index;
		}
		if ((index >= mCurrentCachedPosMin) && (index < mCurrentCachedPosMax))
		{
			return &(*mCachedStruct)[index - mCurrentCachedPosMin];
		}

		recomputeCache(index);

		return &(*mCachedStruct)[index - mCurrentCachedPosMin];
	}

	void	push_back(bufferType& toadd)
	{
		unsigned int index = (unsigned int)(mCurrentSize + 1);
		(*this)[index] = toadd;
	}

	bufferType& push_back()
	{
		unsigned int index = (unsigned int)(mCurrentSize + 1);
		return (*this)[index];
	}

	bufferType& back()
	{
		unsigned int index = (unsigned int)(mCurrentSize);
		return (*this)[index];
	}

	unsigned int size()
	{
		return (unsigned int)mCurrentSize + 1;
	}

	void resetValue(bufferType val)
	{
		if (size() > 0)
		{
			typename std::vector<bufferStruct*>::iterator	itbuf = mBufferList.begin();
			typename std::vector<bufferStruct*>::iterator	itbufend = mBufferList.end();
			for (; itbuf != itbufend; ++itbuf)
			{
				bufferType* writer = *itbuf->buffer();
				for (int i = 0; i < (int)*itbuf->size(); ++i)
					*writer++ = val;
			}
		}
	}

	void resetData(char val)
	{
		if (size() > 0)
		{
			typename std::vector<bufferStruct*>::iterator	itbuf = mBufferList.begin();
			typename std::vector<bufferStruct*>::iterator	itbufend = mBufferList.end();
			for (; itbuf != itbufend; ++itbuf)
			{
				memset((*itbuf)->buffer(), val, (*itbuf)->size() * sizeof(bufferType));
			}
		}
	}
	

	bufferType*	getArray()
	{
		// create an array from current buffers
		if (size() > 0)
		{
			int realSize = size();
			bufferType* result = new bufferType[realSize];
			bufferType* write = result;
			int	copySize = 0;

			typename std::vector<bufferStruct*>::iterator	itbuf = mBufferList.begin();
			typename std::vector<bufferStruct*>::iterator	itbufend = mBufferList.end();
			while (itbuf != itbufend)
			{
				bufferStruct* current = *itbuf;

				if ((copySize + (int)current->size()) < realSize)
				{
					memcpy(write, current->buffer(), current->size()*sizeof(bufferType));
				}
				else
				{
					int remain = realSize - copySize;
					memcpy(write, current->buffer(), remain*sizeof(bufferType));
				}
				copySize += current->size();
				write += current->size();
				++itbuf;
			}

			return result;
		}
		return 0;
	}

protected:

	void growBuffer(unsigned int index)
	{
		if (index < mTotalSize)
		{
			return;
		}
		unsigned int neededSize = index-mTotalSize;
		if (neededSize < mGrowSize)
		{
			neededSize = mGrowSize;
		}
		bufferStruct* toadd = new bufferStruct(neededSize);
		mBufferList.push_back(toadd);
		mTotalSize += neededSize;
	}


	void recomputeCache(unsigned int index)
	{
		if (index >= mTotalSize)
		{
			growBuffer(index);
		}

		typename std::vector<bufferStruct*>::iterator	itbuf = mBufferList.begin();
		typename std::vector<bufferStruct*>::iterator	itbufend = mBufferList.end();

		mCurrentCachedPosMin = 0;
		mCachedStruct = (*itbuf);
		mCurrentCachedPosMax = mCachedStruct->size();

		while (itbuf != itbufend)
		{
			if ((index >= mCurrentCachedPosMin) && (index < mCurrentCachedPosMax))
			{
				break;
			}
			++itbuf;
			mCachedStruct = (*itbuf);
			mCurrentCachedPosMin = mCurrentCachedPosMax;
			mCurrentCachedPosMax += mCachedStruct->size();
		}
	}

	void clearBuffers()
	{
		typename std::vector<bufferStruct*>::iterator	itbuf = mBufferList.begin();
		typename std::vector<bufferStruct*>::iterator	itbufend = mBufferList.end();

		while (itbuf != itbufend)
		{
			delete *itbuf;
			++itbuf;
		}
		mBufferList.clear();

	}

	std::vector<bufferStruct*>	mBufferList;
	bufferStruct*				mCachedStruct;
	unsigned int				mCurrentCachedPosMin;
	unsigned int				mCurrentCachedPosMax;
	unsigned int				mTotalSize;
	unsigned int				mGrowSize;
	int							mCurrentSize;
};


// dynamic buffer but for a unknown structure type (only size is given) 
class	AbstractDynamicGrowingBuffer
{
protected:

	class abstractBufferStruct
	{
	public:
		abstractBufferStruct(unsigned int count, unsigned int structsize) : mItemCount(count), mItemSize(structsize)
		{
			mBuffer = new unsigned char[structsize*count];
		}
		abstractBufferStruct(abstractBufferStruct&& other)
		{
			*this = std::move(other);
		}
		
		// No copy
		abstractBufferStruct(const abstractBufferStruct& other) = delete;
		abstractBufferStruct& operator=(const abstractBufferStruct&	other) = delete;

		abstractBufferStruct& operator=(abstractBufferStruct&& other)
		{
			if (this == &other) return *this;

			delete[] mBuffer;
			mBuffer = other.mBuffer;
			mItemCount = other.mItemCount;
			mItemSize = other.mItemSize;
			other.mBuffer = nullptr;
			other.mItemCount = 0;
			other.mItemSize = 0;
			return *this;
		}

		~abstractBufferStruct()
		{
			delete[] mBuffer;
		}

		void* operator[] (const unsigned int index) const
		{
			return (void*)(mBuffer + (index*mItemSize));
		}

		unsigned int size()
		{
			return mItemCount;
		}

		void*		buffer()
		{
			return (void*)mBuffer;
		}

	protected:
		unsigned char*	mBuffer = nullptr;
		unsigned int	mItemCount;
		unsigned int	mItemSize;

	};

public:
	AbstractDynamicGrowingBuffer(unsigned int initSize, unsigned int structsize) : mTotalSize(initSize), mGrowSize(initSize), mStructSize(structsize), mCurrentSize(-1)
	{
		mBufferList.clear();
		mBufferList.emplace_back(initSize, structsize);
		recomputeCache(0);
	}

	AbstractDynamicGrowingBuffer() : mTotalSize(0), mGrowSize(0), mStructSize(0), mCurrentSize(-1)
	{
		// Don't use it before calling init !
	}


	AbstractDynamicGrowingBuffer(AbstractDynamicGrowingBuffer&& other)
	{
		*this = std::move(other);
	}

	AbstractDynamicGrowingBuffer(const AbstractDynamicGrowingBuffer& other) = delete;

	AbstractDynamicGrowingBuffer& operator=(AbstractDynamicGrowingBuffer&& other)
	{
		if (this == &other) return *this;
		
		mBufferList = std::move(other.mBufferList);
		mCachedStruct = other.mCachedStruct;
		mCurrentCachedPosMin = other.mCurrentCachedPosMin;
		mCurrentCachedPosMax = other.mCurrentCachedPosMax;
		mTotalSize = other.mTotalSize;
		mGrowSize = other.mGrowSize;
		mStructSize = other.mStructSize;
		mCurrentSize = other.mCurrentSize;

		other.mTotalSize = 0;
		other.mGrowSize = 0;
		other.mStructSize = 0;
		other.mCurrentSize = -1;
		other.mCachedStruct = nullptr;
		other.mBufferList = std::vector<abstractBufferStruct>();

		return *this;
	}


	void init(unsigned int initSize, unsigned int structsize)
	{
		mTotalSize = initSize;
		mGrowSize = initSize;
		mStructSize = structsize;
		mCurrentSize = -1;
		mBufferList.clear();
		mBufferList.emplace_back(initSize, structsize);
		recomputeCache(0);
	}


	~AbstractDynamicGrowingBuffer()
	{
		clearBuffers();
	};

	void Clear()
	{
		recomputeCache(0);
		mCurrentSize = -1;
	}

	void* operator[] (const unsigned int index)
	{
		if (((int)index) > mCurrentSize)
		{
			mCurrentSize = index;
		}
		if ((index >= mCurrentCachedPosMin) && (index < mCurrentCachedPosMax))
		{
			return (*mCachedStruct)[index - mCurrentCachedPosMin];
		}

		recomputeCache(index);

		return (*mCachedStruct)[index - mCurrentCachedPosMin];

	}

	void	push_back(void* toadd)
	{
		unsigned int index = (unsigned int)(mCurrentSize + 1);
		memcpy((*this)[index], toadd, mStructSize);
	}

	unsigned int size()
	{
		return (unsigned int)mCurrentSize + 1;
	}

	void*	getArray(unsigned char * addr = NULL)
	{
		// create an array from current buffers
		if (size() > 0)
		{
			int realSize = size();
			unsigned char* result = addr;
			if (addr == NULL)
				result = new unsigned char[realSize*mStructSize];
			unsigned char* write = result;
			int	copySize = 0;

			auto	itbuf = mBufferList.begin();
			auto	itbufend = mBufferList.end();
			while (itbuf != itbufend)
			{
				abstractBufferStruct* current = &*itbuf;

				if ((copySize + (int)current->size()) < realSize)
				{
					memcpy(write, current->buffer(), current->size()*mStructSize);
				}
				else
				{
					int remain = realSize - copySize;
					memcpy(write, current->buffer(), remain*mStructSize);
					break;
				}
				copySize += current->size();
				write += current->size()*mStructSize;
				++itbuf;
			}

			return result;
		}
		return 0;
	}

	void resize(unsigned int size)
	{
		growBuffer(size);
		mCurrentSize = size - 1;
	}

protected:

	void growBuffer(unsigned int index)
	{
		if (index < mTotalSize)
		{
			return;
		}
		unsigned int neededSize =  index - mTotalSize;
		if (neededSize < mGrowSize)
		{
			neededSize = mGrowSize;
		}
		mBufferList.emplace_back(neededSize, mStructSize);
		mTotalSize += neededSize;
	}


	void recomputeCache(unsigned int index)
	{
		if (index >= mTotalSize)
		{
			growBuffer(index);
		}
		mCurrentCachedPosMin = 0;
		mCachedStruct = &mBufferList[0];
		mCurrentCachedPosMax = mCachedStruct->size();
		for (size_t i = 0; i < mBufferList.size(); ++i)
		{
			if ((index >= mCurrentCachedPosMin) && (index < mCurrentCachedPosMax))
			{
				break;
			}
			mCachedStruct = &mBufferList[i+1];
			mCurrentCachedPosMin = mCurrentCachedPosMax;
			mCurrentCachedPosMax += mCachedStruct->size();
		}
	}

	void clearBuffers()
	{
		mBufferList.clear();
	}

	std::vector<abstractBufferStruct>	mBufferList;
	abstractBufferStruct*				mCachedStruct;
	unsigned int						mCurrentCachedPosMin;
	unsigned int						mCurrentCachedPosMax;
	unsigned int						mTotalSize;
	unsigned int						mGrowSize;
	unsigned int						mStructSize;
	int									mCurrentSize;
};

#endif //_DYNAMICGROWINGBUFFER_H_