#ifndef _MODULECOREANIMATION_H_
#define _MODULECOREANIMATION_H_

#include "ModuleBase.h"
#include "CoreSequence.h"
#include "CoreAction.h"
#include "CoreMap.h"
#include "CoreActionFunction.h"

#include "AttributePacking.h"

class	MiniInstanceFactory;

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

	DECLARE_CLASS_INFO(ModuleCoreAnimation,ModuleBase,CoreAnimation)

	//! constructor
    ModuleCoreAnimation(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
         
	//! init module
    void Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params) override;   
	//! close module
    void Close() override;
            
	//! update module
	void Update(const Timer& timer, void* addParam) override;

	SP<CoreSequence> createSequenceFromString(CMSP target, const kstl::string& json, Timer* reftimer = 0);
	SP<CoreSequence> createSequenceFromJSON(CMSP target, const kstl::string& file,Timer* reftimer=0);
	SP<CoreSequence> createSequenceFromCoreMap(CMSP target, CoreItemSP& sequenceDesc,Timer* reftimer=0);
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
	* \fn			kstl::map<kstl::string, CoreItemOperatorCreateMethod>&	GetCoreItemOperatorConstructMap();
	* \brief		return the specific map for CoreItemOperator creation in animation module
	* \return		kstl::map<kstl::string, CoreItemOperatorCreateMethod>& 
	*/
	static kigs::unordered_map<kstl::string, CoreItemOperatorCreateMethod>&	GetCoreItemOperatorConstructMap();

	WRAP_METHODS(addSequence, removeSequence, removeAllSequencesOnTarget, createSequenceFromString, createSequenceFromJSON, startSequence, pauseSequence, stopSequence, startSequenceAtFirstUpdate);

	//! destructor
	virtual ~ModuleCoreAnimation();

protected:
	//! sequences manager
	void updateSequences(const Timer& timer);

	kstl::map<unsigned int,kstl::vector<SP<CoreSequence>> >	mSequences;

	SP<MiniInstanceFactory>	mPrivateMiniFactory;

	// animation specific CoreItemOperator Map
	static kigs::unordered_map<kstl::string, CoreItemOperatorCreateMethod>	mCoreItemOperatorCreateMethodMap;

}; 

#endif //_MODULECOREANIMATION_H_
