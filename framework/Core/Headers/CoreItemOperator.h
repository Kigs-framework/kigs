#ifndef _COREITEMOPERATOR_H
#define _COREITEMOPERATOR_H

#include "CoreVector.h"
#include "CoreValue.h"
#include "AsciiParserUtils.h"

struct CoreItemOperatorStruct
{
	char	myOp;
	int		myPos;
	int		mySize;
	CoreItemSP myOp1 = CoreItemSP(nullptr);
	CoreItemSP myOp2 = CoreItemSP(nullptr);
	int		myPriority;
};

struct CoreItemEvaluationContext
{
	kigs::unordered_map<unsigned int, GenericRefCountedBaseClass*>	myVariableList;
};

extern	CoreItemEvaluationContext*	myCurrentCoreItemEvaluationContext;
extern void	SetCoreItemOperatorContext(CoreItemEvaluationContext* set);
extern void	ReleaseCoreItemOperatorContext();

typedef     CoreVector* (*CoreItemOperatorCreateMethod)();

class SpecificOperator
{
public:
	kstl::string	myKeyWord;	// 3 letter keyword
	CoreItemOperatorCreateMethod	myCreateMethod;
};

class ConstructContext
{
public:
	kigs::unordered_map<kstl::string, CoreItemOperatorCreateMethod>	myMap;
	CoreModifiable* myTarget;
	kstl::vector<SpecificOperator>* mySpecificList;
};

template<typename operandType>
class CoreItemOperator : public CoreVector
{
public:

	virtual inline operator bool() const override
	{
		KIGS_ERROR("cast operator called on base CoreItem", 2);
		return false;
	}

	virtual inline operator kfloat() const override
	{
		KIGS_ERROR("cast operator called on base CoreItem", 2);
		return 0.0f;
	}

	virtual inline operator int() const override
	{
		KIGS_ERROR("cast operator called on base CoreItem", 2);
		return 0;
	}

	virtual inline operator unsigned int() const override
	{
		KIGS_ERROR("cast operator called on base CoreItem", 2);
		return 0;
	}

	virtual inline operator kstl::string() const override
	{
		KIGS_ERROR("cast operator called on base CoreItem", 2);
		return "";
	}

	virtual inline operator usString() const override
	{
		KIGS_ERROR("cast operator called on base CoreItem", 2);
		return usString("");
	}

	virtual inline operator Point2D() const override
	{
		Point2D result;
		KIGS_ERROR("cast operator called on base CoreItem", 2);
		return result;
	}

	virtual inline operator Point3D() const override
	{
		Point3D result;
		KIGS_ERROR("cast operator called on base CoreItem", 2);
		return result;
	}

	virtual inline operator Vector4D() const override
	{
		Vector4D result;
		KIGS_ERROR("cast operator called on base CoreItem", 2);
		return result;
	}

	static void	defaultOperandTypeInit(operandType& _value)
	{
		_value = (operandType)0;
	}

	static CoreItemSP	Construct(const kstl::string& formulae, CoreModifiable* target, kstl::vector<SpecificOperator>* specificList=0);
	static CoreItemSP	Construct(const kstl::string& formulae, CoreModifiable* target, const kigs::unordered_map<kstl::string, CoreItemOperatorCreateMethod>&	myMap);
	static void	ConstructContextMap(kigs::unordered_map<kstl::string, CoreItemOperatorCreateMethod>&	myMap, kstl::vector<SpecificOperator>* specificList = 0);
	static CoreItemSP	Parse(AsciiParserUtils& formulae, ConstructContext& context);

protected:

	static bool	CheckAffectation(char prevChar, int priority, AsciiParserUtils& block, kstl::vector<CoreItemOperatorStruct>& OperatorList);

