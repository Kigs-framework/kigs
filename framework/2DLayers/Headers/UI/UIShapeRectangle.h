#ifndef _UIShapeRectangle_H_
#define _UIShapeRectangle_H_

#include "UIShapeDelegate.h"


// ****************************************
// * UIShapeRectangle class
// * --------------------------------------
/**
* \file	UIShapeRectangle.h
* \class	UIShapeRectangle
* \ingroup 2DLayers
* \brief	draw parent ui as quad
*
*/
// ****************************************

class UIShapeRectangle : public UIShapeDelegate
{
public:
	DECLARE_CLASS_INFO(UIShapeRectangle, UIShapeDelegate, 2DLayers);

	/**
	* \brief	constructor
	* \param	name : instance name
	* \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	*/
	UIShapeRectangle(const std::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	void SetTexUV(UIItem* item, UIVerticesInfo* aQI) override;
	void SetVertexArray(UIItem* item, UIVerticesInfo* aQI) override;
};

#endif //_UIShapeRectangle_H_
