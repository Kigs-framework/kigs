#ifndef _SCENENODE_H_
#define _SCENENODE_H_

#include "CoreModifiable.h"
#include "CoreModifiableAttribute.h"

class TravState;
class TravPath;

enum SceneNodeShow
{
	SceneNode_Render  = 1<<0,
	SceneNode_Collide = 1<<1
};
// ****************************************
// * SceneNode class
// * --------------------------------------
/**
 * \file	SceneNode.h
 * \class	SceneNode
 * \ingroup SceneGraph
 * \brief   Abstract base class for objects in the scene ( Node3D, Drawables...) .
 */
 // ****************************************
class SceneNode : public CoreModifiable 
{
public:
	DECLARE_ABSTRACT_CLASS_INFO(SceneNode,CoreModifiable,SceneGraph)
	DECLARE_CONSTRUCTOR(SceneNode);
	
	static constexpr unsigned int UserFlagNode3D = 1 << ParentClassType::usedUserFlags;
	static constexpr unsigned int UserFlagDrawable = 1 << (ParentClassType::usedUserFlags +1);
	static constexpr unsigned int UserFlagCameraSort = 1 << (ParentClassType::usedUserFlags +2);
	static constexpr unsigned int UserFlagFrontToBackSort = 1 << (ParentClassType::usedUserFlags +3);

	static constexpr unsigned int AllSonsVisible = 1 << (ParentClassType::usedUserFlags + 4);
	static constexpr unsigned int LocalToGlobalMatrixIsDirty = 1 << (ParentClassType::usedUserFlags + 5);
	static constexpr unsigned int GlobalToLocalMatrixIsDirty = 1 << (ParentClassType::usedUserFlags + 6);
	static constexpr unsigned int BoundingBoxIsDirty = 1 << (ParentClassType::usedUserFlags + 7);
	static constexpr unsigned int GlobalBoundingBoxIsDirty = 1 << (ParentClassType::usedUserFlags + 8);
	static constexpr unsigned int IsScaledFlag = 1 << (ParentClassType::usedUserFlags + 9);

	static constexpr unsigned int usedUserFlags = ParentClassType::usedUserFlags + 10;

	bool addItem(const CMSP& item, ItemPosition pos=Last DECLARE_DEFAULT_LINK_NAME) override;
	bool removeItem(const CMSP& item DECLARE_DEFAULT_LINK_NAME) override;

	bool IsInScene() const;

	inline void SetIsRenderable(bool b) { if (b) mShow |= SceneNode_Render; else mShow &= ~SceneNode_Render; }
	inline bool IsRenderable() {
		return (u32)mShow & SceneNode_Render;
	}


	inline void SetIsCollidable(bool b) { if (b) mShow |= SceneNode_Collide; else mShow &= ~SceneNode_Collide; }
	inline bool IsCollidable()
	{
		return (u32)mShow & SceneNode_Collide;
	}

	bool IsVisibleInScene();
	
	virtual void GetNodeBoundingBox(Point3D& pmin, Point3D& pmax) const
	{
		pmin.Set(0.0f, 0.0f, 0.0f);
		pmax.Set(-1.0f, -1.0f, -1.0f);
	}

	bool	CompareMatrix(const Matrix3x4& m1, const Matrix3x4& m2)
	{
		return (memcmp(&m1, &m2, sizeof(Matrix3x4)) == 0);
	}

protected:

	inline bool	IsAllSonsVisible() { return isUserFlagSet(AllSonsVisible); }
	inline void	SetAllSonsVisible() { isUserFlagSet(AllSonsVisible); }
	inline void	UnsetAllSonsVisible() { isUserFlagSet(AllSonsVisible); }

private:
	maUInt mShow = BASE_ATTRIBUTE(Show, UINT_MAX);
};

#endif //_SCENENODE_H_
