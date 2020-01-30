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
* \ingroup SceneGraph
* \brief	 A Node2D is the base class for all objects to be added to a 2DLayer
* \author	ukn
* \version ukn
* \date	ukn
*
* A matrix is associated with each node to give the transformation applied to the node when drawing
*/
// ****************************************


class Node2D : public CoreModifiable
{
public:
	enum SizeMode
	{
		DEFAULT=0,
		MULTIPLY=1,
		ADD=2
	};

	friend class ModuleSceneGraph;
	friend class UIBoxLayout;
	struct PriorityCompare
	{
		//! overload operator () for comparison
		bool operator()(const Node2D * a1, const Node2D * a2) const
		{
			if (a1->myPriority == a2->myPriority)
				return (a1) < (a2);
			return a1->myPriority > a2->myPriority;
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

	/**
	* \brief	retreive father (Node2D can only have one father, they can not be shared)
	* \fn 		virtual Node2D* getFather();
	*/

	//int GetPriority() const { return myPriority; }

	int GetFinalPriority() const;

	v2f GetSize() const { return myRealSize; }
	v2f GetPreScale() const { return v2f{ myPreScaleX, myPreScaleY }; }
	v2f GetPostScale() const { return v2f{ myPostScaleX, myPostScaleY }; }
	v2f GetSizeScaled() const { return GetSize() * GetPreScale() * GetPostScale(); }
	virtual float GetOpacity() { return 1.0f; }

	//inline void								GetSize(unsigned int &X, unsigned int &Y) { X = mySizeX; Y = mySizeY; }
	//inline void								SetSize(unsigned int X, unsigned int Y) { mySizeX = X; mySizeY = Y; }
	//inline void								SetDock(kfloat x, kfloat y) { myDock[0] = x; myDock[1] = y; myNeedUpdatePosition = true; PropagateNeedUpdateToFather(); }
	//inline void								GetPrescale(float &X, float &Y) { X = myPreScaleX; Y = myPreScaleY; }
	//inline void								GetPostscale(float &X, float &Y) { X = myPostScaleX; Y = myPostScaleY; }

	Node2D*								getFather() const;
	Abstract2DLayer*					getLayerFather() const;


	//inline void								 GetScaledSize(unsigned int &X, unsigned int &Y) { X = (unsigned int)((kfloat)mySizeX*(kfloat)myPreScaleX); Y = (unsigned int)((kfloat)mySizeY*(kfloat)myPreScaleY); }

	bool										addItem(const CMSP& item, ItemPosition pos = Last DECLARE_DEFAULT_LINK_NAME) override;
	bool										removeItem(const CMSP& item DECLARE_DEFAULT_LINK_NAME) override;

	inline const kstl::set<Node2D*, Node2D::PriorityCompare>&	GetSons() { return mySons; }

	// transform local points to global
	inline void	TransformPoints(Point2D* totransform, int count) const
	{
		myGlobalTransformMatrix.TransformPoints(totransform, count);
	}

	v2f GetPosition() const { return myPosition; }
	void GetPosition(kfloat &X, kfloat &Y) const
	{
		X = (kfloat)myPosition[0];
		Y = (kfloat)myPosition[1];
	}

	void	GetGlobalPosition(kfloat &X, kfloat &Y);

	inline const Matrix3x3& GetGlobalTransform() const
	{
		return myGlobalTransformMatrix;
	}

	void GetTransformedPoints(Point2D * pt);


	virtual void SetUpNodeIfNeeded();
	// recurse to retreive root 2D Layer
	Abstract2DLayer*									getRootLayerFather() const;
	Node2D*												getRootFather();

	u32													GetNodeFlags() const { return myFlags; }

	enum Flags
	{
		Node2D_SizeChanged = 1u << 0u,
		Node2D_Clipped = 1u << 1u,
		Node2D_Hidden = 1u << 2u,

		Node2D_PropagatedFlags = Node2D_Clipped | Node2D_Hidden,
	};

	bool IsHiddenFlag() const { return (myFlags & Node2D_Hidden) != 0; }
	bool IsInClip(v2f pos) const;

protected:
	/**
	* \brief	initialize modifiable
	* \fn 		void InitModifiable() override
	*/
	void	InitModifiable() override;

	void SetParent(CoreModifiable* value);

	void PropagateNodeFlags();


	u32 myFlags = 0;


	void												PropagateNeedUpdateToFather();

	void												ComputeMatrices();
	virtual void										ComputeRealSize();
	void												ResortSons();

	Node2D*												myParent;
	kstl::set<Node2D*, Node2D::PriorityCompare>			mySons;

	v2f													myRealSize{ 0,0 };

	Matrix3x3											myLocalTransformMatrix;
	Matrix3x3											myGlobalTransformMatrix;

	maInt												myPriority;
	maFloat												mySizeX;
	maFloat												mySizeY;
	maVect2DF											myDock;
	maVect2DF											myAnchor;
	maVect2DF											myPosition;
	maFloat												myAngle;
	maFloat												myPreScaleX;
	maFloat												myPreScaleY;
	maFloat												myPostScaleX;
	maFloat												myPostScaleY;
	maEnum<3>											mySizeModeX;
	maEnum<3>											mySizeModeY;
	maBoolHeritage<1>									myClipSons;
	bool												myNeedUpdatePosition;
	bool												mySonPriorityChanged;
	maReference											myCustomShader = BASE_ATTRIBUTE(CustomShader, "");

	WRAP_METHODS(GetSize);
};

#endif //_NODE3D_H_
