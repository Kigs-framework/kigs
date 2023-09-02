#include "PrecompiledHeaders.h"

#include "AutoOrientedNode3D.h"
#include "TecLibs/Tec3D.h"
#include "CoreBaseApplication.h"

using namespace Kigs::Scene;

// connect to events and create attributes
void	AutoOrientedNode3DUp::Init(CoreModifiable* toUpgrade)
{
	// Connect notify update
	KigsCore::Connect(toUpgrade, "NotifyUpdate", toUpgrade, "AutoOrientedNotifyUpdate");

	mTarget = toUpgrade->AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE::WEAK_REFERENCE, "Target", "");
	toUpgrade->setOwnerNotification("Target", true);
	mCurrentTarget = nullptr;
	v3f axis(0.0f, 0.0f, 1.0f);
	mOrientedAxis = toUpgrade->AddDynamicVectorAttribute("OrientedAxis", (float*)axis.data(), 3);
	axis=v3f(0.0f, 1.0f, 0.0f);
	mPseudoConstantAxis = toUpgrade->AddDynamicVectorAttribute("PseudoConstantAxis", (float*)axis.data(), 3);
	mPseudoConstantAxisDir = toUpgrade->AddDynamicVectorAttribute("PseudoConstantAxisDir", (float*)axis.data(), 3);

	// check if already in auto update mode
	if (toUpgrade->isFlagAsAutoUpdateRegistered())
		mWasdAutoUpdate = true;
	else
		KigsCore::GetCoreApplication()->AddAutoUpdate(toUpgrade);
}

//  remove dynamic attributes and disconnect events
void	AutoOrientedNode3DUp::Destroy(CoreModifiable* toDowngrade, bool toDowngradeDeleted)
{
	if (toDowngradeDeleted) return;

	KigsCore::Disconnect(toDowngrade, "NotifyUpdate", toDowngrade, "CoordinateSystemNotifyUpdate");
	toDowngrade->RemoveDynamicAttribute("Target");
	toDowngrade->RemoveDynamicAttribute("OrientedAxis");
	toDowngrade->RemoveDynamicAttribute("PseudoConstantAxis");
	toDowngrade->RemoveDynamicAttribute("PseudoConstantAxisDir");
	mCurrentTarget = nullptr;
	if (!mWasdAutoUpdate)
		KigsCore::GetCoreApplication()->RemoveAutoUpdate(toDowngrade);
}

DEFINE_UPGRADOR_METHOD(AutoOrientedNode3DUp, AutoOrientedNotifyUpdate)
{
	if (!params.empty())
	{
		u32 labelID;
		params[1]->getValue(labelID, this);

		if (KigsID("Target") == labelID)
		{
			GetUpgrador()->mCurrentTarget = (Node3D*)(CoreModifiable*)(*(maReference*)(GetUpgrador()->mTarget));
		}

	}
	return false;
}

bool isValidAxis(const v3f& v)
{
	float na;
	na = v.squaredNorm();
	if ((na < 0.98f) || (na > 1.02f))
	{
		return false;
	}
	na = fabsf(v.x() + v.y() + v.z());
	if ((na < 0.99f) || (na > 1.01f))
	{
		return false;
	}
	return true;
}

// do orientation
DEFINE_UPGRADOR_UPDATE(AutoOrientedNode3DUp)
{
	// No target, no orientation
	if (!GetUpgrador()->mCurrentTarget)
	{
		return false;
	}

	// check that target axis & up axis are OK ( orthogonal, normalized and X, Y or Z axis colinear )
	v3f axis1,axis2,axis3;
	GetUpgrador()->mOrientedAxis->getValue(axis1, this);
	if(!isValidAxis(axis1))
	{
		KIGS_WARNING("AutoOrientedNode bad OrientedAxis", 1);
		return false;
	}
	
	GetUpgrador()->mPseudoConstantAxis->getValue(axis2, this);
	if (!isValidAxis(axis2))
	{
		KIGS_WARNING("AutoOrientedNode bad PseudoConstantAxis", 1);
		return false;
	}

	Maths::BBox	tstAxis(axis1);
	tstAxis.Update(axis2);
	
	float bboxdiagsqr = tstAxis.Size().squaredNorm();
	if ((bboxdiagsqr < 1.98f) || (bboxdiagsqr > 2.02f))
	{
		KIGS_WARNING("AutoOrientedNode bad axis", 1);
		return false;
	}

	// compute third vector (axis3) from axis1 and axis2
	axis3=axis1.cross(axis2);

	// Axis are OK, let's orient the node
	Node3D::SetupNodeIfNeeded();

	// make sure the target is up to date
	GetUpgrador()->mCurrentTarget->SetupNodeIfNeeded();

	// target pos in global coordinates
	v3f targetpos(*(v3f*)GetUpgrador()->mCurrentTarget->GetLocalToGlobal().e[3]);

	v3f	targetAxis1, targetAxis2, targetAxis3;
	// target vector in global coordinates
	targetAxis1 =targetpos;
	targetAxis1 -= GetLocalToGlobal().col(3);
	// now in father local coordinates
	getFather()->GetGlobalToLocal().TransformVector(&targetAxis1);
	targetAxis1.normalize();

	// get global targetAxis2
	GetUpgrador()->mPseudoConstantAxisDir->getValue(targetAxis2, this);
	// and transform it to father local coordinates
	getFather()->GetGlobalToLocal().TransformVector(&targetAxis2);
	targetAxis2.normalize();

	// if constant axis and target vector are too near, use mLastValidUpAxis
	v3f tstCross;
	tstCross=targetAxis1.cross(targetAxis2);
	if ((tstCross.squaredNorm()) < 0.1f)
	{
		targetAxis2 = GetUpgrador()->mLastValidUpAxis;
		getFather()->GetGlobalToLocal().TransformVector(&targetAxis2);
		targetAxis2.normalize();
	}

	// construct thirdVector (cross product of previous ones)
	targetAxis3 = targetAxis1.cross(targetAxis2);
	targetAxis3.normalize();
	// then compute constantAxis again to orthonormalize matrix
	targetAxis2=targetAxis3.cross(targetAxis1);

	mat3	tm1;
	tm1.col(0) = targetAxis1;
	tm1.col(1) = targetAxis2;
	tm1.col(2) = targetAxis3;

	mat3	tm2;
	tm2.col(0) = axis1;
	tm2.col(1) = axis2;
	tm2.col(2) = axis3;

	tm2.transposeInPlace();

	mat3 result(tm1 * tm2);
	
	mat3x4	targetm(result);
	targetm.SetTranslation(mTransform.GetTranslation());

	ChangeMatrix(targetm);

	// update mLastValidUpAxis 
	// TODO

	return false;
}
