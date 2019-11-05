#ifndef _DYNAMICGROWINGBUFFER_H_
#define _DYNAMICGROWINGBUFFER_H_


#include "Core.h"
#include "CoreModifiable.h"
#include "CoreModifiableAttribute.h"


template<typename datatype>
class RawDataArrayAccess
{
protected:
	datatype*		myArray;
	unsigned int	mySizeX, mySizeY;
public:
	RawDataArrayAccess(datatype* darray, unsigned int arraysizeX, unsigned int arraysizeY) : myArray(darray), mySizeX(arraysizeX), mySizeY(arraysizeY) {};

	inline datatype*	getVertex(unsigned int x, unsigned int y)
	{
		return &myArray[y*mySizeX + x];
	}

	inline datatype*	getVertexLinear(unsigned int x)
	{
		return &myArray[x];
	}

	inline datatype&	getVertexRef(unsigned int x, unsigned int y)
	{
		return myArray[y*mySizeX + x];
	}

	inline unsigned int getIndex(unsigned int x, unsigned int y)
	{
		return y*mySizeX + x;
	}

	inline unsigned int Count()
	{
		return mySizeX*mySizeY;
	}
};



template<class bufferType>
class	DynamicGrowingBuffer
{
protected:

	class bufferStruct
	{
	public:
		bufferStruct(unsigned int size) :m_ItemCount(size)
		{
			m_buffer = new bufferType[size];
		}
		~bufferStruct()
		{
			delete[] m_buffer;
		}

		bufferType& operator[] (const unsigned int index) const
		{
			return m_buffer[index];
		}

		unsigned int size()
		{
			return m_ItemCount;
		}

		bufferType*	buffer()
		{
			return m_buffer;
		}

	protected:
		bufferType*		m_buffer;
		unsigned int	m_ItemCount;

	};

public:
	DynamicGrowingBuffer(unsigned int initSize) : m_totalSize(initSize), m_growSize(initSize), m_currentSize(-1)
	{
		// cannot be null
		if (initSize == 0)
			initSize = 1;

		m_bufferList.clear();
		bufferStruct* first = new bufferStruct(initSize);
		m_bufferList.push_back(first);
		recomputeCache(0);
	}

	DynamicGrowingBuffer(unsigned int initSize,unsigned int growingSize) : m_totalSize(initSize), m_growSize(growingSize), m_currentSize(-1)
	{
		// cannot be null
		if (initSize == 0)
			initSize = 1;

		m_bufferList.clear();
		bufferStruct* first = new bufferStruct(initSize);
		m_bufferList.push_back(first);
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
		if (((int)index) > m_currentSize)
		{
			m_currentSize = index;
		}
		if ((index >= m_currentCachedPosMin) && (index < m_currentCachedPosMax))
		{
			return (*m_cachedStruct)[index - m_currentCachedPosMin];
		}

		recomputeCache(index);

		return (*m_cachedStruct)[index - m_currentCachedPosMin];

	}

	bufferType* at(const unsigned int index)
	{
		if (((int)index) > m_currentSize)
		{
			m_currentSize = index;
		}
		if ((index >= m_currentCachedPosMin) && (index < m_currentCachedPosMax))
		{
			return &(*m_cachedStruct)[index - m_currentCachedPosMin];
		}

		recomputeCache(index);

		return &(*m_cachedStruct)[index - m_currentCachedPosMin];
	}

	void	push_back(bufferType& toadd)
	{
		unsigned int index = (unsigned int)(m_currentSize + 1);
		(*this)[index] = toadd;
	}

	unsigned int size()
	{
		return (unsigned int)m_currentSize + 1;
	}

