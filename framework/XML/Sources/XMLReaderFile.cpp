#include "PrecompiledHeaders.h"
#include "XMLReaderFile.h"
#include "BXMLReaderFile.h"
#include "XML.h"
#include "XMLNode.h"
#include "XMLAttribute.h"
#include "MinimalXML.h"

#include "FilePathManager.h"
#include "ModuleFileManager.h"
#include "Core.h"
#include "AttributePacking.h"


#ifdef _KIGS_ONLY_STATIC_LIB_
#include "Platform/Core/PlatformCore.h"
#endif

void  XMLReaderFile::DeclHandler(void  *userdata,   const char  *version,  const char  *encoding,  int standalone)
{
	MyUserData*	localuserdata=(MyUserData*)userdata;

	if(version)
	{
		localuserdata->myReader->myXML->setVersion( version );
	}
	else
	{
		localuserdata->myReader->myXML->setVersion( "1.0" );
	}

	if(encoding)
	{
		localuserdata->myReader->myXML->setEncoding( encoding );
	}
	else
	{
		localuserdata->myReader->myXML->setEncoding( "" );
	}

	char	c[128];
	sprintf(c, "%d", standalone);

	localuserdata->myReader->myXML->setStandalone( c );
}

//! utility method 
void  XMLReaderFile::StartDescriptionElementDelegate(void *userdata, const char *el, const char **attr)
{
	MyUserData*	localuserdata=(MyUserData*)userdata;

	void*	pParams[2];
	pParams[0]=(void*)el;
	pParams[1]=(void*)attr;

	localuserdata->myDelegate->CallMethod(localuserdata->myReader->myXMLElementStartDescriptionID,(kstl::vector<CoreModifiableAttribute*>&)*((kstl::vector<CoreModifiableAttribute*>*)0),pParams);
}

//! utility method
void  XMLReaderFile::EndDescriptionElementDelegate(void *userdata, const char *el)
{
	MyUserData*	localuserdata=(MyUserData*)userdata;

	localuserdata->myDelegate->CallMethod(localuserdata->myReader->myXMLElementEndDescriptionID,(kstl::vector<CoreModifiableAttribute*>&)*((kstl::vector<CoreModifiableAttribute*>*)0),(void*)el);
}

//! utility method
void  XMLReaderFile::StartCDataDelegate(void  *userData)
{
	MyUserData*	localuserdata=(MyUserData*)userData;

	localuserdata->myDelegate->CallMethod(localuserdata->myReader->myXMLStartCDataID,(kstl::vector<CoreModifiableAttribute*>&)*((kstl::vector<CoreModifiableAttribute*>*)0));
}

//! utility method 
void  XMLReaderFile::EndCDataDelegate(void *userData,const char *s, int len)
{
	MyUserData*	localuserdata=(MyUserData*)userData;

	char	backup = s[len];
	char*	change_s=(char*)s;

	change_s[len]=0;

	localuserdata->myDelegate->CallMethod(localuserdata->myReader->myXMLEndCDataID,(kstl::vector<CoreModifiableAttribute*>&)*((kstl::vector<CoreModifiableAttribute*>*)0),(void*)s);
	change_s[len]=backup;
}

//! utility method 
void  XMLReaderFile::DeclHandlerDelegate(void  *userdata,   const char  *version,  const char  *encoding,  int standalone)
{
	MyUserData*	localuserdata=(MyUserData*)userdata;

	void*	pParams[3];
	pParams[0]=(void*)version;
	pParams[1]=(void*)encoding;
	pParams[2]=(void*)&standalone;

	localuserdata->myDelegate->CallMethod(localuserdata->myReader->myXMLDeclHandlerID,(kstl::vector<CoreModifiableAttribute*>&)*((kstl::vector<CoreModifiableAttribute*>*)0),pParams);
}

//! utility method 
void  XMLReaderFile::CharacterHandlerDelegate(void *userdata,const char *s, int len)
{
	MyUserData*	localuserdata=(MyUserData*)userdata;

	char	backup = s[len];
	char*	change_s=(char*)s;

	change_s[len]=0;

	void*	pParams[2];
	pParams[0]=(void*)s;
	pParams[1]=(void*)&len;

	localuserdata->myDelegate->CallMethod(localuserdata->myReader->myXMLCharacterHandlerID,(kstl::vector<CoreModifiableAttribute*>&)*((kstl::vector<CoreModifiableAttribute*>*)0),pParams);

	change_s[len]=backup;
}

