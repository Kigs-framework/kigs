#include "PrecompiledHeaders.h"
#include "CoreItemOperator.h"
#include "CoreModifiableAttribute.h"

#include <algorithm>


struct RemoveDelimiter
{
	bool operator()(char c)
	{
		return (c == '\r' || c == '\t' || c == ' ' || c == '\n');
	}
};


CoreItemEvaluationContext*	myCurrentCoreItemEvaluationContext = 0;

void	SetCoreItemOperatorContext(CoreItemEvaluationContext* set)
{
	KigsCore::Instance()->GetSemaphore();
	myCurrentCoreItemEvaluationContext = set;
}
void	ReleaseCoreItemOperatorContext()
{
	myCurrentCoreItemEvaluationContext = 0;
	KigsCore::Instance()->ReleaseSemaphore();
}

template<typename operandType>
CoreItem&	CoreItemOperator<operandType>::Construct(const kstl::string& formulae, CoreModifiable* target, kstl::vector<SpecificOperator>* specificList)
{
	kstl::string cleanFormulae = formulae;
	cleanFormulae.erase(std::remove_if(cleanFormulae.begin(), cleanFormulae.end(), RemoveDelimiter()), cleanFormulae.end());
	AsciiParserUtils	parser((char*)cleanFormulae.c_str(), cleanFormulae.length());

	ConstructContext	context;
	context.myTarget = target;

	ConstructContextMap(context.myMap, specificList);

	CoreItem&	result = Parse(parser, context);

	return result;

}

template<typename operandType>
CoreItem&	CoreItemOperator<operandType>::Construct(const kstl::string& formulae, CoreModifiable* target, const kstl::unordered_map<kstl::string, CoreItemOperatorCreateMethod>&	myMap)
{
	kstl::string cleanFormulae = formulae;
	cleanFormulae.erase(std::remove_if(cleanFormulae.begin(), cleanFormulae.end(), RemoveDelimiter()), cleanFormulae.end());
	AsciiParserUtils	parser((char*)cleanFormulae.c_str(), cleanFormulae.length());

	ConstructContext	context;
	context.myTarget = target;
	context.myMap = myMap;

	CoreItem&	result = Parse(parser, context);

	return result;

}

template<typename operandType>
void	CoreItemOperator<operandType>::ConstructContextMap(kstl::unordered_map<kstl::string, CoreItemOperatorCreateMethod>&	myMap, kstl::vector<SpecificOperator>* specificList)
{
	myMap.clear();
	
	myMap["sin"] = &SinusOperator<operandType>::create;
	myMap["cos"] = &CosinusOperator<operandType>::create;
	myMap["tan"] = &TangentOperator<operandType>::create;
	myMap["abs"] = &AbsOperator<operandType>::create;
	myMap["min"] = &MinOperator<operandType>::create;
	myMap["max"] = &MaxOperator<operandType>::create;
	myMap["if"] = &IfThenElseOperator<operandType>::create;

	// push specific
	if (specificList)
	{
		typename kstl::vector<SpecificOperator>::const_iterator itstart = specificList->begin();
		typename kstl::vector<SpecificOperator>::const_iterator itend = specificList->end();
		while (itstart != itend)
		{
			myMap[(*itstart).myKeyWord] = (*itstart).myCreateMethod;
			++itstart;
		}
	}
}

template<>
void	CoreItemOperator<kstl::string>::ConstructContextMap(kstl::unordered_map<kstl::string, CoreItemOperatorCreateMethod>&	myMap, kstl::vector<SpecificOperator>* specificList)
{
	// nothing here
}

template<>
void	CoreItemOperator<Point2D>::ConstructContextMap(kstl::unordered_map<kstl::string, CoreItemOperatorCreateMethod>&	myMap, kstl::vector<SpecificOperator>* specificList)
{
	// nothing here
}

template<>
void	CoreItemOperator<Point3D>::ConstructContextMap(kstl::unordered_map<kstl::string, CoreItemOperatorCreateMethod>&	myMap, kstl::vector<SpecificOperator>* specificList)
{
	// nothing here
}

