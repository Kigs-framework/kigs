#ifndef _XMLMODULE_H_
#define _XMLMODULE_H_

#include "ModuleBase.h"

/*! \defgroup ModuleXML XML module
 *  xml read/write 
*/

// ****************************************
// * XMLModule class
// * --------------------------------------
/**
 * \file	XMLModule.h
 * \class	XMLModule
 * \ingroup ModuleXML
 * \ingroup Module
 * \brief	Generic module for XML management.
 */
 // ****************************************

class XMLModule : public ModuleBase
{
public:
	DECLARE_CLASS_INFO(XMLModule,ModuleBase,XML)

	//! constructor
    XMLModule(const std::string& name,DECLARE_CLASS_NAME_TREE_ARG);
         
	//! init module
    void Init(KigsCore* core, const std::vector<CoreModifiableAttribute*>* params) override;
	//! close module
    void Close() override;
                 
	//! update module
	void Update(const Timer& timer, void* addParam) override;
	virtual ~XMLModule();
protected:
}; 

#endif //_XMLMODULE_H_
