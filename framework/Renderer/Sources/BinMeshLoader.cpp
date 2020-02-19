#include "PrecompiledHeaders.h"

#include "Material.h"
#include "Texture.h"
#include "MaterialStage.h"
#include "Core.h"
#include "BinMeshLoader.h"
#include "KMesh.h"
#include "ModernMesh.h"
#include "TextureFileManager.h"
#include "SceneGraphDefines.h"
#include "CoreVector.h"
#include "FilePathManager.h"

BinMeshLoader::BinMeshLoader() : BinMeshLoaderBase()
{
}

BinMeshLoader::~BinMeshLoader()
{
}

int BinMeshLoader::ImportFile(Mesh *pMesh, const kstl::string &FileName)
{
	int Error;

	if(!pMesh || !FileName.length())
	{
		KIGS_ERROR("No mesh filename or mesh pointer",1);
		return 1;
	}

	pMesh->setValue(LABEL_TO_ID(FileName),FileName);

	myFile=BufferedFile::Open(FileName.c_str());
	if(myFile)
	{
		pMesh->EmptyItemList();
		Error = ReadFile(pMesh);

		if (Error)
		{
			delete(myFile);
			KIGS_ERROR("Error reading mesh file",1);
			return 10+Error;
		}

		delete(myFile);
		return 0;
	}

	KIGS_ERROR("Mesh file can't be open",1);
	return 3;
}

int BinMeshLoader::ImportFile(ModernMesh *pMesh, const kstl::string &FileName)
{
	int Error;

	if(!pMesh || !FileName.length())
	{
		KIGS_ERROR("No mesh filename or mesh pointer",1);
		return 1;
	}

	myFile=BufferedFile::Open(FileName.c_str());
	if(myFile)
	{
		pMesh->EmptyItemList();
		Error = ReadFile(pMesh);

		if (Error)
		{
			delete(myFile);
			KIGS_ERROR("Error reading mesh file",1);
			return 10+Error;
		}

		delete(myFile);
		return 0;
	}

	KIGS_ERROR("Mesh file can't be open",1);
	return 3;
}