template<typename operandType>
CoreItem&	CoreItemOperator<operandType>::Parse(AsciiParserUtils& formulae, ConstructContext& context)
{
	
	// check if real block or parameter to avoid things like (ajhgj)+(bjhb) considered as one block
	if (formulae[0] == '(') // is this a full block ?
	{
		if (formulae[formulae.length() - 1] == ')')
		{
			AsciiParserUtils	block(formulae);
			int oldpos = formulae.GetPosition();
			if (formulae.GetBlock(block, '(', ')'))
			{
				if (block.length() == (formulae.length() - 2))
				{
					CoreItem*	op1 = &Parse(block, context);
					return *op1;
				}
				else
				{
					formulae.SetPosition(oldpos);
				}
			}
		}
	}

	if (formulae[0] == '[') // is this a modifiable method ?
	{
		if (formulae[formulae.length() - 1] == ']')
		{

			AsciiParserUtils	block(formulae);
			int oldpos = formulae.GetPosition();
			formulae.SetPosition(0);

			if (formulae.GetBlock(block, '[', ']'))
			{

				// test method

				AsciiParserUtils	paramblock(block);
				if (block.GetBlock(paramblock, '(', ')'))
				{
					formulae.SetPosition(1);
					AsciiParserUtils leading(formulae);
					formulae.GetString(leading, '(');
					// create a method
					CoreModifiableAttributeOperator<operandType>* opeattribute = new CoreModifiableAttributeOperator<operandType>((const kstl::string&)leading, context.myTarget);
					if (paramblock.length())
					{
						CoreItem*	op1 = 0;
						kstl::vector<kstl::string>	params = FindFirstLevelParams(paramblock, context);

						kstl::vector<kstl::string>::iterator	itparambegin = params.begin();
						kstl::vector<kstl::string>::iterator	itparamend = params.end();

						while (itparambegin != itparamend)
						{
							kstl::string& currentParam = (*itparambegin);
							char* currentParamC = (char*)currentParam.c_str();

							AsciiParserUtils	param(currentParamC, currentParam.length());
							op1 = &Parse(param, context);
							if (op1 != KigsCore::Instance()->NotFoundCoreItem())
							{
								opeattribute->push_back(op1);
								op1->Destroy();
							}
							++itparambegin;
						}
					}

					return *opeattribute;
				}

			}
			formulae.SetPosition(oldpos);
			
		}
	}


	if (formulae[0] == '#') // is this a modifiable attribute ?
	{
		if (formulae[formulae.length() - 1] == '#')
		{
		
			AsciiParserUtils	block(formulae);
			int oldpos = formulae.GetPosition();
			formulae.SetPosition(1);
		
			if (formulae.GetString(block, '#'))
			{
				if (block.length() == (formulae.length() - 2))
				{
					CoreModifiableAttributeOperator<operandType>* opeattribute = new CoreModifiableAttributeOperator<operandType>((const kstl::string&)block, context.myTarget);
					return *opeattribute;
				}
			}

			formulae.SetPosition(oldpos);
			
		}
	}

	kstl::vector<CoreItemOperatorStruct>	FirstLevelOperatorList = FindFirstLevelOperators(formulae,context);

	if (FirstLevelOperatorList.size() == 0) // leaf
	{

		// check starting character
		if (formulae[0] == '-') // is this a neg unary operator ?
		{
			if (!((formulae[1] >= '0') && (formulae[1] <= '9'))) // this is not a constant 
			{
				CoreItem*	op1 = 0;
				AsciiParserUtils	operand(formulae);
				formulae.SetPosition(1);

				if (formulae.GetTrailingPart(operand))
				{
					op1 = &Parse(operand, context);
				}

				if (op1)
				{
					// check where to add neg operator
					NegOperator<operandType>&   neg = *(new NegOperator<operandType>());
					neg.push_back(op1);
					op1->Destroy();
					return neg;
				}
			}
		}
		else if (formulae[0] == '!') // is this a not unary operator ?
		{
			CoreItem*	op1 = 0;
			AsciiParserUtils	operand(formulae);
			formulae.SetPosition(1);

			if (formulae.GetTrailingPart(operand))
			{
				op1 = &Parse(operand, context);
			}

			if (op1)
			{
				// check where to add neg operator
				NotOperator&   neg = *(new NotOperator());
				neg.push_back(op1);
				op1->Destroy();
				return neg;
			}
		}

		formulae.Reset();

		float value;
		if (formulae.ReadFloat(value)) // check for leaf constant
		{
			CoreValue<kfloat>&   corevalue = *(new CoreValue<kfloat>());
			corevalue = value;
			return corevalue;
		}
		
		// try to match other keyword
		kstl::string matchkeywork = "";
		{
			AsciiParserUtils	word(formulae);
			formulae.GetWord(word, '(');
			matchkeywork = word.c_str();
		}
		if (matchkeywork != "")
		{

			CoreItemOperator<operandType>*	newfunction = 0;
			newfunction = getOperator(matchkeywork, context);

			if (newfunction)
			{
				AsciiParserUtils	operand(formulae);
				formulae.SetPosition(matchkeywork.size());

				CoreItem*	op1 = 0;
				if (formulae.GetTrailingPart(operand))
				{
					AsciiParserUtils	paramblock(operand);

					operand.GetBlock(paramblock, '(', ')');

					if (paramblock.length())
					{

						kstl::vector<kstl::string>	params = FindFirstLevelParams(paramblock, context);

						kstl::vector<kstl::string>::iterator	itparambegin = params.begin();
						kstl::vector<kstl::string>::iterator	itparamend = params.end();

						while (itparambegin != itparamend)
						{
							kstl::string& currentParam=(*itparambegin);
							char* currentParamC = (char*)currentParam.c_str();

							AsciiParserUtils	param(currentParamC, currentParam.length());
							op1 = &Parse(param, context);
							if (op1 != KigsCore::Instance()->NotFoundCoreItem())
							{
								newfunction->push_back(op1);
								op1->Destroy();
							}
							++itparambegin;
						}
					}
				}
				return *newfunction;
			}

			// try to find a variable with this name
			CoreItem* variable=getVariable(matchkeywork);
			if (variable)
			{
				return *variable;
			}
			/*// just set value as a string
			CoreValue < kstl::string > &   corevalue = *(new CoreValue< kstl::string>());
			corevalue = matchkeywork;
			return corevalue;
			*/
			// dynamic var
			variable = new DynamicVariableOperator<operandType>(matchkeywork);
			return *variable;
		}

	}
	else // create operator tree
	{
		CoreVector*	newOperator = 0;
		while (FirstLevelOperatorList.size())
		{
			// find higher level
	
			int foundpriority = -1;
			kstl::vector<CoreItemOperatorStruct>::iterator	itfound = FirstLevelOperatorList.end();
			kstl::vector<CoreItemOperatorStruct>::iterator	itcurrent = FirstLevelOperatorList.begin();
			kstl::vector<CoreItemOperatorStruct>::iterator	itend = FirstLevelOperatorList.end();

			int i = 0;
			int ifound = 0;

			while (itcurrent != itend)
			{
				if ((*itcurrent).myPriority>foundpriority)
				{
					foundpriority = (*itcurrent).myPriority;
					itfound = itcurrent;
					ifound = i;
				}
				++itcurrent;
				++i;
			}

			CoreItemOperatorStruct& current = (*itfound);


			switch (current.myOp)
			{
			case ';':
			{
				newOperator = new InstructionListOperator < operandType>();
			}
			break;
			case '*':
			{
				newOperator = new MultOperator < operandType>();
			}
			break;
			case '+':
			{
				newOperator = new AddOperator < operandType>();
			}
			break;
			case '-':
			{
				newOperator = new SubOperator < operandType>();
			}
			break;
			case '/':
			{
				newOperator = new DivOperator < operandType>();
			}
			break;
			case '=':
			{
				newOperator = new EqualOperator();
			}
			break;
			case 'd':
			{
				newOperator = new NotEqualOperator();
			}
			break;
			case 's':
			{
				newOperator = new SupEqualOperator();
			}
			break;
			case 'i':
			{
				newOperator = new InfEqualOperator();
			}
			break;
			case '>':
			{
				newOperator = new SupOperator();
			}
			break;
			case '<':
			{
				newOperator = new InfOperator();
			}
			break;
			case '&':
			{
				newOperator = new AndOperator();
			}
			break;
			case '|':
			{
				newOperator = new OrOperator();
			}
			break;
			case 'a': // affectation
			{
				newOperator = new AffectOperator<operandType>();
			}
			break;

			}

			newOperator->push_back(current.myOp1);
			current.myOp1->Destroy();
			newOperator->push_back(current.myOp2);
			current.myOp2->Destroy();

			// replace newOperator in previous and next 
			if (ifound > 0)
			{
				FirstLevelOperatorList[ifound - 1].myOp2 = newOperator;
			}
			if (ifound < ((int)FirstLevelOperatorList.size() - 1))
			{
				FirstLevelOperatorList[ifound + 1].myOp1 = newOperator;
			}

			FirstLevelOperatorList.erase(itfound);
		}
		return *newOperator;
	}


	return *(KigsCore::Instance()->NotFoundCoreItem());
}

