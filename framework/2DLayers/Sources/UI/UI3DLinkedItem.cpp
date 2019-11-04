#include "UI/UI3DLinkedItem.h"
#include "Camera.h"
#include "RenderingScreen.h"


//IMPLEMENT_AND_REGISTER_CLASS_INFO(UI3DLinkedItem, UI3DLinkedItem, 2DLayers);;
IMPLEMENT_CLASS_INFO(UI3DLinkedItem)

UI3DLinkedItem::UI3DLinkedItem(const kstl::string& name, CLASS_NAME_TREE_ARG)
	: UIItem(name, PASS_CLASS_NAME_TREE_ARG)
	, my3DPosition(*this, false, LABEL_AND_ID(3DPosition), 0.0f, 0.0f, 0.0f)
	, myCamera(*this, false, LABEL_AND_ID(Camera), "")
	, myNode(*this, false, LABEL_AND_ID(Node), "")
	, myUseUpOrientation(*this, false, LABEL_AND_ID(UseUpOrientation), false)
{
	
}

void	UI3DLinkedItem::InitModifiable()
{
	UIItem::InitModifiable();
	if (_isInit)
	{
		myNode.changeNotificationLevel(Owner);
		myCamera.changeNotificationLevel(Owner);
	}
}

void UI3DLinkedItem::NotifyUpdate(const unsigned int labelid)
{
	if (labelid == myNode.getLabelID() || labelid == myCamera.getLabelID())
	{
		if((CoreModifiable*)myCamera==NULL|| (CoreModifiable*)myNode ==NULL)
		{
			myIsHidden = false;
			myDock[0] = 0.5f;
			myDock[1] = 0.5f;
			myNeedUpdatePosition = true;
			PropagateNeedUpdateToFather();
		}
	}
	UIItem::NotifyUpdate(labelid);
}


void UI3DLinkedItem::Update(const Timer&  timer, void* addParam)
{
	Camera*	currentCamera = (Camera*)myCamera;
	if (currentCamera)
	{
		Point3D currentPos(my3DPosition[0], my3DPosition[1], my3DPosition[2]);

		// if a node3D is attached, then 3DPosition is a local position in node space
		Node3D*	currentnode = (Node3D*)(CoreModifiable*)myNode;
		if (currentnode)
		{
			//myIsHidden = !currentnode->IsVisible();

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

			kfloat objectinfront = Dot(camObjectVect, cameraView);
			if (objectinfront < 0)
			{
				myIsHidden = true;
			}
			else
			{

				if (myUseUpOrientation)
				{
					// get camera up
					Point3D	cameraUp;
					Point2D resultUp;
					currentCamera->GetUpVector(cameraUp.x, cameraUp.y, cameraUp.z);
					if (currentCamera->Project(resultUp.x, resultUp.y, currentPos + cameraUp))
					{
						resultUp -= result;
						resultUp.Normalize();
						myAngle = (fPI / 2.0f) - atan2f(resultUp.y, resultUp.x);
					}
				}

				myIsHidden = false;
				// TODO : at the moment, we suppose the UI3DLinkedItem is son of an UI covering the whole screen
				RenderingScreen* renderingScreen = currentCamera->getRenderingScreen();
				if (renderingScreen)
				{
					Point2D size;
					renderingScreen->GetSize(size.x, size.y);
					result.x /= size.x;
					result.y /= size.y;
				}

				myDock[0] = result.x;
				myDock[1] = result.y;
				myNeedUpdatePosition = true;
				PropagateNeedUpdateToFather();
			}
		}
		else
		{
			myIsHidden = true;
		}
	}
}
