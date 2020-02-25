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
, myVertexNeedUpdate(*this,false,LABEL_AND_ID(VertexNeedUpdate),false)
, myColorNeedUpdate(*this,false,LABEL_AND_ID(ColorNeedUpdate),false)
, myTexCoordNeedUpdate(*this,false,LABEL_AND_ID(TexCoordNeedUpdate),false)
, myNormalNeedUpdate(*this,false,LABEL_AND_ID(NormalNeedUpdate),false)
, myShareMaterial(*this,false,LABEL_AND_ID(ShareMaterial),true)
#if _DEBUG
, myShowVertex(*this,false,LABEL_AND_ID(ShowVertex),false)
#endif
, myWireMode(*this,false,LABEL_AND_ID(WireMode),false)
, myFileName(*this, true, LABEL_AND_ID(FileName))
, myDynamicInit(*this, false, LABEL_AND_ID(DynamicInit), false)
{
	VertexArray = 0;
	NormalArray = 0;
	ColorArray = 0;
	TexArray = 0;

  myBoundingBox.m_Min.Set(KFLOAT_CONST(0.0f),KFLOAT_CONST(0.0f),KFLOAT_CONST(0.0f));
  myBoundingBox.m_Max.Set(KFLOAT_CONST(-1.0f),KFLOAT_CONST(-1.0f),KFLOAT_CONST(-1.0f));
}

/*
 *	Destructor
 */
Mesh::~Mesh()
{
	if (VertexArray) 
		delete[](VertexArray);
	VertexArray = NULL;
	if (NormalArray) 
		delete[](NormalArray);
	NormalArray = NULL;

	if (TexArray) 
		delete[](TexArray);
	TexArray = NULL;
	if (ColorArray) 
		delete[](ColorArray);
	ColorArray = NULL;
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
	if(myFirstTriangle)
	{
		delete[] ((Mesh::Triangle*)myFirstTriangle);
	}
}

