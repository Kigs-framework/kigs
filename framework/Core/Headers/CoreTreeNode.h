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

	void getInstances(std::vector<CMSP>& result, bool recursive, bool only_one = false) const;
	void getInstancesByName(std::vector<CMSP>& result, bool recursive, const std::string& name, bool only_one = false);
	void getRootInstances(std::vector<CMSP>& result, bool recursive) const; 


	kigs::unordered_map<KigsID, CoreTreeNode*> mChildren;
	kigs::unordered_map <KigsID, ModifiableMethodStruct> mMethods;
	std::vector<std::pair<CoreModifiable*, std::weak_ptr<CoreModifiable>>> mInstances;
	std::unordered_map<std::string, size_t> mNameLookup;
	bool mHasNameLookup=false;

	CoreTreeNode* mFather;
	KigsID mID;

	mutable std::recursive_mutex mMutex;
};