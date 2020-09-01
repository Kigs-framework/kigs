#ifndef _UI3DLINKEDITEM_H_
#define _UI3DLINKEDITEM_H_

#include "UIItem.h"
#include "maReference.h"


// ****************************************
// * UI3DLinkedItem class
// * --------------------------------------
/**
* \file	UI3DLinkedItem.h
* \class	UI3DLinkedItem
* \ingroup 2DLayers
* \brief	adjust position of the UIItem to match a 3D position in a Scene3D
*
* Suppose the UI2DLayer cover the same surface as the attached camera.
*/
// ****************************************
class UI3DLinkedItem : public UIItem
{
public:
	DECLARE_CLASS_INFO(UI3DLinkedItem, UIItem, 2DLayers);

	/**
	* \brief	constructor
	* \param	name : instance name
	* \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	*/
	UI3DLinkedItem(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

protected:
	void InitModifiable() override;
	void NotifyUpdate(const unsigned int /* labelid */) override;
	void Update(const Timer&  timer, void* /*addParam*/) override;

	maVect3DF				m3DPosition;
	maReference				mCamera;
	maReference				mNode;
	maBool					mUseUpOrientation;
};

#endif //_UI3DLINKEDITEM_H_
