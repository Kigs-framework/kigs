#include "Node3DDelayed.h"
#include "Camera.h"
#include "CullingObject.h"
#include "CoreBaseApplication.h"

#include "GLSLDebugDraw.h"

#include "TecLibs/Math/Algorithm.h"

#include "blockingconcurrentqueue.h"

//IMPLEMENT_AND_REGISTER_CLASS_INFO(Node3DDelayed, Node3DDelayed, SceneGraph);
IMPLEMENT_CLASS_INFO(Node3DDelayed)


#ifdef KIGS_TOOLS
#include "KigsTools.h"
#include "imgui.h"
#endif

using Job = std::pair<CoreModifiable*, std::function<void()>>;
static std::atomic_bool sNode3DThreadStarted = { false };

static moodycamel::BlockingConcurrentQueue<Job> sHighPriorityWorkQueue;
static moodycamel::BlockingConcurrentQueue<Job> sLowPriorityWorkQueue;


//#define DEBUG_NODE3D_DELAYED
#ifdef DEBUG_NODE3D_DELAYED
bool gFreezeNode3DDelayed = false;
#else
constexpr bool gFreezeNode3DDelayed = false;
#endif

#ifdef KIGS_TOOLS
extern std::function<bool(Node3D*)> gDrawBBoxForNode;
#endif


bool Node3DDelayed::sIsFrozen = false;
BBox Node3DDelayed::sAllowedBBox = BBox{v3f(-FLT_MAX, -FLT_MAX, -FLT_MAX), v3f(FLT_MAX, FLT_MAX, FLT_MAX)};

IMPLEMENT_CONSTRUCTOR(Node3DDelayed)
{
	auto before = sNode3DThreadStarted.exchange(true);
	if (!before)
	{
		std::thread th([]() 
		{
			while (true)
			{
				Job job;
				while(sHighPriorityWorkQueue.try_dequeue(job))
				{
					job.second();
				}

				if (sLowPriorityWorkQueue.try_dequeue(job))
				{
					job.second();
				}
				else
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(33));
				}
			}

		});
		th.detach();
	}
}

Node3DDelayed::~Node3DDelayed()
{
	// TODO remove from work queue
}

void Node3DDelayed::InitModifiable()
{
	ParentClassType::InitModifiable();
	KigsCore::GetCoreApplication()->AddAutoUpdate(this);
	mBBoxMin.changeNotificationLevel(Owner);
	mBBoxMax.changeNotificationLevel(Owner);
}

void Node3DDelayed::NotifyUpdate(const unsigned int labelid)
{
	ParentClassType::NotifyUpdate(labelid);
	if (labelid == mBBoxMin.getID() || labelid == mBBoxMax.getID())
	{
		SetFlag(LocalToGlobalMatrixIsDirty);
		SetFlag(GlobalToLocalMatrixIsDirty);
		SetFlag(BoundingBoxIsDirty);
		SetFlag(GlobalBoundingBoxIsDirty);

		PropagateDirtyFlagsToSons(this);
		PropagateDirtyFlagsToParents(this);
	}
}

bool Node3DDelayed::Draw(TravState* state)
{
	if (!ParentClassType::Draw(state)) return false;

	if (sIsFrozen || !mAllowLoading) return true;

	if ((UpdateMode)(int)mUpdateMode != UpdateMode::Camera) return true;

	auto frame_number = state->GetFrameNumber();
	if (mLastFrameNumber != frame_number)
	{
		mLastFrameNumber = frame_number;
		auto gbb = GetGlobalBoundingBox();
		if (Intersection::IntersectionAABBAABB(sAllowedBBox.m_Min, sAllowedBBox.m_Max, gbb.m_Min, gbb.m_Max))
		{
			mDrawCounter++;
		}
	}
	
	if (mAllowLoading 
		&& mLoadState == LoadState::NotLoaded
		&& !gFreezeNode3DDelayed)
	{
		auto gbb = GetGlobalBoundingBox();
		if (Intersection::IntersectionAABBAABB(sAllowedBBox.m_Min, sAllowedBBox.m_Max, gbb.m_Min, gbb.m_Max))
		{
			SetShowContent(true);
		}
	}


	return true;
}



void Node3DDelayed::GetNodeBoundingBox(Point3D &pmin, Point3D &pmax) const
{
	if (mDisplayState == DisplayState::Displayed)
	{
		ParentClassType::GetNodeBoundingBox(pmin, pmax);
		pmin -= v3f(1, 1, 1);
		pmax += v3f(1, 1, 1);
		return;
	}
	pmin = mBBoxMin;
	pmax = mBBoxMax;
}

