#include "GazeWUP.h"
#include "CoreBaseApplication.h"
#include "Core.h"
#include "DeviceItem.h"
#include "Timer.h"
#include "TouchInputEventManager.h"

#include <mutex>

#include "Platform/Main/BaseApp.h"

using namespace winrt::Windows::UI::Input::Spatial;

IMPLEMENT_CLASS_INFO(GazeDeviceWUP);

IMPLEMENT_CONSTRUCTOR(GazeDeviceWUP)
{
	mSpatialInteractionManager = SpatialInteractionManager::GetForCurrentView();
	StartListening();
}

GazeDeviceWUP::~GazeDeviceWUP()
{  

}    

void GazeDeviceWUP::StartListening()
{
	if (!mIsListening)
	{
		mInteractionDetectedToken = mSpatialInteractionManager.InteractionDetected([this](const SpatialInteractionManager& manager, SpatialInteractionDetectedEventArgs args)
		{
		});
		mSourcePressedToken = mSpatialInteractionManager.SourcePressed([this](const SpatialInteractionManager& manager, SpatialInteractionSourceEventArgs args) 
		{
			std::lock_guard<std::mutex> lk{ mMutex };

			if (args.State().Source().Kind() == SpatialInteractionSourceKind::Voice)
			{
				mVoiceSelect = true;
				return;
			}

			auto it = mUpdateList.find(args.State().Source().Id());
			if (it != mUpdateList.end())
			{
				it->second.state = SourceState::Pressed;
				it->second.args = args;
			}
		});
		mSourceReleasedToken = mSpatialInteractionManager.SourceReleased([this](const SpatialInteractionManager& manager, SpatialInteractionSourceEventArgs args) 
		{
			std::lock_guard<std::mutex> lk{ mMutex };
			auto it = mUpdateList.find(args.State().Source().Id());
			if (it != mUpdateList.end())
			{
				it->second.state = SourceState::Released;
				it->second.args = args;
			}
		});
		mSourceLostToken = mSpatialInteractionManager.SourceLost([this](const SpatialInteractionManager& manager, SpatialInteractionSourceEventArgs args)
		{
			std::lock_guard<std::mutex> lk{ mMutex };
			auto it = mUpdateList.find(args.State().Source().Id());
			if (it != mUpdateList.end())
			{
				it->second.state = SourceState::Lost;
				it->second.args = args;
			}
		});
		mSourceUpdatedToken = mSpatialInteractionManager.SourceUpdated([this](const SpatialInteractionManager& manager, SpatialInteractionSourceEventArgs args)
		{
			std::lock_guard<std::mutex> lk{ mMutex };
			auto it = mUpdateList.find(args.State().Source().Id());
			if (it != mUpdateList.end())
			{
				it->second.state = SourceState::Updated;
				it->second.args = args;
			}
		});
		mSourceDetectedToken = mSpatialInteractionManager.SourceDetected([this](const SpatialInteractionManager& manager, SpatialInteractionSourceEventArgs args)
		{
			std::lock_guard<std::mutex> lk{ mMutex };
			auto& source = mUpdateList[args.State().Source().Id()];
			source.state = SourceState::Detected;
			source.args = args;
		});
		mIsListening = true;
	}
}

void GazeDeviceWUP::StopListening()
{
	if (mIsListening)
	{
		mSpatialInteractionManager.InteractionDetected(mInteractionDetectedToken);
		mSpatialInteractionManager.InteractionDetected(mSourcePressedToken);
		mSpatialInteractionManager.InteractionDetected(mSourceReleasedToken);
		mSpatialInteractionManager.InteractionDetected(mSourceLostToken);
		mSpatialInteractionManager.InteractionDetected(mSourceUpdatedToken);
		mSpatialInteractionManager.InteractionDetected(mSourceDetectedToken);
		mIsListening = false;
	}
}

bool	GazeDeviceWUP::Aquire()
{
	if (GazeDevice::Aquire())
	{
		return true;
	}
	return false;
}

bool	GazeDeviceWUP::Release()
{
	if (GazeDevice::Release())
	{
		return true;
	}
	return false;
}

