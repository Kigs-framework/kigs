#ifndef _NODE3DLODDECORATOR_H_
#define _NODE3DLODDECORATOR_H_

#include "Node3D.h"

class	Node3DLodDecorator : public Node3D
{
public:

	DECLARE_DECORATOR_DECORATE()
	{
		DECORATE_METHOD(Cull, Node3D, Node3DLodDecorator);
		cm->AddDynamicAttribute(CoreModifiable::ATTRIBUTE_TYPE::FLOAT, "LodCoef");
		cm->setValue(LABEL_TO_ID(LodCoef), 1.0);
		return true;
	}
	DECLARE_DECORATOR_UNDECORATE()
	{
		cm->RemoveDynamicAttribute("LodCoef");
		UNDECORATE_METHOD(Cull, Node3D, Node3DLodDecorator);
		return true;
	}

protected:
	DECLARE_DECORABLE_DEFINITION(bool, Cull, TravState* state, unsigned int cullingMask);
};




#endif //_NODE3DLODDECORATOR_H_