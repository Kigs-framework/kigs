#include "PrecompiledHeaders.h"

#include <stdlib.h>
#include "KMesh.h"
#include "BinMeshLoader.h"
#ifdef WIN32
#include "STLMeshLoader.h"
#include "ASEMeshLoader.h"
#endif
#include "FilePathManager.h"
#include "Core.h"
#include "Material.h"

using namespace Kigs::Draw;

IMPLEMENT_CLASS_INFO(MeshItemGroup)

IMPLEMENT_CLASS_INFO(Mesh)

/*
 *	Constructor
 */
Mesh::Mesh(const std::string& name,CLASS_NAME_TREE_ARG) : HDrawable(name,PASS_CLASS_NAME_TREE_ARG)
#if _DEBUG
, mShowVertex(*this,"ShowVertex",false)
#endif
{
	setInitParameter("FileName", true);
	mVertexArray = 0;
	mNormalArray = 0;
	mColorArray = 0;
	mTexArray = 0;

	mBoundingBox.m_Min = { 0.0f,0.0f,0.0f };
	mBoundingBox.m_Max = { -1.0f, -1.0f, -1.0f };
}

/*
 *	Destructor
 */
Mesh::~Mesh()
{
	if (mVertexArray) 
		delete[](mVertexArray);
	mVertexArray = NULL;
	if (mNormalArray) 
		delete[](mNormalArray);
	mNormalArray = NULL;

	if (mTexArray) 
		delete[](mTexArray);
	mTexArray = NULL;
	if (mColorArray) 
		delete[](mColorArray);
	mColorArray = NULL;
}

#ifdef WIN32
void Mesh::ExportMeshTo(std::string _pathDirectory, std::string _fileName)
{
	BinMeshLoader loader;
	loader.ExportFile(this,_pathDirectory, _fileName);
}
#endif
MeshItemGroup::~MeshItemGroup()
{
	if(mFirstTriangle)
	{
		delete[] ((Mesh::Triangle*)mFirstTriangle);
	}
}

void	Mesh::InitModifiable()
{
	HDrawable::InitModifiable();
	if(_isInit)
	{
		if (!mDynamicInit)
		{
			auto pathManager = KigsCore::Singleton<File::FilePathManager>();

			std::string fullfilename;
			SmartPointer<File::FileHandle> fullfilenamehandle = pathManager->FindFullName(mFileName);
			if (fullfilenamehandle)
			{
				fullfilename = fullfilenamehandle->mFullFileName;
				std::string	extension=mFileName;

				extension=extension.substr(extension.rfind("."));

				// test file extension
				if(extension==".ase")
				{
#ifdef WIN32
					ASEMeshLoader loader;

					if(loader.ImportFile(this,fullfilename)!=0)
					{
						UninitModifiable();
					}
					else
					{
						ProtectedInit();
					}
#else
					UninitModifiable();
#endif
				}
				else if(extension==".mesh")
				{
					BinMeshLoader loader;

					if(loader.ImportFile(this,fullfilename)!=0)
					{
						KIGS_WARNING("Can not open mesh file :"+mFileName,2);
						UninitModifiable();
					}
					else
					{
						ProtectedInit();
					}
				}
	#ifdef WIN32
				else if(extension==".stl")
				{
					STLMeshLoader	loader;
					if(loader.ImportFile(this,fullfilename)!=0)
					{
						KIGS_WARNING("Can not open mesh file :"+mFileName,2);
						UninitModifiable();
					}
					else
					{
						ProtectedInit();
					}
				}
	#endif
			}
			else
			{
				KIGS_WARNING("Can not open mesh file :"+mFileName,2); 
				UninitModifiable();
			}
		}
		//We are in dynamic mode
		else
		{
			ProtectedInit();
		}
		
    	InitBoundingBox();
	}
}

void  Mesh::InitBoundingBox()
{
	if((_isInit)&&(mVertexCount))
	{
		mBoundingBox.Init(mVertexArray,(int)mVertexCount);
	}
}

/*
 *	Compute the normal at triangle vertices ponderated by the angle
 */
