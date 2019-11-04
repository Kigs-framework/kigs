#ifndef _AOBJECTSKELETONRESOURCE_H_
#define _AOBJECTSKELETONRESOURCE_H_


#include "CoreModifiableAttribute.h"

#include "../AObjectResource.h"
//#include "../ASystem.h"

class LocalToGlobalBaseType;

class AObjectSkeletonResource : public AObjectResource
{
public:
	DECLARE_CLASS_INFO(AObjectSkeletonResource, AObjectResource, Animation);
	DECLARE_CONSTRUCTOR(AObjectSkeletonResource);

	/**
	* \brief	destructor
	* \fn 		~AObjectSkeletonResource();
	*/
	virtual ~AObjectSkeletonResource();

	unsigned int		GetGroupCount()
	{
		return m_BoneCount;
	}

	unsigned int getID(int index)
	{
		return getBoneData(index)->gid;
	}
	unsigned int getUID(int index)
	{
		return getBoneData(index)->uid;
	}

	unsigned int getFatherID(int index)
	{
		return getBoneData(index)->fathergid;
	}

	const Matrix3x4& getInvBindMatrix(int index)
	{
		return getBoneData(index)->inv_bind_matrix;
	}

	// create a skeleton in code
	void	initSkeleton(int boneCount,unsigned int BoneDataSize);
	void	addBone(int index, unsigned int uid, unsigned int gid, unsigned int fgid, const Matrix3x4& inv_bind_matrix);
	void	setStandData(int index, LocalToGlobalBaseType* toSet);

#ifdef WIN32
	void	Export();
#endif

	LocalToGlobalBaseType*	getStandData(int index) 
	{
		return getBoneData(index)->getData();
	}

	// the skeleton file is organized like this :

	// the header : ASkeletonFileHeader
	//                                  - m_ID                      // 4 bytes 
	//                                  - m_BoneCount				// 4 bytes 
	//									- m_BoneDataSize			// 4 bytes   the size of the boneTreeNodeStruct

	//  then for each bone a boneTreeNodeStruct

	//                                  - gid							// 4 bytes	the id of the bone (uid based on the name)
	//                                  - fathergid						// 4 bytes  the id of this bone father (or 0 if root)
	//									- inv_bind_matrix				// 16x4 bytes the inv bind matrix of this bone
	//                                  - data						    // ? bytes the stand data for this bone 

protected:

	void	InitModifiable() override;


	struct ASkeletonFileHeader
	{
		unsigned int			m_ID;
		unsigned int			m_BoneCount;
		unsigned int			m_BoneDataSize;
		Matrix3x4				m_LocalTransformMatrix;
		Matrix3x4				m_globalTransformMatrix;
		

	};

	class	boneTreeNodeStruct
	{
	public:

		boneTreeNodeStruct()
		{
			gid = 0;
			fathergid = 0;
		}

		~boneTreeNodeStruct()
		{
		
		}

		LocalToGlobalBaseType*	getData() const
		{
			return (LocalToGlobalBaseType*)(((const char*)this) + sizeof(boneTreeNodeStruct));
		}

		//! id
		unsigned int			gid;
		unsigned int			fathergid;
		unsigned int			uid;
		Matrix3x4				inv_bind_matrix;
	};


	boneTreeNodeStruct*	getBoneData(unsigned int index)
	{
		return(boneTreeNodeStruct*)(myBoneTreeNodeList + m_BoneDataSize*index);
	}


	maString					mySkeletonFileName;
	
	unsigned int				m_BoneCount;
	
	unsigned int				m_BoneDataSize;

	char*						myBoneTreeNodeList;
};

#endif //_AOBJECTSKELETONRESOURCE_H_
