#include "PrecompiledHeaders.h"
#include "XMLReaderFile.h"
#include "XML.h"
#include "XMLNode.h"
#include "XMLAttribute.h"
#include "MinimalXML.h"

#include "FilePathManager.h"
#include "ModuleFileManager.h"
#include "Core.h"
#include "AttributePacking.h"

#include "Platform/Core/PlatformCore.h"

using namespace Kigs::Xml;
using namespace Kigs::Core;
using namespace Kigs::File;


void  XMLReaderFile::DeclHandler(void  *userdata,   const char  *version,  const char  *encoding,  int standalone)
{
	MyUserData*	localuserdata=(MyUserData*)userdata;

	if(version)
	{
		localuserdata->mReader->mXML->setVersion( version );
	}
	else
	{
		localuserdata->mReader->mXML->setVersion( "1.0" );
	}

	if(encoding)
	{
		localuserdata->mReader->mXML->setEncoding( encoding );
	}
	else
	{
		localuserdata->mReader->mXML->setEncoding( "" );
	}

	char	c[128];
	sprintf(c, "%d", standalone);

	localuserdata->mReader->mXML->setStandalone( c );
}

//! utility method 
void  XMLReaderFile::StartDescriptionElementDelegate(void *userdata, const char *el, const char **attr)
{
	MyUserData*	localuserdata=(MyUserData*)userdata;

	void*	pParams[2];
	pParams[0]=(void*)el;
	pParams[1]=(void*)attr;

    std::vector<CoreModifiableAttribute*> empty;

	localuserdata->mDelegate->CallMethod(localuserdata->mReader->mXMLElementStartDescriptionID,empty,pParams);
}

//! utility method
void  XMLReaderFile::EndDescriptionElementDelegate(void *userdata, const char *el)
{
	MyUserData*	localuserdata=(MyUserData*)userdata;
    std::vector<CoreModifiableAttribute*> empty;
	localuserdata->mDelegate->CallMethod(localuserdata->mReader->mXMLElementEndDescriptionID,empty,(void*)el);
}

//! utility method
void  XMLReaderFile::StartCDataDelegate(void  *userData)
{
	MyUserData*	localuserdata=(MyUserData*)userData;
    std::vector<CoreModifiableAttribute*> empty;
	localuserdata->mDelegate->CallMethod(localuserdata->mReader->mXMLStartCDataID,empty);
}

//! utility method 
void  XMLReaderFile::EndCDataDelegate(void *userData,const char *s, int len)
{
	MyUserData*	localuserdata=(MyUserData*)userData;

	char	backup = s[len];
	char*	change_s=(char*)s;

	change_s[len]=0;
    std::vector<CoreModifiableAttribute*> empty;
	localuserdata->mDelegate->CallMethod(localuserdata->mReader->mXMLEndCDataID,empty,(void*)s);
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
    std::vector<CoreModifiableAttribute*> empty;
	localuserdata->mDelegate->CallMethod(localuserdata->mReader->mXMLDeclHandlerID,empty,pParams);
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
    std::vector<CoreModifiableAttribute*> empty;
	localuserdata->mDelegate->CallMethod(localuserdata->mReader->mXMLCharacterHandlerID,empty,pParams);

	change_s[len]=backup;
}

void  XMLReaderFile::StartDescriptionElement(void *userdata, const char *el, const char **attr)
{
	MyUserData*	localuserdata=(MyUserData*)userdata;

	std::vector<XMLNodeBase*>& nodeHierarchy=localuserdata->mNodeHierarchy;

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

	std::vector<XMLNodeBase*>& nodeHierarchy=localuserdata->mNodeHierarchy;
	// don't pop last node cause it's the root node
	if(nodeHierarchy.size() > 1)
	{
		nodeHierarchy.pop_back();
	}
}


void  XMLReaderFile::EndDescriptionElementStringRef(void* userdata, StringRef* /*el*/)
{
	MyUserData* localuserdata = (MyUserData*)userdata;

	std::vector<XMLNodeBase*>& nodeHierarchy = localuserdata->mNodeHierarchy;
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

	std::vector<XMLNodeBase*>& nodeHierarchy=localuserdata->mNodeHierarchy;

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

	std::vector<XMLNodeBase*>& nodeHierarchy=localuserdata->mNodeHierarchy;

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
	mXMLElementStartDescriptionID=CharToID::GetID("XMLElementStartDescription");
	mXMLElementEndDescriptionID=CharToID::GetID("XMLElementEndDescription");
	mXMLDeclHandlerID=CharToID::GetID("XMLDeclHandler");
	mXMLCharacterHandlerID=CharToID::GetID("XMLCharacterHandler");
	mXMLStartCDataID=CharToID::GetID("XMLStartCData");
	mXMLEndCDataID=CharToID::GetID("XMLEndCData");
}

XMLReaderFile::~XMLReaderFile( )
{
}

