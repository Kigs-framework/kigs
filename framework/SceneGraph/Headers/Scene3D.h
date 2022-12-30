#pragma once

#include "Node3D.h"
#include "AttributePacking.h"
#include "TouchInputEventManager.h"

namespace Kigs
{
	namespace Draw
	{
		class	Camera;
	}
	namespace Input
	{
		struct SortItemsFrontToBackParam;
	}
	namespace Scene
	{
		using namespace Kigs::Draw;


		struct sortCamera {
			bool operator() (const Camera* lhs, const Camera* rhs) const;
		};

		class ManageFrontToBackStruct
		{
		public:
			void Init(Camera* cam);

			Vector3D	camViewVector;
			Point3D		camPos;
			// test only one bbox point
			unsigned char	BBoxPointToTestIndexes[4];
			bool flag_set_in_parents = false;
		};


		// ****************************************
		// * Scene3D class
		// * --------------------------------------
		/**
		 * \file	Scene3D.h
		 * \class	Scene3D
		 * \ingroup SceneGraph
		 * \brief   Root Node3D for a scene.
		 */
		 // ****************************************
		class Scene3D : public Node3D
		{
		public:
			DECLARE_CLASS_INFO(Scene3D, Node3D, SceneGraph);
			DECLARE_CONSTRUCTOR(Scene3D);
			WRAP_METHODS(GetCameraVector, SortItemsFrontToBack);

			int getPriority() { return mPriority; }

			const std::set<CoreModifiable*>& getLights() const { return mLights; }

			/**
			* \brief	draw the scene3D
			* \fn 		virtual bool	Draw(TravState* state);
			* \param	state : the current TravState
			* \return	TRUE if this node is visible or we are in a "all visible" branch of the scenegraph
			*/
			void			TravDraw(TravState* state) override;

			/**
			* \brief	call cull process for this node and all sons at visible positions
			* \fn 		virtual void TravCull(TravState* state);
			* \param	state : travstate
			*/
			void			TravCull(TravState* state) override;

			//! overload operator < for comparison
			inline bool operator<(Scene3D const& A) const
			{
				if ((int)mPriority == (int)A.mPriority)
				{
					return ((uptr)this < (uptr)&A);
				}
				return ((int)mPriority < (int)A.mPriority);
			}

			/**
			* \brief	to be done when changing
			* \fn 		virtual void NotifyUpdate(const unsigned int);
			* \param	int : not used
			*/
			void NotifyUpdate(const unsigned int /* labelid */) override;

			/**
			* \brief	remove a light return true if light was attached to this scene
			* \fn 		virtual bool  removeLight(CoreModifiable* light);
			* \param	light : remove a light
			*/
			virtual bool  removeLight(CoreModifiable* light);

			/**
			* \brief	add a light
			* \fn 		virtual void  addLight(CoreModifiable* light);
			* \param	light : light to add
			*/
			virtual void  addLight(CoreModifiable* light);

			/**
			* \brief	add a camera
			* \fn 		virtual void  addCamera(Camera* camera);
			* \param	camera : camera to add
			*/
			virtual void  addCamera(CoreModifiable* camera);

			/**
			* \brief	true if has a camera
			* \fn 		virtual bool  addCamera(Camera* camera);
			* \param	camera : camera checked
			*/
			virtual bool  hasCamera(CoreModifiable* camera);

			/**
			* \brief	remove a camera return true if camera was attached to this scene
			* \fn 		virtual bool  removeCamera(Camera* camera);
			* \param	camera : remove a camera
			*/
			virtual bool  removeCamera(CoreModifiable* camera);

			/**
			* \brief	true is has a light
			* \fn 		virtual bool  hasLight(CoreModifiable* light);
			* \param	light : light to add
			*/
			virtual bool  hasLight(CoreModifiable* light);

			void GetCameraVector(std::vector<CoreModifiable*>& cameras)
			{
				cameras.clear();
				for (auto caminstance : mCameras)
				{
					cameras.push_back((CoreModifiable*)caminstance);
				}

			}

			virtual void SortItemsFrontToBack(Input::SortItemsFrontToBackParam& param);

			int getSpotLightCount() { return mSpotLightCount; }
			int getPointLightCount() { return mPointLightCount; }
			int getDirLightCount() { return mDirLightCount; }

			void lightHasChanged()
			{
				mLightsHaveChanged = true;
			}
			virtual ~Scene3D();
		protected:


			/**
			* \brief	initialize modifiable
			* \fn 		void InitModifiable() override
			*/
			void	InitModifiable() override;
			void	UninitModifiable() override;

			//! link to the cameras
			std::set<Camera*, sortCamera>	mCameras;
			//! link to the lights
			std::set<CoreModifiable*>		mLights;
			bool							mLightsHaveChanged;

			int mSpotLightCount = 0;
			int mPointLightCount = 0;
			int mDirLightCount = 0;

			//! priority
			maInt			 		        mPriority;
			maBool							mSort3DNodesFrontToBack;


		};

	}
}
