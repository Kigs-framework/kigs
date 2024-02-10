#include "PrecompiledHeaders.h"

#include "DrawableSwitch.h"

using namespace Kigs::Draw;

IMPLEMENT_CLASS_INFO(DrawableSwitch)

DrawableSwitch::DrawableSwitch(const std::string& name,CLASS_NAME_TREE_ARG) : Drawable(name,PASS_CLASS_NAME_TREE_ARG)
{
	mChildrenHaveBbox=false;
	mCurrentDrawable=0;
	mNeedBBoxUpdate=false;
}

// call only myCurrentDrawable DoPreDraw
void	DrawableSwitch::DoPreDraw(TravState* travstate)
{
	if(mCurrentDrawable)
	{
		mCurrentDrawable->CheckPreDraw(travstate);
	}
}

// call only myCurrentDrawable DoDraw
void	DrawableSwitch::DoDraw(TravState* travstate)
{
	if(mCurrentDrawable)
	{
		mCurrentDrawable->CheckDraw(travstate);
	}
}

// call only myCurrentDrawable DoPostDraw
void	DrawableSwitch::DoPostDraw(TravState* travstate)
{
	if(mCurrentDrawable)
	{
		mCurrentDrawable->CheckPostDraw(travstate);
	}
}

bool	DrawableSwitch::addItem(const CMSP& item, ItemPosition pos DECLARE_LINK_NAME)
{
	bool result=Drawable::addItem(item,pos PASS_LINK_NAME(linkName));
	if(result)
	{
		if(item->isSubType(Drawable::mClassID))
		{
			// if a drawable was added, bbox needs update
			mNeedBBoxUpdate=true;
			
			// if no current drawable, then this one will be the one
			if(!mCurrentDrawable)
			{
				mCurrentDrawable=(Drawable*)item.get();
			}
		}
	}

	return result;
}

bool	DrawableSwitch::removeItem(const CMSP& item DECLARE_LINK_NAME)
{
	bool result=Drawable::removeItem(item PASS_LINK_NAME(linkName));
	if(result)
	{
		if(item->isSubType(Drawable::mClassID))
		{
			// if a drawable was removed, bbox needs update
			mNeedBBoxUpdate=true;
			
			// if item is current drawable, then empty current drawable
			if(mCurrentDrawable==(Drawable*)item.get())
			{
				mCurrentDrawable=0;
			}
		}
	}
	return result;
}


void	DrawableSwitch::ComputeLocalBBox(double time)
{
	v3f boundingp[2];

	bool isInit=false;
	std::vector<ModifiableItemStruct>::const_iterator it;

	for (it=getItems().begin();it!=getItems().end();++it)
    {
		if((*it).mItem->isSubType(Drawable::mClassID))
		{
			Drawable* drawable = (*it).mItem->as<Drawable>();

			//! if object has a valid BBox update return true
			if(drawable->BBoxUpdate(time))
			{
				drawable->GetNodeBoundingBox(boundingp[0],boundingp[1]);

				//! check if the box is ok
				if(boundingp[0].x <= boundingp[1].x)
				{
					if(isInit)
					{
						mBoundingBox.Update((v3f *)boundingp,2);
					}
					else
					{
						mBoundingBox.Init((v3f *)boundingp,2);
						isInit=true;
					}
				}
			}
		}
    }

	mChildrenHaveBbox=isInit;
}

void	DrawableSwitch::SetCurrentDrawable(int index)
{
	mCurrentDrawable=0;
	int sonindex=0;
	std::vector<ModifiableItemStruct>::const_iterator it;
	for (it=getItems().begin();it!=getItems().end();++it)
    {
		if((*it).mItem->isSubType(Drawable::mClassID))
		{
			if(sonindex==index)
			{
				mCurrentDrawable=(Drawable*)(*it).mItem.get();
				break;
			}
			sonindex++;

		}
	}
}

void	DrawableSwitch::SetCurrentDrawable(std::string drawablename)
{
	mCurrentDrawable=0;
	std::vector<ModifiableItemStruct>::const_iterator it;
	for (it=getItems().begin();it!=getItems().end();++it)
    {
		if((*it).mItem->getName()==drawablename)
			if((*it).mItem->isSubType(Drawable::mClassID))
		{
			mCurrentDrawable=(Drawable*)(*it).mItem.get();
			break;
		}
	}
}

void	DrawableSwitch::SetCurrentDrawable(Drawable* drawable)
{
	mCurrentDrawable=0;
	std::vector<ModifiableItemStruct>::const_iterator it;
	for (it=getItems().begin();it!=getItems().end();++it)
    {
		if(((*it).mItem)==drawable)
		if((*it).mItem->isSubType(Drawable::mClassID))
		{
			mCurrentDrawable=(Drawable*)(*it).mItem.get();
			break;
		}
	}
}