template<typename operandType>
CoreItemOperator<operandType>* CoreItemOperator<operandType>::getOperator(const kstl::string& keyword, ConstructContext& context)
{

	typename kstl::unordered_map<kstl::string, CoreItemOperatorCreateMethod>::const_iterator itfound = context.myMap.find(keyword);
	while (itfound != context.myMap.end())
	{
		return  (CoreItemOperator<operandType>*)((*itfound).second());
	}
	return 0;
}

template<typename operandType>
CoreItem* CoreItemOperator<operandType>::getVariable(const kstl::string& keyword)
{
	if (myCurrentCoreItemEvaluationContext)
	{
		kstl::unordered_map<unsigned int, RefCountedBaseClass*>::iterator	itfound = myCurrentCoreItemEvaluationContext->myVariableList.find(CharToID::GetID(keyword));
		if (itfound != myCurrentCoreItemEvaluationContext->myVariableList.end())
		{
			return (CoreItem*)(*itfound).second;
		}
	}
	return 0;
}

template<typename operandType>
kstl::vector<kstl::string>	CoreItemOperator<operandType>::FindFirstLevelParams(AsciiParserUtils& block, ConstructContext& context)
{
	int currentPos = 0;
	int prevPos = 0;
	char	currentChar;

	kstl::vector<kstl::string>	paramList;
	paramList.clear();

	int BlockLevel = 0;

	while (block.ReadChar(currentChar))
	{
		if (currentChar == ',')
		{
			if (BlockLevel == 0)
			{
				paramList.push_back(block.subString(prevPos, currentPos - prevPos));
				prevPos = currentPos + 1;
			}
		}
		else if (currentChar == '(')
		{
			++BlockLevel;
		}
		else if (currentChar == ')')
		{
			BlockLevel--;
		}
		++currentPos;
	}

	paramList.push_back(block.subString(prevPos, currentPos - prevPos));

	return paramList;	
}

