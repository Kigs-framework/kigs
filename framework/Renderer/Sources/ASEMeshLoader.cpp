#include "PrecompiledHeaders.h"

#include "Material.h"
#include "Texture.h"
#include "MaterialStage.h"
#include "Core.h"
#include "ASEMeshLoader.h"
#include "TextureFileManager.h"
#include "AsciiParserUtils.h"
#include "ModuleFileManager.h"


ASEMeshLoader::ASEMeshLoader()
{
	myTextureList.clear();
	 myMaterialList.clear();
	MaterialCount = 0;
	m_data=0;
	m_FileParser=0;
}

ASEMeshLoader::~ASEMeshLoader()
{
	myTextureList.clear();
	myMaterialList.clear();

	if(m_FileParser)
	{
		delete m_FileParser;
		m_FileParser=0;
	}

	m_data=0;

}

int ASEMeshLoader::ImportFile(Mesh *pMesh, const kstl::string &FileName)
{
	int Error;

	if(!pMesh || !FileName.length()) return 1;

	pMesh->myFileName=FileName;

	if(m_FileParser)
	{
		delete m_FileParser;
		m_FileParser=0;
	}

	if(m_data)
	{
		m_data=0;
	}

	// first load txt file
	u64 filelen;
	m_data = OwningRawPtrToSmartPtr(ModuleFileManager::LoadFileAsCharString(FileName.c_str(),filelen));
	if (m_data)
	{

		m_FileParser=new AsciiParserUtils(m_data.get());

		Error = ReadFile(pMesh);
		if (Error)
		{
			return 10+Error;
		}

		return 0;
	}

	return 3; //Error
}



