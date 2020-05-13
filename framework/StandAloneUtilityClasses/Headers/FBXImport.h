#ifndef __FBX_IMPORT_H__
#define __FBX_IMPORT_H__

#ifdef USE_FBXSDK

#include "CoreModifiable.h"
#include "Core.h"
#include "Material.h"
#include "CoreModifiableAttribute.h"
#include "Node3D.h"
#include "Scene3D.h"
#include "ModernMesh.h"
#include <vector>
#include <fbxsdk.h>
#include <map>
#include "Base3DImporter.h"

// ****************************************
// * FBXImport class
// * --------------------------------------
/**
* \file	FBXImport.h
* \class	FBXImport
* \ingroup StandAloneUtility
* \brief	 Class used to parse fbx files (.fbx)
* \author	König Jolan
* \version ukn
* \date	10/05/2016
* Class used to parse fbx Files. Need the fbx Sdk 2015 (Autodesk)
*/
// ****************************************

class FBXImport : public Base3DImporter
{
public:
	DECLARE_CLASS_INFO(FBXImport, Base3DImporter, Renderer);

	//! builds an list of kmesh
	FBXImport(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	//Entire scene, with lights and everything
	CMSP getScene(){return myScene;}

	SP<Node3D>	getRootNode()
	{
		return RootNode;
	}
	virtual ~FBXImport();

protected:

	//functions CoreModifiable
	virtual void	InitModifiable();

	//Parsers
	CMSP ParseNode(FbxNode* fatherNode);
	CMSP ParseAttribute(FbxNodeAttribute* attribute, FbxNode* fatherNode, int &numMesh);
	CMSP ParseMesh(FbxMesh* attribute, FbxNode* myNode, int &numMesh);
	CMSP ParseCamera(FbxCamera* fbxcamera);
	CMSP ParseLight(FbxLight* fbxlight);
	void ParseMaterial(FbxSurfaceMaterial* material);
	FbxDouble3 ParseMaterialProperty(const FbxSurfaceMaterial * pMaterial, const char * pPropertyName, const char * pFactorPropertyName, kstl::string & pTextureName);
	void ImportTexture(Material* material, kstl::string textureName, int texture_channel=0);


	//Exporter
	void ExportResult();

	//Tools
	void	RetreiveShortNameAndExt(const kstl::string& filename, kstl::string& shortname, kstl::string& fileext, kstl::string& filepath);

	bool	hasNormalMap(Material*);

	//Objects to stock informations
	class IndexTrio
	{
	public:
		IndexTrio()
		{}
		FbxVector4 posVertex;
		FbxVector4 posNormal;
		FbxColor color;
		FbxVector2 posTexture;
	};
	kstl::map<FbxMesh*, CoreModifiable*> meshParsed;
	std::map<CoreModifiable*,Material*> materialOfMesh;
	std::multimap<Material*, ModernMesh*> sortedMeshes;

	//parameters
	maString								m_FileName;
	maString								m_ExportPath;

	//Objects Created
	SP<Scene3D> myScene;

	kstl::list<CMSP>			m_MeshList;
	kstl::list<kstl::string> m_TextureList;

	struct MaterialStruct
	{
		SP<Material>	kigsMaterial;
		bool		hasNormalMap;
	};

	kstl::map<kstl::string, MaterialStruct>	m_materialList;
	SP<Node3D>			RootNode;

	//statistique
	int nbrUniqueMesh;
	int nbrInstanceMesh;
	int nbrVertex;
	int nbrTriangle;
};
#endif // USE_FBXSDK
#endif //__FBX_IMPORT_H__