void Mesh::Triangle::NormalAngle(const v3f* VertexArray, v3f &Na, v3f &Nb,v3f &Nc)
{
	v3f u,v,w,n;
	u = VertexArray[b]-VertexArray[a];
	v = VertexArray[c]-VertexArray[b];
	w = VertexArray[a]-VertexArray[c];
	normalize(u);
	normalize(v);
	normalize(w);

	n=normalize(cross(u,v));
	
	Na =  (float)acosf(fabsf(dot(u,w))) * n;
	Nb =  (float)acosf(fabsf(dot(v,u))) * n;
	Nc =  (float)acosf(fabsf(dot(w,v))) * n;
}


void Mesh::TexCoord::Load(File::BufferedFile* currentfile)
{
	fread(&u,sizeof(u),1,currentfile);
	fread(&v,sizeof(v),1,currentfile);
}

void Mesh::Triangle::Load(File::BufferedFile* currentfile)
{
	fread(&a,sizeof(a),1,currentfile);
	fread(&b,sizeof(b),1,currentfile);
	fread(&c,sizeof(c),1,currentfile);
}


void Mesh::ThreeColorTriangleStruct::Load(File::BufferedFile* currentfile)
{
	fread(&Ca,sizeof(Ca),1,currentfile);
	fread(&Cb,sizeof(Cb),1,currentfile);
	fread(&Cc,sizeof(Cc),1,currentfile);
}

void Mesh::OneColorTriangleStruct::Load(File::BufferedFile* currentfile)
{
	fread(&C,sizeof(C),1,currentfile);
}

void Mesh::ThreeNormalTriangleStruct::Load(File::BufferedFile* currentfile)
{
	fread(&Na,sizeof(Na),1,currentfile);
	fread(&Nb,sizeof(Nb),1,currentfile);
	fread(&Nc,sizeof(Nc),1,currentfile);
}

void Mesh::OneNormalTriangleStruct::Load(File::BufferedFile* currentfile)
{
	fread(&N,sizeof(N),1,currentfile);
}


int Mesh::getVertexCount()
{
	return (int)mVertexCount;
}

int Mesh::getTriangleCount()
{
	int Count = 0;
	std::vector<ModifiableItemStruct>::const_iterator it;

	for (it=getItems().begin();it!=getItems().end();++it)
	{
		if((*it).mItem->isSubType(MeshItemGroup::mClassID))
		{
			Count += it->mItem->as<MeshItemGroup>()->mTriangleCount;
		}
	}
	return Count;
}

bool Mesh::getVertex(unsigned int index, v3f &Pt)
{
	if (index>=mVertexCount) return false;
	Pt = mVertexArray[index];
	return true;
}

void Mesh::SetVertex(unsigned int index, const v3f &v)
{
	KIGS_ASSERT(index<mVertexCount);
	mVertexArray[index] = v;
	mVertexNeedUpdate = true;
}


void Mesh::SetColor(unsigned int index, const v4f &v)
{
	KIGS_ASSERT(index<mColorCount);
	mColorArray[index]=v;
	mColorNeedUpdate=true;	
}

void Mesh::SetNormal(unsigned int index, const v3f &v)
{
	KIGS_ASSERT(index<mNormalCount);
	mNormalArray[index]=v;
	mNormalNeedUpdate=true;
}

void Mesh::SetTexCoord(unsigned int index, const TexCoord &v)
{
	KIGS_ASSERT(index<mTexCoordCount);
	mTexArray[index]=v;
	mTexCoordNeedUpdate=true;
		
}


bool Mesh::getTriangle(int index, int &a, int &b, int &c)
{
	//int g = 0;
	Mesh::Triangle *Tr=0;

	std::vector<ModifiableItemStruct>::const_iterator it;
	for (it=getItems().begin();it!=getItems().end();++it)
	{
		auto mesh_group = it->mItem->as<MeshItemGroup>();
		if((*it).mItem->isSubType(MeshItemGroup::mClassID))
		{
			if(index>= mesh_group->mTriangleCount)
			{
				index-= mesh_group->mTriangleCount;
			}
			else
			{
				Tr = (Mesh::Triangle *)(((char*)mesh_group->mFirstTriangle) + mesh_group->mTriangleSize*index);
				break;
			}
			if(index<0)
			{
				return false;
			}
		}
	}

	if(Tr==0)
	{
		return false;
	}

	a = (int)Tr->a;
	b = (int)Tr->b;
	c = (int)Tr->c;
	return true;
}

