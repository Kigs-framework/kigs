#ifndef _BINMESHLOADER_H_
#define _BINMESHLOADER_H_

/*! 
 * \defgroup InternalBinMeshLoader Internal binMeshLoader
 * \ingroup Renderer
 * structure of the binMeshFile
*/

#include <stdio.h>
#include "BufferedFile.h"
#include "BinMeshLoaderBase.h"
class Mesh;
class ModernMesh;

// ****************************************
// * BinMeshLoader class
// * --------------------------------------
/**
 * \file	BinMeshLoader.h
 * \class	BinMeshLoader
 * \ingroup Loader
 * \ingroup Renderer
 * \brief	load mesh from binaries files
 * \author	ukn
 * \version ukn
 * \date	ukn
 */
// ****************************************
class BinMeshLoader : public BinMeshLoaderBase
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
	 * \fn 		int ImportFile(Mesh *pMesh, const kstl::string &FileName);
	 * \param 	pMesh : link to the mesh imported
	 * \param 	FileName : name of the file to read
	 * \return 	an error code : <ul><li>0 : no error</li><li>1 : param error (a parameter is NULL)</li><li>3 : no file</li><li>10+ : reading error (+10)</li></ul>
	 */
	int ImportFile(Mesh *pMesh, const kstl::string &FileName);
	int ImportFile(ModernMesh *pMesh, const kstl::string &FileName);

	/**
	 * \brief	read a file
	 * \fn 		int ReadFile(Mesh *pMesh);
	 * \param 	pMesh : link to the mesh imported
	 * \return 	an error code : <ul><li>0 : no error</li></ul> only 0 is implemented
	 */
	int ReadFile(Mesh *pMesh);
	int ReadFile(ModernMesh *pMesh);
#ifdef WIN32
	int ExportFile(Mesh *pMesh, kstl::string _directoryName, kstl::string _FileName);
#endif

protected:

	class grpStruct
	{
	public:
		grpStruct() : myMaterial(0),myTriangleCount(0),myTriangleSize(0),myFirstTriangle(0)
		{
		}

		Material*		myMaterial;
		int				myTriangleCount;
		int				myTriangleSize;
		void*			myFirstTriangle;
		unsigned int	myTriangleType;
	};

};

#endif // _BINMESHLOADER_H_