#ifndef __XML_WRITER_FILE_H__
#define __XML_WRITER_FILE_H__

#include "XML.h"
#include "kstlstring.h"

// ****************************************
// * XMLWriterFile class
// * --------------------------------------
/*!  \class XMLWriterFile
      write a XML file from an XML instance
	  \ingroup XML
*/
// ****************************************
class XMLWriterFile 
{
public:
	//! write the given xml hierarchy in a file
	static bool WriteFile(const kstl::string&,XML& xml);
	//! write the given xml hierarchy in a memory file (string)
	static void WriteString(XML& xml, kstl::string &Result, bool header = true, bool compress=false );

protected:
	//! constructor
    XMLWriterFile() = default;

	//! utility method
    void appendString( kstl::string &sAppend, bool isElementText = false );

	//! utility method to write the xml file header
	void writeHeader( XML &xml );

	//! utility to write the given node (recursive)
    void writeNode( XMLNode *node, int tab );


	std::vector<std::string> mData;
};

#endif 
