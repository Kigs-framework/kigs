#ifndef _KEY_TO_ASCII_H_
#define _KEY_TO_ASCII_H_

#include "CoreModifiable.h"

class KeyboardDevice;

// ****************************************
// * KeyToAscii class
// * --------------------------------------
/**
 * \file	KeyToAscii.h
 * \class	KeyToAscii
 * \ingroup Input
 * \brief	abstract base class for key to ascii classes
 * \author	ukn
 * \version ukn
 * \date	ukn
 */
// ****************************************
class KeyToAscii : public CoreModifiable
{
public:
	DECLARE_ABSTRACT_CLASS_INFO(KeyToAscii,CoreModifiable,Input)
	
	/**
	 * \brief	constructor
	 * \fn 		KeyToAscii(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	 * \param	name : instance name
	 * \param	DECLARE_CLASS_NAME_TREE_ARG : list of arguments
	 */
	KeyToAscii(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
	
	/**
	 * \brief	convert a keyCode to ascii code
	 * \fn 		virtual int Convert(unsigned Code) = 0; 
	 * \param	Code : key code to convert
	 * \return	ascii code of the key
	 */
	virtual int Convert(unsigned Code) = 0;
};

#endif
