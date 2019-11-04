#include "PrecompiledHeaders.h"
#include "BXMLReaderFile.h"
#include "XML.h"
#include "XMLNode.h"
#include "XMLAttribute.h"
#include "BXMLStructures.h"

#include "CoreTypes.h"
#include "ModuleFileManager.h"


//! constructor
BXMLReaderFile::BXMLReaderFile( )
{
}

//! destructor
BXMLReaderFile::~BXMLReaderFile( )
{
}

//! parse the bxml file
XML*	BXMLReaderFile::ReadFile(FileHandle* file )
{
	u64 flen;
	XML*	returnedXML=0;
	
	CoreRawBuffer* rawbuffer=ModuleFileManager::LoadFile(file,flen);
	if(rawbuffer)
	{
		unsigned char* buffer=(unsigned char*)rawbuffer->buffer();
		BXML_Header& head=*(BXML_Header*)buffer;
		if (head.myBXMLID == 1113083212) //'BXML')
		{
			// init dictionnary
			BXML_Dictionnary	dict((const char*)(buffer+sizeof(head)));

			returnedXML = new XML( );

			char* nodedata=(char*)buffer;
			nodedata+=sizeof(head);
			nodedata+=dict.GetSize();
			// dictionnary header
			nodedata+=sizeof(unsigned int)+sizeof(unsigned int);

			returnedXML->setRoot(RecurseReadNode(nodedata,dict));
		}
		// don't forget to free buffer
		rawbuffer->Destroy();
	}

	return returnedXML;
}

bool	BXMLReaderFile::ReadFile(FileHandle* file,CoreModifiable*	delegateObject )
{
	/*
	unsigned long flen;
	XML*	returnedXML=0;

	CoreRawBuffer* rawbuffer=ModuleFileManager::LoadFile(file.c_str(),flen);
	if(rawbuffer)
	{
	BXML_Header& head=*(BXML_Header*)buffer;
	if(head.myBXMLID== 1113083212) //'BXML')
	{
	// init dictionnary
	BXML_Dictionnary	dict((const char*)(buffer+sizeof(head)));

	returnedXML = new XML( );

	char* nodedata=(char*)buffer;
	nodedata+=sizeof(head);
	nodedata+=dict.GetSize();
	// dictionnary header
	nodedata+=sizeof(unsigned int)+sizeof(unsigned int);

	returnedXML->setRoot(RecurseReadNode(nodedata,dict));
	}
	// don't forget to free buffer
	delete[] buffer;
	}

	return returnedXML;*/
	// TODO
	return false;
}

XMLNode*	BXMLReaderFile::RecurseReadNode(char*& readbuffer,const BXML_Dictionnary& dict)
{
	XMLNode* resultnode=new XMLNode(XML_NODE_ELEMENT);
	BXML_Node&	readnode=*(BXML_Node*)readbuffer;

	readbuffer+=sizeof(readnode.myAttributesCount)+sizeof(readnode.mySonsCount)+sizeof(readnode.myNameID);
	int readsize;
	int attributescount=readnode.myAttributesCount;
	int sonscount=readnode.mySonsCount;
	resultnode->setName(dict.GetEntry(readnode.myNameID));
	resultnode->setString(ReadValue(readbuffer,dict,readsize));

	readbuffer+=readsize;
	// read attributes

	int i;
	for(i=0;i<attributescount;i++)
	{
		BXML_Attribute&	currentReadattr=*(BXML_Attribute*)readbuffer;
		readbuffer+=sizeof(BXML_Attribute);
		XMLAttribute*	attr=new XMLAttribute(dict.GetEntry(currentReadattr.myNameID),ReadValue(readbuffer,dict,readsize));
		readbuffer+=readsize;
		resultnode->addAttribute(attr);
	}

	for(i=0;i<sonscount;i++)
	{
		XMLNode* son=RecurseReadNode(readbuffer,dict);
		resultnode->addChild(son);
	}

	return resultnode;
}

kstl::string	BXMLReaderFile::ReadValue(const char* buffer,const BXML_Dictionnary& dict,int& readsize)
{
	char* readbuffer=(char*)buffer;
	unsigned short&  resulttype=*(unsigned short*)readbuffer;
	readsize=2;
	readbuffer+=2;

	char	printfbuffer[64];

	kstl::string resultstring="";

	ValueType		valuetype=(ValueType)resulttype;
	if(valuetype == DicoEntry)
	{
		unsigned short& id=*(unsigned short*)readbuffer;
		readsize+=2;
		return dict.GetEntry(id);
	}
	else
	{
		//unsigned short elemcount=0;
		// for array save element count
		if(resulttype&((unsigned short)ArrayMask))
		{
			resultstring+="{";
			// read elem count
			unsigned int elemcount=(unsigned int)resulttype>>8;
			unsigned int i;
			// read array
			if((resulttype&0xF)==FloatValue)
			{
				for(i=0;i<elemcount;i++)
				{
					float readfloat=ReadFloatValue(readbuffer);
					readsize+=sizeof(float);
					readbuffer+=sizeof(float);
					sprintf(printfbuffer,"%f",readfloat);
					resultstring+=printfbuffer;
					resultstring+=",";
				}
			}
			else // int array
			{
				for(i=0;i<elemcount;i++)
				{
					int	readint=ReadIntValue(readbuffer);
					readsize+=sizeof(int);
					readbuffer+=sizeof(int);
					sprintf(printfbuffer,"%d",readint);
					resultstring+=printfbuffer;
					resultstring+=",";
				}
			}
			resultstring+="}";

		}
		else if(valuetype == FloatValue)
		{
			// read float
			float	readfloat=ReadFloatValue(readbuffer);
			readsize+=sizeof(float);
			sprintf(printfbuffer,"%f",readfloat);
			resultstring+=printfbuffer;

		}
		else if(valuetype == IntegerValue)
		{
			// read int
			int	readint=ReadIntValue(readbuffer);
			readsize+=sizeof(int);
			sprintf(printfbuffer,"%d",readint);
			resultstring+=printfbuffer;
		}

	}

	return resultstring;
}

int		BXMLReaderFile::ReadIntValue(const char* readbuffer)
{
	if(((uptr)readbuffer)&2)
	{
		// read 2 unsigned shorts
		unsigned short s1=*(unsigned short*)readbuffer;
		unsigned short s2=*(unsigned short*)(readbuffer+2);
		unsigned int	v=((((unsigned int)s2)<<16)|((unsigned int)s1));
		return *(int*)&v;
	}
	else
	{
		return *(int*)readbuffer;
	}
}

float	BXMLReaderFile::ReadFloatValue(const char* readbuffer)
{
	if(((uptr)readbuffer)&2)
	{
		// read 2 unsigned shorts
		unsigned short s1=*(unsigned short*)readbuffer;
		unsigned short s2=*(unsigned short*)(readbuffer+2);
		unsigned int	v=((((unsigned int)s2)<<16)|((unsigned int)s1));
		return *(float*)&v;
	}
	else
	{
		return *(float*)readbuffer;
	}
}

