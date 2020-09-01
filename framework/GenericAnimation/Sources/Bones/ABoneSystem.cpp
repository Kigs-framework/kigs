// **********************************************************************
// * FILE  : ABoneSystem.cpp
// * GROUP : Animation Module
// *---------------------------------------------------------------------
// * PURPOSE : Bone Animation System
// * COMMENT :
// *---------------------------------------------------------------------
// **********************************************************************

//#define DRAW_DEBUG

#include "Bones/ABoneSystem.h"
#include "Bones/ABoneChannel.h"
#include "AObject.h"
#include "Node3D.h"
#include "Drawable.h"
#include "GenericAnimationModule.h"
#include "Bones/AObjectSkeletonResource.h"

IMPLEMENT_CLASS_INFO(ABoneSystem)

ABoneSystem::ABoneSystem(const kstl::string& name,CLASS_NAME_TREE_ARG) : ASystem(name,PASS_CLASS_NAME_TREE_ARG)
, mParentNode3D(0)
, mBoneMatrixArray(0)
, mSkeleton(0)
{
	
}

ABoneSystem::~ABoneSystem()
{
	
}


// ******************************
// * Animate
// *-----------------------------
// * - according to the animation mode, call the root channel animate()
// * - 
// ******************************

void    ABoneSystem::Animate(ATimeValue t)
{
	SearchParentNode3D();
	
	InitLocalToGlobalData();
	((ABoneChannel*)mRoot.get())->AnimateRoot(t, this);
	
	ApplyLocalToGlobalData();
	
#ifdef DRAW_DEBUG
	if(mParentNode3D)
		DrawSkeletonRec(0, {}, mParentNode3D->GetLocalToGlobal(), false);
#endif
};


void ABoneSystem::SetupDraw()
{
	AObject* aobject = GetAObject();
	if(!mSkeleton)
	{
		for (unsigned int i = 0; i < aobject->getItems().size(); i++)
		{
			// find skeleton item
			if (aobject->getItems().at(i).mItem->isSubType("AObjectSkeletonResource"))
			{
				mSkeleton = (AObjectSkeletonResource*)aobject->getItems().at(i).mItem.get();
				break;
			}
		}
	}
	if(mSkeleton)
		UpdateBoneMatrices(mSkeleton);
	
	
}


void	ABoneSystem::SearchParentNode3D()
{
	if ( mParentNode3D == 0 )
	{
		CoreModifiable* attachedObject = GetAObject()->GetObject();
		
		while (attachedObject)
		{
			// depending on object type 
			if (attachedObject->isSubType(Node3D::mClassID))
			{
				mParentNode3D = (Node3D*)attachedObject;
				break;
			}
			attachedObject = attachedObject->getFirstParent(SceneNode::mClassID);
		}
		
		if (mParentNode3D) // add skinning
		{
			const Matrix3x4& currentMatrix= mParentNode3D->GetLocal();
			m_pStartingLocalToGlobalData.set(currentMatrix);
			m_pInstantLocalToGlobalData.set(currentMatrix);
			
			GenericAnimationModule* animation = (GenericAnimationModule*)KigsCore::GetModule("GenericAnimationModule");
			CMSP shader = KigsCore::GetInstanceOf(getName()+"SkinShader", "API3DSkinShader");
			shader->GetRef();
			// add matrices to shader
			CMSP uniformMatrixArray = KigsCore::GetInstanceOf(getName() + "SkinShaderMatrix", "API3DUniformMatrixArray");
			
			uniformMatrixArray->setValue(LABEL_TO_ID(ArraySize), 66);
			uniformMatrixArray->setValue(LABEL_TO_ID(Name), "bone_matrix");
			uniformMatrixArray->Init();
			shader->addItem(uniformMatrixArray);
			shader->Init();
			animation->addShader(mParentNode3D, shader.get());
			
			// retreive matrix buffer
			void* buffer = nullptr;
			uniformMatrixArray->getValue(LABEL_TO_ID(MatrixArray), buffer);
			mBoneMatrixArray =(Matrix4x4*)((AlignedCoreRawBuffer<16,char>*)buffer)->buffer();
		}
	}
}

void ABoneSystem::UpdateBoneMatrices(AObjectSkeletonResource* skeleton)
{
	SearchParentNode3D();
	if(!mBoneMatrixArray)
	{
		KIGS_WARNING("No bone matrix array to update", 0);
		return;
	}
	
	for (unsigned int i = 0; i < skeleton->GetGroupCount(); i++)
	{
		unsigned int id = skeleton->getID(i);
		unsigned int mUID = skeleton->getUID(i);
		
		// get channel w/ UID
		ABoneChannel* channel = (ABoneChannel*)this->GetChannelByUID(mUID);
		Matrix3x4 global_transform = channel->GetCurrentPRSMatrix();
		Matrix3x4 mInvBindMatrix = skeleton->getInvBindMatrix(i);
		mBoneMatrixArray[id - 1] = global_transform *mInvBindMatrix;
	}
}