int ASEMeshLoader::ReadFile(Mesh *pMesh)
{

	Mesh::TS_Triangle<1> *TempTriangle;

	unsigned int i = 0;
	int			fake;
	unsigned	FilePos;
	int			TriangleCount;
	bool		HasTexture;
	bool		HasNormal;

	m_FileParser->Reset();

	int matRef=0;
	// get ref Material for the Node

	if(m_FileParser->MoveToString("*MATERIAL_REF"))
	{
		m_FileParser->ReadInt(matRef);
	}
	m_FileParser->Reset();

	// Get Materials
	m_FileParser->MoveToString("*MATERIAL_COUNT");
	m_FileParser->ReadInt(MaterialCount);

	char TextureName[512];
	for(i = 0; i < (unsigned int)MaterialCount; i++)
	{
		m_FileParser->MoveToString("*MATERIAL");
		m_FileParser->MoveToString("*MATERIAL_NAME");
	    int subMatCount=0;

		unsigned CurrentPosition = m_FileParser->GetPosition();
		if(m_FileParser->MoveToString("*NUMSUBMTLS"))
		{
			unsigned submtlpos=m_FileParser->GetPosition();

			m_FileParser->ReadInt(subMatCount);

			m_FileParser->SetPosition(CurrentPosition);

			if(m_FileParser->MoveToString("*MATERIAL"))
			{
				if(m_FileParser->GetPosition()<submtlpos)
				{
					subMatCount=0;
				}
			}
		}

		m_FileParser->SetPosition(CurrentPosition);

		m_FileParser->ReadString(TextureName);
		TextureName[strlen(TextureName)-1]=0;

		myMaterialList.push_back(&TextureName[1]);

		materialStruct  tmpStruct;

		if(m_FileParser->MoveToString("*MATERIAL_AMBIENT"))
		{
			m_FileParser->ReadFloat(tmpStruct.ambient[0]);
			m_FileParser->ReadFloat(tmpStruct.ambient[1]);
			m_FileParser->ReadFloat(tmpStruct.ambient[2]);
		}

		if(m_FileParser->MoveToString("*MATERIAL_DIFFUSE"))
		{
			m_FileParser->ReadFloat(tmpStruct.diffuse[0]);
			m_FileParser->ReadFloat(tmpStruct.diffuse[1]);
			m_FileParser->ReadFloat(tmpStruct.diffuse[2]);
		}

		if(m_FileParser->MoveToString("*MATERIAL_SPECULAR"))
		{
			m_FileParser->ReadFloat(tmpStruct.specular[0]);
			m_FileParser->ReadFloat(tmpStruct.specular[1]);
			m_FileParser->ReadFloat(tmpStruct.specular[2]);
		}

		if(m_FileParser->MoveToString("*MATERIAL_SHINE"))
		{
			m_FileParser->ReadFloat(tmpStruct.shininess);
		}

		kstl::string cutGuillemet1("");
		CurrentPosition = m_FileParser->GetPosition();
		if(m_FileParser->MoveToString("*MAP_DIFFUSE"))
		{
			if(m_FileParser->MoveToString("*BITMAP"))
			{
				m_FileParser->ReadString(TextureName);

				TextureName[strlen(TextureName)-1]=0;

				cutGuillemet1=&TextureName[1];
			}
		}
		m_FileParser->SetPosition(CurrentPosition);

		if(subMatCount)
		{
			m_FileParser->MoveToString("*NUMSUBMTLS");
			for(int j=0;j<subMatCount;j++)
			{
				materialStruct  tmpsubStruct;

				if(m_FileParser->MoveToString("*MATERIAL_AMBIENT"))
				{
					m_FileParser->ReadFloat(tmpsubStruct.ambient[0]);
					m_FileParser->ReadFloat(tmpsubStruct.ambient[1]);
					m_FileParser->ReadFloat(tmpsubStruct.ambient[2]);
				}

				if(m_FileParser->MoveToString("*MATERIAL_DIFFUSE"))
				{
					m_FileParser->ReadFloat(tmpsubStruct.diffuse[0]);
					m_FileParser->ReadFloat(tmpsubStruct.diffuse[1]);
					m_FileParser->ReadFloat(tmpsubStruct.diffuse[2]);
				}

				if(m_FileParser->MoveToString("*MATERIAL_SPECULAR"))
				{
					m_FileParser->ReadFloat(tmpsubStruct.specular[0]);
					m_FileParser->ReadFloat(tmpsubStruct.specular[1]);
					m_FileParser->ReadFloat(tmpsubStruct.specular[2]);
				}

				if(m_FileParser->MoveToString("*MATERIAL_SHINE"))
				{
					m_FileParser->ReadFloat(tmpsubStruct.shininess);
				}
				tmpStruct.subMat.push_back(tmpsubStruct);
			}
		}

		myMaterialStructList.push_back(tmpStruct);

		myTextureList.push_back(cutGuillemet1);
	}
	m_FileParser->Reset();
	//Get Mesh

	m_FileParser->MoveToString("*GEOMOBJECT");
	m_FileParser->MoveToString("*MESH");
	m_FileParser->MoveToString("*MESH_NUMVERTEX");	// jump to the vertices count
	m_FileParser->ReadInt(fake);
	pMesh->VertexCount = fake;
	pMesh->VertexArray = new Point3D[pMesh->VertexCount];
	m_FileParser->MoveToString("*MESH_NUMFACES");
	m_FileParser->ReadInt(TriangleCount);
	TempTriangle = new Mesh::TS_Triangle<1>[TriangleCount];
	MatIdArray = new int[TriangleCount];
	m_FileParser->MoveToString("*MESH_VERTEX_LIST");

	for (i=0;i<pMesh->VertexCount;i++)
	{
		m_FileParser->MoveToString("*MESH_VERTEX");
		m_FileParser->ReadInt(fake);
		m_FileParser->ReadFloat(pMesh->VertexArray[i].x);
		m_FileParser->ReadFloat(pMesh->VertexArray[i].y);
		m_FileParser->ReadFloat(pMesh->VertexArray[i].z);
	}

	m_FileParser->MoveToString("*MESH_FACE_LIST");
	for (i=0;i<(unsigned int)TriangleCount;i++)
	{
		m_FileParser->MoveToString("*MESH_FACE");
		m_FileParser->MoveToString("A:");
		m_FileParser->ReadInt(fake);
		TempTriangle[i].a = fake;
		m_FileParser->MoveToString("B:");
		m_FileParser->ReadInt(fake);
		TempTriangle[i].b = fake;
		m_FileParser->MoveToString("C:");
		m_FileParser->ReadInt(fake);
		TempTriangle[i].c = fake;
		m_FileParser->MoveToString("*MESH_MTLID");
		m_FileParser->ReadInt(MatIdArray[i]);
	}

	FilePos = m_FileParser->GetPosition();
	if (m_FileParser->MoveToString("*MESH_NUMTVERTEX"))
	{
		HasTexture = true;
		m_FileParser->ReadInt(fake);
		pMesh->TexCoordCount = fake;
		pMesh->TexArray = new Mesh::TexCoord[pMesh->TexCoordCount];

		m_FileParser->MoveToString("*MESH_TVERTLIST");
		for (i=0;i<pMesh->TexCoordCount;i++)
		{
			m_FileParser->MoveToString("*MESH_TVERT");
			m_FileParser->ReadInt(fake);
			m_FileParser->ReadFloat(pMesh->TexArray[i].u);
			m_FileParser->ReadFloat(pMesh->TexArray[i].v);
		}

		m_FileParser->MoveToString("*MESH_NUMTVFACES");
		m_FileParser->MoveToString("*MESH_TFACELIST");
		for (i=0;i<(unsigned int)TriangleCount;i++)
		{
			m_FileParser->MoveToString("*MESH_TFACE");
			m_FileParser->ReadInt(fake);
			m_FileParser->ReadInt(fake);
			TempTriangle[i].Ta[0] = fake;
			m_FileParser->ReadInt(fake);
			TempTriangle[i].Tb[0] = fake;
			m_FileParser->ReadInt(fake);
			TempTriangle[i].Tc[0] = fake;
		}

	}
	else
	{
		m_FileParser->SetPosition(FilePos);
		HasTexture = false;
	}

	if (m_FileParser->MoveToString("*MESH_NORMALS"))
	{
		unsigned CurrentPosition = m_FileParser->GetPosition();
		HasNormal = true;

		// Retrieve the Normal Count as the max index
		pMesh->NormalCount = 0;
		for (i=0;i<(unsigned int)TriangleCount;i++)
		{
			m_FileParser->MoveToString("*MESH_VERTEXNORMAL");
			m_FileParser->ReadInt(fake);
			if ((unsigned int)fake>pMesh->NormalCount) pMesh->NormalCount = fake;
			m_FileParser->MoveToString("*MESH_VERTEXNORMAL");
			m_FileParser->ReadInt(fake);
			if ((unsigned int)fake>pMesh->NormalCount) pMesh->NormalCount = fake;
			m_FileParser->MoveToString("*MESH_VERTEXNORMAL");
			m_FileParser->ReadInt(fake);
			if ((unsigned int)fake>pMesh->NormalCount) pMesh->NormalCount = fake;
		}

		pMesh->NormalCount++;
		pMesh->NormalArray = new Vector3D[pMesh->NormalCount];
		m_FileParser->SetPosition(CurrentPosition);
		for (i=0;i<(unsigned int)TriangleCount;i++)
		{
			m_FileParser->MoveToString("*MESH_VERTEXNORMAL");
			m_FileParser->ReadInt(fake);
			TempTriangle[i].Na = fake;
			m_FileParser->ReadFloat(pMesh->NormalArray[fake].x);
			m_FileParser->ReadFloat(pMesh->NormalArray[fake].y);
			m_FileParser->ReadFloat(pMesh->NormalArray[fake].z);
			m_FileParser->MoveToString("*MESH_VERTEXNORMAL");
			m_FileParser->ReadInt(fake);
			TempTriangle[i].Nb = fake;
			m_FileParser->ReadFloat(pMesh->NormalArray[fake].x);
			m_FileParser->ReadFloat(pMesh->NormalArray[fake].y);
			m_FileParser->ReadFloat(pMesh->NormalArray[fake].z);
			m_FileParser->MoveToString("*MESH_VERTEXNORMAL");
			m_FileParser->ReadInt(fake);
			TempTriangle[i].Nc = fake;
			m_FileParser->ReadFloat(pMesh->NormalArray[fake].x);
			m_FileParser->ReadFloat(pMesh->NormalArray[fake].y);
			m_FileParser->ReadFloat(pMesh->NormalArray[fake].z);
		}
	}
	else
	{
		HasNormal = false;
	}


	// Rearrange Triangles in TexGroup Order
	unsigned *TriangleCountByGroup;

	TriangleCountByGroup = new unsigned[MaterialCount];
	for (i=0;i<(unsigned int)MaterialCount;i++)
	{
		TriangleCountByGroup[i] = 0;
	}
	for (i=0;i<(unsigned int)TriangleCount;i++)
	{
		TriangleCountByGroup[MatIdArray[i]]++;
	}

	int mode = 0;
	if (HasNormal) mode|=1;
	if (HasTexture) mode|=2;
	// mode = 0 : Triangle
	// mode = 1 : S_Triangle
	// mode = 2 : T_Triangle
	// mode = 3 : TS_Triangle

	SP<Texture> Tex(nullptr);
	SP < Material> Mat(nullptr);
	SP < MaterialStage>	MatStage(nullptr);

	SP<MeshItemGroup> newgroup(nullptr);

	Mesh::Triangle *mTriangle=0;
	Mesh::S_Triangle *mS_Triangle=0;
	Mesh::T_Triangle<1> *mT_Triangle=0;
	Mesh::TS_Triangle<1> *mTS_Triangle=0;
	int j;

	bool*	IsFirst=new bool[MaterialCount];
	int*	currentindex=new int[MaterialCount];

	for (i=0;i<(unsigned int)MaterialCount;i++)
	{
		if (TriangleCountByGroup[i])
		{
			int realindex=i;

			if(myMaterialStructList[matRef].subMat.size())
			{
				realindex=matRef;
			}

			MatStage = KigsCore::GetInstanceOf(myTextureList[realindex]+"MatStage","MaterialStage");


			if(myTextureList[realindex]!="")
			{

				SP<TextureFileManager>	fileManager=KigsCore::GetSingleton("TextureFileManager");
				Tex = fileManager->GetTexture(myTextureList[realindex], false);
				Tex->setValue(LABEL_TO_ID(ForcePow2),true);

				Tex->Init();

				MatStage->addItem((CMSP&)Tex);
			}

			MatStage->Init();

			if(myMaterialList[realindex] != "")
			{
				Mat =	KigsCore::GetInstanceOf(myMaterialList[realindex],"Material");
			}
			else
			{
				Mat =	KigsCore::GetInstanceOf(myTextureList[realindex]+"Mat","Material");
			}

			if(myMaterialStructList[matRef].subMat.size())
			{
				unsigned int submatIndex=i;
				if(submatIndex>myMaterialStructList[matRef].subMat.size())
				{
					submatIndex=myMaterialStructList[matRef].subMat.size();
				}

				BaseMaterialStruct* currentMat;
				if(submatIndex==0)
				{
					currentMat=&myMaterialStructList[matRef];
				}
				else
				{
					currentMat=&myMaterialStructList[matRef].subMat[submatIndex-1];
				}

				Mat->SetAmbientColor(currentMat->ambient[0],currentMat->ambient[1],currentMat->ambient[2]);
				Mat->SetDiffuseColor(currentMat->diffuse[0],currentMat->diffuse[1],currentMat->diffuse[2]);
				Mat->SetSpecularColor(currentMat->specular[0],currentMat->specular[1],currentMat->specular[2]);
				Mat->setValue(LABEL_TO_ID(Shininess),currentMat->shininess*KFLOAT_CONST(128.0));

			}
			else
			{

				Mat->SetAmbientColor(myMaterialStructList[realindex].ambient[0],myMaterialStructList[realindex].ambient[1],myMaterialStructList[realindex].ambient[2]);
				Mat->SetDiffuseColor(myMaterialStructList[realindex].diffuse[0],myMaterialStructList[realindex].diffuse[1],myMaterialStructList[realindex].diffuse[2]);
				Mat->SetSpecularColor(myMaterialStructList[realindex].specular[0],myMaterialStructList[realindex].specular[1],myMaterialStructList[realindex].specular[2]);
				Mat->setValue(LABEL_TO_ID(Shininess),myMaterialStructList[realindex].shininess*KFLOAT_CONST(128.0));
			}


			Mat->addItem((CMSP&)MatStage);

			char L_tmpinstancename[128] = { 0 };
			sprintf(L_tmpinstancename, "meshitem_%i", realindex);
			newgroup = KigsCore::GetInstanceOf(L_tmpinstancename,"MeshItemGroup");
			newgroup->Init();
			IsFirst[i] = true;
			currentindex[i]=0;

			for (j=0;j<TriangleCount;j++)
			{
				if(MatIdArray[j] == i)
				{
					switch(mode)
					{
					case 0 : // Triangle

						if (IsFirst[i])
						{
							mTriangle = new Mesh::Triangle[TriangleCountByGroup[i]];
							newgroup->myFirstTriangle = mTriangle;
							newgroup->addItem((CMSP&)Mat);
							newgroup->myTriangleCount = 0;
							newgroup->myTriangleSize = sizeof(Mesh::Triangle);
							newgroup->myTriangleType = eF_Triangle;
							IsFirst[i] = false;
						}
						newgroup->myTriangleCount++;
						mTriangle[currentindex[i]++].CopyFrom(TempTriangle[j]);
						break;
					case 1 : // S_Triangle
						if (IsFirst[i])
						{
							mS_Triangle = new Mesh::S_Triangle[TriangleCountByGroup[i]];
							newgroup->myFirstTriangle = mS_Triangle;
							newgroup->addItem((CMSP&)Mat);
							newgroup->myTriangleCount = 0;
							newgroup->myTriangleSize = sizeof(Mesh::S_Triangle);
							newgroup->myTriangleType = eS_Triangle;
							IsFirst[i] = false;
						}
						newgroup->myTriangleCount++;
						mS_Triangle[currentindex[i]++].CopyFrom(TempTriangle[j]);
						break;
					case 2 : // T_Triangle
						if (IsFirst[i])
						{
							mT_Triangle = new Mesh::T_Triangle<1>[TriangleCountByGroup[i]];
							newgroup->myFirstTriangle = mT_Triangle;
							newgroup->addItem((CMSP&)Mat);
							newgroup->myTriangleCount = 0;
							newgroup->myTriangleSize = sizeof(Mesh::T_Triangle<1>);
							newgroup->myTriangleType = eTF_Triangle;
							IsFirst[i] = false;
						}
						newgroup->myTriangleCount++;
						mT_Triangle[currentindex[i]++].CopyFrom(TempTriangle[j]);
						break;
					case 3 : // TS_Triangle
						if (IsFirst[i])
						{
							mTS_Triangle = new Mesh::TS_Triangle<1>[TriangleCountByGroup[i]];
							newgroup->myFirstTriangle = mTS_Triangle;
							newgroup->addItem((CMSP&)Mat);
							newgroup->myTriangleCount = 0;
							newgroup->myTriangleSize = sizeof(Mesh::TS_Triangle<1>);
							newgroup->myTriangleType = eTS_Triangle;
							IsFirst[i] = false;
						}
						newgroup->myTriangleCount++;
						mTS_Triangle[currentindex[i]++].CopyFrom(TempTriangle[j]);
						break;
					} // switch mode
				}
			} // For TriangleCount
			pMesh->addItem((CMSP&)newgroup);
		
		} // if TriangleCountByGroup
	}	// for MaterialCount


	delete [] TempTriangle;
	delete [] TriangleCountByGroup;
	delete [] IsFirst;
	delete [] currentindex;

//	pMesh->ComputeNormals();

	return 0;
}
