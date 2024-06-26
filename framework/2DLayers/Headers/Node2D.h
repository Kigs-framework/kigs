#pragma once

#include "CoreModifiable.h"
#include "CoreModifiableAttribute.h"
#include "TecLibs/Tec3D.h"

#include "AttributePacking.h"

namespace Kigs
{
	namespace Draw
	{
		class Drawable;
		class RendererMatrix;
	}
	namespace Scene
	{
		class TravState;
		class ModuleSceneGraph;
	}
	namespace Draw2D
	{
		using namespace Kigs::Core;
		class Abstract2DLayer;

		// ****************************************
		// * Node2D class
		// * --------------------------------------
		/**
		* \file	Node2D.h
		* \class	Node2D
		* \ingroup  2DLayers
		* \brief	A Node2D is the base class for all objects to be added to a 2DLayer.
		*
		* A matrix is associated with each node to give the transformation applied to the node when drawing.
		*/
		// ****************************************

		class Node2D : public CoreModifiable
		{
		public:
			// realsize compute mode
			enum SizeMode
			{
				DEFAULT = 0,					// use directly the size set in mSizeX or mSizeY
				MULTIPLY = 1,					// multiply father size by mSizeX or mSizeY
				ADD = 2,						// add father size to mSizeX or mSizeY
				CONTENT = 3,				// use content size ( texture size of UIImage )
				CONTENT_MULTIPLY = 4,		// multiply content size by mSizeX or mSizeY
				CONTENT_ADD = 5,			// add content size to mSizeX or mSizeY
				KEEP_RATIO = 6				// compute size x according to size y or size y according to size x. ! Both mode can't be KEEP_RATION ! 
			};

			friend class Scene::ModuleSceneGraph;

			struct PriorityCompare
			{
				//! overload operator () for comparison
				bool operator()(const Node2D* a1, const Node2D* a2) const
				{
					if (a1->mPriority == a2->mPriority)
						return (a1) < (a2);
					return a1->mPriority > a2->mPriority;
				}
			};
			friend struct PriorityCompare;

			DECLARE_CLASS_INFO(Node2D, CoreModifiable, SceneGraph)
				DECLARE_CONSTRUCTOR(Node2D);

			/*
				enum Flags
				{
					Node2D_SizeChanged = 1u << 0u,
					Node2D_Clipped = 1u << 1u,
					Node2D_Hidden = 1u << 2u,
					Node2D_NeedUpdatePosition = 1u << 3u,
					Node2D_SonPriorityChanged = 1u << 4u,
					Node2D_UseCustomShader = 1u << 5u,
					Node2D_hasBGRTexture = 1u << 6u,
					Node2D_ClipSons = 1u << 7u,
					Node2D_NeedVerticeInfoUpdate = 1u << 8u,
					Node2D_PropagatedFlags = Node2D_Clipped | Node2D_Hidden,
				};*/

			static constexpr unsigned int Node2D_SizeChanged = 1 << ParentClassType::usedUserFlags;
			static constexpr unsigned int Node2D_Clipped = 1 << (ParentClassType::usedUserFlags + 1);
			static constexpr unsigned int Node2D_Hidden = 1 << (ParentClassType::usedUserFlags + 2);
			static constexpr unsigned int Node2D_NeedUpdatePosition = 1 << (ParentClassType::usedUserFlags + 3);
			static constexpr unsigned int Node2D_SonPriorityChanged = 1 << (ParentClassType::usedUserFlags + 4);
			static constexpr unsigned int Node2D_UseCustomShader = 1 << (ParentClassType::usedUserFlags + 5);
			static constexpr unsigned int Node2D_hasBGRTexture = 1 << (ParentClassType::usedUserFlags + 6);
			static constexpr unsigned int Node2D_ClipSons = 1 << (ParentClassType::usedUserFlags + 7);
			static constexpr unsigned int Node2D_NeedVerticeInfoUpdate = 1 << (ParentClassType::usedUserFlags + 8);

			static constexpr unsigned int Node2D_PropagatedFlags = Node2D_Clipped | Node2D_Hidden;

			static constexpr unsigned int usedUserFlags = ParentClassType::usedUserFlags + 9;


			// call just before travdraw, set state for branch (this and his descendancy)
			virtual void PreTravDraw(Scene::TravState* state) {}
			virtual void PostTravDraw(Scene::TravState* state) {}

			/**
			* \brief	draw the node
			* \fn 		virtual bool	Draw(TravState* state);
			* \param	state : the current TravState
			* \return	TRUE if this node is visible or we are in a "all visible" branch of the scenegraph
			*/
			virtual bool Draw(Scene::TravState* state);