	static kstl::vector<CoreItemOperatorStruct>	FindFirstLevelOperators(AsciiParserUtils& formulae, ConstructContext& context);
	static kstl::vector<CoreItemOperatorStruct>	FindFirstLevelSeparator(AsciiParserUtils& formulae, ConstructContext& context);
	static kstl::vector<kstl::string>	FindFirstLevelParams(AsciiParserUtils& formulae, ConstructContext& context);
	static CoreItemOperator<operandType>* getOperator(const kstl::string& keyword, ConstructContext& context);
	static GenericRefCountedBaseClass* getVariable(const kstl::string& keyword);
};


template<>
inline CoreItemOperator<bool>::operator kfloat() const
{
	bool result = (bool)*this;
	return result?1.0f:0.0f;
}

template<>
inline CoreItemOperator<bool>::operator int() const
{
	bool result = (bool)*this;
	return result ? 1 : 0;
}

template<>
inline CoreItemOperator<bool>::operator unsigned int() const
{
	bool result = (bool)*this;
	return result ? 1 : 0;
}

template<>
inline CoreItemOperator<bool>::operator kstl::string() const
{
	bool result = (bool)*this;
	return result ? "true" : "false";
}

template<>
inline CoreItemOperator<bool>::operator usString() const
{
	bool result = (bool)*this;
	return result ? usString("true") : usString("false");
}


template<>
inline CoreItemOperator<kfloat>::operator bool() const
{
	kfloat result = (kfloat)*this;
	return result ? true : false;
}

template<>
inline CoreItemOperator<kfloat>::operator int() const
{
	kfloat result = (kfloat)*this;
	return (int)result;
}

template<>
inline CoreItemOperator<kfloat>::operator unsigned int() const
{
	kfloat result = (kfloat)*this;
	return (unsigned int)result;
}

template<>
inline void	CoreItemOperator<kstl::string>::defaultOperandTypeInit(kstl::string& _value)
{
	_value = "";
}

template<>
inline void	CoreItemOperator<Point2D>::defaultOperandTypeInit(Point2D& _value)
{
	_value.Set(0.0f, 0.0f);
}

template<>
inline void	CoreItemOperator<Point3D>::defaultOperandTypeInit(Point3D& _value)
{
	_value.Set(0.0f, 0.0f,0.0f);
}

template<>
inline void	CoreItemOperator<Vector4D>::defaultOperandTypeInit(Vector4D& _value)
{
	_value.Set(0.0f, 0.0f, 0.0f,0.0f);
}



// just evaluate each operand and return the last evaluated one
template<typename operandType>
class InstructionListOperator : public CoreItemOperator<operandType>
{
public:

	virtual inline operator operandType() const
	{
		kstl::vector<CoreItemSP>::const_iterator itOperand = CoreVector::myVector.begin();
		kstl::vector<CoreItemSP>::const_iterator itOperandEnd = CoreVector::myVector.end();

		operandType	result;
		while (itOperand != itOperandEnd)
		{
			result = (*itOperand)->operator operandType();
			++itOperand;
		}
		return result;
	}

};

template<typename operandType>
class AddOperator : public CoreItemOperator<operandType>
{
public:

	virtual inline operator operandType() const
	{
		kstl::vector<CoreItemSP>::const_iterator itOperand = CoreVector::myVector.begin();
		kstl::vector<CoreItemSP>::const_iterator itOperandEnd = CoreVector::myVector.end();

		operandType	result((*itOperand)->operator operandType());
		++itOperand;
		while (itOperand != itOperandEnd)
		{
			result += (*itOperand)->operator operandType();
			++itOperand;
		}
		return result;
	}

};

template <  >
inline AddOperator<kstl::string>::operator kstl::string() const
{
	kstl::vector<CoreItemSP>::const_iterator itOperand = CoreVector::myVector.begin();
	kstl::vector<CoreItemSP>::const_iterator itOperandEnd = CoreVector::myVector.end();

	kstl::string	result("");

	while (itOperand != itOperandEnd)
	{
		result += (kstl::string)(*itOperand);
		++itOperand;
	}
	return result;
}

