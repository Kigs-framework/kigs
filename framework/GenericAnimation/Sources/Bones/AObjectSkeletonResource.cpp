#include "Bones/AObjectSkeletonResource.h"
#include "FilePathManager.h"
#include "ModuleFileManager.h"

IMPLEMENT_CLASS_INFO(AObjectSkeletonResource)

AObjectSkeletonResource::AObjectSkeletonResource(const kstl::string& name, CLASS_NAME_TREE_ARG)
	: AObjectResource(name, PASS_CLASS_NAME_TREE_ARG)
	, mySkeletonFileName(*this, true, LABEL_AND_ID(SkeletonFileName), "")
	, m_BoneCount(0)
	, m_BoneDataSize(0)
	, myBoneTreeNodeList(0)
{

}

void	AObjectSkeletonResource::InitModifiable()
{
	// if already init return
	if (_isInit)
	{
		return;
	}

	AObjectResource::InitModifiable();

	if (!_isInit)
	{
		return;
	}

	SP<FilePathManager>	pathManager = KigsCore::GetSingleton("FilePathManager");

	SmartPointer<FileHandle> fullfilenamehandle = 0;

	if (pathManager)
	{
		fullfilenamehandle = pathManager->FindFullName(mySkeletonFileName.const_ref());
	}

	if (!((fullfilenamehandle->myStatus&FileHandle::Exist) == 0))
	{

		u64 size;

		SmartPointer<CoreRawBuffer> rawbuffer = OwningRawPtrToSmartPtr(ModuleFileManager::LoadFile(fullfilenamehandle.get(), size));

		if (rawbuffer)
		{

			ASkeletonFileHeader* result= (ASkeletonFileHeader*)rawbuffer->buffer();

			char* read = (char*)result;
			read += sizeof(ASkeletonFileHeader);
			myBoneTreeNodeList = (char*)malloc(result->m_BoneCount*result->m_BoneDataSize);
			unsigned int i;

			m_BoneCount = result->m_BoneCount;
			m_BoneDataSize = result->m_BoneDataSize;

			for (i = 0; i < result->m_BoneCount; i++)
			{
				boneTreeNodeStruct* node = getBoneData(i);

				memcpy(node, read, m_BoneDataSize);
				read += m_BoneDataSize;
			}
			return;
		}
	}

	// there's an error somewhere
	UninitModifiable();
}

AObjectSkeletonResource::~AObjectSkeletonResource()
{
	if (myBoneTreeNodeList)
		free(myBoneTreeNodeList);
	myBoneTreeNodeList = 0;
}

void	AObjectSkeletonResource::initSkeleton(int boneCount, unsigned int BoneDataSize)
{
	m_BoneCount = boneCount;
	m_BoneDataSize = BoneDataSize + sizeof(boneTreeNodeStruct);

	myBoneTreeNodeList = (char*)malloc(m_BoneCount*m_BoneDataSize);
	
}
void	AObjectSkeletonResource::addBone(int index, unsigned int uid, unsigned int gid, unsigned int fgid, const Matrix3x4& inv_bind_matrix)
{
	boneTreeNodeStruct* node = getBoneData(index); 
	node->gid = gid;
	node->fathergid = fgid;
	node->uid = uid;
	node->inv_bind_matrix = inv_bind_matrix;
}

void	AObjectSkeletonResource::setStandData(int index, LocalToGlobalBaseType* toSet)
{
	boneTreeNodeStruct* node = getBoneData(index);

	memcpy(node->getData(), toSet, m_BoneDataSize - sizeof(boneTreeNodeStruct));
}

#ifdef WIN32
void	AObjectSkeletonResource::Export()
{
	if (m_BoneCount == 0)
	{
		return;
	}

	SmartPointer<FileHandle> file = Platform_fopen(mySkeletonFileName.const_ref().c_str(), "wb");

	if (file->myFile)
	{

		ASkeletonFileHeader	header;
		header.m_ID = 'ASKL';
		header.m_BoneCount = m_BoneCount;
		header.m_BoneDataSize = m_BoneDataSize;

		Platform_fwrite(&header, 1, sizeof(ASkeletonFileHeader), file.get());

		unsigned int i;
		for (i = 0; i < m_BoneCount; i++)
		{
			boneTreeNodeStruct* node = getBoneData(i);
			Platform_fwrite(node, 1, m_BoneDataSize, file.get());
		}

		Platform_fflush(file.get());
		Platform_fclose(file.get());
		
	}
}
#endif