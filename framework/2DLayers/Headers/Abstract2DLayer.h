#pragma once

#include "Scene3D.h"
#include "maReference.h"

namespace Kigs
{
	namespace Draw
	{
		class RenderingScreen;
	}
	namespace Draw2D
	{
		using namespace Kigs::Core;
		using namespace Kigs::Scene;
		struct LayerMouseInfo;
		class DirectDrawStruct
		{
		public:
			virtual ~DirectDrawStruct() {}

			virtual void Draw(TravState* state) = 0;

		protected:
			DirectDrawStruct() {}
		};

		// ****************************************
		// * Abstract2DLayer class
		// * --------------------------------------
		/**
		* \file	Abstract2DLayer.h
		* \class	Abstract2DLayer
		* \ingroup 2DLayers
		* \brief	Base class for 2D Layers.
		*
		*/
		// ****************************************
		class Abstract2DLayer : public Scene3D
		{
		public:
		public:
			DECLARE_ABSTRACT_CLASS_INFO(Abstract2DLayer, Scene3D, 2DLayers);

			/**
			* \brief	constructor
			* \param	name : instance name
			* \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
			*/
			Abstract2DLayer(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);

			RenderingScreen* GetRenderingScreen() { return (RenderingScreen*)(CoreModifiable*)mRenderingScreen; }

			void	getSize(int& sx, int& sy)
			{
				sx = mSize[0];
				sy = mSize[1];
			}

		protected:
			void InitModifiable() override;

			//! set rendering screen active
			bool	StartDrawing(Scene::TravState* state);
			//! close rendering screen
			void	EndDrawing(Scene::TravState* state);

			//! pointer to the renderingSreen
			maReferenceInit mRenderingScreen;
			maReferenceInit mManager;

			//! size
			v2f			mSize = {-1,-1};
			bool		mClearZBuffer = false;
			bool		mClearColorBuffer = false;
			// if not interactive, don't do update (but still draw)
			bool		mIsInteractive = true;
			v4f			mClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };

			WRAP_ATTRIBUTES(mSize, mClearZBuffer, mClearColorBuffer, mIsInteractive, mClearColor);

		};
	}
}
