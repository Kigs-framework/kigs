#include "PrecompiledHeaders.h"
#include "OBJImport.h"
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

IMPLEMENT_CLASS_INFO(OBJImport)

/*
 *	Constructor
 */
OBJImport::OBJImport(const kstl::string& name,CLASS_NAME_TREE_ARG) : CoreModifiable(name,PASS_CLASS_NAME_TREE_ARG)
,m_FileName(*this,true,LABEL_AND_ID(FileName))
,m_FirstMesh(*this,false,LABEL_AND_ID(FirstMesh),"") // used only to get value
,m_ModernMesh(*this,true,LABEL_AND_ID(ModernMesh),false)
,m_ExportPath(*this, false, LABEL_AND_ID(ExportPath), "")
,m_ReadVertexBuffer(nullptr)
,m_ReadTextCoordsBuffer(nullptr)
,m_ReadNormalBuffer(nullptr)
,m_ReadColorBuffer(nullptr)
,m_CurrentFacetGroup(nullptr)
{
	
}

OBJImport::~OBJImport()
{
	if(m_ReadVertexBuffer)
		delete m_ReadVertexBuffer;
	if(m_ReadTextCoordsBuffer)
		delete m_ReadTextCoordsBuffer;
	if(m_ReadNormalBuffer)
		delete m_ReadNormalBuffer;
	if(m_ReadColorBuffer)
		delete m_ReadColorBuffer;

	m_materialList.clear();
}

void	OBJImport::InitModifiable()
{	
	CoreModifiable::InitModifiable();
	if(_isInit)
	{
		SP<FilePathManager>	pathManager=KigsCore::GetSingleton("FilePathManager");

		// create a faked empty group if no mat
		m_CurrentFacetGroup=&m_FacetGroupList["emptyMat"];
		if(m_CurrentFacetGroup->matName == "")
		{
			m_CurrentFacetGroup->matName="emptyMat";
			m_CurrentFacetGroup->m_ReadFacetBuffer=new	DynamicGrowingBuffer<Triangle>(256);
		}

		m_currentMatRead=&m_materialList["emptyMat"];
		m_currentMatRead->name="emptyMat";

		kstl::string fullfilename;

		SmartPointer<FileHandle> fullfilenamehandle;
		fullfilenamehandle = pathManager->FindFullName(m_FileName);

		if (fullfilenamehandle)
		{
			u64 filelen;
			CoreRawBuffer* rawbuffer=ModuleFileManager::LoadFileAsCharString(fullfilenamehandle.get(),filelen,1);

			if (rawbuffer)
			{
				unsigned char* data=reinterpret_cast<unsigned char*>(rawbuffer->buffer());
				m_CurrentObjectName= fullfilenamehandle->mFileName;
				ReinitReadBuffers();
				AsciiParserUtils FileParser(rawbuffer);
				AsciiParserUtils line(FileParser);
				while(FileParser.GetLine(line,true))
				{
					// buffer a 0 at the end of the line
					AsciiParserUtils word(FileParser);
					if(line.GetWord(word))
					{
						switch(word[0])
						{
						case  '#':
							// comment
							break;
						case  'o':
							// object
							StartObj(line);
							break;
						case  'v':
							// 
							{
								if(word.length() == 1)
								{
									ParseVertex(line);
								}
								else
								{
									if(word[1] == 't')
									{
										ParseTextureCoord(line);
									}
									else if(word[1] == 'n')
									{
										ParseNormal(line);
									}
								}
				
							}
							break;
						case  'f':
							// 
							{
								ParseFacet(line);
							}
							break;
						
						case 'm':
							{
								if(static_cast<kstl::string>(word)=="mtllib")
								{
									AsciiParserUtils fname(line);
									if(line.GetTrailingPart(fname))
									{
										if(static_cast<kstl::string>(fname) != "")
										{
											MTLImport(static_cast<kstl::string>(fname));
										}
									}
								}
							}
							break;
						case 'u':
							{
								if(static_cast<kstl::string>(word)=="usemtl")
								{
									// retrieve mat name
									AsciiParserUtils matname(line);
									if(line.GetWord(matname))
									{
										m_CurrentFacetGroup=&m_FacetGroupList[matname];
										if(m_CurrentFacetGroup->matName == "")
										{
											m_CurrentFacetGroup->matName=matname;
											m_CurrentFacetGroup->m_ReadFacetBuffer=new	DynamicGrowingBuffer<Triangle>(256);
										}
									}
								}
							}
							break;
						default: 
							break;
						}
					}
				}
	
				createObjectFromReadedData();
#ifdef WIN32
				if (m_ExportPath.const_ref() != "")
				{
					ExportResult();
				}
#endif
				rawbuffer->Destroy();
				return;
			}

		}

	}	
	CoreModifiable::UninitModifiable();
}

