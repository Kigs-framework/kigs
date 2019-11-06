#ifndef _XMLMODULE_H_
#define _XMLMODULE_H_

#include "ModuleBase.h"

/*! \defgroup XML XML module
 *  xml read/write 
*/

// ****************************************
// * XMLModule class
// * --------------------------------------
/*!  \class XMLModule
      manage xml read/write using fast non compatible MinimalXML SAX parser
	  But the XML module is a bit special as it is used by core and by other base modules.
	  XML module does not define any CoreModifiable type class
	  \ingroup XML
*/
// ****************************************

class XMLModule : public ModuleBase
{
public:
	DECLARE_CLASS_INFO(XMLModule,ModuleBase,XML)

	//! constructor
    XMLModule(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);
         
	//! init module
    void Init(KigsCore* core, const kstl::vector<CoreModifiableAttribute*>* params) override;
	//! close module
    void Close() override;
                 
	//! update module
	void Update(const Timer& timer, void* addParam) override;
               
protected:
	//! destructor
    virtual ~XMLModule();   

}; 

#endif //_XMLMODULE_H_
