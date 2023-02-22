#pragma once    

#include "Node3D.h"
#include "maReference.h"

#include <atomic>

namespace Kigs
{

    namespace Scene
    {

        // ****************************************
        // * Node3DDelayed class
        // * --------------------------------------
        /**
         * \file	Node3DDelayed.h
         * \class	Node3DDelayed
         * \ingroup SceneGraph
         * \brief	A Node3D loading it's sons only when needed.
         *
         */
         // ****************************************

        class Node3DDelayed : public Node3D
        {
        public:
            DECLARE_CLASS_INFO(Node3DDelayed, Node3D, SceneGraph);
            DECLARE_CONSTRUCTOR(Node3DDelayed);

            static bool sIsFrozen;
            static BBox sAllowedBBox;

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

            void SetContent(SmartPointer<Node3D> content, bool force_display = false);
            Node3D* GetContent() { return mContent.get(); }

            bool SetAllowLoadedStateChange(bool allow)
            {
                bool expected = !allow;
                return mAllowLoading.compare_exchange_strong(expected, allow);
            }

            void LoadContent(bool no_async = false);
            void UnloadContent();

            void SetShowContent(bool show, bool no_async = false);

            virtual ~Node3DDelayed();

        protected:


            void PrepareExport(ExportSettings*) override;
            void EndExport(ExportSettings*) override;

            void InitModifiable() override;
            void Update(const Time::Timer&, void*) override;

            void NotifyUpdate(const unsigned int labelid) override;

            bool                mHighPriority = false;
            float               mLoadDistance = 7.5f;
            float               mUnloadDistance = 10.0f;
            double              mFreeContentTime = 1.0;

            v3f                 mBBoxMin = { 0.0f, 0.0f, 0.0f };
            v3f                 mBBoxMax = { 0.0f, 0.0f, 0.0f };

            v3f                 mDistanceMultiplier = { 1.0f, 1.0f, 1.0f };

            std::string         mFileName = "";

            std::weak_ptr<CoreModifiable> mReferenceObject;

            WRAP_ATTRIBUTES(mHighPriority, mLoadDistance, mUnloadDistance, mFreeContentTime, mBBoxMin, mBBoxMax, mDistanceMultiplier, mFileName, mReferenceObject);

            enum class UpdateMode
            {
                Camera,
                Manual
            };

            maEnum<2> mUpdateMode = BASE_ATTRIBUTE(UpdateMode, "Camera", "Manual");

            std::atomic<LoadState> mLoadState{ LoadState::NotLoaded };
            std::atomic<DisplayState> mDisplayState{ DisplayState::Hidden };

            SmartPointer<Node3D> mContent;

            std::atomic_bool mAllowLoading = { false };
            //std::atomic_bool mContentReady = { false };

            bool mContentAdded = false;

            std::chrono::time_point<std::chrono::steady_clock> mHideTime;

            int mDrawCounter = INT_MIN;
            u32 mLastFrameNumber = UINT_MAX;
        };
    }
}