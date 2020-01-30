#ifndef _BILLBOARD_3D_H_
#define _BILLBOARD_3D_H_

#include "TecLibs/Tec3D.h"
#include "Drawable.h"
#include "Texture.h"
#include "Camera.h"
#include "SmartPointer.h"
#include "SpriteSheetTexture.h"

// ****************************************
// * Billboard3D class
// * --------------------------------------
/**
 * \file	Billboard3D.h
 * \class	Billboard3D
 * \ingroup RendererDrawable
 * \ingroup Drawable
 * \brief	Billoard3D object
 * \author	ukn
 * \version ukn
 * \date	ukn
 * 
 * <dl class="dependency"><dt><b>Dependency:</b></dt><dd>ModuleRenderer</dd></dl>
 * <dl class="exemple"><dt><b>Exemple:</b></dt><dd>
 * <span class="comment"> Manage texture file : </span><br>
 * <span class="code">
 * theFileManager=new ModuleFileManager(<i>instance_name</i>);<br>
 * theFileManager->Init(KigsCore::Instance(),0);<br>
 * theFileManager->AddToPath(<i>directory_path</i>,"tga"); <span class="comment"> //manage .tga file </span><br>
 * </span>
 * <span class="comment"> Load the module :</span><br>
 * <span class="code">
 * theRenderer=new ModuleRenderer(<i>instance_name</i>);<br>
 * theRenderer->Init(KigsCore::Instance(),0);<br>
 * </span>
 * <span class="comment"> Create the object :</span><br>
 * <span class="code">
 * CoreModifiable* Billboard3DObject=(CoreModifiable*)(KigsCore::GetInstanceOf(<i>instance_name</i>,"Billboard3D"));<br>
 * </span>
 * <span class="comment"> Initialization :</span><br>
 * <span class="code">
 * Billboard3DObject->setValue(LABEL_TO_ID(TextureFileName),<i>texture_file_name.tga</i>);<br>
 * Billboard3DObject->Init();<br>
 * </span>
 * </dd></dl>
 *
 * <dl class="exported"><dt><b>Exported parameters :</b></dt><dd>
 * <table>
 * <tr><td>string</td><td><strong>TextureFileName</strong> :</td><td>File name of the texture</td></tr>	
 * </table>
 * </dd></dl>
 */


// ****************************************
class Billboard3D : public Drawable
{
public:
	DECLARE_ABSTRACT_CLASS_INFO(Billboard3D,Drawable,Renderer)

