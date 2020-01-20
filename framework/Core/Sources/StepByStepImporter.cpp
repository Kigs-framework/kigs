#include "PrecompiledHeaders.h"

#include "StepByStepImporter.h"
#include "XMLIncludes.h"
#include "XMLReaderFile.h"
#include "TimerIncludes.h"
#include "Core.h"

//! a do nothing object is just an instance of CoreModifiable with empty methods
IMPLEMENT_CLASS_INFO(StepByStepImporter)

//! constructor
StepByStepImporter::StepByStepImporter(const kstl::string& name,CLASS_NAME_TREE_ARG) : CoreModifiable(name,PASS_CLASS_NAME_TREE_ARG),
myFileName(*this,true,LABEL_AND_ID(FileName)),
myDeltaTimePerStep(*this,false,LABEL_AND_ID(DeltaTimePerStep),KFLOAT_CONST(0.01f))
{
	myImportedRoot=0;
	myXmlfile=0;
	myXMLRootNode=0;
	myImportTreeRoot=0;
	myLoadedItems.clear();
	myCurrentImportedTreeNode=0;
	myTimer=0;
	myLoadingIsDone = 0;
}     

void	StepByStepImporter::InitModifiable()
{
	CoreModifiable::InitModifiable();
	if(_isInit)
	{
		printf("create %s\n",myFileName.c_str());
		myXmlfile=(XML*)XML::ReadFile(myFileName,0);

		if(!myXmlfile)
		{
			UninitModifiable();
			return;
		}

		myLoadedItems.clear();
		myXMLRootNode=(XMLNode*)myXmlfile->getRoot();
		myImportTreeRoot=new ImportTree(myXMLRootNode,CMSP(nullptr));
		myCurrentImportedTreeNode=myImportTreeRoot;
		myTimer=KigsCore::GetInstanceOf("StepByStepTimer","Timer");
		myTimer->Init();
		myLoadingIsDone = 0;
	}
}

StepByStepImporter::~StepByStepImporter()
{
	if(myImportTreeRoot)
		delete myImportTreeRoot;

	if(myXmlfile)
		delete myXmlfile;
};

int StepByStepImporter::UpdateImporter()
{
	if(_isInit)
	{
		kfloat startTime=(kfloat)myTimer->GetTime();
		if(!_isInit)
		{
			Init();
			if(!_isInit)
			{
				return -1;
			}
		}

		kfloat totalTime=(kfloat)myTimer->GetTime()-startTime;
		while((myCurrentImportedTreeNode)&&(totalTime<(kfloat)myDeltaTimePerStep))
		{
			myCurrentImportedTreeNode=StepImport(myCurrentImportedTreeNode);
			totalTime=(kfloat)myTimer->GetTime()-startTime;
		}

		// not finished
		if(myCurrentImportedTreeNode)
		{
			return 0;
		}

		// this time everything is loaded
		myImportedRoot=myImportTreeRoot->myCurrent;

		myTimer=nullptr;

		UninitModifiable();
		myLoadingIsDone = true;

		return 1;
	}
	else
		return myLoadingIsDone;
	
}


StepByStepImporter::ImportTree::ImportTree(XMLNode* xmlnode,CMSP currentCM)
{
	myFatherNode=xmlnode;
	myCurrentCoreModifiable = currentCM;
	mySonsImport.clear();
	myNeedInit=false;
	myIsDone=false;
	myLinkID=-1;
	myFatherImportTreeNode=0;
	myBrotherImportTreeNode=0;
	myCurrent=0;
}

StepByStepImporter::ImportTree::~ImportTree()
{
	kstl::vector<ImportTree*>::iterator it = mySonsImport.begin();
	kstl::vector<ImportTree*>::iterator end = mySonsImport.end();
	while(it!=end)
	{
		delete (*it);
		it++;
	}
	mySonsImport.clear();
}

