#include "PrecompiledHeaders.h"

#include "AutoOrientedNode3D.h"
#include "TecLibs/Tec3D.h"


//IMPLEMENT_AND_REGISTER_CLASS_INFO(AutoOrientedNode3D, AutoOrientedNode3D, SceneGraph);
IMPLEMENT_CLASS_INFO(AutoOrientedNode3D)

IMPLEMENT_CONSTRUCTOR(AutoOrientedNode3D)
,myTargetName(*this,false,"TargetName","")
,myPosX(*this,false,"PositionX",0.0f)
,myPosY(*this,false,"PositionY",0.0f)
,myPosZ(*this,false,"PositionZ",0.0f)
,myOrientedAxis(*this,true,"OrientedAxis","X","Y","Z")
,myPseudoConstantAxis(*this,true,"PseudoConstantAxis","X","Y","Z")
,myPseudoConstantAxisDir(*this,true,"PseudoConstantAxisDir",0.0f,0.0f,1.0f)
{
   	myCurrentTarget=0; 
	myPseudoConstantAxis.setValue("X");
	myLastTargetPos.Set(-FLT_MAX, -FLT_MAX, -FLT_MAX);
}     
 

void AutoOrientedNode3D::setPosition(kfloat posX,kfloat posY,kfloat posZ)
{
	myPosX = posX;
	myPosY = posY;
	myPosZ = posZ;
	auto matrix = GetLocal();
	matrix.SetTranslation({ posX, posY, posZ });
	ChangeMatrix(matrix);
}

void AutoOrientedNode3D::getPosition(kfloat& posX,kfloat& posY,kfloat& posZ) const
{
	posX = myPosX;
	posY = myPosY;
	posZ = myPosZ;
}

void	AutoOrientedNode3D::DoOrientation()
{
	bool searchtarget=false;

	if(myCurrentTarget)
	{
		if(myCurrentTarget->getName() != myTargetName.const_ref())
		{
			// search target
			searchtarget=true;
		}
	}
	else 
	{
		searchtarget=true;
	}

	if(searchtarget)
	{
		kstl::vector<CMSP>	instances=	GetInstancesByName("Node3D",myTargetName.const_ref());

		if(instances.size())
		{
			myCurrentTarget=(Node3D*)(*instances.begin()).get();
		}
		else
		{
			myCurrentTarget=0;
		}
	}

	if(myCurrentTarget)
	{
		// check if this node is up to date
		Node3D::SetUpNode();

		// make sure the target is up to date
		myCurrentTarget->SetupNodeIfNeeded();

		// retreive target position in my space

		Point3D targetpos(*(Point3D*)myCurrentTarget->GetLocalToGlobal().e[3]);

		

		GetGlobalToLocal().TransformPoints(&targetpos,1);

		auto matrix = GetLocal();

		kfloat dist=Dist(targetpos,myLastTargetPos);
		
		Vector3D	previouspos=*(Vector3D*)matrix.e[3];

		if(dist>0.001)
		{
			myLastTargetPos=targetpos;

			// global view direction
			targetpos=(*(Point3D*)myCurrentTarget->GetLocalToGlobal().e[3]);
			targetpos-=(*(Point3D*)GetLocalToGlobal().e[3]);

			// need to orient

			Vector3D	alignment(targetpos);
			Vector3D	constant(*((Vector3D*)myPseudoConstantAxisDir.getVector()));

			alignment.Normalize();

			Vector3D thirdv;

			thirdv.CrossProduct(constant,alignment);
			dist=Norm(thirdv);

			thirdv*=1.0f/(kfloat)dist;

			Vector3D newconstant(alignment^thirdv);

			switch((int)myOrientedAxis)
			{
			case 0: // X
				(*(Vector3D*)matrix.e[0])=alignment;


				if(dist>0.0001) // if alignement equal the constant vector, then keep the old one
				{
					switch((int)myPseudoConstantAxis)
					{
					case 0: // very bad !
					case 2:
						(*(Vector3D*)matrix.e[2])=newconstant;
						(*(Vector3D*)matrix.e[1])=thirdv;
						break;
					case 1:
						(*(Vector3D*)matrix.e[1])=newconstant;
						(*(Vector3D*)matrix.e[2])=-thirdv;
						break;
					}
				}
				
				break;
			case 1: // Y
				(*(Vector3D*)matrix.e[1])=alignment;
				dist=Dist(alignment,constant);
				if(dist>0.0001) // if alignement equal the constant vector, then keep the old one
				{
					switch((int)myPseudoConstantAxis)
					{
					case 1: // very bad !
					case 2:
						(*(Vector3D*)matrix.e[2])=newconstant;
						(*(Vector3D*)matrix.e[0])=-thirdv;
						break;
					case 0:
						(*(Vector3D*)matrix.e[0])=newconstant;
						(*(Vector3D*)matrix.e[2])=thirdv;
						break;
					}
				}
				break;
			case 2: // Z
				(*(Vector3D*)matrix.e[2])=alignment;
				dist=Dist(alignment,constant);
				if(dist>0.0001) // if alignement equal the constant vector, then keep the old one
				{
					switch((int)myPseudoConstantAxis)
					{
					case 2: // very bad !
					case 0:
						(*(Vector3D*)matrix.e[0])=newconstant;
						(*(Vector3D*)matrix.e[1])=-thirdv;
						break;
					case 1:
						(*(Vector3D*)matrix.e[1])=newconstant;
						(*(Vector3D*)matrix.e[0])=thirdv;
						break;
					}
				}
				break;
			}

			const kstl::vector<CoreModifiable*>& parents=GetParents();
			kstl::vector<CoreModifiable*>::const_iterator it;

			for(it=parents.begin();it!=parents.end();++it)
			{
			  if((*it)->isSubType(Node3D::myClassID))
			  {
				Node3D* father=(Node3D*)(*it);
			  
				father->SetupNodeIfNeeded();
				// multiply by father global to local
				matrix *= father->GetGlobalToLocal();
				// a node3D should only have one node3D father...
				break;
			  }
			}
			
			// reset translation
			matrix.SetTranslation(previouspos);
			ChangeMatrix(matrix);
		}
		
	}

}

void     AutoOrientedNode3D::TravCull(TravState* state)
{
	DoOrientation();
	ParentClassType::TravCull(state);
}    
