#include "Bones/AObjectSkeletonResource.h"
#include "FilePathManager.h"
#include "ModuleFileManager.h"

IMPLEMENT_CLASS_INFO(AObjectSkeletonResource)

AObjectSkeletonResource::AObjectSkeletonResource(const std::string& name, CLASS_NAME_TREE_ARG)
	: AObjectResource(name, PASS_CLASS_NAME_TREE_ARG)
	, mSkeletonFileName(*this, true, "SkeletonFileName", "")
	, mBoneCount(0)
	, mBoneDataSize(0)
	, mBoneTreeNodeList(0)
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
		fullfilenamehandle = pathManager->FindFullName(mSkeletonFileName.const_ref());
	}

	if (!((fullfilenamehandle->mStatus&FileHandle::Exist) == 0))
	{

		u64 size;

		SmartPointer<CoreRawBuffer> rawbuffer = ModuleFileManager::LoadFile(fullfilenamehandle.get(), size);

		if (rawbuffer)
		{

			ASkeletonFileHeader* result= (ASkeletonFileHeader*)rawbuffer->buffer();

			char* read = (char*)result;
			read += sizeof(ASkeletonFileHeader);
			mBoneTreeNodeList = (char*)malloc(result->mBoneCount*result->mBoneDataSize);
			unsigned int i;

			mBoneCount = result->mBoneCount;
			mBoneDataSize = result->mBoneDataSize;

			for (i = 0; i < result->mBoneCount; i++)
			{
				boneTreeNodeStruct* node = getBoneData(i);

				memcpy(node, read, mBoneDataSize);
				read += mBoneDataSize;
			}
			return;
		}
	}

	// there's an error somewhere
	UninitModifiable();
}

AObjectSkeletonResource::~AObjectSkeletonResource()
{
	if (mBoneTreeNodeList)
		free(mBoneTreeNodeList);
	mBoneTreeNodeList = 0;
}

void	AObjectSkeletonResource::initSkeleton(int boneCount, unsigned int BoneDataSize)
{
	mBoneCount = boneCount;
	mBoneDataSize = BoneDataSize + sizeof(boneTreeNodeStruct);

	mBoneTreeNodeList = (char*)malloc(mBoneCount*mBoneDataSize);
	
}
void	AObjectSkeletonResource::addBone(int index, unsigned int mUID, unsigned int mGID, unsigned int fgid, const Matrix3x4& mInvBindMatrix)
{
	boneTreeNodeStruct* node = getBoneData(index); 
	node->mGID = mGID;
	node->mFatherGID = fgid;
	node->mUID = mUID;
	node->mInvBindMatrix = mInvBindMatrix;
}

void	AObjectSkeletonResource::setStandData(int index, LocalToGlobalBaseType* toSet)
{
	boneTreeNodeStruct* node = getBoneData(index);

	memcpy(node->getData(), toSet, mBoneDataSize - sizeof(boneTreeNodeStruct));
}

#ifdef WIN32
void	AObjectSkeletonResource::Export()
{
	if (mBoneCount == 0)
	{
		return;
	}

	SmartPointer<FileHandle> file = Platform_fopen(mSkeletonFileName.const_ref().c_str(), "wb");

	if (file->mFile)
	{

		ASkeletonFileHeader	header;
		header.mID = 'ASKL';
		header.mBoneCount = mBoneCount;
		header.mBoneDataSize = mBoneDataSize;

		Platform_fwrite(&header, 1, sizeof(ASkeletonFileHeader), file.get());

		unsigned int i;
		for (i = 0; i < mBoneCount; i++)
		{
			boneTreeNodeStruct* node = getBoneData(i);
			Platform_fwrite(node, 1, mBoneDataSize, file.get());
		}

		Platform_fflush(file.get());
		Platform_fclose(file.get());
		
	}
}
#endif