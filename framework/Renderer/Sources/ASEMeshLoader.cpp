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
	mTextureList.clear();
	 mMaterialList.clear();
	mMaterialCount = 0;
	mData=0;
}

ASEMeshLoader::~ASEMeshLoader()
{
}

int ASEMeshLoader::ImportFile(Mesh *pMesh, const kstl::string &FileName)
{
	int Error;

	if(!pMesh || !FileName.length()) return 1;

	pMesh->mFileName=FileName;

	if(mData)
	{
		mData=0;
	}

	// first load txt file
	u64 filelen;
	mData = ModuleFileManager::LoadFileAsCharString(FileName.c_str(),filelen,1);
	if (mData)
	{
		mFileParser = AsciiParserUtils(mData);

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

	mFileParser.Reset();

	int matRef=0;
	// get ref Material for the Node

	if(mFileParser.MoveToString("*MATERIAL_REF"))
	{
		mFileParser.ReadInt(matRef);
	}
	mFileParser.Reset();

	// Get Materials
	mFileParser.MoveToString("*MATERIAL_COUNT");
	mFileParser.ReadInt(mMaterialCount);

	char TextureName[512];
	for(i = 0; i < (unsigned int)mMaterialCount; i++)
	{
		mFileParser.MoveToString("*MATERIAL");
		mFileParser.MoveToString("*MATERIAL_NAME");
	    int subMatCount=0;

		unsigned CurrentPosition = mFileParser.GetPosition();
		if(mFileParser.MoveToString("*NUMSUBMTLS"))
		{
			unsigned submtlpos=mFileParser.GetPosition();

			mFileParser.ReadInt(subMatCount);

			mFileParser.SetPosition(CurrentPosition);

			if(mFileParser.MoveToString("*MATERIAL"))
			{
				if(mFileParser.GetPosition()<submtlpos)
				{
					subMatCount=0;
				}
			}
		}

		mFileParser.SetPosition(CurrentPosition);

		mFileParser.ReadString(TextureName);
		TextureName[strlen(TextureName)-1]=0;

		mMaterialList.push_back(&TextureName[1]);

		materialStruct  tmpStruct;

		if(mFileParser.MoveToString("*MATERIAL_AMBIENT"))
		{
			mFileParser.ReadFloat(tmpStruct.ambient[0]);
			mFileParser.ReadFloat(tmpStruct.ambient[1]);
			mFileParser.ReadFloat(tmpStruct.ambient[2]);
		}

		if(mFileParser.MoveToString("*MATERIAL_DIFFUSE"))
		{
			mFileParser.ReadFloat(tmpStruct.diffuse[0]);
			mFileParser.ReadFloat(tmpStruct.diffuse[1]);
			mFileParser.ReadFloat(tmpStruct.diffuse[2]);
		}

		if(mFileParser.MoveToString("*MATERIAL_SPECULAR"))
		{
			mFileParser.ReadFloat(tmpStruct.specular[0]);
			mFileParser.ReadFloat(tmpStruct.specular[1]);
			mFileParser.ReadFloat(tmpStruct.specular[2]);
		}

		if(mFileParser.MoveToString("*MATERIAL_SHINE"))
		{
			mFileParser.ReadFloat(tmpStruct.shininess);
		}

		kstl::string cutGuillemet1("");
		CurrentPosition = mFileParser.GetPosition();
		if(mFileParser.MoveToString("*MAP_DIFFUSE"))
		{
			if(mFileParser.MoveToString("*BITMAP"))
			{
				mFileParser.ReadString(TextureName);

				TextureName[strlen(TextureName)-1]=0;

				cutGuillemet1=&TextureName[1];
			}
		}
		mFileParser.SetPosition(CurrentPosition);

		if(subMatCount)
		{
			mFileParser.MoveToString("*NUMSUBMTLS");
			for(int j=0;j<subMatCount;j++)
			{
				materialStruct  tmpsubStruct;

				if(mFileParser.MoveToString("*MATERIAL_AMBIENT"))
				{
					mFileParser.ReadFloat(tmpsubStruct.ambient[0]);
					mFileParser.ReadFloat(tmpsubStruct.ambient[1]);
					mFileParser.ReadFloat(tmpsubStruct.ambient[2]);
				}

				if(mFileParser.MoveToString("*MATERIAL_DIFFUSE"))
				{
					mFileParser.ReadFloat(tmpsubStruct.diffuse[0]);
					mFileParser.ReadFloat(tmpsubStruct.diffuse[1]);
					mFileParser.ReadFloat(tmpsubStruct.diffuse[2]);
				}

				if(mFileParser.MoveToString("*MATERIAL_SPECULAR"))
				{
					mFileParser.ReadFloat(tmpsubStruct.specular[0]);
					mFileParser.ReadFloat(tmpsubStruct.specular[1]);
					mFileParser.ReadFloat(tmpsubStruct.specular[2]);
				}

				if(mFileParser.MoveToString("*MATERIAL_SHINE"))
				{
					mFileParser.ReadFloat(tmpsubStruct.shininess);
				}
				tmpStruct.subMat.push_back(tmpsubStruct);
			}
		}

		mMaterialStructList.push_back(tmpStruct);

		mTextureList.push_back(cutGuillemet1);
	}
	mFileParser.Reset();
	//Get Mesh

	mFileParser.MoveToString("*GEOMOBJECT");
	mFileParser.MoveToString("*MESH");
	mFileParser.MoveToString("*MESH_NUMVERTEX");	// jump to the vertices count
	mFileParser.ReadInt(fake);
	pMesh->mVertexCount = fake;
	pMesh->mVertexArray = new Point3D[pMesh->mVertexCount];
	mFileParser.MoveToString("*MESH_NUMFACES");
	mFileParser.ReadInt(TriangleCount);
	TempTriangle = new Mesh::TS_Triangle<1>[TriangleCount];
	mMatIdArray = new int[TriangleCount];
	mFileParser.MoveToString("*MESH_VERTEX_LIST");

	for (i=0;i<pMesh->mVertexCount;i++)
	{
		mFileParser.MoveToString("*MESH_VERTEX");
		mFileParser.ReadInt(fake);
		mFileParser.ReadFloat(pMesh->mVertexArray[i].x);
		mFileParser.ReadFloat(pMesh->mVertexArray[i].y);
		mFileParser.ReadFloat(pMesh->mVertexArray[i].z);
	}

	mFileParser.MoveToString("*MESH_FACE_LIST");
	for (i=0;i<(unsigned int)TriangleCount;i++)
	{
		mFileParser.MoveToString("*MESH_FACE");
		mFileParser.MoveToString("A:");
		mFileParser.ReadInt(fake);
		TempTriangle[i].a = fake;
		mFileParser.MoveToString("B:");
		mFileParser.ReadInt(fake);
		TempTriangle[i].b = fake;
		mFileParser.MoveToString("C:");
		mFileParser.ReadInt(fake);
		TempTriangle[i].c = fake;
		mFileParser.MoveToString("*MESH_MTLID");
		mFileParser.ReadInt(mMatIdArray[i]);
	}

	FilePos = mFileParser.GetPosition();
	if (mFileParser.MoveToString("*MESH_NUMTVERTEX"))
	{
		HasTexture = true;
		mFileParser.ReadInt(fake);
		pMesh->mTexCoordCount = fake;
		pMesh->mTexArray = new Mesh::TexCoord[pMesh->mTexCoordCount];

		mFileParser.MoveToString("*MESH_TVERTLIST");
		for (i=0;i<pMesh->mTexCoordCount;i++)
		{
			mFileParser.MoveToString("*MESH_TVERT");
			mFileParser.ReadInt(fake);
			mFileParser.ReadFloat(pMesh->mTexArray[i].u);
			mFileParser.ReadFloat(pMesh->mTexArray[i].v);
		}

		mFileParser.MoveToString("*MESH_NUMTVFACES");
		mFileParser.MoveToString("*MESH_TFACELIST");
		for (i=0;i<(unsigned int)TriangleCount;i++)
		{
			mFileParser.MoveToString("*MESH_TFACE");
			mFileParser.ReadInt(fake);
			mFileParser.ReadInt(fake);
			TempTriangle[i].Ta[0] = fake;
			mFileParser.ReadInt(fake);
			TempTriangle[i].Tb[0] = fake;
			mFileParser.ReadInt(fake);
			TempTriangle[i].Tc[0] = fake;
		}

	}
	else
	{
		mFileParser.SetPosition(FilePos);
		HasTexture = false;
	}

	if (mFileParser.MoveToString("*MESH_NORMALS"))
	{
		unsigned CurrentPosition = mFileParser.GetPosition();
		HasNormal = true;

		// Retrieve the Normal Count as the max index
		pMesh->mNormalCount = 0;
		for (i=0;i<(unsigned int)TriangleCount;i++)
		{
			mFileParser.MoveToString("*MESH_VERTEXNORMAL");
			mFileParser.ReadInt(fake);
			if ((unsigned int)fake>pMesh->mNormalCount) pMesh->mNormalCount = fake;
			mFileParser.MoveToString("*MESH_VERTEXNORMAL");
			mFileParser.ReadInt(fake);
			if ((unsigned int)fake>pMesh->mNormalCount) pMesh->mNormalCount = fake;
			mFileParser.MoveToString("*MESH_VERTEXNORMAL");
			mFileParser.ReadInt(fake);
			if ((unsigned int)fake>pMesh->mNormalCount) pMesh->mNormalCount = fake;
		}

		pMesh->mNormalCount++;
		pMesh->mNormalArray = new Vector3D[pMesh->mNormalCount];
		mFileParser.SetPosition(CurrentPosition);
		for (i=0;i<(unsigned int)TriangleCount;i++)
		{
			mFileParser.MoveToString("*MESH_VERTEXNORMAL");
			mFileParser.ReadInt(fake);
			TempTriangle[i].Na = fake;
			mFileParser.ReadFloat(pMesh->mNormalArray[fake].x);
			mFileParser.ReadFloat(pMesh->mNormalArray[fake].y);
			mFileParser.ReadFloat(pMesh->mNormalArray[fake].z);
			mFileParser.MoveToString("*MESH_VERTEXNORMAL");
			mFileParser.ReadInt(fake);
			TempTriangle[i].Nb = fake;
			mFileParser.ReadFloat(pMesh->mNormalArray[fake].x);
			mFileParser.ReadFloat(pMesh->mNormalArray[fake].y);
			mFileParser.ReadFloat(pMesh->mNormalArray[fake].z);
			mFileParser.MoveToString("*MESH_VERTEXNORMAL");
			mFileParser.ReadInt(fake);
			TempTriangle[i].Nc = fake;
			mFileParser.ReadFloat(pMesh->mNormalArray[fake].x);
			mFileParser.ReadFloat(pMesh->mNormalArray[fake].y);
			mFileParser.ReadFloat(pMesh->mNormalArray[fake].z);
		}
	}
	else
	{
		HasNormal = false;
	}


	// Rearrange Triangles in TexGroup Order
	unsigned *TriangleCountByGroup;

	TriangleCountByGroup = new unsigned[mMaterialCount];
	for (i=0;i<(unsigned int)mMaterialCount;i++)
	{
		TriangleCountByGroup[i] = 0;
	}
	for (i=0;i<(unsigned int)TriangleCount;i++)
	{
		TriangleCountByGroup[mMatIdArray[i]]++;
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

	bool*	IsFirst=new bool[mMaterialCount];
	int*	currentindex=new int[mMaterialCount];

	for (i=0;i<(unsigned int)mMaterialCount;i++)
	{
		if (TriangleCountByGroup[i])
		{
			int realindex=i;

			if(mMaterialStructList[matRef].subMat.size())
			{
				realindex=matRef;
			}

			MatStage = KigsCore::GetInstanceOf(mTextureList[realindex]+"MatStage","MaterialStage");


			if(mTextureList[realindex]!="")
			{

				auto texfileManager = KigsCore::Singleton<TextureFileManager>();
				Tex = texfileManager->GetTexture(mTextureList[realindex], false);
				Tex->setValue(LABEL_TO_ID(ForcePow2),true);

				Tex->Init();

				MatStage->addItem(Tex);
			}

			MatStage->Init();

			if(mMaterialList[realindex] != "")
			{
				Mat =	KigsCore::GetInstanceOf(mMaterialList[realindex],"Material");
			}
			else
			{
				Mat =	KigsCore::GetInstanceOf(mTextureList[realindex]+"Mat","Material");
			}

			if(mMaterialStructList[matRef].subMat.size())
			{
				unsigned int submatIndex=i;
				if(submatIndex>mMaterialStructList[matRef].subMat.size())
				{
					submatIndex=(unsigned int)mMaterialStructList[matRef].subMat.size();
				}

				BaseMaterialStruct* currentMat;
				if(submatIndex==0)
				{
					currentMat=&mMaterialStructList[matRef];
				}
				else
				{
					currentMat=&mMaterialStructList[matRef].subMat[submatIndex-1];
				}

				Mat->SetAmbientColor(currentMat->ambient[0],currentMat->ambient[1],currentMat->ambient[2]);
				Mat->SetDiffuseColor(currentMat->diffuse[0],currentMat->diffuse[1],currentMat->diffuse[2]);
				Mat->SetSpecularColor(currentMat->specular[0],currentMat->specular[1],currentMat->specular[2]);
				Mat->setValue(LABEL_TO_ID(Shininess),currentMat->shininess*KFLOAT_CONST(128.0));

			}
			else
			{

				Mat->SetAmbientColor(mMaterialStructList[realindex].ambient[0],mMaterialStructList[realindex].ambient[1],mMaterialStructList[realindex].ambient[2]);
				Mat->SetDiffuseColor(mMaterialStructList[realindex].diffuse[0],mMaterialStructList[realindex].diffuse[1],mMaterialStructList[realindex].diffuse[2]);
				Mat->SetSpecularColor(mMaterialStructList[realindex].specular[0],mMaterialStructList[realindex].specular[1],mMaterialStructList[realindex].specular[2]);
				Mat->setValue(LABEL_TO_ID(Shininess),mMaterialStructList[realindex].shininess*KFLOAT_CONST(128.0));
			}


			Mat->addItem(MatStage);

			char L_tmpinstancename[128] = { 0 };
			sprintf(L_tmpinstancename, "meshitem_%i", realindex);
			newgroup = KigsCore::GetInstanceOf(L_tmpinstancename,"MeshItemGroup");
			newgroup->Init();
			IsFirst[i] = true;
			currentindex[i]=0;

			for (j=0;j<TriangleCount;j++)
			{
				if(mMatIdArray[j] == i)
				{
					switch(mode)
					{
					case 0 : // Triangle

						if (IsFirst[i])
						{
							mTriangle = new Mesh::Triangle[TriangleCountByGroup[i]];
							newgroup->mFirstTriangle = mTriangle;
							newgroup->addItem(Mat);
							newgroup->mTriangleCount = 0;
							newgroup->mTriangleSize = sizeof(Mesh::Triangle);
							newgroup->mTriangleType = eF_Triangle;
							IsFirst[i] = false;
						}
						newgroup->mTriangleCount++;
						mTriangle[currentindex[i]++].CopyFrom(TempTriangle[j]);
						break;
					case 1 : // S_Triangle
						if (IsFirst[i])
						{
							mS_Triangle = new Mesh::S_Triangle[TriangleCountByGroup[i]];
							newgroup->mFirstTriangle = mS_Triangle;
							newgroup->addItem(Mat);
							newgroup->mTriangleCount = 0;
							newgroup->mTriangleSize = sizeof(Mesh::S_Triangle);
							newgroup->mTriangleType = eS_Triangle;
							IsFirst[i] = false;
						}
						newgroup->mTriangleCount++;
						mS_Triangle[currentindex[i]++].CopyFrom(TempTriangle[j]);
						break;
					case 2 : // T_Triangle
						if (IsFirst[i])
						{
							mT_Triangle = new Mesh::T_Triangle<1>[TriangleCountByGroup[i]];
							newgroup->mFirstTriangle = mT_Triangle;
							newgroup->addItem(Mat);
							newgroup->mTriangleCount = 0;
							newgroup->mTriangleSize = sizeof(Mesh::T_Triangle<1>);
							newgroup->mTriangleType = eTF_Triangle;
							IsFirst[i] = false;
						}
						newgroup->mTriangleCount++;
						mT_Triangle[currentindex[i]++].CopyFrom(TempTriangle[j]);
						break;
					case 3 : // TS_Triangle
						if (IsFirst[i])
						{
							mTS_Triangle = new Mesh::TS_Triangle<1>[TriangleCountByGroup[i]];
							newgroup->mFirstTriangle = mTS_Triangle;
							newgroup->addItem(Mat);
							newgroup->mTriangleCount = 0;
							newgroup->mTriangleSize = sizeof(Mesh::TS_Triangle<1>);
							newgroup->mTriangleType = eTS_Triangle;
							IsFirst[i] = false;
						}
						newgroup->mTriangleCount++;
						mTS_Triangle[currentindex[i]++].CopyFrom(TempTriangle[j]);
						break;
					} // switch mode
				}
			} // For TriangleCount
			pMesh->addItem(newgroup);
		
		} // if TriangleCountByGroup
	}	// for MaterialCount


	delete [] TempTriangle;
	delete [] TriangleCountByGroup;
	delete [] IsFirst;
	delete [] currentindex;

//	pMesh->ComputeNormals();

	return 0;
}
