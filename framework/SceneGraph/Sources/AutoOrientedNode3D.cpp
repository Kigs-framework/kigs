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
	mOrientedAxis = toUpgrade->AddDynamicVectorAttribute("OrientedAxis", (float*)&axis.x, 3);
	axis = { 0.0f, 1.0f, 0.0f };
	mPseudoConstantAxis = toUpgrade->AddDynamicVectorAttribute("PseudoConstantAxis", (float*)&axis.x, 3);
	mPseudoConstantAxisDir = toUpgrade->AddDynamicVectorAttribute("PseudoConstantAxisDir", (float*)&axis.x, 3);

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
	na = length2(v);
	if ((na < 0.98f) || (na > 1.02f))
	{
		return false;
	}
	na = fabsf(v.x + v.y + v.z);
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

	BBox	tstAxis(axis1);
	tstAxis.Update(axis2);
	
	float bboxdiagsqr = length2(tstAxis.Size());
	if ((bboxdiagsqr < 1.98f) || (bboxdiagsqr > 2.02f))
	{
		KIGS_WARNING("AutoOrientedNode bad axis", 1);
		return false;
	}

	// compute third vector (axis3) from axis1 and axis2
	axis3=cross(axis1, axis2);

	// Axis are OK, let's orient the node
	Node3D::SetupNodeIfNeeded();

	// make sure the target is up to date
	GetUpgrador()->mCurrentTarget->SetupNodeIfNeeded();

	// target pos in global coordinates
	v3f targetpos(column(GetUpgrador()->mCurrentTarget->GetLocalToGlobal(),3));

	v3f	targetAxis1, targetAxis2, targetAxis3;
	// target vector in global coordinates
	targetAxis1 =targetpos;
	targetAxis1 -= v3f(column(GetLocalToGlobal(),3));
	// now in father local coordinates
	transformVector(getFather()->GetGlobalToLocal(),targetAxis1);
	targetAxis1 = normalize(targetAxis1);

	// get global targetAxis2
	GetUpgrador()->mPseudoConstantAxisDir->getValue(targetAxis2, this);
	// and transform it to father local coordinates
	transformVector(getFather()->GetGlobalToLocal(),targetAxis2);
	targetAxis2 = normalize(targetAxis2);

	// if constant axis and target vector are too near, use mLastValidUpAxis
	v3f tstCross;
	tstCross=cross(targetAxis1, targetAxis2);
	if (length2(tstCross) < 0.1f)
	{
		targetAxis2 = GetUpgrador()->mLastValidUpAxis;
		transformVector(getFather()->GetGlobalToLocal(),targetAxis2);
		targetAxis2 = normalize(targetAxis2);
	}

	// construct thirdVector (cross product of previous ones)
	targetAxis3=cross(targetAxis1, targetAxis2);
	targetAxis3 = normalize(targetAxis3);
	// then compute constantAxis again to orthonormalize matrix
	targetAxis2=cross(targetAxis3, targetAxis1);

	mat3	tm1;
	column(tm1, 0, targetAxis1);
	column(tm1, 1, targetAxis2);
	column(tm1, 2, targetAxis3);

	mat3	tm2;
	column(tm2, 0, axis1);
	column(tm2, 1, axis2);
	column(tm2, 2, axis3);

	tm2 = transpose(tm2);

	mat3 result(tm1 * tm2);
	
	mat4	targetm(result);
	column(targetm, 3, column(mTransform,3));

	ChangeMatrix(targetm);

	// update mLastValidUpAxis 
	// TODO

	return false;
}
