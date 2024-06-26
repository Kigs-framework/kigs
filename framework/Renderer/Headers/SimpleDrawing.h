#pragma once

#include "Drawable.h"

namespace Kigs
{

	namespace Draw
	{
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
			DECLARE_ABSTRACT_CLASS_INFO(SimpleDrawing, Drawable, Renderer)

				/**
				 * \brief	constructor
				 * \fn 		SimpleDrawing(const std::string& name,DECLARE_CLASS_NAME_TREE_ARG);
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
			 * \fn 		virtual bool	BBoxUpdate(double)
			 * \param	double : world time
			 * \return	TRUE because has a bounding box
			 */
			bool	BBoxUpdate(double/* time */) override { return true; }

			/**
			 * \brief	retreive the bounding box of the bitmap (point min and point max)
			 * \fn 		virtual void	GetBoundingBox(v3f& pmin,v3f& pmax) const {pmin=myBBoxMin; pmax=mBBoxMax;}
			 * \param	pmin : point min of the bounding box (in/out param)
			 * \param	pmax : point max of the bounding box (in/out param)
			 */
			void GetNodeBoundingBox(v3f& pmin, v3f& pmax) const override;

			/**
			 * \brief	set the color
			 * \fn 		void setColor(float R, float G, float B);
			 * \param	R : red value (0 to 1)
			 * \param	G : green value (0 to 1)
			 * \param	B : blue value (0 to 1)
			 */
			void setColor(float R, float G, float B);
			/**
			 * \brief	set the alpha value
			 * \fn 		void setAlpha(float A);
			 * \param	A : alpha value (0 to 1)
			 */
			void setAlpha(float A);

			/**
			 * \brief	set the color
			 * \fn 		void setColor(float R, float G, float B, float A);
			 * \param	R : red value (0 to 1)
			 * \param	G : green value (0 to 1)
			 * \param	B : blue value (0 to 1)
			 * \param	A : alpha value (0 to 1)
			 */
			void setColor(float R, float G, float B, float A);

			/**
			 * \brief	add a vertex in the vertice list at the specified index
			 * \fn 		void setVertex(int index, v3f Position);
			 * \param	index : index of the vertex
			 * \param	Position : vertex to add
			 */
			void setVertex(int index, v3f Position);

			/**
			 * \brief	set the vertice list (cler the old vertice list if not null)
			 * \fn 		void setVertexArray(v3f *Position, int Count);
			 * \param	Position : new vertice list
			 * \param	Count :  number of the vertice
			 */
			void setVertexArray(v3f* Position, int Count);

			/**
			 * \brief	set the SimpleDrawing as a rectangle
			 * \fn 		void setToRectangle(const float &w, const float &h, bool bWire, char cNormal = 'X');
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
			void setToRectangle(const float& w, const float& h, bool bWire, char cNormal = 'X');

			/**
			 * \brief	set the SimpleDrawing as a box
			 * \fn 		void setToBox(const float &w, const float &h, const float &d, bool bWire);
			 * \param	w : width of the rectangle
			 * \param	h : height of the rectangle
			 * \param	d : depth of the rectangle
			 * \param	bWire : draw in wire or plain mode
			 */
			void setToBox(const float& w, const float& h, const float& d, bool bWire);

			/**
			 * \brief	set the SimpleDrawing as a disk (with (0,0,0) as center)
			 * \fn 		void setToDisk(const float &r, const int &precision, bool bWire,  char cNormal = 'X');
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
			void setToDisk(const float& r, const int& precision, bool bWire, char cNormal = 'X');
		protected:
			/**
			 * \brief	prepare the drawing
			 * \fn 		virtual void PrepareDrawing() = 0;
			 */
			virtual void PrepareDrawing() = 0;

			//! list of vertice
			v3f*	mVertex = nullptr;
			//! number of vertice
			int			mVertexCount = 0;
			//! drawing mode
			s32			mMode = 0;
			//! width
			float		mWidth = 1.0f;
			//! alpha value
			float		mAlpha = 1.0f;
			//! color value
			v3f			mColor = { 1.0f, 1.0f, 1.0f };

			WRAP_ATTRIBUTES(mMode, mWidth, mAlpha, mColor);
		};

	}
}