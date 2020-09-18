
#ifndef __XML_READER_FILE_H__
#define __XML_READER_FILE_H__

#include "XML.h"
#include "XMLNode.h"
#include "CoreTypes.h"

class FileHandle;
class StringRef;

// ****************************************
// * XMLReaderFile class
// * --------------------------------------
/**
 * \file	XMLReaderFile.h
 * \class	XMLReaderFile
 * \ingroup ModuleXML
 * \brief	Read an XML file into an XML structure.
 */
 // ****************************************
class XMLReaderFile 
{
public:
 
	//! read the whole file and return the XML hierarchy as an XML instance
	static XMLBase* ReadFile(const kstl::string& file ,const char* force_as_format);

	//! parse whole file using the given delegate object 
	static bool ReadFile(const kstl::string& file,CoreModifiable*	delegateObject ,const char* force_as_format);

	//! parse buffer
	static bool ReadFile(char* Buff,CoreModifiable*	delegateObject,u64 buffsize ,char* encoding=0);

	//! read a "memory" xml file (in a string)
	static XMLBase* ReadString( const char * TheString );

	//! utility method 
	static void  StartDescriptionElement(void *data, const char *el, const char **attr);
	//! utility method 
    static void  EndDescriptionElement(void *data, const char *el);  
	//! utility method
	template<typename StringType>
	static void  StartDescriptionElementStringRef(void* data, StringRef* el, StringRef** attr);
	//! utility method 
	static void  EndDescriptionElementStringRef(void* data, StringRef* el);
	//! utility method
	static void  DeclHandler(void  *userData,   const char  *version,  const char  *encoding,  int standalone);
	//! utility method
	static void  CharacterHandler(void *userData,const char *s, int len);
	//! utility method
	static void  StartCData(void  *userData);
	//! utility method 
	static void  EndCData(void *userData,const char *s, int len);
	//! utility method
	template<typename StringType>
	static void  CharacterHandlerStringRef(void* userData, const char* s, int len);
	//! utility method 
	template<typename StringType>
	static void  EndCDataStringRef(void* userData, const char* s, int len);

	//! utility method
	static void  StartDescriptionElementDelegate(void *data, const char *el, const char **attr);
	//! utility method
    static void  EndDescriptionElementDelegate(void *data, const char *el);  
	//! utility method
	static void  DeclHandlerDelegate(void  *userData,   const char  *version,  const char  *encoding,  int standalone);
	//! utility method 
	static void  CharacterHandlerDelegate(void *userData,const char *s, int len);
	//! utility method
	static void  StartCDataDelegate(void  *userData);
	//! utility method 
	static void  EndCDataDelegate(void *userData,const char *s, int len);


protected:

	//! utility method 
	static bool CheckNotEmpty(const char *s, int len);

	//! internal method
	XMLBase* ProtectedReadFile(FileHandle* file );
	XMLBase* ProtectedReadFile(CoreRawBuffer* buffer, char* encoding = 0);

	XMLBase* ProtectedReadFileString(CoreRawBuffer* buffer, char* encoding = 0);
	XMLBase* ProtectedReadFileString(char* data, size_t size, char* encoding = 0);
	XMLBase* ProtectedReadFileStringRef(CoreRawBuffer* buffer, char* encoding = 0);

	bool	ProtectedReadFile(FileHandle* file,CoreModifiable*	delegateObject );

	bool	ProtectedReadFile(char* buffer,CoreModifiable*	delegateObject ,u64 size,char* encoding=0);


	//! constructor
	XMLReaderFile( );
	
	//! destructor
	virtual ~XMLReaderFile( );

	// ****************************************
	// * MyUserData class
	// * --------------------------------------
	/**
	 * \file	XMLReaderFile.h
	 * \class	MyUserData
	 * \ingroup ModuleXML
	 * \brief	
	 */
	 // ****************************************
	class MyUserData
	{
	public:
		kstl::vector<XMLNodeBase*>	mNodeHierarchy;
		XMLReaderFile*				mReader;
		CoreModifiable*				mDelegate;
	};

	//!  associated XML instance
	XMLBase*					mXML;

