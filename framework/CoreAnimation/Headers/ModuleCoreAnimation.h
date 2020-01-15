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

	CoreSequence*   createSequenceFromString(CoreModifiable* target, const kstl::string& json, Timer* reftimer = 0);
	CoreSequence*	createSequenceFromJSON(CoreModifiable* target,const kstl::string& file,Timer* reftimer=0);
	CoreSequence*	createSequenceFromCoreMap(CoreModifiable* target,CoreItemSP& sequenceDesc,Timer* reftimer=0);
	CoreItemSP		createAction(CoreSequence* sequence, CoreItemSP& actiondesc);

	void	addSequence(CoreSequence* sequence);
    void	removeSequence(CoreSequence* sequence);

	void	startSequence(CoreSequence* sequence, Timer* t = 0);
	void	pauseSequence(CoreSequence* sequence, Timer* t = 0);
	void	stopSequence(CoreSequence* sequence, Timer* t = 0);
	void	startSequenceAtFirstUpdate(CoreSequence* sequence);

	void	removeAllSequencesOnTarget(CoreModifiable* target);

	// callback on animated objects
	DECLARE_METHOD(OnDestroyCallBack)

	/**
	* \fn			kstl::map<kstl::string, CoreItemOperatorCreateMethod>&	GetCoreItemOperatorConstructMap();
	* \brief		return the specific map for CoreItemOperator creation in animation module
	* \return		kstl::map<kstl::string, CoreItemOperatorCreateMethod>& 
	*/
	static kstl::unordered_map<kstl::string, CoreItemOperatorCreateMethod>&	GetCoreItemOperatorConstructMap();

	WRAP_METHODS(addSequence, removeSequence, removeAllSequencesOnTarget, createSequenceFromString, createSequenceFromJSON, startSequence, pauseSequence, stopSequence, startSequenceAtFirstUpdate);
protected:
	//! destructor
    virtual ~ModuleCoreAnimation();    

	//! sequences manager
	void updateSequences(const Timer& timer);

	kstl::map<unsigned int,kstl::vector<CoreSequence*> >	mySequences;

	SP<MiniInstanceFactory>	myPrivateMiniFactory;

	// animation specific CoreItemOperator Map
	static kstl::unordered_map<kstl::string, CoreItemOperatorCreateMethod>	myCoreItemOperatorCreateMethodMap;

}; 

#endif //_MODULECOREANIMATION_H_
