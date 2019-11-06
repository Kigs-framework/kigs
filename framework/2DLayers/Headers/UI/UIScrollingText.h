#ifndef _UISCROLLINGTEXT_H_
#define _UISCROLLINGTEXT_H_

#include "UIText.h"

class UIScrollingText : public UIText
{
public:
	DECLARE_CLASS_INFO(UIScrollingText, UIText, 2DLayers);
	
	/**
	 * \brief	constructor
	 * \param	name : instance name
	 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	 */
	UIScrollingText(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);


protected:
	void NotifyUpdate(const unsigned int labelid) override;
	void			Update(const Timer& a_timer, void* addParam) override;

	maFloat					myCaracterPerSeconde;
	unsigned int			myScrollIndex;
	double					mydElapsedTime;

};

#endif //_UISCROLLINGTEXT_H_