template<typename operandType>
class SubOperator : public CoreItemOperator<operandType>
{
public:

	virtual inline operator operandType() const
	{
		kstl::vector<CoreItemSP>::const_iterator itOperand = CoreVector::myVector.begin();
		kstl::vector<CoreItemSP>::const_iterator itOperandEnd = CoreVector::myVector.end();

		operandType	result((*itOperand)->operator operandType());
		++itOperand;
		while (itOperand != itOperandEnd)
		{
			result -= (*itOperand)->operator operandType();
			++itOperand;
		}
		return result;
	}

};

template <  >
inline SubOperator<kstl::string>::operator kstl::string() const
{
	kstl::vector<CoreItemSP>::const_iterator itOperand = CoreVector::myVector.begin();

	kstl::string	result((kstl::string)(*itOperand));

	// no sub on string
	return result;
}


template<typename operandType>
class MultOperator : public CoreItemOperator<operandType>
{
public:

	virtual inline operator operandType() const
	{
		kstl::vector<CoreItemSP>::const_iterator itOperand = CoreVector::myVector.begin();
		kstl::vector<CoreItemSP>::const_iterator itOperandEnd = CoreVector::myVector.end();

		operandType	result((*itOperand)->operator operandType());
		++itOperand;
		while (itOperand != itOperandEnd)
		{
			result *= (*itOperand)->operator operandType();
			++itOperand;
		}
		return result;
	}

};

template <  >
inline MultOperator<kstl::string>::operator kstl::string() const
{
	kstl::vector<CoreItemSP>::const_iterator itOperand = CoreVector::myVector.begin();

	kstl::string	result((kstl::string)(*itOperand));

	// no mult on string
	return result;
}


template<typename operandType>
class DivOperator : public CoreItemOperator<operandType>
{
public:

	virtual inline operator operandType() const
	{
		kstl::vector<CoreItemSP>::const_iterator itOperand = CoreVector::myVector.begin();
		kstl::vector<CoreItemSP>::const_iterator itOperandEnd = CoreVector::myVector.end();

		operandType	result((*itOperand)->operator operandType());
		++itOperand;
		while (itOperand != itOperandEnd)
		{
			result /= (*itOperand)->operator operandType();
			++itOperand;
		}
		return result;
	}

};

template <  >
inline DivOperator<kstl::string>::operator kstl::string() const
{
	kstl::vector<CoreItemSP>::const_iterator itOperand = CoreVector::myVector.begin();

	kstl::string	result((kstl::string)(*itOperand));

	// no div on string
	return result;
}


template<typename operandType>
class AbsOperator : public CoreItemOperator<operandType>
{
public:

	virtual inline operator operandType() const
	{
		kstl::vector<CoreItemSP>::const_iterator itOperand = CoreVector::myVector.begin();
		
		operandType	result((operandType)(*itOperand));

		return (result<(operandType)0) ? (-result) : result;
	}

	static CoreVector* create()
	{
		return new AbsOperator<operandType>();
	}

};

template <  >
inline AbsOperator<kstl::string>::operator kstl::string() const
{
	kstl::vector<CoreItemSP>::const_iterator itOperand = CoreVector::myVector.begin();

	kstl::string	result((kstl::string)(*itOperand));

	// no abs on string
	return result;
}


template<typename operandType>
class MaxOperator : public CoreItemOperator<operandType>
{
public:

	virtual inline operator operandType() const
	{

		kstl::vector<CoreItemSP>::const_iterator itOperand = CoreVector::myVector.begin();
		kstl::vector<CoreItemSP>::const_iterator itOperandEnd = CoreVector::myVector.end();

		operandType	result((operandType)(*itOperand));
		++itOperand;
		while (itOperand != itOperandEnd)
		{
			operandType current = (operandType)(*itOperand);
			if (current > result)
				result = current;
			++itOperand;
		}
		return result;
	}

