#include "PLYImport.h"
#include "FilePathManager.h"
#include "ModuleFileManager.h"
#include "ModernMesh.h"
#include "Material.h"
#include "TextureFileManager.h"
#include "Texture.h"
#include "MaterialStage.h"

IMPLEMENT_CLASS_INFO(PLYImport)

/*
*	Constructor
*/
PLYImport::PLYImport(const kstl::string& name, CLASS_NAME_TREE_ARG) : CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG)
, m_FileName(*this, true, LABEL_AND_ID(FileName))
, m_isASCII(false)
{

}

PLYImport::~PLYImport()
{
	
}

void	PLYImport::InitModifiable()
{
	CoreModifiable::InitModifiable();
	if (_isInit)
	{
		SP<FilePathManager>	pathManager = KigsCore::GetSingleton("FilePathManager");

		SmartPointer<FileHandle> fullfilenamehandle;
		fullfilenamehandle = pathManager->FindFullName(m_FileName);

		if (!fullfilenamehandle.isNil())
		{
			m_CurrentObjectName = fullfilenamehandle->mFileName;
			u64 filelen;
			CoreRawBuffer* rawbuffer = ModuleFileManager::LoadFileAsCharString(fullfilenamehandle->mFullFileName.c_str(), filelen,1);

			if (rawbuffer)
			{
				unsigned char* data = (unsigned char*)rawbuffer->buffer();
				
				AsciiParserUtils FileParser(rawbuffer);
				AsciiParserUtils line(FileParser);

				// check first line is "ply"
				if (FileParser.GetLine(line, true))
				{
					kstl::string	content = line;
					if (content != "ply")
					{
						rawbuffer->Destroy();
						goto init_error;
					}
				}
				while (FileParser.GetLine(line, true))
				{
					AsciiParserUtils word(line);
					line.GetWord(word);
					
					kstl::string	strWord= word;


					if (strWord =="comment")
					{
						// skip this comment
					}
					else if (strWord == "format")
					{
						line.GetWord(word);
						strWord = word;

						if (strWord.substr(0, 5) == "ascii")
						{
							m_isASCII = true;
						}
						else if (strWord.substr(0, 6) == "binary")
						{
							m_isASCII = false;
						}

					}
					else if (strWord == "element")
					{

						elementDesc	toSet;
						

						// element
						line.GetWord(word);
						strWord = word;

						toSet.m_typename = strWord;

						int count;
						if (line.ReadInt(count))
						{
							toSet.m_count = count;
							toSet.m_properties.clear();
							m_ElementDescriptors.push_back(toSet);
						}

						
					}
					else if (strWord == "property")
					{
						// property
						if (m_ElementDescriptors.size())
						{
							elementDesc&	currentdesc= m_ElementDescriptors[m_ElementDescriptors.size()-1];
							currentdesc.addProperty(line);
						}
					}
					else if (strWord == "end_header")
					{
						// header is finished go to data reading now
						break;
					}
				}

				// check if data was found
				
				if (m_ElementDescriptors.size())
				{

					SP<ModernMesh> newmesh = KigsCore::GetInstanceOf(m_CurrentObjectName, "ModernMesh");
					newmesh->setValue("Optimize", true);
					newmesh->StartMeshBuilder();

					SP<ModernMeshItemGroup> created = 0;

					if (m_isASCII)
					{
						created = readAsciiData(FileParser, newmesh.get());
					}
					else
					{
						created = readBinData(data + FileParser.GetPosition(), newmesh.get());
					}

					if (created)
					{
						SP<Material> m_Material = KigsCore::GetInstanceOf(m_CurrentObjectName + "Mat", "Material");

						// as there's no material in ply files, just set diffuse and specular to white
						m_Material->SetDiffuseColor(1.0,1.0,1.0);
						m_Material->SetSpecularColor(1.0, 1.0, 1.0);


						SP<MaterialStage> MatStage = KigsCore::GetInstanceOf(m_CurrentObjectName + "MatStage", "MaterialStage");

						m_Material->addItem((CMSP&)MatStage);
						

						created->addItem((CMSP&)m_Material);

						newmesh->EndMeshBuilder();

						newmesh->Init();

						m_MeshList.push_back(newmesh);
					}
				}

				rawbuffer->Destroy();
				return;
			}

		}

	}
init_error:
	CoreModifiable::UninitModifiable();
}