#ifdef DRAW_DEBUG
#include "GLSLDebugDraw.h"

void ABoneSystem::DrawSkeletonRec(int current_index, const Matrix3x4& parent_transform, const Matrix3x4& root, bool need_draw)
{
	if(!mSkeleton) return;
	
	// get current bone data id 
	unsigned int mGID = mSkeleton->getID(current_index);
	// get current bone data mUID 
	unsigned int mUID = mSkeleton->getUID(current_index);
	
	// get current channel w/ bone data mUID
	ABoneChannel* channel = (ABoneChannel*)GetChannelByUID(mUID);
	
	//  get current local transformation (PRS matrix)
	Matrix3x4 local_transform = channel->GetCurrentPRSMatrix();
	
	// global = local because parent*local already computed in parser
	Matrix3x4 global_transform = root*local_transform;
	
	// draw bones
	if (need_draw)
	{
		// draw bone (arrow)
		dd::arrow(parent_transform.GetTranslation(), global_transform.GetTranslation(), {255, 255, 0}, 0.05f, 0 );
		
		// copy of global_tranform 
		auto global_transform_copy = global_transform;
		
		// reset translation
		global_transform_copy.SetTranslation({ 0, 0, 0 });
		
		// define bone local axis
		Vector3D localAxisX = global_transform_copy * Vector3D(1, 0, 0);
		Vector3D localAxisY = global_transform_copy * Vector3D(0, 1, 0);
		Vector3D localAxisZ = global_transform_copy * Vector3D(0, 0, 1);
		
		// draw bone local axis (line)
		Point3D axisFrom = global_transform.GetTranslation();
		Point3D axisTo_X = axisFrom + localAxisX / 4;
		Point3D axisTo_Y = axisFrom + localAxisY / 4;
		Point3D axisTo_Z = axisFrom + localAxisZ / 4;
		
		
		dd::line(axisFrom, axisTo_X, {255, 0, 0}, 0 );
		dd::line(axisFrom, axisTo_Y, {0, 255, 0}, 0 );
		dd::line(axisFrom, axisTo_Z, {0, 0, 255}, 0 );
	}
	else
	{
		dd::sphere(global_transform.GetTranslation(), {0,0,255}, 5);
		
		Matrix3x4 m;
		m_pInstantLocalToGlobalData.get(m);
		dd::sphere(m.GetTranslation(), {0,255,255}, 4);
	}
	
	// iterate
	for (unsigned int i = 0; i<mSkeleton->GetGroupCount(); ++i)
	{
		if (mSkeleton->getFatherID(i) == mGID)
		{
			DrawSkeletonRec(i, global_transform, root, true);
		}
	}
}
#endif

void	ABoneSystem::InitLocalToGlobalData()
{
	if (mParentNode3D == 0)
	{
		return;
	}
	
	const Matrix3x4& currentMatrix = mParentNode3D->GetLocal();
	
	ABaseStream* tmp_stream = GetValidStream();
	if (mUseAnimationLocalToGlobal == false)
	{
		if(m_LinkedChannel)
		{
			ABaseStream* valid_stream = GetValidStream();
			if(valid_stream)
				valid_stream->CopyData(&m_pInstantLocalToGlobalData, m_LinkedChannel->GetChannelLocalToGlobalData());
			/*
			ABoneSystem* othersystem = (ABoneSystem*)lchannel->GetSystem();
			ABaseStream* valid_stream = GetValidStream();
			PRSKey m; m.set(othersystem->mParentNode3D->GetLocal());
			valid_stream->MulData(&m_pInstantLocalToGlobalData, &m);*/
		}
		else
			m_pInstantLocalToGlobalData.set(currentMatrix);
	}
	else
	{
		PRSKey m; m.set(currentMatrix);
		Matrix3x4 old; m_pInstantLocalToGlobalData.get(old);
		//@Refactor use a proper != operator
		if (memcmp(&currentMatrix.e, &old.e, sizeof(Matrix3x4)) != 0)
			tmp_stream->SetAndModifyData(&m_pInstantLocalToGlobalData, &m, &m_pStartingLocalToGlobalData); 
	}
	
};

void	ABoneSystem::ApplyLocalToGlobalData()
{
	if (!mParentNode3D)
		return;
	
	if (mUseAnimationLocalToGlobal || m_LinkedChannel)
	{
		Matrix3x4 m; m_pInstantLocalToGlobalData.get(m);
		mParentNode3D->ChangeMatrix(m);
	}
	
	
}