	void resetValue(bufferType val)
	{
		if (size() > 0)
		{
			typename kstl::vector<bufferStruct*>::iterator	itbuf = m_bufferList.begin();
			typename kstl::vector<bufferStruct*>::iterator	itbufend = m_bufferList.end();
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
			typename kstl::vector<bufferStruct*>::iterator	itbuf = m_bufferList.begin();
			typename kstl::vector<bufferStruct*>::iterator	itbufend = m_bufferList.end();
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

			typename kstl::vector<bufferStruct*>::iterator	itbuf = m_bufferList.begin();
			typename kstl::vector<bufferStruct*>::iterator	itbufend = m_bufferList.end();
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
		if (index < m_totalSize)
		{
			return;
		}
		unsigned int neededSize = index-m_totalSize;
		if (neededSize < m_growSize)
		{
			neededSize = m_growSize;
		}
		bufferStruct* toadd = new bufferStruct(neededSize);
		m_bufferList.push_back(toadd);
		m_totalSize += neededSize;
	}


	void recomputeCache(unsigned int index)
	{
		if (index >= m_totalSize)
		{
			growBuffer(index);
		}

		typename kstl::vector<bufferStruct*>::iterator	itbuf = m_bufferList.begin();
		typename kstl::vector<bufferStruct*>::iterator	itbufend = m_bufferList.end();

		m_currentCachedPosMin = 0;
		m_cachedStruct = (*itbuf);
		m_currentCachedPosMax = m_cachedStruct->size();

		while (itbuf != itbufend)
		{
			if ((index >= m_currentCachedPosMin) && (index < m_currentCachedPosMax))
			{
				break;
			}
			++itbuf;
			m_cachedStruct = (*itbuf);
			m_currentCachedPosMin = m_currentCachedPosMax;
			m_currentCachedPosMax += m_cachedStruct->size();
		}
	}

	void clearBuffers()
	{
		typename kstl::vector<bufferStruct*>::iterator	itbuf = m_bufferList.begin();
		typename kstl::vector<bufferStruct*>::iterator	itbufend = m_bufferList.end();

		while (itbuf != itbufend)
		{
			delete *itbuf;
			++itbuf;
		}
		m_bufferList.clear();

	}

	kstl::vector<bufferStruct*>	m_bufferList;
	bufferStruct*				m_cachedStruct;
	unsigned int				m_currentCachedPosMin;
	unsigned int				m_currentCachedPosMax;
	unsigned int				m_totalSize;
	unsigned int				m_growSize;
	int							m_currentSize;
};


// dynamic buffer but for a unknown structure type (only size is given) 
class	AbstractDynamicGrowingBuffer
{
protected:

	class abstractBufferStruct
	{
	public:
		abstractBufferStruct(unsigned int count, unsigned int structsize) : m_ItemCount(count), m_ItemSize(structsize)
		{
			m_buffer = new unsigned char[structsize*count];
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

			delete[] m_buffer;
			m_buffer = other.m_buffer;
			m_ItemCount = other.m_ItemCount;
			m_ItemSize = other.m_ItemSize;
			other.m_buffer = nullptr;
			other.m_ItemCount = 0;
			other.m_ItemSize = 0;
			return *this;
		}

		~abstractBufferStruct()
		{
			delete[] m_buffer;
		}

		void* operator[] (const unsigned int index) const
		{
			return (void*)(m_buffer + (index*m_ItemSize));
		}

		unsigned int size()
		{
			return m_ItemCount;
		}

		void*		buffer()
		{
			return (void*)m_buffer;
		}

	protected:
		unsigned char*	m_buffer = nullptr;
		unsigned int	m_ItemCount;
		unsigned int	m_ItemSize;

	};

public:
	AbstractDynamicGrowingBuffer(unsigned int initSize, unsigned int structsize) : m_totalSize(initSize), m_growSize(initSize), m_structSize(structsize), m_currentSize(-1)
	{
		m_bufferList.clear();
		m_bufferList.emplace_back(initSize, structsize);
		recomputeCache(0);
	}

	AbstractDynamicGrowingBuffer() : m_totalSize(0), m_growSize(0), m_structSize(0), m_currentSize(-1)
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
		
		m_bufferList = std::move(other.m_bufferList);
		m_cachedStruct = other.m_cachedStruct;
		m_currentCachedPosMin = other.m_currentCachedPosMin;
		m_currentCachedPosMax = other.m_currentCachedPosMax;
		m_totalSize = other.m_totalSize;
		m_growSize = other.m_growSize;
		m_structSize = other.m_structSize;
		m_currentSize = other.m_currentSize;

