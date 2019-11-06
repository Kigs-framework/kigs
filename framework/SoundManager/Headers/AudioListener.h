#ifndef _AUDIOLISTENER_H_
#define _AUDIOLISTENER_H_

#include "CoordinateSystem.h"
#include "Drawable.h"

/**
* \class AudioListenerActivator
* \brief the audio listener activator is a drawable used to update and activate audio listener during predraw process, the activator is always attached under its listener
* \ingroup SoundManager
*/

class	AudioListener;
class	ModuleSoundManager;

class AudioListenerActivator : public Drawable
{
public:
	DECLARE_CLASS_INFO(AudioListenerActivator,Drawable,SoundManager)
	/// Kigs constructor
	AudioListenerActivator(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	
	/// compute time, position and velocity, and activate current listener
	bool PreDraw(TravState*) override;
	
	/// desactivate current listener and activate the next one
	bool PostDraw(TravState* travstate) override;
	
	/// bounding box update (does nothing)
	bool BBoxUpdate(kdouble time) override;	
	
	/// bounding box accessor
	void GetBoundingBox(Point3D& pmin,Point3D& pmax) const override;

	friend class AudioListener;

protected:

	void	SetFatherListener(AudioListener*	father)
	{
		myFatherListener=father;
	}

	/// last update time
	kdouble  myLastTime;
	/// last update position
	Point3D  myLastPos;
	/// current position
	Point3D  myCurrentPos;
	/// velocity
	Vector3D myVelocity;

	AudioListener*	myFatherListener;
	/// destructor
	virtual ~AudioListenerActivator(); 
};

/**
* \class AudioListener
* \brief the audio listener holds parameters for computing heard sounds from audio sources
* \ingroup SoundManager
*/
class AudioListener : public CoordinateSystem
{
public:
	DECLARE_ABSTRACT_CLASS_INFO(AudioListener,CoordinateSystem,SoundManager)
	
	/// Kigs constructor
	AudioListener(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	
	/// \return true if the current listener is active
	bool ListenerIsActive()
	{
		return myIsActive;
	}
	
	/// transform the specified position and velocity vector into local coordinates
	void TransformSource(Point3D& pos,Vector3D& vel);

	friend class AudioListenerActivator;

	// warning ! Initialisation must occur after node is attached to the scene 
	void		InitModifiable() override;
	
	// push this listener on the stack
	void PushOnStack(TravState* state);
	
	// pop this listener from the stack
	void PopFromStack(TravState* state);
	
protected:
	/// destructor
	virtual ~AudioListener(); 
	
	/// activation method
	virtual void Activate()=0;
	
	/// desactivation method
	virtual void Desactivate()=0;
	
	/// name of the camera
	maString myCameraName;
	
	/// sound gain
	maFloat myGain;
	
	/// active state
	bool myIsActive;
	
	AudioListenerActivator*				myActivator;

	// pointer on ModuleSoundManager

	ModuleSoundManager*					mySoundManager;

	bool								myIsAttachToScene;
	
};    

/*
inline code
*/
inline bool AudioListenerActivator::BBoxUpdate(kdouble /*time*/) {return true;}

inline void AudioListenerActivator::GetBoundingBox(Point3D& pmin,Point3D& pmax)const {
	pmin.Set(KFLOAT_CONST(-1000.0),KFLOAT_CONST(-1000.0),KFLOAT_CONST(-1000.0)); 
	pmax.Set(KFLOAT_CONST(1000.0),KFLOAT_CONST(1000.0),KFLOAT_CONST(1000.0));
}

#endif //_AUDIOLISTENER_H_