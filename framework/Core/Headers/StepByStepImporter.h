#pragma once

#include "CoreModifiable.h"
#include "CoreModifiableAttribute.h"
#include "XML.h"

namespace Kigs
{
	namespace Time
	{
		class Timer;
	}
	namespace Core
	{


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
			DECLARE_CLASS_INFO(StepByStepImporter, CoreModifiable, KigsCore)

				/**
				 * \brief	constructor
				 * \fn 		StepByStepImporter(const std::string& name,DECLARE_CLASS_NAME_TREE_ARG);
				 * \param	name : instance name
				 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
				 */
				StepByStepImporter(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);

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

			Kigs::Xml::XML*						mXmlfile;
			std::vector<CMSP>					mLoadedItems;
			Kigs::Xml::XMLNode*					mXMLRootNode;

			CMSP								mImportedRoot;

			class	ImportTree
			{
			public:
				ImportTree(Kigs::Xml::XMLNode*, CMSP);
				virtual ~ImportTree();

				Kigs::Xml::XMLNode* mFatherNode;
				CMSP							mCurrentCoreModifiable;
				CMSP							mCurrent;
				int								mLinkID;
				std::vector	<ImportTree*>	mSonsImport;
				bool							mNeedInit;
				ImportTree* mFatherImportTreeNode;
				ImportTree* mBrotherImportTreeNode;
				bool							mIsDone;
			};

			ImportTree* mImportTreeRoot;
			ImportTree* mCurrentImportedTreeNode;

			ImportTree* StepImport(ImportTree*);

			SP<Kigs::Time::Timer>				mTimer;
			bool								mLoadingIsDone;
		};

	}
}
