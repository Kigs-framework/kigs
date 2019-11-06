#pragma once

#include "Drawable.h"
#include "TecLibs/Tec3D.h"
#include "RendererMatrix.h"
#include "TravPath.h"

class SceneNode;


class DrawableSorterItem
{
public:
	TravPath::Way			myWay;
	kfloat					m_FloatVal;

	inline friend bool operator<(const DrawableSorterItem &Item1,const DrawableSorterItem &Item2);
};

//! overload < operator for comparison
bool operator<(const DrawableSorterItem &Item1,const DrawableSorterItem &Item2)
{
	if(Item1.m_FloatVal==Item2.m_FloatVal)
	{
		if(Item1.myWay.myDrawable == Item2.myWay.myDrawable)
		{
			// check path
			if(Item1.myWay.myWay.size() == Item2.myWay.myWay.size())
			{
				int pathIndex;
				for(pathIndex=0;pathIndex<(int)Item1.myWay.myWay.size();pathIndex++)
				{
					if(Item1.myWay.myWay[(unsigned int)pathIndex] != Item2.myWay.myWay[(unsigned int)pathIndex])
					{
						return (Item1.myWay.myWay[(unsigned int)pathIndex]>Item2.myWay.myWay[(unsigned int)pathIndex]);
					}
				}
			}
			return (Item1.myWay.myWay.size()>Item2.myWay.myWay.size());
		}

		return Item1.myWay.myDrawable > Item2.myWay.myDrawable;
	}

	return Item1.m_FloatVal>Item2.m_FloatVal;
}


class DrawableSorter
{
public:
	DrawableSorter() = default;
	virtual ~DrawableSorter() = default;


	void Clear() 
	{
		m_DrawableList.clear();
	}

	virtual void AddDrawable(Drawable *pDrawable,TravState* state);
	virtual void CompleteInformationFor(DrawableSorterItem *pItem,TravState* state) = 0;


	void	PushCurrentPath(int i)
	{
		myCurrentPath.push_back(i);
	}

	void	PopCurrentPath()
	{
		myCurrentPath.pop_back();
	}

	void	ConstructPath(TravPath& path);


	kstl::vector<int>& GetCurrentWay(){return myCurrentPath;}

protected:
	//! list of drawable
	kstl::vector<DrawableSorterItem>	m_DrawableList;
	//! current path
	kstl::vector<int>				myCurrentPath;
};

