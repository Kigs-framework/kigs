//#include "PrecompiledHeaders.h"
#include "AudioListener.h"
#include "ModuleSoundManager.h"
#include "TravState.h"
#include "Core.h"
#include "Camera.h"
#include "Scene3D.h"


IMPLEMENT_CLASS_INFO(AudioListenerActivator)


AudioListenerActivator::AudioListenerActivator(const kstl::string& name,CLASS_NAME_TREE_ARG)
: Drawable(name,PASS_CLASS_NAME_TREE_ARG)
{
   m_PassMask = 1; // actif une seule fois !
     myLastTime=-1;
}

AudioListenerActivator::~AudioListenerActivator() {}

bool AudioListenerActivator::PreDraw(TravState* state)
{
	Matrix3x4 myGlobalToLocal=(myFatherListener->GetGlobalToLocal());
	
	kdouble currentTime= state->GetTime();
	
	if(myLastTime!=currentTime)
	{
		if(myLastTime==KDOUBLE_CONST(-1.f))
		{
			myLastTime= currentTime;
			myLastPos.Set(myGlobalToLocal.e[3][0],myGlobalToLocal.e[3][1],myGlobalToLocal.e[3][2]);  
			myVelocity.Set(0,0,0);
		}
		else
		{
			myVelocity.Set(myGlobalToLocal.e[3][0],myGlobalToLocal.e[3][1],myGlobalToLocal.e[3][2]);
			myVelocity-=myLastPos;
			myVelocity*=(kfloat) (currentTime-myLastTime);
			myLastPos.Set(myGlobalToLocal.e[3][0],myGlobalToLocal.e[3][1],myGlobalToLocal.e[3][2]);  
			myLastTime=state->GetTime();
		}
	}

	myCurrentPos.Set(myGlobalToLocal.e[3][0],myGlobalToLocal.e[3][1],myGlobalToLocal.e[3][2]);

	if(Drawable::PreDraw(state))
	{	
		myFatherListener->PushOnStack(state);
		return true;
	}
	
	return false;
}


bool AudioListenerActivator::PostDraw(TravState* travstate)
{
	if(Drawable::PostDraw(travstate))
	{
		myFatherListener->PopFromStack(travstate);
		return true;
	}
	return false;
}

IMPLEMENT_CLASS_INFO(AudioListener)


AudioListener::AudioListener(const kstl::string& name,CLASS_NAME_TREE_ARG)
: CoordinateSystem(name,PASS_CLASS_NAME_TREE_ARG),
myCameraName(*this,false,LABEL_AND_ID(CameraName),""),
myGain(*this,false,LABEL_AND_ID(Gain),KFLOAT_CONST(1.0))
{
   myIsActive = false;

  
	// retreive listener stack
	kstl::set<CoreModifiable*>	instances;
	CoreModifiable::GetInstances("ModuleSoundManager",instances);

	// we know only one instance is available
	mySoundManager=(ModuleSoundManager*)(*instances.begin());
}


AudioListener::~AudioListener() {}

void  AudioListener::InitModifiable()
{
	CoreModifiable::InitModifiable();
	if(_isInit)
	{
		bool addToScene=true;
		// if a camera is attached to this, then only add activator to father
		// else add activator to the scene
		if(((kstl::string&)myCameraName) != "")
		{
			addToScene=false;
		}

		// search Scene3D node
		Node3D* scene=0;
		Node3D* father=this;

		while(father)
		{
			const kstl::vector<CoreModifiable*>& parents=father->GetParents();
			kstl::vector<CoreModifiable*>::const_iterator it;
			father=0;
			for(it=parents.begin();it!=parents.end();++it)
			{
				if((*it)->isSubType(Node3D::myClassID))
				{
					//! the father node was found
					father=(Node3D*)(*it);
					//! a Node3D should only have one Node3D father so we can break here
					break;
				}
			}
			if(addToScene) // continue until scene is found
			{
				if(father->isSubType(Scene3D::myClassID))
				{
					scene=(Node3D*)father;
					break;
				}
			}
			else // just add to first father
			{
				scene=(Node3D*)father;
				break;
			}
		}
		if(scene)
		{
			// create drawable activator
			myActivator = (AudioListenerActivator*)KigsCore::GetInstanceOf(getName()+"AudioListenerActivator",_S_2_ID("AudioListenerActivator"));
			myActivator->Init();

			//! add renderer matrix to this 
			scene->addItem(myActivator);
			myActivator->Destroy();

			myActivator->SetFatherListener(this);
		}
		else	// bad init, listener is not fully attached to scenegraph 
		{
			UninitModifiable();
		}
	}

}


void	AudioListener::PushOnStack(TravState* state)
{
	// if a camera is attached to this listener, then push it on stack only 
	// if the good camera is activated

	bool	DoActivation=false;
	if(((kstl::string&)myCameraName) != "")
	{
		if(((kstl::string&)myCameraName) == state->GetCurrentCamera()->getName())
		{
			DoActivation=true;
		}
	}
	else
	{
		DoActivation=true;
	}

	if(DoActivation)
	{	
		kstl::vector<AudioListener*>& myListenerStack=mySoundManager->GetListenerStack();
		
		// deactivate current listener
		unsigned long currentsize=myListenerStack.size();
		if(currentsize)
		{
			myListenerStack[currentsize-1]->Desactivate();
		}

		// push this on stack and activate it
		myListenerStack.push_back(this);
		this->Activate();
		myIsActive=true;
	}
}

void	AudioListener::PopFromStack(TravState* /*state*/)
{

	kstl::vector<AudioListener*>& myListenerStack=mySoundManager->GetListenerStack();
	unsigned long currentsize=myListenerStack.size();
	if(currentsize)
	{
		// check if current listener is on stack
		if(myListenerStack[currentsize-1] == this)
		{
			// if this is on stack, then deactivate this and pop stack
			myListenerStack[currentsize-1]->Desactivate();
			myListenerStack.pop_back();

			// if another listener is there, activate it
			currentsize=myListenerStack.size();
			if(currentsize)
			{
				myListenerStack[currentsize-1]->Activate();
			}
			myIsActive=false;
		}
	}
}



void AudioListener::TransformSource(Point3D& pos,Vector3D& vel)
{
	GetGlobalToLocal().TransformPoints(&pos,1);
	GetGlobalToLocal().TransformVector(&vel);
	vel-=myActivator->myVelocity;

}