		other.m_totalSize = 0;
		other.m_growSize = 0;
		other.m_structSize = 0;
		other.m_currentSize = -1;
		other.m_cachedStruct = nullptr;
		other.m_bufferList = kstl::vector<abstractBufferStruct>();

		return *this;
	}


	void init(unsigned int initSize, unsigned int structsize)
	{
		m_totalSize = initSize;
		m_growSize = initSize;
		m_structSize = structsize;
		m_currentSize = -1;
		m_bufferList.clear();
		m_bufferList.emplace_back(initSize, structsize);
		recomputeCache(0);
	}


	~AbstractDynamicGrowingBuffer()
	{
		clearBuffers();
	};

	void Clear()
	{
		recomputeCache(0);
		m_currentSize = -1;
	}

	void* operator[] (const unsigned int index)
	{
		if (((int)index) > m_currentSize)
		{
			m_currentSize = index;
		}
		if ((index >= m_currentCachedPosMin) && (index < m_currentCachedPosMax))
		{
			return (*m_cachedStruct)[index - m_currentCachedPosMin];
		}

		recomputeCache(index);

		return (*m_cachedStruct)[index - m_currentCachedPosMin];

	}

	void	push_back(void* toadd)
	{
		unsigned int index = (unsigned int)(m_currentSize + 1);
		memcpy((*this)[index], toadd, m_structSize);
	}

	unsigned int size()
	{
		return (unsigned int)m_currentSize + 1;
	}

	void*	getArray(unsigned char * addr = NULL)
	{
		// create an array from current buffers
		if (size() > 0)
		{
			int realSize = size();
			unsigned char* result = addr;
			if (addr == NULL)
				result = new unsigned char[realSize*m_structSize];
			unsigned char* write = result;
			int	copySize = 0;

			auto	itbuf = m_bufferList.begin();
			auto	itbufend = m_bufferList.end();
			while (itbuf != itbufend)
			{
				abstractBufferStruct* current = &*itbuf;

				if ((copySize + (int)current->size()) < realSize)
				{
					memcpy(write, current->buffer(), current->size()*m_structSize);
				}
				else
				{
					int remain = realSize - copySize;
					memcpy(write, current->buffer(), remain*m_structSize);
					break;
				}
				copySize += current->size();
				write += current->size()*m_structSize;
				++itbuf;
			}

			return result;
		}
		return 0;
	}

	void resize(unsigned int size)
	{
		growBuffer(size);
		m_currentSize = size - 1;
	}

protected:

	void growBuffer(unsigned int index)
	{
		if (index < m_totalSize)
		{
			return;
		}
		unsigned int neededSize =  index - m_totalSize;
		if (neededSize < m_growSize)
		{
			neededSize = m_growSize;
		}
		m_bufferList.emplace_back(neededSize, m_structSize);
		m_totalSize += neededSize;
	}


	void recomputeCache(unsigned int index)
	{
		if (index >= m_totalSize)
		{
			growBuffer(index);
		}
		m_currentCachedPosMin = 0;
		m_cachedStruct = &m_bufferList[0];
		m_currentCachedPosMax = m_cachedStruct->size();
		for (size_t i = 0; i < m_bufferList.size(); ++i)
		{
			if ((index >= m_currentCachedPosMin) && (index < m_currentCachedPosMax))
			{
				break;
			}
			m_cachedStruct = &m_bufferList[i+1];
			m_currentCachedPosMin = m_currentCachedPosMax;
			m_currentCachedPosMax += m_cachedStruct->size();
		}
	}

	void clearBuffers()
	{
		m_bufferList.clear();
	}

	kstl::vector<abstractBufferStruct>	m_bufferList;
	abstractBufferStruct*				m_cachedStruct;
	unsigned int						m_currentCachedPosMin;
	unsigned int						m_currentCachedPosMax;
	unsigned int						m_totalSize;
	unsigned int						m_growSize;
	unsigned int						m_structSize;
	int									m_currentSize;
};

#endif //_DYNAMICGROWINGBUFFER_H_