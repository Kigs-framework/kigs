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
 * \ingroup Core
 * \brief	Import an xml and create the corresponding CoreModifiable tree step by step
 *
 * ?? Obsolete ??
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
		return mImportedRoot;
	}
	/**
	 * \fn 		virtual ~StepByStepImporter(){};
	 * \brief	destructor
	 *
	 * Do nothing
	 */
	virtual ~StepByStepImporter();

protected:
	/**
	 * \brief	initialize modifiable
	 * \fn 		void InitModifiable() override
	 */ 
	void	InitModifiable() override;


	maString							mFileName;
	maFloat								mDeltaTimePerStep;

	XML *								mXmlfile;
	kstl::vector<CMSP>					mLoadedItems;
	XMLNode*							mXMLRootNode;

	CMSP								mImportedRoot;

	class	ImportTree
	{
	public:
		ImportTree(XMLNode*,CMSP);
		virtual ~ImportTree();

		XMLNode*						mFatherNode;
		CMSP							mCurrentCoreModifiable;
		CMSP							mCurrent;
		int								mLinkID;
		kstl::vector	<ImportTree*>	mSonsImport;
		bool							mNeedInit;
		ImportTree*						mFatherImportTreeNode;	
		ImportTree*						mBrotherImportTreeNode;	
		bool							mIsDone;
	};

	ImportTree*							mImportTreeRoot;
	ImportTree*							mCurrentImportedTreeNode;

	ImportTree*		StepImport(ImportTree*);

	SP<Timer>								mTimer;
	bool								mLoadingIsDone;
};

#endif //_STEPBYSTEPIMPORTER_H_
