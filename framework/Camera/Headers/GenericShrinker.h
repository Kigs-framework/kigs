#pragma once

#include "FrameBufferStream.h"

namespace Kigs
{
	namespace Camera
	{

		// ****************************************
		// * GenericShrinker class
		// * --------------------------------------
		/**
		* \file	GenericShrinker.h
		* \class	GenericShrinker
		* \ingroup Camera
		* \brief Reduce frame buffer resolution.
		*/
		// ****************************************
		class GenericShrinker : public FrameBufferStream
		{
		public:


			DECLARE_ABSTRACT_CLASS_INFO(GenericShrinker, FrameBufferStream, CameraModule)

				//! constructor
				GenericShrinker(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);

			void	GetBufferSize(int& sizex, int& sizey) override
			{
				sizex = mResizeX;
				sizey = mResizeY;
			}
		protected:

			//! destructor
			virtual ~GenericShrinker();


			maInt	mResizeX;
			maInt	mResizeY;


		};

	}
}
