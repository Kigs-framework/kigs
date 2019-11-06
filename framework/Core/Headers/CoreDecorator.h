#ifndef _COREDECORATOR_H
#define _COREDECORATOR_H

#include "CoreModifiable.h"

// ****************************************
// * CoreDecorator class
// * --------------------------------------
/**
 * \file	CoreDecorator.h
 * \class	CoreDecorator
 * \ingroup KigsCore
 * \brief	 base class for decorator class 
 * \author	ukn
 * \version ukn
 * \date	ukn
 */
// ****************************************

template<typename DecoratedType,typename returnType>
class	DecoratedFuncBase
{
public:
	
	typedef returnType (DecoratedType::*MF)();

	typedef typename std::vector<MF>	mfVector;
	typedef typename mfVector::iterator mfVectorIt;

	DecoratedFuncBase(DecoratedType* localthis,MF mf)
		: m_LocalThis(localthis)
		, m_CurrentCallPos(0)
	{
		m_stack.clear();
		push_back(mf);
	}

	returnType operator ()()
	{
	
		m_CurrentCallPos=m_stack.size()-1;
		if(m_CurrentCallPos>=0)
		{
			(m_LocalThis->*m_stack[m_CurrentCallPos])();
		}
	}
	
	returnType	father()
	{
		m_CurrentCallPos--;
		if(m_CurrentCallPos>=0)
		{
			(m_LocalThis->*m_stack[m_CurrentCallPos])();
		}
	}

	~DecoratedFuncBase()
	{
	}
	
	void	push_back(MF to_add)
	{
		m_stack.push_back(to_add);
	}

	void	replace(MF to_replace, MF to_add)
	{
		if (m_stack.size())
		{
			mfVectorIt itbegin = m_stack.begin();
			mfVectorIt itend = m_stack.end();
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
		mfVectorIt itbegin=m_stack.begin();
		mfVectorIt itend=m_stack.end();
		while(itbegin != itend)
		{
			if((*itbegin) == to_remove)
			{
				m_stack.erase(itbegin);
				break;
			}
			++itbegin;
		}
	}

protected:
	
	int									m_CurrentCallPos;
	mfVector							m_stack;
	DecoratedType*						m_LocalThis;
};

template<typename DecoratedType,typename returnType,typename P1>
class	DecoratedFuncBase1Param
{
public:
	
	typedef returnType (DecoratedType::*MF)(P1 param);
	typedef typename std::vector<MF>	mfVector;
	typedef typename mfVector::iterator mfVectorIt;

	DecoratedFuncBase1Param(DecoratedType* localthis,MF mf)
		: m_LocalThis(localthis)
		, m_CurrentCallPos(0)
	{
		m_stack.clear();
		push_back(mf);
	}

	returnType operator ()(P1 param)
	{
	
		m_CurrentCallPos=m_stack.size()-1;
		if(m_CurrentCallPos>=0)
		{
			(m_LocalThis->*m_stack[m_CurrentCallPos])(param);
		}
	}
	
	returnType	father(P1 param)
	{
		m_CurrentCallPos--;
		if(m_CurrentCallPos>=0)
		{
			(m_LocalThis->*m_stack[m_CurrentCallPos])(param);
		}
	}

	~DecoratedFuncBase1Param()
	{
	}
	
	void	push_back(MF to_add)
	{
		m_stack.push_back(to_add);
	}


	void	replace(MF to_replace, MF to_add)
	{
		if (m_stack.size())
		{
			mfVectorIt itbegin = m_stack.begin();
			mfVectorIt itend = m_stack.end();
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
		mfVectorIt itbegin=m_stack.begin();
		mfVectorIt itend=m_stack.end();
		while(itbegin != itend)
		{
			if((*itbegin) == to_remove)
			{
				m_stack.erase(itbegin);
				break;
			}
			++itbegin;
		}
	}

protected:
	
	int									m_CurrentCallPos;
	mfVector							m_stack;
	DecoratedType*						m_LocalThis;
};

template<typename DecoratedType,typename returnType,typename P1,typename P2>
class	DecoratedFuncBase2Param
{
public:
	
