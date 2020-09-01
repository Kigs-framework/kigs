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

IMPLEMENT_CLASS_INFO(MeshItemGroup)

IMPLEMENT_CLASS_INFO(Mesh)

/*
 *	Constructor
 */
Mesh::Mesh(const kstl::string& name,CLASS_NAME_TREE_ARG) : HDrawable(name,PASS_CLASS_NAME_TREE_ARG)
, mVertexNeedUpdate(*this,false,LABEL_AND_ID(VertexNeedUpdate),false)
, mColorNeedUpdate(*this,false,LABEL_AND_ID(ColorNeedUpdate),false)
, mTexCoordNeedUpdate(*this,false,LABEL_AND_ID(TexCoordNeedUpdate),false)
, mNormalNeedUpdate(*this,false,LABEL_AND_ID(NormalNeedUpdate),false)
, mShareMaterial(*this,false,LABEL_AND_ID(ShareMaterial),true)
#if _DEBUG
, mShowVertex(*this,false,LABEL_AND_ID(ShowVertex),false)
#endif
, mWireMode(*this,false,LABEL_AND_ID(WireMode),false)
, mFileName(*this, true, LABEL_AND_ID(FileName))
, mDynamicInit(*this, false, LABEL_AND_ID(DynamicInit), false)
{
	mVertexArray = 0;
	mNormalArray = 0;
	mColorArray = 0;
	mTexArray = 0;

  mBoundingBox.m_Min.Set(KFLOAT_CONST(0.0f),KFLOAT_CONST(0.0f),KFLOAT_CONST(0.0f));
  mBoundingBox.m_Max.Set(KFLOAT_CONST(-1.0f),KFLOAT_CONST(-1.0f),KFLOAT_CONST(-1.0f));
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
void Mesh::ExportMeshTo(kstl::string _pathDirectory, kstl::string _fileName)
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
			auto& pathManager = KigsCore::Singleton<FilePathManager>();

			kstl::string fullfilename;
			SmartPointer<FileHandle> fullfilenamehandle = pathManager->FindFullName(mFileName);
			if (fullfilenamehandle)
			{
				fullfilename = fullfilenamehandle->mFullFileName;
				kstl::string	extension=mFileName;

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
						KIGS_WARNING("Can not open mesh file :"+mFileName.const_ref(),2);
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
						KIGS_WARNING("Can not open mesh file :"+mFileName.const_ref(),2);
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
				KIGS_WARNING("Can not open mesh file :"+(kstl::string&)mFileName,2); 
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
void Mesh::Triangle::NormalAngle(const Point3D* VertexArray, Vector3D &Na, Vector3D &Nb,Vector3D &Nc)
{
	Vector3D u,v,w,n;
	u = VertexArray[b]-VertexArray[a];
	v = VertexArray[c]-VertexArray[b];
	w = VertexArray[a]-VertexArray[c];
	u.Normalize();
	v.Normalize();
	w.Normalize();

	n.CrossProduct(u,v);
	n.Normalize();
	Na =  (kfloat)acosf(fabsf(Dot(u,w))) * n;
	Nb =  (kfloat)acosf(fabsf(Dot(v,u))) * n;
	Nc =  (kfloat)acosf(fabsf(Dot(w,v))) * n;
}


void Mesh::TexCoord::Load(BufferedFile* currentfile)
{
	fread(&u,sizeof(u),1,currentfile);
	fread(&v,sizeof(v),1,currentfile);
}

void Mesh::Triangle::Load(BufferedFile* currentfile)
{
	fread(&a,sizeof(a),1,currentfile);
	fread(&b,sizeof(b),1,currentfile);
	fread(&c,sizeof(c),1,currentfile);
}


void Mesh::ThreeColorTriangleStruct::Load(BufferedFile* currentfile)
{
	fread(&Ca,sizeof(Ca),1,currentfile);
	fread(&Cb,sizeof(Cb),1,currentfile);
	fread(&Cc,sizeof(Cc),1,currentfile);
}

void Mesh::OneColorTriangleStruct::Load(BufferedFile* currentfile)
{
	fread(&C,sizeof(C),1,currentfile);
}

void Mesh::ThreeNormalTriangleStruct::Load(BufferedFile* currentfile)
{
	fread(&Na,sizeof(Na),1,currentfile);
	fread(&Nb,sizeof(Nb),1,currentfile);
	fread(&Nc,sizeof(Nc),1,currentfile);
}

void Mesh::OneNormalTriangleStruct::Load(BufferedFile* currentfile)
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
	kstl::vector<ModifiableItemStruct>::const_iterator it;

	for (it=getItems().begin();it!=getItems().end();++it)
	{
		if((*it).mItem->isSubType(MeshItemGroup::mClassID))
		{
			Count += ((SP<MeshItemGroup>&)(*it).mItem)->mTriangleCount;
		}
	}
	return Count;
}

bool Mesh::getVertex(unsigned int index, Point3D &Pt)
{
	if (index>=mVertexCount) return false;
	Pt = mVertexArray[index];
	return true;
}

void Mesh::SetVertex(unsigned int index, const Point3D &v)
{
	KIGS_ASSERT(index<mVertexCount);
	mVertexArray[index] = v;
	mVertexNeedUpdate = true;
}


void Mesh::SetColor(unsigned int index, const Vector4D &v)
{
	KIGS_ASSERT(index<mColorCount);
	mColorArray[index]=v;
	mColorNeedUpdate=true;	
}

void Mesh::SetNormal(unsigned int index, const Vector3D &v)
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

	kstl::vector<ModifiableItemStruct>::const_iterator it;
	for (it=getItems().begin();it!=getItems().end();++it)
	{
		if((*it).mItem->isSubType(MeshItemGroup::mClassID))
		{
			if(index>=((SP<MeshItemGroup>&)(*it).mItem)->mTriangleCount)
			{
				index-=((SP<MeshItemGroup>&)(*it).mItem)->mTriangleCount;
			}
			else
			{
				Tr = (Mesh::Triangle *)(((char*)((SP<MeshItemGroup>&)(*it).mItem)->mFirstTriangle) + ((SP<MeshItemGroup>&)(*it).mItem)->mTriangleSize*index);
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

bool Mesh::CopyVertexAndTriangle(int &VCount, int &TCount, Point3D *&VArray, int *&IndexArray)
{
	int j,k;

	TCount = getTriangleCount();

	if (mVertexCount==0 || TCount==0) 
	{
		return false;
	}

	VCount = (int)mVertexCount;
	VArray = new Point3D[VCount];
	memcpy(VArray,mVertexArray,VCount*sizeof(Point3D));

	IndexArray = new int[TCount*3];

	k=0;

	kstl::vector<ModifiableItemStruct>::const_iterator it;
	for (it=getItems().begin();it!=getItems().end();++it)
	{
		if((*it).mItem->isSubType(MeshItemGroup::mClassID))
		{
			Mesh::Triangle *Tr=(Mesh::Triangle *)(((SP<MeshItemGroup>&)(*it).mItem)->mFirstTriangle) ;

			int	sizeoftriangle=(((SP<MeshItemGroup>&)(*it).mItem)->mTriangleSize);

			for (j=0;j<(((SP<MeshItemGroup>&)(*it).mItem)->mTriangleCount);j++)
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

bool	Mesh::GetVertexPointer(CoreModifiable* sender,kstl::vector<CoreModifiableAttribute*>& params,void* privateParams)
{
	CoreModifiableAttribute*	result=0;
	CoreModifiableAttribute*	buffersize=0;

	kstl::vector<CoreModifiableAttribute*>::const_iterator ci;
	for(ci=params.begin();ci!=params.end();++ci)
	{
		CoreModifiableAttribute* current=(*ci);
		if (current->getLabel() == "result")
		{
			result=current;
		}
		if (current->getLabel() == "buffersize")
		{
			buffersize=current;
		}
	}

	if(result)
	{
		if(buffersize)
		{
			buffersize->setValue((int)mVertexCount);
		}
		result->setValue((u64)mVertexArray);
		return true;
	}

	return false;
}

bool	Mesh::GetColorPointer(CoreModifiable* sender,kstl::vector<CoreModifiableAttribute*>& params,void* privateParams)
{
	CoreModifiableAttribute*	result=0;

	kstl::vector<CoreModifiableAttribute*>::const_iterator ci;
	for(ci=params.begin();ci!=params.end();++ci)
	{
		CoreModifiableAttribute* current=(*ci);
		if (current->getLabel() == "result")
		{
			result=current;
			break;
		}
	}

	if(result)
	{
		result->setValue((u64)mColorArray);
		return true;
	}

	return false;
}

bool	Mesh::GetNormalPointer(CoreModifiable* sender,kstl::vector<CoreModifiableAttribute*>& params,void* privateParams)
{
	CoreModifiableAttribute*	result=0;

	kstl::vector<CoreModifiableAttribute*>::const_iterator ci;
	for(ci=params.begin();ci!=params.end();++ci)
	{
		CoreModifiableAttribute* current=(*ci);
		if (current->getLabel() == "result")
		{
			result=current;
			break;
		}
	}

	if(result)
	{
		result->setValue((u64)mNormalArray);
		return true;
	}

	return false;
}	

bool	Mesh::GetTexCoordPointer(CoreModifiable* sender,kstl::vector<CoreModifiableAttribute*>& params,void* privateParams)
{
	CoreModifiableAttribute*	result=0;

	kstl::vector<CoreModifiableAttribute*>::const_iterator ci;
	for(ci=params.begin();ci!=params.end();++ci)
	{
		CoreModifiableAttribute* current=(*ci);
		if (current->getLabel() == "result")
		{
			result=current;
			break;
		}
	}

	if(result)
	{
		result->setValue((u64)mTexArray);
		return true;
	}

	return false;
}



// rebuild all normals from triangles 
void	Mesh::RecomputeNormals()
{
	Vector3D na,nb,nc;
	unsigned int i;

	if(mNormalCount == 0)
	{
		return;
	}
	
	memset(mNormalArray,0,mNormalCount*sizeof(Vector3D));
	
	kstl::vector<ModifiableItemStruct>::const_iterator it;

	for (it=getItems().begin();it!=getItems().end();++it)
	{
		if((*it).mItem->isSubType(MeshItemGroup::mClassID))
		{
			SP<MeshItemGroup>& currentGroup= ((SP<MeshItemGroup>&)(*it).mItem);

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
		mNormalArray[i].Normalize();
	}

}

// recompute all normals with faster but less precise algorithm
void	Mesh::RecomputeNormalsFast()
{
	Vector3D na;
	int i;

	if(mNormalCount == 0)
	{
		return;
	}
	
	memset(mNormalArray,0,mNormalCount*sizeof(Vector3D));
	
	kstl::vector<ModifiableItemStruct>::const_iterator it;

	for (it=getItems().begin();it!=getItems().end();++it)
	{
		if((*it).mItem->isSubType(MeshItemGroup::mClassID))
		{
			SP<MeshItemGroup>& currentGroup= ((SP<MeshItemGroup>&)(*it).mItem);

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
		mNormalArray[i].Normalize();
	}
}