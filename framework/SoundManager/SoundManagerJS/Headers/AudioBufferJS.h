#ifndef _AUDIOBUFFERJS_H_
#define _AUDIOBUFFERJS_H_

#include "AudioBuffer.h"

class AudioBufferJS : public AudioBuffer
{
public:
	DECLARE_CLASS_INFO(AudioBufferJS, AudioBuffer, SoundManager)

	AudioBufferJS(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	virtual void  Stream(AudioSource* src)  override;
	virtual void  Empty(AudioSource* src) override;

	int  GetJSBufferID(){ return myID; }
	void Play()  override;
	virtual void								NotifyUpdate(const unsigned int labelid)  override;

protected:

	virtual bool LoadSound(const kstl::string&)  override;


  virtual ~AudioBufferJS();

  int			myID;

  maFloat		myVolume;
  maBool		myLoop;
};    

#endif //_AUDIOBUFFERJS_H_
