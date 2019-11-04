#ifndef _FIXEDSIZESTACK_H
#define _FIXEDSIZESTACK_H



template<typename objectType,unsigned int MaxSize>
class FixedSizeStack
{
public:
	FixedSizeStack():myPos(-1)
	{
	}

	void	push_back(const objectType& topush)
	{
		++myPos;
#ifdef _DEBUG	// check overflow in debug
		if(myPos>=MaxSize)
		{
			return;
		}
#endif

		myPreallocatedStack[myPos]=topush;
	}

	void	push_back()
	{
		++myPos;
	}

	int	size() const
	{
		return myPos+1;
	}

	void	pop_back()
	{
		myPos--;
#ifdef _DEBUG	// check overflow in debug
		if(myPos<0)
		{
			return;
		}
#endif
	}

	objectType&	back()
	{
#ifdef _DEBUG	// check overflow in debug
		if(myPos<0)
		{
			return	*(objectType*)0;
		}
#endif
		return myPreallocatedStack[myPos];
	}

	objectType&	operator[](unsigned int index)
	{
#ifdef _DEBUG	// check overflow in debug
		if(((int)index)>myPos)
		{
			return *(objectType*)0;
		}
#endif
		return myPreallocatedStack[index];
	}

protected:

	objectType		myPreallocatedStack[MaxSize];
	int				myPos;
};

#endif //_FIXEDSIZESTACK_H