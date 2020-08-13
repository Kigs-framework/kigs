#pragma once

#include <CoreBaseApplication.h>

class KigsDocs : public CoreBaseApplication
{
public:
	DECLARE_CLASS_INFO(KigsDocs, CoreBaseApplication, Core);
	DECLARE_CONSTRUCTOR(KigsDocs);



protected:


	void	ProtectedInit() override;
	void	ProtectedUpdate() override;
	void	ProtectedClose() override;

	struct documentedInstances
	{
		CMSP					mInstance;
		std::string				mClassName;
		documentedInstances*	mFather=nullptr;
		std::map<std::string, CoreModifiableAttribute*>		mAttributes;
		std::map<std::string, ModifiableMethodStruct>		mMethods;

		static bool isParentAttribute(documentedInstances* parent,const std::string& attrName)
		{
			if (parent)
			{
				auto found = parent->mAttributes.find(attrName);
				if (found != parent->mAttributes.end())
				{
					return true;
				}
				if (parent->mFather)
				{
					return isParentAttribute(parent->mFather, attrName);
				}
			}
			return false;
		}
		static bool isParentMethod(documentedInstances* parent, const std::string& methodName)
		{
			if (parent)
			{
				auto found = parent->mMethods.find(methodName);
				if (found != parent->mMethods.end())
				{
					return true;
				}
				if (parent->mFather)
				{
					return isParentMethod(parent->mFather, methodName);
				}
			}
			return false;
		}

		bool hasAttribute() const
		{
			return (mAttributes.size() > 0);
		}

		bool hasMethod() const
		{
			return (mMethods.size() > 0);
		}

		bool hierarchyHasAttribute() const
		{
			const documentedInstances* current = this;
			while (current)
			{
				if (current->hasAttribute())
				{
					return true;
				}
				current = current->mFather;
			}
			return false;
		}
		bool hierarchyHasMethod() const
		{
			const documentedInstances* current = this;
			while (current)
			{
				if (current->hasMethod())
				{
					return true;
				}
				current = current->mFather;
			}
			return false;
		}
	};
	void	RecursiveTreatClass(CoreTreeNode* n, documentedInstances* parent);

	void	ExportDoc(const std::pair<std::string, documentedInstances>& i);
	void	ExportDetailedAttribute(std::ofstream& DocFile, const std::pair<std::string, documentedInstances>& i, const std::pair < std::string, CoreModifiableAttribute*>& a,const std::string& ExactType);
	void	ExportInheritedAttributeList(std::ofstream& DocFile, documentedInstances*);

	void	ExportDetailedMethod(std::ofstream& DocFile, const std::pair<std::string, documentedInstances>& i, const std::pair < std::string, ModifiableMethodStruct>& a, const std::string& ExactType);
	void	ExportInheritedMethodList(std::ofstream& DocFile, documentedInstances*);

	void usage();

	std::unordered_map<std::string, documentedInstances>	mMap;

	maString	mOutPath = BASE_ATTRIBUTE(OutPath, "./");
};
