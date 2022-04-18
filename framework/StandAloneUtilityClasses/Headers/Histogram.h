#pragma once
#include <vector>
#include "TecLibs/Tec3D.h"

// utility class to manage Histograms
template<typename storageType,bool soft=false>
class Histogram
{
public:
	Histogram(const v2f& inputRange,const unsigned int columnCount);
	~Histogram()
	{

	}

	void				addValue(float v);

	storageType			getColumnValue(const unsigned int columnIndex)
	{
		if (columnIndex < mColumnCount)
		{
			return mHistogram[columnIndex];
		}
		return 0;
	}

	unsigned int		getMaxValueColumnIndex()
	{
		storageType bestFound = mHistogram[0];
		unsigned int bestIndex = 0;
		for (unsigned int ci = 1; ci < mHistogram.size(); ci++)
		{
			if (bestFound < mHistogram[ci])
			{
				bestFound = mHistogram[ci];
				bestIndex = ci;
			}
		}
		return bestIndex;
	}

	void				normalize(float sum = 1.0f)
	{
		if (mValueCount)
		{
			float applyC = sum / (float)mValueCount;
			for (auto& c : mHistogram)
			{
				c = (storageType)(((float)c)* applyC);
			}
		}
	}

	size_t size()
	{
		return mHistogram.size();
	}

	// to be called after normalization
	std::vector<float>	getPercentList(const std::vector<float>& vals)
	{
		std::vector<float> results;
		float sumHist = 0.0f;
		int currentValIndex = 0;
		for (int i = 0; i < mHistogram.size(); i++)
		{
			sumHist += getColumnValue(i);
			while(sumHist >= vals[currentValIndex])
			{
				currentValIndex++;
				float val = (((float)i)+0.5f)/ mInputRangeCoef;
				val += mInputRange.x;
				results.push_back(val);
				if (currentValIndex == vals.size())
				{
					return results;
				}
			}
		}
		return results;
	}
		

protected:

	float		getColumn(float v)
	{
		v -= mInputRange.x;
		v *= mInputRangeCoef;

		if (v < 0.0f)
		{
			v = 0.0f;
		}
		else if (v >= mColumnLimit)
		{
			v = mColumnLimit-0.001f;
		}
		return v;
	}

private:

	std::vector<storageType>	mHistogram;
	v2f							mInputRange;
	unsigned int				mColumnCount;
	unsigned int				mValueCount;
	bool						mIsValid=false;
	// precomputed values
	float						mInputRangeCoef=0.0f;
	float						mColumnLimit = 0.0f;
};

template<typename storageType, bool soft>
Histogram<storageType, soft>::Histogram(const v2f& inputRange, const unsigned int columnCount) : mInputRange(inputRange), mColumnCount(columnCount), mValueCount(0)
{
	// check validity
	if ((mColumnCount > 1) && ((mInputRange.y - mInputRange.x) > 0.0f))
	{
		mIsValid = true;
	}
	else
	{
		return;
	}

	mHistogram.assign(columnCount, 0);
	mColumnLimit = (float)(mColumnCount);
	mInputRangeCoef = mColumnLimit /(mInputRange.y - mInputRange.x);
}

template<typename storageType, bool soft>
void		Histogram<storageType, soft>::addValue(float v)
{
	if (soft)
	{
		// TODO 

	}
	else
	{
		float c = getColumn(v);
		int ic = (int)c;
		mHistogram[ic] += 1.0f;
		mValueCount++;
	}
}
