#ifndef _UITexturedItem_H_
#define _UITexturedItem_H_

#include "UI/UIDrawableItem.h"
#include "UIVerticesInfo.h"
#include "SmartPointer.h"
#include "Texture.h"

class UITexturedItem : public UIDrawableItem
{
public:
	DECLARE_CLASS_INFO(UITexturedItem, UIDrawableItem, 2DLayers);

	/**
	* \brief	constructor
	* \param	name : instance name
	* \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	*/
	UITexturedItem(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	Texture* GetTexture() { return myTexture.get(); }
	void     SetTexture(Texture* t);

	// manage texture directly added 
	bool	addItem(CoreModifiable *item, ItemPosition pos = Last DECLARE_DEFAULT_LINK_NAME) override;
	bool	removeItem(CoreModifiable *item DECLARE_DEFAULT_LINK_NAME) override;


protected:
	virtual ~UITexturedItem();
	void NotifyUpdate(const unsigned int labelid) override;

	void SetTexUV(UIVerticesInfo * aQI) override;

	int GetTransparencyType() override;
	void PreDraw(TravState* state) override;  // use for texture predraw if needed
	void PostDraw(TravState* state) override; // use for texture postdraw if needed

	SmartPointer<Texture>				myTexture;

	v2f mUVMin{ FLT_MAX, FLT_MAX };
	v2f mUVMax{ FLT_MAX, FLT_MAX };

};

#endif //_UIItem_H_