	static CoreVector* create()
	{
		return new MaxOperator<operandType>();
	}
};

template <  >
inline MaxOperator<kstl::string>::operator kstl::string() const
{
	kstl::vector<CoreItemSP>::const_iterator itOperand = CoreVector::myVector.begin();

	kstl::string	result((kstl::string)(*itOperand));

	// no max on string
	return result;
}

template<typename operandType>
class MinOperator : public CoreItemOperator<operandType>
{
public:

	virtual inline operator operandType() const
	{

		kstl::vector<CoreItemSP>::const_iterator itOperand = CoreVector::myVector.begin();
		kstl::vector<CoreItemSP>::const_iterator itOperandEnd = CoreVector::myVector.end();

		operandType	result((operandType)(*itOperand));
		++itOperand;
		while (itOperand != itOperandEnd)
		{
			operandType current = (operandType)(*itOperand);
			if (current < result)
				result = current;
			++itOperand;
		}
		return result;
	}

	static CoreVector* create()
	{
		return new MinOperator<operandType>();
	}
};

template <  >
inline MinOperator<kstl::string>::operator kstl::string() const
{
	kstl::vector<CoreItemSP>::const_iterator itOperand = CoreVector::myVector.begin();

	kstl::string	result((kstl::string)(*itOperand));

	// no min on string
	return result;
}



template<typename operandType>
class SinusOperator : public CoreItemOperator<operandType>
{
public:

	virtual inline operator operandType() const
	{
		kstl::vector<CoreItemSP>::const_iterator itOperand = CoreVector::myVector.begin();

		operandType	result((operandType)(*itOperand));

		return sinf(result);
	}

	static CoreVector* create()
	{
		return new SinusOperator<operandType>();
	}
};

template<typename operandType>
class CosinusOperator : public CoreItemOperator<operandType>
{
public:

	virtual inline operator operandType() const
	{
		kstl::vector<CoreItemSP>::const_iterator itOperand = CoreVector::myVector.begin();

		operandType	result((operandType)(*itOperand));

		return cosf(result);
	}

	static CoreVector* create()
	{
		return new CosinusOperator<operandType>();
	}

};

template<typename operandType>
class TangentOperator : public CoreItemOperator<operandType>
{
public:

	virtual inline operator operandType() const
	{
		kstl::vector<CoreItemSP>::const_iterator itOperand = CoreVector::myVector.begin();

		operandType	result((operandType)(*itOperand));

		return tanf(result);
	}

	static CoreVector* create()
	{
		return new TangentOperator<operandType>();
	}

};


template<typename operandType>
class NegOperator : public CoreItemOperator<operandType>
{
public:

	virtual inline operator operandType() const
	{
		kstl::vector<CoreItemSP>::const_iterator itOperand = CoreVector::myVector.begin();

		operandType	result((*itOperand)->operator operandType());

		return -result;
	}

};

template <  >
inline NegOperator<kstl::string>::operator kstl::string() const
{
	kstl::vector<CoreItemSP>::const_iterator itOperand = CoreVector::myVector.begin();

	kstl::string	result((kstl::string)(*itOperand));

	// no neg on string
	return result;
}

// boolean
class NotOperator : public CoreItemOperator<bool>
{
public:

	virtual inline operator bool() const
	{
		kstl::vector<CoreItemSP>::const_iterator itOperand = CoreVector::myVector.begin();

		bool	result(false);

		if ( ((bool)(*itOperand))==false)
		{
			result=true;
		}

		return result;
	}

};



class EqualOperator : public CoreItemOperator<bool>
{
public:

	virtual inline operator bool() const
	{
		kstl::vector<CoreItemSP>::const_iterator itOperand = CoreVector::myVector.begin();
		
		bool	result(false);
		
		if (CoreVector::myVector.size() == 2)
		{
			const CoreItem&	op1((*(*itOperand).get()));
			++itOperand;
			const CoreItem&	op2((*(*itOperand).get()));

			if (op1 == op2)
			{
				result = 1;
			}
		}

		return result;
	}

};


