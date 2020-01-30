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
	
	void	addShader(CoreModifiable* parent, CoreModifiable* shader)
	{
		postAddShaderList.push_back(std::pair<CoreModifiable*, CoreModifiable*>{parent, shader});
	}
	
	protected:
	
	
	
	kstl::map<kstl::string, CoreRawBuffer*>	myResourceInfoMap;
	
	void ManagePostAdd()
	{
		kstl::vector<std::pair<CoreModifiable*, CoreModifiable*> >::iterator	itc = postAddShaderList.begin();
		kstl::vector<std::pair<CoreModifiable*, CoreModifiable*> >::iterator	ite = postAddShaderList.end();
		
		while (itc != ite)
		{
			CMSP toAdd((*itc).second, GetRefTag{});
			(*itc).first->addItem(toAdd);
			itc++;
		}
		postAddShaderList.clear();
	}
	kstl::vector<std::pair<CoreModifiable*, CoreModifiable*> >	postAddShaderList;
	
    virtual ~GenericAnimationModule();    
	
}; 

#endif //_GENERICANIMATIONMODULE_H_