template<typename operandType>
kstl::vector<CoreItemOperatorStruct>	CoreItemOperator<operandType>::FindFirstLevelOperators(AsciiParserUtils& block, ConstructContext& context)
{
	//int currentPos = -1;

	char	currentChar,prevChar;

	kstl::vector<CoreItemOperatorStruct>	OperatorList;
	OperatorList.clear();

	int BlockLevel=0;
	bool prevIsValid = false;
	bool insideAttribute = false;
	prevChar = 0;

	while (block.ReadChar(currentChar))
	{
		int priority = 0;
		bool isValid = true;
		switch (currentChar)
		{
		case ';': // separator operator is higher priority
			++priority;
		case '*':
			++priority;
		case '/':
			++priority;
		case '+':
			++priority;
		case '-':
		{
			++priority;
			if ((BlockLevel == 0) && prevIsValid)
			{
				CoreItemOperatorStruct toAdd;
				toAdd.myOp = currentChar;
				toAdd.myPos = block.GetPosition();
				toAdd.myOp1 = 0;
				toAdd.myOp2 = 0;
				toAdd.myPriority = priority;
				toAdd.mySize = 1;
				OperatorList.push_back(toAdd);
				isValid = false;
			}
		}
		break;
		case '(':
		{
			++BlockLevel;
		}
		break;
		case ')':
		{
			BlockLevel--;
		}
		break;
		case '[':
		{
			++BlockLevel;
		}
		break;
		case ']':
		{
			BlockLevel--;
		}
		break;
		case '#':
		{
			if (insideAttribute)
			{
				BlockLevel--;
				insideAttribute = false;
			}
			else
			{
				BlockLevel++;
				insideAttribute = true;
			}
		}
		break;
		// logical op
		case '=':
		{
			if ((BlockLevel == 0) && prevIsValid)
			{
				if (prevChar == '=')
				{
					CoreItemOperatorStruct toAdd;
					toAdd.myOp = currentChar;
					toAdd.myPos = block.GetPosition()-1;
					toAdd.myOp1 = 0;
					toAdd.myOp2 = 0;
					toAdd.myPriority = priority;
					toAdd.mySize = 2;
					OperatorList.push_back(toAdd);
					isValid = false;
				}
				else if (prevChar == '!')
				{
					CoreItemOperatorStruct toAdd;
					toAdd.myOp = 'd';
					toAdd.myPos = block.GetPosition()-1;
					toAdd.myOp1 = 0;
					toAdd.myOp2 = 0;
					toAdd.myPriority = priority;
					toAdd.mySize = 2;
					OperatorList.push_back(toAdd);
					isValid = false;
				}
				else if (prevChar == '<') 
				{
					// change previous
					OperatorList[OperatorList.size() - 1].myOp = 'i';
					OperatorList[OperatorList.size() - 1].mySize = 2;
					isValid = false;
				}
				else if (prevChar == '>')
				{
					// change previous
					OperatorList[OperatorList.size() - 1].myOp = 's';
					OperatorList[OperatorList.size() - 1].mySize = 2;
					isValid = false;
				}
			}
		}
		break;
		case '<':
		case '>':
		{
			if ((BlockLevel == 0) && prevIsValid)
			{
				CoreItemOperatorStruct toAdd;
				toAdd.myOp = currentChar;
				toAdd.myPos = block.GetPosition();
				toAdd.myOp1 = 0;
				toAdd.myOp2 = 0;
				toAdd.myPriority = priority;
				toAdd.mySize = 1;
				OperatorList.push_back(toAdd);
			}
		}
		break;
		case '&':
		{
			if ((BlockLevel == 0) && prevIsValid)
			{
				if (prevChar == '&')
				{
					CoreItemOperatorStruct toAdd;
					toAdd.myOp = currentChar;
					toAdd.myPos = block.GetPosition()-1;
					toAdd.myOp1 = 0;
					toAdd.myOp2 = 0;
					toAdd.myPriority = priority;
					toAdd.mySize = 2;
					OperatorList.push_back(toAdd);
				}
			}
		}
		break;
		case '|':
		{
			if ((BlockLevel == 0) && prevIsValid)
			{
				if (prevChar == '|')
				{
					CoreItemOperatorStruct toAdd;
					toAdd.myOp = currentChar;
					toAdd.myPos = block.GetPosition() - 1;
					toAdd.myOp1 = 0;
					toAdd.myOp2 = 0;
					toAdd.myPriority = priority;
					toAdd.mySize = 2;
					OperatorList.push_back(toAdd);
				}
			}
		}
		break;
		default:
		{
			// check if previous was a '=' affect operator
			if ((BlockLevel == 0) && prevIsValid)
			{
				if (prevChar == '=')
				{
					CoreItemOperatorStruct toAdd;
					toAdd.myOp = 'a';
					toAdd.myPos = block.GetPosition() - 1;
					toAdd.myOp1 = 0;
					toAdd.myOp2 = 0;
					toAdd.myPriority = priority;
					toAdd.mySize = 1;
					OperatorList.push_back(toAdd);
				}
			}
		}
		}
		prevChar = currentChar;
		prevIsValid = isValid;
	}

	// if size, init operand
	if (OperatorList.size())
	{
		AsciiParserUtils	remaining(block);
		int starting = 0;
		int i;
		for (i = 0; i < (int)OperatorList.size(); i++)
		{
			AsciiParserUtils	operand(remaining);
			remaining.SetPosition(OperatorList[i].myPos - 1-starting);
			
			if (remaining.GetLeadingPart(operand))
			{
				OperatorList[i].myOp1 = &Parse(operand, context);
			}
			starting += remaining.GetPosition() + OperatorList[i].mySize;
			remaining.SetPosition(remaining.GetPosition() + OperatorList[i].mySize);
			remaining.GetTrailingPart(operand);

			remaining = operand;
		}

		// last one
		OperatorList[OperatorList.size() - 1].myOp2 = &Parse(remaining,context);
	
		// set op2
		for (i = 0; i < ((int)OperatorList.size())-1; i++)
		{
			OperatorList[i].myOp2 = OperatorList[i+1].myOp1;
		}
	}

	return OperatorList;

}