int BinMeshLoader::ReadFile(Mesh *pMesh)
{
	Header	header;

	BinFileStruct::ReadStruct(&header,myFile);

	unsigned int fileVersion=header.GetFileVersion();
	int nbGroup=(int)header.GetGroupCount();

	int i,j;
	for(i=0;i<nbGroup;i++)
	{
		GroupDesc grpdesc;
		BinFileStruct::ReadStruct(&grpdesc,myFile);

		// convert old enum to new ones
		if(fileVersion<1)
		{
			switch(grpdesc.triangleType)
			{
			case 1:
				grpdesc.triangleType=eF_Triangle;
				break;
			case 2:
				grpdesc.triangleType=eS_Triangle;
				break;
			case 3:
				grpdesc.triangleType=eFC_Triangle;
				break;
			case 4:
				grpdesc.triangleType=eSC_Triangle;
				break;
			case 5:
				grpdesc.triangleType=eFG_Triangle;
				break;
			case 6:
				grpdesc.triangleType=eSG_Triangle;
				break;
			case 7:
				grpdesc.triangleType=eTF_Triangle;
				break;
			case 8:
				grpdesc.triangleType=eTS_Triangle;
				break;
			case 9:
				grpdesc.triangleType=eTFC_Triangle;
				break;
			case 10:
				grpdesc.triangleType=eTSC_Triangle;
				break;
			case 11:
				grpdesc.triangleType=eTFG_Triangle;
				break;
			case 12:
				grpdesc.triangleType=eTSG_Triangle;
				break;
			}
		}

		// load material
		MaterialDesc currentmat;

		BinFileStruct::ReadStruct(&currentmat,myFile);

		char	index[16];
		sprintf(index,"%d",i);
		kstl::string objname=pMesh->getName();
		objname+="MeshItemGroup";
		objname+=index;

		SP<MeshItemGroup> newgroup=KigsCore::GetInstanceOf(objname,"MeshItemGroup");
		newgroup->Init();
		newgroup->myTriangleType=(TriangleType)grpdesc.triangleType;

		objname=pMesh->getName();
		objname+="Material";
		objname+=index;

		SP<Material> newMaterial=KigsCore::GetInstanceOf(objname,"Material");
		newMaterial->setValue(LABEL_TO_ID(BlendFuncSource),currentmat.myBlendFuncSource);
		newMaterial->setValue(LABEL_TO_ID(BlendFuncDest),currentmat.myBlendFuncDest);
		newMaterial->setValue(LABEL_TO_ID(BlendEnabled),currentmat.myBlendEnabled);

		newMaterial->setValue(LABEL_TO_ID(MaterialColorEnabled),currentmat.myMaterialColorEnabled);
		if(currentmat.myBlendEnabled)
		{
			newMaterial->setValue("TransarencyFlag", true);
			pMesh->setValue("TransarencyFlag", true);
		}

		if(currentmat.shininess>128.0f)
		{
			currentmat.shininess=128.0f;
		}

		newMaterial->SetAmbientColor((kfloat)currentmat.ambient[0],(kfloat)currentmat.ambient[1],(kfloat)currentmat.ambient[2]);
		newMaterial->SetSpecularColor((kfloat)currentmat.specular[0],(kfloat)currentmat.specular[1],(kfloat)currentmat.specular[2]);
		newMaterial->SetEmissionColor((kfloat)currentmat.emissive[0],(kfloat)currentmat.emissive[1],(kfloat)currentmat.emissive[2]);
		newMaterial->SetDiffuseColor((kfloat)currentmat.diffuse[0],(kfloat)currentmat.diffuse[1],(kfloat)currentmat.diffuse[2],(kfloat)currentmat.alpha);
		newMaterial->setValue(LABEL_TO_ID(Shininess),(kfloat)currentmat.shininess);
		newMaterial->setValue(LABEL_TO_ID(Transparency),(kfloat)currentmat.alpha);

		if((kfloat)currentmat.alpha != KFLOAT_CONST(1.0))
		{
			pMesh->setValue("TransarencyFlag", true);
		}

		newgroup->myTriangleCount=(int)currentmat.triangleCount;

		currentmat.stages = new StageDesc[currentmat.stageCount];
		for(j=0;j<currentmat.stageCount;j++)
		{
			StageDesc& stagedesc= currentmat.stages[j];

			fread(&(stagedesc.structSize),sizeof(stagedesc.structSize),1,myFile);
			fread(&(stagedesc.DescStructSize),sizeof(stagedesc.DescStructSize),1,myFile);

			stagedesc.myTexture=ReadString(myFile);

			fread(&(stagedesc.myStageIndex),sizeof(int),1,myFile);
			fread(&(stagedesc.myTexEnv),sizeof(TexEnvType),1,myFile);

#ifndef NO_MULTISTAGE_RENDERING
#ifndef DO_MULTISTAGE_RENDERING
#error	"missing ScenegraphDefines.h"
#endif
			sprintf(index,"%d",j);
			objname=newMaterial->getName();
			objname+="MaterialStage";
			objname+=index;

			SP<MaterialStage> MatStage = (KigsCore::GetInstanceOf(objname,"MaterialStage"));

			MatStage->setValue(LABEL_TO_ID(StageIndex),stagedesc.myStageIndex);
			MatStage->setValue(LABEL_TO_ID(TexEnv),stagedesc.myTexEnv);

			if(stagedesc.myTexture != "empty")
			{
				auto& texfileManager = KigsCore::Singleton<TextureFileManager>();
				SP<Texture> Tex = texfileManager->GetTexture(stagedesc.myTexture, false);
				Tex->setValue(LABEL_TO_ID(ForcePow2),true);
				Tex->Init();

				MatStage->addItem((CMSP&)Tex);
				
			}

			MatStage->Init();

			newMaterial->addItem((CMSP&)MatStage);
		
#else //NO_MULTISTAGE_RENDERING
			// if no material stages, add texture to material directly
			if(stagedesc.myTexture != "empty")
			{
				TextureFileManager*	fileManager=(TextureFileManager*)KigsCore::GetSingleton("TextureFileManager");
				Texture* Tex=fileManager->GetTexture(stagedesc.myTexture,false);
				Tex->setValue(LABEL_TO_ID(ForcePow2),true);
				Tex->Init();

				newMaterial->addItem(Tex);
				Tex->Destroy();
			}

#endif // NO_MULTISTAGE_RENDERING
		}

		bool	sharedMaterial=true;

		pMesh->getValue(LABEL_TO_ID(ShareMaterial),sharedMaterial);

		// check existing materials
		SP<Material> current(nullptr);
		bool		found=false;
		if(sharedMaterial)
		{
			kstl::set<CoreModifiable*>	instances;
			kstl::set<CoreModifiable*>::iterator	it;
			CoreModifiable::GetInstances("Material",instances);

			for(it=instances.begin();it!=instances.end();++it)
			{
				current = CMSP((Material*)(*it), StealRefTag{});
				if(current.get() != newMaterial.get())
					if(current->Equal(*newMaterial.get()))
					{
						found=true;
						break;
					}
			}
		}
		else // not shared, add a flag
		{
			newMaterial->AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE::BOOL,"IsShared");
			newMaterial->setValue(LABEL_TO_ID(IsShared),false);
		}
		if(found)
		{
			newgroup->addItem((CMSP&)current);
		}
		else
		{
			newgroup->addItem((CMSP&)newMaterial);
		}
		

		delete[] currentmat.stages;

		switch(grpdesc.triangleType)
		{
		case	eF_Triangle:
			{
				Mesh::F_Triangle* tmptriangles=new Mesh::F_Triangle[newgroup->myTriangleCount];

				for(j=0;j<newgroup->myTriangleCount;j++)
				{
					tmptriangles[j].Load(myFile);
				}
				newgroup->myFirstTriangle=(Mesh::Triangle*)tmptriangles;
				newgroup->myTriangleSize = sizeof(Mesh::F_Triangle);
				break;
			}
		case	eS_Triangle:
			{
				Mesh::S_Triangle* tmptriangles=new Mesh::S_Triangle[newgroup->myTriangleCount];

				for(j=0;j<newgroup->myTriangleCount;j++)
				{
					tmptriangles[j].Load(myFile);
				}
				newgroup->myFirstTriangle=(Mesh::Triangle*)tmptriangles;
				newgroup->myTriangleSize = sizeof(Mesh::S_Triangle);
				break;
			}
		case	eFC_Triangle:
			{

				Mesh::FC_Triangle* tmptriangles=new Mesh::FC_Triangle[newgroup->myTriangleCount];

				for(j=0;j<newgroup->myTriangleCount;j++)
				{
					tmptriangles[j].Load(myFile);
				}
				newgroup->myFirstTriangle=(Mesh::Triangle*)tmptriangles;
				newgroup->myTriangleSize = sizeof(Mesh::FC_Triangle);
				break;
			}
		case	eSC_Triangle:
			{

				Mesh::SC_Triangle* tmptriangles=new Mesh::SC_Triangle[newgroup->myTriangleCount];

				for(j=0;j<newgroup->myTriangleCount;j++)
				{
					tmptriangles[j].Load(myFile);
				}
				newgroup->myFirstTriangle=(Mesh::Triangle*)tmptriangles;
				newgroup->myTriangleSize = sizeof(Mesh::SC_Triangle);
				break;
			}
		case	eFG_Triangle:
			{
				Mesh::FG_Triangle* tmptriangles=new Mesh::FG_Triangle[newgroup->myTriangleCount];

				for(j=0;j<newgroup->myTriangleCount;j++)
				{
					tmptriangles[j].Load(myFile);
				}
				newgroup->myFirstTriangle=(Mesh::Triangle*)tmptriangles;
				newgroup->myTriangleSize = sizeof(Mesh::FG_Triangle);
				break;
			}
		case	eSG_Triangle:
			{
				Mesh::SG_Triangle* tmptriangles=new Mesh::SG_Triangle[newgroup->myTriangleCount];

				for(j=0;j<newgroup->myTriangleCount;j++)
				{
					tmptriangles[j].Load(myFile);
				}
				newgroup->myFirstTriangle=(Mesh::Triangle*)tmptriangles;
				newgroup->myTriangleSize = sizeof(Mesh::SG_Triangle);
				break;
			}
		case	eTF_Triangle:
			{
				Mesh::TF_Triangle<1>* tmptriangles=new Mesh::TF_Triangle<1>[newgroup->myTriangleCount];

				for(j=0;j<newgroup->myTriangleCount;j++)
				{
					tmptriangles[j].Load(myFile);
				}
				newgroup->myFirstTriangle=(Mesh::Triangle*)tmptriangles;
				newgroup->myTriangleSize = sizeof(Mesh::TF_Triangle<1>);
				break;
			}
		case	eTS_Triangle:
			{
				Mesh::TS_Triangle<1>* tmptriangles=new Mesh::TS_Triangle<1>[newgroup->myTriangleCount];

				for(j=0;j<newgroup->myTriangleCount;j++)
				{
					tmptriangles[j].Load(myFile);
				}
				newgroup->myFirstTriangle=(Mesh::Triangle*)tmptriangles;
				newgroup->myTriangleSize = sizeof(Mesh::TS_Triangle<1>);
				break;
			}
		case	eTFC_Triangle:
			{
				Mesh::TFC_Triangle<1>* tmptriangles=new Mesh::TFC_Triangle<1>[newgroup->myTriangleCount];

				for(j=0;j<newgroup->myTriangleCount;j++)
				{
					tmptriangles[j].Load(myFile);
				}
				newgroup->myFirstTriangle=(Mesh::Triangle*)tmptriangles;
				newgroup->myTriangleSize = sizeof(Mesh::TFC_Triangle<1>);
				break;
			}
		case	eTSC_Triangle:
			{
				Mesh::TSC_Triangle<1>* tmptriangles=new Mesh::TSC_Triangle<1>[newgroup->myTriangleCount];

				for(j=0;j<newgroup->myTriangleCount;j++)
				{
					tmptriangles[j].Load(myFile);
				}
				newgroup->myFirstTriangle=(Mesh::Triangle*)tmptriangles;
				newgroup->myTriangleSize = sizeof(Mesh::TSC_Triangle<1>);

				break;
			}
		case	eTFG_Triangle:
			{
				Mesh::TFG_Triangle<1>* tmptriangles=new Mesh::TFG_Triangle<1>[newgroup->myTriangleCount];

				for(j=0;j<newgroup->myTriangleCount;j++)
				{
					tmptriangles[j].Load(myFile);
				}
				newgroup->myFirstTriangle=(Mesh::Triangle*)tmptriangles;
				newgroup->myTriangleSize = sizeof(Mesh::TFG_Triangle<1>);
				break;
			}
		case	eTSG_Triangle:
			{
				Mesh::TSG_Triangle<1>* tmptriangles=new Mesh::TSG_Triangle<1>[newgroup->myTriangleCount];

				for(j=0;j<newgroup->myTriangleCount;j++)
				{
					tmptriangles[j].Load(myFile);
				}
				newgroup->myFirstTriangle=(Mesh::Triangle*)tmptriangles;
				newgroup->myTriangleSize = sizeof(Mesh::TSG_Triangle<1>);

				break;
			}
		}

		pMesh->addItem((CMSP&)newgroup);
	}

	OtherDataStruct datasize;
	BinFileStruct::ReadStruct(&datasize,myFile);

	fread(&(pMesh->VertexCount),sizeof(unsigned int),1,myFile);
	if(pMesh->VertexCount)
	{
		pMesh->VertexArray=new Point3D[pMesh->VertexCount];

		ReadFloatArray((kfloat*)pMesh->VertexArray,pMesh->VertexCount*3,myFile);
	}

	fread(&(pMesh->NormalCount),sizeof(unsigned int),1,myFile);
	if(pMesh->NormalCount)
	{
		pMesh->NormalArray=new Vector3D[pMesh->NormalCount];

		ReadFloatArray((kfloat*)pMesh->NormalArray,pMesh->NormalCount*3,myFile);
	}

	fread(&(pMesh->TexCoordCount),sizeof(unsigned int),1,myFile);
	if(pMesh->TexCoordCount)
	{
		pMesh->TexArray=new Mesh::TexCoord[pMesh->TexCoordCount];

		ReadFloatArray((kfloat*)pMesh->TexArray,pMesh->TexCoordCount*2,myFile);
	}

	fread(&(pMesh->ColorCount),sizeof(unsigned int),1,myFile);
	if(pMesh->ColorCount)
	{
		pMesh->ColorArray=new Vector4D[pMesh->ColorCount];

		ReadFloatArray((kfloat*)pMesh->ColorArray,pMesh->ColorCount*4,myFile);
	}

	return 0;
}