void	GazeDeviceWUP::UpdateDevice()
{
	// remove old touch
	{
		auto itr = myTouchList.begin();
		auto end = myTouchList.end();
		while (itr != end) 
		{
			if (itr->second.removed)
			{
				itr = myTouchList.erase(itr);
			}
			else 
			{
				++itr;
			}
		}
	}

	std::lock_guard<std::mutex> lk{ mMutex };

	if (mVoiceSelect)
	{
		ModuleInput::Get()->getTouchManager()->ForceClick();
		mVoiceSelect = false;
	}

	std::vector<u32> toRemove;
	GazeTouch* s = nullptr;
	if (mUpdateList.size())
	{
		double time = KigsCore::GetCoreApplication()->GetApplicationTimer()->GetTime();
		for(auto& itr : mUpdateList)
		{
			auto& source = itr.second;
			auto id = itr.first;
			// keep only hand (for now)
			auto kind = source.args.State().Source().Kind();
			if (kind != SpatialInteractionSourceKind::Hand &&
				kind != SpatialInteractionSourceKind::Controller)
			{
				kigsprintf("discard interaction %d\n", kind);
				toRemove.push_back(id);
				continue;
			}

			auto found = myTouchList.find(id);
			if (found != myTouchList.end()) // update
			{
				s = &found->second;
			}
			else //create new one
			{
				s = &myTouchList[id];
				s->Time = time;
			}

			s->ID = id;
			auto before = s->pressed;
			s->pressed = (time - s->Time>0.1)&&(source.args.State().IsPressed());

			/*if (before != s->pressed)
			{
				kigsprintf("ID: %d : %s\n", s->ID, s->pressed ? "pressed" : "not pressed");
			}*/

			if (source.args.State().Source().Controller() && source.args.State().Source().Controller().HasThumbstick())
			{
				myThumbstickList[id] = v2f{ (float)source.args.State().ControllerProperties().ThumbstickX(), (float)source.args.State().ControllerProperties().ThumbstickY() };
			}

			if (itr.second.state != SourceState::Lost)
			{
				SpatialInteractionSourceLocation sourceLoc = source.args.State().Properties().TryGetLocation(App::GetApp()->GetStationaryReferenceFrame().CoordinateSystem());
				if (sourceLoc)
				{
					if (sourceLoc.SourcePointerPose())
					{
						s->Position.Set(sourceLoc.SourcePointerPose().Position().x, sourceLoc.SourcePointerPose().Position().y, sourceLoc.SourcePointerPose().Position().z);
						s->Forward.Set(sourceLoc.SourcePointerPose().ForwardDirection().x, sourceLoc.SourcePointerPose().ForwardDirection().y, sourceLoc.SourcePointerPose().ForwardDirection().z);
						s->Up.Set(sourceLoc.SourcePointerPose().UpDirection().x, sourceLoc.SourcePointerPose().UpDirection().y, sourceLoc.SourcePointerPose().UpDirection().z);
						s->hasPosition = true;
					}
					else if (sourceLoc.Position())
					{
						if (sourceLoc.Orientation())
						{
							Quaternion q;
							q.Set(sourceLoc.Orientation().Value().x, sourceLoc.Orientation().Value().y, sourceLoc.Orientation().Value().z, sourceLoc.Orientation().Value().w);
							v3f forward{ 0,0,1 };
							v3f up{ 0,1,0 };
							s->Forward = q * forward;
							s->Up = q * up;
						}
						s->Position.Set(sourceLoc.Position().Value().x, sourceLoc.Position().Value().y, sourceLoc.Position().Value().z);
						s->hasPosition = true;
					}
				}
			}

			s->state = itr.second.state;
			if (itr.second.state == SourceState::Lost)
			{
				s->removed = true;
				s->pressed = false;
				toRemove.push_back(id);
			}
		}
	}

	for(auto it : toRemove)
	{
		mUpdateList.erase(it);
	}
}

void	GazeDeviceWUP::DoInputDeviceDescription()
{
}

bool GazeDeviceWUP::GetTouchPosition(u32 ID, v3f& pos)const
{
	auto found = myTouchList.find(ID);
	if (found == myTouchList.end())
		return false;

	pos.x = found->second.Position.x;
	pos.y = found->second.Position.y;
	pos.z = found->second.Position.z;
	return true;
}

const GazeTouch* GazeDeviceWUP::GetTouchEvent(u32 ID) const
{
	auto found = myTouchList.find(ID);
	if (found != myTouchList.end())
		return &(found->second);

	return nullptr;
}
bool GazeDeviceWUP::GetTouchState(u32 ID, SourceState& state) const
{
	auto found = myTouchList.find(ID);
	if (found == myTouchList.end())
		return false;

	state = found->second.state;
	return true;
}