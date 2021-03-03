#ifndef _UIShapeDisc_H_
#define _UIShapeDisc_H_

#include "UIShapeDelegate.h"
#include "CoreModifiableAttribute.h"


// ****************************************
// * UIShapeDisc class
// * --------------------------------------
/**
* \file	UIShapeDisc.h
* \class	UIShapeDisc
* \ingroup 2DLayers
* \brief	draw parent ui as a disc
*
*/
// ****************************************

class UIShapeDisc : public UIShapeDelegate
{
public:
	DECLARE_CLASS_INFO(UIShapeDisc, UIShapeDelegate, 2DLayers);

	/**
	* \brief	constructor
	* \param	name : instance name
	* \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	*/
	UIShapeDisc(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	void SetTexUV(UIItem* item, UIVerticesInfo* aQI) override;
	void SetVertexArray(UIItem* item, UIVerticesInfo* aQI) override;

protected:
	virtual ~UIShapeDisc()
	{

	}
	
	maInt	mSliceCount=BASE_ATTRIBUTE(SliceCount,16);

};

#endif //_UIShapeDisc_H_