void  XMLReaderFile::StartDescriptionElement(void *userdata, const char *el, const char **attr)
{
	MyUserData*	localuserdata=(MyUserData*)userdata;

	kstl::vector<XMLNodeBase*>& nodeHierarchy=localuserdata->myNodeHierarchy;

	XMLNode* xmlNode = new XMLNode(XML_NODE_ELEMENT,el);

	if(nodeHierarchy.size())
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
			XMLAttribute *attribute = new XMLAttribute(attr[i], attr[i + 1]);
			xmlNode->addAttribute(attribute);
		}
	}
	nodeHierarchy.push_back(xmlNode);
}

void  XMLReaderFile::EndDescriptionElement(void *userdata, const char * /*el */)
{
	MyUserData*	localuserdata=(MyUserData*)userdata;

	kstl::vector<XMLNodeBase*>& nodeHierarchy=localuserdata->myNodeHierarchy;
	// don't pop last node cause it's the root node
	if(nodeHierarchy.size() > 1)
	{
		nodeHierarchy.pop_back();
	}
}


void  XMLReaderFile::EndDescriptionElementStringRef(void* userdata, StringRef* /*el*/)
{
	MyUserData* localuserdata = (MyUserData*)userdata;

	kstl::vector<XMLNodeBase*>& nodeHierarchy = localuserdata->myNodeHierarchy;
	// don't pop last node cause it's the root node
	if (nodeHierarchy.size() > 1)
	{
		nodeHierarchy.pop_back();
	}
}


bool XMLReaderFile::CheckNotEmpty(const char *s, int len)
{
	for(int i=0;i<len;i++)
	{
		unsigned char*	check=(unsigned char*)s;

		if(check[i]>32)
		{
			return true;
		}
	}

	return false;
}

void  XMLReaderFile::CharacterHandler(void *userdata,const char *s, int len)
{
	if(!CheckNotEmpty(s,len))
		return;

	char	backup = s[len];
	char*	change_s=(char*)s;

	change_s[len]=0;

	MyUserData*	localuserdata=(MyUserData*)userdata;

	kstl::vector<XMLNodeBase*>& nodeHierarchy=localuserdata->myNodeHierarchy;

	if(nodeHierarchy.size())
	{
		XMLNode* xmlNode = new XMLNode();

#ifdef USE_PARENT_AND_SIBLING
		xmlNode->setParent(nodeHierarchy[nodeHierarchy.size()-1]);
#endif //USE_PARENT_AND_SIBLING
		nodeHierarchy[nodeHierarchy.size()-1]->addChild(xmlNode);

		xmlNode->setType( XML_NODE_TEXT );
		xmlNode->setName( "xml.text" );
		xmlNode->setString( change_s );
	}

	change_s[len]=backup;
}


void  XMLReaderFile::StartCData(void  *userData)
{
	// nothing here
}

void  XMLReaderFile::EndCData(void *userdata,const char *s, int len)
{
	if(!CheckNotEmpty(s,len))
		return;

	char	backup = s[len];
	char*	change_s=(char*)s;

	change_s[len]=0;

	MyUserData*	localuserdata=(MyUserData*)userdata;

	kstl::vector<XMLNodeBase*>& nodeHierarchy=localuserdata->myNodeHierarchy;

	if(nodeHierarchy.size())
	{
		XMLNode* xmlNode = new XMLNode();
#ifdef USE_PARENT_AND_SIBLING
		xmlNode->setParent(nodeHierarchy[nodeHierarchy.size()-1]);
#endif //USE_PARENT_AND_SIBLING
		nodeHierarchy[nodeHierarchy.size()-1]->addChild(xmlNode);

		xmlNode->setType( XML_NODE_TEXT_NO_CHECK );
		xmlNode->setName( "CData" );
		xmlNode->setString( change_s );
	}

	change_s[len]=backup;
}



