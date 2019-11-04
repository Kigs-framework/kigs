#include "PrecompiledHeaders.h"

#include "DrawableSorter.h"
#include "Core.h"
#include "Node3D.h"
#include "Scene3D.h"

#include <algorithm>

void DrawableSorter::AddDrawable(Drawable *pDrawable,TravState* state)
{
	DrawableSorterItem newItem;

	newItem.myWay.myDrawable = pDrawable;
	CompleteInformationFor(&newItem,state);
	newItem.myWay.myWay=myCurrentPath;

	m_DrawableList.push_back(newItem);
}

void	DrawableSorter::ConstructPath(TravPath& path)
{
	std::sort(m_DrawableList.begin(), m_DrawableList.end());
	kstl::set<DrawableSorterItem>::iterator it;
	for(auto& it : m_DrawableList)
	{
		path.Push(it.myWay);
	}
	
}

