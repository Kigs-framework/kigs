#ifndef _UI3DLINKEDITEM_H_
#define _UI3DLINKEDITEM_H_

#include "UIItem.h"
#include "maReference.h"

// Adjust the position of all children according to a flow layout algorithm

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

	maVect3DF				my3DPosition;
	maReference				myCamera;
	maReference				myNode;
	maBool					myUseUpOrientation;
};

#endif //_UI3DLINKEDITEM_H_