			/**
			* \brief	draw the TravState
			* \fn 		virtual void			TravDraw(TravState* state);
			* \param	state : the current TravState
			*/
			virtual void TravDraw(Scene::TravState* state);

			/**
			* \brief	this method is called to notify this that one of its attribute has changed.
			* \fn 		virtual void NotifyUpdate(const unsigned int);
			* \param	int : not used
			*/
			void NotifyUpdate(const unsigned int /* labelid */) override;

			// real drawing
			virtual void ProtectedDraw(Scene::TravState* state) { ; }

			int GetFinalPriority() const;

			v2f GetSize() const { return mRealSize; }
			v2f GetPreScale() const { return mPreScale; }
			v2f GetPostScale() const { return mPostScale; }
			v2f GetSizeScaled() const { return GetSize() * GetPreScale() * GetPostScale(); }
			virtual float GetOpacity() { return 1.0f; }

			Node2D* getFather() const;
			Abstract2DLayer* getLayerFather() const;

			bool										addItem(const CMSP& item, ItemPosition pos = Last DECLARE_DEFAULT_LINK_NAME) override;
			bool										removeItem(const CMSP& item DECLARE_DEFAULT_LINK_NAME) override;

			inline const std::set<Node2D*, Node2D::PriorityCompare>& GetSons() { ResortSons(); return mSons; }

			// transform local points to global
			inline void	TransformPoints(v2f* totransform, int count) const
			{
				for(auto i=0;i<count;i++)
				{
					*totransform = mGlobalTransformMatrix * v3f(totransform->x, totransform->y,1.0f);
					++totransform;
				}
			}

			v2f GetPosition() const { return mPosition; }
			void GetPosition(float& X, float& Y) const
			{
				X = (float)mPosition[0];
				Y = (float)mPosition[1];
			}

			void	GetGlobalPosition(float& X, float& Y);

			inline const mat3& GetGlobalTransform() const
			{
				return mGlobalTransformMatrix;
			}

			void GetTransformedPoints(v2f* pt);


			virtual void SetUpNodeIfNeeded();
			// recurse to retreive root 2D Layer
			Abstract2DLayer* getRootLayerFather() const;
			Node2D* getRootFather();

			inline bool												GetNodeFlag(u32 f)
			{
				return isUserFlagSet(f);
			}
			inline void												SetNodeFlag(u32 f)
			{
				setUserFlag(f);
			}
			inline void												ClearNodeFlag(u32 f)
			{
				unsetUserFlag(f);
			}

			inline void												ChangeNodeFlag(u32 f, bool set)
			{
				if (set)
				{
					SetNodeFlag(f);
				}
				else
				{
					ClearNodeFlag(f);
				}
			}

			bool IsHiddenFlag() const { return isUserFlagSet(Node2D_Hidden); }
			bool IsInClip(v2f pos) const;

		protected:

			virtual v2f	GetContentSize()
			{
				return mRealSize;
			}

			/**
			* \brief	initialize modifiable
			* \fn 		void InitModifiable() override
			*/
			void	InitModifiable() override;

			void SetParent(CoreModifiable* value);

			void PropagateNodeFlags();

			void												PropagateNeedUpdateToFather();

			void												ComputeMatrices();
			virtual void										ComputeRealSize();
			void												ResortSons();

			Node2D* mParent;
			std::set<Node2D*, Node2D::PriorityCompare>			mSons;

			v2f													mRealSize{ 0,0 };

			mat3											mLocalTransformMatrix;
			mat3											mGlobalTransformMatrix;
			maEnum<7>											mSizeModeX = BASE_ATTRIBUTE(SizeModeX, "Default", "Multiply", "Add", "Content", "ContentMult", "ContentAdd", "KeepRatio");
			maEnum<7>											mSizeModeY = BASE_ATTRIBUTE(SizeModeY, "Default", "Multiply", "Add", "Content", "ContentMult", "ContentAdd", "KeepRatio");


			int													mPriority = 0;
			float												mRotationAngle = 0.0f;			
			bool												mClipSons = false;
			v2f													mSize = {0.0f,0.0f};
			v2f													mDock = { 0.0f,0.0f };
			v2f													mAnchor = { 0.0f,0.0f };
			v2f													mPosition = { 0.0f,0.0f };
			v2f													mPreScale = { 1.0f,1.0f };
			v2f													mPostScale = { 1.0f,1.0f };


			WRAP_ATTRIBUTES(mPriority, mRotationAngle, mSize, mDock, mAnchor, mPosition, mPreScale, mPostScale, mClipSons);

			maReference											mCustomShader = BASE_ATTRIBUTE(CustomShader, "");

			WRAP_METHODS(GetSize);
		};

	}
}
