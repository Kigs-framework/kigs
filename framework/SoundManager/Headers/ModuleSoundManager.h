#ifndef _MODULESOUNDMANAGER_H_
#define _MODULESOUNDMANAGER_H_

#include "ModuleBase.h"

class AudioListener;

/// \defgroup SoundManager Soundmanager base module

/**
* \class ModuleSoundManager
* \brief manage sound playing and sound object in the scene
* \ingroup SoundManager
*/
class ModuleSoundManager : public ModuleBase
{
public:
	DECLARE_CLASS_INFO(ModuleSoundManager,ModuleBase,SoundManager)
	
	/// Kigs constructor
	ModuleSoundManager(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	
	/// module init method
	void Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params) override;
	
	/// module close method
	void Close() override;
	
	/// module update method
	void Update(const Timer& timer, void* addParam) override;
	
	virtual kstl::vector<AudioListener*>&	GetListenerStack();

	bool	CurrentListenerIsActive();

protected:
	/// destructor
	virtual ~ModuleSoundManager();
	
	/// enumeration of distance models
	maEnum<4> m_DistanceModel;
	
	/// clamp flag, true if sound is clamped when distance is under the reference distance
	maBool m_DistanceClamp;

	kstl::vector<AudioListener*>	MyListenerStack;
	
}; 

#endif //_MODULESOUNDMANAGER_H_
