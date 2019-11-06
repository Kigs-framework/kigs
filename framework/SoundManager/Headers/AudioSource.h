#ifndef _AUDIOSOURCE_H_
#define _AUDIOSOURCE_H_

#include "CoordinateSystem.h"

class AudioBuffer;
class	ModuleSoundManager;

/**
* \class AudioSource
* \brief the audio source holds parameters for computing emitted sounds
* \ingroup SoundManager
*/
class AudioSource : public CoordinateSystem
{
public:
	DECLARE_ABSTRACT_CLASS_INFO(AudioSource,CoordinateSystem,SoundManager)
	
	/// allow ModuleSoundManager to play sources by itself (no scene graph required)
	friend class ModuleSoundManager;
	
	/// Kigs constructor
	AudioSource(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	
	/// Draw sound if the current listener is active
	bool Draw(TravState* state) override;
	
	/**
	* add an item
	* \note if item is an AudioBuffer, empty the current buffer (if needed) and add the new one
	* \return true is item was successfully added
	*/
	bool addItem(CoreModifiable *item, ItemPosition pos = Last DECLARE_DEFAULT_LINK_NAME) override;
	
	/**
	* remove an item
	* \note if item is an AudioBuffer, empty and discard it
	* \return true is item was successfully removed
	*/
	bool removeItem(CoreModifiable* item DECLARE_DEFAULT_LINK_NAME) override;

	/**
	* audio source must be always drawned
	* 
	*/
	void	GetBoundingBox(Point3D& pmin,Point3D& pmax) const override
	{
		pmin.Set(-KFLOAT_CONST(1024.f),-KFLOAT_CONST(1024.f),-KFLOAT_CONST(1024.f));
		pmax.Set(KFLOAT_CONST(1024.f),KFLOAT_CONST(1024.f),KFLOAT_CONST(1024.f));
	}

	
protected:
	/// destructor
	virtual ~AudioSource(); 
	
	/// method used to play sound data from current buffer
	virtual void Protected1Draw(TravState* state)=0;
	
	/// sound pitch (affect sound speed and tone)
	maFloat myPitch;
	/// sound gain (affect sound volume)
	maFloat myGain;
	/// looping flag (if true, sound will loop)
	maBool  myIsLooping;
	/// playing flag (true if the sound is playing)
	maBool  myIsPlaying;
	/// current state (0:stop, 1:play, 2:pause)
	maUInt  myState;
	
	/// minimum distance used for attenuation computing
	maFloat m_ReferenceDistance;
	/// maximum distance used for attenuation computing
	maFloat m_MaxDistance;
	/// attenuation factor (higher value means faster attenuation)
	maFloat m_RollOfFactor;
	
	/// current sound buffer
	AudioBuffer* myBuffer;

	ModuleSoundManager*					mySoundManager;

	
};    

#endif //_AUDIOSOURCE_H_
