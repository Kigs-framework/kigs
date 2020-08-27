#ifndef _SIMPLE_DRAWING_H_
#define _SIMPLE_DRAWING_H_

#include "Drawable.h"

// ****************************************
// * SimpleDrawing class
// * --------------------------------------
/**
* \file	SimpleDrawing.h
* \class	SimpleDrawing
* \ingroup Renderer
* \brief draw simple objects...
* 
* Probably obsolete or same usage as DrawVertice / DirectRenderingMethods.
*
*/
// ****************************************
class SimpleDrawing : public Drawable
{
public:
	DECLARE_ABSTRACT_CLASS_INFO(SimpleDrawing,Drawable,Renderer)

	/**
	 * \brief	constructor
	 * \fn 		SimpleDrawing(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	 * \param	name : instance name
	 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	 */
	DECLARE_CONSTRUCTOR(SimpleDrawing)
	
	/**
	 * \brief	destructor
	 * \fn 		~SimpleDrawing();
	 */
	virtual ~SimpleDrawing();

	/**
	 * \brief	initialise draw method
	 * \fn 		virtual bool Draw(TravState* travstate) = 0;
	 * \param	travstate : camera state
	 * \return	TRUE if a could draw
	 */
	bool Draw(TravState* travstate) override = 0;
	
	/**
	 * \brief	update the bounding box
	 * \fn 		virtual bool	BBoxUpdate(kdouble)
	 * \param	kdouble : world time
	 * \return	TRUE because has a bounding box
	 */
	bool	BBoxUpdate(kdouble/* time */) override {return true;}

	/**
	 * \brief	retreive the bounding box of the bitmap (point min and point max)
	 * \fn 		virtual void	GetBoundingBox(Point3D& pmin,Point3D& pmax) const {pmin=myBBoxMin; pmax=mBBoxMax;}
	 * \param	pmin : point min of the bounding box (in/out param)
	 * \param	pmax : point max of the bounding box (in/out param)
	 */
	void GetNodeBoundingBox(Point3D& pmin,Point3D& pmax) const override;

	/**
	 * \brief	set the color
	 * \fn 		void setColor(kfloat R, kfloat G, kfloat B);
	 * \param	R : red value (0 to 1)
	 * \param	G : green value (0 to 1)
	 * \param	B : blue value (0 to 1)
	 */
	void setColor(kfloat R, kfloat G, kfloat B);
	/**
	 * \brief	set the alpha value
	 * \fn 		void setAlpha(kfloat A);
	 * \param	A : alpha value (0 to 1)
	 */
	void setAlpha(kfloat A);

	/**
	 * \brief	set the color
	 * \fn 		void setColor(kfloat R, kfloat G, kfloat B, kfloat A);
	 * \param	R : red value (0 to 1)
	 * \param	G : green value (0 to 1)
	 * \param	B : blue value (0 to 1)
	 * \param	A : alpha value (0 to 1)
	 */
	void setColor(kfloat R, kfloat G, kfloat B, kfloat A);

	/**
	 * \brief	add a vertex in the vertice list at the specified index
	 * \fn 		void setVertex(int index, Point3D Position);
	 * \param	index : index of the vertex
	 * \param	Position : vertex to add
	 */
	void setVertex(int index, Point3D Position);

	/**
	 * \brief	set the vertice list (cler the old vertice list if not null)
	 * \fn 		void setVertexArray(Point3D *Position, int Count);
	 * \param	Position : new vertice list
	 * \param	Count :  number of the vertice
	 */
	void setVertexArray(Point3D *Position, int Count);

	/**
	 * \brief	set the SimpleDrawing as a rectangle
	 * \fn 		void setToRectangle(const kfloat &w, const kfloat &h, bool bWire, char cNormal = 'X');
	 * \param	w : width of the rectangle
	 * \param	h : height of the rectangle
	 * \param	bWire : draw in wire or plain mode
	 * \param	cNormal : orientation of the rectangle
	 *
	 * type of cNormal :<br>
	 * <ul>
	 * <li>
	 *		<strong>X</strong> : normal : +X axis (1,0,0)
	 * </li>
	 * <li>
	 *		<strong>x</strong> : normal : -X axis (-1,0,0)
	 * </li>
	 * <li>
	 *		<strong>Y</strong> : normal : -Y axis (0,1,0)
	 * </li>
	 * <li>
	 *		<strong>y</strong> : normal : -Y axis (0,-1,0)
	 * </li>
	 * <li>
	 *		<strong>Z</strong> : normal : -Z axis (0,0,1)
	 * </li>
	 * <li>
	 *		<strong>z</strong> : normal : -Z axis (0,0,-1)
	 * </li>
	 * </ul>
	 */
	void setToRectangle(const kfloat &w, const kfloat &h, bool bWire, char cNormal = 'X');

	/**
	 * \brief	set the SimpleDrawing as a box
	 * \fn 		void setToBox(const kfloat &w, const kfloat &h, const kfloat &d, bool bWire);
	 * \param	w : width of the rectangle
	 * \param	h : height of the rectangle
	 * \param	d : depth of the rectangle
	 * \param	bWire : draw in wire or plain mode
	 */
	void setToBox(const kfloat &w, const kfloat &h, const kfloat &d, bool bWire);
	
	/**
	 * \brief	set the SimpleDrawing as a disk (with (0,0,0) as center)
	 * \fn 		void setToDisk(const kfloat &r, const int &precision, bool bWire,  char cNormal = 'X');
	 * \param	r : size of the disk
	 * \param	precision : precision of the disk
	 * \param	bWire : draw in wire or plain mode
	 * \param	cNormal : orientation of the disk
	 *
	 * type of cNormal :<br>
	 * <ul>
	 * <li>
	 *		<strong>X</strong> : normal : +X axis (1,0,0)
	 * </li>
	 * <li>
	 *		<strong>x</strong> : normal : -X axis (-1,0,0)
	 * </li>
	 * <li>
	 *		<strong>Y</strong> : normal : -Y axis (0,1,0)
	 * </li>
	 * <li>
	 *		<strong>y</strong> : normal : -Y axis (0,-1,0)
	 * </li>
	 * <li>
	 *		<strong>Z</strong> : normal : -Z axis (0,0,1)
	 * </li>
	 * <li>
	 *		<strong>z</strong> : normal : -Z axis (0,0,-1)
	 * </li>
	 * </ul>
	 */
	void setToDisk(const kfloat &r, const int &precision, bool bWire,  char cNormal = 'X');
protected:
	/**
	 * \brief	prepare the drawing
	 * \fn 		virtual void PrepareDrawing() = 0;
	 */
	virtual void PrepareDrawing() = 0;

	//! list of vertice
	Point3D*	mVertex = nullptr;
	//! number of vertice
	int			mVertexCount = 0;
	//! drawing mode
	maInt		mMode = BASE_ATTRIBUTE(Mode, 0);
	//! width
	maFloat		mWidth = BASE_ATTRIBUTE(Width, 1);
	//! alpha value
	maFloat		mAlpha = BASE_ATTRIBUTE(Alpha, 1);
	//! color value
	maVect3DF	mColor = BASE_ATTRIBUTE(Color, 1, 1, 1);
};

#endif