template<typename T>
void CoreModifiableAttributeOperator<T>::GetAttribute() const
{
	if (myAttributePath != "")
	{
		// search attribute
		kstl::string modifiablename;
		kstl::string attributename;

		CoreModifiableAttribute::ParseAttributePath(myAttributePath, modifiablename, attributename);

		if ((modifiablename == "") && (attributename == ""))
		{
			attributename = myAttributePath;
		}

		if (attributename != "")
		{
			CoreModifiable*	Owner = (CoreModifiable*)myTarget;
			if (modifiablename != "")
			{
				if (myTarget)
				{
					Owner = myTarget->GetInstanceByPath(modifiablename);
				}
				else
				{
					Owner = CoreModifiable::GetInstanceByGlobalPath(modifiablename);
				}
			}
			if (Owner)
			{
				// fake const
				CoreModifiableAttributeOperator<T>* other = (CoreModifiableAttributeOperator<T>*)(this);

				int attrindex = -1;
				// look for .x .y .z or .w at the end of attribute name
				if (attributename[attributename.size() - 2] == '.')
				{
					kstl::string extension = attributename.substr(attributename.size() - 2, 2);
					attributename = attributename.substr(0, attributename.size() - 2);
					switch (extension[1])
					{
					case'x':
						attrindex = 0;
						break;
					case'y':
						attrindex = 1;
						break;
					case'z':
						attrindex = 2;
						break;
					case'w':
						attrindex = 3;
						break;
					}
				}

				other->myAttribute = Owner->getAttribute(attributename);

				if (myAttribute)
				{

					if ((myAttribute->size() > 1) && (attrindex >= 0))
					{
						if (attrindex > (myAttribute->size() - 1))
						{
							attrindex = myAttribute->size() - 1;
						}
						other->myArrayAttributeIndex = attrindex;
					}
					other->myIsMethod = 0;
				}
				else if (Owner->HasMethod(attributename))// check for method
				{
					unsigned int id = CharToID::GetID(attributename);
					other->myIsMethod = 1;
					other->myMethodID = id;

					// for method, change target
					other->myTarget = Owner;
				}
			}
		}
	}

}


