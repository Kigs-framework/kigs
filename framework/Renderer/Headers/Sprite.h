#ifndef _SPRITE_H_
#define _SPRITE_H_

#include "Drawable.h"
#include "TextureHandler.h"

//! default displacement
#define KIGS_SPRITE_DEFAULT_DISPLACEMENT KFLOAT_CONST(0.002f)

class Texture;

// ****************************************
// * Sprite class
// * --------------------------------------
/**
* \file	Sprite.h
* \class	Sprite
* \ingroup Renderer
* \brief Draw a sprite in the 3D scene.
*
* ?? obsolete ?? can be replaced by Billboard 3D or AutoOrientedNode3D or UINode3DLayer ?
*
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
	virtual void setPosition(kfloat x, kfloat y) {mPosition[0]=x; mPosition[1]=y;}

	/**
	 * \brief	set the texture
	 * \fn 		virtual void setTexture(const kfloat &u1,const kfloat &v1,const kfloat &u2,const kfloat &v2) 
	 * \param	u1 : 
	 * \param	v1 : 
	 * \param	u2 : 
	 * \param	v2 : 
	 */
	virtual void setTexture(const kfloat &u1,const kfloat &v1,const kfloat &u2,const kfloat &v2) { mTexUV[0]=u1; mTexUV[2]=u2; mTexUV[1]=v1;; mTexUV[3]=v2;}
	
	/**
	 * \brief	set the size
	 * \fn 		virtual void setSize(const kfloat &s)
	 * \param	s : new size
	 */
	virtual void setSize(const kfloat &s) {mSize[0]=mSize[1]=s;}
	
	/**
	 * \brief	set the size
	 * \fn 		virtual void setSize(const kfloat &sx, const kfloat &sy)
	 * \param	sx : size on x axis
	 * \param	sy : size on y axis
	 */
	virtual void setSize(const kfloat &sx, const kfloat &sy) {mSize[0]=sx;mSize[1]=sy;}
	
	/**
	 * \brief	get the size
	 * \fn 		virtual void getSize(kfloat &sx, kfloat &sy)
	 * \param	sx : size on x axis (in/out param)
	 * \param	sy : size on y axis (in/out param)
	 */
	virtual void getSize(kfloat &sx, kfloat &sy) {sx=mSize[0];sy=mSize[1];}


protected:
	/**
	 * \brief	destructor
	 * \fn 		~Sprite();
	 */
	virtual	~Sprite();


	/**
	 * \brief	retreive the bounding box of the bitmap (point min and point max)
	 * \fn 		virtual void	GetBoundingBox(Point3D& pmin,Point3D& pmax) const {pmin=myBBoxMin; pmax=mBBoxMax;}
	 * \param	pmin : point min of the bounding box (in/out param)
	 * \param	pmax : point max of the bounding box (in/out param)
	 */
	void	GetNodeBoundingBox(Point3D& pmin,Point3D& pmax) const override {pmin.Set((kfloat)mPosition[0],KFLOAT_CONST(0.0f),(kfloat)mPosition[1]); pmax.Set((kfloat)mPosition[0] +(kfloat)mSize[0],(kfloat)mDisplacement,(kfloat)mPosition[1] +(kfloat)mSize[1]);}

	//! file name of the used texture
	//maString mTextureFileName;
	//! displacement
	maFloat mDisplacement;
	//! position
	maVect2DF mPosition;
	//! size 
	maVect2DF mSize;
	//! coord texture u1,v1,u2,v2
	maVect4DF mTexUV;
	//! color
	maVect4DF	mColor;

	//! link to the texture
	//SP<Texture>	mTexture;
	INSERT_FORWARDSP(TextureHandler, mTexturePointer);
};

#endif //_SPRITE_H_
