#include <KigsDocs.h>
#include "ModuleGUI.h"
#include "ModuleInput.h"
#include "ModuleRenderer.h"
#include "ModuleSceneGraph.h"
#include "Module2DLayers.h"
#include "ModuleCoreAnimation.h"
#include "LuaKigsBindModule.h"
#include "GenericAnimationModule.h"
#include "CollisionModule.h"
#include "HTTPRequestModule.h"
#include "ModuleThread.h"

#include <iostream>
#include <fstream>

void	KigsDocs::usage()
{
	printf("Usage : \n");
	printf("KigsDocs [-p outputPath] \n");
}


IMPLEMENT_CLASS_INFO(KigsDocs);

IMPLEMENT_CONSTRUCTOR(KigsDocs)
{

}

void	KigsDocs::RecursiveTreatClass(CoreTreeNode* n, KigsDocs::documentedInstances* parent)
{
	
	KigsDocs::documentedInstances* keepParent = parent;

	if (mMap.find(n->mID._id_name) == mMap.end())
	{
		documentedInstances& toAdd = mMap[n->mID._id_name];
		toAdd.mClassName = n->mID._id_name;
		
		keepParent = &toAdd;
		toAdd.mInstance = KigsCore::GetInstanceOf(n->mID._id_name + "_instance", n->mID);
		
		toAdd.mFather = parent;
		if (toAdd.mInstance)
		{
			if (!toAdd.mInstance->isSubType("DoNothingObject") || (n->mID._id_name == "DoNothingObject"))
			{
				for (auto& a : toAdd.mInstance->getAttributes())
				{
					if (!documentedInstances::isParentAttribute(parent, a.first._id_name))
					{
						toAdd.mAttributes[a.first._id_name] = a.second;
					}
				}

				// instances methods
				auto methods = toAdd.mInstance->GetMethods();

				if (methods)
					for (auto& m : *methods)
					{
						if (!documentedInstances::isParentMethod(parent, m.first._id_name))
						{
							toAdd.mMethods[m.first._id_name] = (m.second);
						}
					}

				// class methods
				auto classmethods = &n->mMethods;
				for (auto& m : *classmethods)
				{
					if (!documentedInstances::isParentMethod(parent, m.first._id_name))
					{
						toAdd.mMethods[m.first._id_name] = (m.second);
					}
				}
			}
		}
	}

	for (auto& s : n->mChildren)
	{
		RecursiveTreatClass(s.second, keepParent);
	}
}

void	KigsDocs::ProtectedInit()
{
	// Base modules have been created at this point
	// lets say that the update will sleep 1ms

	CoreCreateModule(ModuleGUI, 0);
	CoreCreateModule(ModuleInput, 0);
	CoreCreateModule(ModuleRenderer, 0);
	CoreCreateModule(ModuleSceneGraph, 0);
	CoreCreateModule(Module2DLayers, 0);
	CoreCreateModule(ModuleCoreAnimation, 0);
	CoreCreateModule(LuaKigsBindModule, 0);
	CoreCreateModule(GenericAnimationModule, 0);
	CoreCreateModule(CollisionModule, 0);
	CoreCreateModule(HTTPRequestModule, 0);
	CoreCreateModule(ModuleThread, 0);

	SetUpdateSleepTime(1);

	// parse arguments
	// retreive args
	kstl::vector<kstl::string>::iterator itArgs = mArgs.begin();
	// skip app name
	itArgs++;

	kstl::string outpath = "";

	for (; itArgs != mArgs.end(); itArgs++)
	{
		kstl::string& current = (*itArgs);

		if (current.at(0) == '-')
		{
			char argtype = current.at(1);
			switch (argtype)
			{
			case 'p':
			{
				if (outpath != "")
				{
					usage();
					exit(-1);
				}
				itArgs++;
				outpath = (*itArgs);
			}
			break;
			default:
				usage();
				exit(-1);
				break;
			}
		}
	}

	if (outpath != "")
	{
		mOutPath = outpath + "/";
	}

	auto r = KigsCore::GetRootNode();
	auto coreModifiableR = r->mChildren["CoreModifiable"];
	RecursiveTreatClass(coreModifiableR,nullptr);


	for (const auto& i : mMap)
	{
		if ((i.second.hierarchyHasAttribute()) || (i.second.hierarchyHasMethod()))
		{
			ExportDoc(i);
		}
	}

	// clear all instances
	mMap.clear();
}

