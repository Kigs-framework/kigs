#include "PrecompiledHeaders.h"

#include "Core.h"
#include "BinMeshLoaderBase.h"

using namespace Kigs::Utils;
using namespace Kigs::File;

BinMeshLoaderBase::BinMeshLoaderBase() : mFile(0)
{
}

BinMeshLoaderBase::~BinMeshLoaderBase()
{
}


std::string	BinMeshLoaderBase::ReadString(BufferedFile*)
{
	std::string result="";

	char	tmpbuf;
	while(fread(&tmpbuf,sizeof(char),1,mFile) == 1)
	{
		if(tmpbuf != 0)
		{
			result+=tmpbuf;
		}
		else
		{
			break;
		}
	}

	return result;
}

void	BinMeshLoaderBase::BinFileStruct::WriteStruct(BinFileStruct* result, FILE* file, BinFileStructDesc* desc)
{
	fwrite(result,sizeof(BinFileStruct),1,file);
	if((desc)||(result->DescStructSize))
	{
		// todo
	}

	int fullsize=(int)result->structSize-(int)sizeof(BinFileStruct);
	char* offset=(char*)result;
	offset+=sizeof(BinFileStruct);

	fwrite(offset,fullsize,1,file);
}

void	BinMeshLoaderBase::BinFileStruct::ReadStruct(BinFileStruct* result, BufferedFile* file, BinFileStructDesc* desc)
{
	fread(result,sizeof(BinFileStruct),1,file);
	if((desc)||(result->DescStructSize))
	{
		// todo
	}

	int fullsize=(int)result->structSize-(int)sizeof(BinFileStruct);
	char* offset=(char*)result;
	offset+=sizeof(BinFileStruct);

	fread(offset,fullsize,1,file);
}

void			BinMeshLoaderBase::ReadFloatArray(float* dst,unsigned int count,BufferedFile* file)
{
	// create temporary float array
	float*	tmpArray=new float[count];
	fread(tmpArray,(int)sizeof(float)*(int)count,1,file);

	float* readarray=tmpArray;

	unsigned int i;	
	for(i=0;i<count;i++)
	{
		*dst++=(float)*readarray++;
	}
	delete[] tmpArray;
}
