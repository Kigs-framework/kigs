#pragma once    

#include "Node3D.h"
#include "maReference.h"

#include <atomic>

class Node3DDelayed : public Node3D
{
public:
    DECLARE_CLASS_INFO(Node3DDelayed, Node3D, SceneGraph);
    DECLARE_CONSTRUCTOR(Node3DDelayed);
	
	static bool sIsFrozen;
	static BBox sAllowedBBox;

    /*enum class LoadedState
    {
        Unloaded,
        Loaded,
        Displayed
    };*/

	enum class DisplayState
	{
		Hidden,
		Displayed
	};

	enum class LoadState
	{
		NotLoaded,
		InQueue,
		Loading,
		Loaded
	};

    void GetNodeBoundingBox(Point3D& pmin, Point3D& pmax) const override;

	bool Draw(TravState* state) override;

    void SetContent(SmartPointer<Node3D> content, bool force_display=false);
    Node3D* GetContent() { return mContent.get(); }

    bool SetAllowLoadedStateChange(bool allow)
    {
        bool expected = !allow;
        return mAllowLoading.compare_exchange_strong(expected, allow);
    }

	void LoadContent(bool no_async = false);
	void UnloadContent();

	void SetShowContent(bool show, bool no_async = false);

	//void ChangeLoadedState(LoadedState loaded, bool no_async = false);

protected:
    
    virtual ~Node3DDelayed();

    void PrepareExport(ExportSettings*) override;
    void EndExport(ExportSettings*) override;

    void InitModifiable() override;
    void Update(const Timer&, void*) override;

    void NotifyUpdate(const unsigned int labelid) override;
    
    maString mFileName = BASE_ATTRIBUTE(FileName, "");
    maVect3DF mBBoxMin = BASE_ATTRIBUTE(BBoxMin, 0, 0, 0);
    maVect3DF mBBoxMax = BASE_ATTRIBUTE(BBoxMax, 0, 0, 0);

    maVect3DF mDistanceMultiplier = BASE_ATTRIBUTE(DistanceMultiplier, 1, 1, 1);

    maFloat mLoadDistance = BASE_ATTRIBUTE(LoadDistance, 7.5f);
    maFloat mUnloadDistance = BASE_ATTRIBUTE(UnloadDistance, 10.0f);

    enum class UpdateMode
    {
        Camera,
        Manual
    };

    maEnum<2> mUpdateMode = BASE_ATTRIBUTE(UpdateMode, "Camera", "Manual");
    maReference mReferenceObject = BASE_ATTRIBUTE(ReferenceObject, "Camera:camera");

	maBool mHighPrio = BASE_ATTRIBUTE(HighPriority, false);

    //std::atomic<LoadedState> mLoadedState { LoadedState::Unloaded };

	std::atomic<LoadState> mLoadState{ LoadState::NotLoaded };
	std::atomic<DisplayState> mDisplayState{ DisplayState::Hidden};

    SmartPointer<Node3D> mContent;

	std::atomic_bool mAllowLoading = { false };
	//std::atomic_bool mContentReady = { false };

    bool mContentAdded = false;

	maDouble mFreeContentTime = BASE_ATTRIBUTE(FreeContentTime, 1.0);
	std::chrono::time_point<std::chrono::steady_clock> mHideTime;

	int mDrawCounter = INT_MIN;
	u32 mLastFrameNumber = UINT_MAX;
};