int BinMeshLoader::ReadFile(ModernMesh *pMesh)
{
	Header	header;

	pMesh->StartMeshBuilder();

	BinFileStruct::ReadStruct(&header,myFile);

	unsigned int fileVersion=header.GetFileVersion();
	int nbGroup=(int)header.GetGroupCount();

	kstl::vector<grpStruct>	grpList;

	int i,j;
	for(i=0;i<nbGroup;i++)
	{
		GroupDesc grpdesc;
		BinFileStruct::ReadStruct(&grpdesc,myFile);

		// convert old enum to new ones
		if(fileVersion<1)
		{
			switch(grpdesc.triangleType)
			{
			case 1:
				grpdesc.triangleType=eF_Triangle;
				break;
			case 2:
				grpdesc.triangleType=eS_Triangle;
				break;
			case 3:
				grpdesc.triangleType=eFC_Triangle;
				break;
			case 4:
				grpdesc.triangleType=eSC_Triangle;
				break;
			case 5:
				grpdesc.triangleType=eFG_Triangle;
				break;
			case 6:
				grpdesc.triangleType=eSG_Triangle;
				break;
			case 7:
				grpdesc.triangleType=eTF_Triangle;
				break;
			case 8:
				grpdesc.triangleType=eTS_Triangle;
				break;
			case 9:
				grpdesc.triangleType=eTFC_Triangle;
				break;
			case 10:
				grpdesc.triangleType=eTSC_Triangle;
				break;
			case 11:
				grpdesc.triangleType=eTFG_Triangle;
				break;
			case 12:
				grpdesc.triangleType=eTSG_Triangle;
				break;
			}
		}

		// load material
		MaterialDesc currentmat;

		BinFileStruct::ReadStruct(&currentmat,myFile);

		char	index[16];
		sprintf(index,"%d",i);
		kstl::string objname=pMesh->getName();
		objname+="MeshItemGroup";
		objname+=index;

		grpStruct	newgroup;

/*		MeshItemGroup* newgroup=(MeshItemGroup*)(KigsCore::GetInstanceOf(objname,"MeshItemGroup"));

		newgroup->myTriangleType=(TriangleType)grpdesc.triangleType;*/

		objname=pMesh->getName();
		objname+="Material";
		objname+=index;

		SP<Material> newMaterial=KigsCore::GetInstanceOf(objname,"Material");
		newMaterial->setValue(LABEL_TO_ID(BlendFuncSource),currentmat.myBlendFuncSource);
		newMaterial->setValue(LABEL_TO_ID(BlendFuncDest),currentmat.myBlendFuncDest);
		newMaterial->setValue(LABEL_TO_ID(BlendEnabled),currentmat.myBlendEnabled);

		newMaterial->setValue(LABEL_TO_ID(MaterialColorEnabled),currentmat.myMaterialColorEnabled);
		if(currentmat.myBlendEnabled)
		{
			newMaterial->setValue("TransarencyFlag", true);
			pMesh->setValue("TransarencyFlag", true);
		}

		if(currentmat.shininess>128.0f)
		{
			currentmat.shininess=128.0f;
		}

		newMaterial->SetAmbientColor((kfloat)currentmat.ambient[0],(kfloat)currentmat.ambient[1],(kfloat)currentmat.ambient[2]);
		newMaterial->SetSpecularColor((kfloat)currentmat.specular[0],(kfloat)currentmat.specular[1],(kfloat)currentmat.specular[2]);
		newMaterial->SetEmissionColor((kfloat)currentmat.emissive[0],(kfloat)currentmat.emissive[1],(kfloat)currentmat.emissive[2]);
		newMaterial->SetDiffuseColor((kfloat)currentmat.diffuse[0],(kfloat)currentmat.diffuse[1],(kfloat)currentmat.diffuse[2],(kfloat)currentmat.alpha);
		newMaterial->setValue(LABEL_TO_ID(Shininess),(kfloat)currentmat.shininess);
		newMaterial->setValue(LABEL_TO_ID(Transparency),(kfloat)currentmat.alpha);

		if((kfloat)currentmat.alpha != KFLOAT_CONST(1.0))
		{
			pMesh->setValue("TransarencyFlag", true);
		}

		newgroup.myTriangleCount=(int)currentmat.triangleCount;
		//newgroup->myTriangleCount=(int)currentmat.triangleCount;

		currentmat.stages = new StageDesc[currentmat.stageCount];
		for(j=0;j<currentmat.stageCount;j++)
		{
			StageDesc& stagedesc= currentmat.stages[j];

			fread(&(stagedesc.structSize),sizeof(stagedesc.structSize),1,myFile);
			fread(&(stagedesc.DescStructSize),sizeof(stagedesc.DescStructSize),1,myFile);

			stagedesc.myTexture=ReadString(myFile);

			fread(&(stagedesc.myStageIndex),sizeof(int),1,myFile);
			fread(&(stagedesc.myTexEnv),sizeof(TexEnvType),1,myFile);

#ifndef NO_MULTISTAGE_RENDERING

			sprintf(index,"%d",j);
			objname=newMaterial->getName();
			objname+="MaterialStage";
			objname+=index;

			SP<MaterialStage> MatStage = KigsCore::GetInstanceOf(objname,"MaterialStage");

			MatStage->setValue(LABEL_TO_ID(StageIndex),stagedesc.myStageIndex);
			MatStage->setValue(LABEL_TO_ID(TexEnv),stagedesc.myTexEnv);

			if(stagedesc.myTexture != "empty")
			{
				auto& texfileManager = KigsCore::Singleton<TextureFileManager>();
				SP<Texture> Tex = texfileManager->GetTexture(stagedesc.myTexture, false);
				Tex->setValue(LABEL_TO_ID(ForcePow2),true);
				Tex->Init();

				MatStage->addItem((CMSP&)Tex);
				
			}

			MatStage->Init();

			newMaterial->addItem((CMSP&)MatStage);
	
#else //NO_MULTISTAGE_RENDERING
			// if no material stages, add texture to material directly
			if(stagedesc.myTexture != "empty")
			{
				TextureFileManager*	fileManager=(TextureFileManager*)KigsCore::GetSingleton("TextureFileManager");
				Texture* Tex=fileManager->GetTexture(stagedesc.myTexture,false);
				Tex->setValue(LABEL_TO_ID(ForcePow2),true);
				Tex->Init();

				newMaterial->addItem(Tex);
				Tex->Destroy();
			}

#endif // NO_MULTISTAGE_RENDERING
		}


		newMaterial->AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE::BOOL,"IsShared");
		newMaterial->setValue(LABEL_TO_ID(IsShared),false);

		newgroup.myMaterial=newMaterial;
		//newgroup->addItem(newMaterial);

		//newMaterial->Destroy();

		delete[] currentmat.stages;

		newgroup.myTriangleType=(unsigned int)grpdesc.triangleType;

		switch(grpdesc.triangleType)
		{
		case	eF_Triangle:
			{
				Mesh::F_Triangle* tmptriangles=new Mesh::F_Triangle[newgroup.myTriangleCount];

				for(j=0;j<newgroup.myTriangleCount;j++)
				{
					tmptriangles[j].Load(myFile);
				}
				newgroup.myFirstTriangle=(Mesh::Triangle*)tmptriangles;
				newgroup.myTriangleSize = sizeof(Mesh::F_Triangle);
				break;
			}
		case	eS_Triangle:
			{
				Mesh::S_Triangle* tmptriangles=new Mesh::S_Triangle[newgroup.myTriangleCount];

				for(j=0;j<newgroup.myTriangleCount;j++)
				{
					tmptriangles[j].Load(myFile);
				}
				newgroup.myFirstTriangle=(Mesh::Triangle*)tmptriangles;
				newgroup.myTriangleSize = sizeof(Mesh::S_Triangle);
				break;
			}
		case	eFC_Triangle:
			{

				Mesh::FC_Triangle* tmptriangles=new Mesh::FC_Triangle[newgroup.myTriangleCount];

				for(j=0;j<newgroup.myTriangleCount;j++)
				{
					tmptriangles[j].Load(myFile);
				}
				newgroup.myFirstTriangle=(Mesh::Triangle*)tmptriangles;
				newgroup.myTriangleSize = sizeof(Mesh::FC_Triangle);
				break;
			}
		case	eSC_Triangle:
			{

				Mesh::SC_Triangle* tmptriangles=new Mesh::SC_Triangle[newgroup.myTriangleCount];

				for(j=0;j<newgroup.myTriangleCount;j++)
				{
					tmptriangles[j].Load(myFile);
				}
				newgroup.myFirstTriangle=(Mesh::Triangle*)tmptriangles;
				newgroup.myTriangleSize = sizeof(Mesh::SC_Triangle);
				break;
			}
		case	eFG_Triangle:
			{
				Mesh::FG_Triangle* tmptriangles=new Mesh::FG_Triangle[newgroup.myTriangleCount];

				for(j=0;j<newgroup.myTriangleCount;j++)
				{
					tmptriangles[j].Load(myFile);
				}
				newgroup.myFirstTriangle=(Mesh::Triangle*)tmptriangles;
				newgroup.myTriangleSize = sizeof(Mesh::FG_Triangle);
				break;
			}
		case	eSG_Triangle:
			{
				Mesh::SG_Triangle* tmptriangles=new Mesh::SG_Triangle[newgroup.myTriangleCount];

				for(j=0;j<newgroup.myTriangleCount;j++)
				{
					tmptriangles[j].Load(myFile);
				}
				newgroup.myFirstTriangle=(Mesh::Triangle*)tmptriangles;
				newgroup.myTriangleSize = sizeof(Mesh::SG_Triangle);
				break;
			}
		case	eTF_Triangle:
			{
				Mesh::TF_Triangle<1>* tmptriangles=new Mesh::TF_Triangle<1>[newgroup.myTriangleCount];

				for(j=0;j<newgroup.myTriangleCount;j++)
				{
					tmptriangles[j].Load(myFile);
				}
				newgroup.myFirstTriangle=(Mesh::Triangle*)tmptriangles;
				newgroup.myTriangleSize = sizeof(Mesh::TF_Triangle<1>);
				break;
			}
		case	eTS_Triangle:
			{
				Mesh::TS_Triangle<1>* tmptriangles=new Mesh::TS_Triangle<1>[newgroup.myTriangleCount];

				for(j=0;j<newgroup.myTriangleCount;j++)
				{
					tmptriangles[j].Load(myFile);
				}
				newgroup.myFirstTriangle=(Mesh::Triangle*)tmptriangles;
				newgroup.myTriangleSize = sizeof(Mesh::TS_Triangle<1>);
				break;
			}
		case	eTFC_Triangle:
			{
				Mesh::TFC_Triangle<1>* tmptriangles=new Mesh::TFC_Triangle<1>[newgroup.myTriangleCount];

				for(j=0;j<newgroup.myTriangleCount;j++)
				{
					tmptriangles[j].Load(myFile);
				}
				newgroup.myFirstTriangle=(Mesh::Triangle*)tmptriangles;
				newgroup.myTriangleSize = sizeof(Mesh::TFC_Triangle<1>);
				break;
			}
		case	eTSC_Triangle:
			{
				Mesh::TSC_Triangle<1>* tmptriangles=new Mesh::TSC_Triangle<1>[newgroup.myTriangleCount];

				for(j=0;j<newgroup.myTriangleCount;j++)
				{
					tmptriangles[j].Load(myFile);
				}
				newgroup.myFirstTriangle=(Mesh::Triangle*)tmptriangles;
				newgroup.myTriangleSize = sizeof(Mesh::TSC_Triangle<1>);

				break;
			}
		case	eTFG_Triangle:
			{
				Mesh::TFG_Triangle<1>* tmptriangles=new Mesh::TFG_Triangle<1>[newgroup.myTriangleCount];

				for(j=0;j<newgroup.myTriangleCount;j++)
				{
					tmptriangles[j].Load(myFile);
				}
				newgroup.myFirstTriangle=(Mesh::Triangle*)tmptriangles;
				newgroup.myTriangleSize = sizeof(Mesh::TFG_Triangle<1>);
				break;
			}
		case	eTSG_Triangle:
			{
				Mesh::TSG_Triangle<1>* tmptriangles=new Mesh::TSG_Triangle<1>[newgroup.myTriangleCount];

				for(j=0;j<newgroup.myTriangleCount;j++)
				{
					tmptriangles[j].Load(myFile);
				}
				newgroup.myFirstTriangle=(Mesh::Triangle*)tmptriangles;
				newgroup.myTriangleSize = sizeof(Mesh::TSG_Triangle<1>);

				break;
			}
		}


		grpList.push_back(newgroup);

		//pMesh->addItem(newgroup);
		//newgroup->Destroy();
	}

	OtherDataStruct datasize;
	BinFileStruct::ReadStruct(&datasize,myFile);

	unsigned int	VertexCount=0;
	Point3D*		VertexArray=0;

	fread(&(VertexCount),sizeof(unsigned int),1,myFile);
	if(VertexCount)
	{
		VertexArray=new Point3D[VertexCount];

		ReadFloatArray((kfloat*)VertexArray,VertexCount*3,myFile);
	}

	unsigned int	NormalCount=0;
	Vector3D*		NormalArray=0;

	fread(&(NormalCount),sizeof(unsigned int),1,myFile);
	if(NormalCount)
	{
		NormalArray=new Vector3D[NormalCount];
		ReadFloatArray((kfloat*)NormalArray,NormalCount*3,myFile);
	}

	unsigned int		TexCoordCount=0;
	Mesh::TexCoord*		TexArray=0;

	fread(&(TexCoordCount),sizeof(unsigned int),1,myFile);
	if(TexCoordCount)
	{
		TexArray=new Mesh::TexCoord[TexCoordCount];
		ReadFloatArray((kfloat*)TexArray,TexCoordCount*2,myFile);
	}

	unsigned int		ColorCount=0;
	Vector4D*			ColorArray=0;

	fread(&(ColorCount),sizeof(unsigned int),1,myFile);
	if(ColorCount)
	{
		ColorArray=new Vector4D[ColorCount];
		ReadFloatArray((kfloat*)ColorArray,ColorCount*4,myFile);
	}

	// manage grps
	kstl::vector<grpStruct>::iterator	itgrp;
	for(itgrp=grpList.begin();itgrp!=grpList.end();++itgrp)
	{
		grpStruct& currentgrp=(*itgrp);

		int structSize=0;

		CoreItemSP	description = CoreItemSP(new CoreVector(), StealRefTag{});
		CoreItemSP	vertices	= CoreItemSP(new CoreNamedVector("vertices"), StealRefTag{});
		description->set("",vertices);

		structSize+=3*sizeof(float);

		// vertices have a color
		if(currentgrp.myTriangleType & 6)
		{
			CoreItemSP	colors	= CoreItemSP(new CoreNamedVector("colors"), StealRefTag{});
			description->set("",colors);
			structSize+=4*sizeof(float);
		}

		// always have normals in kmesh
		{
			CoreItemSP	normal	= CoreItemSP(new CoreNamedVector("normals"), StealRefTag{});
			description->set("",normal);
			structSize+=3*sizeof(float);
		}

		if(currentgrp.myTriangleType & 8)
		{
			CoreItemSP	texCoords	= CoreItemSP(new CoreNamedVector("texCoords"), StealRefTag{});
			description->set("",texCoords);
			structSize+=2*sizeof(float);
		}

		unsigned char* v[3];
		v[0]=new unsigned char[structSize];
		v[1]=new unsigned char[structSize];
		v[2]=new unsigned char[structSize];

		pMesh->StartMeshGroup((CoreVector*)description.get());
		
		unsigned int tri_index;

		unsigned char*	readTriangle=(unsigned char*)currentgrp.myFirstTriangle;

		for(tri_index=0;tri_index<(unsigned int)currentgrp.myTriangleCount;tri_index++)
		{
			int decal=0;
			int copysize=3*sizeof(float);

			Mesh::Triangle* currenttri=(Mesh::Triangle*)readTriangle;

			memcpy(v[0],&VertexArray[currenttri->a],copysize);
			memcpy(v[1],&VertexArray[currenttri->b],copysize);
			memcpy(v[2],&VertexArray[currenttri->c],copysize);

			switch(currentgrp.myTriangleType)
			{
				case	eF_Triangle:
				{
					Mesh::F_Triangle* tmptriangles=(Mesh::F_Triangle*)currenttri;

					decal+=copysize;
					copysize=3*sizeof(float);
					memcpy(v[0]+decal,&NormalArray[tmptriangles->N],copysize);
					memcpy(v[1]+decal,&NormalArray[tmptriangles->N],copysize);
					memcpy(v[2]+decal,&NormalArray[tmptriangles->N],copysize);

					break;
				}
				case	eS_Triangle:
				{
					Mesh::S_Triangle* tmptriangles=(Mesh::S_Triangle*)currenttri;

					decal+=copysize;
					copysize=3*sizeof(float);
					memcpy(v[0]+decal,&NormalArray[tmptriangles->Na],copysize);
					memcpy(v[1]+decal,&NormalArray[tmptriangles->Nb],copysize);
					memcpy(v[2]+decal,&NormalArray[tmptriangles->Nc],copysize);

					break;
				}
				case	eFC_Triangle:
				{
					Mesh::FC_Triangle* tmptriangles=(Mesh::FC_Triangle*)currenttri;

					decal+=copysize;
					copysize=4*sizeof(float);

					memcpy(v[0]+decal,&ColorArray[tmptriangles->C],copysize);
					memcpy(v[1]+decal,&ColorArray[tmptriangles->C],copysize);
					memcpy(v[2]+decal,&ColorArray[tmptriangles->C],copysize);

					decal+=copysize;
					copysize=3*sizeof(float);
					memcpy(v[0]+decal,&NormalArray[tmptriangles->N],copysize);
					memcpy(v[1]+decal,&NormalArray[tmptriangles->N],copysize);
					memcpy(v[2]+decal,&NormalArray[tmptriangles->N],copysize);

					break;
				}
				case	eSC_Triangle:
				{

					Mesh::SC_Triangle* tmptriangles=(Mesh::SC_Triangle*)currenttri;
					decal+=copysize;
					copysize=4*sizeof(float);

					memcpy(v[0]+decal,&ColorArray[tmptriangles->C],copysize);
					memcpy(v[1]+decal,&ColorArray[tmptriangles->C],copysize);
					memcpy(v[2]+decal,&ColorArray[tmptriangles->C],copysize);

					decal+=copysize;
					copysize=3*sizeof(float);
					memcpy(v[0]+decal,&NormalArray[tmptriangles->Na],copysize);
					memcpy(v[1]+decal,&NormalArray[tmptriangles->Nb],copysize);
					memcpy(v[2]+decal,&NormalArray[tmptriangles->Nc],copysize);

					break;
				}
				case	eFG_Triangle:
				{
					Mesh::FG_Triangle* tmptriangles=(Mesh::FG_Triangle*)currenttri;

					decal+=copysize;
					copysize=4*sizeof(float);

					memcpy(v[0]+decal,&ColorArray[tmptriangles->Ca],copysize);
					memcpy(v[1]+decal,&ColorArray[tmptriangles->Cb],copysize);
					memcpy(v[2]+decal,&ColorArray[tmptriangles->Cc],copysize);

					decal+=copysize;
					copysize=3*sizeof(float);
					memcpy(v[0]+decal,&NormalArray[tmptriangles->N],copysize);
					memcpy(v[1]+decal,&NormalArray[tmptriangles->N],copysize);
					memcpy(v[2]+decal,&NormalArray[tmptriangles->N],copysize);

					break;
				}
				case	eSG_Triangle:
				{
					Mesh::SG_Triangle* tmptriangles=(Mesh::SG_Triangle*)currenttri;

					decal+=copysize;
					copysize=4*sizeof(float);

					memcpy(v[0]+decal,&ColorArray[tmptriangles->Ca],copysize);
					memcpy(v[1]+decal,&ColorArray[tmptriangles->Cb],copysize);
					memcpy(v[2]+decal,&ColorArray[tmptriangles->Cc],copysize);

					decal+=copysize;
					copysize=3*sizeof(float);
					memcpy(v[0]+decal,&NormalArray[tmptriangles->Na],copysize);
					memcpy(v[1]+decal,&NormalArray[tmptriangles->Nb],copysize);
					memcpy(v[2]+decal,&NormalArray[tmptriangles->Nc],copysize);

					break;
				}
				case	eTF_Triangle:
				{
					Mesh::TF_Triangle<1>* tmptriangles=(Mesh::TF_Triangle<1>*)currenttri;

					decal+=copysize;
					copysize=3*sizeof(float);
					memcpy(v[0]+decal,&NormalArray[tmptriangles->N],copysize);
					memcpy(v[1]+decal,&NormalArray[tmptriangles->N],copysize);
					memcpy(v[2]+decal,&NormalArray[tmptriangles->N],copysize);

					decal+=copysize;
					copysize=2*sizeof(float);
					memcpy(v[0]+decal,&TexArray[tmptriangles->Ta[0]],copysize);
					memcpy(v[1]+decal,&TexArray[tmptriangles->Tb[0]],copysize);
					memcpy(v[2]+decal,&TexArray[tmptriangles->Tc[0]],copysize);

					break;
				}
				case	eTS_Triangle:
				{
					Mesh::TS_Triangle<1>* tmptriangles=(Mesh::TS_Triangle<1>*)currenttri;

					decal+=copysize;
					copysize=3*sizeof(float);
					memcpy(v[0]+decal,&NormalArray[tmptriangles->Na],copysize);
					memcpy(v[1]+decal,&NormalArray[tmptriangles->Nb],copysize);
					memcpy(v[2]+decal,&NormalArray[tmptriangles->Nc],copysize);

					decal+=copysize;
					copysize=2*sizeof(float);
					memcpy(v[0]+decal,&TexArray[tmptriangles->Ta[0]],copysize);
					memcpy(v[1]+decal,&TexArray[tmptriangles->Tb[0]],copysize);
					memcpy(v[2]+decal,&TexArray[tmptriangles->Tc[0]],copysize);

					break;
				}
				case	eTFC_Triangle:
				{
					Mesh::TFC_Triangle<1>* tmptriangles=(Mesh::TFC_Triangle<1>*)currenttri;
					decal+=copysize;
					copysize=4*sizeof(float);

					memcpy(v[0]+decal,&ColorArray[tmptriangles->C],copysize);
					memcpy(v[1]+decal,&ColorArray[tmptriangles->C],copysize);
					memcpy(v[2]+decal,&ColorArray[tmptriangles->C],copysize);

					decal+=copysize;
					copysize=3*sizeof(float);
					memcpy(v[0]+decal,&NormalArray[tmptriangles->N],copysize);
					memcpy(v[1]+decal,&NormalArray[tmptriangles->N],copysize);
					memcpy(v[2]+decal,&NormalArray[tmptriangles->N],copysize);

					decal+=copysize;
					copysize=2*sizeof(float);
					memcpy(v[0]+decal,&TexArray[tmptriangles->Ta[0]],copysize);
					memcpy(v[1]+decal,&TexArray[tmptriangles->Tb[0]],copysize);
					memcpy(v[2]+decal,&TexArray[tmptriangles->Tc[0]],copysize);

					break;
				}
				case	eTSC_Triangle:
				{
					Mesh::TSC_Triangle<1>* tmptriangles=(Mesh::TSC_Triangle<1>*)currenttri;
					decal+=copysize;
					copysize=4*sizeof(float);

					memcpy(v[0]+decal,&ColorArray[tmptriangles->C],copysize);
					memcpy(v[1]+decal,&ColorArray[tmptriangles->C],copysize);
					memcpy(v[2]+decal,&ColorArray[tmptriangles->C],copysize);

					decal+=copysize;
					copysize=3*sizeof(float);
					memcpy(v[0]+decal,&NormalArray[tmptriangles->Na],copysize);
					memcpy(v[1]+decal,&NormalArray[tmptriangles->Nb],copysize);
					memcpy(v[2]+decal,&NormalArray[tmptriangles->Nc],copysize);

					decal+=copysize;
					copysize=2*sizeof(float);
					memcpy(v[0]+decal,&TexArray[tmptriangles->Ta[0]],copysize);
					memcpy(v[1]+decal,&TexArray[tmptriangles->Tb[0]],copysize);
					memcpy(v[2]+decal,&TexArray[tmptriangles->Tc[0]],copysize);

					break;
				}
				case	eTFG_Triangle:
				{
					Mesh::TFG_Triangle<1>* tmptriangles=(Mesh::TFG_Triangle<1>*)currenttri;

					decal+=copysize;
					copysize=4*sizeof(float);

					memcpy(v[0]+decal,&ColorArray[tmptriangles->Ca],copysize);
					memcpy(v[1]+decal,&ColorArray[tmptriangles->Cb],copysize);
					memcpy(v[2]+decal,&ColorArray[tmptriangles->Cc],copysize);

					decal+=copysize;
					copysize=3*sizeof(float);
					memcpy(v[0]+decal,&NormalArray[tmptriangles->N],copysize);
					memcpy(v[1]+decal,&NormalArray[tmptriangles->N],copysize);
					memcpy(v[2]+decal,&NormalArray[tmptriangles->N],copysize);

					decal+=copysize;
					copysize=2*sizeof(float);
					memcpy(v[0]+decal,&TexArray[tmptriangles->Ta[0]],copysize);
					memcpy(v[1]+decal,&TexArray[tmptriangles->Tb[0]],copysize);
					memcpy(v[2]+decal,&TexArray[tmptriangles->Tc[0]],copysize);

					break;
				}
				case	eTSG_Triangle:
				{
					Mesh::TSG_Triangle<1>* tmptriangles=(Mesh::TSG_Triangle<1>*)currenttri;

					decal+=copysize;
					copysize=4*sizeof(float);

					memcpy(v[0]+decal,&ColorArray[tmptriangles->Ca],copysize);
					memcpy(v[1]+decal,&ColorArray[tmptriangles->Cb],copysize);
					memcpy(v[2]+decal,&ColorArray[tmptriangles->Cc],copysize);

					decal+=copysize;
					copysize=3*sizeof(float);
					memcpy(v[0]+decal,&NormalArray[tmptriangles->Na],copysize);
					memcpy(v[1]+decal,&NormalArray[tmptriangles->Nb],copysize);
					memcpy(v[2]+decal,&NormalArray[tmptriangles->Nc],copysize);

					decal+=copysize;
					copysize=2*sizeof(float);
					memcpy(v[0]+decal,&TexArray[tmptriangles->Ta[0]],copysize);
					memcpy(v[1]+decal,&TexArray[tmptriangles->Tb[0]],copysize);
					memcpy(v[2]+decal,&TexArray[tmptriangles->Tc[0]],copysize);

					break;
				}

			}

			pMesh->AddTriangle(v[0],v[1],v[2]);

			readTriangle+=currentgrp.myTriangleSize;
		}

		delete[] v[0];
		delete[] v[1];
		delete[] v[2];


		SP<ModernMeshItemGroup> created=pMesh->EndMeshGroup();
		created->addItem((CMSP&)currentgrp.myMaterial);
	}


	// delete grps

	for(itgrp=grpList.begin();itgrp!=grpList.end();++itgrp)
	{
		grpStruct& currentgrp=(*itgrp);

		currentgrp.myMaterial=nullptr;
		delete[] ((Mesh::Triangle*)currentgrp.myFirstTriangle);
	}

	grpList.clear();

	// delete arrays
	if(ColorArray)
	{
		delete [] ColorArray;
	}
	if(TexArray)
	{
		delete [] TexArray;
	}
	if(NormalArray)
	{
		delete [] NormalArray;
	}
	if(VertexArray)
	{
		delete [] VertexArray;
	}

	pMesh->EndMeshBuilder();
	pMesh->Init();
	return 0;
}

