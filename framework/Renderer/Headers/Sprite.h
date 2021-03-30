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
	virtual void setPosition(kfloat x, kfloat y) {mPosX=x;mPosY=y;}

	/**
	 * \brief	set the texture
	 * \fn 		virtual void setTexture(const kfloat &u1,const kfloat &v1,const kfloat &u2,const kfloat &v2) 
	 * \param	u1 : 
	 * \param	v1 : 
	 * \param	u2 : 
	 * \param	v2 : 
	 */
	virtual void setTexture(const kfloat &u1,const kfloat &v1,const kfloat &u2,const kfloat &v2) {mTexu1=u1;mTexu2=u2;mTexv1=v1;;mTexv2=v2;}
	
	/**
	 * \brief	set the size
	 * \fn 		virtual void setSize(const kfloat &s)
	 * \param	s : new size
	 */
	virtual void setSize(const kfloat &s) {mSizeX=mSizeY=s;}
	
	/**
	 * \brief	set the size
	 * \fn 		virtual void setSize(const kfloat &sx, const kfloat &sy)
	 * \param	sx : size on x axis
	 * \param	sy : size on y axis
	 */
	virtual void setSize(const kfloat &sx, const kfloat &sy) {mSizeX=sx;mSizeY=sy;}
	
	/**
	 * \brief	get the size
	 * \fn 		virtual void getSize(kfloat &sx, kfloat &sy)
	 * \param	sx : size on x axis (in/out param)
	 * \param	sy : size on y axis (in/out param)
	 */
	virtual void getSize(kfloat &sx, kfloat &sy) {sx=mSizeX;sy=mSizeY;}


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
	void	GetNodeBoundingBox(Point3D& pmin,Point3D& pmax) const override {pmin.Set((kfloat)mPosX,KFLOAT_CONST(0.0f),(kfloat)mPosY); pmax.Set((kfloat)mPosX+(kfloat)mSizeX,(kfloat)mDisplacement,(kfloat)mPosY+(kfloat)mSizeY);}

	//! file name of the used texture
	//maString mTextureFileName;
	//! displacement
	maFloat mDisplacement;
	//! position on x axis
	maFloat mPosX;
	//! position on y axis
	maFloat mPosY;
	//! size on x axis
	maFloat mSizeX;
	//! size on y axis
	maFloat mSizeY;
	//! coord texture u
	maFloat mTexu1;
	//! coord texture u
	maFloat mTexu2;
	//! coord texture v
	maFloat mTexv1;
	//! coord texture v
	maFloat mTexv2;
	//! color
	maVect4DF	mColor;

	//! link to the texture
	//SP<Texture>	mTexture;
	INSERT_FORWARDSP(TextureHandler, mTexturePointer);
};

#endif //_SPRITE_H_
