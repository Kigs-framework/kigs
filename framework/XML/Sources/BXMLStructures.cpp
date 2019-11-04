#include "PrecompiledHeaders.h"
#include "BXMLStructures.h"
#include "XML.h"
#include "XMLNode.h"
#include "XMLAttribute.h"

void	BXML_Dictionnary::Build(XML& xml)
{
	XMLNode*	root=(XMLNode * )xml.getRoot();

	// add all strings entry
	RecursiveBuild(root);

	// build string list
	BuildStringList();

}

void	BXML_Dictionnary::RecursiveBuild(XMLNode* node)
{
	// add node name
	AddEntry(node->getName());

	// add node value if string value
	if(BXML_Attribute::CheckValueType(node->getString())==DicoEntry)
	{
		AddEntry(node->getString());
	}

	// add attributes names and values if string
	int i;
	int attrcount=node->getAttributeCount();
	for(i=0;i<attrcount;i++)
	{
		XMLAttribute*	attr=node->getAttribute(i);
		AddEntry(attr->getName());
		if(BXML_Attribute::CheckValueType(attr->getString())==DicoEntry)
		{
			AddEntry(attr->getString());
		}
	}

	// recurse to sons
	int childcount=node->getChildCount();
	for(i=0;i<childcount;i++)
	{
		RecursiveBuild(node->getChildElement(i));
	}

}

ValueType	BXML_Attribute::CheckValueType(const kstl::string& value)
{
	void*		returnedval=0;
	size_t		returnedsize;
	size_t		valuecount;
	ValueType	result;

	// not optimized at all (but used for save only)
	returnedval=GetValue(value,result,returnedsize,valuecount);

	if(returnedval)
	{
		// not optimized at all (but used for save only)
		delete[] ((char*)returnedval);
	}
	return result;
}

void*	BXML_Attribute::GetValue(const kstl::string& value,ValueType& resulttype,size_t& valuesize,size_t& valuecount)
{
	size_t len=value.length();
	size_t startpos=0;
	size_t endpos=len-1;
	int result=(int)Unknow;
	valuecount=1;

	if(len)
	{
		if((value[startpos]=='{')&&(value[endpos]=='}')&&(len>2))
		{
			// array
			result=(int)ArrayMask;
			kstl::string arrayValues=value.substr(1,value.length()-2);
			size_t intvalcount=0;
			size_t floatvalcount=0;
			size_t	stringvalcount=0;
			size_t startpos=0;
			bool found=true;
			// first find values count in array and store in a list

			kstl::vector<kstl::string>	valuelist;
			valuelist.clear();

			while(found)
			{
				size_t offset=arrayValues.find(',',startpos);
				if(offset==kstl::string::npos)
				{
					valuelist.push_back(arrayValues.substr(startpos,arrayValues.length()-startpos));
					found=false;
				}
				else
				{
					valuelist.push_back(arrayValues.substr(startpos,(offset-startpos)));
				}
				// check type
				float	testfloat;
				int		testint;

				if(IsFloat(valuelist[valuelist.size()-1],testfloat))
				{
					floatvalcount++;
				}
				else if(IsInt(valuelist[valuelist.size()-1],testint))
				{
					intvalcount++;
				}
				else
				{
					stringvalcount++;
				}

				startpos=offset+1;

			}

			// if one float store all as float
			if((floatvalcount)&&(stringvalcount==0)) 
			{
				result|=(int)FloatValue;
				resulttype=(ValueType)result;
				float* returnedvalue=new float[floatvalcount+intvalcount];
				valuesize=sizeof(float)*(floatvalcount+intvalcount);
				valuecount=floatvalcount+intvalcount;

				// fill array value
				unsigned int i;
				for(i=0;i<valuelist.size();i++)
				{
					float copyval;
					if(!IsFloat(valuelist[i],copyval))
					{
						int icopyval;
						IsInt(valuelist[i],icopyval);
						copyval=(float)icopyval;
					}
					returnedvalue[i]=copyval;
				}
				return returnedvalue;
			}
			// else store as int
			else if((intvalcount)&&(stringvalcount==0)) 
			{
				result|=(int)IntegerValue;
				resulttype=(ValueType)result;
				int* returnedvalue=new int[intvalcount];
				valuesize=sizeof(int)*intvalcount;
				valuecount=intvalcount;

				// fill array value
				unsigned int i;
				for(i=0;i<valuelist.size();i++)
				{					
					int icopyval;
					IsInt(valuelist[i],icopyval);
				
					returnedvalue[i]=icopyval;
				}

				return returnedvalue;

			}
		}
		// not an array, test numerical values
		else if((value[endpos]>='0')&&(value[endpos]<='9')) 
		{
			kstl::string currentval=value.substr(startpos,(endpos-startpos+1));
			float	testfloat;
			int		testint;	
			// check float
			if(IsFloat(currentval,testfloat))
			{
				result=(int)FloatValue;
				resulttype=(ValueType)result;
				float* returnedvalue=new float;
				valuesize=sizeof(float);
				*returnedvalue=testfloat;
				return returnedvalue;
			}
			else if(IsInt(currentval,testint))
			{
				result=(int)IntegerValue;
				resulttype=(ValueType)result;
				int* returnedvalue=new int;
				valuesize=sizeof(int);
				*returnedvalue=testint;
				return returnedvalue;
			}
		}
	}
	// else return string
	result=(int)DicoEntry;
	char* creturnedvalue=(new char[value.length()+1]);
	valuesize=value.length()+1;
	memcpy(creturnedvalue,value.c_str(),value.length());
	creturnedvalue[valuesize-1]=0;
	resulttype=(ValueType)result;
	return creturnedvalue;

}

// return true if value can be convert in float
bool		BXML_Attribute::IsFloat(const kstl::string& value,float& val)
{
	// check float
	size_t pos=value.find(".");
	if(pos!=kstl::string::npos)
	{
		if(sscanf(value.c_str(),"%f",&val)==1)
		{
			return true;
		}
	}
	return false;

}

// return true if value can be convert as an integer
bool		BXML_Attribute::IsInt(const kstl::string& value,int& val)
{
	if(sscanf(value.c_str(),"%d",&val)==1)
	{
		return true;
	}
	return false;

}
