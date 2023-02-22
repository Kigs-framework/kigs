#pragma once

#include "Drawable.h"
#include "TextureHandler.h"

namespace Kigs
{

	namespace Draw
	{
		//! default displacement
#define KIGS_SPRITE_DEFAULT_DISPLACEMENT (0.002f)

		class Texture;

		// ****************************************
		// * Sprite class
		// * --------------------------------------
		/**
		* \file	Sprite.h
		* \class	Sprite
		* \ingroup Renderer
		* \brief Draw a sprite in the 3D scene.
		*
		* ?? obsolete ?? can be replaced by Billboard 3D or AutoOrientedNode3D or UINode3DLayer ?
		*
		*/
		// ****************************************
		class Sprite : public Drawable
		{
		public:
			DECLARE_ABSTRACT_CLASS_INFO(Sprite, Drawable, Renderer)

				/**
				 * \brief	constructor
				 * \fn 		Sprite(const std::string& name,DECLARE_CLASS_NAME_TREE_ARG);
				 * \param	name : instance name
				 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
				 */
				Sprite(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);

			/**
			 * \brief	set the position
			 * \fn 		virtual void setPosition(float x, float y)
			 * \param	x : position on x axis
			 * \param	y : position on y axis
			 */
			virtual void setPosition(float x, float y) { mPosition[0] = x; mPosition[1] = y; }

			/**
			 * \brief	set the texture
			 * \fn 		virtual void setTexture(const float &u1,const float &v1,const float &u2,const float &v2)
			 * \param	u1 :
			 * \param	v1 :
			 * \param	u2 :
			 * \param	v2 :
			 */
			virtual void setTexture(const float& u1, const float& v1, const float& u2, const float& v2) { mTexUV[0] = u1; mTexUV[2] = u2; mTexUV[1] = v1;; mTexUV[3] = v2; }

			/**
			 * \brief	set the size
			 * \fn 		virtual void setSize(const float &s)
			 * \param	s : new size
			 */
			virtual void setSize(const float& s) { mSize[0] = mSize[1] = s; }

			/**
			 * \brief	set the size
			 * \fn 		virtual void setSize(const float &sx, const float &sy)
			 * \param	sx : size on x axis
			 * \param	sy : size on y axis
			 */
			virtual void setSize(const float& sx, const float& sy) { mSize[0] = sx; mSize[1] = sy; }

			/**
			 * \brief	get the size
			 * \fn 		virtual void getSize(float &sx, float &sy)
			 * \param	sx : size on x axis (in/out param)
			 * \param	sy : size on y axis (in/out param)
			 */
			virtual void getSize(float& sx, float& sy) { sx = mSize[0]; sy = mSize[1]; }


		protected:
			/**
			 * \brief	destructor
			 * \fn 		~Sprite();
			 */
			virtual	~Sprite();


			/**
			 * \brief	retreive the bounding box of the bitmap (point min and point max)
			 * \fn 		virtual void	GetBoundingBox(Point3D& pmin,Point3D& pmax) const {pmin=myBBoxMin; pmax=mBBoxMax;}
			 * \param	pmin : point min of the bounding box (in/out param)
			 * \param	pmax : point max of the bounding box (in/out param)
			 */
			void	GetNodeBoundingBox(Point3D& pmin, Point3D& pmax) const override { pmin.Set((float)mPosition[0], 0.0f, (float)mPosition[1]); pmax.Set((float)mPosition[0] + (float)mSize[0], (float)mDisplacement, (float)mPosition[1] + (float)mSize[1]); }

			//! displacement
			float			mDisplacement = KIGS_SPRITE_DEFAULT_DISPLACEMENT;
			//! position
			v2f				mPosition = { -0.5f, -0.5f };
			//! size 
			v2f				mSize = { 1.5f, 1.5f };
			//! coord texture u1,v1,u2,v2
			v4f				mTexUV = { 0.0f,0.0f,1.0f,1.0f };
			//! color
			v4f				mColor = { 1.0f, 1.0f, 1.0f, 1.0f };

			WRAP_ATTRIBUTES(mDisplacement, mPosition, mSize, mTexUV, mColor);

			//! link to the texture
			//SP<Texture>	mTexture;
			INSERT_FORWARDSP(TextureHandler, mTexturePointer);
		};

	}
}