XMLBase* XMLReaderFile::ReadFile(const std::string& file,const char* force_as_format )
{
	std::string forceFormat = "";
	if (force_as_format)
	{
		forceFormat = force_as_format;
	}

	// 
	SmartPointer<FileHandle> lFileHandle;
	auto pathManager = KigsCore::Singleton<FilePathManager>();
	if (pathManager)
		lFileHandle = pathManager->FindFullName(file);

	if (!lFileHandle || (lFileHandle->mStatus&FileHandle::Exist)==0)
		return NULL;

	XMLBase*	xml=0;

	std::string realExtension = pathManager->ResolveAlias(lFileHandle->mExtension);

	if (realExtension == ".kxml" || forceFormat == "kxml") // compressed
	{
		CMSP uncompressManager= KigsCore::GetSingleton("KXMLManager");
		if (uncompressManager)
		{
			u64 size;
			auto compressedbuffer = ModuleFileManager::LoadFile(lFileHandle.get(), size);
			if (compressedbuffer)
			{
				auto result = std::make_shared<CoreRawBuffer>();
				uncompressManager->SimpleCall("UncompressZeroEndedData", compressedbuffer.get(), result.get());
				if (result->length())
				{
					XMLReaderFile reader;
					xml = reader.ProtectedReadFile(result.get());
				}
			}
		}
		else
		{
			KIGS_ERROR("trying to uncompress kxml, but KXMLManager was not declared", 1);
		}
	}
	else // default is xml
	{
		XMLReaderFile reader;
		xml = reader.ProtectedReadFile(lFileHandle.get());
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

bool XMLReaderFile::ReadFile( const std::string &file ,CoreModifiable*	delegateObject,const char* force_as_format)
{
	bool result=false;

	auto pathManager = KigsCore::Singleton<FilePathManager>();
	SP<FileHandle> fullfilenamehandle=pathManager->FindFullName(file);

	
	XMLReaderFile reader;
	result=reader.ProtectedReadFile(fullfilenamehandle.get(),delegateObject);
	

	return result;
}

XMLBase* XMLReaderFile::ReadCoreRawBuffer(CoreRawBuffer* buffer)
{
	XMLReaderFile reader;
	return reader.ProtectedReadFileString((char*)buffer->buffer(), buffer->length());
}

XMLBase* XMLReaderFile::ReadString( const char * Buff )
{
#if 1
	XMLReaderFile reader;
	return reader.ProtectedReadFileString((char*)Buff, (u64)strlen(Buff));
#else
	MinimalXML p;
	MyUserData userdata;

	XMLReaderFile *Reader = new XMLReaderFile();

	userdata.mReader=Reader;

	XML *theXML = Reader->mXML = new XML( );
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
	
	if(userdata.mNodeHierarchy.size())
		theXML->setRoot(userdata.mNodeHierarchy[0]);

	userdata.mNodeHierarchy.clear();
	delete Reader;

	return theXML;
#endif
}

XMLBase* XMLReaderFile::ProtectedReadFile(CoreRawBuffer* buffer, char* encoding)
{
#ifdef KIGS_TOOLS
	return ProtectedReadFileString(buffer, encoding);
#else
	return ProtectedReadFileStringRef(buffer->shared_from_this(), encoding);
#endif
}

bool	XMLReaderFile::ProtectedReadFile(char* Buff, CoreModifiable* delegateObject,u64 size,char* encoding )
{	
	MinimalXML p;

	MyUserData userdata;

	userdata.mReader=this;
	userdata.mDelegate=delegateObject;

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
	auto rawbuffer = ModuleFileManager::LoadFileAsCharString(file,size,1);
	if(rawbuffer)
	{
		char* Buff=(char*)rawbuffer->buffer();
		bool result=false;
		result = ProtectedReadFile(Buff, delegateObject,size);
		return result;
	}

	return false;
}

XMLBase* XMLReaderFile::ProtectedReadFileString(CoreRawBuffer* buffer, char* encoding)
{
	return ProtectedReadFileString(buffer->buffer(), buffer->size(), encoding);
}

XMLBase* XMLReaderFile::ProtectedReadFileString(char* data, size_t size, char* encoding)
{
	MinimalXML p;

	MyUserData userdata;

	userdata.mReader=this;

	mXML = new XML( );

	p.setElementHandlerStringRef(StartDescriptionElementStringRef<std::string>, EndDescriptionElementStringRef);
	p.setXmlDeclHandler(DeclHandler);
	p.setCDataHandler(StartCData,EndCData);
	p.setCharacterDataHandler(CharacterHandler);
	p.setUserData(&userdata);
	
	if (!p.Parse((unsigned char*)data, size))
	{
		delete mXML;
		return 0;
	}
	
	if(userdata.mNodeHierarchy.size())
		mXML->setRoot(userdata.mNodeHierarchy[0]);

	userdata.mNodeHierarchy.clear();

	return (XML*)mXML;
}


XMLBase* XMLReaderFile::ProtectedReadFileStringRef(const SP<CoreRawBuffer> buffer, char* encoding)
{
	MinimalXML p;

	MyUserData userdata;

	userdata.mReader = this;

	mXML = new XMLStringRef(buffer);

	p.setElementHandlerStringRef(StartDescriptionElementStringRef<std::string_view>, EndDescriptionElementStringRef);
	p.setXmlDeclHandler(DeclHandler);
	p.setCDataHandler(StartCData, EndCDataStringRef<std::string_view>);
	p.setCharacterDataHandler(CharacterHandlerStringRef<std::string_view>);
	p.setUserData(&userdata);

	u64 size = buffer->length();
	char* Buff = (char*)buffer->buffer();

	if (!p.Parse((unsigned char*)Buff, size))
	{
		delete mXML;
		return 0;
	}

	if (userdata.mNodeHierarchy.size())
		mXML->setRoot(userdata.mNodeHierarchy[0]);

	userdata.mNodeHierarchy.clear();

	return (XMLStringRef*)mXML;
}


XMLBase* XMLReaderFile::ProtectedReadFile( FileHandle * file )
{
	u64 size;
	auto rawbuffer = ModuleFileManager::LoadFileAsCharString(file,size,1);

	if(rawbuffer)
	{
		XMLBase* result=0;
		result = ProtectedReadFile(rawbuffer.get());
		return result;	
	}

	return 0;
}