XMLReaderFile::XMLReaderFile( )
{
	myXMLElementStartDescriptionID=CharToID::GetID("XMLElementStartDescription");
	myXMLElementEndDescriptionID=CharToID::GetID("XMLElementEndDescription");
	myXMLDeclHandlerID=CharToID::GetID("XMLDeclHandler");
	myXMLCharacterHandlerID=CharToID::GetID("XMLCharacterHandler");
	myXMLStartCDataID=CharToID::GetID("XMLStartCData");
	myXMLEndCDataID=CharToID::GetID("XMLEndCData");
}

XMLReaderFile::~XMLReaderFile( )
{
}

XMLBase* XMLReaderFile::ReadFile(const kstl::string& file,const char* force_as_format )
{
	kstl::string forceFormat = "";
	if (force_as_format)
	{
		forceFormat = force_as_format;
	}

	// 
	SmartPointer<FileHandle> lFileHandle;
	SP<FilePathManager>	pathManager = KigsCore::GetSingleton("FilePathManager");
	if (!pathManager.isNil())
		lFileHandle = pathManager->FindFullName(file);

	if (!lFileHandle || (lFileHandle->myStatus&FileHandle::Exist)==0)
		return NULL;

	XMLBase*	xml=0;

	if(lFileHandle->myExtension ==".xml" || forceFormat=="xml")
	{
		XMLReaderFile reader;
		xml=reader.ProtectedReadFile(lFileHandle.get());
	}
	else if (lFileHandle->myExtension == ".kxml" || forceFormat == "kxml") // compressed
	{
		CMSP uncompressManager= KigsCore::GetSingleton("KXMLManager");
		if (uncompressManager)
		{
			u64 size;
			CoreRawBuffer* compressedbuffer = ModuleFileManager::LoadFile(lFileHandle.get(), size);
			if (compressedbuffer)
			{
				CoreRawBuffer* result=new CoreRawBuffer();
				uncompressManager->SimpleCall("UncompressZeroEndedData", compressedbuffer,result);
				compressedbuffer->Destroy();
				if (result->length())
				{
					XMLReaderFile reader;
					xml = reader.ProtectedReadFile(result);
				}
				result->Destroy();
			}
		}
		else
		{
			KIGS_ERROR("trying to uncompress kxml, but KXMLManager was not declared", 1);
		}
	}
	else // bxml
	{
		BXMLReaderFile reader;
		xml=reader.ReadFile(lFileHandle.get());
	}
	xml->setPath(file);
	return xml;
}

bool XMLReaderFile::ReadFile(char* Buff,CoreModifiable*	delegateObject ,u64 buffsize,char* encoding)
{
	bool result=false;
	XMLReaderFile *reader = new XMLReaderFile();
	result=reader->ProtectedReadFile(Buff,delegateObject,buffsize,encoding);
	delete reader;
	return result;
}

bool XMLReaderFile::ReadFile( const kstl::string &file ,CoreModifiable*	delegateObject,const char* force_as_format)
{
	bool result=false;

	SP<FilePathManager>	pathManager=KigsCore::GetSingleton("FilePathManager");
	SP<FileHandle> fullfilenamehandle=pathManager->FindFullName(file);

	// get file extension to choose XML or BXML ?
	if(fullfilenamehandle->myExtension==".xml" || strcmp(force_as_format, "xml")==0)
	{
		XMLReaderFile reader;
		result=reader.ProtectedReadFile(fullfilenamehandle.get(),delegateObject);
	}
	else // bxml
	{
		BXMLReaderFile reader;
		result=reader.ReadFile(fullfilenamehandle.get(),delegateObject);
	}

	return result;
}