void	Mesh::InitModifiable()
{
	HDrawable::InitModifiable();
	if(_isInit)
	{
		if (!myDynamicInit)
		{
			auto& pathManager = KigsCore::Singleton<FilePathManager>();

			kstl::string fullfilename;
			SmartPointer<FileHandle> fullfilenamehandle = pathManager->FindFullName(myFileName);
			if (fullfilenamehandle)
			{
				fullfilename = fullfilenamehandle->myFullFileName;
				kstl::string	extension=myFileName;

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
						KIGS_WARNING("Can not open mesh file :"+myFileName.const_ref(),2);
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
						KIGS_WARNING("Can not open mesh file :"+myFileName.const_ref(),2);
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
				KIGS_WARNING("Can not open mesh file :"+(kstl::string&)myFileName,2); 
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
	if((_isInit)&&(VertexCount))
	{
		myBoundingBox.Init(VertexArray,(int)VertexCount);
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
	return (int)VertexCount;
}

int Mesh::getTriangleCount()
{
	int Count = 0;
	kstl::vector<ModifiableItemStruct>::const_iterator it;

	for (it=getItems().begin();it!=getItems().end();++it)
	{
		if((*it).myItem->isSubType(MeshItemGroup::myClassID))
		{
			Count += ((SP<MeshItemGroup>&)(*it).myItem)->myTriangleCount;
		}
	}
	return Count;
}

bool Mesh::getVertex(unsigned int index, Point3D &Pt)
{
	if (index>=VertexCount) return false;
	Pt = VertexArray[index];
	return true;
}

void Mesh::SetVertex(unsigned int index, const Point3D &v)
{
	KIGS_ASSERT(index<VertexCount);
	VertexArray[index] = v;
	myVertexNeedUpdate = true;
}


void Mesh::SetColor(unsigned int index, const Vector4D &v)
{
	KIGS_ASSERT(index<ColorCount);
	ColorArray[index]=v;
	myColorNeedUpdate=true;	
}

void Mesh::SetNormal(unsigned int index, const Vector3D &v)
{
	KIGS_ASSERT(index<NormalCount);
	NormalArray[index]=v;
	myNormalNeedUpdate=true;
}

void Mesh::SetTexCoord(unsigned int index, const TexCoord &v)
{
	KIGS_ASSERT(index<TexCoordCount);
	TexArray[index]=v;
	myTexCoordNeedUpdate=true;
		
}


bool Mesh::getTriangle(int index, int &a, int &b, int &c)
{
	//int g = 0;
	Mesh::Triangle *Tr=0;

	kstl::vector<ModifiableItemStruct>::const_iterator it;
	for (it=getItems().begin();it!=getItems().end();++it)
	{
		if((*it).myItem->isSubType(MeshItemGroup::myClassID))
		{
			if(index>=((SP<MeshItemGroup>&)(*it).myItem)->myTriangleCount)
			{
				index-=((SP<MeshItemGroup>&)(*it).myItem)->myTriangleCount;
			}
			else
			{
				Tr = (Mesh::Triangle *)(((char*)((SP<MeshItemGroup>&)(*it).myItem)->myFirstTriangle) + ((SP<MeshItemGroup>&)(*it).myItem)->myTriangleSize*index);
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

	if (VertexCount==0 || TCount==0) 
	{
		return false;
	}

	VCount = (int)VertexCount;
	VArray = new Point3D[VCount];
	memcpy(VArray,VertexArray,VCount*sizeof(Point3D));

	IndexArray = new int[TCount*3];

	k=0;

	kstl::vector<ModifiableItemStruct>::const_iterator it;
	for (it=getItems().begin();it!=getItems().end();++it)
	{
		if((*it).myItem->isSubType(MeshItemGroup::myClassID))
		{
			Mesh::Triangle *Tr=(Mesh::Triangle *)(((SP<MeshItemGroup>&)(*it).myItem)->myFirstTriangle) ;

			int	sizeoftriangle=(((SP<MeshItemGroup>&)(*it).myItem)->myTriangleSize);

			for (j=0;j<(((SP<MeshItemGroup>&)(*it).myItem)->myTriangleCount);j++)
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
	

	if(myVertexNeedUpdate||myColorNeedUpdate||myTexCoordNeedUpdate||myNormalNeedUpdate)
	{
		UpdateMesh();
		myColorNeedUpdate=false;
		myNormalNeedUpdate=false;
		myTexCoordNeedUpdate=false;
		myVertexNeedUpdate=false;
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
			buffersize->setValue((int)VertexCount);
		}
		result->setValue((u64)VertexArray);
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
		result->setValue((u64)ColorArray);
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
		result->setValue((u64)NormalArray);
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
		result->setValue((u64)TexArray);
		return true;
	}

	return false;
}



// rebuild all normals from triangles 
void	Mesh::RecomputeNormals()
{
	Vector3D na,nb,nc;
	unsigned int i;

	if(NormalCount == 0)
	{
		return;
	}
	
	memset(NormalArray,0,NormalCount*sizeof(Vector3D));
	
	kstl::vector<ModifiableItemStruct>::const_iterator it;

	for (it=getItems().begin();it!=getItems().end();++it)
	{
		if((*it).myItem->isSubType(MeshItemGroup::myClassID))
		{
			SP<MeshItemGroup>& currentGroup= ((SP<MeshItemGroup>&)(*it).myItem);

			if( ((unsigned int)currentGroup->myTriangleType) & 1 ) // check if smooth triangle
			{
				Mesh::S_Triangle *Tr=(Mesh::S_Triangle *)currentGroup->myFirstTriangle;
			
				for(i=0;i<(unsigned int)currentGroup->myTriangleCount;i++)
				{
					Tr->NormalAngle(VertexArray,na,nb,nc);

					NormalArray[Tr->Na]+=na;
					NormalArray[Tr->Nb]+=nb;
					NormalArray[Tr->Nc]+=nc;

					Tr=(Mesh::S_Triangle *)(((unsigned char*)Tr)+currentGroup->myTriangleSize);
				}
			}
			else // flat triangle
			{
				Mesh::F_Triangle *Tr=(Mesh::F_Triangle *)currentGroup->myFirstTriangle;
			
				for (i = 0; i<(unsigned int)currentGroup->myTriangleCount; i++)
				{
					Tr->Normal(VertexArray,na);

					NormalArray[Tr->N]=-na;
					
					Tr=(Mesh::F_Triangle *)(((unsigned char*)Tr)+currentGroup->myTriangleSize);
				}
			}
		}
	}

	// then normalize all again
	for(i=0;i<NormalCount;i++)
	{
		NormalArray[i].Normalize();
	}

}

// recompute all normals with faster but less precise algorithm
void	Mesh::RecomputeNormalsFast()
{
	Vector3D na;
	int i;

	if(NormalCount == 0)
	{
		return;
	}
	
	memset(NormalArray,0,NormalCount*sizeof(Vector3D));
	
	kstl::vector<ModifiableItemStruct>::const_iterator it;

	for (it=getItems().begin();it!=getItems().end();++it)
	{
		if((*it).myItem->isSubType(MeshItemGroup::myClassID))
		{
			SP<MeshItemGroup>& currentGroup= ((SP<MeshItemGroup>&)(*it).myItem);

			if( ((unsigned int)currentGroup->myTriangleType) & 1 ) // check if smooth triangle
			{
				Mesh::S_Triangle *Tr=(Mesh::S_Triangle *)currentGroup->myFirstTriangle;
			
				for(i=0;i<currentGroup->myTriangleCount;i++)
				{
					Tr->Normal(VertexArray,na);

					NormalArray[Tr->Na]-=na;
					NormalArray[Tr->Nb]-=na;
					NormalArray[Tr->Nc]-=na;

					Tr=(Mesh::S_Triangle *)(((unsigned char*)Tr)+currentGroup->myTriangleSize);
				}
			}
			else // flat triangle
			{
				Mesh::F_Triangle *Tr=(Mesh::F_Triangle *)currentGroup->myFirstTriangle;
			
				for(i=0;i<currentGroup->myTriangleCount;i++)
				{
					Tr->Normal(VertexArray,na);

					NormalArray[Tr->N]-=na;
					
					Tr=(Mesh::F_Triangle *)(((unsigned char*)Tr)+currentGroup->myTriangleSize);
				}
			}
		}
	}

	// then normalize all again
	for (i = 0; i<(int)NormalCount; i++)
	{
		NormalArray[i].Normalize();
	}
}