#pragma once

#include "ModuleBase.h"
#include "CoreSequence.h"
#include "CoreAction.h"
#include "CoreMap.h"
#include "CoreActionFunction.h"

#include "AttributePacking.h"



namespace Kigs
{
	namespace Core
	{
		class	MiniInstanceFactory;
	}
	namespace Action
	{
		using namespace Kigs::Core;
		/*! \defgroup CoreAnimation
		 *
		*/

		// ****************************************
		// * ModuleCoreAnimation class
		// * --------------------------------------
		/**
		 * \file	ModuleCoreAnimation.h
		 * \class	ModuleCoreAnimation
		 * \ingroup CoreAnimation
		 * \ingroup Module
		 * \brief	manage animated values
		 */
		 // ****************************************

		class ModuleCoreAnimation : public ModuleBase
		{
		public:

			DECLARE_CLASS_INFO(ModuleCoreAnimation, ModuleBase, CoreAnimation)

				//! constructor
				ModuleCoreAnimation(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);

			//! init module
			void Init(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params) override;
			//! close module
			void Close() override;

			//! update module
			void Update(const Timer& timer, void* addParam) override;

			SP<CoreSequence> createSequenceFromString(CMSP target, const std::string& json, Timer* reftimer = 0);
			SP<CoreSequence> createSequenceFromJSON(CMSP target, const std::string& file, Timer* reftimer = 0);
			SP<CoreSequence> createSequenceFromCoreMap(CMSP target, CoreItemSP& sequenceDesc, Timer* reftimer = 0);
			CoreItemSP		 createAction(CoreSequence* sequence, CoreItemSP& actiondesc);

			void	addSequence(CoreSequence* sequence);
			void	removeSequence(CoreSequence* sequence);

			void	startSequence(CoreSequence* sequence, Timer* t = 0);
			void	pauseSequence(CoreSequence* sequence, Timer* t = 0);
			void	stopSequence(CoreSequence* sequence, Timer* t = 0);
			void	startSequenceAtFirstUpdate(CoreSequence* sequence);

			void	removeAllSequencesOnTarget(CoreModifiable* target);

			// callback on animated objects
			DECLARE_METHOD(OnDestroyCallBack)
				COREMODIFIABLE_METHODS(OnDestroyCallBack);
			/**
			* \fn			std::map<std::string, CoreItemOperatorCreateMethod>&	GetCoreItemOperatorConstructMap();
			* \brief		return the specific map for CoreItemOperator creation in animation module
			* \return		std::map<std::string, CoreItemOperatorCreateMethod>&
			*/
			static unordered_map<std::string, CoreItemOperatorCreateMethod>& GetCoreItemOperatorConstructMap();

			WRAP_METHODS(addSequence, removeSequence, removeAllSequencesOnTarget, createSequenceFromString, createSequenceFromJSON, startSequence, pauseSequence, stopSequence, startSequenceAtFirstUpdate);

			//! destructor
			virtual ~ModuleCoreAnimation();

		protected:
			//! sequences manager
			void updateSequences(const Timer& timer);

			std::map<unsigned int, std::vector<SP<CoreSequence>> >	mSequences;

			SP<MiniInstanceFactory>	mPrivateMiniFactory;

			// animation specific CoreItemOperator Map
			static unordered_map<std::string, CoreItemOperatorCreateMethod>	mCoreItemOperatorCreateMethodMap;

		};

	}
}
