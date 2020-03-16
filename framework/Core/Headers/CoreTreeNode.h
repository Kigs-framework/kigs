#pragma once

#include "CoreModifiable.h"

class CoreTreeNode
{
public:
	explicit CoreTreeNode(CoreTreeNode* father, KigsID id) : myChildren(), myInstances(), myFather(father), myID(id) /*, myClassNameTree(0) */ {}
	explicit CoreTreeNode(const CoreTreeNode& node) = delete;
	CoreTreeNode& operator=(const CoreTreeNode& node) = delete;

	~CoreTreeNode();

	void AddInstance(CoreModifiable* toAdd);
	void RemoveInstance(CoreModifiable* toRemove);
	size_t GetInstanceCount() const;

	// Not thread safe if getref is false
	void getInstances(std::set<CoreModifiable*>& instances, bool recursive, bool only_one = false, bool getref = false) const;
	void getInstancesByName(std::set<CoreModifiable*>& instances, bool recursive, const std::string& name, bool only_one = false, bool getref = false);
	void getRootInstances(std::set<CoreModifiable*>& instances, bool recursive, bool get_ref = false) const;
	/////

	void getInstances(std::vector<CMSP>& result, bool recursive, bool only_one = false) const;
	void getInstancesByName(std::vector<CMSP>& result, bool recursive, const std::string& name, bool only_one = false);


	kigs::unordered_map<KigsID, CoreTreeNode*> myChildren;
	kigs::unordered_map <KigsID, ModifiableMethodStruct> myMethods;
	std::vector<CoreModifiable*> myInstances;
	
	bool myInstanceVectorNeedSort = false;

	void sortInstanceVector();
	CoreTreeNode* myFather;
	KigsID myID;

	mutable std::recursive_mutex myMutex;
};