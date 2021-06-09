#ifndef _GENERICANIMATIONMODULE_H_
#define _GENERICANIMATIONMODULE_H_

#include "ModuleBase.h"
#include "AMDefines.h"

/*! \defgroup Animation Animation module
 *  animation module
*/

// ****************************************
// * GenericAnimationModule class
// * --------------------------------------
/*!  \class GenericAnimationModule
     this class is the module manager class.
  \ingroup Animation
*/
// ****************************************

#include "kstlmap.h"
#include "kstlstring.h"
#include "AnimationResourceInfo.h"

#include "AttributePacking.h"

class AObject;
class AAnimatedValue;
class GenericAnimationModule;

extern GenericAnimationModule* gGenericAnimationModule;

class GenericAnimationModule : public ModuleBase
{
public:
	DECLARE_CLASS_INFO(GenericAnimationModule, ModuleBase, Animation);
    DECLARE_CONSTRUCTOR(GenericAnimationModule);
	
	//! module init
	void Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params) override;
	
	//! module close
	void Close() override;
	
	//! module update
    void Update(const Timer& timer,void* addParam) override;
	
	

	WRAP_METHODS(LoadAnimation);


	AnimationResourceInfo*	LoadAnimation(const kstl::string& fileName);
	
	
	void	UnLoad(const kstl::string& fileName);
	void	UnLoad(AnimationResourceInfo* info);
	
	void	addShader(SP<CoreModifiable> parent, SP<CoreModifiable> shader)
	{
		mPostAddShaderList.push_back(std::pair<SP<CoreModifiable>, SP<CoreModifiable>>{parent, shader});
	}
	virtual ~GenericAnimationModule();

	protected:
	
	
	kstl::map<kstl::string, SP<CoreRawBuffer>>	mResourceInfoMap;
	
	void ManagePostAdd()
	{
		kstl::vector<std::pair<SP<CoreModifiable>, SP<CoreModifiable>> >::iterator	itc = mPostAddShaderList.begin();
		kstl::vector<std::pair<SP<CoreModifiable>, SP<CoreModifiable>> >::iterator	ite = mPostAddShaderList.end();
		
		while (itc != ite)
		{
			(*itc).first->addItem((*itc).second,CoreModifiable::First);
			itc++;
		}
		mPostAddShaderList.clear();
	}
	kstl::vector<std::pair<SP<CoreModifiable>, SP<CoreModifiable>> >	mPostAddShaderList;
	

}; 

#endif //_GENERICANIMATIONMODULE_H_
