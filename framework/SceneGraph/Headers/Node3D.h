#pragma once
#include "CoreModifiable.h"
#include "TecLibs/Tec3D.h"
#include "TecLibs/3D/3DObject/BBox.h"
#include "SceneNode.h"
#include "RendererMatrix.h"
#include "Upgrador.h"

namespace Kigs
{
	namespace Draw
	{
		class Drawable;
	}
	namespace Scene
	{
		class TravState;

		// ****************************************
		// * Node3D class
		// * --------------------------------------
		/**
		 * \file	Node3D.h
		 * \class	Node3D
		 * \ingroup SceneGraph
		 * \brief	Base class for 3D node in the scene graph ( with a 3x4 matrix ).
		 *
		 * A Node3D must be unique ( can not be attached to several Node3D ).
		 */
		 // ****************************************
		class Node3D : public SceneNode
		{
		public:
			DECLARE_CLASS_INFO(Node3D, SceneNode, SceneGraph)
				DECLARE_CONSTRUCTOR(Node3D)


				// return TRUE if this node is visible or we are in a "all visible" branch of the scenegraph
				virtual bool Draw(TravState* state);

			/**
			* set node up to date, and compute visibility. Return FALSE when node is totally outside of the culling volume.
			* TRUE if this node is visible or we are in a "all visible" branch of the scenegraph
			*/
			virtual bool Cull(TravState* state, unsigned int CullingMask);

			virtual void TravDraw(TravState* state);
			virtual void TravCull(TravState* state);

			const BBox& GetLocalBoundingBox() {
				if (isUserFlagSet(BoundingBoxIsDirty))
					RecomputeBoundingBox();
				return mLocalBBox;
			}
			void GetLocalBoundingBox(v3f& pmin, v3f& pmax)
			{
				if (isUserFlagSet(BoundingBoxIsDirty))
					RecomputeBoundingBox();
				pmin = mLocalBBox.m_Min; pmax = mLocalBBox.m_Max;
			}

			const BBox& GetBoundingBox() {
				if (isUserFlagSet(BoundingBoxIsDirty))
					RecomputeBoundingBox();
				return mBBox;
			}
			void GetBoundingBox(v3f& pmin, v3f& pmax)
			{
				if (isUserFlagSet(BoundingBoxIsDirty))
					RecomputeBoundingBox();
				pmin = mBBox.m_Min; pmax = mBBox.m_Max;
			}

			const BBox& GetGlobalBoundingBox() {
				if (isUserFlagSet(GlobalBoundingBoxIsDirty))
					RecomputeGlobalBoundingBox();
				return mGlobalBBox;
			}
			void	GetGlobalBoundingBox(v3f& pmin, v3f& pmax)
			{
				if (isUserFlagSet(GlobalBoundingBoxIsDirty))
					RecomputeGlobalBoundingBox();
				pmin = mGlobalBBox.m_Min; pmax = mGlobalBBox.m_Max;
			}


			bool	addItem(const CMSP& item, ItemPosition pos = Last DECLARE_DEFAULT_LINK_NAME) override;
			bool	removeItem(const CMSP& item DECLARE_DEFAULT_LINK_NAME) override;


			void SetAllSonsVisible(bool t)
			{
				if (t) SceneNode::SetAllSonsVisible();
				else SceneNode::UnsetAllSonsVisible();
			}

			/**
			* \brief	set mIsVisible member, mIsVisible store current frame index so that it can be used to test if culling was already done
			* \fn 		void SetVisible(int f)
			* \param	f : set mIsVisible to t
			*/
			inline void SetVisibleFrame(unsigned int f) { mIsVisible = f; }

			/**
			* \brief	check if the last seen frame is the current one
			* \fn 		void GetVisible(unsigned int f)
			* \param	currentFrame : index of the current frame
			*/
			inline bool IsVisibleInFrame(unsigned int currentFrame) { return mIsVisible != currentFrame; }


			Node3D* getFather() { return mFatherNode; }

			const mat4& GetLocalToGlobal() { if (isUserFlagSet(LocalToGlobalMatrixIsDirty)) RecomputeLocalToGlobal(); return mLocalToGlobal; }
			const mat4& GetGlobalToLocal() { if (isUserFlagSet(GlobalToLocalMatrixIsDirty)) RecomputeGlobalToLocal(); return mGlobalToLocal; }
			const mat4& GetLocal() { return mTransform; }

			virtual void ChangeMatrix(const mat4& newmatrix);

			void ChangeMatrixGlobal(const mat4& new_global_matrix);

			void ApplyLocalTransform(const mat4& transform);
			void ApplyGlobalTransform(const mat4& transform);

			// add rendering matrix before export if needed
			void PrepareExport(ExportSettings* settings) override;
			void EndExport(ExportSettings* settings) override;

