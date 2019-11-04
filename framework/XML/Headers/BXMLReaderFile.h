#ifndef __BXML_READER_FILE_H__
#define __BXML_READER_FILE_H__

#include <string>
#include "XML.h"
#include "XMLNode.h"

class BXML_Dictionnary;
class FileHandle;
class CoreModifiable;

// ****************************************
// * BXMLReaderFile class
// * --------------------------------------
/*!  \class BXMLReaderFile
      read a XML file in bin format
	  \ingroup XML
*/
// ****************************************
class BXMLReaderFile 
{
public:
 
	//! constructor
	BXMLReaderFile( );
	
	//! destructor
	virtual ~BXMLReaderFile( );

	//! parse the bxml file
	XML*	ReadFile(FileHandle* file );

	bool	ReadFile(FileHandle* file,CoreModifiable*	delegateObject );

protected:
	XMLNode*		RecurseReadNode(char*& buffer,const BXML_Dictionnary& dict);	
	// read value type and reconstruct string
	kstl::string	ReadValue(const char* readbuffer,const BXML_Dictionnary& dict,int& readsize);

	// read unaligned
	int		ReadIntValue(const char* readbuffer);
	float	ReadFloatValue(const char* readbuffer);

	//!  associated XML instance
	XML*						myXML;

};

#endif  //__BXML_READER_FILE_H__