void	OBJImport::ReinitReadBuffers()
{
	if(m_ReadVertexBuffer)
	{
		delete m_ReadVertexBuffer;
	}
	m_ReadVertexBuffer=new DynamicGrowingBuffer<Point3D>(512);
	m_ReadVertexIndex=0;
	if(m_ReadTextCoordsBuffer)
	{
		delete m_ReadTextCoordsBuffer;
	}
	m_ReadTextCoordsBuffer=new DynamicGrowingBuffer<Point3D>(512);
	m_ReadTextCoordIndex=0;
	if(m_ReadNormalBuffer)
	{
		delete m_ReadNormalBuffer;
	}
	m_ReadNormalBuffer=new DynamicGrowingBuffer<Point3D>(512);
	m_ReadNormalIndex=0;

	if(m_ReadColorBuffer)
	{
		delete m_ReadColorBuffer;
	}
	m_ReadColorBuffer=new DynamicGrowingBuffer<Vector4D>(512);
	m_ReadColorIndex=0;
}

void	OBJImport::createObjectFromReadedData()
{
	// init all materials

	kstl::map<kstl::string,ReadMaterial>::iterator itmat=m_materialList.begin();
	kstl::map<kstl::string,ReadMaterial>::iterator itmatend=m_materialList.end();

	while(itmat != itmatend)
	{
		(*itmat).second.Init();
		++itmat;
	}

	if(m_ModernMesh)
	{
		SP<ModernMesh> newmesh=KigsCore::GetInstanceOf(m_CurrentObjectName,"ModernMesh");
		newmesh->setValue("Optimize", true);
		// create each group
		kstl::map<kstl::string,FacetGroup>::iterator itgroup=m_FacetGroupList.begin();
		kstl::map<kstl::string,FacetGroup>::iterator itgroupend=m_FacetGroupList.end();
		
		newmesh->StartMeshBuilder();	

		while(itgroup != itgroupend)
		{

			FacetGroup& current=(*itgroup).second;
			if(current.m_ReadFacetCount)
			{
				itmat=m_materialList.find(current.matName);
				if(itmat != m_materialList.end())
				{
					int structSize=0;

					CoreItemSP	description= CoreItemSP::getCoreVector();
					CoreItemSP	vertices= CoreItemSP::getCoreItemOfType<CoreNamedVector>("vertices");
					description->set("",vertices);

					structSize+=3*sizeof(float);

					// vertices have a color, so  
					if(m_ReadColorIndex)
					{
						CoreItemSP	colors	= CoreItemSP::getCoreItemOfType<CoreNamedVector>("colors");
						description->set("", colors);
						structSize+=4*sizeof(float);
					}

					if(current.m_HasNormal)
					{
						CoreItemSP	normal	= CoreItemSP::getCoreItemOfType<CoreNamedVector>("normals");
						description->set("", normal);
						structSize+=3*sizeof(float);
					}

					if(current.m_HasTextCoords)
					{
						CoreItemSP	texCoords	= CoreItemSP::getCoreItemOfType<CoreNamedVector>("texCoords");
						description->set("", texCoords);
						structSize+=2*sizeof(float);
					}
					
					unsigned char* v[3];
					v[0]=new unsigned char[structSize];
					v[1]=new unsigned char[structSize];
					v[2]=new unsigned char[structSize];

					newmesh->StartMeshGroup((CoreVector*)description.get());
				
					// build triangles

					unsigned int tri_index;
					for(tri_index=0;tri_index<current.m_ReadFacetCount;tri_index++)
					{
						int decal=0;
						int copysize=3*sizeof(float);

						Triangle* currenttri=&((*current.m_ReadFacetBuffer)[tri_index]);

						memcpy(v[0],&(*m_ReadVertexBuffer)[currenttri->indexes[0].iv-1],copysize);
						memcpy(v[1],&(*m_ReadVertexBuffer)[currenttri->indexes[1].iv-1],copysize);
						memcpy(v[2],&(*m_ReadVertexBuffer)[currenttri->indexes[2].iv-1],copysize);

						if(m_ReadColorIndex)
						{
							decal+=copysize;
							copysize=4*sizeof(float);
							memcpy(v[0]+decal,&(*m_ReadColorBuffer)[currenttri->indexes[0].iv-1],copysize);
							memcpy(v[1]+decal,&(*m_ReadColorBuffer)[currenttri->indexes[1].iv-1],copysize);
							memcpy(v[2]+decal,&(*m_ReadColorBuffer)[currenttri->indexes[2].iv-1],copysize);
						}

						if(current.m_HasNormal)
						{
							decal+=copysize;
							copysize=3*sizeof(float);
							memcpy(v[0]+decal,&(*m_ReadNormalBuffer)[currenttri->indexes[0].in-1],copysize);
							memcpy(v[1]+decal,&(*m_ReadNormalBuffer)[currenttri->indexes[1].in-1],copysize);
							memcpy(v[2]+decal,&(*m_ReadNormalBuffer)[currenttri->indexes[2].in-1],copysize);
						}

						if(current.m_HasTextCoords)
						{
							decal+=copysize;
							copysize=2*sizeof(float);
							memcpy(v[0]+decal,&(*m_ReadTextCoordsBuffer)[currenttri->indexes[0].it-1],copysize);
							memcpy(v[1]+decal,&(*m_ReadTextCoordsBuffer)[currenttri->indexes[1].it-1],copysize);
							memcpy(v[2]+decal,&(*m_ReadTextCoordsBuffer)[currenttri->indexes[2].it-1],copysize);
						}
						newmesh->AddTriangle(v[0],v[1],v[2]);
					}
					
					delete[] v[0];
					delete[] v[1];
					delete[] v[2];


					SP<ModernMeshItemGroup> created=newmesh->EndMeshGroup();


					created->addItem((CMSP&)(*itmat).second.m_Material);

				}
			}
			++itgroup;
		}
		newmesh->EndMeshBuilder();

		newmesh->Init();

		if(m_MeshList.size()==0)
		{
			m_FirstMesh=newmesh.get();
		}
		m_MeshList.push_back(newmesh);
	}
	else
	{
		// create mesh

		SP<Mesh> newmesh=KigsCore::GetInstanceOf(m_CurrentObjectName,"Mesh");

		newmesh->Set_VertexCount(m_ReadVertexIndex);
		Point3D* varray=new Point3D[m_ReadVertexIndex];
		unsigned int i;
		for(i=0;i<m_ReadVertexIndex;i++)
		{
			varray[i]=(*m_ReadVertexBuffer)[i];
		}

		newmesh->Set_VertexArray(varray);

		bool needNormalComputation=false;

		if(m_ReadTextCoordIndex)
		{
			newmesh->Set_TexCoordCount(m_ReadTextCoordIndex);
			Mesh::TexCoord* tcarray=new Mesh::TexCoord[m_ReadTextCoordIndex];
			for(i=0;i<m_ReadTextCoordIndex;i++)
			{
				tcarray[i].u=(*m_ReadTextCoordsBuffer)[i].x;
				tcarray[i].v=(*m_ReadTextCoordsBuffer)[i].y;
			}
			newmesh->Set_TexArray(tcarray);
		}

		if(m_ReadNormalIndex)
		{
			newmesh->Set_NormalCount( m_ReadNormalIndex );
			Vector3D* narray=new Vector3D[m_ReadNormalIndex];
			for(i=0;i<m_ReadNormalIndex;i++)
			{
				narray[i]=(*m_ReadNormalBuffer)[i];
			}
			newmesh->Set_NormalArray(narray);
		}
		else // flat create one normal per triangle
		{
			// compute total triangle count
			unsigned int total_triangle=0;
			kstl::map<kstl::string,FacetGroup>::iterator itgroup=m_FacetGroupList.begin();
			kstl::map<kstl::string,FacetGroup>::iterator itgroupend=m_FacetGroupList.end();
			while(itgroup != itgroupend)
			{
				FacetGroup& current=(*itgroup).second;
				total_triangle+=current.m_ReadFacetCount;
				++itgroup;
			}
			newmesh->Set_NormalCount( total_triangle );
			Vector3D* narray=new Vector3D[total_triangle];
			Vector3D  dummyN(1.0,0.0,0.0);	
			for(i=0;i<total_triangle;i++)
			{
				narray[i]=dummyN;
			}
			newmesh->Set_NormalArray(narray);
			needNormalComputation=true;
		}

		if(m_ReadColorIndex)
		{
			newmesh->Set_ColorCount( m_ReadColorIndex );
			Vector4D* carray=new Vector4D[m_ReadColorIndex];
			for(i=0;i<m_ReadColorIndex;i++)
			{
				carray[i]=(*m_ReadColorBuffer)[i];
			}
			newmesh->Set_ColorArray(carray);
		}

		// create each group
		kstl::map<kstl::string,FacetGroup>::iterator itgroup=m_FacetGroupList.begin();
		kstl::map<kstl::string,FacetGroup>::iterator itgroupend=m_FacetGroupList.end();
		int triangle_index=0;
		while(itgroup != itgroupend)
		{
			FacetGroup& current=(*itgroup).second;
			if(current.m_ReadFacetCount)
			{

				itmat=m_materialList.find(current.matName);
				if(itmat != m_materialList.end())
				{
					char L_tmpinstancename[128] = { 0 };
					sprintf(L_tmpinstancename, "meshitem_%u", newmesh->getItems().size());
					SP<MeshItemGroup> newgroup = KigsCore::GetInstanceOf(L_tmpinstancename, "MeshItemGroup");
					newgroup->Init();
					newgroup->mTriangleCount=current.m_ReadFacetCount;

					if((current.m_HasTextCoords) && (current.m_HasNormal))
					{
						current.InitSTTriangle(newgroup.get(),triangle_index);
					}
					else if(current.m_HasTextCoords)
					{
						current.InitTTriangle(newgroup.get(),triangle_index);
					}
					else if(current.m_HasNormal)
					{
						if(m_ReadColorIndex)
						{
							current.InitSGTriangle(newgroup.get(),triangle_index);
						}
						else
						{
						current.InitSTriangle(newgroup.get(),triangle_index);
						}
					}
					else
					{
						current.InitTriangle(newgroup.get(),triangle_index);
					}

					if(m_ReadColorIndex)
					{
						(*itmat).second.m_Material->setValue(LABEL_TO_ID(MaterialColorEnabled),true);
					}
					newgroup->addItem((CMSP&)(*itmat).second.m_Material);

					newmesh->addItem((CMSP&)newgroup);
				}
			}
			triangle_index+=current.m_ReadFacetCount;
			++itgroup;
		}

		// no normal in file, compute them
		if(needNormalComputation)
		{
			newmesh->RecomputeNormalsFast();
		}

		newmesh->setValue(LABEL_TO_ID(DynamicInit), true);
		newmesh->Init();
		newmesh->setValue(LABEL_TO_ID(DynamicInit), false);

		if(m_MeshList.size()==0)
		{
			m_FirstMesh=newmesh.get();
		}

		m_MeshList.push_back(newmesh);

	#ifdef _DEBUG
		//printf("%d vertex read\n",m_ReadVertexIndex);
	#endif
	}
}