class NotEqualOperator : public CoreItemOperator<bool>
{
public:

	virtual inline operator bool() const
	{
		kstl::vector<CoreItemSP>::const_iterator itOperand = CoreVector::myVector.begin();

		bool	result(false);

		if (CoreVector::myVector.size() == 2)
		{
			const CoreItem&	op1((*(*itOperand).get()));
			++itOperand;
			const CoreItem&	op2((*(*itOperand).get()));

			if(! (op1 == op2))
			{
				result = true;
			}
		}

		return result;
	}

};

class AndOperator : public CoreItemOperator<bool>
{
public:

	virtual inline operator bool() const
	{
		kstl::vector<CoreItemSP>::const_iterator itOperand = CoreVector::myVector.begin();

		bool	result(false);

		if (CoreVector::myVector.size() == 2)
		{
			bool	op1((bool)(*(*itOperand).get()));
			++itOperand;
			bool	op2((bool)(*(*itOperand).get()));

			if (op1&&op2)
			{
				result = true;
			}
		}

		return result;
	}

};


class OrOperator : public CoreItemOperator<bool>
{
public:

	virtual inline operator bool() const
	{
		kstl::vector<CoreItemSP>::const_iterator itOperand = CoreVector::myVector.begin();

		bool	result(false);

		if (CoreVector::myVector.size() == 2)
		{
			bool	op1((bool)(*(*itOperand).get()));
			++itOperand;
			bool	op2((bool)(*(*itOperand).get()));

			if (op1 || op2)
			{
				result = true;
			}
		}

		return result;
	}

};

class SupOperator : public CoreItemOperator<bool>
{
public:

	virtual inline operator bool() const
	{
		kstl::vector<CoreItemSP>::const_iterator itOperand = CoreVector::myVector.begin();

		bool	result(false);

		if (CoreVector::myVector.size() == 2)
		{
			kfloat	op1((kfloat)(*itOperand));
			++itOperand;
			kfloat	op2((kfloat)(*itOperand));

			if (op1 > op2)
			{
				result = true;
			}
		}

		return result;
	}

};

class SupEqualOperator : public CoreItemOperator<bool>
{
public:

	virtual inline operator bool() const
	{
		kstl::vector<CoreItemSP>::const_iterator itOperand = CoreVector::myVector.begin();

		bool	result(false);

		if (CoreVector::myVector.size() == 2)
		{
			kfloat	op1((kfloat)(*itOperand));
			++itOperand;
			kfloat	op2((kfloat)(*itOperand));

			if (op1 >= op2)
			{
				result = true;
			}
		}

		return result;
	}

};


class InfOperator : public CoreItemOperator<bool>
{
public:

	virtual inline operator bool() const
	{
		kstl::vector<CoreItemSP>::const_iterator itOperand = CoreVector::myVector.begin();

		bool	result(false);

		if (CoreVector::myVector.size() == 2)
		{
			kfloat	op1((kfloat)(*itOperand));
			++itOperand;
			kfloat	op2((kfloat)(*itOperand));

			if (op1 < op2)
			{
				result = true;
			}
		}

		return result;
	}

};

class InfEqualOperator : public CoreItemOperator<bool>
{
public:

	virtual inline operator bool() const
	{
		kstl::vector<CoreItemSP>::const_iterator itOperand = CoreVector::myVector.begin();

		bool	result(false);

		if (CoreVector::myVector.size() == 2)
		{
			kfloat	op1((kfloat)(*itOperand));
			++itOperand;
			kfloat	op2((kfloat)(*itOperand));

			if (op1 <= op2)
			{
				result = true;
			}
		}

		return result;
	}

};


template<typename operandType>
class AffectOperator : public CoreItemOperator<operandType>
{
public:

