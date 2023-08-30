#pragma once


#include <stdio.h>
#include "BufferedFile.h"

namespace Kigs
{

	namespace Draw
	{
		class Mesh;

		// ****************************************
		// * STLMeshLoader class
		// * --------------------------------------
		/**
		* \file	STLMeshLoader.h
		* \class	STLMeshLoader
		* \ingroup Renderer
		* \brief Load an STL file and return an old
		*
		*/
		// ****************************************
		class STLMeshLoader
		{
		public:
			/**
			 * \brief	constructor
			 * \fn 		STLMeshLoader();
			 */
			STLMeshLoader();

			/**
			 * \brief	destructor
			 * \fn 		~STLMeshLoader();
			 */
			~STLMeshLoader();

			/**
			 * \brief	import a mesh from a file
			 * \fn 		int ImportFile(Mesh *pMesh, const std::string &FileName);
			 * \param 	pMesh : link to the mesh imported
			 * \param 	FileName : name of the file to read
			 * \return 	an error code : <ul><li>0 : no error</li><li>1 : param error (a parameter is NULL)</li><li>3 : no file</li><li>10+ : reading error (+10)</li></ul>
			 */
			int ImportFile(Mesh* pMesh, const std::string& FileName);

			/**
			 * \brief	read a file
			 * \fn 		int ReadFile(Mesh *pMesh);
			 * \param 	pMesh : link to the mesh imported
			 * \return 	an error code : <ul><li>0 : no error</li></ul> only 0 is implemented
			 */
			int ReadFile(Mesh* pMesh);

			//! type of environment
			enum TexEnvType
			{
				TEX_ENV_MODULATE,
				TEX_ENV_DECAL,
				TEX_ENV_BLEND,
				TEX_ENV_REPLACE
			};

			//! type of blend source
			enum BlendFuncSource
			{
				S_ZERO,
				S_ONE,
				S_DST_COLOR,
				S_ONE_MINUS_DST_COLOR,
				S_SRC_ALPHA,
				S_ONE_MINUS_SRC_ALPHA,
				S_DST_ALPHA,
				S_ONE_MINUS_DST_ALPHA,
				S_SRC_ALPHA_SATURATE
			};

			//! type of blend destination
			enum BlendFuncDest
			{
				D_ZERO,
				D_ONE,
				D_SRC_COLOR,
				D_ONE_MINUS_SRC_COLOR,
				D_SRC_ALPHA,
				D_ONE_MINUS_SRC_ALPHA,
				D_DST_ALPHA,
				D_ONE_MINUS_DST_ALPHA
			};

		protected:

			struct readFacet
			{
				v3f	mNormal;
				v3f		mVertex[3];
			};

			//! link to the readed file which contain the binMesh
			File::BufferedFile* mFile;
		};

	}
}