void	OBJImport::FacetGroup::InitTriangle(MeshItemGroup* mig,int startTriangleIndex)
{
	Mesh::F_Triangle* mF_Triangle = new Mesh::F_Triangle[m_ReadFacetCount];
	mig->mFirstTriangle = mF_Triangle;
	mig->mTriangleType=eF_Triangle;
	mig->mTriangleSize = sizeof(Mesh::F_Triangle);
	
	unsigned int i;
	for(i=0;i<m_ReadFacetCount;i++)
	{
		Triangle* current=&((*m_ReadFacetBuffer)[i]);
		mF_Triangle[i].a=current->indexes[0].iv-1;
		mF_Triangle[i].b=current->indexes[1].iv-1;
		mF_Triangle[i].c=current->indexes[2].iv-1;
		mF_Triangle[i].N=startTriangleIndex+i;
	}
}
void	OBJImport::FacetGroup::InitSTriangle(MeshItemGroup* mig,int startTriangleIndex)
{
	Mesh::S_Triangle* mS_Triangle = new Mesh::S_Triangle[m_ReadFacetCount];
	mig->mFirstTriangle = mS_Triangle;
	mig->mTriangleSize = sizeof(Mesh::S_Triangle);
	mig->mTriangleType=eS_Triangle;

	unsigned int i;
	for(i=0;i<m_ReadFacetCount;i++)
	{
		Triangle* current=&((*m_ReadFacetBuffer)[i]);
		mS_Triangle[i].a=current->indexes[0].iv-1;
		mS_Triangle[i].b=current->indexes[1].iv-1;
		mS_Triangle[i].c=current->indexes[2].iv-1;
		mS_Triangle[i].Na=current->indexes[0].in-1;
		mS_Triangle[i].Nb=current->indexes[1].in-1;
		mS_Triangle[i].Nc=current->indexes[2].in-1;
	}
}