XML *XMLReaderFile::ReadString( const char * Buff )
{
#if 1
	XMLReaderFile reader;
	return reader.ProtectedReadFile((char*)Buff, (u64)strlen(Buff));
#else
	MinimalXML p;
	MyUserData userdata;

	XMLReaderFile *Reader = new XMLReaderFile();

	userdata.myReader=Reader;

	XML *theXML = Reader->myXML = new XML( );
	p.setElementHandler(StartDescriptionElement, EndDescriptionElement);
	p.setXmlDeclHandler(DeclHandler);
	p.setCDataHandler(StartCData, EndCData);
	p.setCharacterDataHandler(CharacterHandler);
	p.setUserData(&userdata);

	// parse
	if (!p.Parse((unsigned char*)Buff, (int)strlen(Buff)))
	{
		delete theXML;
		delete Reader;
		return 0;
	}
	
	if(userdata.myNodeHierarchy.size())
		theXML->setRoot(userdata.myNodeHierarchy[0]);

	userdata.myNodeHierarchy.clear();
	delete Reader;

	return theXML;
#endif
}

bool	XMLReaderFile::ProtectedReadFile(char* Buff,CoreModifiable*	delegateObject,u64 size,char* encoding )
{	
	MinimalXML p;

	MyUserData userdata;

	userdata.myReader=this;
	userdata.myDelegate=delegateObject;

	p.setElementHandler(StartDescriptionElementDelegate, EndDescriptionElementDelegate);
	p.setCDataHandler(StartCDataDelegate,EndCDataDelegate);
	p.setXmlDeclHandler(DeclHandlerDelegate);
	p.setCharacterDataHandler(CharacterHandlerDelegate);
	p.setUserData(&userdata);


	if (!p.Parse((unsigned char*)Buff, size) )
	{

		return false;
	}
	
	return true;
}

bool	XMLReaderFile::ProtectedReadFile(FileHandle* file,CoreModifiable*	delegateObject )
{
	u64 size;
	CoreRawBuffer* rawbuffer=ModuleFileManager::LoadFileAsCharString(file,size);

	if(rawbuffer)
	{
		char* Buff=(char*)rawbuffer->buffer();
		bool result=false;
		result=ProtectedReadFile(Buff,delegateObject,size);
		rawbuffer->Destroy();
		return result;
	}

	return false;
}

XML*	XMLReaderFile::ProtectedReadFile(char* Buff,u64 size,char* encoding)
{
	MinimalXML p;

	MyUserData userdata;

	userdata.myReader=this;

	myXML = new XML( );

	p.setElementHandlerStringRef(StartDescriptionElementStringRef<std::string>, EndDescriptionElementStringRef);
	p.setXmlDeclHandler(DeclHandler);
	p.setCDataHandler(StartCData,EndCData);
	p.setCharacterDataHandler(CharacterHandler);
	p.setUserData(&userdata);
	
	if (!p.Parse((unsigned char*)Buff, size))
	{
		delete myXML;
		return 0;
	}
	
	if(userdata.myNodeHierarchy.size())
		myXML->setRoot(userdata.myNodeHierarchy[0]);

	userdata.myNodeHierarchy.clear();

	return (XML*)myXML;
}


XMLStringRef* XMLReaderFile::ProtectedReadFile(CoreRawBuffer* buffer, char* encoding)
{
	MinimalXML p;

	MyUserData userdata;

	userdata.myReader = this;

	myXML = new XMLStringRef(buffer);

	p.setElementHandlerStringRef(StartDescriptionElementStringRef<std::string_view>, EndDescriptionElementStringRef);
	p.setXmlDeclHandler(DeclHandler);
	p.setCDataHandler(StartCData, EndCDataStringRef<std::string_view>);
	p.setCharacterDataHandler(CharacterHandlerStringRef<std::string_view>);
	p.setUserData(&userdata);

	u64 size = buffer->length();
	char* Buff = (char*)buffer->buffer();

	if (!p.Parse((unsigned char*)Buff, size))
	{
		delete myXML;
		return 0;
	}

	if (userdata.myNodeHierarchy.size())
		myXML->setRoot(userdata.myNodeHierarchy[0]);

	userdata.myNodeHierarchy.clear();

	return (XMLStringRef*)myXML;
}


XMLStringRef *XMLReaderFile::ProtectedReadFile( FileHandle * file )
{
	u64 size;
	CoreRawBuffer* rawbuffer=ModuleFileManager::LoadFileAsCharString(file,size);

	if(rawbuffer)
	{
		XMLStringRef* result=0;
		result=ProtectedReadFile(rawbuffer);
		rawbuffer->Destroy();
		return result;	
	}

	return 0;
}