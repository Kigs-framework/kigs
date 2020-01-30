#include "PrecompiledHeaders.h"
#include "CoreActionFunction.h"
#include "ModuleCoreAnimation.h"

template<>
inline void CoreActionFunction<kfloat, 1>::init(CoreSequence* sequence, CoreVector* params)
{
	myTarget = sequence->getTarget();
#ifdef _DEBUG // test parameters count
	if ((params->size()<3))
	{
		return;
	}
#endif
	float readfloat;
	(*params)[0]->getValue(readfloat);
	myDuration = readfloat;

	kstl::string readstring;
	(*params)[1]->getValue(readstring);

	// eval
	CoreItemSP eval = CoreItemOperator<kfloat>::Construct(readstring, myTarget, ModuleCoreAnimation::GetCoreItemOperatorConstructMap());
	if (!eval.isNil())
	{
		myFunctions[0] = eval;		
	}

	(*params)[2]->getValue(readstring);
	myTarget = checkSubTarget(readstring);
	myParamID = CharToID::GetID(readstring);
}

template<typename dataType, int dimension>
inline void CoreActionFunction<dataType, dimension>::init(CoreSequence* sequence, CoreVector* params)
{
	myTarget = sequence->getTarget();
#ifdef _DEBUG // test parameters count
	if ((params->size()<3))
	{
		return;
	}
#endif

	float readfloat;
	(*params)[0]->getValue(readfloat);
	myDuration = readfloat;

	kstl::string readstring;

	// if (*params)[1] is a vector, create each float CoreItemOperator 
	if ((*params)[1]->size() >= dimension)
	{
		int i;
		for (i = 0; i < dimension; i++)
		{
			readstring = (const kstl::string&)((*params)[1][i]);
			// eval
			CoreItemSP eval = CoreItemOperator<kfloat>::Construct(readstring, myTarget, ModuleCoreAnimation::GetCoreItemOperatorConstructMap());
			if (!eval.isNil())
			{
				myFunctions[i] = eval;
			}
		}
	}
	else if ((*params)[1]->size() == 1)
	{
		myHasUniqueMultidimensionnalFunc = true;
		
		(*params)[1]->getValue(readstring);
		// eval
		CoreItemSP eval = CoreItemOperator<dataType>::Construct(readstring, myTarget, ModuleCoreAnimation::GetCoreItemOperatorConstructMap());
		if (!eval.isNil())
		{
			myFunctions[0] = eval;
		}
	}

	(*params)[2]->getValue(readstring);
	myTarget = checkSubTarget(readstring);
	myParamID = CharToID::GetID(readstring);

}

template class CoreActionFunction<kfloat,1>;
template class CoreActionFunction<Point2D, 2>;
template class CoreActionFunction<Point3D, 3>;
template class CoreActionFunction<Vector4D, 4>;