void	OBJImport::FacetGroup::InitSGTriangle(MeshItemGroup* mig,int startTriangleIndex)
{
	Mesh::SG_Triangle* mSG_Triangle = new Mesh::SG_Triangle[m_ReadFacetCount];
	mig->mFirstTriangle = mSG_Triangle;
	mig->mTriangleSize = sizeof(Mesh::SG_Triangle);
	mig->mTriangleType=eSG_Triangle;

	unsigned int i;
	for(i=0;i<m_ReadFacetCount;i++)
	{
		Triangle* current=&((*m_ReadFacetBuffer)[i]);
		mSG_Triangle[i].a=current->indexes[0].iv-1;
		mSG_Triangle[i].b=current->indexes[1].iv-1;
		mSG_Triangle[i].c=current->indexes[2].iv-1;
		// colors are at same index as vertex
		mSG_Triangle[i].Ca=current->indexes[0].iv-1;
		mSG_Triangle[i].Cb=current->indexes[1].iv-1;
		mSG_Triangle[i].Cc=current->indexes[2].iv-1;
		mSG_Triangle[i].Na=current->indexes[0].in-1;
		mSG_Triangle[i].Nb=current->indexes[1].in-1;
		mSG_Triangle[i].Nc=current->indexes[2].in-1;
	}
}