template<>
CoreModifiableAttributeOperator<kfloat>::operator kfloat() const
{
	kfloat	result = 0.0f;
	if ((!myAttribute) && (!myIsMethod))
	{
		GetAttribute();
	}
	if (myIsMethod == 0)
	{
		if (myAttribute)
		{
			if (myArrayAttributeIndex >= 0)
			{
				myAttribute->getArrayElementValue(result, 0, myArrayAttributeIndex);
			}
			else
			{
				myAttribute->getValue(result);
			}
		}
	}
	else
	{
		// push attributes
		kstl::vector<CoreModifiableAttribute*>	myAttributes;
		kstl::vector<RefCountedBaseClass*>::const_iterator itOperand = CoreVector::myVector.begin();
		kstl::vector<RefCountedBaseClass*>::const_iterator itOperandEnd = CoreVector::myVector.end();

		
		while (itOperand != itOperandEnd)
		{
			CoreModifiableAttribute* attribute = ((CoreItem*)(*itOperand))->createAttribute(myTarget);

			if (!attribute)
			{
				kfloat val = (kfloat)((CoreItem&)*((CoreItem*)(*itOperand)));
				attribute = new maFloat(*myTarget, false, LABEL_AND_ID(Val), val);
			}
			myAttributes.push_back(attribute);

			itOperand++;
		}

		// add param for result
		maFloat* resultval = new maFloat(*myTarget, false, LABEL_AND_ID(Result), 0.0);
		myAttributes.push_back(resultval);

		// check if current context has sender or data
		CoreModifiable* sendervariable = (CoreModifiable*)getVariable("sender");
		void* datavariable = (void*)getVariable("data");
		myTarget->CallMethod(myMethodID, myAttributes, datavariable, sendervariable);
	
		result = *resultval;


		kstl::vector<CoreModifiableAttribute*>::iterator itattr = myAttributes.begin();
		kstl::vector<CoreModifiableAttribute*>::iterator itattrEnd = myAttributes.end();

		// delete attributes and set result
		while (itattr != itattrEnd)
		{
			delete (*itattr);

			itattr++;
		}

		myAttributes.clear();
		
	}

	return result;
}

template<>
CoreModifiableAttributeOperator<kstl::string>::operator kstl::string() const
{
	kstl::string	result = "";
	if ((!myAttribute) && (!myIsMethod))
	{
		GetAttribute();
	}
	if (myIsMethod == 0)
	{
		if (myAttribute)
		{
			if (myArrayAttributeIndex >= 0)
			{
				myAttribute->getArrayElementValue(result, 0, myArrayAttributeIndex);
			}
			else
			{
				myAttribute->getValue(result);
			}
		}
	}
	else
	{
		// push attributes
		kstl::vector<CoreModifiableAttribute*>	myAttributes;
		kstl::vector<RefCountedBaseClass*>::const_iterator itOperand = CoreVector::myVector.begin();
		kstl::vector<RefCountedBaseClass*>::const_iterator itOperandEnd = CoreVector::myVector.end();


		while (itOperand != itOperandEnd)
		{
			CoreModifiableAttribute* attribute = ((CoreItem*)(*itOperand))->createAttribute(myTarget);

			if (!attribute)
			{
				kstl::string val = (kstl::string)((CoreItem&)*((CoreItem*)(*itOperand)));
				attribute = new maString(*myTarget, false, LABEL_AND_ID(Val), val);
			}
			myAttributes.push_back(attribute);

			itOperand++;
		}

		// add param for result
		maString* resultval = new maString(*myTarget, false, LABEL_AND_ID(Result), "");
		myAttributes.push_back(resultval);

		// check if current context has sender or data
		CoreModifiable* sendervariable = (CoreModifiable*)getVariable("sender");
		void* datavariable = (void*)getVariable("data");
		myTarget->CallMethod(myMethodID, myAttributes, datavariable, sendervariable);

		result = *resultval;


		kstl::vector<CoreModifiableAttribute*>::iterator itattr = myAttributes.begin();
		kstl::vector<CoreModifiableAttribute*>::iterator itattrEnd = myAttributes.end();

		// delete attributes and set result
		while (itattr != itattrEnd)
		{
			delete (*itattr);

			itattr++;
		}

		myAttributes.clear();

	}

	return result;
}


