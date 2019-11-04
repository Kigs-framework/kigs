#ifndef _AUDIOBUFFER_H_
#define _AUDIOBUFFER_H_

#include "Drawable.h"

class AudioSource;

/**
* \class AudioBuffer
* \briefs the audio buffer holds sound data
* \ingroup SoundManager
*/
class AudioBuffer : public Drawable
{
public:
	DECLARE_ABSTRACT_CLASS_INFO(AudioBuffer,Drawable,SoundManager)
	
	/// Kigs constructor
	AudioBuffer(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	
	/// bounding box update (no bounding box, source already has a bounding box) 
	bool BBoxUpdate(kdouble /*time*/) override {return false;}

	/// method used for streaming/updating
	virtual void Stream(AudioSource* src)=0;
	
	/// method used for clearing the stream
	virtual void Empty(AudioSource* src)=0;

	/// main loading method
	virtual bool LoadSound(const kstl::string&) = 0;

	virtual void Play() = 0;
	

protected:
	/// destructor
	virtual ~AudioBuffer();    
	
	/// coreModifiable initialization
	void InitModifiable() override;
	
	/// file name
	maString myFileName;
	
};    

#endif //_AUDIOBUFFER_H_
