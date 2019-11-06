#ifndef _BXMLSTRUCTURES_H_
#define _BXMLSTRUCTURES_H_

#include "kstlstring.h"
#include "kstlmap.h"
#include "kstlvector.h"

#include "XML.h"
#include "XMLNode.h"


class BXML_Header
{
public:

	BXML_Header()
	{
		myBXMLID = 1113083212;// 'BXML';
		myFlags=0;
	}

	// 'BXML'
	unsigned int	myBXMLID;
	unsigned int	myFlags;	// compression ? future use
};

enum	ValueType
{
	Unknow			=	0,
	DicoEntry		=	1,
	IntegerValue	=	2,
	FloatValue		=	3,
	//...
	ArrayMask		=	16
};

// dictionnary
class BXML_Dictionnary
{
public:

	BXML_Dictionnary()
	{
		myEntryCount=0;
		mySize=0;
		myBuffer=0;
		myEntryOffset=0;
		myExternalBuffer=false;
	}

	BXML_Dictionnary(const char* buffer)
	{
		BXML_Dictionnary* pdic=(BXML_Dictionnary*)buffer;
		myEntryCount=pdic->myEntryCount;
		mySize=pdic->mySize;
		myBuffer=(char*)buffer;
		myBuffer+=sizeof(myEntryCount)+sizeof(mySize);

		myExternalBuffer=true;

		myEntryOffset=new size_t[myEntryCount];

		unsigned int index=0;
		unsigned int i;
		myEntryOffset[index++]=0;
		for(i=0;i<mySize;i++)
		{
			if(myBuffer[i]==0)
			{
				myEntryOffset[index++]=(i+1);
			}
		}
	}

	~BXML_Dictionnary()
	{
		ClearBuffer();
	}

	void	Build(XML& xml);


	const kstl::string	GetEntry(unsigned int index) const
	{

		if((index<myEntryCount)&&myBuffer)
		{
			char* str=&(myBuffer[myEntryOffset[index]]);
			return str;
		}
		return "";
	}

	unsigned short	GetIndex(const kstl::string& value)
	{
		if(myEntries.find(value)!=myEntries.end())
		{
			return (unsigned short)myEntries[value];
		}
		// bad !!
		return (unsigned short)-1;
	}

	size_t	GetSize()
	{
		return	mySize;
	}

	unsigned int	GetEntryCount()
	{
		return myEntryCount;
	}

	char*	GetBuffer()
	{
		return myBuffer;
	}

protected:

	unsigned int	myEntryCount;
	size_t	mySize;

	void	ClearBuffer()
	{
		if((myBuffer)&&(!myExternalBuffer))
		{
			delete[] myBuffer;
			myBuffer=0;
		}
		else if(myExternalBuffer)
		{
			myBuffer=0;
			myExternalBuffer=false;
		}
		if(myEntryOffset)
		{
			delete[] myEntryOffset;
			myEntryOffset=0;
		}
	}

	void	AddEntry(const kstl::string& entry)
	{
		if(myEntries.find(entry)==myEntries.end())
		{
			myEntries[entry]=0;
			myEntryCount++;
			mySize+=entry.length()+1;	// 0 ending string
		}
	}

	// map to vector
	void	BuildStringList()
	{
		ClearBuffer();

		// align mySize on 4 bytes
		if(mySize&3)
		{
			mySize+=4-(mySize&3);
		}

		if(myEntryCount)
		{
			myEntryOffset=new size_t[myEntryCount];
			myBuffer=new char [mySize]; 
			size_t offset=0;
			int index=0;
			kstl::map<kstl::string, int>::iterator	itmap;
			for(itmap=myEntries.begin();itmap!=myEntries.end();itmap++)
			{
				myEntryOffset[index]=offset;
				memcpy(&(myBuffer[offset]),(*itmap).first.c_str(),(*itmap).first.length());
				// adjust offset
				offset+=(*itmap).first.length()+1;
				myBuffer[offset-1]=0;
				(*itmap).second=index;
				index++;
			}
		}
	}

	void	RecursiveBuild(XMLNode* );

	kstl::map<kstl::string, int> myEntries;
	char*                        myBuffer;
	size_t*                      myEntryOffset;
	bool                         myExternalBuffer;
};

class BXML_Node
{
public:	
	unsigned short	myAttributesCount;
	unsigned short	mySonsCount;
	unsigned short	myNameID;
	// then value 
	// ...
	// then attributes
	// ...
	// then sons
	// ...
};

class BXML_Attribute
{
public:
	// if array (mask ArrayMask), first BYTE is element count
	unsigned short		myNameID;
	
	// then	Value is stored
	// ...


	// utility method

	static ValueType	CheckValueType(const kstl::string& value);

	static void*	GetValue(const kstl::string& value,ValueType& resulttype,size_t& valuesize,size_t& valuecount);

	static bool IsFloat(const kstl::string& value,float& val);
	static bool IsInt(const kstl::string& value,int& val);

};



#endif //_BXMLSTRUCTURES_H_
