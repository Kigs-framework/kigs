#pragma once

namespace Kigs
{
	namespace Core
	{

		template<typename objectType, size_t MaxSize>
		class FixedSizeStack
		{
		public:
			FixedSizeStack() : mCurrentSize(0)
			{
			}

			void	push_back(const objectType& topush)
			{
				assert(mCurrentSize < MaxSize);
				mPreallocatedStack[mCurrentSize] = topush;
				++mCurrentSize;
			}

			void	push_back()
			{
				assert(mCurrentSize < MaxSize);
				++mCurrentSize;
			}

			int	size() const
			{
				return mCurrentSize;
			}

			void pop_back()
			{
				assert(mCurrentSize > 0);
				mCurrentSize--;
			}

			objectType& back()
			{
				assert(mCurrentSize > 0);
				return mPreallocatedStack[mCurrentSize - 1];
			}

			objectType& operator[](size_t index)
			{
				assert(index < mCurrentSize);
				return mPreallocatedStack[index];
			}

		protected:

			objectType		mPreallocatedStack[MaxSize];
			size_t			mCurrentSize;
		};

	}
}