/*
void Node3DDelayed::ChangeLoadedState(LoadedState state, bool no_async)
{
	auto old_state = mLoadedState.exchange(state);
	if (old_state == state)
		return;

	if (old_state == LoadedState::Displayed)
	{
		if (mContentAdded)
		{
			removeItem(mContent.get());
			mContentAdded = false;
		}
	}
	
	if (state == LoadedState::Loaded)
	{
		mHideTime = std::chrono::steady_clock::now();
		return;
	}

	if (state == LoadedState::Unloaded)
	{
		if (mContentAdded)
		{
			removeItem(mContent.get());
			mContentAdded = false;
		}
		mContentReady = false;
		mContent.Reset();
		return;
	}


	auto job = Job{ this, [this, state]()
	{
		if (mLoadedState != state) return;
		if (state == LoadedState::Loaded || state == LoadedState::Displayed)
		{
			if (!mContent)
				mContent = OwningRawPtrToSmartPtr(Import(mFileName));

			mContentReady = true;
		}
		else
		{
			mContentReady = false;
		}
	} };

	if (!no_async)
	{
		if(mHighPrio)
			sHighPriorityWorkQueue.enqueue(std::move(job));
		else
			sLowPriorityWorkQueue.enqueue(std::move(job));
	}
	else
		job.second();
	
	if(state == LoadedState::Displayed)
	{
		mDrawCounter = 0;
		if(mContentReady)
		{
			addItem(mContent.get());
			mContentAdded = true;
			return;
		}
	}
}
*/

void Node3DDelayed::LoadContent(bool no_async)
{
	LoadState exp = LoadState::NotLoaded;
	if (mLoadState.compare_exchange_strong(exp, LoadState::InQueue))
	{
		auto job = Job{ this, [this]()
		{
			LoadState exp = LoadState::InQueue;
			if (mLoadState.compare_exchange_strong(exp, LoadState::Loading))
			{
				mContent = Import(mFileName);
				mLoadState = LoadState::Loaded;
			}
		} };

		if (!no_async)
		{
			if (mHighPrio)
				sHighPriorityWorkQueue.enqueue(std::move(job));
			else
				sLowPriorityWorkQueue.enqueue(std::move(job));
		}
		else
			job.second();
	}
}

void Node3DDelayed::UnloadContent()
{
	LoadState exp1 = LoadState::Loaded;
	LoadState exp2 = LoadState::InQueue;
	
	mDisplayState = DisplayState::Hidden;

	if (mLoadState.compare_exchange_strong(exp1, LoadState::NotLoaded)
		|| mLoadState.compare_exchange_strong(exp2, LoadState::NotLoaded))
	{
		if (mContentAdded)
			removeItem((CMSP&)mContent);
		mContentAdded = false;
		mContent.Reset();
	}
}

void Node3DDelayed::Update(const Timer&, void*)
{
	if (mDisplayState == DisplayState::Displayed 
		&& !mContentAdded 
		&& mLoadState == LoadState::Loaded
		&& !sIsFrozen)
	{
		addItem((CMSP&)mContent);
		mContentAdded = true;
		mDrawCounter = 0;
	}

	if ((UpdateMode)(int)mUpdateMode != UpdateMode::Camera) return;
	if (!mAllowLoading) return;

	if(!sIsFrozen)
		--mDrawCounter;

	if (mLoadState == LoadState::Loaded)
	{
		// If the node has been hidden for a while, unload it
		if (mDisplayState == DisplayState::Hidden)
		{
			std::chrono::duration<double> t = std::chrono::steady_clock::now() - mHideTime;
			if (t.count() > mFreeContentTime)
			{
				UnloadContent();
			}
		}

		// if the node hasn't been drawn in a while, hide it
		if (mDrawCounter < -5 && mDisplayState == DisplayState::Displayed)
		{
			SetShowContent(false);
		}
	}
}

void Node3DDelayed::SetShowContent(bool show, bool no_async)
{
	DisplayState exp = !show ? DisplayState::Displayed : DisplayState::Hidden;
	if (mDisplayState.compare_exchange_strong(exp, show ? DisplayState::Displayed : DisplayState::Hidden))
	{
		if (show)
		{
			LoadContent(no_async);
		}
		else
		{
			mHideTime = std::chrono::steady_clock::now();
			if (mContentAdded)
			{
				removeItem((CMSP&)mContent);
				mContentAdded = false;
			}
		}
	}
}

void Node3DDelayed::SetContent(SmartPointer<Node3D> content, bool force_display)
{
	EmptyItemList();
	addItem((CMSP&)content);

	if (force_display)
	{
		mDisplayState = DisplayState::Displayed;
	}

	if (mDisplayState == DisplayState::Displayed)
	{
		mContentAdded = true;
	}

	auto bb = content->GetLocalBoundingBox();
	mBBoxMin = bb.m_Min;
	mBBoxMax = bb.m_Max;

	if (!mContentAdded)
		removeItem((CMSP&)content);

	mContent = content;
	mLoadState = LoadState::Loaded;
}

void Node3DDelayed::PrepareExport(ExportSettings *settings)
{
	ParentClassType::PrepareExport(settings);
	if (mDisplayState == DisplayState::Displayed)
	{
		removeItem((CMSP&)mContent);
		mContentAdded = false;
	}

	if (mFileName.const_ref().empty())
	{
		mFileName = getName() + "_content.xml";
	}

	if (mContent)
		Export(mFileName, mContent.get(), true, settings);
}

void Node3DDelayed::EndExport(ExportSettings *settings)
{
	if (mDisplayState == DisplayState::Displayed)
	{
		addItem((CMSP&)mContent);
		mContentAdded = true;
	}
	ParentClassType::EndExport(settings);
}