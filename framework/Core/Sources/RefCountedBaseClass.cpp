#include "PrecompiledHeaders.h"
#include "RefCountedBaseClass.h"

#include "Core.h"


#include "InstanceFactory.h"

//#include <stdio.h>


#ifdef KIGS_TOOLS
//#include <typeinfo>
#define TRACEREF_RETAIN  kigsprintf("+++ REF ON %p (%s) (%03d>%03d) \n",this, getExactTypeID()._id_name.c_str(), myRefCounter-1, myRefCounter);
#define TRACEREF_RELEASE kigsprintf("--- REF ON %p (%s) (%03d>%03d) \n",this, getExactTypeID()._id_name.c_str(),myRefCounter, myRefCounter-1);
#define TRACEREF_DELETE  kigsprintf("### REF ON %p (%s)\n",this, getExactTypeID()._id_name.c_str());
#else
#define TRACEREF_RETAIN
#define TRACEREF_RELEASE
#define TRACEREF_DELETE
#endif

TRACK_ITEM_STATICIMPL;

#ifdef USE_REFCOUNTED_LINKEDLIST
	#include <algorithm>
	#include "CoreItem.h"
	#include "CoreMap.h"
	#include "CoreVector.h"

	RefCountedBaseClass*		RefCountedBaseClass::m_root=0;
	unsigned int				RefCountedBaseClass::m_currentAllocPage=0;
	RefCountedBaseClass*		RefCountedBaseClass::m_ParentArray[LINKEDLIST_PARENT_ARRAY_SIZE];
#endif //USE_REFCOUNTED_LINKEDLIST

//#include <iostream>
//#include <strstream>

//! class used to register an new class to instance factory
void RegisterClassToInstanceFactory(KigsCore* core, kstl::string moduleName, KigsID classID, createMethod method)
{
	core->GetInstanceFactory()->RegisterClass(method, classID, moduleName);
}

IMPLEMENT_CLASS_INFO(RefCountedBaseClass)

	//! constructor
	RefCountedBaseClass::RefCountedBaseClass() : GenericRefCountedBaseClass()
{
#ifdef USE_REFCOUNTED_LINKEDLIST
	insertThisInDLinkedList();
#endif // USE_REFCOUNTED_LINKEDLIST


	TRACEREF_IMPL;
	TRACK_ITEM_ADDOBJ;
}



void RefCountedBaseClass::GetClassNameTree(CoreClassNameTree& classNameTree) 
{ 
	classNameTree.addClassName(RefCountedBaseClass::myClassID, RefCountedBaseClass::myRuntimeType); 
}


#ifdef USE_REFCOUNTED_LINKEDLIST

void	RefCountedBaseClass::markCoreModifiableItems(RefCountedBaseClass* parent,unsigned int index)
{

	CoreModifiable* parentModifiable=(CoreModifiable*)parent;

	const kstl::vector<ModifiableItemStruct>&	items=parentModifiable->getItems();

	unsigned int	count=items.size();
	unsigned int	i;
	unsigned int	flaggedIndex=index<<LINKEDLIST_PARENT_ARRAY_INDEX_DECAL;
	for(i=0;i<count;i++)
	{
		RefCountedBaseClass*	current=items[i].myItem;
		if(!current->getObjectParentObject())
			current->m_allocPage|=flaggedIndex;
	}

}
void	RefCountedBaseClass::markCoreItems(RefCountedBaseClass* parent,unsigned int index)
{
	unsigned int	count;
	if(((CoreItem*)parent)->GetType() & CoreItem::COREVECTOR)
	{
		CoreVector* currentVector=(CoreVector*)parent;

		count=currentVector->size();
		kstl::vector<RefCountedBaseClass*>::const_iterator it1=currentVector->begin();
		kstl::vector<RefCountedBaseClass*>::const_iterator it2=currentVector->end();

		unsigned int	flaggedIndex=index<<LINKEDLIST_PARENT_ARRAY_INDEX_DECAL;
		while(it1!=it2)
		{
			RefCountedBaseClass*	current=*it1;
			if(!current->getObjectParentObject())
				current->m_allocPage|=flaggedIndex;
			++it1;
		}
	}
	else if(((CoreItem*)parent)->GetType() & CoreItem::COREMAP)
	{
		CoreMap<kstl::string>* currentMap=(CoreMap<kstl::string>*)parent;

		count=currentMap->size();
		
		unsigned int	flaggedIndex=index<<LINKEDLIST_PARENT_ARRAY_INDEX_DECAL;

		kstl::map<kstl::string, RefCountedBaseClass*>::const_iterator it1=currentMap->begin();
		kstl::map<kstl::string, RefCountedBaseClass*>::const_iterator it2=currentMap->end();

		while(it1!=it2)
		{
			RefCountedBaseClass*	current=(*it1).second;
			if(!current->getObjectParentObject())
				current->m_allocPage|=flaggedIndex;
			++it1;
		}
	}
}

