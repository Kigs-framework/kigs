#include "PrecompiledHeaders.h"
#include "CoreActionFunction.h"
#include "ModuleCoreAnimation.h"

using namespace Kigs::Action;

template<>
inline void CoreActionFunction<float, 1>::init(CoreSequence* sequence, CoreVector* params)
{
	mTarget = sequence->getTarget();
	auto ptr = mTarget.lock();
	if (!ptr) return;

#ifdef _DEBUG // test parameters count
	if ((params->size()<3))
	{
		return;
	}
#endif
	float readfloat;
	(*params)[0]->getValue(readfloat);
	mDuration = readfloat;

	std::string readstring;
	(*params)[1]->getValue(readstring);

	// eval
	CoreItemSP eval = CoreItemOperator<float>::Construct(readstring, ptr.get(), ModuleCoreAnimation::GetCoreItemOperatorConstructMap());
	if (eval)
	{
		mFunctions[0] = eval;		
	}

	(*params)[2]->getValue(readstring);
	mTarget = checkSubTarget(readstring);
	mParamID = CharToID::GetID(readstring);
}

template<typename dataType, int dimension>
inline void CoreActionFunction<dataType, dimension>::init(CoreSequence* sequence, CoreVector* params)
{
	mTarget = sequence->getTarget();

	auto ptr = mTarget.lock();
	if (!ptr) return;

#ifdef _DEBUG // test parameters count
	if ((params->size()<3))
	{
		return;
	}
#endif

	float readfloat;
	(*params)[0]->getValue(readfloat);
	mDuration = readfloat;

	std::string readstring;

	// if (*params)[1] is a vector, create each float CoreItemOperator 
	if ((*params)[1]->size() >= dimension)
	{
		int i;
		for (i = 0; i < dimension; i++)
		{
			auto one = (*params)[1];
			auto two = (*one)[i];
			readstring = (std::string)*two;
			// eval
			CoreItemSP eval = CoreItemOperator<float>::Construct(readstring, ptr.get(), ModuleCoreAnimation::GetCoreItemOperatorConstructMap());
			if (eval)
			{
				mFunctions[i] = eval;
			}
		}
	}
	else if ((*params)[1]->size() == 1)
	{
		mHasUniqueMultidimensionnalFunc = true;
		
		(*params)[1]->getValue(readstring);
		// eval
		CoreItemSP eval = CoreItemOperator<dataType>::Construct(readstring, ptr.get(), ModuleCoreAnimation::GetCoreItemOperatorConstructMap());
		if (eval)
		{
			mFunctions[0] = eval;
		}
	}

	(*params)[2]->getValue(readstring);
	mTarget = checkSubTarget(readstring);
	mParamID = CharToID::GetID(readstring);

}

namespace Kigs
{
	namespace Action
	{
		template class CoreActionFunction<float, 1>;
		template class CoreActionFunction<Point2D, 2>;
		template class CoreActionFunction<Point3D, 3>;
		template class CoreActionFunction<Vector4D, 4>;
	}
}