#pragma once

#include "Drawable.h"
#include "TecLibs/Tec3D.h"
#include "RendererMatrix.h"
#include "TravPath.h"

class SceneNode;

// ****************************************
// * DrawableSorterItem class
// * --------------------------------------
/**
 * \file	DrawableSorter.h
 * \class	DrawableSorterItem
 * \ingroup SceneGraph
 * \brief	Utility structure to sort drawable.
 */
 // ****************************************
class DrawableSorterItem
{
public:
	TravPath::Way			mWay;
	kfloat					mFloatVal;

	inline friend bool operator<(const DrawableSorterItem &Item1,const DrawableSorterItem &Item2);
};

//! overload < operator for comparison
bool operator<(const DrawableSorterItem &Item1,const DrawableSorterItem &Item2)
{
	if(Item1.mFloatVal==Item2.mFloatVal)
	{
		if(Item1.mWay.mDrawable == Item2.mWay.mDrawable)
		{
			// check path
			if(Item1.mWay.mWay.size() == Item2.mWay.mWay.size())
			{
				int pathIndex;
				for(pathIndex=0;pathIndex<(int)Item1.mWay.mWay.size();pathIndex++)
				{
					if(Item1.mWay.mWay[(unsigned int)pathIndex] != Item2.mWay.mWay[(unsigned int)pathIndex])
					{
						return (Item1.mWay.mWay[(unsigned int)pathIndex]>Item2.mWay.mWay[(unsigned int)pathIndex]);
					}
				}
			}
			return (Item1.mWay.mWay.size()>Item2.mWay.mWay.size());
		}

		return Item1.mWay.mDrawable > Item2.mWay.mDrawable;
	}

	return Item1.mFloatVal>Item2.mFloatVal;
}

// ****************************************
// * DrawableSorter class
// * --------------------------------------
/**
 * \file	DrawableSorter.h
 * \class	DrawableSorter
 * \ingroup SceneGraph
 * \brief	Abstract base class for Drawable sorting.
 */
 // ****************************************
class DrawableSorter
{
public:
	DrawableSorter() = default;
	virtual ~DrawableSorter() = default;


	void Clear() 
	{
		mDrawableList.clear();
	}

	virtual void AddDrawable(Drawable *pDrawable,TravState* state);
	virtual void CompleteInformationFor(DrawableSorterItem *pItem,TravState* state) = 0;


	void	PushCurrentPath(int i)
	{
		mCurrentPath.push_back(i);
	}

	void	PopCurrentPath()
	{
		mCurrentPath.pop_back();
	}

	void	ConstructPath(TravPath& path);


	kstl::vector<int>& GetCurrentWay(){return mCurrentPath;}

protected:
	//! list of drawable
	kstl::vector<DrawableSorterItem>	mDrawableList;
	//! current path
	kstl::vector<int>				mCurrentPath;
};

