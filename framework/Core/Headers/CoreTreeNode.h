#pragma once

#include "CoreModifiable.h"

// ****************************************
// * CoreTreeNode class
// * --------------------------------------
/**
 * \file	CoreTreeNode.h
 * \class	CoreTreeNode
 * \ingroup Core
 * \brief	Manage all CoreModifiable classes and instances tree.
 */
 // ****************************************

class CoreTreeNode
{
public:
	explicit CoreTreeNode(CoreTreeNode* father, KigsID id) : mChildren(), mInstances(), mFather(father), mID(id) {}
	explicit CoreTreeNode(const CoreTreeNode& node) = delete;
	CoreTreeNode& operator=(const CoreTreeNode& node) = delete;

	~CoreTreeNode();

	void AddInstance(CoreModifiable* toAdd);
	void RemoveInstance(CoreModifiable* toRemove);
	size_t GetInstanceCount() const;

	// Not thread safe if getref is false
	/*void getInstances(std::set<CoreModifiable*>& instances, bool recursive, bool only_one = false, bool getref = false) const;
	void getInstancesByName(std::set<CoreModifiable*>& instances, bool recursive, const std::string& name, bool only_one = false, bool getref = false);
	void getRootInstances(std::set<CoreModifiable*>& instances, bool recursive, bool get_ref = false) const;*/
	/////

	void getInstances(std::vector<CMSP>& result, bool recursive, bool only_one = false) const;
	void getInstancesByName(std::vector<CMSP>& result, bool recursive, const std::string& name, bool only_one = false);
	void getRootInstances(std::vector<CMSP>& result, bool recursive) const; 


	kigs::unordered_map<KigsID, CoreTreeNode*> mChildren;
	kigs::unordered_map <KigsID, ModifiableMethodStruct> mMethods;
	std::vector<std::pair<CoreModifiable*, std::weak_ptr<CoreModifiable>>> mInstances;
	
	bool mInstanceVectorNeedSort = false;

	void sortInstanceVector();
	CoreTreeNode* mFather;
	KigsID mID;

	mutable std::recursive_mutex mMutex;
};