#ifndef _BINMESHLOADERBASE_H_
#define _BINMESHLOADERBASE_H_


#include <stdio.h>
#include "BufferedFile.h"


// ****************************************
// * BinMeshLoaderBase class
// * --------------------------------------
/**
 * \file	BinMeshLoaderBase.h
 * \class	BinMeshLoaderBase
 * \ingroup StandAlone
 * \brief	Utility class to load mesh files.
 *
 */
 // ****************************************

class BinMeshLoaderBase
{
public:

	struct triangleInfo
	{
		unsigned int tabTriangle [3];
		unsigned int* suppIndex;
		unsigned int* T_type[3];
		bool isT_Type;
		int suppSize;
	};

	/**
	 * \brief	constructor
	 * \fn 		BinMeshLoader();
	 */
	BinMeshLoaderBase();

	/**
	 * \brief	destructor
	 * \fn 		~BinMeshLoader();
	 */
	virtual ~BinMeshLoaderBase();

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


	class BinFileStructDesc
	{
	public:
	};

	class BinFileStruct
	{
	public:
		//! size of the struct
		unsigned int	structSize;
		//! size of the BinFileStructDesc (not used)
		unsigned int	DescStructSize;

		/**
		 * \brief	read and fill a struct from the file
		 * \fn 		static void	ReadStruct(BinFileStruct* result, BufferedFile* file, BinFileStructDesc* desc=0);
		 * \param 	result : Structure to fill, NOT NULL
		 * \param 	file : file to read, NOT NULL
		 * \param 	desc : link to the BinFileStructDesc
		 * \todo	use of BinFileStructDesc
		 */
		static void	ReadStruct(BinFileStruct* result, BufferedFile* file, BinFileStructDesc* desc=0);
		static void	WriteStruct(BinFileStruct* result, FILE* file, BinFileStructDesc* desc=0);
	};

	// ****************************************
	// * Header class
	// * --------------------------------------
	/**
	 * \class	Header
	 * \brief	contain the header of the binMesh file
	 */
	// ****************************************
	class Header : public BinFileStruct
	{
	public:
		
		// give the group count
		unsigned int	GetGroupCount()
		{
			return (mGroupCount&0xFFFFFF);
		}

		// give the version of the file
		unsigned int	GetFileVersion()
		{
			return ((mGroupCount &0xFF000000)>>24);
		}

		//! higher 8 bits for the file format version
		//! then 24 bits for the grounp count
		//! number of group in the binMesh
		unsigned int	mGroupCount;
	};

	// ****************************************
	// * GroupDesc class
	// * --------------------------------------
	/**
	 * \class	GroupDesc
	 * \brief	contain the group description of the binMesh file
	 */
	// ****************************************
	class GroupDesc : public BinFileStruct
	{
	public:
		//! type of triangle used in the group
		unsigned int	mTriangleType;
	};

	// ****************************************
	// * StageDesc class
	// * --------------------------------------
	/**
	 * \class	StageDesc
	 * \brief	contain the stage description of the binMesh file
	 */
	// ****************************************
	class StageDesc : public BinFileStruct
	{
	public:
		//! texture name
		std::string	mTexture;
		//! stage index
		int				mStageIndex;
		//! environment of the texture
		TexEnvType		mTexEnv;
	};

	// ****************************************
	// * MaterialDesc class
	// * --------------------------------------
	/**
	 * \class	MaterialDesc
	 * \brief	contain the material description of the binMesh file
	 */
	// ****************************************
	class MaterialDesc : public BinFileStruct
	{
	public:
		//! link to the Blend Function Source 
		BlendFuncSource	mBlendFuncSource;
		//! link to the Blend Function Destination
		BlendFuncDest	mBlendFuncDest;
		//! TRUE if blending is enabled
		char			mBlendEnabled;
		//! TRUE if material color is enabled
		char			mMaterialColorEnabled;

		char			mUseless_One;
		char			mUseless_Two;

		//! number of stage
		int				stageCount;
		//! ambient color (float[4])
		float			ambient[4];
		//! diffuse color (float[4])
		float			diffuse[4];
		//! specular color (float[4])
		float			specular[4];
		//! emissive color (float[4])
		float			emissive[4];
		//! alpha value
		float			alpha;
		//! shininess value
		float			shininess;
		//! triangle number
		unsigned int	triangleCount;
		//! link to the StageDesc structure
		StageDesc*		stages;
	};

	// ****************************************
	// * OtherDataStruct class
	// * --------------------------------------
	/**
	 * \class	OtherDataStruct
	 * \ingroup InternalBinMeshLoader
	 * \brief	contain other data information of the binMesh file
	 */
	// ****************************************
	class OtherDataStruct : public BinFileStruct
	{
	public:
		//! coordinate size
		unsigned int	coordSize;
		//! number of vertex coordinate
		unsigned int	vertexCoordCount;
		//! number of normal coordinate
		unsigned int	normalCoordCount;
		//! colorComp Size
		unsigned int	colorCompSize;
		//! number of colorComp
		unsigned int	colorCompCount;
		//! uvComp Size
		unsigned int	uvCompSize;
		//! number of uvComp
		unsigned int	uvCompCount;
	};

protected:
	/**
	 * \brief	read a szString in myFile from the current read position
	 * \fn 		std::string	ReadString(BufferedFile*);
	 * \param 	BufferedFile : not used (read from the member BufferedFile)
	 * \return 	the next szString in myFile, "" if no string
	 */
	std::string	ReadString(BufferedFile*);
	
	/**
	 * \brief	read a kFloat array in a buffered file from the current read position
	 * \fn 		void ReadFloatArray(float* dst,unsigned int count,BufferedFile* file);
	 * \param 	dst : array of kFloat readed (in/out param)
	 * \param 	count : number of kFloat readed
	 * \param 	file : link to the file to read
	 */
	void			ReadFloatArray(float* dst,unsigned int count,BufferedFile* file);

	//! link to the readed file which contain the binMesh
	BufferedFile*	mFile;
};

#endif // _BINMESHLOADERBASE_H_