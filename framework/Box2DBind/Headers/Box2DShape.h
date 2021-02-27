#pragma once

#include "CoreModifiable.h"
#include "CoreModifiableAttribute.h"
#include "TecLibs/Tec3D.h"
#include "maCoreItem.h"

#include <box2D/b2_shape.h>

// ****************************************
// * Box2DShape class
// * --------------------------------------
/**
* \file	Box2DShape.h
* \class	Box2DShape
* \ingroup  Box2DBind
* \brief	Wrap a box2D shape 
*
*/
// ****************************************

class Box2DShape : public CoreModifiable
{
public:
	DECLARE_CLASS_INFO(Box2DShape, CoreModifiable, Box2DBind)
	DECLARE_CONSTRUCTOR(Box2DShape);
protected:
	void InitModifiable() override;

	virtual ~Box2DShape();

	b2Shape* mShape=nullptr;

	// shape type can not be changed after init
	maEnum<5>	mType = INIT_ATTRIBUTE(Type, "Circle", "Edge", "Polygon", "Chain" , "Box");

	// init params depends on the shape type
	maCoreItem mInitParams = INIT_ATTRIBUTE(InitParams);
};