	/**
	 * \brief	constructor
	 * \fn 		Billboard3D(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	 * \param	name : instance name
	 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	 */
	Billboard3D(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	
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
	 * \fn 		void SetSize(const kfloat &Width, const kfloat &Height);
	 * \param	Width : new width
	 * \param	Height : new height
	 */
	void SetSize(const kfloat &Width, const kfloat &Height);

	/**
	 * \brief	set the texture position
	 * \fn 		void SetTexCoord(const kfloat &u1, const kfloat &v1, const kfloat &u2, const kfloat &v2);
	 * \param	u1 : 
	 * \param	v1 : 
	 * \param	u2 : 
	 * \param	v2 : 
	 */
	void SetTexCoord(const kfloat &u1, const kfloat &v1, const kfloat &u2, const kfloat &v2);

	/**
	 * \brief	set the position and the size
	 * \fn 		void SetPositionAndSize(const Point3D &Position, const kfloat &Width, const kfloat &Height);
	 * \param	Position : new position
	 * \param	Width : new width
	 * \param	Height : new height
	 */
	void SetPositionAndSize(const Point3D &Position, const kfloat &Width, const kfloat &Height);

	/**
	 * \brief	set the color
	 * \fn 		void SetColor(const kfloat &R, const kfloat &G, const kfloat &B, const kfloat &A);
	 * \param	R : red value (0 to 1)
	 * \param	G : green value (0 to 1)
	 * \param	B : blue value (0 to 1)
	 * \param	A : alpha value (0 to 1)
	 */
	void SetColor(const kfloat &R, const kfloat &G, const kfloat &B, const kfloat &A);

	/**
	 * \brief	set the alpha value
	 * \fn 		void SetAlpha(const kfloat &A);
	 * \param	A : alpha value (0 to 1)
	 */
	void SetAlpha(const kfloat &A);

	/**
	 * \brief	set the father node
	 * \fn 		SetFatherNode(Node3D *pNode)
	 * \param	pNode : link to the father
	 */
	void SetFatherNode(Node3D *pNode) {pFatherNode=pNode;}

	/**
	 * \brief	getter of the size (width and height)
	 * \fn 		void GetSize(kfloat &w, kfloat &h)
	 * \param	w : receive the width
	 * \param	h : receive the height
	 */
	void GetSize(kfloat &w, kfloat &h) {w=m_Width;h=m_Height;}

	/**
	 * \brief	getter of the position
	 * \fn 		const Point3D& GetPosition() 
	 * \return	the position
	 */
	const Point3D GetPosition() {return Point3D(m_Position[0],m_Position[1],m_Position[2]);}
	
	/**
	 * \brief	getter of the father node
	 * \fn 		Node3D *GetFatherNode()
	 * \return	the father node
	 */
	Node3D *GetFatherNode() {return pFatherNode;}

	inline void	SetCamera(Camera* a_value){myCamera = a_value;}
	inline bool	Get_AnimationFinished() const {return m_bAnimationFinished;}

	void Update(const Timer& _timer, void* addParam) override;
protected:
	/**
	 * \brief	initialize modifiable
	 * \fn 		virtual void InitModifiable();
	 */
	void InitModifiable() override;

	/**
	* \brief	Change sprite sheet animation
	* \fn 		void	ChangeAnimation(const char* _newAnimation)
	* \param	_newAnimation : New animation
	*/
	void	ChangeAnimation(const kstl::string& _newAnimation);

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

	void NotifyUpdate(const unsigned int labelid) override;

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
	maString m_TextureFileName;
	//! Pointer to a Texture... If ==NULL, Billboard should belong to a Billboard Group
	SP<Texture> m_Tex;
	//! Center of BillBoard
	maVect3DF m_Position;
	//! Size = Width,Height
	maFloat m_Width;
	//! Size = Width,Height
	maFloat m_Height;
	//!Texture coordinate 
	kfloat m_u1;
	//!Texture coordinate 
	kfloat m_v1;
	//!Texture coordinate 
	kfloat m_u2;
	//!Texture coordinate 
	kfloat m_v2;
	//! Color
	kfloat m_Color[4];
	//! Used To make the billboard Facing the Camera... Updated with UpdateOrientation
	Vector3D m_HorizontalVector;
	//! Used To make the billboard Facing the Camera... Updated with UpdateOrientation
	Vector3D m_VerticalVector;
	//! Used When Billboard Belongs to a BillboardGroup
	kfloat m_DistanceToCamera;
	//! link to the father node
	Node3D *pFatherNode;
	//! reference to Camera
	Camera*			myCamera;
	//! BillBoard Anchor point
	maVect2DF			myAnchor;

	//----------------------------------SPRITE SHEET TEXTURE-----------------------------------//
	//! Animation name
	maString	m_CurrentAnimation;
	//! Frame per second playing per the spritesheet texture
	maUInt		m_FramePerSecond;
	//! item is enable?
	maBool		m_IsEnabled;
	//! this item use a spritesheet texture
	maBool		m_IsSpriteSheet;
	//! ratio between scene unit and pixel unit
	maFloat				m_ratio;

	//! Spritesheet texture
	SP<SpriteSheetTexture>	myTexture;
	int					m_CurrentFrame;
	float				m_AnimationSpeed;
	double				m_dElpasedTime;
	unsigned int		m_FrameNumber;
	maBool				m_bLoopAnimation;
	maBool				m_bReverse;
	bool				m_bAnimationFinished;
};

#endif