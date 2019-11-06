#ifndef _KEY_TO_ASCII_DX_H_
#define _KEY_TO_ASCII_DX_H_

#include "KeyToAscii.h"

// ****************************************
// * KeyToAsciiDX class
// * --------------------------------------
/*!  \class KeyToAsciiDX
     DirectX keyboard management
	 \ingroup InputDX
*/
// ****************************************

class KeyToAsciiDX : public KeyToAscii
{
public:
    DECLARE_CLASS_INFO(KeyToAsciiDX,KeyToAscii,Input)
    KeyToAsciiDX(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	virtual int Convert(unsigned Code);
};

#endif
