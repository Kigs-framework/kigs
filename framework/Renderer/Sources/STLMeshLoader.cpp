#include "PrecompiledHeaders.h"

#include "Material.h"
#include "Texture.h"
#include "MaterialStage.h"
#include "Core.h"
#include "STLMeshLoader.h"
#include "KMesh.h"
#include "TextureFileManager.h"
#include "SceneGraphDefines.h"

using namespace Kigs::Draw;

STLMeshLoader::STLMeshLoader()
{

}

STLMeshLoader::~STLMeshLoader()
{

}

int STLMeshLoader::ImportFile(Mesh *pMesh, const std::string &FileName)
{
	int Error;

	if(!pMesh || !FileName.length())
	{
		KIGS_ERROR("No mesh filename or mesh pointer",1);
		return 1;
	}

	pMesh->setValue("FileName",FileName);

	mFile=File::BufferedFile::Open(FileName.c_str());
	if(mFile)
	{
		Error = ReadFile(pMesh);
		if (Error)
		{
			delete(mFile);
			KIGS_ERROR("Error reading mesh file",1);
			return 10+Error;
		}
		delete(mFile);
		return 0;
	}

	KIGS_ERROR("Mesh file can't be open",1);
	return 3;
}
int STLMeshLoader::ReadFile(Mesh *pMesh)
{
	// check if ascii or binary file

	int fileType=0;

	char tmpbuf[10];
	if(fread(&tmpbuf,sizeof(char),5,mFile) == 5)
	{
		tmpbuf[5]=0;
		std::string compare=	tmpbuf;
		if(compare == "solid")
		{
			fileType=1;
		}
		else
		{
			fileType=2;
		}
	}
	else
	{
		return 1;
	}

	std::vector<readFacet>	myFacetList;

	if(fileType == 1)
	{
		// fill face list with ascii
		while(mFile->searchNextWord("facet"))
		{
			readFacet	toAdd;
			if(mFile->searchNextWord("normal"))
			{
				if(	mFile->getNextFloat(toAdd.mNormal.x) &&
					mFile->getNextFloat(toAdd.mNormal.y) &&
					mFile->getNextFloat(toAdd.mNormal.z) )
				{
					int		vertexIndex;
					bool	vertexOK=true;
					for(vertexIndex=0;vertexIndex<3;vertexIndex++)
					{
						if(mFile->searchNextWord("vertex"))
						{
							if(!(	mFile->getNextFloat(toAdd.mVertex[vertexIndex].x) &&
									mFile->getNextFloat(toAdd.mVertex[vertexIndex].y) &&
									mFile->getNextFloat(toAdd.mVertex[vertexIndex].z) ) )
							{
								vertexOK=false;
								break;
							}
						}
						else
						{
							vertexOK=false;
							break;
						}
					}
					if(vertexOK)
					{
						if(mFile->searchNextWord("endfacet"))
						{
							myFacetList.push_back(toAdd);
						}
					}
				}
			}
		}

		if(myFacetList.size() == 0)
		{
			return 1;
		}

	}
	else
	{
		// read binary
	}


	std::string objname=pMesh->getName();
	objname+="MeshItemGroup";

	SP<MeshItemGroup> newgroup=KigsCore::GetInstanceOf(objname,"MeshItemGroup");
	newgroup->Init();
	newgroup->mTriangleType=eF_Triangle;

	objname=pMesh->getName();
	objname+="Material";

	SP<Material> newMaterial=KigsCore::GetInstanceOf(objname,"Material");
	newgroup->mTriangleCount=(int)myFacetList.size();

	objname=newMaterial->getName();
	objname+="MaterialStage";
	SP<MaterialStage> MatStage = KigsCore::GetInstanceOf(objname,"MaterialStage");
	MatStage->Init();

	newMaterial->addItem(MatStage);
	newgroup->addItem(newMaterial);


	// totally unoptimized !
	Mesh::F_Triangle* tmptriangles=new Mesh::F_Triangle[newgroup->mTriangleCount];

	pMesh->mVertexCount = newgroup->mTriangleCount * 3;
	pMesh->mVertexArray=new v3f[pMesh->mVertexCount];
	pMesh->mNormalCount = newgroup->mTriangleCount;
	pMesh->mNormalArray=new v3f[pMesh->mNormalCount];

	int j;
	unsigned int indexV=0;
	unsigned int indexN=0;
	for(j=0;j<newgroup->mTriangleCount;j++)
	{
		readFacet&	current=myFacetList[j];

		pMesh->mVertexArray[indexV]=current.mVertex[0];
		tmptriangles[j].a=indexV++;
		pMesh->mVertexArray[indexV]=current.mVertex[1];
		tmptriangles[j].b=indexV++;
		pMesh->mVertexArray[indexV]=current.mVertex[2];
		tmptriangles[j].c=indexV++;
		pMesh->mNormalArray[indexN]=current.mNormal;
		tmptriangles[j].N=indexN++;
	}
	newgroup->mFirstTriangle=(Mesh::Triangle*)tmptriangles;
	newgroup->mTriangleSize = sizeof(Mesh::F_Triangle);

	pMesh->addItem(newgroup);
	

	return 0;
}
