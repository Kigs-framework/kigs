#include "PrecompiledHeaders.h"
#include "BXMLWriterFile.h"
#include "BXMLStructures.h"
#include "XMLNode.h"
#include "XMLAttribute.h"

#include "FilePathManager.h"

//! constructor
BXMLWriterFile::BXMLWriterFile( )
{
}

//! write the file
bool BXMLWriterFile::WriteFile(const kstl::string& filename,XML& xml)
{
	SmartPointer<FileHandle> file = Platform_fopen(filename.c_str(), "wb");
	if (!file->myFile)
	{
		return false;
	}

	// first construct dictionnary
	BXML_Dictionnary	dictionnary;
	dictionnary.Build(xml);

	// save header
	BXML_Header	head;
	Platform_fwrite(&head,sizeof(head),1,file.get());

	// save dictionnary
	size_t data=dictionnary.GetEntryCount();
	Platform_fwrite(&data,sizeof(data),1,file.get());
	data=dictionnary.GetSize();
	Platform_fwrite(&data,sizeof(data),1,file.get());
	Platform_fwrite(dictionnary.GetBuffer(),dictionnary.GetSize(),1,file.get());

	// save node tree
	RecursiveNodeSave(file,dictionnary, (XMLNode *)xml.getRoot());

	Platform_fclose(file.get());

	return true;
}

//! destructor
BXMLWriterFile::~BXMLWriterFile( )
{
}

void	BXMLWriterFile::RecursiveNodeSave(SmartPointer<FileHandle>& file, BXML_Dictionnary& dict, XMLNode* current)
{
	BXML_Node	currentSaved;
	//void* tostore;

	currentSaved.myAttributesCount=current->getAttributeCount();
	currentSaved.mySonsCount=current->getChildCount();
	currentSaved.myNameID=dict.GetIndex(current->getName());
	// save BXML_Node header
	Platform_fwrite(&currentSaved,sizeof(currentSaved),1,file.get());
	// save node value 
	StoreValue(file,dict,current->getString());
	// save all attributes
	
	int i;
	for(i=0;i<currentSaved.myAttributesCount;i++)
	{
		BXML_Attribute	currentSavedattr;
		XMLAttribute*	attr=current->getAttribute(i);
//		unsigned int	valuesize;
//		unsigned int	valuecount;
//		ValueType		valuetype;

		currentSavedattr.myNameID=dict.GetIndex(attr->getName());
		// save currentSavedattr header
		Platform_fwrite(&currentSavedattr,sizeof(currentSavedattr),1,file.get());

		StoreValue(file,dict,attr->getString());
	}
	// then save all sons
	for(i=0;i<currentSaved.mySonsCount;i++)
	{
		RecursiveNodeSave(file,dict,current->getChildElement(i));
	}	
}

void	BXMLWriterFile::StoreValue(SmartPointer<FileHandle>& file, BXML_Dictionnary& dict, const kstl::string& value)
{
	size_t			valuesize;
	size_t			valuecount;
	ValueType		valuetype;
	unsigned short  resulttype;

	void* tostore=BXML_Attribute::GetValue(value,valuetype,valuesize,valuecount);
	resulttype=(unsigned short)valuetype;
	
	if(tostore)
	{
		// dictionnary entry
		if(valuetype == DicoEntry)
		{
			// write type
			Platform_fwrite(&resulttype,sizeof(resulttype),1,file.get());
			unsigned short id=dict.GetIndex(value);
			Platform_fwrite(&id,sizeof(id),1,file.get());
		}
		else
		{
			// for array save element count
			if(resulttype&((unsigned short)ArrayMask))
			{
				resulttype|=(unsigned short)(valuecount<<8);
			}
			// write type
			Platform_fwrite(&resulttype,sizeof(resulttype),1,file.get());

			// then save value
			Platform_fwrite(tostore,valuesize,1,file.get());
		}
		delete[] ((unsigned char*)tostore);
	}
}