#pragma once

#include "CoreModifiable.h"

namespace Kigs
{
	namespace Core
	{
		// ****************************************
		// * CoreDecorator class
		// * --------------------------------------
		/**
		 * \file	CoreDecorator.h
		 * \class	CoreDecorator
		 * \ingroup Core
		 * \brief	 base class for decorator class
		 *
		 * Only used for Node3D Culling at the moment.
		 *
		 */
		 // ****************************************

		template<typename DecoratedType, typename returnType>
		class	DecoratedFuncBase
		{
		public:

			typedef returnType(DecoratedType::* MF)();

			typedef typename std::vector<MF>	mfVector;
			typedef typename mfVector::iterator mfVectorIt;

			DecoratedFuncBase(DecoratedType* localthis, MF mf)
				: mLocalThis(localthis)
				, mCurrentCallPos(0)
			{
				mStack.clear();
				push_back(mf);
			}

			returnType operator ()()
			{

				mCurrentCallPos = mStack.size() - 1;
				if (mCurrentCallPos >= 0)
				{
					(mLocalThis->*mStack[mCurrentCallPos])();
				}
			}

			returnType	father()
			{
				mCurrentCallPos--;
				if (mCurrentCallPos >= 0)
				{
					(mLocalThis->*mStack[mCurrentCallPos])();
				}
			}

			~DecoratedFuncBase()
			{
			}

			void	push_back(MF to_add)
			{
				mStack.push_back(to_add);
			}

			void	replace(MF to_replace, MF to_add)
			{
				if (mStack.size())
				{
					mfVectorIt itbegin = mStack.begin();
					mfVectorIt itend = mStack.end();
					while (itbegin != itend)
					{
						if ((*itbegin) == to_replace)
						{
							*itbegin = to_add;
							break;
						}
						++itbegin;
					}
				}
			}

			void	remove(MF to_remove)
			{
				mfVectorIt itbegin = mStack.begin();
				mfVectorIt itend = mStack.end();
				while (itbegin != itend)
				{
					if ((*itbegin) == to_remove)
					{
						mStack.erase(itbegin);
						break;
					}
					++itbegin;
				}
			}

		protected:

			int									mCurrentCallPos;
			mfVector							mStack;
			DecoratedType* mLocalThis;
		};

		//! weird, do not use base class ??
		template<typename DecoratedType, typename returnType, typename P1>
		class	DecoratedFuncBase1Param
		{
		public:

			typedef returnType(DecoratedType::* MF)(P1 param);
			typedef typename std::vector<MF>	mfVector;
			typedef typename mfVector::iterator mfVectorIt;

			DecoratedFuncBase1Param(DecoratedType* localthis, MF mf)
				: mLocalThis(localthis)
				, mCurrentCallPos(0)
			{
				mStack.clear();
				push_back(mf);
			}

			returnType operator ()(P1 param)
			{

				mCurrentCallPos = mStack.size() - 1;
				if (mCurrentCallPos >= 0)
				{
					(mLocalThis->*mStack[mCurrentCallPos])(param);
				}
			}

			returnType	father(P1 param)
			{
				mCurrentCallPos--;
				if (mCurrentCallPos >= 0)
				{
					(mLocalThis->*mStack[mCurrentCallPos])(param);
				}
			}

			~DecoratedFuncBase1Param()
			{
			}

			void	push_back(MF to_add)
			{
				mStack.push_back(to_add);
			}


			void	replace(MF to_replace, MF to_add)
			{
				if (mStack.size())
				{
					mfVectorIt itbegin = mStack.begin();
					mfVectorIt itend = mStack.end();
					while (itbegin != itend)
					{
						if ((*itbegin) == to_replace)
						{
							*itbegin = to_add;
							break;
						}
						++itbegin;
					}
				}
			}

			void	remove(MF to_remove)
			{
				mfVectorIt itbegin = mStack.begin();
				mfVectorIt itend = mStack.end();
				while (itbegin != itend)
				{
					if ((*itbegin) == to_remove)
					{
						mStack.erase(itbegin);
						break;
					}
					++itbegin;
				}
			}

		protected:

			int									mCurrentCallPos;
			mfVector							mStack;
			DecoratedType* mLocalThis;
		};

		//! weird, do not use base class ??
		template<typename DecoratedType, typename returnType, typename P1, typename P2>
		class	DecoratedFuncBase2Param
		{
		public:

			typedef returnType(DecoratedType::* MF)(P1 param1, P2 param2);
			typedef typename std::vector<MF>	mfVector;
			typedef typename mfVector::iterator mfVectorIt;

			DecoratedFuncBase2Param(DecoratedType* localthis, MF mf) :
				mCurrentCallPos(0)
				, mLocalThis(localthis)
			{
				mStack.clear();
				push_back(mf);
			}

			returnType operator ()(P1 param1, P2 param2)
			{

				mCurrentCallPos = (int)mStack.size() - 1;
				if (mCurrentCallPos >= 0)
				{
					return (mLocalThis->*mStack[mCurrentCallPos])(param1, param2);
				}

				return 0;
			}

			returnType	father(P1 param1, P2 param2)
			{
				mCurrentCallPos--;
				if (mCurrentCallPos >= 0)
				{
					return (mLocalThis->*mStack[mCurrentCallPos])(param1, param2);
				}

				return 0;
			}

			~DecoratedFuncBase2Param()
			{
			}

			void	push_back(MF to_add)
			{
				mStack.push_back(to_add);
			}


			void	replace(MF to_replace, MF to_add)
			{
				if (mStack.size())
				{
					mfVectorIt itbegin = mStack.begin();
					mfVectorIt itend = mStack.end();
					while (itbegin != itend)
					{
						if ((*itbegin) == to_replace)
						{
							*itbegin = to_add;
							break;
						}
						++itbegin;
					}
				}
			}