			// Useless legacy stuff
			void ComputeGlobalBBox() {}
			void SetUpNode() {}
			void SetupNodeIfNeeded() {}
			void NeedBoundingBoxUpdate(SceneNode* node = 0, bool force = false) {}
			void NeedLocalToGlobalMatrixUpdate(bool force = false) {}
			void NeedSceneGraphUpdate(SceneNode* node = 0) {}
			void ComputeLocalToGlobal() {}
			void RecursiveUpdateAndBoundingVolume(double time) {}
			//

			bool IsScaled() const { return isUserFlagSet(IsScaledFlag); }
			void PropagateDirtyFlags(SceneNode* source);

			// use positive numbers 0,10,20,50... for priority, the higher the priority is, the earliest the node should be draw in travdraw
			void	setPriority(int priority)
			{
				mDrawPriority = priority;
				PropagateNodePriorityDirtyToParents();
			}

			int getDrawPriority()
			{
				return mDrawPriority;
			}

			//! move local node position ( pos = move + pos )
			void	localMove(const v3f& move);
			//! move global node position ( gpos = gmove + gpos )
			void	globalMove(const v3f& move);

		protected:

			void removeUser(CoreModifiable* user) override;
			void addUser(CoreModifiable* user) override;
			void InitModifiable() override;

			Node3D* mFatherNode;

			//! valid after scene TravCull : mIsVisible store current frame index if object is visible
			unsigned int mIsVisible;


			//! call predraw for drawable sons of this node return true if at least one drawable son was found
			void	PreDrawDrawable(TravState* state);
			//! call draw for drawable sons of this node
			void	DrawDrawable(TravState* state);
			//! call postdraw for drawable sons of this node
			void	PostDrawDrawable(TravState* state);

			//!	first visible Node in ModuleSceneGraph::mVisiblesNodeList
			// values are update during TravCull and used during TravDraw
			unsigned int mFirstVisibleNodeIndex;
			//!	number of visible node
			unsigned int mVisibleNodeCount;
			unsigned int mCullingMask;

			//!	Local transformation matrix (father => this local coordinate system)
			mat4 mTransform;
			//!	Local to global transformation matrix (Local coordinate system => global "world" coordinate system)
			mat4 mLocalToGlobal;
			//!	Global to local transformation matrix (Global "world" coordinate system => local coordinate system)
			mat4 mGlobalToLocal;

			//! AABounding box in local coordinate system
			BBox mLocalBBox;
			//! AABounding box in father coordinate system
			BBox mBBox;
			//! AABounding box in global "world" coordinate system
			BBox mGlobalBBox;

			bool mIgnoreBBox = false;

			void RecomputeLocalToGlobal();
			void RecomputeGlobalToLocal();
			void RecomputeGlobalBoundingBox();

			virtual void RecomputeBoundingBox();

			virtual void PropagateDirtyFlagsToSons(SceneNode* source);
			virtual void PropagateDirtyFlagsToParents(SceneNode* source);

			// draw priority management (not strict, just hint)

			int			mDrawPriority;
			void		PropagateNodePriorityDirtyToParents();
			int			ComputeNodePriority();

			WRAP_ATTRIBUTES(mIgnoreBBox);
		};

		mat4 GetLocalLookAtPoint(Node3D* node, v3f global_point, bool force_up = false, v3f up_axis = v3f(0, 1, 0));


		// base class for specific culling functions using upgrador
		class CullUpgrador : public Upgrador<Node3D>
		{
		public:
			CullUpgrador()
			{
				mFlags = 1;
			}
			virtual bool Cull(CoreModifiable* toUpdate, TravState* state, unsigned int CullingMask) = 0;

		};


#define START_CULL_UPGRADOR(name) \
	static inline const KigsID	m_ID = #name;\
	const KigsID& getID() const override {return m_ID; };\
	typedef name UpgradorType; 

#define DECLARE_CULL_UPGRADOR() class UpgradorMethods : public currentBaseClass \
	{ \
	 public:\
		UpgradorType* GetUpgrador(){return (UpgradorType*)CoreModifiable::GetUpgrador();}\
		bool Cull(TravState* state, unsigned int CullingMask);\
	};\
	public:\
	bool	UpgradorUpdate(CoreModifiable* toUpdate, const Time::Timer& timer, void* addParam) override\
	{\
		return false;\
	}\
	virtual void GetMethodTable(std::vector<std::pair<KigsID, CoreModifiable::ModifiableMethod>>& table) override\
	{\
	}\
	bool Cull(CoreModifiable* toUpdate, TravState* state, unsigned int CullingMask) override\
	{\
		return ((UpgradorMethods*)toUpdate)->Cull(state, CullingMask);\
	}

#define DEFINE_CULL_UPGRADOR_METHOD(upgrador) bool upgrador::UpgradorMethods::Cull(TravState* state, unsigned int CullingMask)

	}
}