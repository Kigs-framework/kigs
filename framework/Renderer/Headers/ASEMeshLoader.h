#ifndef _ASEMESHLOADER_H_
#define _ASEMESHLOADER_H_


#include "KMesh.h"
#include "CoreRawBuffer.h"
#include "AsciiParserUtils.h"


// ****************************************
// * ASEMeshLoader class
// * --------------------------------------
/**
 * \file	ASEMeshLoader.h
 * \class	ASEMeshLoader
 * \ingroup Renderer
 * \brief	Load mesh in ASE format.
 * 
 * Obsolete
 */
 // ****************************************
class ASEMeshLoader 
{
public:
	ASEMeshLoader();
	~ASEMeshLoader();
	int ImportFile(Mesh *pMesh, const std::string &FileName);
	int ReadFile(Mesh *pMesh);
private:
 
  struct BaseMaterialStruct
  {
    float ambient[3];
    float specular[3];
    float diffuse[3];
    float shininess;
  };

  struct materialStruct : public BaseMaterialStruct
  {
    std::vector<BaseMaterialStruct> subMat;
  };

  std::vector<materialStruct> mMaterialStructList;

	std::vector<std::string>	mTextureList;
	std::vector<std::string>	mMaterialList;

	char *mTextureName;
	int mMaterialCount;
	int *mMatIdArray;	
	SmartPointer<CoreRawBuffer>			mData;
	AsciiParserUtils					mFileParser;
};

#endif // _ASEMESHLOADER_H_
