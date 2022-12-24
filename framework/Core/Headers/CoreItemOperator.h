#pragma once

#include "CoreVector.h"
#include "CoreValue.h"
#include "AsciiParserUtils.h"

namespace Kigs
{
	namespace Core
	{

		struct CoreItemOperatorStruct
		{
			char	mOp;
			int		mPos;
			int		mSize;
			CoreItemSP mOp1 = CoreItemSP(nullptr);
			CoreItemSP mOp2 = CoreItemSP(nullptr);
			int		mPriority;
		};

		class CoreItemEvaluationContext
		{
		public:
			unordered_map<unsigned int, std::vector<SP<GenericRefCountedBaseClass>>>	mVariableList;

			// static method to set or release context
			static void	SetContext(CoreItemEvaluationContext* set);
			static void	ReleaseContext();
			static CoreItemEvaluationContext* GetContext()
			{
				return mCurrentContext;
			}
		protected:
			// lock/unlock 
			static std::mutex	mMutex;
			// current context
			static CoreItemEvaluationContext* mCurrentContext;
		};



		typedef std::unique_ptr<CoreVector> (*CoreItemOperatorCreateMethod)();

		class SpecificOperator
		{
		public:
			std::string	mKeyWord;	// 3 letter keyword
			CoreItemOperatorCreateMethod	mCreateMethod;
		};

		class ConstructContext
		{
		public:
			unordered_map<std::string, CoreItemOperatorCreateMethod>	mMap;
			CoreModifiable* mTarget;
			std::vector<SpecificOperator>* mSpecificList;	
		};


		// ****************************************
		// * CoreItemOperator class
		// * --------------------------------------
		/**
		* \class	CoreItemOperator
		* \file     CoreItemOperator.h
		* \ingroup Core
		* \brief	Base class for CoreItemOperator
		* 
		* CoreItemOperator can be used as a little script langage to do some simple calculations / manipulations
		* on coremodifiable attributes for example.
		* 
		*/
		// ****************************************

		template<typename operandType>
		class CoreItemOperator : public CoreVector
		{
		public:

			virtual inline operator bool() const override
			{
				KIGS_ERROR("cast operator called on base CoreItem", 2);
				return false;
			}

			virtual inline operator float() const override
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

			virtual inline operator std::string() const override
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

			static CoreItemSP	Construct(const std::string& formulae, CoreModifiable* target, std::vector<SpecificOperator>* specificList=0);
			static CoreItemSP	Construct(const std::string& formulae, CoreModifiable* target, const unordered_map<std::string, CoreItemOperatorCreateMethod>&	lmap);
			static void	ConstructContextMap(unordered_map<std::string, CoreItemOperatorCreateMethod>&	lmap, std::vector<SpecificOperator>* specificList = 0);
			static CoreItemSP	Parse(AsciiParserUtils& formulae, ConstructContext& context);

		protected:

			static bool	CheckAffectation(char prevChar, int priority, AsciiParserUtils& block, std::vector<CoreItemOperatorStruct>& OperatorList);

			static std::vector<CoreItemOperatorStruct>	FindFirstLevelOperators(AsciiParserUtils& formulae, ConstructContext& context);
			static std::vector<CoreItemOperatorStruct>	FindFirstLevelSeparator(AsciiParserUtils& formulae, ConstructContext& context);
			static std::vector<std::string>	FindFirstLevelParams(AsciiParserUtils& formulae, ConstructContext& context);
			static SP<CoreItemOperator<operandType>> getOperator(const std::string& keyword, ConstructContext& context);
			static SP<GenericRefCountedBaseClass> getVariable(const std::string& keyword);
		};


		template<>
		inline CoreItemOperator<bool>::operator float() const
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
		inline CoreItemOperator<bool>::operator std::string() const
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
		inline CoreItemOperator<float>::operator bool() const
		{
			float result = (float)*this;
			return result ? true : false;
		}

		template<>
		inline CoreItemOperator<float>::operator int() const
		{
			float result = (float)*this;
			return (int)result;
		}

