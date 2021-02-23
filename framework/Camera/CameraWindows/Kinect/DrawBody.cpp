#include "DrawBody.h"
#include "UI\UIImage.h"

IMPLEMENT_CLASS_INFO(DrawBody)
//! constructor
DrawBody::DrawBody(const kstl::string& name, CLASS_NAME_TREE_ARG) : CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG)
{

}

//! destructor
DrawBody::~DrawBody()
{
	
}


void DrawBody::relier(Point2D ptA, Point2D ptB, int index, int indexBody)
{
	//je place le rond
	bodyCircles[indexBody][index]->Set_Position(Point2D(ptA.x, ptA.y));

	//calcul de l'angle de l'os
	kfloat angle = atan((ptB.y - ptA.y) / (ptB.x - ptA.x));
	if ((ptB.x - ptA.x) < 0)
		angle += PI;
	//je place l'os sur le premier joint
	os[indexBody][index]->Set_Position(Point2D(ptA.x, ptA.y));
	//je le tourne
	os[indexBody][index]->Set_RotationAngle(angle);
	//	printf("%f\n", angle);

	//calcul de la longueur de l'os
	kfloat distanceAB = sqrt((ptB.x - ptA.x)*(ptB.x - ptA.x) + (ptB.y - ptA.y)*(ptB.y - ptA.y));
	//112 = largeur de l'image d'os
	kfloat scaleX = distanceAB / 112;
	os[indexBody][index]->Set_PreScale(scaleX, 1);
}

void DrawBody::InitDrawBody(kstl::string jointFileName, kstl::string boneFileName)
{
	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 25; j++)
		{
			bodyCircles[i][j] = (UIImage*)Core::GetInstanceOf("bodyCircles" + i, _S_2_ID("UIImage"));
			bodyCircles[i][j]->setValue(LABEL_TO_ID(Position), "{1000,1000}");
			bodyCircles[i][j]->setValue(LABEL_TO_ID(Anchor), "{0.5,0.5}");
			bodyCircles[i][j]->setValue(LABEL_TO_ID(Dock), "{0,0}");
			bodyCircles[i][j]->setValue(LABEL_TO_ID(Texture), jointFileName);
			bodyCircles[i][j]->setValue(LABEL_TO_ID(SizeX), 20);
			bodyCircles[i][j]->setValue(LABEL_TO_ID(SizeY), 20);
			bodyCircles[i][j]->setValue(LABEL_TO_ID(Priority), 1);
			bodyCircles[i][j]->setValue(LABEL_TO_ID(Opacity), 1);
			bodyCircles[i][j]->setValue(LABEL_TO_ID(IsHidden), false);
		}
	}

	for (int i = 0; i < 6; i++)
	{
		for (int j = 0; j < 24; j++)
		{
			os[i][j] = (UIImage*)Core::GetInstanceOf("os", _S_2_ID("UIImage"));
			os[i][j]->setValue(LABEL_TO_ID(Position), "{1000,1000}");
			os[i][j]->setValue(LABEL_TO_ID(Anchor), "{0,0.5}");
			os[i][j]->setValue(LABEL_TO_ID(Dock), "{0,0}");
			os[i][j]->setValue(LABEL_TO_ID(Texture), boneFileName);
			os[i][j]->setValue(LABEL_TO_ID(SizeX), 112);
			os[i][j]->setValue(LABEL_TO_ID(SizeY), 29);
			os[i][j]->setValue(LABEL_TO_ID(Priority), 1);
			os[i][j]->setValue(LABEL_TO_ID(Opacity), 1);
			os[i][j]->setValue(LABEL_TO_ID(IsHidden), false);
		}
	}

}

void DrawBody::DrawBodies(Point2D jointsCoords[][25])
{
	for (int i = 0; i < 6; i++)
	{

		if (jointsCoords[i][0].x != 100000 && jointsCoords[i][0].y != 100000)
		{
			// Torso
			relier(jointsCoords[i][JointType_Head], jointsCoords[i][JointType_Neck], 0, i);
			relier(jointsCoords[i][JointType_Neck], jointsCoords[i][JointType_SpineShoulder], 1, i);
			relier(jointsCoords[i][JointType_SpineShoulder], jointsCoords[i][JointType_SpineMid], 2, i);
			relier(jointsCoords[i][JointType_SpineMid], jointsCoords[i][JointType_SpineBase], 3, i);
			relier(jointsCoords[i][JointType_SpineShoulder], jointsCoords[i][JointType_ShoulderRight], 4, i);
			relier(jointsCoords[i][JointType_SpineShoulder], jointsCoords[i][JointType_ShoulderLeft], 5, i);
			relier(jointsCoords[i][JointType_SpineBase], jointsCoords[i][JointType_HipRight], 6, i);
			relier(jointsCoords[i][JointType_SpineBase], jointsCoords[i][JointType_HipLeft], 7, i);

			// Right Arm    
			relier(jointsCoords[i][JointType_ShoulderRight], jointsCoords[i][JointType_ElbowRight], 8, i);
			relier(jointsCoords[i][JointType_ElbowRight], jointsCoords[i][JointType_WristRight], 9, i);
			relier(jointsCoords[i][JointType_WristRight], jointsCoords[i][JointType_HandRight], 10, i);
			relier(jointsCoords[i][JointType_HandRight], jointsCoords[i][JointType_HandTipRight], 11, i);
			relier(jointsCoords[i][JointType_WristRight], jointsCoords[i][JointType_ThumbRight], 12, i);

			// Left Arm
			relier(jointsCoords[i][JointType_ShoulderLeft], jointsCoords[i][JointType_ElbowLeft], 13, i);
			relier(jointsCoords[i][JointType_ElbowLeft], jointsCoords[i][JointType_WristLeft], 14, i);
			relier(jointsCoords[i][JointType_WristLeft], jointsCoords[i][JointType_HandLeft], 15, i);
			relier(jointsCoords[i][JointType_HandLeft], jointsCoords[i][JointType_HandTipLeft], 16, i);
			relier(jointsCoords[i][JointType_WristLeft], jointsCoords[i][JointType_ThumbLeft], 17, i);

			// Right Leg
			relier(jointsCoords[i][JointType_HipRight], jointsCoords[i][JointType_KneeRight], 18, i);
			relier(jointsCoords[i][JointType_KneeRight], jointsCoords[i][JointType_AnkleRight], 19, i);
			relier(jointsCoords[i][JointType_AnkleRight], jointsCoords[i][JointType_FootRight], 20, i);

			// Left Leg
			relier(jointsCoords[i][JointType_HipLeft], jointsCoords[i][JointType_KneeLeft], 21, i);
			relier(jointsCoords[i][JointType_KneeLeft], jointsCoords[i][JointType_AnkleLeft], 22, i);
			relier(jointsCoords[i][JointType_AnkleLeft], jointsCoords[i][JointType_FootLeft], 23, i);
		}
		else{
			//si le body disparaît j'enlève le squelette
			for (int j = 0; j < 25; ++j)
			{
				bodyCircles[i][j]->Set_Position(Point2D(1000, 1000));
				if (j<24)
					os[i][j]->Set_Position(Point2D(1000, 1000));
			}
		}
	}
}