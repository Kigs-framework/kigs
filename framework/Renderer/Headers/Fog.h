#pragma once
#include "Drawable.h"
#include "TecLibs/Tec3D.h"

namespace Kigs
{
	namespace Draw
	{
		// ****************************************
		// * Fog class
		// * --------------------------------------
		/**
		 * \file	Fog.h
		 * \class	Fog
		 * \ingroup Renderer
		 * \brief	Add fog to the scene.
		 *
		 * ?? Obsolete ??
		 */
		 // ****************************************
		class Fog : public Drawable
		{
		public:
			DECLARE_ABSTRACT_CLASS_INFO(Fog, Drawable, Renderer)

				/**
				 * \brief	constructor
				 * \fn 		Fog(const std::string& name,DECLARE_CLASS_NAME_TREE_ARG);
				 * \param	name : instance name
				 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
				 */
				Fog(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);

		protected:
			/**
			 * \brief	destructor
			 * \fn 		~Fog();
			 */
			virtual ~Fog() { ; }

			//! start distance
			float		mStartDistance = 1.0f;
			//! end distance
			float		mEndDistance = 100.0f;
			//! fog density
			float		mDensity = 1.0f;
			//! fog color
			v4f			mColor = { 0.1f, 0.1f, 0.1f, 1.0f };

			WRAP_ATTRIBUTES(mStartDistance, mEndDistance, mDensity, mColor);

			//! rendering mode
			maEnum<3>	mMode;
		};
	}
}
