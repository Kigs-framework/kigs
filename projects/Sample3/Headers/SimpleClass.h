#pragma once
#include "CoreModifiable.h"
#include "CoreModifiableAttribute.h"
#include "maReference.h"


class SimpleClass : public CoreModifiable
{
public:
	DECLARE_CLASS_INFO(SimpleClass, CoreModifiable, Application);
	DECLARE_CONSTRUCTOR(SimpleClass);

	void	DoSomethingFun();

protected:

	virtual void NotifyUpdate(const u32 labelid) override;

	maInt			m_IntValue;
	maString		m_StringValue;
	maReference		m_Ref = BASE_ATTRIBUTE(Reference,"");

	maVect4DF		m_Vector4D = BASE_ATTRIBUTE(Vector, 0.0,0.0,1.0,2.0);
};

