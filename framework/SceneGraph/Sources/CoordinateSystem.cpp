#include "PrecompiledHeaders.h"

#include "CoordinateSystem.h"
#include "CoreBaseApplication.h"
#include "TecLibs/Tec3D.h"
#include "glm/gtx/matrix_decompose.hpp"

using namespace Kigs::Scene;

// connect to events and create attributes
void	CoordinateSystemUp::Init(CoreModifiable* toUpgrade)
{
	// Connect notify update
	KigsCore::Connect(toUpgrade, "NotifyUpdate", toUpgrade, "CoordinateSystemNotifyUpdate");

	// retreive current values ( suppose the matrix is a correct PRS matrix )
	mat4 current = ((Node3D*)toUpgrade)->GetLocal();
	v3f scale,pos, skew;
	quat rot;
	v4f perspective;

	decompose(current, scale, rot,pos, skew, perspective);

	rot = glm::conjugate(rot);

	glm::vec3 euler = glm::eulerAngles(rot) * 3.14159f / 180.f;

	mScale =toUpgrade->AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE::FLOAT, "Scale", 1.0f);
	toUpgrade->setOwnerNotification("Scale",true);
	mPos = toUpgrade->AddDynamicVectorAttribute("Position", (float*)&pos.x, 3);
	toUpgrade->setOwnerNotification("Position", true);
	mRot = toUpgrade->AddDynamicVectorAttribute("Rotation", (float*)&euler.x, 3);
	toUpgrade->setOwnerNotification("Rotation", true);

	// check if already in auto update mode
	/*
	if (toUpgrade->isFlagAsAutoUpdateRegistered())
		mWasdAutoUpdate = true;
	else
		KigsCore::GetCoreApplication()->AddAutoUpdate(toUpgrade);
	*/
	
}

//  remove dynamic attributes and disconnect events
void	CoordinateSystemUp::Destroy(CoreModifiable* toDowngrade, bool toDowngradeDeleted)
{
	if (toDowngradeDeleted) return;
	KigsCore::Disconnect(toDowngrade, "NotifyUpdate", toDowngrade, "CoordinateSystemNotifyUpdate");
	toDowngrade->RemoveDynamicAttribute("Scale");
	toDowngrade->RemoveDynamicAttribute("Position");
	toDowngrade->RemoveDynamicAttribute("Rotation");

	/*
	if (!mWasdAutoUpdate)
		KigsCore::GetCoreApplication()->RemoveAutoUpdate(toDowngrade);
	*/
}

DEFINE_UPGRADOR_METHOD(CoordinateSystemUp, CoordinateSystemNotifyUpdate)
{
	if (!params.empty())
	{
		u32 labelID;
		params[1]->getValue(labelID,this);
		
		if (KigsID("Scale") == labelID
			|| KigsID("Position") == labelID
			|| KigsID("Rotation") == labelID)
		{
			v3f	rot; GetUpgrador()->mRot->getValue(rot, this);
			v3f pos; GetUpgrador()->mPos->getValue(pos, this);
			float scale; GetUpgrador()->mScale->getValue(scale, this);

			mat4 matrix  = glm::eulerAngleYXZ(rot.y, rot.x, rot.z);
			glm::scale(matrix, { scale ,scale ,scale });
			translate(matrix, pos);
			ChangeMatrix(matrix);
		}

	}
	return false;
}

// WARNING ! Not tested
DEFINE_UPGRADOR_METHOD(CoordinateSystemUp, AngAxisRotate)
{

	// TODO
	/*
	if (!params.empty())
	{
		float angle;
		params[0]->getValue(angle, this);
		v3f axis;
		params[1]->getValue(axis, this);

		quat q;
		q.SetAngAxis(axis, angle);

		mat4	angAxis(q);

		v3f	rot;
		v3f pos;
		float scale;

		getValue("Rotation", rot);
		getValue("Position", pos);
		getValue("Scale", scale);

		mat4 matrix;
		matrix.SetRotationXYZ(rot.x, rot.y, rot.z);
		matrix.PreScale(scale, scale, scale);
		matrix.SetTranslation(pos);

		matrix.PostMultiply(angAxis);

		matrix.GetPRS(pos, rot, scale);

		setValue("Rotation", rot);
		setValue("Position", pos);
		setValue("Scale", scale);
	}*/
	return false;
}

DEFINE_UPGRADOR_METHOD(CoordinateSystemUp, localMoveNode)
{
	if (!params.empty())
	{
		v3f move;
		params[0]->getValue(move, this);

		localMove(move);
	}
	return false;
}

DEFINE_UPGRADOR_METHOD(CoordinateSystemUp, globalMoveNode)
{
	if (!params.empty())
	{
		v3f move;
		params[0]->getValue(move, this);

		globalMove(move);
	}
	return false;
}

DEFINE_UPGRADOR_UPDATE(CoordinateSystemUp)
{

	return false; // mmhh ?
	// TODO
	/*
	if (GetUpgrador()->mWasChanged)
	{
		v3f	rot;
		v3f pos;
		float scale;

		getValue("Rotation", rot);
		getValue("Position", pos);
		getValue("Scale", scale);

		mat4 matrix;
		matrix.SetRotationXYZ(rot.x,rot.y,rot.z);
		matrix.PreScale(scale, scale, scale);
		matrix.SetTranslation(pos);
		ChangeMatrix(matrix);

		GetUpgrador()->mWasChanged = false;
	}
	*/
}