void	OBJImport::FacetGroup::InitTTriangle(MeshItemGroup* mig,int startTriangleIndex)
{
	Mesh::TF_Triangle<1>* mFT_Triangle = new Mesh::TF_Triangle<1>[m_ReadFacetCount];
	mig->mFirstTriangle = mFT_Triangle;
	mig->mTriangleSize = sizeof(Mesh::TF_Triangle<1>);
	mig->mTriangleType = eTF_Triangle;

	unsigned int i;
	for(i=0;i<m_ReadFacetCount;i++)
	{
		Triangle* current=&((*m_ReadFacetBuffer)[i]);
		mFT_Triangle[i].Ta[0]=current->indexes[0].it-1;
		mFT_Triangle[i].Tb[0]=current->indexes[1].it-1;
		mFT_Triangle[i].Tc[0]=current->indexes[2].it-1;
		mFT_Triangle[i].a=current->indexes[0].iv-1;
		mFT_Triangle[i].b=current->indexes[1].iv-1;
		mFT_Triangle[i].c=current->indexes[2].iv-1;
		mFT_Triangle[i].N=startTriangleIndex+i;
	}
}
void	OBJImport::FacetGroup::InitSTTriangle(MeshItemGroup* mig,int startTriangleIndex)
{
	Mesh::TS_Triangle<1>* mTS_Triangle = new Mesh::TS_Triangle<1>[m_ReadFacetCount];
	mig->mFirstTriangle = mTS_Triangle;
	mig->mTriangleSize = sizeof(Mesh::TS_Triangle<1>);
	mig->mTriangleType = eTS_Triangle;

	unsigned int i;
	for(i=0;i<m_ReadFacetCount;i++)
	{
		Triangle* current=&((*m_ReadFacetBuffer)[i]);
		mTS_Triangle[i].Ta[0]=current->indexes[0].it-1;
		mTS_Triangle[i].Tb[0]=current->indexes[1].it-1;
		mTS_Triangle[i].Tc[0]=current->indexes[2].it-1;
		mTS_Triangle[i].a=current->indexes[0].iv-1;
		mTS_Triangle[i].b=current->indexes[1].iv-1;
		mTS_Triangle[i].c=current->indexes[2].iv-1;
		mTS_Triangle[i].Na=current->indexes[0].in-1;
		mTS_Triangle[i].Nb=current->indexes[1].in-1;
		mTS_Triangle[i].Nc=current->indexes[2].in-1;
	}

}

