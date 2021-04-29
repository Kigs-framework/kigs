#ifndef _NODE2D_H_
#define _NODE2D_H_

#include "CoreModifiable.h"
#include "CoreModifiableAttribute.h"
#include "TecLibs/Tec3D.h"

#include "AttributePacking.h"

class TravState;
class Drawable;
class RendererMatrix;
class Abstract2DLayer;

// ****************************************
// * Node2D class
// * --------------------------------------
/**
* \file	Node2D.h
* \class	Node2D
* \ingroup  2DLayers
* \brief	A Node2D is the base class for all objects to be added to a 2DLayer.
*
* A matrix is associated with each node to give the transformation applied to the node when drawing.
*/
// ****************************************

class Node2D : public CoreModifiable
{
public:
	// realsize compute mode
	enum SizeMode
	{
		DEFAULT=0,					// use directly the size set in mSizeX or mSizeY
		MULTIPLY=1,					// multiply father size by mSizeX or mSizeY
		ADD=2,						// add father size to mSizeX or mSizeY
		CONTENT = 3,				// use content size ( texture size of UIImage )
		CONTENT_MULTIPLY = 4,		// multiply content size by mSizeX or mSizeY
		CONTENT_ADD = 5,			// add content size to mSizeX or mSizeY
		KEEP_RATIO	=6				// compute size x according to size y or size y according to size x. ! Both mode can't be KEEP_RATION ! 
	};

	friend class ModuleSceneGraph;

	struct PriorityCompare
	{
		//! overload operator () for comparison
		bool operator()(const Node2D * a1, const Node2D * a2) const
		{
			if (a1->mPriority == a2->mPriority)
				return (a1) < (a2);
			return a1->mPriority > a2->mPriority;
		}
	};
	friend struct PriorityCompare;

	DECLARE_CLASS_INFO(Node2D, CoreModifiable, SceneGraph)
	DECLARE_CONSTRUCTOR(Node2D);

	// call just before travdraw, set state for branch (this and his descendancy)
	virtual void PreTravDraw(TravState* state) {}
	virtual void PostTravDraw(TravState* state) {}

	/**
	* \brief	draw the node
	* \fn 		virtual bool	Draw(TravState* state);
	* \param	state : the current TravState
	* \return	TRUE if this node is visible or we are in a "all visible" branch of the scenegraph
	*/
	virtual bool Draw(TravState* state);

	/**
	* \brief	draw the TravState
	* \fn 		virtual void			TravDraw(TravState* state);
	* \param	state : the current TravState
	*/
	virtual void TravDraw(TravState* state);

	/**
	* \brief	this method is called to notify this that one of its attribute has changed.
	* \fn 		virtual void NotifyUpdate(const unsigned int);
	* \param	int : not used
	*/
	void NotifyUpdate(const unsigned int /* labelid */) override;

	// real drawing
	virtual void ProtectedDraw(TravState* state) { ; }

	int GetFinalPriority() const;

	v2f GetSize() const { return mRealSize; }
	v2f GetPreScale() const { return v2f{ mPreScaleX, mPreScaleY }; }
	v2f GetPostScale() const { return v2f{ mPostScaleX, mPostScaleY }; }
	v2f GetSizeScaled() const { return GetSize() * GetPreScale() * GetPostScale(); }
	virtual float GetOpacity() { return 1.0f; }

	Node2D*								getFather() const;
	Abstract2DLayer*					getLayerFather() const;

	bool										addItem(const CMSP& item, ItemPosition pos = Last DECLARE_DEFAULT_LINK_NAME) override;
	bool										removeItem(const CMSP& item DECLARE_DEFAULT_LINK_NAME) override;

	inline const kstl::set<Node2D*, Node2D::PriorityCompare>& GetSons() { ResortSons(); return mSons; }

	// transform local points to global
	inline void	TransformPoints(Point2D* totransform, int count) const
	{
		mGlobalTransformMatrix.TransformPoints(totransform, count);
	}

	v2f GetPosition() const { return mPosition; }
	void GetPosition(kfloat &X, kfloat &Y) const
	{
		X = (kfloat)mPosition[0];
		Y = (kfloat)mPosition[1];
	}

	void	GetGlobalPosition(kfloat &X, kfloat &Y);

	inline const Matrix3x3& GetGlobalTransform() const
	{
		return mGlobalTransformMatrix;
	}

	void GetTransformedPoints(Point2D * pt);


	virtual void SetUpNodeIfNeeded();
	// recurse to retreive root 2D Layer
	Abstract2DLayer*									getRootLayerFather() const;
	Node2D*												getRootFather();

	enum Flags
	{
		Node2D_SizeChanged = 1u << 0u,
		Node2D_Clipped = 1u << 1u,
		Node2D_Hidden = 1u << 2u,
		Node2D_NeedUpdatePosition = 1u << 3u,
		Node2D_SonPriorityChanged = 1u << 4u,

		Node2D_PropagatedFlags = Node2D_Clipped | Node2D_Hidden,
	};

	u32													GetNodeFlags() const { return mFlags; }
	inline bool												GetNodeFlag(Flags f)
	{
		return mFlags & f;
	}
	inline void												SetNodeFlag(Flags f)
	{
		mFlags |= f;
	}
	inline void												ClearNodeFlag(Flags f)
	{
		mFlags &= ~(f);
	}

	bool IsHiddenFlag() const { return (mFlags & Node2D_Hidden) != 0; }
	bool IsInClip(v2f pos) const;

protected:

	virtual v2f	GetContentSize()
	{
		return mRealSize;
	}

	/**
	* \brief	initialize modifiable
	* \fn 		void InitModifiable() override
	*/
	void	InitModifiable() override;

	void SetParent(CoreModifiable* value);

	void PropagateNodeFlags();

	void												PropagateNeedUpdateToFather();

	void												ComputeMatrices();
	virtual void										ComputeRealSize();
	void												ResortSons();

	u32													mFlags = 0;

	Node2D*												mParent;
	kstl::set<Node2D*, Node2D::PriorityCompare>			mSons;

	v2f													mRealSize{ 0,0 };

	Matrix3x3											mLocalTransformMatrix;
	Matrix3x3											mGlobalTransformMatrix;

	maInt												mPriority;
	maFloat												mSizeX;
	maFloat												mSizeY;
	maVect2DF											mDock;
	maVect2DF											mAnchor;
	maVect2DF											mPosition;
	maFloat												mRotationAngle;
	maFloat												mPreScaleX;
	maFloat												mPreScaleY;
	maFloat												mPostScaleX;
	maFloat												mPostScaleY;
	maEnum<7>											mSizeModeX = BASE_ATTRIBUTE(SizeModeX, "Default", "Multiply", "Add", "Content","ContentMult","ContentAdd","KeepRatio");
	maEnum<7>											mSizeModeY = BASE_ATTRIBUTE(SizeModeY, "Default", "Multiply", "Add", "Content", "ContentMult", "ContentAdd", "KeepRatio");
	maBoolHeritage<1>									mClipSons;
	//bool												mNeedUpdatePosition;
	//bool												mSonPriorityChanged;
	maReference											mCustomShader = BASE_ATTRIBUTE(CustomShader, "");

	WRAP_METHODS(GetSize);
};

#endif //_NODE3D_H_
