#ifndef _KEY_TO_ASCII_DX_H_
#define _KEY_TO_ASCII_DX_H_

#include "KeyToAscii.h"

// ****************************************
// * KeyToAsciiDX class
// * --------------------------------------
/**
* \file	KeyToAsciiDX.h
* \class	KeyToAsciiDX
* \ingroup Input
* \brief Specific DirectX KeyToAscii utility class
*
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