void	OBJImport::StartObj(AsciiParserUtils& line)
{
	if(m_ReadVertexIndex)
	{
		createObjectFromReadedData();
	}

	AsciiParserUtils word(line);
	if(line.GetWord(word))
	{
		m_CurrentObjectName=word;
	}
	ReinitReadBuffers();
}
void	OBJImport::StartGroup(AsciiParserUtils& line)
{
}
void	OBJImport::ParseVertex(AsciiParserUtils& line)
{
	Point3D& point3Dtoadd=(*m_ReadVertexBuffer)[m_ReadVertexIndex];
	++m_ReadVertexIndex;
	//float x,y,z;
	line.ReadFloat(point3Dtoadd.x);
	line.ReadFloat(point3Dtoadd.y);
	line.ReadFloat(point3Dtoadd.z);
	/*toadd.x=x;
	toadd.y=y;
	toadd.z=z;*/

	/*if((m_ReadVertexIndex&0xff) == 0xff)
	{
		printf("%d vertex read\n",m_ReadVertexIndex);
	}*/

	// retreive vertex color if present
	AsciiParserUtils	colors(line);
	if(line.GetTrailingPart(colors))
		if(colors.length())
		{
			Vector4D& vector4Dtoadd=(*m_ReadColorBuffer)[m_ReadColorIndex];
			++m_ReadColorIndex;
			//float x,y,z;
			colors.ReadFloat(vector4Dtoadd.x);
			colors.ReadFloat(vector4Dtoadd.y);
			colors.ReadFloat(vector4Dtoadd.z);
			/*toadd.x=x;
			toadd.y=y;
			toadd.z=z;*/
			vector4Dtoadd.w=1.0f;
		}

}

void	OBJImport::ParseFacet(AsciiParserUtils& line)
{
	kstl::vector<IndexTrio>	indexlist;

	int indexv;
	bool	found=true;
	bool	foundt=true;
	bool	foundn=true;
	while(found)
	{
		found=line.ReadInt(indexv);
		if(found)
		{
			IndexTrio	toAdd;
			toAdd.iv=indexv;
			(line[line.GetPosition()]=='/')?foundt&=line.GoAfterNextSeparator('/'):foundt&=false;
			if(foundt)
				foundt&=line.ReadInt(toAdd.it);
			(line[line.GetPosition()]=='/')?foundn&=line.GoAfterNextSeparator('/'):foundn&=false;
			if(foundn)
				foundn&=line.ReadInt(toAdd.in);
		
			indexlist.push_back(toAdd);
		}
	}

	m_CurrentFacetGroup->m_HasNormal&=foundn;
	m_CurrentFacetGroup->m_HasTextCoords&=foundt;

	if(indexlist.size() == 3)
	{
		Triangle* current=&(*m_CurrentFacetGroup->m_ReadFacetBuffer)[m_CurrentFacetGroup->m_ReadFacetCount++];
		current->indexes[0]=indexlist.at(0);
		current->indexes[1]=indexlist.at(1);
		current->indexes[2]=indexlist.at(2);
	}
	else if(indexlist.size() == 4) // quad
	{
		Triangle* current=&(*m_CurrentFacetGroup->m_ReadFacetBuffer)[m_CurrentFacetGroup->m_ReadFacetCount++];
		current->indexes[0]=indexlist.at(0);
		current->indexes[1]=indexlist.at(1);
		current->indexes[2]=indexlist.at(2);
		current=&(*m_CurrentFacetGroup->m_ReadFacetBuffer)[m_CurrentFacetGroup->m_ReadFacetCount++];
		current->indexes[0]=indexlist.at(0);
		current->indexes[1]=indexlist.at(2);
		current->indexes[2]=indexlist.at(3);
	}
}