template<>
CoreModifiableAttributeOperator<Point2D>::operator Point2D() const
{
	Point2D	result(0.0f,0.0f);
	if ((!myAttribute) && (!myIsMethod))
	{
		GetAttribute();
	}
	if (myIsMethod == 0)
	{
		if (myAttribute)
		{
			if (myArrayAttributeIndex >= 0)
			{
				kfloat getaValue(0.0f);
				myAttribute->getArrayElementValue(getaValue, 0, myArrayAttributeIndex);
				result[myArrayAttributeIndex] = getaValue;
			}
			else
			{
				// retreive two values
				kfloat getaValue(0.0f);
				myAttribute->getArrayElementValue(getaValue, 0, 0);
				result.x = getaValue;
				myAttribute->getArrayElementValue(getaValue, 0, 1);
				result.y = getaValue;
			}
		}
	}
	else
	{
		// push attributes
		kstl::vector<CoreModifiableAttribute*>	myAttributes;
		kstl::vector<RefCountedBaseClass*>::const_iterator itOperand = CoreVector::myVector.begin();
		kstl::vector<RefCountedBaseClass*>::const_iterator itOperandEnd = CoreVector::myVector.end();


		while (itOperand != itOperandEnd)
		{
			CoreModifiableAttribute* attribute = ((CoreItem*)(*itOperand))->createAttribute(myTarget);
			
			if (!attribute)
			{
				Point2D val;
				((CoreItem*)(*itOperand))->getValue(val);
				attribute = new maVect2DF(*myTarget, false, LABEL_AND_ID(Val), &(val.x));
			}

			myAttributes.push_back(attribute);

			itOperand++;
		}

		// add param for result
		maVect2DF* resultval = new maVect2DF(*myTarget, false, LABEL_AND_ID(Result), 0.0f,0.0f);
		myAttributes.push_back(resultval);

		// check if current context has sender or data
		CoreModifiable* sendervariable = (CoreModifiable*)getVariable("sender");
		void* datavariable = (void*)getVariable("data");
		myTarget->CallMethod(myMethodID, myAttributes, datavariable, sendervariable);

		result = (Point2D)(*resultval);


		kstl::vector<CoreModifiableAttribute*>::iterator itattr = myAttributes.begin();
		kstl::vector<CoreModifiableAttribute*>::iterator itattrEnd = myAttributes.end();

		// delete attributes and set result
		while (itattr != itattrEnd)
		{
			delete (*itattr);

			itattr++;
		}

		myAttributes.clear();

	}

	return result;
}



template<>
CoreModifiableAttributeOperator<Point3D>::operator Point3D() const
{
	Point3D	result(0.0f, 0.0f,0.0f);
	if ((!myAttribute)&&(!myIsMethod))
	{
		GetAttribute();
	}
	if (myIsMethod == 0)
	{
		if (myAttribute)
		{
			if (myArrayAttributeIndex >= 0)
			{
				kfloat getaValue(0.0f);
				myAttribute->getArrayElementValue(getaValue, 0, myArrayAttributeIndex);
				result[myArrayAttributeIndex] = getaValue;
			}
			else
			{
				// retreive three values
				kfloat getaValue(0.0f);
				myAttribute->getArrayElementValue(getaValue, 0, 0);
				result.x = getaValue;
				myAttribute->getArrayElementValue(getaValue, 0, 1);
				result.y = getaValue;
				myAttribute->getArrayElementValue(getaValue, 0, 2);
				result.z = getaValue;
			}
		}
	}
	else
	{
		// push attributes
		kstl::vector<CoreModifiableAttribute*>	myAttributes;
		kstl::vector<RefCountedBaseClass*>::const_iterator itOperand = CoreVector::myVector.begin();
		kstl::vector<RefCountedBaseClass*>::const_iterator itOperandEnd = CoreVector::myVector.end();


		while (itOperand != itOperandEnd)
		{
			CoreModifiableAttribute* attribute = ((CoreItem*)(*itOperand))->createAttribute(myTarget);

			if (!attribute)
			{
				Point3D val = (Point3D)((CoreItem&)*((CoreItem*)(*itOperand)));
				attribute = new maVect3DF(*myTarget, false, LABEL_AND_ID(Val), &(val.x));
			}
			myAttributes.push_back(attribute);

			itOperand++;
		}

		// add param for result
		maVect3DF* resultval = new maVect3DF(*myTarget, false, LABEL_AND_ID(Result), 0.0f, 0.0f,0.0f);
		myAttributes.push_back(resultval);

		// check if current context has sender or data
		CoreModifiable* sendervariable = (CoreModifiable*)getVariable("sender");
		void* datavariable = (void*)getVariable("data");
		myTarget->CallMethod(myMethodID, myAttributes, datavariable, sendervariable);

		result = *resultval;


		kstl::vector<CoreModifiableAttribute*>::iterator itattr = myAttributes.begin();
		kstl::vector<CoreModifiableAttribute*>::iterator itattrEnd = myAttributes.end();

		// delete attributes and set result
		while (itattr != itattrEnd)
		{
			delete (*itattr);

			itattr++;
		}

		myAttributes.clear();

	}

	return result;
}

