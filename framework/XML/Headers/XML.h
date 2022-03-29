#ifndef __XML_H__
#define __XML_H__

#include <string>
#include "MinimalXML.h"
#include <SmartPointer.h>

class CoreModifiable;
class XMLNodeBase;
class CoreRawBuffer;

template<typename T> class XMLNodeTemplate;


// ****************************************
// * XMLBase class
// * --------------------------------------
/**
 * \file	XML.h
 * \class	XMLBase
 * \ingroup ModuleXML
 * \brief	Base class for XML template structure.
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
		mVersion = "1.0";
		mEncoding = "";
		mStandalone = "";
	}

	XMLBase(const SP<CoreRawBuffer>& buffer);

	virtual ~XMLBase();

	//! set the encoding type (string)
	void setEncoding(std::string encoding){
		mEncoding = encoding;
	}

	//! set the version (string)
	void setVersion(std::string version) {
		mVersion = version;
	}

	//! set standalone (string)
	void setStandalone(std::string standalone) {
		mStandalone = standalone;
	}

	void setPath(const std::string& path) { mPath = path; };

	//! return version (string)
	std::string getVersion()	{
		return mVersion;
	}
	//! return encoding (string)
	std::string getEncoding()	{
		return mEncoding;
	}
	//! return standalone (string)
	std::string getStandalone()	{
		return mStandalone;
	}

	std::string getPath() { return mPath; }

	//! set the root node
	void setRoot(XMLNodeBase* root)	{
		mRoot = root;
	}

	//! return root node
	XMLNodeBase* getRoot() {
		return mRoot;
	}

	virtual bool	useStringRef() = 0;
	
		//! read the given xml file, and return the XMLTemplate instance giving the xml hierarchy 
	static XMLBase *ReadFile( const std::string &filename ,const char* force_as_format);

	//! read the given xml file, using the given delegate to parse
	static bool ReadFile( const std::string &filename ,CoreModifiable*	delegateObject,const char* force_as_format);

	//! write the current xml hierarchy to a xml file
	void	WriteFile( const std::string &filename );
	SP<CoreRawBuffer> WriteBuffer();


protected:
	//! encoding 
	std::string mEncoding;
	//! version 
	std::string mVersion;
	//! standalone
	std::string mStandalone;

	//! original path wheh loadded if available
	std::string mPath;

	//! root node
	XMLNodeBase* mRoot=nullptr;
	SP<CoreRawBuffer> mReadedRawBuffer = nullptr;
};



// ****************************************
// * XMLTemplate class
// * --------------------------------------
/**
 * \file	XML.h
 * \class	XMLTemplate
 * \ingroup ModuleXML
 * \brief	Manage XML structure using string or string_view.
 */
 // ****************************************
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
	XMLTemplate(const SP<CoreRawBuffer>& buffer) : XMLBase(buffer)
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
