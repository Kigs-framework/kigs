#include "PrecompiledHeaders.h"

#include "StepByStepImporter.h"
#include "XMLIncludes.h"
#include "XMLReaderFile.h"
#include "TimerIncludes.h"
#include "Core.h"

using namespace Kigs::Core;
using namespace Kigs::Xml;
//! a do nothing object is just an instance of CoreModifiable with empty methods
IMPLEMENT_CLASS_INFO(StepByStepImporter)

//! constructor
StepByStepImporter::StepByStepImporter(const std::string& name,CLASS_NAME_TREE_ARG) : CoreModifiable(name,PASS_CLASS_NAME_TREE_ARG),
mFileName(*this,true,"FileName"),
mDeltaTimePerStep(*this, false, "DeltaTimePerStep", 0.01f)
{
	mImportedRoot=0;
	mXmlfile=0;
	mXMLRootNode=0;
	mImportTreeRoot=0;
	mLoadedItems.clear();
	mCurrentImportedTreeNode=0;
	mTimer=0;
	mLoadingIsDone = 0;
}     

void	StepByStepImporter::InitModifiable()
{
	CoreModifiable::InitModifiable();
	if(_isInit)
	{
		printf("create %s\n",mFileName.c_str());
		mXmlfile=(XML*)XML::ReadFile(mFileName,0);

		if(!mXmlfile)
		{
			UninitModifiable();
			return;
		}

		mLoadedItems.clear();
		mXMLRootNode=(XMLNode*)mXmlfile->getRoot();
		mImportTreeRoot=new ImportTree(mXMLRootNode,CMSP(nullptr));
		mCurrentImportedTreeNode=mImportTreeRoot;
		mTimer = KigsCore::GetInstanceOf("StepByStepTimer","Timer");
		mTimer->Init();
		mLoadingIsDone = 0;
	}
}

StepByStepImporter::~StepByStepImporter()
{
	if(mImportTreeRoot)
		delete mImportTreeRoot;

	if(mXmlfile)
		delete mXmlfile;
};

int StepByStepImporter::UpdateImporter()
{
	if(_isInit)
	{
		float startTime=(float)mTimer->GetTime();
		if(!_isInit)
		{
			Init();
			if(!_isInit)
			{
				return -1;
			}
		}

		float totalTime=(float)mTimer->GetTime()-startTime;
		while((mCurrentImportedTreeNode)&&(totalTime<(float)mDeltaTimePerStep))
		{
			mCurrentImportedTreeNode=StepImport(mCurrentImportedTreeNode);
			totalTime=(float)mTimer->GetTime()-startTime;
		}

		// not finished
		if(mCurrentImportedTreeNode)
		{
			return 0;
		}

		// this time everything is loaded
		mImportedRoot=mImportTreeRoot->mCurrent;

		mTimer=nullptr;

		UninitModifiable();
		mLoadingIsDone = true;

		return 1;
	}
	else
		return mLoadingIsDone;
	
}


StepByStepImporter::ImportTree::ImportTree(XMLNode* xmlnode,CMSP currentCM)
{
	mFatherNode=xmlnode;
	mCurrentCoreModifiable = currentCM;
	mSonsImport.clear();
	mNeedInit=false;
	mIsDone=false;
	mLinkID=-1;
	mFatherImportTreeNode=0;
	mBrotherImportTreeNode=0;
	mCurrent=0;
}

StepByStepImporter::ImportTree::~ImportTree()
{
	std::vector<ImportTree*>::iterator it = mSonsImport.begin();
	std::vector<ImportTree*>::iterator end = mSonsImport.end();
	while(it!=end)
	{
		delete (*it);
		it++;
	}
	mSonsImport.clear();
}

