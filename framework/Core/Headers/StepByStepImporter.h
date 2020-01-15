#ifndef _STEPBYSTEPIMPORTER_H_
#define _STEPBYSTEPIMPORTER_H_

#include "CoreModifiable.h"
#include "CoreModifiableAttribute.h"
#include "XML.h"

class Timer;

// ****************************************
// * StepByStepImporter class
// * --------------------------------------
/**
 * \file	StepByStepImporter.h
 * \class	StepByStepImporter
 * \ingroup KigsCore
 * \brief	import an xml and create the corresponding CoreModifiable tree step by step
 * \author	ukn
 * \version ukn
 * \date	ukn
 *
 */
// ****************************************
class StepByStepImporter : public CoreModifiable
{
public:

	// instanciable class derived from CoreModifiable
	DECLARE_CLASS_INFO(StepByStepImporter,CoreModifiable,KigsCore)

	/**
	 * \brief	constructor
	 * \fn 		StepByStepImporter(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	 * \param	name : instance name
	 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	 */
	StepByStepImporter(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	/**
	 * \fn 		CoreModifiable* Update()
	 * \brief	import step by step. return 0 while not finished, -1 if an error occured or 1 when finished
	 * 
	 * 
	 */

	int UpdateImporter();
	//using CoreModifiable::Update;

	CMSP	GetImported()
	{
		return myImportedRoot;
	}

protected:
	/**
	 * \fn 		virtual ~StepByStepImporter(){};
	 * \brief	destructor
	 * 
	 * Do nothing
	 */
	virtual ~StepByStepImporter();


	/**
	 * \brief	initialize modifiable
	 * \fn 		void InitModifiable() override
	 */ 
	void	InitModifiable() override;


	maString							myFileName;
	maFloat								myDeltaTimePerStep;

	XML *								myXmlfile;
	kstl::vector<CMSP>					myLoadedItems;
	XMLNode*							myXMLRootNode;

	CMSP								myImportedRoot;

	class	ImportTree
	{
	public:
		ImportTree(XMLNode*,CMSP);
		virtual ~ImportTree();

		XMLNode*						myFatherNode;
		CMSP							myCurrentCoreModifiable;
		CMSP							myCurrent;
		int								myLinkID;
		kstl::vector	<ImportTree*>	mySonsImport;
		bool							myNeedInit;
		ImportTree*						myFatherImportTreeNode;	
		ImportTree*						myBrotherImportTreeNode;	
		bool							myIsDone;
	};

	ImportTree*							myImportTreeRoot;
	ImportTree*							myCurrentImportedTreeNode;

	ImportTree*		StepImport(ImportTree*);

	SP<Timer>								myTimer;
	bool								myLoadingIsDone;
};

#endif //_STEPBYSTEPIMPORTER_H_
