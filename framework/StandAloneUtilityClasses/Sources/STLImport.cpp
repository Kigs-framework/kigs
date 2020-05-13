#include "PrecompiledHeaders.h"
#include "STLImport.h"
#include "FilePathManager.h"
#include "ModuleFileManager.h"
#include "CoreVector.h"
#include <iostream>
#include <queue>
#include <fstream>
#include <algorithm>
// mesh includes
#include "KMesh.h"
#include "ModernMesh.h"
#include "Material.h"

#include "TextureFileManager.h"
#include "Texture.h"
#include "MaterialStage.h"

IMPLEMENT_CLASS_INFO(STLImport)

/*
 *	Constructor
 */
STLImport::STLImport(const kstl::string& name,CLASS_NAME_TREE_ARG) : CoreModifiable(name,PASS_CLASS_NAME_TREE_ARG)
,m_FileName(*this,true,LABEL_AND_ID(FileName))
,m_ReadTriangleBuffer(nullptr)
{
	
}

STLImport::~STLImport()
{
	if(m_ReadTriangleBuffer)
		delete m_ReadTriangleBuffer;
}

void	STLImport::InitModifiable()
{	
	CoreModifiable::InitModifiable();
	if(_isInit)
	{
		SP<FilePathManager>	pathManager=KigsCore::GetSingleton("FilePathManager");

		kstl::string fullfilename;

		SmartPointer<FileHandle> fullfilenamehandle;
		fullfilenamehandle = pathManager->FindFullName(m_FileName);

		if (fullfilenamehandle)
		{
			u64 filelen;
			CoreRawBuffer* rawbuffer=ModuleFileManager::LoadFileAsCharString(fullfilenamehandle.get(),filelen,1);


			if (rawbuffer && filelen>=15)
			{
				unsigned char* data=reinterpret_cast<unsigned char*>(rawbuffer->buffer());
				m_CurrentObjectName= fullfilenamehandle->myFileName;
				ReinitReadBuffers();

			
				if (CheckAscii(data, filelen)) // parse ascii
				{
					AsciiParserUtils FileParser(rawbuffer);
					AsciiParserUtils line(FileParser);
					FileParser.GetLine(line, true);
					AsciiParserUtils word(line);
					line.GetWord(word); // solid
					line.GetWord(word); // solid name
					m_CurrentObjectName = (kstl::string)(word);

					FileParser.GetLine(line, true);
					line.GetWord(word); // facet
					while ((kstl::string)word == "facet")
					{
						readFacet& current = (*m_ReadTriangleBuffer)[m_ReadTriangleIndex];
						line.GetWord(word); // normal
						if ((kstl::string)word == "normal")
						{
							line.ReadFloat(current.mNormal.x);
							line.ReadFloat(current.mNormal.y);
							line.ReadFloat(current.mNormal.z);
						}

						FileParser.GetLine(line, true); // outer loop, no check

						for (int vi = 0; vi < 3; vi++)
						{
							FileParser.GetLine(line, true); // vertex vi
							line.GetWord(word); // normal
							if ((kstl::string)word == "vertex")
							{
								line.ReadFloat(current.mVertex[vi].x);
								line.ReadFloat(current.mVertex[vi].y);
								line.ReadFloat(current.mVertex[vi].z);
							}
						}

						FileParser.GetLine(line, true); // endloop, no check
						FileParser.GetLine(line, true); // endfacet, no check

						FileParser.GetLine(line, true);
						line.GetWord(word); // facet

						m_ReadTriangleIndex++;
					}
				}
				else // parse binary
				{
					unsigned char* readData = data + 80;

					unsigned int triangleCount = *(unsigned int*)readData;
					readData += sizeof(unsigned int);

					for (int ti = 0; ti < triangleCount; ti++)
					{
						readFacet& current = (*m_ReadTriangleBuffer)[m_ReadTriangleIndex];

						memcpy(&current, readData, sizeof(readFacet));

						readData += sizeof(readFacet);
						readData += sizeof(unsigned short); // attribute

						m_ReadTriangleIndex++;
					}

				}
			
				createObjectFromReadedData();

				rawbuffer->Destroy();
				return;
			}

		}

	}	
	CoreModifiable::UninitModifiable();
}

bool	STLImport::CheckAscii(unsigned char* buffer, unsigned int l)
{
	AsciiParserUtils FileParser((char*)buffer,l);
	AsciiParserUtils line(FileParser);
	FileParser.GetLine(line, true);
	AsciiParserUtils word(line);
	if (line.GetChars(word, 6))
	{
		if ((kstl::string)word == "solid ")
		{
			FileParser.GetLine(line, true); // get next line
			line.GetWord(word);
			if (((kstl::string)word == "endsolid") || ((kstl::string)word == "facet"))
			{
				return true;
			}
		}
	}

	return false;
	
}

void	STLImport::ReinitReadBuffers()
{
	if(m_ReadTriangleBuffer)
	{
		delete m_ReadTriangleBuffer;
	}
	m_ReadTriangleBuffer=new DynamicGrowingBuffer<readFacet>(512);
	m_ReadTriangleIndex=0;


}

void	STLImport::createObjectFromReadedData()
{
	
	if (m_ReadTriangleIndex == 0)
	{
		return;
	}
	
	SP<ModernMesh> newmesh=KigsCore::GetInstanceOf(m_CurrentObjectName,"ModernMesh");

#ifndef _DEBUG
	newmesh->setValue("Optimize", true);
#endif

	newmesh->StartMeshBuilder();	

		
	int structSize=0;

	CoreItemSP	description = CoreItemSP::getCoreVector();
	CoreItemSP	vertices = CoreItemSP::getCoreItemOfType<CoreNamedVector>("vertices");
	description->set("", vertices);

	structSize += 3 * sizeof(float);


	/*
	CoreNamedVector*	normals = new CoreNamedVector("generate_normals");
	description->push_back(normals);
	normals->Destroy();
	*/

	unsigned char* v[3];
	v[0]=new unsigned char[structSize];
	v[1]=new unsigned char[structSize];
	v[2]=new unsigned char[structSize];

	newmesh->StartMeshGroup((CoreVector*)description.get(), m_ReadTriangleIndex);
	
		// build triangles

	unsigned int tri_index;
	for(tri_index=0;tri_index<m_ReadTriangleIndex;tri_index++)
	{
		int decal=0;
		int copysize=3*sizeof(float);

		readFacet* currenttri=&((*m_ReadTriangleBuffer)[tri_index]);

		memcpy(v[0], &(currenttri->mVertex[0]),copysize);
		memcpy(v[1], &(currenttri->mVertex[1]),copysize);
		memcpy(v[2], &(currenttri->mVertex[2]),copysize);

		newmesh->AddTriangle(v[0],v[1],v[2]);
	}
					
	delete[] v[0];
	delete[] v[1];
	delete[] v[2];

	SP<ModernMeshItemGroup> created=newmesh->EndMeshGroup();


	// create and add empty material

	SP<Material> m_Material = KigsCore::GetInstanceOf(m_CurrentObjectName + "Mat", "Material");

	SP<MaterialStage> MatStage = KigsCore::GetInstanceOf(m_CurrentObjectName + "MatStage", "MaterialStage");

	m_Material->addItem((CMSP&)MatStage);
	

	created->addItem((CMSP&)m_Material);

	newmesh->EndMeshBuilder();

	newmesh->Init();

	m_MeshList.push_back(newmesh);
	
}

