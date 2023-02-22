#pragma once

#include "Drawable.h"
#include "Texture.h"

namespace Kigs
{

	namespace Draw
	{

		// ****************************************
		// * SkyBox class
		// * --------------------------------------
		/**
		* \file	SkyBox.h
		* \class	SkyBox
		* \ingroup Renderer
		* \brief Draw a skybox with a cube texture.
		*
		* ?? obsolete ?? or to be updated
		*
		*/
		// ****************************************
		class SkyBox : public Drawable
		{
		public:
			DECLARE_ABSTRACT_CLASS_INFO(SkyBox, Drawable, Renderer)

				/**
				 * \brief	constructor
				 * \fn 		SkyBox(const std::string& name,DECLARE_CLASS_NAME_TREE_ARG);
				 * \param	name : instance name
				 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
				 */
				SkyBox(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);

			/**
			 * \brief	destructor
			 * \fn 		~SkyBox();
			 */
			virtual ~SkyBox();

			/**
			 * \brief	retreive the bounding box of the bitmap (point min and point max)
			 * \fn 		virtual void	GetBoundingBox(Point3D& pmin,Point3D& pmax) const {pmin=myBBoxMin; pmax=mBBoxMax;}
			 * \param	pmin : point min of the bounding box (in/out param)
			 * \param	pmax : point max of the bounding box (in/out param)
			 */
			void	GetNodeBoundingBox(Point3D& pmin, Point3D& pmax) const  override { pmin = mBBoxMin; pmax = mBBoxMax; }


			/**
			* \brief	set the size of the SkyBox
			* \fn 		void SetSize(const float& Size)
			* \param	Size : size of the SkyBox
			*/
			void SetSize(const float& Size) { setValue("Size",Size); }

			/**
			 * \brief	update the bounding box
			 * \fn 		virtual bool	BBoxUpdate(double)
			 * \param	double : world time
			 * \return	TRUE because has a bounding box
			 */
			bool	BBoxUpdate(double/* time */) override { return true; }

		protected:
			/**
			 * \brief	initialize modifiable
			 * \fn 		virtual void InitModifiable();
			 */
			void InitModifiable() override;


			//! point min of the bounding box
			Point3D		mBBoxMin;
			//! point max of the bounding box
			Point3D		mBBoxMax;

			//! size of the SkyBox
			float			mSize = 5000.0f;
			//! name of the file
			std::string		mFileName = "SkyBox.tga";

			WRAP_ATTRIBUTES(mSize, mFileName);
			//! used texture
			SP<Texture>	mTexture;

		};

	}
}
