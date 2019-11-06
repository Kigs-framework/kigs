#ifndef _PREALLOCATED_CLASS_NEW_H_
#define _PREALLOCATED_CLASS_NEW_H_

#include <mutex>

#ifdef JAVASCRIPT

#define DECLARE_PREALLOCATED_NEW(className,instanceCount) 
#define DECLARE_PREALLOCATED_NEW_NOTHREAD(className,instanceCount) 
#define IMPLEMENT_PREALLOCATED_NEW(className,instanceCount)

#else

#define DECLARE_PREALLOCATED_NEW(className,instanceCount) \
protected:\
static PreallocateBlocs<className, 4096>	mPreallocatedBlocks;\
public:\
	void * operator new(size_t size)\
	{\
		std::lock_guard<std::mutex> lk(mPreallocatedBlocks.mMutex);\
		void* result = mPreallocatedBlocks.allocate();\
		if (result)\
			return result;\
		return  malloc(size);\
	}\
	void operator delete(void * p)\
	{\
		std::lock_guard<std::mutex> lk(mPreallocatedBlocks.mMutex);\
		if (!mPreallocatedBlocks.free(p))\
		{\
			free(p);\
		}\
	}

#define DECLARE_PREALLOCATED_NEW_NOTHREAD(className,instanceCount) \
protected:\
static PreallocateBlocs<className, 4096>	mPreallocatedBlocks;\
public:\
	void * operator new(size_t size)\
	{\
		void* result = mPreallocatedBlocks.allocate();\
		if (result)\
			return result;\
		return  malloc(size);\
	}\
	void operator delete(void * p)\
	{\
		if (!mPreallocatedBlocks.free(p))\
		{\
			free(p);\
		}\
	}
	
#define IMPLEMENT_PREALLOCATED_NEW(className,instanceCount) PreallocateBlocs<className,instanceCount>	className::mPreallocatedBlocks;

namespace PreallocatedClassNew
{
	inline constexpr size_t UpperPowerOf2(unsigned short x)
	{
		x -= 1;
		size_t a = x | (x >> 1);
		size_t b = a | (a >> 2);
		size_t c = b | (b >> 4);
		size_t d = c | (c >> 8);
		d = d + 1;
		return d;
	}

	inline constexpr size_t NeededUnsignedInt(unsigned short x)
	{
		x -= 1;
		x /= 32;
		x += 1;
		return x;
	}

	inline unsigned char	GetFirstLeftBit(unsigned int n)
	{
		static	unsigned char addlast16[16] = { 0,0,1,1,2,2,2,2,3,3,3,3,3,3,3,3 };

		unsigned char result = 0;

		if (n & 0xffff0000)
		{
			result = 16;
			n >>= 16;
		}

		if (n & 0xff00)
		{
			result += 8;
			n >>= 8;
		}

		if (n & 0xf0)
		{
			result += 4;
			n >>= 4;
		}

		result += addlast16[n];

		return result;
	}
}


template<typename classType,int preallocCount>
class PreallocateBlocs
{
protected:
	int					mRemainingPrealloc= preallocCount;
	unsigned char		mPreallocatedArray[PreallocatedClassNew::UpperPowerOf2(sizeof(classType))*preallocCount];
	unsigned int		mAllocatedFlag[PreallocatedClassNew::NeededUnsignedInt(preallocCount)];
	
public:
	mutable				std::mutex mMutex;
	PreallocateBlocs() // init
	{
		memset(mAllocatedFlag, 0xFFFFFFFF, PreallocatedClassNew::NeededUnsignedInt(preallocCount) * sizeof(unsigned int));
	}

	void* allocate()
	{
		if (mRemainingPrealloc == 0)
			return nullptr;
		
		int globalIndex = 0;
		for (unsigned int i = 0; i < PreallocatedClassNew::NeededUnsignedInt(preallocCount); i++)
		{
			if (mAllocatedFlag[i] != 0) // some free blocs here
			{
				int first= PreallocatedClassNew::GetFirstLeftBit(mAllocatedFlag[i]);
				globalIndex += first;
				mAllocatedFlag[i] = mAllocatedFlag[i] ^ (1 << first);
				break;
			}
			globalIndex += 32;
		}
		
		mRemainingPrealloc--;
	
		return (void*)(mPreallocatedArray + globalIndex * PreallocatedClassNew::UpperPowerOf2(sizeof(classType)));
	}
	bool  free(void* p)
	{
		ptrdiff_t	bstart = (ptrdiff_t)p;
		ptrdiff_t	preallocStart = (ptrdiff_t)mPreallocatedArray;
		if (bstart < preallocStart)
			return false;
		bstart -= preallocStart;
		if (bstart >= (ptrdiff_t)(PreallocatedClassNew::UpperPowerOf2(sizeof(classType))*preallocCount))
			return false;

		bstart /= PreallocatedClassNew::UpperPowerOf2(sizeof(classType));

		int flagIndex = bstart >> 5;
		int bitIndex = bstart & 31;

		
		mAllocatedFlag[flagIndex] = mAllocatedFlag[flagIndex] ^ (1 << bitIndex);
		mRemainingPrealloc++;
		return true;
	}
	
};

#endif // not javascript

#endif //_PREALLOCATED_CLASS_NEW_H_