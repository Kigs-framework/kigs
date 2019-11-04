#ifndef _DIRECTRENDERINGMETHODS_H_
#define _DIRECTRENDERINGMETHODS_H_

#include "CoreModifiable.h"
#include "TecLibs/Tec3D.h"

class BBox;
class TravState;
class ModuleSpecificRenderer;

// ****************************************
// * DirectRenderingMethods class
// * --------------------------------------
/**
 * \file	DirectRenderingMethods.h
 * \class	DirectRenderingMethods
 * \ingroup Renderer
 * \brief	direct rendering methods
 * \author	ukn
 * \version ukn
 * \date	ukn
 */
// ****************************************
class DirectRenderingMethods : public CoreModifiable
{
public:
    DECLARE_ABSTRACT_CLASS_INFO(DirectRenderingMethods,CoreModifiable,Renderer)    
    
	/**
	 * \brief	constructor
	 * \fn 		DirectRenderingMethods(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	 * \param	name : instance name
	 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	 */
    DirectRenderingMethods(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	/**
	 * \brief	Draw 3D bounding box in wireframe
	 * \fn 		virtual void DrawBoundingBox(BBox* bbox,kfloat R=KFLOAT_CONST(1.0f),kfloat G=KFLOAT_CONST(0.0f),kfloat B=KFLOAT_CONST(.0f)) const =0;
	 * \param	bbox : 3D bounding box to draw
	 * \param	R : red color
	 * \param	G : green color
	 * \param	B : blue color
	 */
    virtual void DrawBoundingBox(TravState* state,BBox* bbox,kfloat R=KFLOAT_CONST(1.0f),kfloat G=KFLOAT_CONST(0.0f),kfloat B=KFLOAT_CONST(.0f)) const =0;
	
	/**
	 * \brief	Draw quad with the currently bound texture. Vertex order  : winded
	 * \fn 		virtual void DrawTexturedQuad(Point3D Vertices[4], Point3D UV[4] ,kfloat R,kfloat G,kfloat B) const = 0;
	 * \param	Vertices : vertice position
	 * \param	UV : texture position
	 * \param	R : red color
	 * \param	G : green color
	 * \param	B : blue color
	 */
	virtual void DrawTexturedQuad(TravState* state,Point3D Vertices[4], Point3D UV[4] ,kfloat R,kfloat G,kfloat B) const = 0;

	/**
	 * \brief	Draw non-textured quad. Vertex order  : winded
	 * \fn 		virtual void DrawSolidQuad(Point3D Vertices[4], kfloat R,kfloat G,kfloat B, bool EnableBlend=false) const = 0;
	 * \param	Vertices : vertice position
	 * \param	R : red color
	 * \param	G : green color
	 * \param	B : blue color
	 * \param	EnableBlend : should blend
	 */
	virtual void DrawSolidQuad(TravState* state,Point3D Vertices[4], kfloat R,kfloat G,kfloat B, bool EnableBlend=false) const = 0;

	/**
	 * \brief	Draw a rounded rectangle
	 * \fn 		virtual void DrawRoundedRectangle(Point3D Vertices[2], kfloat R,kfloat G,kfloat B,kfloat RBorder,kfloat GBorder,kfloat BBorder,kfloat Radius) const =0;
	 * \param	Vertices : vertice position
	 * \param	R : red color
	 * \param	G : green color
	 * \param	B : blue color
	 * \param	RBorder : red color of the border
	 * \param	GBorder : green color of the border
	 * \param	BBorder : blue color of the border
	 * \param	Radius : corner radius
	 */
	virtual void DrawRoundedRectangle(TravState* state,Point3D Vertices[2], kfloat R,kfloat G,kfloat B,kfloat RBorder,kfloat GBorder,kfloat BBorder,kfloat Radius) const =0;

	virtual void drawSphere(ModuleSpecificRenderer* state, kfloat r, int lats, int longs, Point3D* pos, kfloat R, kfloat G, kfloat B) const {} 
	
	virtual void DrawLine(ModuleSpecificRenderer* state, Point3D * Vertices, int VerticeCount, kfloat R, kfloat G, kfloat B) const {} 

protected:
	/**
	 * \brief	destructor
	 * \fn 		~DirectRenderingMethods();
	 */
	virtual ~DirectRenderingMethods();  
}; 

#endif //_DIRECTRENDERINGMETHODS_H_
