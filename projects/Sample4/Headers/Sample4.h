#pragma once

#include <CoreBaseApplication.h>
#include "CoreModifiableAttribute.h"

class Sample4 : public CoreBaseApplication
{
public:
	DECLARE_CLASS_INFO(Sample4, CoreBaseApplication, Core);
	DECLARE_CONSTRUCTOR(Sample4);

protected:
	void	ProtectedInit() override;
	void	ProtectedUpdate() override;
	void	ProtectedClose() override;

	// add a string attribute
	maString m_StringParam = BASE_ATTRIBUTE("StringParam", "Sample4 string attribute");
};
