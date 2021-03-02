#pragma once

#include "CoreModifiable.h"

// a Wall is defined by a position and normal
class Wall
{
protected:

	v2f	mPos;
	v2f	mNormal;

public:
	Wall(v2f pos, v2f normal) : mPos(pos), mNormal(normal)
	{
		mNormal.Normalize();// normalize normal vector 
	}

	v2f	GetPos() const
	{
		return mPos;
	}
	v2f	GetNormal() const
	{
		return mNormal;
	}

};