void	RefCountedBaseClass::prepareParentArrayList()
{
	// empty array
	memset(m_ParentArray,0,LINKEDLIST_PARENT_ARRAY_SIZE*sizeof(RefCountedBaseClass*));

	// clean parent flag
	RefCountedBaseClass*	current=m_root;
	while(current)
	{
		current->m_allocPage&=LINKEDLIST_PAGE_MASK;
		current=current->m_next;
	}
		
	// search CCArray or CCDict
	current=m_root;
	int index=0;
	while(current)
	{	
		// if index is > LINKEDLIST_PARENT_ARRAY_SIZE-1 can't log parent anymore, sons are marker with "out of buffer parent"
		if(current->isSubType("CoreModifiable"))
		{
			// test if has sons
			CoreModifiable* parentModifiable=(CoreModifiable*)current;
			if(parentModifiable->getItems().size())
			{
				if(index<(LINKEDLIST_PARENT_ARRAY_SIZE-1))
					m_ParentArray[index++]=current;

				markCoreModifiableItems(current,index);
			}
					
		}
		else if(current->isSubType("CoreItem"))
		{
			int count=0;

			if(((CoreItem*)current)->GetType() & CoreItem::COREVECTOR)
			{
				count=((CoreVector*)current)->size();
			}
			else if(((CoreItem*)current)->GetType() & CoreItem::COREMAP)
			{
				count=((CoreMap<kstl::string>*)current)->size();
			}

			if(count)
			{
				if(index<(LINKEDLIST_PARENT_ARRAY_SIZE-1))
					m_ParentArray[index++]=current;

				markCoreItems(current,index);
			}
		}

		current=current->m_next;
	}


}

void	RefCountedBaseClass::logObjectInfos(RefCountedBaseClass* obj)
{
	RefCountedBaseClass* checkParent=obj;
	std::string	printParent="";
	char tmpBuffer[128];
	int countParent=0;
	while(checkParent->getObjectParentObject())
	{
		if(checkParent->getObjectParentObject() == (LINKEDLIST_PARENT_ARRAY_SIZE-1))
		{
			// out of buffer parent
			printParent="[out of buffer parent] - "+printParent;
			break;
		}
		if(countParent>2)
		{
			// out of buffer parent
			printParent="[...] - "+printParent;
			break;
		}
		checkParent=m_ParentArray[checkParent->getObjectParentObject()-1];
#ifdef KEEP_NAME_AS_STRING
		sprintf(tmpBuffer,"[%s(%s) at %p with page %d] - ",checkParent->getExactType().c_str(),checkParent->GetRuntimeType(),checkParent,checkParent->getObjectAllocPage());
#else
		sprintf(tmpBuffer,"[type %d(%d) at %p with page %d] - ",checkParent->getExactType(),checkParent->GetRuntimeType(),checkParent,checkParent->getObjectAllocPage());
#endif // KEEP_NAME_AS_STRING

		printParent=tmpBuffer+printParent;
		countParent++;
	}
	printf("%s",printParent.c_str());
#ifdef KEEP_NAME_AS_STRING
	printf(" %s(%s) at %p with page %d\n",obj->getExactType().c_str(), obj->GetRuntimeType(), obj,obj->getObjectAllocPage());
#else
	printf("type %d(%d) at %p with page %d\n",obj->getExactType(), obj->GetRuntimeType(), obj,obj->getObjectAllocPage());
#endif //KEEP_NAME_AS_STRING

}

void	RefCountedBaseClass::searchObjectWithPage(unsigned int page)
{
	prepareParentArrayList();
		
	RefCountedBaseClass*	current=m_root;
	while(current)
	{
		if(current->getObjectAllocPage() == page)
		{
			logObjectInfos(current);
		}
		current=current->m_next;
	}
}
void	RefCountedBaseClass::searchObjectWithinPage(unsigned int pagemin,unsigned int pagemax)
{
	prepareParentArrayList();
	RefCountedBaseClass*	current=m_root;
	while(current)
	{
		if((current->getObjectAllocPage() >= pagemin)&&(current->getObjectAllocPage() <= pagemin))
		{
			logObjectInfos(current);
		}
		current=current->m_next;
	}
}

void	RefCountedBaseClass::searchObjectWithPageList(const std::vector<int>& pagelist)
{
	if(pagelist.size() == 0)
	{
		return;
	}
		
	// prepare
	int minPage,maxPage;
	std::vector<int>::const_iterator it1=pagelist.begin();
	std::vector<int>::const_iterator it2=pagelist.end();

	minPage=maxPage=*it1;

	++it1;
	while(it1!=it2)
	{
		if((*it1)<minPage)
		{
			minPage=(*it1);
		}
		if((*it1)>maxPage)
		{
			maxPage=(*it1);
		}

		++it1;
	}

	prepareParentArrayList();

	// optimized version
	if((maxPage-minPage)<128)
	{
		unsigned char pageCache[128];
		memset(pageCache,0,128);
		it1=pagelist.begin();
		while(it1!=it2)
		{
			pageCache[(*it1)-minPage]=1;
			++it1;
		}
		RefCountedBaseClass*	current=m_root;
		while(current)
		{
			unsigned int page=current->getObjectAllocPage();
			if((page>=minPage) && (page<=maxPage))
			{
				if(pageCache[page-minPage])
				{
					logObjectInfos(current);
				}
			}
			current=current->m_next;
		}


	}
	else // slow one
	{
		RefCountedBaseClass*	current=m_root;
		it1=pagelist.begin();
		it2=pagelist.end();
		while(current)
		{
			std::vector<int>::const_iterator found=std::find(it1,it2,current->getObjectAllocPage());

			if(found != pagelist.end())
			{
				logObjectInfos(current);
			}
			current=current->m_next;
		}
	}
}

#endif //USE_REFCOUNTED_LINKEDLIST

#if defined (_DEBUG) && ( defined(WIN32) || defined(WUP))
//! increment refcounter
void    RefCountedBaseClass::GetRef()
{
	GenericRefCountedBaseClass::GetRef();
	if(myTraceRef)
		TRACEREF_RETAIN
}
#endif

bool     RefCountedBaseClass::checkDestroy()
{
#if defined (_DEBUG) && defined(WIN32)
	if(myTraceRef)
		TRACEREF_DELETE
#endif
	ProtectedDestroy();

	return false;
}