bool Mesh::CopyVertexAndTriangle(int &VCount, int &TCount, v3f *&VArray, int *&IndexArray)
{
	int j,k;

	TCount = getTriangleCount();

	if (mVertexCount==0 || TCount==0) 
	{
		return false;
	}

	VCount = (int)mVertexCount;
	VArray = new v3f[VCount];
	memcpy(VArray,mVertexArray,VCount*sizeof(v3f));

	IndexArray = new int[TCount*3];

	k=0;

	std::vector<ModifiableItemStruct>::const_iterator it;
	for (it=getItems().begin();it!=getItems().end();++it)
	{
		if((*it).mItem->isSubType(MeshItemGroup::mClassID))
		{
			auto mesh_group = it->mItem->as<MeshItemGroup>();
			Mesh::Triangle *Tr=(Mesh::Triangle *)(mesh_group->mFirstTriangle) ;

			int	sizeoftriangle=(mesh_group->mTriangleSize);

			for (j=0;j<(mesh_group->mTriangleCount);j++)
			{
				Mesh::Triangle* CurrentT=(Mesh::Triangle*)((unsigned char*)Tr+sizeoftriangle*j);

				IndexArray[k*3+0] = (int)CurrentT->a;
				IndexArray[k*3+1] = (int)CurrentT->b;
				IndexArray[k*3+2] = (int)CurrentT->c;
				k++;
			}
		}
	}

	return true;
}

void	Mesh::DoPreDraw(TravState* state)
{
	

	if(mVertexNeedUpdate||mColorNeedUpdate||mTexCoordNeedUpdate||mNormalNeedUpdate)
	{
		UpdateMesh();
		mColorNeedUpdate=false;
		mNormalNeedUpdate=false;
		mTexCoordNeedUpdate=false;
		mVertexNeedUpdate=false;
	}

	HDrawable::DoPreDraw(state);
}

bool	Mesh::GetVertexPointer(CoreModifiable* sender,std::vector<CoreModifiableAttribute*>& params,void* privateParams)
{
	CoreModifiableAttribute*	result=0;
	CoreModifiableAttribute*	buffersize=0;

	std::vector<CoreModifiableAttribute*>::const_iterator ci;
	for(ci=params.begin();ci!=params.end();++ci)
	{
		CoreModifiableAttribute* current=(*ci);
		if (current->id() == KigsID("result")._id)
		{
			result=current;
		}
		if (current->id() == KigsID("buffersize")._id)
		{
			buffersize=current;
		}
	}

	if(result)
	{
		if(buffersize)
		{
			buffersize->setValue((int)mVertexCount,nullptr);
		}
		result->setValue((u64)mVertexArray, nullptr);
		return true;
	}

	return false;
}

bool	Mesh::GetColorPointer(CoreModifiable* sender,std::vector<CoreModifiableAttribute*>& params,void* privateParams)
{
	CoreModifiableAttribute*	result=0;

	std::vector<CoreModifiableAttribute*>::const_iterator ci;
	for(ci=params.begin();ci!=params.end();++ci)
	{
		CoreModifiableAttribute* current=(*ci);
		if (current->id() == KigsID("result")._id)
		{
			result=current;
			break;
		}
	}

	if(result)
	{
		result->setValue((u64)mColorArray, nullptr);
		return true;
	}

	return false;
}

bool	Mesh::GetNormalPointer(CoreModifiable* sender,std::vector<CoreModifiableAttribute*>& params,void* privateParams)
{
	CoreModifiableAttribute*	result=0;

	std::vector<CoreModifiableAttribute*>::const_iterator ci;
	for(ci=params.begin();ci!=params.end();++ci)
	{
		CoreModifiableAttribute* current=(*ci);
		if (current->id() == KigsID("result")._id)
		{
			result=current;
			break;
		}
	}

	if(result)
	{
		result->setValue((u64)mNormalArray, nullptr);
		return true;
	}

	return false;
}	

