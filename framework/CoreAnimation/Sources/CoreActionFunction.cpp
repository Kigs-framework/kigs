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
		myFunctions[0] = eval.get();		
	}

	(*params)[2]->getValue(readstring);
	myTarget = checkSubTarget(readstring);
	myParamID = CharToID::GetID(readstring);
}

template<>
inline void CoreActionFunction<Point2D, 2>::init(CoreSequence* sequence, CoreVector* params)
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

	int i;
	for (i = 0; i < 2; i++)
	{
		readstring = (const kstl::string&)((*params)[1][i]);
		// eval
		CoreItemSP eval = CoreItemOperator<kfloat>::Construct(readstring, myTarget, ModuleCoreAnimation::GetCoreItemOperatorConstructMap());
		if (!eval.isNil())
		{
			myFunctions[i] = eval.get();
		}
	}
	(*params)[2]->getValue(readstring);
	myTarget = checkSubTarget(readstring);
	myParamID = CharToID::GetID(readstring);

}
template<>
inline void CoreActionFunction<Point3D, 3>::init(CoreSequence* sequence, CoreVector* params)
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

	int i;
	for (i = 0; i < 3; i++)
	{
		readstring = (const kstl::string&)((*params)[1][i]);
		// eval
		CoreItemSP eval = CoreItemOperator<kfloat>::Construct(readstring, myTarget, ModuleCoreAnimation::GetCoreItemOperatorConstructMap());
		if (!eval.isNil())
		{
			myFunctions[i] = eval.get();
		}
	}
	(*params)[2]->getValue(readstring);
	myTarget = checkSubTarget(readstring);
	myParamID = CharToID::GetID(readstring);

}

template<>
inline void CoreActionFunction<Quaternion, 4>::init(CoreSequence* sequence, CoreVector* params)
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

	int i;
	for (i = 0; i < 4; i++)
	{
		readstring = (const kstl::string&)((*params)[1][i]);
		// eval
		CoreItemSP eval = CoreItemOperator<kfloat>::Construct(readstring, myTarget, ModuleCoreAnimation::GetCoreItemOperatorConstructMap());
		if (!eval.isNil())
		{
			myFunctions[i] = eval.get();
		}
	}
	(*params)[2]->getValue(readstring);
	myTarget = checkSubTarget(readstring);
	myParamID = CharToID::GetID(readstring);

}

template class CoreActionFunction<kfloat,1>;
template class CoreActionFunction<Point2D, 2>;
template class CoreActionFunction<Point3D, 3>;
template class CoreActionFunction<Quaternion, 4>;