void	KigsDocs::ExportDoc(const std::pair<std::string, documentedInstances>& i)
{
	if (i.second.mInstance->isSubType("DoNothingObject") && (i.second.mClassName != "DoNothingObject"))
	{
		return;
	}

	std::string exactType = i.first;

	// type and exact type don't match ?
	if (i.second.mInstance->getExactType() != i.first)
	{
		CoreClassNameTree classNameTree;
		i.second.mInstance->ConstructClassNameTree(classNameTree);

		bool foundInParent=false;
		for (const auto& names : classNameTree.classNames())
		{
			if (names.mClassName == i.first)
			{
				foundInParent = true;
				break;
			}
		}

		// not found in parent, use exact type
		if (foundInParent == false)
		{
			exactType = i.second.mInstance->getExactType();
		}
	}


	std::string filename = mOutPath.const_ref() + exactType + ".txt";
	std::ofstream DocFile;
	DocFile.open(filename);

	// common part 

	DocFile << "/*!" << std::endl << "* \\class ";

	// class name
	DocFile << exactType;

	// attributes
	if (i.second.hasAttribute())
	{
		DocFile << std::endl << "## Attributes";
		
		// Init attributes first
		for (const auto& a : i.second.mAttributes)
		{
			CoreModifiableAttribute* currentAttr = i.second.mInstance->getAttribute(a.first);
			if (currentAttr->isInitParam())
			{
				ExportDetailedAttribute(DocFile, i, a, exactType);
			}
		}

		// Then others 
		for (const auto& a : i.second.mAttributes)
		{
			CoreModifiableAttribute* currentAttr = i.second.mInstance->getAttribute(a.first);
			if (!currentAttr->isInitParam())
			{
				ExportDetailedAttribute(DocFile, i, a, exactType);
			}
		}

		DocFile << std::endl;
	}

	// inherited attributes ?
	if (i.second.mFather->hierarchyHasAttribute())
	{
		DocFile << std::endl << "## Inherited Attributes";
		DocFile << std::endl << "\\secreflist" << std::endl;
		documentedInstances* current = i.second.mFather;
		while (current)
		{
			ExportInheritedAttributeList(DocFile, current);
			current = current->mFather;
		}

		DocFile << std::endl << "\\endsecreflist" << std::endl;
	}

	// methods
	if (i.second.hasMethod())
	{
		DocFile << std::endl << "## Methods";
		
		for (const auto& a : i.second.mMethods)
		{
			ExportDetailedMethod(DocFile, i, a,exactType);
		}
		DocFile << std::endl;
	}

	// inherited methods ?
	if (i.second.mFather->hierarchyHasMethod())
	{
		DocFile << std::endl << "## Inherited Methods";
		DocFile << std::endl << "\\secreflist" << std::endl;
		documentedInstances* current = i.second.mFather;
		while (current)
		{
			ExportInheritedMethodList(DocFile, current);
			current = current->mFather;
		}

		DocFile << std::endl << "\\endsecreflist" << std::endl;
	}


	DocFile << "*/";

	DocFile.close();
}

void	KigsDocs::ExportInheritedAttributeList(std::ofstream& DocFile, documentedInstances* current)
{
	if (current->hasAttribute())
	{
		std::string className = current->mClassName;
		for (const auto& a : current->mAttributes)
		{
			CoreModifiableAttribute* currentAttr = current->mInstance->getAttribute(a.first);
			std::string anchor_name = className + "_" + a.first + "_attr";
			DocFile << "\\refitem " << anchor_name << " \"(" << className << ") " << a.first << "\" " << std::endl;
		}
	}
}

