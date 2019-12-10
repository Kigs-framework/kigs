#pragma once
#include "CoreModifiable.h"
#include <iostream>
#include "Core.h"
#include "AttributePacking.h"

class SimpleMaterialClass : public CoreModifiable
{
public:
	DECLARE_CLASS_INFO(SimpleMaterialClass, CoreModifiable, Application);
	DECLARE_INLINE_CONSTRUCTOR(SimpleMaterialClass) { std::cout << "SimpleMaterialClass constructor" << std::endl; }

protected:

	// RGB color 
	maVect3DF	m_Color = BASE_ATTRIBUTE(Color,1.0,0.0,0.0);
	// shininess 
	maFloat		m_Shininess = BASE_ATTRIBUTE(Shininess, 0.5);
};