void	PLYImport::elementDesc::addProperty(AsciiParserUtils& line)
{
	AsciiParserUtils word(line);
	line.GetWord(word);
	kstl::string	strWord = word;

	propertyDesc	toSet;
	toSet.m_type = toSet.m_ListIndexSize = toSet.m_ListIndexType = propertyUnknown;
	

	propertyType currentType = getTypeFromString(strWord);
	toSet.m_type = currentType;
	if (currentType == propertyList)
	{
		// read list info
		line.GetWord(word);
		strWord = word;

		toSet.m_ListIndexSize=getTypeFromString(strWord);

		line.GetWord(word);
		strWord = word;

		toSet.m_ListIndexType = getTypeFromString(strWord);

	}
	line.GetWord(word);
	toSet.m_propertyname = word;
	
	if (toSet.m_type != propertyUnknown)
	{
		m_properties.push_back(toSet);
	}
}

SP<ModernMeshItemGroup>	PLYImport::readBinData(unsigned char* bindata,ModernMesh* currentMesh)
{
	
	// should have two elements : vertex and face
	if (m_ElementDescriptors.size() != 2)
	{
		return 0;
	}


	// TODO : manage different kind of face structure

	if ((m_ElementDescriptors[1].m_properties[0].m_ListIndexSize != propertyChar) || (m_ElementDescriptors[1].m_properties[0].m_ListIndexType != propertyInt))
	{
		return 0;
	}

	int readVertexSize=getElementSize(m_ElementDescriptors[0]);

	unsigned char* facedata = bindata + (readVertexSize*m_ElementDescriptors[0].m_count);

	int readsize = 0;

	// check structure type
	int structSize = 0;
	CoreItemSP	description = CoreItemSP::getCoreVector();
	// always have vertices
	CoreItemSP	vertices = CoreItemSP::getCoreItemOfType<CoreNamedVector>("vertices");
	description->set("",vertices);

	structSize += 3 * sizeof(float);

	int l_hasNormal = hasNormal(m_ElementDescriptors[0]);
	int l_hasColor = hasColor(m_ElementDescriptors[0]);

	// vertices have a color, so  
	if (l_hasColor != -1)
	{
		CoreItemSP	colors = CoreItemSP::getCoreItemOfType<CoreNamedVector>("colors");
		description->set("", colors);
		structSize += 4 * sizeof(float);
	}

	if (l_hasNormal != -1)
	{
		CoreItemSP	normal = CoreItemSP::getCoreItemOfType<CoreNamedVector>("normals");
		description->set("", normal);
		structSize += 3 * sizeof(float);
	}

	unsigned char* v[3];
	v[0] = new unsigned char[structSize];
	v[1] = new unsigned char[structSize];
	v[2] = new unsigned char[structSize];

	currentMesh->StartMeshGroup((CoreVector*)description.get());

	// build triangles

	unsigned int face_index;
	unsigned char* currrentReadInFace = facedata;

	kstl::vector<unsigned int>	indices;
	indices.reserve(4);

	for (face_index = 0; face_index<m_ElementDescriptors[1].m_count; face_index++)
	{
		int indexCount = *currrentReadInFace;
		++currrentReadInFace;
		indices.clear();

		for (int indicesI = 0; indicesI < indexCount; indicesI++)
		{
			indices.push_back(*((int*)currrentReadInFace));
			currrentReadInFace += 4;
		}
		
		for (int triangles = 0; triangles < indices.size() - 2; triangles++)
		{
			for (int verticeI = 0; verticeI < 3; verticeI++)
			{
				int realVerticeI = verticeI;
				if (verticeI != 0)
				{
					realVerticeI += triangles;
				}

				int decal = 0;
				int copysize = 3 * sizeof(float);

				memcpy(v[verticeI] + decal, bindata+readVertexSize*indices[realVerticeI], copysize);
	
				if (l_hasColor != -1)
				{
					decal += copysize;
					copysize = 4 * sizeof(float);

					unsigned char*	rgbaColor = (bindata + l_hasColor + readVertexSize*indices[realVerticeI]);

					Vector4D	convertedColor((int)rgbaColor[0], (int)rgbaColor[1], (int)rgbaColor[2], (int)rgbaColor[3]);

					convertedColor *= 1.0f / 255.0f;

					memcpy(v[verticeI] + decal, &convertedColor, copysize);
				}

				if (l_hasNormal != -1)
				{
					decal += copysize;
					copysize = 3 * sizeof(float);
					memcpy(v[verticeI] + decal, (bindata + l_hasNormal + readVertexSize*indices[realVerticeI]), copysize);
					
				}
			}
			currentMesh->AddTriangle(v[0], v[1], v[2]);
		}

	}

	delete[] v[0];
	delete[] v[1];
	delete[] v[2];
	SP<ModernMeshItemGroup> created = currentMesh->EndMeshGroup();

	return created;
}

SP<ModernMeshItemGroup>	PLYImport::readAsciiData(AsciiParserUtils& asciiData, ModernMesh* currentMesh)
{
	// TODO
	return nullptr;
}