	unsigned int				mXMLElementStartDescriptionID;
	unsigned int				mXMLElementEndDescriptionID;
	unsigned int				mXMLDeclHandlerID;
	unsigned int				mXMLCharacterHandlerID;
	unsigned int				mXMLStartCDataID;
	unsigned int				mXMLEndCDataID;

};

template<typename StringType>
void  XMLReaderFile::StartDescriptionElementStringRef(void* data, StringRef* el, StringRef** attr)
{
	MyUserData* localuserdata = (MyUserData*)data;

	kstl::vector<XMLNodeBase*>& nodeHierarchy = localuserdata->mNodeHierarchy;

	XMLNodeTemplate<StringType>* xmlNode = new XMLNodeTemplate<StringType>(XML_NODE_ELEMENT, (const char*)el->mStringStart, (unsigned int)(el->mStringEnd - el->mStringStart));
	
	if (nodeHierarchy.size())
	{
#ifdef USE_PARENT_AND_SIBLING
		xmlNode->setParent(nodeHierarchy.back());
#endif //USE_PARENT_AND_SIBLING
		nodeHierarchy.back()->addChild(xmlNode);
	}

	int i;
	int attrcount = 0;
	// first count attributes 
	for (i = 0; attr[i]; i += 2)
	{
		attrcount++;
	}
	if (attrcount)
	{
		xmlNode->reserveAttribute(attrcount);
		for (i = 0; attr[i]; i += 2)
		{
			XMLAttributeTemplate<StringType>* attribute = new XMLAttributeTemplate<StringType>((const char*)attr[i]->mStringStart, (unsigned int)(attr[i]->mStringEnd - attr[i]->mStringStart), (const char*)attr[i + 1]->mStringStart, (unsigned int)(attr[i + 1]->mStringEnd - attr[i + 1]->mStringStart));
			xmlNode->addAttribute(attribute);
		}
	}
	nodeHierarchy.push_back(xmlNode);
}

template<typename StringType>
void  XMLReaderFile::EndCDataStringRef(void* userdata, const char* s, int len)
{
	if (!CheckNotEmpty(s, len))
		return;

	char	backup = s[len];
	char* change_s = (char*)s;

	change_s[len] = 0;

	MyUserData* localuserdata = (MyUserData*)userdata;

	kstl::vector<XMLNodeBase*>& nodeHierarchy = localuserdata->mNodeHierarchy;

	if (nodeHierarchy.size())
	{
		XMLNodeTemplate<StringType> * xmlNode = new XMLNodeTemplate<StringType>();
#ifdef USE_PARENT_AND_SIBLING
		xmlNode->setParent(nodeHierarchy[nodeHierarchy.size() - 1]);
#endif //USE_PARENT_AND_SIBLING
		nodeHierarchy[nodeHierarchy.size() - 1]->addChild(xmlNode);

		xmlNode->setType(XML_NODE_TEXT_NO_CHECK);
		xmlNode->setName("CData");
		xmlNode->setString(change_s);
	}

	change_s[len] = backup;
}

template<typename StringType>
void  XMLReaderFile::CharacterHandlerStringRef(void* userdata, const char* s, int len)
{
	if (!CheckNotEmpty(s, len))
		return;

	char	backup = s[len];
	char* change_s = (char*)s;

	change_s[len] = 0;

	MyUserData* localuserdata = (MyUserData*)userdata;

	kstl::vector<XMLNodeBase*>& nodeHierarchy = localuserdata->mNodeHierarchy;

	if (nodeHierarchy.size())
	{
		XMLNodeTemplate<StringType>* xmlNode = new XMLNodeTemplate<StringType>();

#ifdef USE_PARENT_AND_SIBLING
		xmlNode->setParent(nodeHierarchy[nodeHierarchy.size() - 1]);
#endif //USE_PARENT_AND_SIBLING
		nodeHierarchy[nodeHierarchy.size() - 1]->addChild(xmlNode);

		xmlNode->setType(XML_NODE_TEXT);
		xmlNode->setName("xml.text");
		xmlNode->setString(change_s);
	}

	change_s[len] = backup;
}



#endif 