	virtual inline operator operandType() const
	{
		kstl::vector<CoreItemSP>::const_iterator itOperand = CoreVector::myVector.begin();
		kstl::vector<CoreItemSP>::const_iterator itOperandEnd = CoreVector::myVector.end();

		CoreItem& result=(*(CoreItem*)(*itOperand).get());
		
		++itOperand;
		if (itOperand != itOperandEnd)
		{
			result= (*itOperand)->operator operandType();
		}
		return result.operator operandType();

	}

};


template<typename operandType>
class DynamicVariableOperator : public CoreItemOperator<operandType>
{
public:

	DynamicVariableOperator(const kstl::string& varname) : myVarName(varname)
	{

	}

	virtual operator operandType() const;

protected:

	
	kstl::string	myVarName;

};



template<typename operandType>
class CoreModifiableAttributeOperator : public CoreItemOperator<operandType>
{
public:

	CoreModifiableAttributeOperator(const kstl::string& path, CoreModifiable* target) : myAttributePath(path), myTarget(target), myAttribute(0), myArrayAttributeIndex(-1), myIsMethod(0){};

	// construct from an existing attribute
	CoreModifiableAttributeOperator(CoreModifiableAttribute* attr) : myTarget(0), myAttribute(attr), myArrayAttributeIndex(-1), myIsMethod(0) 
	{
		myAttributePath = "";
	};

	virtual operator operandType() const override;

	virtual inline bool operator==(const CoreItem& other) const override
	{
		return (((kfloat)(*this)) == (kfloat)other);
	}

	virtual CoreItem& operator=(const operandType& other) override;

protected:

	void GetAttribute() const;

	kstl::string				myAttributePath;
	CoreModifiable*				myTarget;

	union
	{
		CoreModifiableAttribute*	myAttribute;
		unsigned int				myMethodID;
	}; 

	short						myArrayAttributeIndex;
	short						myIsMethod;
};

template<>
inline bool CoreModifiableAttributeOperator<kstl::string>::operator == (const CoreItem& other) const
{
	kstl::string	othervalue;
	other.getValue(othervalue);

	kstl::string	thisvalue;
	getValue(thisvalue);

	return (thisvalue == othervalue);
}

template<>
inline bool CoreModifiableAttributeOperator<Point2D>::operator == (const CoreItem& other) const
{
	Point2D	othervalue;
	other.getValue(othervalue);

	Point2D	thisvalue;
	getValue(thisvalue);

	return (thisvalue == othervalue);
}


template<>
inline bool CoreModifiableAttributeOperator<Point3D>::operator == (const CoreItem& other) const
{
	Point3D	othervalue;
	other.getValue(othervalue);
	Point3D	thisvalue;
	getValue(thisvalue);

	return (thisvalue == othervalue);
}

template<>
inline bool CoreModifiableAttributeOperator<Vector4D>::operator == (const CoreItem& other) const
{
	Vector4D	othervalue;
	other.getValue(othervalue);
	Vector4D	thisvalue;
	getValue(thisvalue);

	return (thisvalue == othervalue);
}


template<typename operandType>
class IfThenElseOperator : public CoreItemOperator<operandType>
{
public:

	virtual inline operator operandType() const override
	{
		kstl::vector<CoreItemSP>::const_iterator itOperand = CoreVector::myVector.begin();

		operandType	result(0);

		bool	test((bool)(*(*itOperand).get()));
		
		if (test != 0)
		{
			if (CoreVector::myVector.size() > 1)
			{
				++itOperand;
				result = (operandType)*(*itOperand).get();
			}
		}
		else
		{
			if (CoreVector::myVector.size() > 2)
			{
				++itOperand;
				++itOperand;

				result = (operandType)*(*itOperand).get();
			}
		}

		
		return result;
	}
	static CoreVector* create()
	{
		return new IfThenElseOperator<operandType>();
	}
};


#endif // _COREITEMOPERATOR_H