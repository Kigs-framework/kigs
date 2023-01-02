#pragma once

#include "CoreModifiableAttribute.h"

#include "../AObjectResource.h"

namespace Kigs
{
	namespace Anim
	{
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
				return mBoneCount;
			}

			unsigned int getID(int index)
			{
				return getBoneData(index)->mGID;
			}
			unsigned int getUID(int index)
			{
				return getBoneData(index)->mUID;
			}

			unsigned int getFatherID(int index)
			{
				return getBoneData(index)->mFatherGID;
			}

			const Matrix3x4& getInvBindMatrix(int index)
			{
				return getBoneData(index)->mInvBindMatrix;
			}

			// create a skeleton in code
			void	initSkeleton(int boneCount, unsigned int BoneDataSize);
			void	addBone(int index, unsigned int mUID, unsigned int mGID, unsigned int fgid, const Matrix3x4& mInvBindMatrix);
			void	setStandData(int index, LocalToGlobalBaseType* toSet);

#ifdef WIN32
			void	Export();
#endif

			LocalToGlobalBaseType* getStandData(int index)
			{
				return getBoneData(index)->getData();
			}

			// the skeleton file is organized like this :

			// the header : ASkeletonFileHeader
			//                                  - mID                      // 4 bytes 
			//                                  - mBoneCount				// 4 bytes 
			//									- mBoneDataSize			// 4 bytes   the size of the boneTreeNodeStruct

			//  then for each bone a boneTreeNodeStruct

			//                                  - mGID							// 4 bytes	the id of the bone (mUID based on the name)
			//                                  - mFatherGID						// 4 bytes  the id of this bone father (or 0 if root)
			//									- mInvBindMatrix				// 16x4 bytes the inv bind matrix of this bone
			//                                  - data						    // ? bytes the stand data for this bone 

		protected:

			void	InitModifiable() override;


			struct ASkeletonFileHeader
			{
				unsigned int			mID;
				unsigned int			mBoneCount;
				unsigned int			mBoneDataSize;
				Matrix3x4				mLocalTransformMatrix;
				Matrix3x4				mGlobalTransformMatrix;


			};

			class	boneTreeNodeStruct
			{
			public:

				boneTreeNodeStruct()
				{
					mGID = 0;
					mFatherGID = 0;
				}

				~boneTreeNodeStruct()
				{

				}

				LocalToGlobalBaseType* getData() const
				{
					return (LocalToGlobalBaseType*)(((const char*)this) + sizeof(boneTreeNodeStruct));
				}

				//! id
				unsigned int			mGID;
				unsigned int			mFatherGID;
				unsigned int			mUID;
				Matrix3x4				mInvBindMatrix;
			};


			boneTreeNodeStruct* getBoneData(unsigned int index)
			{
				return(boneTreeNodeStruct*)(mBoneTreeNodeList + mBoneDataSize * index);
			}


			maString					mSkeletonFileName;

			unsigned int				mBoneCount;

			unsigned int				mBoneDataSize;

			char* mBoneTreeNodeList;
		};

	}
}
