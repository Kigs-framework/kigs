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

class SceneNode : public CoreModifiable 
{
public:
	DECLARE_ABSTRACT_CLASS_INFO(SceneNode,CoreModifiable,SceneGraph)
	DECLARE_CONSTRUCTOR(SceneNode);
	
	bool addItem(CoreModifiable *item, ItemPosition pos=Last DECLARE_DEFAULT_LINK_NAME) override;
	bool removeItem(CoreModifiable* item DECLARE_DEFAULT_LINK_NAME) override;

	bool IsInScene() const;

	inline bool HasFlag(unsigned int flag) const { return ((flag&myFlags) == flag); }
	inline void SetFlag(unsigned int flag) { myFlags |= flag; }
	inline void UnsetFlag(unsigned int flag) { myFlags &= ~flag; }

	inline void SetIsRenderable(bool b) { if (b) myShowNode |= SceneNode_Render; else myShowNode &= ~SceneNode_Render; }
	inline bool IsRenderable() {
		return (u32)myShowNode & SceneNode_Render;
	}


	inline void SetIsCollidable(bool b) { if (b) myShowNode |= SceneNode_Collide; else myShowNode &= ~SceneNode_Collide; }
	inline bool IsCollidable()
	{
		return (u32)myShowNode & SceneNode_Collide;
	}

	bool IsVisibleInScene();
	
	virtual void GetNodeBoundingBox(Point3D& pmin, Point3D& pmax) const
	{
		pmin.Set(0.0f, 0.0f, 0.0f);
		pmax.Set(-1.0f, -1.0f, -1.0f);
	}

	static constexpr unsigned int usedFlags = 6;

	enum SceneNodeFlags : unsigned int
	{
		AllSonsVisible = 1 << 0,
		LocalToGlobalMatrixIsDirty = 1 << 1,
		GlobalToLocalMatrixIsDirty = 1 << 2,
		BoundingBoxIsDirty = 1 << 3,
		GlobalBoundingBoxIsDirty = 1 << 4,
		IsScaledFlag = 1 << 5
	};

	bool	CompareMatrix(const Matrix3x4& m1, const Matrix3x4& m2)
	{
		return (memcmp(&m1, &m2, sizeof(Matrix3x4)) == 0);
	}

protected:

	unsigned int	myFlags;
	

	inline bool	IsAllSonsVisible() { return HasFlag(AllSonsVisible); }
	inline void	SetAllSonsVisible() { SetFlag(AllSonsVisible); }
	inline void	UnsetAllSonsVisible() { UnsetFlag(AllSonsVisible); }

private:
	maUInt myShowNode = BASE_ATTRIBUTE(Show, UINT_MAX);
};

#endif //_SCENENODE_H_
