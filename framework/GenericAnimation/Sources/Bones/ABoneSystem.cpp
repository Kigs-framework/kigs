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
, myParentNode3D(0)
, myBoneMatrixArray(0)
, mySkeleton(0)
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
	((ABoneChannel*)mp_Root)->AnimateRoot(t, this);
	
	ApplyLocalToGlobalData();
	
#ifdef DRAW_DEBUG
	if(myParentNode3D)
		DrawSkeletonRec(0, {}, myParentNode3D->GetLocalToGlobal(), false);
#endif
};


void ABoneSystem::SetupDraw()
{
	AObject* aobject = GetAObject();
	if(!mySkeleton)
	{
		for (unsigned int i = 0; i < aobject->getItems().size(); i++)
		{
			// find skeleton item
			if (aobject->getItems().at(i).myItem->isSubType(_S_2_ID("AObjectSkeletonResource")))
			{
				mySkeleton = (AObjectSkeletonResource*)aobject->getItems().at(i).myItem;
				break;
			}
		}
	}
	if(mySkeleton)
		UpdateBoneMatrices(mySkeleton);
	
	
}


void	ABoneSystem::SearchParentNode3D()
{
	if ( myParentNode3D == 0 )
	{
		CoreModifiable* attachedObject = GetAObject()->GetObject();
		
		while (attachedObject)
		{
			// depending on object type 
			if (attachedObject->isSubType(Node3D::myClassID))
			{
				myParentNode3D = (Node3D*)attachedObject;
				break;
			}
			attachedObject = attachedObject->getFirstParent(SceneNode::myClassID);
		}
		
		if (myParentNode3D) // add skinning
		{
			const Matrix3x4& currentMatrix= myParentNode3D->GetLocal();
			m_pStartingLocalToGlobalData.set(currentMatrix);
			m_pInstantLocalToGlobalData.set(currentMatrix);
			
			GenericAnimationModule* animation = (GenericAnimationModule*)KigsCore::GetModule(_S_2_ID("GenericAnimationModule"));
			CoreModifiable* shader = KigsCore::GetInstanceOf(getName()+"SkinShader", _S_2_ID("GLSLSkinShader"));
			
			// add matrices to shader
			CoreModifiable* uniformMatrixArray = KigsCore::GetInstanceOf(getName() + "SkinShaderMatrix", _S_2_ID("GLSLUniformMatrixArray"));
			
			uniformMatrixArray->setValue(LABEL_TO_ID(ArraySize), 66);
			uniformMatrixArray->setValue(LABEL_TO_ID(Name), "bone_matrix");
			uniformMatrixArray->Init();
			shader->addItem(uniformMatrixArray);
			uniformMatrixArray->Destroy();
			shader->Init();
			animation->addShader(myParentNode3D, shader);
			
			// retreive matrix buffer
			CheckUniqueObject retreiveMatrix;
			uniformMatrixArray->getValue(LABEL_TO_ID(MatrixArray), retreiveMatrix);
			
			myBoneMatrixArray =(Matrix4x4*)((AlignedCoreRawBuffer<16,char>*)(RefCountedClass*)retreiveMatrix)->buffer();
		}
	}
}

void ABoneSystem::UpdateBoneMatrices(AObjectSkeletonResource* skeleton)
{
	SearchParentNode3D();
	if(!myBoneMatrixArray)
	{
		KIGS_WARNING("No bone matrix array to update", 0);
		return;
	}
	
	for (unsigned int i = 0; i < skeleton->GetGroupCount(); i++)
	{
		unsigned int id = skeleton->getID(i);
		unsigned int uid = skeleton->getUID(i);
		
		// get channel w/ UID
		ABoneChannel* channel = (ABoneChannel*)this->GetChannelByUID(uid);
		Matrix3x4 global_transform = channel->GetCurrentPRSMatrix();
		Matrix3x4 inv_bind_matrix = skeleton->getInvBindMatrix(i);
		myBoneMatrixArray[id - 1] = global_transform *inv_bind_matrix;
	}
}

#ifdef DRAW_DEBUG
#include "GLSLDebugDraw.h"

void ABoneSystem::DrawSkeletonRec(int current_index, const Matrix3x4& parent_transform, const Matrix3x4& root, bool need_draw)
{
	if(!mySkeleton) return;
	
	// get current bone data id 
	unsigned int gid = mySkeleton->getID(current_index);
	// get current bone data uid 
	unsigned int uid = mySkeleton->getUID(current_index);
	
	// get current channel w/ bone data uid
	ABoneChannel* channel = (ABoneChannel*)GetChannelByUID(uid);
	
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
	for (unsigned int i = 0; i<mySkeleton->GetGroupCount(); ++i)
	{
		if (mySkeleton->getFatherID(i) == gid)
		{
			DrawSkeletonRec(i, global_transform, root, true);
		}
	}
}
#endif

void	ABoneSystem::InitLocalToGlobalData()
{
	if (myParentNode3D == 0)
	{
		return;
	}
	
	const Matrix3x4& currentMatrix = myParentNode3D->GetLocal();
	
	ABaseStream* tmp_stream = GetValidStream();
	if (m_UseAnimationLocalToGlobal == false)
	{
		if(m_LinkedChannel)
		{
			ABaseStream* valid_stream = GetValidStream();
			if(valid_stream)
				valid_stream->CopyData(&m_pInstantLocalToGlobalData, m_LinkedChannel->GetChannelLocalToGlobalData());
			/*
			ABoneSystem* othersystem = (ABoneSystem*)lchannel->GetSystem();
			ABaseStream* valid_stream = GetValidStream();
			PRSKey m; m.set(othersystem->myParentNode3D->GetLocal());
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
	if (!myParentNode3D)
		return;
	
	if (m_UseAnimationLocalToGlobal || m_LinkedChannel)
	{
		Matrix3x4 m; m_pInstantLocalToGlobalData.get(m);
		myParentNode3D->ChangeMatrix(m);
	}
	
	
}