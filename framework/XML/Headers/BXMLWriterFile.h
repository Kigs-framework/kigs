#ifndef __BXML_WRITER_FILE_H__
#define __BXML_WRITER_FILE_H__

#include "XML.h"
#include "Core.h"

class BXML_Dictionnary;

// ****************************************
// * BXMLWriterFile class
// * --------------------------------------
/*!  \class BXMLWriterFile
      write a XML file from an XML instance
	  \ingroup XML
*/
// ****************************************
class BXMLWriterFile 
{
public:
	//! constructor
    BXMLWriterFile( );

	//! write the file
	bool WriteFile(const kstl::string&,XML& xml);

	//! destructor
	virtual ~BXMLWriterFile( );
  
protected:

	// return size of current node
	void	RecursiveNodeSave(SmartPointer<FileHandle>& file, BXML_Dictionnary& dict, XMLNode* current);

	// store a single value
	void	StoreValue(SmartPointer<FileHandle>& file, BXML_Dictionnary& dict, const kstl::string& value);


};

#endif //__BXML_WRITER_FILE_H__