bool	Mesh::GetTexCoordPointer(CoreModifiable* sender,std::vector<CoreModifiableAttribute*>& params,void* privateParams)
{
	CoreModifiableAttribute*	result=0;

	std::vector<CoreModifiableAttribute*>::const_iterator ci;
	for(ci=params.begin();ci!=params.end();++ci)
	{
		CoreModifiableAttribute* current=(*ci);
		if (current->id() == KigsID("result")._id)
		{
			result=current;
			break;
		}
	}

	if(result)
	{
		result->setValue((u64)mTexArray, nullptr);
		return true;
	}

	return false;
}



// rebuild all normals from triangles 
void	Mesh::RecomputeNormals()
{
	v3f na,nb,nc;
	unsigned int i;

	if(mNormalCount == 0)
	{
		return;
	}
	
	memset(mNormalArray,0,mNormalCount*sizeof(v3f));
	
	std::vector<ModifiableItemStruct>::const_iterator it;

	for (it=getItems().begin();it!=getItems().end();++it)
	{
		if((*it).mItem->isSubType(MeshItemGroup::mClassID))
		{
			MeshItemGroup* currentGroup= (*it).mItem->as<MeshItemGroup>();

			if( ((unsigned int)currentGroup->mTriangleType) & 1 ) // check if smooth triangle
			{
				Mesh::S_Triangle *Tr=(Mesh::S_Triangle *)currentGroup->mFirstTriangle;
			
				for(i=0;i<(unsigned int)currentGroup->mTriangleCount;i++)
				{
					Tr->NormalAngle(mVertexArray,na,nb,nc);

					mNormalArray[Tr->Na]+=na;
					mNormalArray[Tr->Nb]+=nb;
					mNormalArray[Tr->Nc]+=nc;

					Tr=(Mesh::S_Triangle *)(((unsigned char*)Tr)+currentGroup->mTriangleSize);
				}
			}
			else // flat triangle
			{
				Mesh::F_Triangle *Tr=(Mesh::F_Triangle *)currentGroup->mFirstTriangle;
			
				for (i = 0; i<(unsigned int)currentGroup->mTriangleCount; i++)
				{
					Tr->Normal(mVertexArray,na);

					mNormalArray[Tr->N]=-na;
					
					Tr=(Mesh::F_Triangle *)(((unsigned char*)Tr)+currentGroup->mTriangleSize);
				}
			}
		}
	}

	// then normalize all again
	for(i=0;i<mNormalCount;i++)
	{
		normalize(mNormalArray[i]);
	}

}

// recompute all normals with faster but less precise algorithm
void	Mesh::RecomputeNormalsFast()
{
	v3f na;
	int i;

	if(mNormalCount == 0)
	{
		return;
	}
	
	memset(mNormalArray,0,mNormalCount*sizeof(v3f));
	
	std::vector<ModifiableItemStruct>::const_iterator it;

	for (it=getItems().begin();it!=getItems().end();++it)
	{
		if((*it).mItem->isSubType(MeshItemGroup::mClassID))
		{
			MeshItemGroup* currentGroup= (*it).mItem->as<MeshItemGroup>();

			if( ((unsigned int)currentGroup->mTriangleType) & 1 ) // check if smooth triangle
			{
				Mesh::S_Triangle *Tr=(Mesh::S_Triangle *)currentGroup->mFirstTriangle;
			
				for(i=0;i<currentGroup->mTriangleCount;i++)
				{
					Tr->Normal(mVertexArray,na);

					mNormalArray[Tr->Na]-=na;
					mNormalArray[Tr->Nb]-=na;
					mNormalArray[Tr->Nc]-=na;

					Tr=(Mesh::S_Triangle *)(((unsigned char*)Tr)+currentGroup->mTriangleSize);
				}
			}
			else // flat triangle
			{
				Mesh::F_Triangle *Tr=(Mesh::F_Triangle *)currentGroup->mFirstTriangle;
			
				for(i=0;i<currentGroup->mTriangleCount;i++)
				{
					Tr->Normal(mVertexArray,na);

					mNormalArray[Tr->N]-=na;
					
					Tr=(Mesh::F_Triangle *)(((unsigned char*)Tr)+currentGroup->mTriangleSize);
				}
			}
		}
	}

	// then normalize all again
	for (i = 0; i<(int)mNormalCount; i++)
	{
		normalize(mNormalArray[i]);
	}
}