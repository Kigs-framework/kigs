#include "UI/UI3DLinkedItem.h"
#include "Camera.h"
#include "RenderingScreen.h"

using namespace Kigs::Draw2D;
using namespace Kigs::Draw;


IMPLEMENT_CLASS_INFO(UI3DLinkedItem)

UI3DLinkedItem::UI3DLinkedItem(const std::string& name, CLASS_NAME_TREE_ARG)
	: UIItem(name, PASS_CLASS_NAME_TREE_ARG)
	, m3DPosition(*this, false, "3DPosition", 0.0f, 0.0f, 0.0f)
	, mCamera(*this, false, "Camera", "")
	, mNode(*this, false, "Node", "")
	, mUseUpOrientation(*this, false, "UseUpOrientation", false)
{
	
}

void	UI3DLinkedItem::InitModifiable()
{
	UIItem::InitModifiable();
	if (_isInit)
	{
		mNode.changeNotificationLevel(Owner);
		mCamera.changeNotificationLevel(Owner);
	}
}

void UI3DLinkedItem::NotifyUpdate(const unsigned int labelid)
{
	if (labelid == mNode.getLabelID() || labelid == mCamera.getLabelID())
	{
		if((CoreModifiable*)mCamera==NULL|| (CoreModifiable*)mNode ==NULL)
		{
			mIsHidden = false;
			mDock[0] = 0.5f;
			mDock[1] = 0.5f;
			SetNodeFlag(Node2D_NeedUpdatePosition);
			PropagateNeedUpdateToFather();
		}
	}
	UIItem::NotifyUpdate(labelid);
}


void UI3DLinkedItem::Update(const Timer&  timer, void* addParam)
{
	Camera*	currentCamera = (Camera*)mCamera;
	if (currentCamera)
	{
		Point3D currentPos(m3DPosition[0], m3DPosition[1], m3DPosition[2]);

		// if a node3D is attached, then 3DPosition is a local position in node space
		Node3D*	currentnode = (Node3D*)(CoreModifiable*)mNode;
		if (currentnode)
		{
			Point3D toTransform(currentPos);
			currentnode->GetLocalToGlobal().TransformPoint(&toTransform, &currentPos);
		}

		Point2D result;
		if (currentCamera->Project(result.x, result.y, currentPos))
		{
			// fast check if point is in front
			Point3D	cameraView,cameraPos,camObjectVect;
			currentCamera->GetViewVector(cameraView.x, cameraView.y, cameraView.z);
			cameraView.Normalize();
			currentCamera->GetPosition(cameraPos.x, cameraPos.y, cameraPos.z);
			camObjectVect = currentPos;
			camObjectVect -=cameraPos;
			camObjectVect.Normalize();

			float objectinfront = Dot(camObjectVect, cameraView);
			if (objectinfront < 0)
			{
				mIsHidden = true;
			}
			else
			{

				if (mUseUpOrientation)
				{
					// get camera up
					Point3D	cameraUp;
					Point2D resultUp;
					currentCamera->GetUpVector(cameraUp.x, cameraUp.y, cameraUp.z);
					if (currentCamera->Project(resultUp.x, resultUp.y, currentPos + cameraUp))
					{
						resultUp -= result;
						resultUp.Normalize();
						mRotationAngle = (fPI / 2.0f) - atan2f(resultUp.y, resultUp.x);
					}
				}

				mIsHidden = false;
				// TODO : at the moment, we suppose the UI3DLinkedItem is son of an UI covering the whole screen
				RenderingScreen* renderingScreen = currentCamera->getRenderingScreen();
				if (renderingScreen)
				{
					Point2D size;
					renderingScreen->GetSize(size.x, size.y);
					result.x /= size.x;
					result.y /= size.y;
				}

				mDock[0] = result.x;
				mDock[1] = result.y;
				SetNodeFlag(Node2D_NeedUpdatePosition);
				PropagateNeedUpdateToFather();
			}
		}
		else
		{
			mIsHidden = true;
		}
	}
}
