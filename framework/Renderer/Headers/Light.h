#pragma once

#include "Node3D.h"
#include "maReference.h"
#include "TecLibs/Tec3D.h"

namespace Kigs
{

	namespace Draw
	{

		// ****************************************
		// * Light class
		// * --------------------------------------
		/**
		* \file	Light.h
		* \class	Light
		* \ingroup Renderer
		* \brief	Base class, generic light object.
		*
		*/
		// ****************************************
		class Light : public Node3D
		{
		public:
			DECLARE_ABSTRACT_CLASS_INFO(Light, Node3D, Renderer)

				/**
				 * \brief	constructor
				 * \fn 		Light(const std::string& name,DECLARE_CLASS_NAME_TREE_ARG);
				 * \param	name : instance name
				 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
				 */
				Light(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);

			/**
			 * \brief	set the diffuse color
			 * \fn 		void	SetDiffuseColor(float r,float g,float b,float a)
			 * \param	r : red color
			 * \param	g : green color
			 * \param	b : blue color
			 * \param	a : alpha value
			 */
			void	SetDiffuseColor(float r, float g, float b)
			{
				mDiffuseColor[0] = r;
				mDiffuseColor[1] = g;
				mDiffuseColor[2] = b;
				NotifyUpdate(KigsID("DiffuseColor")._id);
			}

			/**
			 * \brief	set the specular color
			 * \fn 		void	SetSpecularColor(float r,float g,float b,float a)
			 * \param	r : red color
			 * \param	g : green color
			 * \param	b : blue color
			 * \param	a : alpha value
			 */
			void	SetSpecularColor(float r, float g, float b)
			{
				mSpecularColor[0] = r;
				mSpecularColor[1] = g;
				mSpecularColor[2] = b;
				NotifyUpdate(KigsID("SpecularColor")._id);
			}

			/**
			 * \brief	set the ambient color
			 * \fn 		void	SetAmbientColor(float r,float g,float b,float a)
			 * \param	r : red color
			 * \param	g : green color
			 * \param	b : blue color
			 * \param	a : alpha value
			 */
			void	SetAmbientColor(float r, float g, float b)
			{
				mAmbientColor[0] = r;
				mAmbientColor[1] = g;
				mAmbientColor[2] = b;
				NotifyUpdate(KigsID("AmbientColor")._id);
			}

			inline void setIsOn(bool a_value) { mIsOn = a_value; }
			inline bool getIsOn() const { return mIsOn; }

		protected:

			void InitModifiable() override;

			/**
			 * \brief	destructor
			 * \fn 		~Light();
			 */
			virtual ~Light();

			//! TRUE if the light is on
			bool			mIsOn = true;

			//! spot attenuation
			float			mSpotAttenuation = 0.0f;
			//! spot cut off
			float			mSpotCutOff = 1.0f;
			//! attenuation constante
			float			mConstAttenuation = 1.0f;
			//! attenuation linear
			float			mLinAttenuation = 0.01f;
			//! attenuation quad
			float			mQuadAttenuation = 0.0001f;

			//! specular color
			v3f				mSpecularColor = { 1.0f ,1.0f ,1.0f };
			//! ambient color
			v3f				mAmbientColor = { 0.0f ,0.0f ,0.0f };
			//! diffuse color
			v3f				mDiffuseColor = { 1.0f ,1.0f ,1.0f };

			WRAP_ATTRIBUTES(mIsOn, mSpotAttenuation, mSpotCutOff, mConstAttenuation, mLinAttenuation, mQuadAttenuation, mSpecularColor, mAmbientColor, mDiffuseColor);

			//0 for point, 1 for directional, 2 for spot
			maEnumInit<3>	mLightType;

		};

	}
}