	typedef returnType (DecoratedType::*MF)(P1 param1,P2 param2);
	typedef typename std::vector<MF>	mfVector;
	typedef typename mfVector::iterator mfVectorIt;

	DecoratedFuncBase2Param(DecoratedType* localthis,MF mf): 
		  m_CurrentCallPos(0)
		, m_LocalThis(localthis)
	{
		m_stack.clear();
		push_back(mf);
	}

	returnType operator ()(P1 param1,P2 param2)
	{
	
		m_CurrentCallPos=(int)m_stack.size()-1;
		if(m_CurrentCallPos>=0)
		{
			return (m_LocalThis->*m_stack[m_CurrentCallPos])(param1,param2);
		}
        
        return 0;
	}
	
	returnType	father(P1 param1,P2 param2)
	{
		m_CurrentCallPos--;
		if(m_CurrentCallPos>=0)
		{
			return (m_LocalThis->*m_stack[m_CurrentCallPos])(param1,param2);
		}
        
        return 0;
	}

	~DecoratedFuncBase2Param()
	{
	}
	
	void	push_back(MF to_add)
	{
		m_stack.push_back(to_add);
	}


	void	replace(MF to_replace, MF to_add)
	{
		if (m_stack.size())
		{
			mfVectorIt itbegin = m_stack.begin();
			mfVectorIt itend = m_stack.end();
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
		mfVectorIt itbegin=m_stack.begin();
		mfVectorIt itend=m_stack.end();
		while(itbegin != itend)
		{
			if((*itbegin) == to_remove)
			{
				m_stack.erase(itbegin);
				break;
			}
			++itbegin;
		}
	}

protected:
	
	int									m_CurrentCallPos;
	mfVector							m_stack;
	DecoratedType*						m_LocalThis;
};

template<typename DecoratedType,typename returnType,typename P1,typename P2,typename P3>
class	DecoratedFuncBase3Param
{
public:
	
	typedef returnType (DecoratedType::*MF)(P1 param1,P2 param2,P3 param3);
	typedef typename std::vector<MF>	mfVector;
	typedef typename mfVector::iterator mfVectorIt;

	DecoratedFuncBase3Param(DecoratedType* localthis,MF mf)
		: m_LocalThis(localthis)
		, m_CurrentCallPos(0)
	{
		m_stack.clear();
		push_back(mf);
	}

	returnType operator ()(P1 param1,P2 param2,P3 param3)
	{
	
		m_CurrentCallPos=m_stack.size()-1;
		if(m_CurrentCallPos>=0)
		{
			(m_LocalThis->*m_stack[m_CurrentCallPos])(param1,param2,param3);
		}
	}
	
	returnType	father(P1 param1,P2 param2,P3 param3)
	{
		m_CurrentCallPos--;
		if(m_CurrentCallPos>=0)
		{
			(m_LocalThis->*m_stack[m_CurrentCallPos])(param1,param2,param3);
		}
	}

	~DecoratedFuncBase3Param()
	{
	}
	
	void	push_back(MF to_add)
	{
		m_stack.push_back(to_add);
	}

	void	replace(MF to_replace, MF to_add)
	{
		if (m_stack.size())
		{
			mfVectorIt itbegin = m_stack.begin();
			mfVectorIt itend = m_stack.end();
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
		mfVectorIt itbegin=m_stack.begin();
		mfVectorIt itend=m_stack.end();
		while(itbegin != itend)
		{
			if((*itbegin) == to_remove)
			{
				m_stack.erase(itbegin);
				break;
			}
			++itbegin;
		}
	}

protected:
	
	int									m_CurrentCallPos;
	mfVector							m_stack;
	DecoratedType*						m_LocalThis;
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
	bool	funcname(CoreModifiable* sender,kstl::vector<CoreModifiableAttribute*>& params,void* privateParams)




#endif // _COREDECORATOR_H