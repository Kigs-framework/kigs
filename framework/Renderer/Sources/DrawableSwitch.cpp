#include "PrecompiledHeaders.h"

#include "DrawableSwitch.h"

IMPLEMENT_CLASS_INFO(DrawableSwitch)

DrawableSwitch::DrawableSwitch(const kstl::string& name,CLASS_NAME_TREE_ARG) : Drawable(name,PASS_CLASS_NAME_TREE_ARG)
{
	myChildrenHaveBbox=false;
	myCurrentDrawable=0;
	myNeedBBoxUpdate=false;
}

// call only myCurrentDrawable DoPreDraw
void	DrawableSwitch::DoPreDraw(TravState* travstate)
{
	if(myCurrentDrawable)
	{
		myCurrentDrawable->CheckPreDraw(travstate);
	}
}

// call only myCurrentDrawable DoDraw
void	DrawableSwitch::DoDraw(TravState* travstate)
{
	if(myCurrentDrawable)
	{
		myCurrentDrawable->CheckDraw(travstate);
	}
}

// call only myCurrentDrawable DoPostDraw
void	DrawableSwitch::DoPostDraw(TravState* travstate)
{
	if(myCurrentDrawable)
	{
		myCurrentDrawable->CheckPostDraw(travstate);
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
			myNeedBBoxUpdate=true;
			
			// if no current drawable, then this one will be the one
			if(!myCurrentDrawable)
			{
				myCurrentDrawable=(Drawable*)item.get();
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
			myNeedBBoxUpdate=true;
			
			// if item is current drawable, then empty current drawable
			if(myCurrentDrawable==(Drawable*)item.get())
			{
				myCurrentDrawable=0;
			}
		}
	}
	return result;
}


void	DrawableSwitch::ComputeLocalBBox(kdouble time)
{
	Point3D boundingp[2];

	bool isInit=false;
	kstl::vector<ModifiableItemStruct>::const_iterator it;

	for (it=getItems().begin();it!=getItems().end();++it)
    {
		if((*it).mItem->isSubType(Drawable::mClassID))
		{
			SP<Drawable>& drawable = (SP<Drawable>&)(*it).mItem;

			//! if object has a valid BBox update return true
			if(drawable->BBoxUpdate(time))
			{
				drawable->GetNodeBoundingBox(boundingp[0],boundingp[1]);

				//! check if the box is ok
				if(boundingp[0].x <= boundingp[1].x)
				{
					if(isInit)
					{
						myBoundingBox.Update((Point3D *)boundingp,2);
					}
					else
					{
						myBoundingBox.Init((Point3D *)boundingp,2);
						isInit=true;
					}
				}
			}
		}
    }

	myChildrenHaveBbox=isInit;
}

void	DrawableSwitch::SetCurrentDrawable(int index)
{
	myCurrentDrawable=0;
	int sonindex=0;
	kstl::vector<ModifiableItemStruct>::const_iterator it;
	for (it=getItems().begin();it!=getItems().end();++it)
    {
		if((*it).mItem->isSubType(Drawable::mClassID))
		{
			if(sonindex==index)
			{
				myCurrentDrawable=(Drawable*)(*it).mItem.get();
				break;
			}
			sonindex++;

		}
	}
}

void	DrawableSwitch::SetCurrentDrawable(kstl::string drawablename)
{
	myCurrentDrawable=0;
	kstl::vector<ModifiableItemStruct>::const_iterator it;
	for (it=getItems().begin();it!=getItems().end();++it)
    {
		if((*it).mItem->getName()==drawablename)
			if((*it).mItem->isSubType(Drawable::mClassID))
		{
			myCurrentDrawable=(Drawable*)(*it).mItem.get();
			break;
		}
	}
}

void	DrawableSwitch::SetCurrentDrawable(Drawable* drawable)
{
	myCurrentDrawable=0;
	kstl::vector<ModifiableItemStruct>::const_iterator it;
	for (it=getItems().begin();it!=getItems().end();++it)
    {
		if(((*it).mItem)==drawable)
		if((*it).mItem->isSubType(Drawable::mClassID))
		{
			myCurrentDrawable=(Drawable*)(*it).mItem.get();
			break;
		}
	}
}
