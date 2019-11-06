#ifndef _ASEMESHLOADER_H_
#define _ASEMESHLOADER_H_


#include "KMesh.h"
#include "CoreRawBuffer.h"
#include "AsciiParserUtils.h"

class ASEMeshLoader 
{
public:
	ASEMeshLoader();
	~ASEMeshLoader();
	int ImportFile(Mesh *pMesh, const kstl::string &FileName);
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
    kstl::vector<BaseMaterialStruct> subMat;
  };

  kstl::vector<materialStruct> myMaterialStructList;

	kstl::vector<kstl::string>	myTextureList;
	kstl::vector<kstl::string>	myMaterialList;

	char *TextureName;
	int MaterialCount;
	int *MatIdArray;	
	SmartPointer<CoreRawBuffer>			m_data;
	AsciiParserUtils*		m_FileParser;
};

#endif // _ASEMESHLOADER_H_
