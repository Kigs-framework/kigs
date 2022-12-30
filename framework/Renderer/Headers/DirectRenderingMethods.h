#pragma once

#include "CoreModifiable.h"
#include "TecLibs/Tec3D.h"


namespace Kigs
{
	namespace Maths
	{
		class BBox;
	}
	namespace Scene
	{
		class TravState;
	}
	namespace Draw
	{
		using namespace Kigs::Core;
		class ModuleSpecificRenderer;

		// ****************************************
		// * DirectRenderingMethods class
		// * --------------------------------------
		/**
		 * \file	DirectRenderingMethods.h
		 * \class	DirectRenderingMethods
		 * \ingroup Renderer
		 * \brief	Base class to draw specific objects directly (in the current rendering context)
		 */
		 // ****************************************
		class DirectRenderingMethods : public CoreModifiable
		{
		public:
			DECLARE_ABSTRACT_CLASS_INFO(DirectRenderingMethods, CoreModifiable, Renderer)

				/**
				 * \brief	constructor
				 * \fn 		DirectRenderingMethods(const std::string& name,DECLARE_CLASS_NAME_TREE_ARG);
				 * \param	name : instance name
				 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
				 */
				DirectRenderingMethods(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);

			/**
			 * \brief	Draw 3D bounding box in wireframe
			 * \fn 		virtual void DrawBoundingBox(BBox* bbox,float R=1.0f,float G=0.0f,float B=0.0f) const =0;
			 * \param	bbox : 3D bounding box to draw
			 * \param	R : red color
			 * \param	G : green color
			 * \param	B : blue color
			 */
			virtual void DrawBoundingBox(Scene::TravState* state, Maths::BBox* bbox, float R = 1.0f, float G = 0.0f, float B = 0.0f) const = 0;

			/**
			 * \brief	Draw quad with the currently bound texture. Vertex order  : winded
			 * \fn 		virtual void DrawTexturedQuad(Point3D Vertices[4], Point3D UV[4] ,float R,float G,float B) const = 0;
			 * \param	Vertices : vertice position
			 * \param	UV : texture position
			 * \param	R : red color
			 * \param	G : green color
			 * \param	B : blue color
			 */
			virtual void DrawTexturedQuad(Scene::TravState* state, Point3D Vertices[4], Point3D UV[4], float R, float G, float B) const = 0;

			/**
			 * \brief	Draw non-textured quad. Vertex order  : winded
			 * \fn 		virtual void DrawSolidQuad(Point3D Vertices[4], float R,float G,float B, bool EnableBlend=false) const = 0;
			 * \param	Vertices : vertice position
			 * \param	R : red color
			 * \param	G : green color
			 * \param	B : blue color
			 * \param	EnableBlend : should blend
			 */
			virtual void DrawSolidQuad(Scene::TravState* state, Point3D Vertices[4], float R, float G, float B, bool EnableBlend = false) const = 0;

			/**
			 * \brief	Draw a rounded rectangle
			 * \fn 		virtual void DrawRoundedRectangle(Point3D Vertices[2], float R,float G,float B,float RBorder,float GBorder,float BBorder,float Radius) const =0;
			 * \param	Vertices : vertice position
			 * \param	R : red color
			 * \param	G : green color
			 * \param	B : blue color
			 * \param	RBorder : red color of the border
			 * \param	GBorder : green color of the border
			 * \param	BBorder : blue color of the border
			 * \param	Radius : corner radius
			 */
			virtual void DrawRoundedRectangle(Scene::TravState* state, Point3D Vertices[2], float R, float G, float B, float RBorder, float GBorder, float BBorder, float Radius) const = 0;

			virtual void drawSphere(ModuleSpecificRenderer* state, float r, int lats, int longs, Point3D* pos, float R, float G, float B) const {}

			virtual void DrawLine(ModuleSpecificRenderer* state, Point3D* Vertices, int VerticeCount, float R, float G, float B) const {}

		protected:
			/**
			 * \brief	destructor
			 * \fn 		~DirectRenderingMethods();
			 */
			virtual ~DirectRenderingMethods();
		};

	}
}