void	KigsDocs::ExportInheritedMethodList(std::ofstream& DocFile, documentedInstances* current)
{
	if (current->hasMethod())
	{
		std::string className = current->mClassName;
		for (const auto& m : current->mMethods)
		{
			std::string anchor_name = className + "_" + m.first + "_meth";
			DocFile << "\\refitem " << anchor_name << " \"(" << className << ") " << m.first << "\" " << std::endl;
		}
	}
}

void	KigsDocs::ExportDetailedAttribute(std::ofstream& DocFile, const std::pair<std::string, documentedInstances>& i, const std::pair < std::string, CoreModifiableAttribute*>& a, const std::string& ExactType)
{
	CoreModifiableAttribute* currentAttr = i.second.mInstance->getAttribute(a.first);

	// first create an anchor

	DocFile << std::endl << "\\anchor " << i.first << "_" << a.first << "_attr";

	// key and is init attribute ?
	DocFile << std::endl << "### Key : ";
	DocFile << a.first;
	if (currentAttr->isInitParam())
	{
		DocFile << " (Init attribute)";
	}
	DocFile << std::endl;
	// type 
	DocFile << "- Type : " << CoreModifiableAttribute::typeToString(currentAttr->getType());
	// type specific details
	if (currentAttr->getType() == ATTRIBUTE_TYPE::ENUM)
	{
		kstl::vector<kstl::string> values = currentAttr->getEnumElements();
		DocFile << " { \"";
		bool first = true;
		for (const auto& str : values)
		{
			if (!first)
			{
				DocFile << "\" , \"";
			}
			first = false;
			DocFile << str;
		}
		DocFile << "\" }";
	}
	else if (currentAttr->getType() == ATTRIBUTE_TYPE::ARRAY)
	{
		if (currentAttr->getNbArrayLines() > 1) // 2D
		{
			DocFile << " [ " << currentAttr->getNbArrayLines() << " , " << currentAttr->getNbArrayColumns() << " ]";
		}
		else
		{
			DocFile << " [ " << currentAttr->getNbArrayColumns() << " ]";
		}
	}

	DocFile << std::endl;

	// default value
	std::string defaultvalue = "";
	currentAttr->getValue(defaultvalue);
	if (defaultvalue != "")
	{
		DocFile << "- Default Value : " << defaultvalue << std::endl;
	}

	// exact type
	DocFile << "\\copydoc " << ExactType << "::m" << a.first << std::endl;
	
	// then with parents type
	CoreClassNameTree classNameTree;
	i.second.mInstance->ConstructClassNameTree(classNameTree);

	for(auto& v : classNameTree.classNames())
	{
		if ((v.mRuntimeName._id_name == "") && (v.mClassName._id_name != ExactType))
		{
			std::string parentName = v.mClassName._id_name;
			DocFile << "\\copydoc " << parentName << "::m" << a.first << std::endl;
		}
	}
}

void	KigsDocs::ExportDetailedMethod(std::ofstream& DocFile, const std::pair<std::string, documentedInstances>& i, const std::pair < std::string, ModifiableMethodStruct>& m, const std::string& ExactType)
{
	const ModifiableMethodStruct& currentMethod = m.second;

	// first create an anchor

	DocFile << std::endl << "\\anchor " << i.first << "_" << m.first << "_meth";

	// key and is init attribute ?
	DocFile << std::endl << "### Key : " << m.first << std::endl;
	
	// exact type
	DocFile << "\\copydoc " << ExactType << "::" << m.first << "()" << std::endl;
	// then with parents type
	CoreClassNameTree classNameTree;
	i.second.mInstance->ConstructClassNameTree(classNameTree);

	for (auto& v : classNameTree.classNames())
	{
		if ((v.mRuntimeName._id_name == "") && (v.mClassName._id_name != ExactType))
		{
			std::string parentName = v.mClassName._id_name;
			DocFile << "\\copydoc " << parentName << "::" << m.first << "()" << std::endl;
		}
	}
}

void	KigsDocs::ProtectedUpdate()
{
	// brute force exit
	exit(0);
}

void	KigsDocs::ProtectedClose()
{
}
