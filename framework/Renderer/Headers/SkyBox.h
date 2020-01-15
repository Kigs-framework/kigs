#ifndef _SKYBOX_H_
#define _SKYBOX_H_

#include "Drawable.h"
#include "Texture.h"

// ****************************************
// * SkyBox class
// * --------------------------------------
/**
 * \file	SkyBox.h
 * \class	SkyBox
 * \ingroup Drawable
 * \ingroup RendererDrawable
 * \brief	SkyBox object
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
 * CoreModifiable* SkyBoxObject=(CoreModifiable*)(KigsCore::GetInstanceOf(<i>instance_name</i>,"SkyBox"));<br>
 * </span>
 * <span class="comment"> Initialization :</span><br>
 * <span class="code">
 * SkyBoxObject->setValue("FileName",<i>file_name*</i>);<br>
 * SkyBoxObject->Init();<br>
 * </span>
 *
 * <span class="comment"> * : the file name has to be 'basename'.tga which refer to basename_x.tga<br>
 * the texture base name is used to find all the six textures<br>
 * by adding _# where # is 1 to 6<br>
 * 1 = XMAX = LEFT ?<br>
 * 2 = XMIN = RIGHT ?<br>
 * 3 = YMAX = FRONT ?<br>
 * 4 = YMIN = BACK ?<br>
 * 5 = ZMAX	= TOP<br>
 * 6 = ZMIN = BOTTOM<br>
 * </span>
 * </dd></dl>
 *
 * <dl class="exported"><dt><b>Exported parameters :</b></dt><dd>
 * <table>
 * <tr><td>kfloat</td><td><strong>SkyBox Size</strong> :</td><td>size of the SkyBox</td></tr>	
 * <tr><td>string</td><td><strong>FileName</strong> :</td><td>Name of the used texture file</td></tr>	
 * </table>
 * </dd></dl>
 */
// ****************************************
class SkyBox : public Drawable
{
public:
	DECLARE_ABSTRACT_CLASS_INFO(SkyBox,Drawable,Renderer)
	
	/**
	 * \brief	constructor
	 * \fn 		SkyBox(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	 * \param	name : instance name
	 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	 */
	SkyBox(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	
	/**
	 * \brief	destructor
	 * \fn 		~SkyBox();
	 */
	virtual ~SkyBox();

	/**
	 * \brief	retreive the bounding box of the bitmap (point min and point max)
	 * \fn 		virtual void	GetBoundingBox(Point3D& pmin,Point3D& pmax) const {pmin=myBBoxMin; pmax=myBBoxMax;}
	 * \param	pmin : point min of the bounding box (in/out param)
	 * \param	pmax : point max of the bounding box (in/out param)
	 */
	void	GetNodeBoundingBox(Point3D& pmin,Point3D& pmax) const  override {pmin=myBBoxMin; pmax=myBBoxMax;}

	/*void SetTextures(const kstl::string& ZMax,
					 const kstl::string& Xmax, 
					 const kstl::string& ZMin, 
					 const kstl::string& XMin, 
					 const kstl::string& Top, 
					 const kstl::string& Bottom);*/

	/**
	 * \brief	set the size of the SkyBox
	 * \fn 		void SetSize(const kfloat& Size)
	 * \param	Size : size of the SkyBox
	 */
	void SetSize(const kfloat& Size) {m_Size.setValue(Size);}

	/**
	 * \brief	update the bounding box
	 * \fn 		virtual bool	BBoxUpdate(kdouble)
	 * \param	kdouble : world time
	 * \return	TRUE because has a bounding box
	 */
	bool	BBoxUpdate(kdouble/* time */) override {return true;}

protected:
	/**
	 * \brief	initialize modifiable
	 * \fn 		virtual void InitModifiable();
	 */
    void InitModifiable() override;

	//! used texture
	SP<Texture>	myTexture;
	//! size of the SkyBox
	maFloat		m_Size;
	//! name of the file
	maString	myFileName;
	
	//! point min of the bounding box
	Point3D		myBBoxMin;
	//! point max of the bounding box
	Point3D		myBBoxMax;
};

#endif
