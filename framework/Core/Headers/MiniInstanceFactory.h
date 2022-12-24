#pragma once

//! instance factory for classes outside the framework
// useful when there's a need to create structures / classes instances from an ini or xml file

// use it as a singleton

/**
	Usage sample :

	MiniInstanceFactory* sampleFactory=(MiniInstanceFactory*)KigsCore::GetSingleton("MiniInstanceFactory");
	MiniFactoryRegister(sampleFactory,"SampleClass",SampleClass);

	...
	...
	...

	SampleClass* sqrb=(SampleClass*)sampleFactory->CreateClassInstance("SampleClass");

	...
	...

*/

#include "Core.h"
#include "CoreModifiable.h"

#include <unordered_map>

namespace Kigs
{
	namespace Core
	{
#define MiniFactoryRegister(factory,a,b)	{factory->RegisterNewClass<b>(a);}

		class	MiniInstanceFactory : public CoreModifiable
		{
		public:

			DECLARE_CLASS_INFO(MiniInstanceFactory, CoreModifiable, InstanceFactory)

				//! constructor
				MiniInstanceFactory(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);

			class FactoryCreateStruct
			{
			public:
				virtual ~FactoryCreateStruct()
				{
				}
				virtual void* createStruct() = 0;
			};

			template<typename T>
			class	RegisterClass : public FactoryCreateStruct
			{
			public:
				// nothing else to do
				RegisterClass() { ; }

				void* createStruct()
				{
					return new T();
				}
			protected:
			};

			template<typename T>
			void	RegisterNewClass(const std::string& name)
			{
				auto alreadyfound = mFactoryMap.find(name);
				if (alreadyfound != mFactoryMap.end())
				{
					KIGS_WARNING("Class already registered in MiniFactory\n", 1);
				}

				mFactoryMap[name] = new RegisterClass<T>();

			}
			void* CreateClassInstance(const std::string& name)
			{
				auto alreadyfound = mFactoryMap.find(name);
				if (alreadyfound == mFactoryMap.end())
				{
					KIGS_ERROR("Can not find class in MiniFactory\n", 1);
					return 0;
				}

				return (*alreadyfound).second->createStruct();
			}
			virtual ~MiniInstanceFactory();

		protected:
			std::unordered_map<std::string, FactoryCreateStruct*>	mFactoryMap;
		};


	}
}