StepByStepImporter::ImportTree*		StepByStepImporter::StepImport(StepByStepImporter::ImportTree* treatedNode)
{

	XMLNode * currentNode=treatedNode->mFatherNode;
	CMSP currentModifiable=treatedNode->mCurrentCoreModifiable;

	if(treatedNode->mIsDone)
	{
		if(treatedNode->mNeedInit)
		{
			treatedNode->mCurrent->Init();
			treatedNode->mNeedInit=false;
		}
		// return brother if any

		if(treatedNode->mBrotherImportTreeNode)
		{
			return treatedNode->mBrotherImportTreeNode;
		}

		return treatedNode->mFatherImportTreeNode;
	}
	
	if(!treatedNode->mCurrent)
	{
		if (currentNode->XMLNodeBase::compareName("Instance"))
		{ 
			XMLAttributeBase *NameAttribute = currentNode->getAttribute("Name");

			std::string name;
			if (NameAttribute)
				name = NameAttribute->getString();

			XMLAttributeBase*	type=currentNode->getAttribute("Type");

			//if has a type then it's not a reference
			if(type)
			{
				treatedNode->mNeedInit=true;
				// check that parent doesn't already have this node
				
				if(currentModifiable)
				{
					const std::vector<ModifiableItemStruct>& instances=currentModifiable->getItems();
					//std::vector<int>	linklist=currentModifiable->getItemLinkTypes();
					std::vector<ModifiableItemStruct>::const_iterator itson;
					//std::vector<int>::const_iterator	itsonlink=linklist.begin();
					for(itson=instances.begin();itson!=instances.end();++itson)
					{
						CMSP son=(*itson).mItem;

						if(son->getName() == name)
						{

							if(son->isSubType(type->getString()))
							{
								treatedNode->mNeedInit=false;
								treatedNode->mCurrent=son;
								break;
							}
						}

					}
				}
				
				if(treatedNode->mNeedInit)
				{
					treatedNode->mCurrent=KigsCore::GetInstanceOf(name,type->getString());
					mLoadedItems.push_back(treatedNode->mCurrent);
				}
				ImportTree* lastimport=0;
				for(int i=0;i<currentNode->getChildCount();i++)
				{
					XMLNodeBase*	sonXML=currentNode->getChildElement(i);
					if(sonXML->getType() == XML_NODE_ELEMENT)
					{
						if(sonXML->compareName("CoreModifiableAttribute"))
						{
							// init attribute
							// TODO : check for UTF-8
							KIGS_ASSERT(false);
							//InitAttribute(sonXML,treatedNode->mCurrent,false);
						}
						else
						{
							// create new ImportTree node
							ImportTree* newimport=new ImportTree((XMLNode*)sonXML,treatedNode->mCurrent);
							newimport->mFatherImportTreeNode=mCurrentImportedTreeNode;
							mCurrentImportedTreeNode->mSonsImport.push_back(newimport);
							XMLAttributeBase*	linktype=sonXML->getAttribute("LinkIDToParent");

							if(linktype)
							{
								newimport->mLinkID=linktype->getInt();
							}
							if(lastimport)
							{
								lastimport->mBrotherImportTreeNode=newimport;
							}
							lastimport=newimport;
						}
					}
				}

				// test if a son was added
				if(mCurrentImportedTreeNode->mSonsImport.size())
				{
					return mCurrentImportedTreeNode->mSonsImport[0];
				}
			}
			else
			{	
				treatedNode->mCurrent=InitReference(currentNode,mLoadedItems,name);
			}
		}
	}

	if(treatedNode->mCurrent&&currentModifiable)
	{
		// check if son need add
		bool sonNeedAdd=true;
		
		const std::vector<ModifiableItemStruct>& instances=currentModifiable->getItems();
		std::vector<ModifiableItemStruct>::const_iterator itson;
		for(itson=instances.begin();itson!=instances.end();++itson)
		{
			CMSP son=(*itson).mItem;
			if(son==treatedNode->mCurrent)
			{
				sonNeedAdd=false;
				break;
			}
		}
		
		if(sonNeedAdd)
		{
			{
				currentModifiable->addItem(treatedNode->mCurrent);
			}
		}
	}

	treatedNode->mIsDone=true;

	return treatedNode;
}