			void	remove(MF to_remove)
			{
				mfVectorIt itbegin = mStack.begin();
				mfVectorIt itend = mStack.end();
				while (itbegin != itend)
				{
					if ((*itbegin) == to_remove)
					{
						mStack.erase(itbegin);
						break;
					}
					++itbegin;
				}
			}

		protected:

			int									mCurrentCallPos;
			mfVector							mStack;
			DecoratedType* mLocalThis;
		};

		//! weird, do not use base class ??
		template<typename DecoratedType, typename returnType, typename P1, typename P2, typename P3>
		class	DecoratedFuncBase3Param
		{
		public:

			typedef returnType(DecoratedType::* MF)(P1 param1, P2 param2, P3 param3);
			typedef typename std::vector<MF>	mfVector;
			typedef typename mfVector::iterator mfVectorIt;

			DecoratedFuncBase3Param(DecoratedType* localthis, MF mf)
				: mLocalThis(localthis)
				, mCurrentCallPos(0)
			{
				mStack.clear();
				push_back(mf);
			}

			returnType operator ()(P1 param1, P2 param2, P3 param3)
			{

				mCurrentCallPos = mStack.size() - 1;
				if (mCurrentCallPos >= 0)
				{
					(mLocalThis->*mStack[mCurrentCallPos])(param1, param2, param3);
				}
			}

			returnType	father(P1 param1, P2 param2, P3 param3)
			{
				mCurrentCallPos--;
				if (mCurrentCallPos >= 0)
				{
					(mLocalThis->*mStack[mCurrentCallPos])(param1, param2, param3);
				}
			}

			~DecoratedFuncBase3Param()
			{
			}

			void	push_back(MF to_add)
			{
				mStack.push_back(to_add);
			}

			void	replace(MF to_replace, MF to_add)
			{
				if (mStack.size())
				{
					mfVectorIt itbegin = mStack.begin();
					mfVectorIt itend = mStack.end();
					while (itbegin != itend)
					{
						if ((*itbegin) == to_replace)
						{
							*itbegin = to_add;
							break;
						}
						++itbegin;
					}
				}
			}

			void	remove(MF to_remove)
			{
				mfVectorIt itbegin = mStack.begin();
				mfVectorIt itend = mStack.end();
				while (itbegin != itend)
				{
					if ((*itbegin) == to_remove)
					{
						mStack.erase(itbegin);
						break;
					}
					++itbegin;
				}
			}

		protected:

			int									mCurrentCallPos;
			mfVector							mStack;
			DecoratedType* mLocalThis;
		};


#define DECLARE_DECORABLE(returntype,funcname,baseClass) \
DecoratedFuncBase<baseClass,returntype>	funcname; \
typedef DecoratedFuncBase<baseClass,returntype> type_##funcname; 

#define DECLARE_DECORABLE_1_PARAMS(returntype,funcname,baseClass,param1type) \
DecoratedFuncBase1Param<baseClass,returntype,param1type>	funcname; \
typedef DecoratedFuncBase1Param<baseClass,returntype,param1type> type_##funcname; 

#define DECLARE_DECORABLE_2_PARAMS(returntype,funcname,baseClass,param1type,param2type) \
DecoratedFuncBase2Param<baseClass,returntype,param1type,param2type>	funcname; \
typedef DecoratedFuncBase2Param<baseClass,returntype,param1type,param2type> type_##funcname; 

#define DECLARE_DECORABLE_3_PARAMS(returntype,funcname,baseClass,param1type,param2type,param3type) \
DecoratedFuncBase3Param<baseClass,returntype,param1type,param2type,param3type>	funcname; \
typedef DecoratedFuncBase3Param<baseClass,returntype,param1type,param2type,param3type> type_##funcname; 

#define OVERLOAD_DECORABLE(funcname,baseClass,currentClass) \
	funcname.replace((type_##funcname::MF)&baseClass::Implement_##funcname,(type_##funcname::MF)&currentClass::Implement_##funcname)

#define DECLARE_DECORABLE_DEFINITION(returntype,funcname,...) \
	 returntype Implement_##funcname(__VA_ARGS__)

#define DECLARE_DECORABLE_IMPLEMENT(returntype,funcname,baseClass,...) \
	returntype baseClass::Implement_##funcname(__VA_ARGS__)

#define CONSTRUCT_DECORABLE(funcname,classType) \
	funcname(this,&classType::Implement_##funcname)

#define DECLARE_DECORATOR_DECORATE() \
	static bool	Decorate(CoreModifiable* cm)

#define DECLARE_DECORATOR_UNDECORATE() \
	static bool	UnDecorate(CoreModifiable* cm)

#define DECORATE_METHOD(funcname,baseClass,currentClass) \
	((baseClass*)cm)->funcname.push_back((type_##funcname::MF)&currentClass::Implement_##funcname);

#define UNDECORATE_METHOD(funcname,baseClass,currentClass) \
	((baseClass*)cm)->funcname.remove((type_##funcname::MF)&currentClass::Implement_##funcname);

#define ADD_DYNAMIC_METHOD(funcname,baseClass) \
	baseClass::CoreModifiableMethod<baseClass>*	dynMethod=new baseClass::CoreModifiableMethod<baseClass>(*(baseClass*)cm,(ModifiableMethod)&funcname,#funcname);

#define REMOVE_DYNAMIC_METHOD(funcname) \
	cm->RemoveMethod(#funcname);

#define IMPLEMENT_DYNAMIC_METHOD(funcname) \
	bool	funcname(CoreModifiable* sender,std::vector<CoreModifiableAttribute*>& params,void* privateParams)



	}
}