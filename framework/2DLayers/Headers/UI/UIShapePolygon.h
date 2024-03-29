#pragma once

#include "UIShapeDelegate.h"
#include "CoreModifiableAttribute.h"
#include "maCoreItem.h"

namespace Kigs
{

	namespace Draw2D
	{
		// ****************************************
		// * UIShapePolygon class
		// * --------------------------------------
		/**
		* \file	UIShapePolygon.h
		* \class	UIShapePolygon
		* \ingroup 2DLayers
		* \brief	draw parent ui as a polygon which vertices are given in maCoreItem array
		*
		*/
		// ****************************************

		class UIShapePolygon : public UIShapeDelegate
		{
		public:
			DECLARE_CLASS_INFO(UIShapePolygon, UIShapeDelegate, 2DLayers);

			/**
			* \brief	constructor
			* \param	name : instance name
			* \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
			*/
			UIShapePolygon(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);

			void SetTexUV(UIItem* item, UIVerticesInfo* aQI) override;
			void SetVertexArray(UIItem* item, UIVerticesInfo* aQI) override;

		protected:
			void InitModifiable() override;
			virtual void NotifyUpdate(const unsigned int /* labelid */)override;

			void	triangulatePolygon();

			// TODO
			bool	isConvex()
			{
				return true;
			}

			// no test is done at the moment on the polygon validity
			maCoreItem	mVertices = BASE_ATTRIBUTE(Vertices);

			std::vector<v2f>	mTriangulatedPoly;
		};

	}
}
