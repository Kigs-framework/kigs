#ifndef _OPENGLSPHERE_H
#define _OPENGLSPHERE_H

#include "Drawable.h"

class GLUquadric;

class OpenGLSphere : public Drawable
{
public:
    DECLARE_CLASS_INFO(OpenGLSphere,Drawable,Renderer)

    OpenGLSphere(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	bool	Draw(TravState* travstate);
	// return true because has a bounding box
	virtual bool	BBoxUpdate(kdouble/* time */) {return true;}
    
	void	GetBoundingBox(Point3D& pmin,Point3D& pmax) const;

    
protected:
    maFloat m_Radius;
	maVect4DF	myColor;

    GLUquadric *myQuadObj; 
    virtual ~OpenGLSphere();

	unsigned int		myListIndex;

};    

#endif //_OPENGLSPHERE_H