void	OBJImport::ParseTextureCoord(AsciiParserUtils& line)
{
	Point3D& toadd=(*m_ReadTextCoordsBuffer)[m_ReadTextCoordIndex];
	++m_ReadTextCoordIndex;
	//float tx,ty,tz;
	line.ReadFloat(toadd.x);
	line.ReadFloat(toadd.y);
	line.ReadFloat(toadd.z);
	toadd.y=1.0f-toadd.y;
	/*toadd.x=tx;
	toadd.y=ty;
	toadd.z=tz;*/
}
void	OBJImport::ParseNormal(AsciiParserUtils& line)
{
	Point3D& toadd=(*m_ReadNormalBuffer)[m_ReadNormalIndex];
	++m_ReadNormalIndex;
	//float nx,ny,nz;
	line.ReadFloat(toadd.x);
	line.ReadFloat(toadd.y);
	line.ReadFloat(toadd.z);
	/*toadd.x=nx;
	toadd.y=ny;
	toadd.z=nz;*/
}


void	OBJImport::MTLImport(const kstl::string& name)
{
	SP<FilePathManager>	pathManager=KigsCore::GetSingleton("FilePathManager");

	kstl::string fullfilename;
	SmartPointer<FileHandle> fullfilenamehandle;


	fullfilenamehandle = pathManager->FindFullName(name);

	if (fullfilenamehandle)
	{	
		u64 filelen;
		CoreRawBuffer* rawbuffer=ModuleFileManager::LoadFileAsCharString(fullfilenamehandle.get(),filelen,1);
		if (!rawbuffer)
		{
			kstl::string shortname, ext, path;
			RetreiveShortNameAndExt(m_FileName.const_ref(), shortname, ext, path);
			fullfilenamehandle = pathManager->FindFullName(path + name);
			rawbuffer = ModuleFileManager::LoadFileAsCharString(fullfilenamehandle.get(), filelen,1);
		}

		if (rawbuffer)
		{
			unsigned char* data=reinterpret_cast<unsigned char*>(rawbuffer->buffer());
			m_currentMatRead=0;
			AsciiParserUtils FileParser(rawbuffer);
			AsciiParserUtils line(FileParser);
			while(FileParser.GetLine(line,true))
			{
				AsciiParserUtils word(line);
				if(line.GetWord(word))
				{
					if(word[0] != '#')
					{
						kstl::string key=word;
						if(key == "newmtl")
						{
							AsciiParserUtils matname(line);
							if(line.GetWord(matname))
							{
								m_currentMatRead=&m_materialList[matname];
								m_currentMatRead->name=matname;
							}
						}
						else if(key == "Ka")
						{
							line.ReadFloat(m_currentMatRead->Ka[0]);
							line.ReadFloat(m_currentMatRead->Ka[1]);
							line.ReadFloat(m_currentMatRead->Ka[2]);
						}
						else if(key == "Kd")
						{
							line.ReadFloat(m_currentMatRead->Kd[0]);
							line.ReadFloat(m_currentMatRead->Kd[1]);
							line.ReadFloat(m_currentMatRead->Kd[2]);
						}
						else if(key == "Ks")
						{
							line.ReadFloat(m_currentMatRead->Ks[0]);
							line.ReadFloat(m_currentMatRead->Ks[1]);
							line.ReadFloat(m_currentMatRead->Ks[2]);
						}
						else if((key == "d") || (key == "Tr"))
						{
							line.ReadFloat(m_currentMatRead->d);
						}
						else if(key == "Ns")
						{
							line.ReadFloat(m_currentMatRead->Ns);
						}
						else if(key == "illum")
						{
							line.ReadInt(m_currentMatRead->illum);
						}
						else if(key == "map_Kd")
						{
							// just read texture name for now so skip previous params
							AsciiParserUtils textureName(line);
							if (line.GetTrailingPart(textureName))
							{
								if (m_currentMatRead != nullptr && static_cast<kstl::string>(textureName) != "")
								{
									m_currentMatRead->textureName = textureName;
									m_TextureList.push_back(textureName);
								}
							}
						}
					}
				}
			}
	
			rawbuffer->Destroy();
		}
	}
}

OBJImport::ReadMaterial::ReadMaterial() : d(1.0f),Ns(1.0f),name(""),illum(1),textureName(""),m_Material(nullptr)
{
	Ka[0]=Ka[1]=Ka[2]=0.2f;
	Kd[0]=Kd[1]=Kd[2]=0.8f;
	Ks[0]=Ks[1]=Ks[2]=0.0f;
}

OBJImport::ReadMaterial::~ReadMaterial()
{
	if(m_Material)
	{
		m_Material->Destroy();
	}
}

