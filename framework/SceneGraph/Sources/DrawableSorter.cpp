#include "PrecompiledHeaders.h"

#include "DrawableSorter.h"
#include "Core.h"
#include "Node3D.h"
#include "Scene3D.h"

#include <algorithm>

using namespace Kigs::Scene;
using namespace Kigs::Draw;

void DrawableSorter::AddDrawable(Drawable *pDrawable,TravState* state)
{
	DrawableSorterItem newItem;

	newItem.mWay.mDrawable = pDrawable;
	CompleteInformationFor(&newItem,state);
	newItem.mWay.mWay=mCurrentPath;

	mDrawableList.push_back(newItem);
}

void	DrawableSorter::ConstructPath(TravPath& path)
{
	std::sort(mDrawableList.begin(), mDrawableList.end());
	std::set<DrawableSorterItem>::iterator it;
	for(auto& it : mDrawableList)
	{
		path.Push(it.mWay);
	}
	
}

