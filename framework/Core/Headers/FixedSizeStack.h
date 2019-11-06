#ifndef _FIXEDSIZESTACK_H
#define _FIXEDSIZESTACK_H

template<typename objectType, size_t MaxSize>
class FixedSizeStack
{
public:
	FixedSizeStack() : myCurrentSize(0)
	{
	}

	void	push_back(const objectType& topush)
	{
		assert(myCurrentSize < MaxSize);
		myPreallocatedStack[myCurrentSize] = topush;
		++myCurrentSize;
	}

	void	push_back()
	{
		assert(myCurrentSize < MaxSize);
		++myCurrentSize;
	}

	int	size() const
	{
		return myCurrentSize;
	}

	void pop_back()
	{
		assert(myCurrentSize > 0);
		myCurrentSize--;
	}

	objectType&	back()
	{
		assert(myCurrentSize > 0);
		return myPreallocatedStack[myCurrentSize - 1];
	}

	objectType&	operator[](size_t index)
	{
		assert(index < myCurrentSize);
		return myPreallocatedStack[index];
	}

protected:

	objectType		myPreallocatedStack[MaxSize];
	size_t			myCurrentSize;
};

#endif //_FIXEDSIZESTACK_H