void	OBJImport::ReadMaterial::Init()
{
	if(!m_Material)
	{
		m_Material =	KigsCore::GetInstanceOf(name,"Material");
#ifndef NO_MULTISTAGE_RENDERING
#ifndef DO_MULTISTAGE_RENDERING
#error	"missing ScenegraphDefines.h"
#endif
		SP<MaterialStage> MatStage = KigsCore::GetInstanceOf(name + "MatStage","MaterialStage");
		if(textureName != "")
		{
			SP<TextureFileManager>	fileManager=KigsCore::GetSingleton("TextureFileManager");
			SP<Texture> Tex=fileManager->GetTexture(textureName,false);
			Tex->setValue(LABEL_TO_ID(ForcePow2),true);
			Tex->setValue(LABEL_TO_ID(HasMipmap), true);
			Tex->Init();

			MatStage->addItem((CMSP&)Tex);
		}

		MatStage->Init();

		m_Material->addItem((CMSP&)MatStage);
		MatStage->Destroy();
#else
		if (textureName != "")
		{
			TextureFileManager*	fileManager = (TextureFileManager*)KigsCore::GetSingleton("TextureFileManager");
			Texture* Tex = fileManager->GetTexture(textureName, false);
			Tex->setValue(LABEL_TO_ID(ForcePow2), true);
			Tex->Init();

			m_Material->addItem(Tex);
			Tex->Destroy();
		}

#endif
		
		m_Material->SetAmbientColor(Ka[0],Ka[1],Ka[2]);
		m_Material->SetDiffuseColor(Kd[0],Kd[1],Kd[2]);
		m_Material->SetSpecularColor(Ks[0],Ks[1],Ks[2]);
		m_Material->setValue(LABEL_TO_ID(Shininess),Ns);
		m_Material->setValue(LABEL_TO_ID(Transparency),d);
		
		m_Material->Init();
	}
}

#ifdef WIN32
void OBJImport::ExportResult()
{
	kstl::string shortname, ext, path;
	RetreiveShortNameAndExt(m_FileName, shortname, ext, path);

	SP<ModernMesh> bigMesh = KigsCore::GetInstanceOf(shortname, "ModernMesh");
	for (std::vector<CMSP>::iterator it = m_MeshList.begin(); it != m_MeshList.end(); ++it)
	{
		bigMesh->addItem((CMSP&)*it);
	}
	Export(m_ExportPath.const_ref() + shortname + ".xml", bigMesh.get(), true);
	for (kstl::list<kstl::string>::iterator it = m_TextureList.begin(); it != m_TextureList.end(); it++)
	{
		std::string exportp = (m_ExportPath.const_ref() + (*it)).c_str();
		std::ofstream outfile(exportp, std::ios_base::binary);
		std::filebuf infile;

		if (!outfile || !infile.open((path + (*it)).c_str(), std::ios_base::binary | std::ios_base::in)) {
			break;
		}

		typedef std::filebuf::traits_type traits;

		if (traits::eq_int_type(infile.sgetc(), traits::eof())) {
			break;
		}

		if (!(outfile << &infile)) {
			break;
		}

		if (infile.sgetc() != traits::eof()) {
			break;
		}
	}
}
#endif //WIN32

void	OBJImport::RetreiveShortNameAndExt(const kstl::string& filename, kstl::string& shortname, kstl::string& fileext, kstl::string& filepath)
{
	int pos = static_cast<int>(filename.rfind("/")) + 1;
	int pos1 = static_cast<int>(filename.rfind("\\")) + 1;

	if (pos1 > pos)
	{
		pos = pos1;
	}

	bool	filenameIsShortfilename = false;
	filepath = filename.substr(0, pos);
	if (pos == 0)
	{
		shortname = filename;
		filenameIsShortfilename = true;
	}
	else
	{
		shortname = filename.substr(static_cast<unsigned int>(pos), filename.length() - pos);
	}

	fileext = "";
	pos = static_cast<int>(shortname.rfind(".")) + 1;
	if (pos)
	{
		fileext.append(shortname, static_cast<unsigned int>(pos), shortname.length() - pos);
		shortname = shortname.substr(0, pos - 1);
	}

	// lower case fileext
	std::transform(fileext.begin(), fileext.end(), fileext.begin(), ::tolower);


}



