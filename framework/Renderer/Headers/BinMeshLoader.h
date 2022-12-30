#pragma once

#include <stdio.h>
#include "BufferedFile.h"
#include "BinMeshLoaderBase.h"

namespace Kigs
{
	namespace Draw
	{
		class Mesh;
		class ModernMesh;
	

		// ****************************************
		// * BinMeshLoader class
		// * --------------------------------------
		/**
		 * \file	BinMeshLoader.h
		 * \class	BinMeshLoader
		 * \ingroup Renderer
		 * \brief	Import mesh from a binary file
		 */
		 // ****************************************
		class BinMeshLoader : public Utils::BinMeshLoaderBase
		{
		public:

			/**
			 * \brief	constructor
			 * \fn 		BinMeshLoader();
			 */
			BinMeshLoader();

			/**
			 * \brief	destructor
			 * \fn 		~BinMeshLoader();
			 */
			virtual ~BinMeshLoader();

			/**
			 * \brief	import a mesh from a file
			 * \fn 		int ImportFile(Mesh *pMesh, const std::string &FileName);
			 * \param 	pMesh : link to the mesh imported
			 * \param 	FileName : name of the file to read
			 * \return 	an error code : <ul><li>0 : no error</li><li>1 : param error (a parameter is NULL)</li><li>3 : no file</li><li>10+ : reading error (+10)</li></ul>
			 */
			int ImportFile(Mesh* pMesh, const std::string& FileName);
			int ImportFile(ModernMesh* pMesh, const std::string& FileName);

			/**
			 * \brief	read a file
			 * \fn 		int ReadFile(Mesh *pMesh);
			 * \param 	pMesh : link to the mesh imported
			 * \return 	an error code : <ul><li>0 : no error</li></ul> only 0 is implemented
			 */
			int ReadFile(Mesh* pMesh);
			int ReadFile(ModernMesh* pMesh);
#ifdef WIN32
			int ExportFile(Mesh* pMesh, std::string _directoryName, std::string _FileName);
#endif

		protected:

			class grpStruct
			{
			public:
				grpStruct() : mMaterial(nullptr), mTriangleCount(0), mTriangleSize(0), mFirstTriangle(0)
				{
				}

				SP<Material>		mMaterial;
				int				mTriangleCount;
				int				mTriangleSize;
				void* mFirstTriangle;
				unsigned int	mTriangleType;
			};

		};

	}
}