void CoordinateSystemUp::toEuler(float x,float y,float z,float angle,float& heading,float& attitude,float& bank) 
{
	float s=sinf(angle);
	float c=cosf(angle);
	float t=1.0f-c;
	//  if axis is not already normalised then uncomment this
	// double magnitude = Math.sqrt(x*x + y*y + z*z);
	// if (magnitude==0) throw error;
	// x /= magnitude;
	// y /= magnitude;
	// z /= magnitude;
	if ((x*y*t + z*s) > 0.998f) { // north pole singularity detected
		heading = 2.0f*atan2f(x*sinf(angle*0.5f),cosf(angle*0.5f));
		attitude = glm::pi<float>()*0.5f;
		bank = 0.0f;
		return;
	}
	if ((x*y*t + z*s) < -0.998f) { // south pole singularity detected
		heading = -2.0f*atan2f(x*sinf(angle*0.5f),cosf(angle*0.5f));
		attitude = -glm::pi<float>()*0.5f;
		bank = 0.0f;
		return;
	}
	heading = atan2f(y * s- x * z * t , 1.0f - (y*y+ z*z ) * t);
	attitude = asinf(x * y * t + z * s) ;
	bank = atan2f(x * s - y * z * t , 1.0f - (x*x + z*z) * t);
}


// PIVOT Upgrador

// connect to events and create attributes
void	PivotUp::Init(CoreModifiable* toUpgrade)
{
	// Connect notify update
	KigsCore::Connect(toUpgrade, "NotifyUpdate", toUpgrade, "PivotNotifyUpdate");

	// store current matrix
	mInitMatrix = ((Node3D*)toUpgrade)->GetLocal();
	
	v3f PivotPosition(0, 0, 0);
	mAngle = toUpgrade->AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE::FLOAT, "Angle", 0.0f);
	toUpgrade->setOwnerNotification("Angle", true);
	mPivotPosition = toUpgrade->AddDynamicVectorAttribute("PivotPosition", (float*)&PivotPosition.x, 3);
	toUpgrade->setOwnerNotification("PivotPosition", true);
	PivotPosition = { 0.0f, 1.0f, 0.0f };
	mPivotAxis = toUpgrade->AddDynamicVectorAttribute("PivotAxis", (float*)&PivotPosition.x, 3);
	toUpgrade->setOwnerNotification("PivotAxis", true);
	mIsGlobal = toUpgrade->AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE::BOOL, "IsGlobal", false);
	toUpgrade->setOwnerNotification("IsGlobal", true);

	// check if already in auto update mode
	if (toUpgrade->isFlagAsAutoUpdateRegistered())
		mWasdAutoUpdate = true;
	else
		KigsCore::GetCoreApplication()->AddAutoUpdate(toUpgrade);


}

//  remove dynamic attributes and disconnect events
void	PivotUp::Destroy(CoreModifiable* toDowngrade, bool toDowngradeDeleted)
{
	if (toDowngradeDeleted) return;

	KigsCore::Disconnect(toDowngrade, "NotifyUpdate", toDowngrade, "PivotNotifyUpdate");
	toDowngrade->RemoveDynamicAttribute("Angle");
	toDowngrade->RemoveDynamicAttribute("PivotPosition");
	toDowngrade->RemoveDynamicAttribute("PivotAxis");
	toDowngrade->RemoveDynamicAttribute("IsGlobal");

	if (!mWasdAutoUpdate)
		KigsCore::GetCoreApplication()->RemoveAutoUpdate(toDowngrade);
}

DEFINE_UPGRADOR_UPDATE(PivotUp)
{
	// TODO
	/*
	if (GetUpgrador()->mWasChanged)
	{
		bool isGlobal;

		// get local matrix
		mat4 matrix = GetUpgrador()->mInitMatrix;
		v3f	ppos;
		v3f paxis;
		float	pangle;
		getValue("PivotPosition", ppos);
		getValue("PivotAxis", paxis);
		getValue("Angle", pangle);
		getValue("IsGlobal", isGlobal);

		if (isGlobal)
		{
			// move to local 
			mGlobalToLocal.TransformPoint(&ppos);
			mGlobalToLocal.TransformVector(&paxis);
		}

		quat q;
		q.SetAngAxis(paxis, pangle);

		mat4 transform(q);

		v3f originPos = matrix.GetTranslation();
		matrix.SetTranslation({ 0,0,0 });

		matrix = transform * matrix;

		originPos -= ppos;
		transform.TransformVector(&originPos);
		originPos += ppos;

		matrix.SetTranslation(originPos);

		ChangeMatrix(matrix);

		GetUpgrador()->mWasChanged = false;
	}*/
	return false;
}

DEFINE_UPGRADOR_METHOD(PivotUp, PivotNotifyUpdate)
{
	if (!params.empty())
	{
		u32 labelID;
		params[1]->getValue(labelID, this);

		if (KigsID("Angle") == labelID)
		{
			GetUpgrador()->mWasChanged = true;
		}
		if (KigsID("PivotPosition") == labelID)
		{
			// should the transform matrix be reset if pivot position or axis changes ?
			GetUpgrador()->mWasChanged = true;
		}
		if (KigsID("PivotAxis") == labelID)
		{
			// should the transform matrix be reset if pivot position or axis changes ?
			GetUpgrador()->mWasChanged = true;
		}
		if (KigsID("IsGlobal") == labelID)
		{
			GetUpgrador()->mWasChanged = true;
		}
	}
	return false;
}