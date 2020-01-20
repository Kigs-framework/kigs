#include "PrecompiledHeaders.h"

#include "Material.h"
#include "Texture.h"
#include "MaterialStage.h"
#include "Core.h"
#include "STLMeshLoader.h"
#include "KMesh.h"
#include "TextureFileManager.h"
#include "SceneGraphDefines.h"

STLMeshLoader::STLMeshLoader()
{

}

STLMeshLoader::~STLMeshLoader()
{

}

int STLMeshLoader::ImportFile(Mesh *pMesh, const kstl::string &FileName)
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
int STLMeshLoader::ReadFile(Mesh *pMesh)
{
	// check if ascii or binary file

	int fileType=0;

	char tmpbuf[10];
	if(fread(&tmpbuf,sizeof(char),5,myFile) == 5)
	{
		tmpbuf[5]=0;
		kstl::string compare=	tmpbuf;
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
		while(myFile->searchNextWord("facet"))
		{
			readFacet	toAdd;
			if(myFile->searchNextWord("normal"))
			{
				if(	myFile->getNextFloat(toAdd.mNormal.x) &&
					myFile->getNextFloat(toAdd.mNormal.y) &&
					myFile->getNextFloat(toAdd.mNormal.z) )
				{
					int		vertexIndex;
					bool	vertexOK=true;
					for(vertexIndex=0;vertexIndex<3;vertexIndex++)
					{
						if(myFile->searchNextWord("vertex"))
						{
							if(!(	myFile->getNextFloat(toAdd.mVertex[vertexIndex].x) &&
									myFile->getNextFloat(toAdd.mVertex[vertexIndex].y) &&
									myFile->getNextFloat(toAdd.mVertex[vertexIndex].z) ) )
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
						if(myFile->searchNextWord("endfacet"))
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


	kstl::string objname=pMesh->getName();
	objname+="MeshItemGroup";

	SP<MeshItemGroup> newgroup=KigsCore::GetInstanceOf(objname,"MeshItemGroup");
	newgroup->Init();
	newgroup->myTriangleType=eF_Triangle;

	objname=pMesh->getName();
	objname+="Material";

	SP<Material> newMaterial=KigsCore::GetInstanceOf(objname,"Material");
	newgroup->myTriangleCount=(int)myFacetList.size();

	objname=newMaterial->getName();
	objname+="MaterialStage";
	SP<MaterialStage> MatStage = KigsCore::GetInstanceOf(objname,"MaterialStage");
	MatStage->Init();

	newMaterial->addItem((CMSP&)MatStage);
	newgroup->addItem((CMSP&)newMaterial);


	// totally unoptimized !
	Mesh::F_Triangle* tmptriangles=new Mesh::F_Triangle[newgroup->myTriangleCount];

	pMesh->VertexCount = newgroup->myTriangleCount * 3;
	pMesh->VertexArray=new Point3D[pMesh->VertexCount];
	pMesh->NormalCount = newgroup->myTriangleCount;
	pMesh->NormalArray=new Vector3D[pMesh->NormalCount];

	int j;
	unsigned int indexV=0;
	unsigned int indexN=0;
	for(j=0;j<newgroup->myTriangleCount;j++)
	{
		readFacet&	current=myFacetList[j];

		pMesh->VertexArray[indexV]=current.mVertex[0];
		tmptriangles[j].a=indexV++;
		pMesh->VertexArray[indexV]=current.mVertex[1];
		tmptriangles[j].b=indexV++;
		pMesh->VertexArray[indexV]=current.mVertex[2];
		tmptriangles[j].c=indexV++;
		pMesh->NormalArray[indexN]=current.mNormal;
		tmptriangles[j].N=indexN++;
	}
	newgroup->myFirstTriangle=(Mesh::Triangle*)tmptriangles;
	newgroup->myTriangleSize = sizeof(Mesh::F_Triangle);

	pMesh->addItem((CMSP&)newgroup);
	

	return 0;
}
