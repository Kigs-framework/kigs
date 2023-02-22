#pragma once

#include "Drawable.h"
#include "TecLibs/Tec3D.h"

namespace Kigs
{

	namespace Draw
	{
		// ****************************************
		// * TextureMatrix class
		// * --------------------------------------
		/**
		* \file	TextureMatrix.h
		* \class	TextureMatrix
		* \ingroup Renderer
		* \brief Texture coordinate ( U V W ) transform matrix
		*
		*/
		// ****************************************
		class TextureMatrix : public Drawable
		{
		public:
			DECLARE_ABSTRACT_CLASS_INFO(TextureMatrix, Drawable, Renderer)

				/**
				 * \brief	constructor
				 * \fn 		TextureMatrix(const std::string& name,DECLARE_CLASS_NAME_TREE_ARG);
				 * \param	name : instance name
				 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
				 */
				TextureMatrix(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);

			/**
			 * \brief		initialize with a 3x4 matrix
			 * \fn			void	Init(const Matrix3x4& matrix);
			 * \param		matrix : used matrix
			 */
			void	Init(const Matrix3x4& matrix);

			/**
			 * \brief		initialize with a 4x4 matrix
			 * \fn			void	Init(const Matrix4x4& matrix);
			 * \param		matrix : used matrix
			 */
			void	Init(const Matrix4x4& matrix);

			/**
			 * \brief		initialize with the identity matrix
			 * \fn			void	InitToIdentity();
			 */
			void	InitToIdentity();

		protected:
			/**
			 * \brief	destructor
			 * \fn 		~TextureMatrix();
			 */
			virtual ~TextureMatrix();

			//! link to the used matrix
			Matrix4x4		mMatrix;
			WRAP_ATTRIBUTES(mMatrix);
		};

	}
}
