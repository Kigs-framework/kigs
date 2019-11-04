#ifndef __XML_H__
#define __XML_H__

#include <string>
#include "MinimalXML.h"


class CoreModifiable;
class XMLNodeBase;
class CoreRawBuffer;

template<typename T> class XMLNodeTemplate;

// ****************************************
// * XMLTemplate class
// * --------------------------------------
/*!  \class XMLTemplate
      interface class to manage xml read/write
	  XMLTemplate instance manage general informations and a root xml node
	  \ingroup XMLTemplate
*/
// ****************************************

class XMLBase
{
public:
	friend class XMLReaderFile;
	friend class XMLWriterFile;
	friend class BXMLReaderFile;
	friend class BXMLWriterFile;

	XMLBase()
	{
		m_version = "1.0";
		m_encoding = "";
		m_standalone = "";
	}

	XMLBase(CoreRawBuffer* buffer);

	virtual ~XMLBase();

	//! set the encoding type (string)
	void setEncoding(std::string encoding){
		m_encoding = encoding;
	}

	//! set the version (string)
	void setVersion(std::string version) {
		m_version = version;
	}

	//! set standalone (string)
	void setStandalone(std::string standalone) {
		m_standalone = standalone;
	}

	void setPath(const std::string& path) { m_path = path; };

	//! return version (string)
	std::string getVersion()	{
		return m_version;
	}
	//! return encoding (string)
	std::string getEncoding()	{
		return m_encoding;
	}
	//! return standalone (string)
	std::string getStandalone()	{
		return m_standalone;
	}

	std::string getPath() { return m_path; }

	//! set the root node
	void setRoot(XMLNodeBase* root)	{
		m_root = root;
	}

	//! return root node
	XMLNodeBase* getRoot() {
		return m_root;
	}

	virtual bool	useStringRef() = 0;
	
		//! read the given xml file, and return the XMLTemplate instance giving the xml hierarchy 
	static XMLBase *ReadFile( const std::string &filename ,const char* force_as_format);

	//! read the given xml file, using the given delegate to parse
	static bool ReadFile( const std::string &filename ,CoreModifiable*	delegateObject,const char* force_as_format);

	//! write the current xml hierarchy to a xml file
	void	WriteFile( const std::string &filename );


protected:
	//! encoding 
	std::string m_encoding;
	//! version 
	std::string m_version;
	//! standalone
	std::string m_standalone;

	//! original path wheh loadded if available
	std::string m_path;

	//! root node
	XMLNodeBase* m_root=nullptr;
	CoreRawBuffer* m_ReadedRawBuffer = nullptr;
};

template<typename StringType>
class XMLTemplate :public XMLBase
{


public:
	//! destructor
	virtual ~XMLTemplate() {}
	//!default constructor
	XMLTemplate() : XMLBase()
	{

	}
	XMLTemplate(CoreRawBuffer* buffer) : XMLBase(buffer)
	{

	}



	//! create a empty XMLTemplate instance
    static XMLTemplate *createXML( );



  

	virtual bool	useStringRef() override;
private:

};



typedef XMLTemplate<std::string> XML;
typedef XMLTemplate<std::string_view> XMLStringRef;


#include "XMLReaderFile.h"

template<>
inline bool	XML::useStringRef()
{
	return false;
}

template<>
inline bool	XMLStringRef::useStringRef()
{
	return true;
}


template<typename StringType>
XMLTemplate< StringType>* XMLTemplate<StringType>::createXML()
{
	XMLTemplate* xml = new XMLTemplate();
	return xml;
}


#endif // __XML_H__
