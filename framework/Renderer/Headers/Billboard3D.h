#ifndef _BILLBOARD_3D_H_
#define _BILLBOARD_3D_H_

#include "TecLibs/Tec3D.h"
#include "Drawable.h"
#include "Texture.h"
#include "Camera.h"
#include "SmartPointer.h"
#include "TextureHandler.h"

// ****************************************
// * Billboard3D class
// * --------------------------------------
/**
 * \file	Billboard3D.h
 * \class	Billboard3D
 * \ingroup Renderer
 * \brief	Billboard ( front facing camera texture ) object.
 *
 * Obsolete
 */
 // ****************************************
class Billboard3D : public Drawable
{
public:
	DECLARE_ABSTRACT_CLASS_INFO(Billboard3D,Drawable,Renderer)

	/**
	 * \brief	constructor
	 * \fn 		Billboard3D(const std::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	 * \param	name : instance name
	 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	 */
	Billboard3D(const std::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	
	/**
	 * \brief	destructor
	 * \fn 		~Billboard3D();
	 */
	virtual ~Billboard3D();

	/**
	 * \brief	add a texture
	 * \fn 		void SetTexture(Texture *TexPointer);
	 * \param	TexPointer : new texture
	 */
	void SetTexture(Texture *TexPointer);

	/**
	 * \brief	set the position
	 * \fn 		void SetPosition(const Point3D &Position);
	 * \param	Position : new position
	 */
	void SetPosition(const Point3D &Position);

	/**
	 * \brief	set the size
	 * \fn 		void SetSize(const float &Width, const float &Height);
	 * \param	Width : new width
	 * \param	Height : new height
	 */
	void SetSize(const float &Width, const float &Height);

	/**
	 * \brief	set the texture position
	 * \fn 		void SetTexCoord(const float &u1, const float &v1, const float &u2, const float &v2);
	 * \param	u1 : 
	 * \param	v1 : 
	 * \param	u2 : 
	 * \param	v2 : 
	 */
	void SetTexCoord(const float &u1, const float &v1, const float &u2, const float &v2);

	/**
	 * \brief	set the position and the size
	 * \fn 		void SetPositionAndSize(const Point3D &Position, const float &Width, const float &Height);
	 * \param	Position : new position
	 * \param	Width : new width
	 * \param	Height : new height
	 */
	void SetPositionAndSize(const Point3D &Position, const float &Width, const float &Height);

	/**
	 * \brief	set the color
	 * \fn 		void SetColor(const float &R, const float &G, const float &B, const float &A);
	 * \param	R : red value (0 to 1)
	 * \param	G : green value (0 to 1)
	 * \param	B : blue value (0 to 1)
	 * \param	A : alpha value (0 to 1)
	 */
	void SetColor(const float &R, const float &G, const float &B, const float &A);

	/**
	 * \brief	set the alpha value
	 * \fn 		void SetAlpha(const float &A);
	 * \param	A : alpha value (0 to 1)
	 */
	void SetAlpha(const float &A);

	/**
	 * \brief	set the father node
	 * \fn 		SetFatherNode(Node3D *pNode)
	 * \param	pNode : link to the father
	 */
	void SetFatherNode(Node3D *pNode) {mFatherNode=pNode;}

	/**
	 * \brief	getter of the size (width and height)
	 * \fn 		void GetSize(float &w, float &h)
	 * \param	w : receive the width
	 * \param	h : receive the height
	 */
	void GetSize(float &w, float &h) {w=mWidth;h=mHeight;}

	/**
	 * \brief	getter of the position
	 * \fn 		const Point3D& GetPosition() 
	 * \return	the position
	 */
	const Point3D GetPosition() {return Point3D(mPosition[0],mPosition[1],mPosition[2]);}
	
	/**
	 * \brief	getter of the father node
	 * \fn 		Node3D *GetFatherNode()
	 * \return	the father node
	 */
	Node3D *GetFatherNode() {return mFatherNode;}

	inline void	SetCamera(Camera* a_value){mCamera = a_value;}

	
protected:

	/**
	 * \brief	prepare the vertex for the drawing
	 * \fn 		virtual void PrepareVertexBufferPos() = 0;	
	 */
	virtual void PrepareVertexBufferPos() = 0;

	/**
	 * \brief	prepare the vertex of the texture for the drawing
	 * \fn 		virtual void PrepareVertexBufferTex() = 0;
	 */
	virtual void PrepareVertexBufferTex() = 0;

	/**
	 * \brief	prepare the vertex of the color for the drawing
	 * \fn 		virtual void PrepareVertexBufferCol() = 0;
	 */
	virtual void PrepareVertexBufferCol() = 0;

protected:
	/**
	 * \brief	update the orientation far a specific camera
	 * \fn 		void UpdateOrientation(Camera *pCam);
	 * \param	pCam : link to the camera
	 */
	void UpdateOrientation();

	/**
	 * \brief	initialise draw method
	 * \fn 		virtual bool Draw(TravState*);
	 * \param	TravState : camera state
	 * \return	TRUE if a could draw
	 */
	bool Draw(TravState*) override;

	//! file name of the texture
	//maString mTextureFileName;
	//! Pointer to a Texture... If ==NULL, Billboard should belong to a Billboard Group
	//SP<Texture> mTex;
	INSERT_FORWARDSP(TextureHandler, mTexturePointer);
	//! Center of BillBoard
	maVect3DF mPosition;
	//! Size = Width,Height
	maFloat mWidth;
	//! Size = Width,Height
	maFloat mHeight;
	//!Texture coordinate 
	float mU1;
	//!Texture coordinate 
	float mV1;
	//!Texture coordinate 
	float mU2;
	//!Texture coordinate 
	float mV2;
	//! Color
	float mColor[4];
	//! Used To make the billboard Facing the Camera... Updated with UpdateOrientation
	Vector3D mHorizontalVector;
	//! Used To make the billboard Facing the Camera... Updated with UpdateOrientation
	Vector3D mVerticalVector;
	//! Used When Billboard Belongs to a BillboardGroup
	float mDistanceToCamera;
	//! link to the father node
	Node3D *mFatherNode;
	//! reference to Camera
	Camera*			mCamera;
	//! BillBoard Anchor point
	maVect2DF			mAnchor;

	//! item is enable?
	maBool		mIsEnabled;

	//! ratio between scene unit and pixel unit
	maFloat				mRatio;

};

#endif