#ifdef WIN32
int BinMeshLoader::ExportFile(Mesh *pMesh, kstl::string _directoryName, kstl::string _FileName)
{
	pMesh->Set_FileName(_FileName.c_str());
	//------------------------------------------------------------------------------------------------------
	//--------------------------------Recherche les meshItemGroup et les enregistres------------------------
	//------------------------------------------------------------------------------------------------------
	//Releve les meshItemGroup
	kstl::vector<MeshItemGroup*> GroupList;
	int nombredeGroupe = 0;

	kstl::vector<ModifiableItemStruct> MeshsonsList = pMesh->getItems();
	kstl::vector<ModifiableItemStruct>::iterator It = MeshsonsList.begin();

	//parcour les fils de mon mesh
	while(It != MeshsonsList.end())
	{
		//check if this children is a MeshItemGroup
		if ( ((*It).myItem)->isSubType(MeshItemGroup::myClassID) )
		{
			nombredeGroupe++;
			MeshItemGroup* newgroup = ((MeshItemGroup*)(*It).myItem.get());

			GroupList.push_back(newgroup);
		}
		++It;
	}//Parcour le mesh

	//------------------------------------------------------------------------------------------------------
	//--------------------------------parcour les meshItemGroup et remplis les structs----------------------
	//------------------------------------------------------------------------------------------------------

	//Ecriture
	Header	header;
	header.DescStructSize = 0;
	header.structSize = 12;
	header.nbGroup = nombredeGroupe | (1<<24);
	unsigned int fileVersion = header.GetFileVersion();

	kstl::string _fullFileName = _directoryName +"\\"+ _FileName;

	FILE* pFile = fopen ( _fullFileName.c_str() , "wb" );

	if (!pFile)
	{
		KIGS_MESSAGE("OPEN FILE FAILED");
		return 0;
	}
	//Ecriture du header
	BinFileStruct::WriteStruct(&header,pFile);

	MaterialDesc currentmat;
	GroupDesc grpdesc;


	kstl::vector<triangleInfo> triangleList;
	kstl::vector<StageDesc*> StagedescList;
	//Parcour les meshitemGroup
	kstl::vector<MeshItemGroup*>::iterator meshitemGroup_It = GroupList.begin();
	while ( meshitemGroup_It != GroupList.end())
	{
		//Raz de la liste
		triangleList.clear();
		StagedescList.clear();
		MeshItemGroup* newgroup = (*meshitemGroup_It);

		grpdesc.triangleType = newgroup->myTriangleType;

		kstl::vector<ModifiableItemStruct> ItemGroupSonsList = newgroup->getItems();
		kstl::vector<ModifiableItemStruct>::iterator It_ItemGroup = ItemGroupSonsList.begin();

		//parcour les fils de mon mesh
		while(It_ItemGroup != ItemGroupSonsList.end())
		{
			//check if this children is a Material
			if ( ((*It_ItemGroup).myItem)->isSubType(Material::myClassID) )
			{
				int value = 0;
				((*It_ItemGroup).myItem)->getValue("BlendFuncSource",value);
				currentmat.myBlendFuncSource = (BlendFuncSource)value;
				((*It_ItemGroup).myItem)->getValue("BlendFuncDest",value);
				currentmat.myBlendFuncDest = (BlendFuncDest)value;
				bool bvalue = false;
				((*It_ItemGroup).myItem)->getValue("BlendEnabled",bvalue);
				currentmat.myBlendEnabled = bvalue;
				((*It_ItemGroup).myItem)->getValue("MaterialColorEnabled",bvalue);
				currentmat.myMaterialColorEnabled = bvalue;

				//ambiant Color
				float ambiantColor[4];
				((*It_ItemGroup).myItem)->getArrayValue("AmbientColor", ambiantColor, 4);
				currentmat.ambient[0] = ambiantColor[0];
				currentmat.ambient[1] = ambiantColor[1];
				currentmat.ambient[2] = ambiantColor[2];
				currentmat.ambient[3] = 0;

				//Specular
				float specularColor[4];
				((*It_ItemGroup).myItem)->getArrayValue("SpecularColor", specularColor, 4);
				currentmat.specular[0] = specularColor[0];
				currentmat.specular[1] = specularColor[1];
				currentmat.specular[2] = specularColor[2];
				currentmat.specular[3] = 0;

				//EmissionColor
				float emissionColor[4];
				((*It_ItemGroup).myItem)->getArrayValue("EmissionColor", emissionColor, 4);
				currentmat.emissive[0] = emissionColor[0];
				currentmat.emissive[1] = emissionColor[1];
				currentmat.emissive[2] = emissionColor[2];
				currentmat.emissive[3] = 0;

				//DiffuseColor
				float diffuseColor[4];
				((*It_ItemGroup).myItem)->getArrayValue("DiffuseColor", diffuseColor, 4);
				currentmat.diffuse[0] = diffuseColor[0];
				currentmat.diffuse[1] = diffuseColor[1];
				currentmat.diffuse[2] = diffuseColor[2];
				currentmat.diffuse[3] = 0;
				currentmat.alpha = diffuseColor[3];

				((*It_ItemGroup).myItem)->getValue("Shininess",currentmat.shininess);
				((*It_ItemGroup).myItem)->getValue("Transparency",currentmat.alpha);

				currentmat.triangleCount = newgroup->myTriangleCount;

				//parcour les enfant du material
				kstl::vector<ModifiableItemStruct> MaterialSonsList = ((*It_ItemGroup).myItem)->getItems();
				kstl::vector<ModifiableItemStruct>::iterator It_Material = MaterialSonsList.begin();
				unsigned int number = 0;

				//parcour les fils de mon mesh
				while(It_Material != MaterialSonsList.end())
				{
#ifndef NO_MULTISTAGE_RENDERING
					//check if this children is a Material
					if ( ((*It_Material).myItem)->isSubType(MaterialStage::myClassID) )
					{
						number++;
					}
#else //NO_MULTISTAGE_RENDERING
					//check if this children is a Material
					if ( ((*It_Material).myItem)->isSubType(Texture::myClassID) )
					{
						number++;
					}
#endif // NO_MULTISTAGE_RENDERING

					It_Material++;
				}
				currentmat.stageCount = number;

				It_Material = MaterialSonsList.begin();
				//parcour les fils de mon mesh
				while(It_Material != MaterialSonsList.end())
				{
					bool keep=false;
					StageDesc* stagedesc = new StageDesc();

#ifndef NO_MULTISTAGE_RENDERING
					//check if this children is a Material
					if ( ((*It_Material).myItem)->isSubType(MaterialStage::myClassID) )
					{
						keep=true;
						SP<MaterialStage>& MatStage = (SP<MaterialStage> & )(*It_Material).myItem;

						int valuematstage=0;
						MatStage->getValue("StageIndex",valuematstage);
						stagedesc->myStageIndex = valuematstage;
						MatStage->getValue("TexEnv",valuematstage);
						stagedesc->myTexEnv = (TexEnvType)valuematstage;

						//parcour les enfant
						const kstl::vector<ModifiableItemStruct>& MatStageSonsList = MatStage->getItems();
						if ( MatStageSonsList.empty() )
						{
							stagedesc->myTexture = "empty";
						}
						else
						{
							kstl::vector<ModifiableItemStruct>::const_iterator It_MatStage = MatStageSonsList.begin();

							//parcour les fils de mon mesh
							while(It_MatStage != MatStageSonsList.end())
							{
								//check if this children is a Texture
								if ( ((*It_MatStage).myItem)->isSubType(Texture::myClassID) )
								{
									SP<Texture>& temptex =(SP<Texture> &) (*It_MatStage).myItem;
									temptex->getValue("FileName",stagedesc->myTexture);
								}
								It_MatStage++;
							}
						}
					}
#else //NO_MULTISTAGE_RENDERING
					//check if this children is a Material
					if ( ((CoreModifiable*)(*It_Material).myItem)->isSubType(Texture::myClassID) )
					{
						keep=true;
						((Texture*)(*It_Material).myItem)->getValue("FileName",stagedesc->myTexture);
						if ( stagedesc->myTexture.size() == 0 )
						{
							stagedesc->myTexture = "empty";
						}
					}
#endif // NO_MULTISTAGE_RENDERING

					if(keep)
						StagedescList.push_back(stagedesc);
					else
						delete stagedesc;

					It_Material++;

				}
				switch(grpdesc.triangleType)
				{
				case	eF_Triangle:
					{
						Mesh::F_Triangle* tmptriangles  = (Mesh::F_Triangle *)newgroup->myFirstTriangle;

						for(int j=0;j<newgroup->myTriangleCount;j++)
						{
							triangleInfo TriangleStruct;
							TriangleStruct.tabTriangle[0] = tmptriangles[j].a;
							TriangleStruct.tabTriangle[1] = tmptriangles[j].b;
							TriangleStruct.tabTriangle[2] = tmptriangles[j].c;
							TriangleStruct.suppIndex = new unsigned int [1];
							TriangleStruct.suppIndex[0] = tmptriangles[j].N;
							TriangleStruct.suppSize = 1;
							TriangleStruct.isT_Type = false;
							triangleList.push_back(TriangleStruct);
						}
						break;
					}
				case	eS_Triangle:
					{
						Mesh::S_Triangle* tmptriangles  = (Mesh::S_Triangle *)newgroup->myFirstTriangle;

						for(int j=0;j<newgroup->myTriangleCount;j++)
						{
							triangleInfo TriangleStruct;
							TriangleStruct.tabTriangle[0] = tmptriangles[j].a;
							TriangleStruct.tabTriangle[1] = tmptriangles[j].b;
							TriangleStruct.tabTriangle[2] = tmptriangles[j].c;
							TriangleStruct.suppIndex = new unsigned int [3];
							TriangleStruct.suppIndex[0] = tmptriangles[j].Na;
							TriangleStruct.suppIndex[1] = tmptriangles[j].Nb;
							TriangleStruct.suppIndex[2] = tmptriangles[j].Nc;
							TriangleStruct.suppSize = 3;
							TriangleStruct.isT_Type = false;
							triangleList.push_back(TriangleStruct);
						}
						break;
					}
				case	eFC_Triangle:
					{
						Mesh::FC_Triangle* tmptriangles  = (Mesh::FC_Triangle *)newgroup->myFirstTriangle;

						for(int j=0;j<newgroup->myTriangleCount;j++)
						{
							triangleInfo TriangleStruct;
							TriangleStruct.tabTriangle[0] = tmptriangles[j].a;
							TriangleStruct.tabTriangle[1] = tmptriangles[j].b;
							TriangleStruct.tabTriangle[2] = tmptriangles[j].c;
							TriangleStruct.suppIndex = new unsigned int [2];
							TriangleStruct.suppIndex[0] = tmptriangles[j].N;
							TriangleStruct.suppIndex[1] = tmptriangles[j].C;
							TriangleStruct.suppSize = 2;
							TriangleStruct.isT_Type = false;

							triangleList.push_back(TriangleStruct);
						}
						break;
					}
				case	eSC_Triangle:
					{
						Mesh::SC_Triangle* tmptriangles  = (Mesh::SC_Triangle *)newgroup->myFirstTriangle;

						for(int j=0;j<newgroup->myTriangleCount;j++)
						{
							triangleInfo TriangleStruct;
							TriangleStruct.tabTriangle[0] = tmptriangles[j].a;
							TriangleStruct.tabTriangle[1] = tmptriangles[j].b;
							TriangleStruct.tabTriangle[2] = tmptriangles[j].c;
							TriangleStruct.suppIndex = new unsigned int [4];
							TriangleStruct.suppIndex[0] = tmptriangles[j].Na;
							TriangleStruct.suppIndex[1] = tmptriangles[j].Nb;
							TriangleStruct.suppIndex[2] = tmptriangles[j].Nc;
							TriangleStruct.suppIndex[3] = tmptriangles[j].C;
							TriangleStruct.suppSize = 4;
							TriangleStruct.isT_Type = false;

							triangleList.push_back(TriangleStruct);
						}
						break;
					}
				case	eFG_Triangle:
					{
						Mesh::FG_Triangle* tmptriangles  = (Mesh::FG_Triangle *)newgroup->myFirstTriangle;

						for(int j=0;j<newgroup->myTriangleCount;j++)
						{
							triangleInfo TriangleStruct;
							TriangleStruct.tabTriangle[0] = tmptriangles[j].a;
							TriangleStruct.tabTriangle[1] = tmptriangles[j].b;
							TriangleStruct.tabTriangle[2] = tmptriangles[j].c;
							TriangleStruct.suppIndex = new unsigned int [4];
							TriangleStruct.suppIndex[0] = tmptriangles[j].N;
							TriangleStruct.suppIndex[1] = tmptriangles[j].Ca;
							TriangleStruct.suppIndex[2] = tmptriangles[j].Cb;
							TriangleStruct.suppIndex[3] = tmptriangles[j].Cc;
							TriangleStruct.suppSize = 4;
							TriangleStruct.isT_Type = false;

							triangleList.push_back(TriangleStruct);
						}
						break;
					}
				case	eSG_Triangle:
					{
						Mesh::SG_Triangle* tmptriangles  = (Mesh::SG_Triangle *)newgroup->myFirstTriangle;

						for(int j=0;j<newgroup->myTriangleCount;j++)
						{
							triangleInfo TriangleStruct;
							TriangleStruct.tabTriangle[0] = tmptriangles[j].a;
							TriangleStruct.tabTriangle[1] = tmptriangles[j].b;
							TriangleStruct.tabTriangle[2] = tmptriangles[j].c;
							TriangleStruct.suppIndex = new unsigned int [6];
							TriangleStruct.suppIndex[0] = tmptriangles[j].Na;
							TriangleStruct.suppIndex[1] = tmptriangles[j].Nb;
							TriangleStruct.suppIndex[2] = tmptriangles[j].Nc;
							TriangleStruct.suppIndex[3] = tmptriangles[j].Ca;
							TriangleStruct.suppIndex[4] = tmptriangles[j].Cb;
							TriangleStruct.suppIndex[5] = tmptriangles[j].Cc;
							TriangleStruct.suppSize = 6;
							TriangleStruct.isT_Type = false;

							triangleList.push_back(TriangleStruct);
						}
						break;
					}
				case	eTF_Triangle:
					{
						Mesh::TF_Triangle<1>* tmptriangles  = (Mesh::TF_Triangle<1> *)newgroup->myFirstTriangle;

						for(int j=0;j<newgroup->myTriangleCount;j++)
						{
							triangleInfo TriangleStruct;
							TriangleStruct.tabTriangle[0] = tmptriangles[j].a;
							TriangleStruct.tabTriangle[1] = tmptriangles[j].b;
							TriangleStruct.tabTriangle[2] = tmptriangles[j].c;
							TriangleStruct.suppIndex = new unsigned int [1];
							TriangleStruct.suppIndex[0] = tmptriangles[j].N;
							TriangleStruct.T_type[0] = tmptriangles[j].Ta;
							TriangleStruct.T_type[1] = tmptriangles[j].Tb;
							TriangleStruct.T_type[2] = tmptriangles[j].Tc;
							TriangleStruct.suppSize = 1;
							TriangleStruct.isT_Type = true;

							triangleList.push_back(TriangleStruct);
						}
						break;
					}
				case	eTS_Triangle:
					{
						Mesh::TS_Triangle<1>* tmptriangles  = (Mesh::TS_Triangle<1> *)newgroup->myFirstTriangle;

						for(int j=0;j<newgroup->myTriangleCount;j++)
						{
							triangleInfo TriangleStruct;
							TriangleStruct.tabTriangle[0] = tmptriangles[j].a;
							TriangleStruct.tabTriangle[1] = tmptriangles[j].b;
							TriangleStruct.tabTriangle[2] = tmptriangles[j].c;
							TriangleStruct.suppIndex = new unsigned int [3];
							TriangleStruct.suppIndex[0] = tmptriangles[j].Na;
							TriangleStruct.suppIndex[1] = tmptriangles[j].Nb;
							TriangleStruct.suppIndex[2] = tmptriangles[j].Nc;
							TriangleStruct.T_type[0] = tmptriangles[j].Ta;
							TriangleStruct.T_type[1] = tmptriangles[j].Tb;
							TriangleStruct.T_type[2] = tmptriangles[j].Tc;
							TriangleStruct.suppSize = 3;
							TriangleStruct.isT_Type = true;

							triangleList.push_back(TriangleStruct);
						}
						break;
					}
				case	eTFC_Triangle:
					{
						Mesh::TFC_Triangle<1>* tmptriangles  = (Mesh::TFC_Triangle<1> *)newgroup->myFirstTriangle;

						for(int j=0;j<newgroup->myTriangleCount;j++)
						{
							triangleInfo TriangleStruct;
							TriangleStruct.tabTriangle[0] = tmptriangles[j].a;
							TriangleStruct.tabTriangle[1] = tmptriangles[j].b;
							TriangleStruct.tabTriangle[2] = tmptriangles[j].c;
							TriangleStruct.suppIndex = new unsigned int [2];
							TriangleStruct.suppIndex[0] = tmptriangles[j].N;
							TriangleStruct.suppIndex[1] = tmptriangles[j].C;
							TriangleStruct.T_type[0] = tmptriangles[j].Ta;
							TriangleStruct.T_type[1] = tmptriangles[j].Tb;
							TriangleStruct.T_type[2] = tmptriangles[j].Tc;
							TriangleStruct.suppSize = 2;
							TriangleStruct.isT_Type = true;

							triangleList.push_back(TriangleStruct);
						}
						break;
					}
				case	eTSC_Triangle:
					{

						Mesh::TSC_Triangle<1>* tmptriangles  = (Mesh::TSC_Triangle<1> *)newgroup->myFirstTriangle;

						for(int j=0;j<newgroup->myTriangleCount;j++)
						{
							triangleInfo TriangleStruct;
							TriangleStruct.tabTriangle[0] = tmptriangles[j].a;
							TriangleStruct.tabTriangle[1] = tmptriangles[j].b;
							TriangleStruct.tabTriangle[2] = tmptriangles[j].c;
							TriangleStruct.suppIndex = new unsigned int [4];
							TriangleStruct.suppIndex[0] = tmptriangles[j].Na;
							TriangleStruct.suppIndex[1] = tmptriangles[j].Nb;
							TriangleStruct.suppIndex[2] = tmptriangles[j].Nc;
							TriangleStruct.suppIndex[3] = tmptriangles[j].C;
							TriangleStruct.T_type[0] = tmptriangles[j].Ta;
							TriangleStruct.T_type[1] = tmptriangles[j].Tb;
							TriangleStruct.T_type[2] = tmptriangles[j].Tc;
							TriangleStruct.suppSize = 4;
							TriangleStruct.isT_Type = true;

							triangleList.push_back(TriangleStruct);
						}
						break;
					}
				case	eTFG_Triangle:
					{
						Mesh::TFG_Triangle<1>* tmptriangles  = (Mesh::TFG_Triangle<1> *)newgroup->myFirstTriangle;

						for(int j=0;j<newgroup->myTriangleCount;j++)
						{
							triangleInfo TriangleStruct;
							TriangleStruct.tabTriangle[0] = tmptriangles[j].a;
							TriangleStruct.tabTriangle[1] = tmptriangles[j].b;
							TriangleStruct.tabTriangle[2] = tmptriangles[j].c;
							TriangleStruct.suppIndex = new unsigned int [4];
							TriangleStruct.suppIndex[0] = tmptriangles[j].N;
							TriangleStruct.suppIndex[1] = tmptriangles[j].Ca;
							TriangleStruct.suppIndex[2] = tmptriangles[j].Cb;
							TriangleStruct.suppIndex[3] = tmptriangles[j].Cc;
							TriangleStruct.T_type[0] = tmptriangles[j].Ta;
							TriangleStruct.T_type[1] = tmptriangles[j].Tb;
							TriangleStruct.T_type[2] = tmptriangles[j].Tc;
							TriangleStruct.suppSize = 4;
							TriangleStruct.isT_Type = true;

							triangleList.push_back(TriangleStruct);
						}
						break;
					}
				case	eTSG_Triangle:
					{
						Mesh::TSG_Triangle<1>* tmptriangles  = (Mesh::TSG_Triangle<1> *)newgroup->myFirstTriangle;

						for(int j=0;j<newgroup->myTriangleCount;j++)
						{
							triangleInfo TriangleStruct;
							TriangleStruct.tabTriangle[0] = tmptriangles[j].a;
							TriangleStruct.tabTriangle[1] = tmptriangles[j].b;
							TriangleStruct.tabTriangle[2] = tmptriangles[j].c;
							TriangleStruct.suppIndex = new unsigned int [6];
							TriangleStruct.suppIndex[0] = tmptriangles[j].Na;
							TriangleStruct.suppIndex[1] = tmptriangles[j].Nb;
							TriangleStruct.suppIndex[2] = tmptriangles[j].Nc;
							TriangleStruct.suppIndex[3] = tmptriangles[j].Ca;
							TriangleStruct.suppIndex[4] = tmptriangles[j].Cb;
							TriangleStruct.suppIndex[5] = tmptriangles[j].Cc;
							TriangleStruct.T_type[0] = tmptriangles[j].Ta;
							TriangleStruct.T_type[1] = tmptriangles[j].Tb;
							TriangleStruct.T_type[2] = tmptriangles[j].Tc;
							TriangleStruct.suppSize = 6;
							TriangleStruct.isT_Type = true;

							triangleList.push_back(TriangleStruct);
						}
						break;
					}
				}//Fin switch
			}//type material
			It_ItemGroup++;
		}//fin du parcour des fils du meshitemGroup


		grpdesc.structSize = 12;
		grpdesc.DescStructSize = 0;
		//Ecrit le grpdesc
		BinFileStruct::WriteStruct(&grpdesc,pFile);

		currentmat.structSize = 100;
		currentmat.DescStructSize = 0;
		currentmat.myUseless_One = 18;
		currentmat.myUseless_Two=0;
		//Ecrit le material
		BinFileStruct::WriteStruct(&currentmat,pFile);

		//Parcour les stageDesc
		kstl::vector<BinMeshLoader::StageDesc*>::iterator Stagedesc_It = StagedescList.begin();
		while(Stagedesc_It != StagedescList.end())
		{
			StageDesc* stagedesc = (*Stagedesc_It);
			stagedesc->structSize = 21;
			stagedesc->DescStructSize = 0;
			fwrite(&(stagedesc->structSize),sizeof(stagedesc->structSize),1,pFile);
			fwrite(&(stagedesc->DescStructSize),sizeof(stagedesc->DescStructSize),1,pFile);

			fwrite(stagedesc->myTexture.c_str(),sizeof(char) * stagedesc->myTexture.size() + 1 ,1,pFile);

			fwrite(&(stagedesc->myStageIndex),sizeof(int),1,pFile);
			fwrite(&(stagedesc->myTexEnv),sizeof(TexEnvType),1,pFile);

			//Incrémentation de l'itérateur
			++Stagedesc_It;
		}

		if (!triangleList.empty())
		{
			kstl::vector<triangleInfo>::iterator triangleList_It = triangleList.begin();
			while (triangleList_It != triangleList.end())
			{
				triangleInfo temp = (*triangleList_It);
				fwrite(&temp.tabTriangle[0],sizeof(temp.tabTriangle[0]),1,pFile);
				fwrite(&temp.tabTriangle[1],sizeof(temp.tabTriangle[1]),1,pFile);
				fwrite(&temp.tabTriangle[2],sizeof(temp.tabTriangle[2]),1,pFile);

				for (int index = 0; index < temp.suppSize; index++)
				{
					fwrite(&temp.suppIndex[index],sizeof(temp.suppIndex[index]),1,pFile);;
				}

				if(temp.isT_Type)
				{
					fwrite(&temp.T_type[0],sizeof(unsigned int),1,pFile);
					fwrite(&temp.T_type[1],sizeof(unsigned int),1,pFile);
					fwrite(&temp.T_type[2],sizeof(unsigned int),1,pFile);
				}

				//Incrémentation de l'itérateur
				++triangleList_It;
			}
		}

		//Incrémentation de l'iterateur
		++meshitemGroup_It;

	}//fin du parcour des meshitemGroup

	OtherDataStruct datasize;
	datasize.structSize = 36;
	datasize.DescStructSize = 0;
	datasize.coordSize = 4;
	datasize.vertexCoordCount = 3;
	datasize.normalCoordCount = 3;
	datasize.colorCompSize = 4;
	datasize.colorCompCount = 4;
	datasize.uvCompSize = 4;
	datasize.uvCompCount = 2;
	BinFileStruct::WriteStruct(&datasize,pFile);

	float* vertexArray = NULL;
	float* normalArray = NULL;
	float* texArray = NULL;
	float* colorArray = NULL;
	int sizeVertexArray = 0;
	int sizeNormalArray = 0;
	int sizeTexArray = 0;
	int sizeColorArray = 0;

	unsigned int VertexCount = pMesh->VertexCount;
	if (pMesh->VertexArray)
	{
		fwrite(&VertexCount,sizeof(unsigned int),1,pFile);
		vertexArray = (float*)pMesh->VertexArray;
		sizeVertexArray = (int)sizeof(float)*(int)pMesh->VertexCount*3;
		fwrite(vertexArray, sizeVertexArray, 1, pFile);
	}
	else
	{
		fwrite(&VertexCount, sizeVertexArray, 1, pFile);
	}

	unsigned int Normalcount = pMesh->NormalCount;
	if(pMesh->NormalCount)
	{
		fwrite(&Normalcount, sizeof(unsigned int), 1, pFile);
		normalArray = (float*)pMesh->NormalArray;
		sizeNormalArray = (int)sizeof(float)*(int)Normalcount*3;
		fwrite(normalArray, sizeNormalArray, 1, pFile);
	}
	else
	{
		fwrite(&Normalcount, sizeof(unsigned int), 1, pFile);
	}

	unsigned int TexCoordCount = pMesh->TexCoordCount;
	if(pMesh->TexCoordCount)
	{
		fwrite(&TexCoordCount,sizeof(unsigned int),1,pFile);
		texArray = (float*)pMesh->TexArray;
		sizeTexArray = (int)sizeof(float)*(int)pMesh->TexCoordCount*2;
		fwrite(texArray, sizeTexArray, 1, pFile);
	}
	else
	{
		fwrite(&TexCoordCount, sizeof(unsigned int), 1, pFile);
	}
	unsigned int colorCount = pMesh->ColorCount;
	if(pMesh->ColorCount)
	{
		fwrite(&colorCount,sizeof(unsigned int),1,pFile);
		colorArray = (float*)pMesh->ColorArray;
		sizeColorArray =  (int)sizeof(float)*(int)pMesh->ColorCount*4;
		fwrite(colorArray, sizeColorArray, 1, pFile);
	}
	else
	{
		fwrite(&colorCount, sizeof(unsigned int), 1, pFile);
	}

	fclose (pFile);
	return 1;
}
#endif