StepByStepImporter::ImportTree*		StepByStepImporter::StepImport(StepByStepImporter::ImportTree* treatedNode)
{

	XMLNode * currentNode=treatedNode->myFatherNode;
	CMSP currentModifiable=treatedNode->myCurrentCoreModifiable;

	if(treatedNode->myIsDone)
	{
		if(treatedNode->myNeedInit)
		{
			treatedNode->myCurrent->Init();
			treatedNode->myNeedInit=false;
		}
		// return brother if any

		if(treatedNode->myBrotherImportTreeNode)
		{
			return treatedNode->myBrotherImportTreeNode;
		}

		return treatedNode->myFatherImportTreeNode;
	}
	
	if(!treatedNode->myCurrent)
	{
		if (currentNode->getName()=="Instance")
		{ 
			XMLAttribute *NameAttribute = currentNode->getAttribute("Name");

			kstl::string name;
			if (NameAttribute)
				name = NameAttribute->getString();

			XMLAttribute*	type=currentNode->getAttribute("Type");

			//if has a type then it's not a reference
			if(type)
			{

#ifdef USE_LINK_TYPE
				XMLAttribute*	linktypeID=currentNode->getAttribute("LinkIDToParent");
				int	linkIDInt=-1;
				if(linktypeID)
				{
					linkIDInt=linktypeID->getInt();
				}
#endif
				
				treatedNode->myNeedInit=true;
				// check that parent doesn't already have this node
				
				if(currentModifiable)
				{
					const kstl::vector<ModifiableItemStruct>& instances=currentModifiable->getItems();
					//kstl::vector<int>	linklist=currentModifiable->getItemLinkTypes();
					kstl::vector<ModifiableItemStruct>::const_iterator itson;
					//kstl::vector<int>::const_iterator	itsonlink=linklist.begin();
					for(itson=instances.begin();itson!=instances.end();++itson)
					{
						CMSP son=(*itson).myItem;
#ifdef USE_LINK_TYPE
						int linktypeint=(*itson).myLinkType;
#endif
						if(son->getName() == name)
						{
#ifdef USE_LINK_TYPE
							if(linktypeint==linkIDInt)
#endif
								if(son->isSubType(type->getString()))
							{
								treatedNode->myNeedInit=false;
								treatedNode->myCurrent=son;
								break;
							}
						}

					}
				}
				
				if(treatedNode->myNeedInit)
				{
					treatedNode->myCurrent=KigsCore::GetInstanceOf(name,type->getString());
					myLoadedItems.push_back(treatedNode->myCurrent);
				}
				ImportTree* lastimport=0;
				for(int i=0;i<currentNode->getChildCount();i++)
				{
					XMLNode*	sonXML=currentNode->getChildElement(i);
					if(sonXML->getType() == XML_NODE_ELEMENT)
					{
						if(sonXML->getName()=="CoreModifiableAttribute")
						{
							// init attribute
							// TODO : check for UTF-8
							KIGS_ASSERT(false);
							//InitAttribute(sonXML,treatedNode->myCurrent,false);
						}
						else
						{
							// create new ImportTree node
							ImportTree* newimport=new ImportTree(sonXML,treatedNode->myCurrent);
							newimport->myFatherImportTreeNode=myCurrentImportedTreeNode;
							myCurrentImportedTreeNode->mySonsImport.push_back(newimport);
							XMLAttribute*	linktype=sonXML->getAttribute("LinkIDToParent");

							if(linktype)
							{
								newimport->myLinkID=linktype->getInt();
							}
							if(lastimport)
							{
								lastimport->myBrotherImportTreeNode=newimport;
							}
							lastimport=newimport;
						}
					}
				}

				// test if a son was added
				if(myCurrentImportedTreeNode->mySonsImport.size())
				{
					return myCurrentImportedTreeNode->mySonsImport[0];
				}
			}
			else
			{	
				treatedNode->myCurrent=InitReference(currentNode,myLoadedItems,name);
			}
		}
	}

	if(treatedNode->myCurrent&&currentModifiable)
	{
		// check if son need add
		bool sonNeedAdd=true;
		
		const kstl::vector<ModifiableItemStruct>& instances=currentModifiable->getItems();
		kstl::vector<ModifiableItemStruct>::const_iterator itson;
		for(itson=instances.begin();itson!=instances.end();++itson)
		{
			CMSP son=(*itson).myItem;
			if(son==treatedNode->myCurrent)
			{
				sonNeedAdd=false;
				break;
			}
		}
		
		if(sonNeedAdd)
		{
#ifdef USE_LINK_TYPE
			XMLAttribute*	linktype=currentNode->getAttribute("LinkIDToParent");

			if(linktype)
			{
				currentModifiable->addItem(treatedNode->myCurrent,(*currentModifiable->getDeclaredLinkTypes())[(unsigned int)linktype->getInt()]);
			}
			else
#endif
			{
				currentModifiable->addItem(treatedNode->myCurrent);
			}
		}
	}

	treatedNode->myIsDone=true;

	return treatedNode;
}