template<typename operandType>
CoreItem& CoreModifiableAttributeOperator<operandType>::operator=(const operandType& other)
{
	if ((!myAttribute) && (!myIsMethod))
	{
		GetAttribute();
	}
	if (myIsMethod == 0)
	{
		if (myAttribute)
		{
			if (myArrayAttributeIndex >= 0)
			{
				myAttribute->setArrayElementValue(other, 0, myArrayAttributeIndex);
			}
			else
			{
				// retreive three values
				myAttribute->setValue(other);
			}
		}
	}
	return *this;
}

template<>
CoreItem& CoreModifiableAttributeOperator<Point2D>::operator=(const Point2D& other)
{
	if ((!myAttribute) && (!myIsMethod))
	{
		GetAttribute();
	}
	if (myIsMethod == 0)
	{
		if (myAttribute)
		{
			if (myArrayAttributeIndex >= 0)
			{
				switch (myArrayAttributeIndex)
				{
				case 0:
					myAttribute->setArrayElementValue(other.x, 0, myArrayAttributeIndex);
					break;
				case 1:
					myAttribute->setArrayElementValue(other.y, 1, myArrayAttributeIndex);
					break;
				}
			}
			else
			{
				myAttribute->setArrayElementValue(other.x, 0, myArrayAttributeIndex);
				myAttribute->setArrayElementValue(other.y, 1, myArrayAttributeIndex);
			}
		}
	}
	return *this;
}

template<>
CoreItem& CoreModifiableAttributeOperator<Point3D>::operator=(const Point3D& other)
{
	if ((!myAttribute) && (!myIsMethod))
	{
		GetAttribute();
	}
	if (myIsMethod == 0)
	{
		if (myAttribute)
		{
			if (myArrayAttributeIndex >= 0)
			{
				switch (myArrayAttributeIndex)
				{
				case 0:
					myAttribute->setArrayElementValue(other.x, 0, myArrayAttributeIndex);
					break;
				case 1:
					myAttribute->setArrayElementValue(other.y, 1, myArrayAttributeIndex);
					break;
				case 2:
					myAttribute->setArrayElementValue(other.z, 2, myArrayAttributeIndex);
					break;
				}
			}
			else
			{
				myAttribute->setArrayElementValue(other.x, 0, myArrayAttributeIndex);
				myAttribute->setArrayElementValue(other.y, 1, myArrayAttributeIndex);
				myAttribute->setArrayElementValue(other.z, 2, myArrayAttributeIndex);
			}
		}
	}
	return *this;
}



template<>
DynamicVariableOperator<kstl::string>::operator kstl::string() const
{
	CoreItem* var = getVariable(myVarName);
	if (var)
	{
		return (kstl::string)(*var);
	}
	return myVarName;
}


template<>
DynamicVariableOperator<kfloat>::operator kfloat() const
{
	CoreItem* var = getVariable(myVarName);
	if (var)
	{
		return (kfloat)(*var);
	}

	// atof

	return (kfloat)atof(myVarName.c_str());
}

template<>
DynamicVariableOperator<Point2D>::operator Point2D() const
{
	CoreItem* var = getVariable(myVarName);
	if (var)
	{
		return (Point2D)(*var);
	}
	return Point2D(0,0);
}

template<>
DynamicVariableOperator<Point3D>::operator Point3D() const
{
	CoreItem* var = getVariable(myVarName);
	if (var)
	{
		return (Point3D)(*var);
	}
	return Point3D(0, 0,0);
}


//template class CoreItemOperator<int>;
template class CoreItemOperator<kfloat>;
template class CoreItemOperator<kstl::string>;
template class CoreItemOperator<Point2D>;
template class CoreItemOperator<Point3D>;