		template<>
		inline CoreItemOperator<float>::operator unsigned int() const
		{
			float result = (float)*this;
			return (unsigned int)result;
		}

		template<>
		inline void	CoreItemOperator<std::string>::defaultOperandTypeInit(std::string& _value)
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
				std::vector<CoreItemSP>::const_iterator itOperand = CoreVector::mVector.begin();
				std::vector<CoreItemSP>::const_iterator itOperandEnd = CoreVector::mVector.end();

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
				std::vector<CoreItemSP>::const_iterator itOperand = CoreVector::mVector.begin();
				std::vector<CoreItemSP>::const_iterator itOperandEnd = CoreVector::mVector.end();

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
		inline AddOperator<std::string>::operator std::string() const
		{
			std::vector<CoreItemSP>::const_iterator itOperand = CoreVector::mVector.begin();
			std::vector<CoreItemSP>::const_iterator itOperandEnd = CoreVector::mVector.end();

			std::string	result("");

			while (itOperand != itOperandEnd)
			{
				result += (std::string)(**itOperand);
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
				std::vector<CoreItemSP>::const_iterator itOperand = CoreVector::mVector.begin();
				std::vector<CoreItemSP>::const_iterator itOperandEnd = CoreVector::mVector.end();

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
		inline SubOperator<std::string>::operator std::string() const
		{
			std::vector<CoreItemSP>::const_iterator itOperand = CoreVector::mVector.begin();

			std::string	result((std::string)(**itOperand));

			// no sub on string
			return result;
		}


		template<typename operandType>
		class MultOperator : public CoreItemOperator<operandType>
		{
		public:

			virtual inline operator operandType() const
			{
				std::vector<CoreItemSP>::const_iterator itOperand = CoreVector::mVector.begin();
				std::vector<CoreItemSP>::const_iterator itOperandEnd = CoreVector::mVector.end();

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
		inline MultOperator<std::string>::operator std::string() const
		{
			std::vector<CoreItemSP>::const_iterator itOperand = CoreVector::mVector.begin();

			std::string	result((std::string)(**itOperand));

			// no mult on string
			return result;
		}


		template<typename operandType>
		class DivOperator : public CoreItemOperator<operandType>
		{
		public:

			virtual inline operator operandType() const
			{
				std::vector<CoreItemSP>::const_iterator itOperand = CoreVector::mVector.begin();
				std::vector<CoreItemSP>::const_iterator itOperandEnd = CoreVector::mVector.end();

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
		inline DivOperator<std::string>::operator std::string() const
		{
			std::vector<CoreItemSP>::const_iterator itOperand = CoreVector::mVector.begin();

			std::string	result((std::string)(**itOperand));

			// no div on string
			return result;
		}


		template<typename operandType>
		class AbsOperator : public CoreItemOperator<operandType>
		{
		public:

			virtual inline operator operandType() const
			{
				std::vector<CoreItemSP>::const_iterator itOperand = CoreVector::mVector.begin();
		
				operandType	result((operandType)(**itOperand));

				return (result<(operandType)0) ? (-result) : result;
			}

			static std::unique_ptr<CoreVector> create()
			{
				return std::unique_ptr<CoreVector>(new AbsOperator<operandType>());
			}

		};

		template <  >
		inline AbsOperator<std::string>::operator std::string() const
		{
			std::vector<CoreItemSP>::const_iterator itOperand = CoreVector::mVector.begin();

			std::string	result((std::string)(**itOperand));

			// no abs on string
			return result;
		}


		template<typename operandType>
		class MaxOperator : public CoreItemOperator<operandType>
		{
		public:

			virtual inline operator operandType() const
			{

				std::vector<CoreItemSP>::const_iterator itOperand = CoreVector::mVector.begin();
				std::vector<CoreItemSP>::const_iterator itOperandEnd = CoreVector::mVector.end();

				operandType	result((operandType)(**itOperand));
				++itOperand;
				while (itOperand != itOperandEnd)
				{
					operandType current = (operandType)(**itOperand);
					if (current > result)
						result = current;
					++itOperand;
				}
				return result;
			}

			static std::unique_ptr<CoreVector> create()
			{
				return std::unique_ptr<CoreVector>(new MaxOperator<operandType>());
			}
		};

		template <  >
		inline MaxOperator<std::string>::operator std::string() const
		{
			std::vector<CoreItemSP>::const_iterator itOperand = CoreVector::mVector.begin();

			std::string	result((std::string)(**itOperand));

			// no max on string
			return result;
		}

		template<typename operandType>
		class MinOperator : public CoreItemOperator<operandType>
		{
		public:

			virtual inline operator operandType() const
			{

				std::vector<CoreItemSP>::const_iterator itOperand = CoreVector::mVector.begin();
				std::vector<CoreItemSP>::const_iterator itOperandEnd = CoreVector::mVector.end();

				operandType	result((operandType)(**itOperand));
				++itOperand;
				while (itOperand != itOperandEnd)
				{
					operandType current = (operandType)(**itOperand);
					if (current < result)
						result = current;
					++itOperand;
				}
				return result;
			}

			static std::unique_ptr<CoreVector> create()
			{
				return std::unique_ptr<CoreVector>(new MinOperator<operandType>());
			}
		};

		template <  >
		inline MinOperator<std::string>::operator std::string() const
		{
			std::vector<CoreItemSP>::const_iterator itOperand = CoreVector::mVector.begin();

			std::string	result((std::string)(**itOperand));

			// no min on string
			return result;
		}



		template<typename operandType>
		class SinusOperator : public CoreItemOperator<operandType>
		{
		public:

			virtual inline operator operandType() const
			{
				std::vector<CoreItemSP>::const_iterator itOperand = CoreVector::mVector.begin();

				operandType	result((operandType)(**itOperand));

				return sinf(result);
			}

			static std::unique_ptr<CoreVector> create()
			{
				return std::unique_ptr<CoreVector>(new SinusOperator<operandType>());
			}
		};

		template<typename operandType>
		class CosinusOperator : public CoreItemOperator<operandType>
		{
		public:

			virtual inline operator operandType() const
			{
				std::vector<CoreItemSP>::const_iterator itOperand = CoreVector::mVector.begin();

				operandType	result((operandType)(**itOperand));

				return cosf(result);
			}

			static std::unique_ptr<CoreVector> create()
			{
				return std::unique_ptr<CoreVector>(new CosinusOperator<operandType>());
			}

		};

		template<typename operandType>
		class TangentOperator : public CoreItemOperator<operandType>
		{
		public:

			virtual inline operator operandType() const
			{
				std::vector<CoreItemSP>::const_iterator itOperand = CoreVector::mVector.begin();

				operandType	result((operandType)(**itOperand));

				return tanf(result);
			}

			static std::unique_ptr<CoreVector> create()
			{
				return std::unique_ptr<CoreVector>(new TangentOperator<operandType>());
			}

		};


		template<typename operandType>
		class NegOperator : public CoreItemOperator<operandType>
		{
		public:

			virtual inline operator operandType() const
			{
				std::vector<CoreItemSP>::const_iterator itOperand = CoreVector::mVector.begin();

				operandType	result((*itOperand)->operator operandType());

				return -result;
			}

		};

		template <  >
		inline NegOperator<std::string>::operator std::string() const
		{
			std::vector<CoreItemSP>::const_iterator itOperand = CoreVector::mVector.begin();

			std::string	result((std::string)(**itOperand));

			// no neg on string
			return result;
		}

		// boolean
		class NotOperator : public CoreItemOperator<bool>
		{
		public:

			virtual inline operator bool() const
			{
				std::vector<CoreItemSP>::const_iterator itOperand = CoreVector::mVector.begin();

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
				std::vector<CoreItemSP>::const_iterator itOperand = CoreVector::mVector.begin();
		
				bool	result(false);
		
				if (CoreVector::mVector.size() == 2)
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
				std::vector<CoreItemSP>::const_iterator itOperand = CoreVector::mVector.begin();

				bool	result(false);

				if (CoreVector::mVector.size() == 2)
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
				std::vector<CoreItemSP>::const_iterator itOperand = CoreVector::mVector.begin();

				bool	result(false);

				if (CoreVector::mVector.size() == 2)
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
				std::vector<CoreItemSP>::const_iterator itOperand = CoreVector::mVector.begin();

				bool	result(false);

				if (CoreVector::mVector.size() == 2)
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
				std::vector<CoreItemSP>::const_iterator itOperand = CoreVector::mVector.begin();

				bool	result(false);

				if (CoreVector::mVector.size() == 2)
				{
					float	op1((float)(**itOperand));
					++itOperand;
					float	op2((float)(**itOperand));

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
				std::vector<CoreItemSP>::const_iterator itOperand = CoreVector::mVector.begin();

				bool	result(false);

				if (CoreVector::mVector.size() == 2)
				{
					float	op1((float)(**itOperand));
					++itOperand;
					float	op2((float)(**itOperand));

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
				std::vector<CoreItemSP>::const_iterator itOperand = CoreVector::mVector.begin();

				bool	result(false);

				if (CoreVector::mVector.size() == 2)
				{
					float	op1((float)(**itOperand));
					++itOperand;
					float	op2((float)(**itOperand));

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
				std::vector<CoreItemSP>::const_iterator itOperand = CoreVector::mVector.begin();

				bool	result(false);

				if (CoreVector::mVector.size() == 2)
				{
					float	op1((float)(**itOperand));
					++itOperand;
					float	op2((float)(**itOperand));

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
				std::vector<CoreItemSP>::const_iterator itOperand = CoreVector::mVector.begin();
				std::vector<CoreItemSP>::const_iterator itOperandEnd = CoreVector::mVector.end();

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

			DynamicVariableOperator(const std::string& varname) : mVarName(varname)
			{

			}

			virtual operator operandType() const;

		protected:

	
			std::string	mVarName;

		};



		template<typename operandType>
		class CoreModifiableAttributeOperator : public CoreItemOperator<operandType>
		{
		public:

			CoreModifiableAttributeOperator(const std::string& path, CoreModifiable* target) : mAttributePath(path), mTarget(target), mAttribute(0), mArrayAttributeIndex(-1), mIsMethod(0){};

			// construct from an existing attribute
			CoreModifiableAttributeOperator(CoreModifiableAttribute* attr) : mTarget(0), mAttribute(attr), mArrayAttributeIndex(-1), mIsMethod(0) 
			{
				mAttributePath = "";
			};

			virtual operator operandType() const override;

			virtual inline bool operator==(const CoreItem& other) const override
			{
				return (((float)(*this)) == (float)other);
			}

			virtual CoreItem& operator=(const operandType& other) override;

		protected:

			void GetAttribute() const;

			std::string				mAttributePath;
			CoreModifiable*				mTarget;

			union
			{
				CoreModifiableAttribute*	mAttribute;
				unsigned int				mMethodID;
			}; 

			short						mArrayAttributeIndex;
			short						mIsMethod;
		};

		template<>
		inline bool CoreModifiableAttributeOperator<std::string>::operator == (const CoreItem& other) const
		{
			std::string	othervalue;
			other.getValue(othervalue);

			std::string	thisvalue;
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
				std::vector<CoreItemSP>::const_iterator itOperand = CoreVector::mVector.begin();

				operandType	result(0);

				bool	test((bool)(*(*itOperand).get()));
		
				if (test != 0)
				{
					if (CoreVector::mVector.size() > 1)
					{
						++itOperand;
						result = (operandType)*(*itOperand).get();
					}
				}
				else
				{
					if (CoreVector::mVector.size() > 2)
					{
						++itOperand;
						++itOperand;

						result = (operandType)*(*itOperand).get();
					}
				}

		
				return result;
			}
			static std::unique_ptr<CoreVector> create()
			{
				return std::unique_ptr<CoreVector>(new IfThenElseOperator<operandType>());
			}
		};


	}

}