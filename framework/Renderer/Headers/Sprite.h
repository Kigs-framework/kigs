#ifndef _SPRITE_H_
#define _SPRITE_H_

#include "Drawable.h"

//! default displacement
#define KIGS_SPRITE_DEFAULT_DISPLACEMENT KFLOAT_CONST(0.002f)

class Texture;

// ****************************************
// * Sprite class
// * --------------------------------------
/**
 * \file	Sprite.h
 * \class	Sprite
 * \ingroup Drawable
 * \ingroup RendererDrawable
 * \brief	Sprite object
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
 * CoreModifiable* SpriteObject=(CoreModifiable*)(KigsCore::GetInstanceOf(<i>instance_name</i>,"Sprite"));<br>
 * </span>
 * <span class="comment"> Initialization :</span><br>
 * <span class="code">
 * SpriteObject->setValue("TextureFileName",<i>file_name.tga</i>);<br>
 * SpriteObject->Init();<br>
 * </span>
 * </dd></dl>
 *
 * <dl class="exported"><dt><b>Exported parameters :</b></dt><dd>
 * <table>
 * <tr><td>string</td><td><strong>TextureFileName</strong> :</td>	<td>Name of the texture file</td></tr>	
 * <tr><td>kfloat</td><td><strong>Displacement</strong> :</td>		<td>displacement</td></tr>
 * <tr><td>kfloat</td><td><strong>PosX</strong> :</td>				<td>position on x axis</td></tr>
 * <tr><td>kfloat</td><td><strong>PosY</strong> :</td>				<td>position on y axis</td></tr>
 * <tr><td>kfloat</td><td><strong>SizeX</strong> :</td>				<td>size on x axis</td></tr>
 * <tr><td>kfloat</td><td><strong>SizeY</strong> :</td>				<td>size on y axis</td></tr>
 * <tr><td>kfloat</td><td><strong>Texu1</strong> :</td>				<td>coord texture u</td></tr>
 * <tr><td>kfloat</td><td><strong>Texu2</strong> :</td>				<td>coord texture u</td></tr>
 * <tr><td>kfloat</td><td><strong>Texv1</strong> :</td>				<td>coord texture v</td></tr>
 * <tr><td>kfloat</td><td><strong>Texv2</strong> :</td>				<td>coord texture v</td></tr>
 * <tr><td>kfloat</td><td><strong>Color[4]</strong> :</td>			<td>color</td></tr>		
 * </table>
 * </dd></dl>
 */
// ****************************************
class Sprite : public Drawable
{
public:
	DECLARE_ABSTRACT_CLASS_INFO(Sprite,Drawable,Renderer)
	
	/**
	 * \brief	constructor
	 * \fn 		Sprite(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	 * \param	name : instance name
	 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	 */
	Sprite(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	
	/**
	 * \brief	set the position
	 * \fn 		virtual void setPosition(kfloat x, kfloat y)
	 * \param	x : position on x axis
	 * \param	y : position on y axis
	 */
	virtual void setPosition(kfloat x, kfloat y) {m_PosX=x;m_PosY=y;}

	/**
	 * \brief	set the texture
	 * \fn 		virtual void setTexture(const kfloat &u1,const kfloat &v1,const kfloat &u2,const kfloat &v2) 
	 * \param	u1 : 
	 * \param	v1 : 
	 * \param	u2 : 
	 * \param	v2 : 
	 */
	virtual void setTexture(const kfloat &u1,const kfloat &v1,const kfloat &u2,const kfloat &v2) {m_Texu1=u1;m_Texu2=u2;m_Texv1=v1;;m_Texv2=v2;}
	
	/**
	 * \brief	set the size
	 * \fn 		virtual void setSize(const kfloat &s)
	 * \param	s : new size
	 */
	virtual void setSize(const kfloat &s) {m_SizeX=m_SizeY=s;}
	
	/**
	 * \brief	set the size
	 * \fn 		virtual void setSize(const kfloat &sx, const kfloat &sy)
	 * \param	sx : size on x axis
	 * \param	sy : size on y axis
	 */
	virtual void setSize(const kfloat &sx, const kfloat &sy) {m_SizeX=sx;m_SizeY=sy;}
	
	/**
	 * \brief	get the size
	 * \fn 		virtual void getSize(kfloat &sx, kfloat &sy)
	 * \param	sx : size on x axis (in/out param)
	 * \param	sy : size on y axis (in/out param)
	 */
	virtual void getSize(kfloat &sx, kfloat &sy) {sx=m_SizeX;sy=m_SizeY;}
	
	/**
	 * \brief	change the texture
	 * \fn 		virtual void changeTexture(kstl::string FileName);
	 * \param	FileName : file name of the texture
	 */
	virtual void changeTexture(kstl::string FileName);

protected:
	/**
	 * \brief	destructor
	 * \fn 		~Sprite();
	 */
	virtual	~Sprite();

	/**
	 * \brief	initialize modifiable
	 * \fn 		virtual void InitModifiable();
	 */
	void InitModifiable() override;

	/**
	 * \brief	retreive the bounding box of the bitmap (point min and point max)
	 * \fn 		virtual void	GetBoundingBox(Point3D& pmin,Point3D& pmax) const {pmin=myBBoxMin; pmax=myBBoxMax;}
	 * \param	pmin : point min of the bounding box (in/out param)
	 * \param	pmax : point max of the bounding box (in/out param)
	 */
	void	GetNodeBoundingBox(Point3D& pmin,Point3D& pmax) const override {pmin.Set((kfloat)m_PosX,KFLOAT_CONST(0.0f),(kfloat)m_PosY); pmax.Set((kfloat)m_PosX+(kfloat)m_SizeX,(kfloat)m_Displacement,(kfloat)m_PosY+(kfloat)m_SizeY);}

	//! file name of the used texture
	maString m_TextureFileName;
	//! displacement
	maFloat m_Displacement;
	//! position on x axis
	maFloat m_PosX;
	//! position on y axis
	maFloat m_PosY;
	//! size on x axis
	maFloat m_SizeX;
	//! size on y axis
	maFloat m_SizeY;
	//! coord texture u
	maFloat m_Texu1;
	//! coord texture u
	maFloat m_Texu2;
	//! coord texture v
	maFloat m_Texv1;
	//! coord texture v
	maFloat m_Texv2;
	//! color
	maVect4DF	m_Color;

	//! link to the texture
	Texture*	myTexture;
};

#endif //_SPRITE_H_
