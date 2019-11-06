#include "PrecompiledHeaders.h"
#include "JSonFileParser.h"
#include "FilePathManager.h"
#include "ModuleFileManager.h"
#include "CoreModifiableAttribute.h"
#include "maUSString.h"
#include "CoreMap.h"

template <typename stringType, typename parserType>
JSonFileParserBase<stringType,parserType>::JSonFileParserBase(const kstl::string& filename, CoreModifiable*	delegateObject) :
myDelegateObject(delegateObject)
{
	InitParser(filename);
}

template <typename stringType, typename parserType>
JSonFileParserBase<stringType,parserType>::JSonFileParserBase()
{
}

template <typename stringType, typename parserType>
void JSonFileParserBase<stringType,parserType>::InitParserFromString(CoreRawBuffer* Buff)
{
	// init often used IDs
	myJSonObjectStartID=CharToID::GetID("JSonObjectStart");
	myJSonObjectEndID=CharToID::GetID("JSonObjectEnd");
	myJSonArrayStartID=CharToID::GetID("JSonArrayStart");
	myJSonArrayEndID=CharToID::GetID("JSonArrayEnd");
	myJSonParamListID=CharToID::GetID("JSonParamList");

	parserType	asciiParser(Buff);
	parserType	mainBlock(Buff);

	// check if first character is a { or a [

	asciiParser.GoToNextNonWhitespace();
	if (*asciiParser == '{')
	{
		if (asciiParser.GetBlockExcludeString(mainBlock, '{', '}'))
		{
			myParamList.clear();

			myDelegateObject->CallMethod(myJSonObjectStartID, myParamList);

			if (!ParseBlock(mainBlock))
			{
				KIGS_ERROR("JSon Parser : malformed json block ", 1);
			}

			myDelegateObject->CallMethod(myJSonArrayEndID, myParamList);
		}
		else
		{
			KIGS_ERROR("JSon Parser : malformed json file", 1);
		}
	}
	else if (*asciiParser == '[')
	{
		if (asciiParser.GetBlockExcludeString(mainBlock, '[', ']'))
		{
			if (mainBlock.length())
			{
				myParamList.clear();
				myDelegateObject->CallMethod(myJSonArrayStartID, myParamList);

				if (!ParseArray(mainBlock))
				{
					KIGS_ERROR("JSon Parser : malformed json block ", 1);
				}

				myDelegateObject->CallMethod(myJSonArrayEndID, myParamList);
			}
		}
		else
		{
			KIGS_ERROR("JSon Parser : malformed json file", 1);
		}
	}
	else
	{
		KIGS_ERROR("JSon Parser : malformed json file", 1);
	}
}

template <typename stringType, typename parserType>
void JSonFileParserBase<stringType,parserType>::InitParser(const kstl::string& filename)
{
	FilePathManager*	pathManager=(FilePathManager*)KigsCore::GetSingleton("FilePathManager");

	SmartPointer<::FileHandle> fullfilenamehandle=pathManager->FindFullName(filename);

	u64 size;
	CoreRawBuffer* Buff=ModuleFileManager::LoadFileAsCharString(fullfilenamehandle.get(),size);

	if(Buff)
	{
		InitParserFromString(Buff);
		Buff->Destroy();
	}
#ifdef _DEBUG
	else
	{
		printf("FILE NOT FOUND : %s\n",filename.c_str());
	}
#endif
}


// specialized
template <>
void 	JSonFileParserBase<kstl::string, AsciiParserUtils>::AddValueToParamList(kstl::string strObjName, kstl::string objparamValue)
{
	CoreModifiableAttribute* Value;
	kstl::string	strvalue = objparamValue;

	myParamList.push_back(new maString(*myDelegateObject, false, strObjName, strObjName));

	// check if string, numeric or boolean
	if (strvalue[0] == '"')
	{
		// string
		// remove quotes
		kstl::string paramValue = objparamValue.substr(1, objparamValue.length() - 2);
		Value = new maString(*myDelegateObject, false, strObjName, paramValue);
	}
	else if ((strvalue == "true") || (strvalue == "false"))
	{
		// boolean
		Value = new maBool(*myDelegateObject, false, strObjName, false);
		Value->setValue(strvalue);
	}
	else
	{
		if (strvalue.find('.') != std::string::npos)
		{
			// float
			Value = new maFloat(*myDelegateObject, false, strObjName, KFLOAT_ZERO);
		}
		else
		{
			// int
			// check negative
			if (strvalue.find('-') != std::string::npos)
			{
				Value = new maInt(*myDelegateObject, false, strObjName, 0);
			}
			else
			{
				Value = new maUInt(*myDelegateObject, false, strObjName, 0);
			}
		}

		Value->setValue(strvalue);
	}

	myParamList.push_back(Value);
}

template <>
void 	JSonFileParserBase<usString, US16ParserUtils>::AddValueToParamList(usString strObjName, usString objparamValue)
{
	CoreModifiableAttribute* Value;
	usString	strvalue = objparamValue;

	kstl::string name = strObjName.ToString();

	myParamList.push_back(new maUSString(*myDelegateObject, false, name, strObjName));
	// check if string, numeric or boolean
	if (strvalue[0] == '"')
	{
		// string
		// remove quotes
		usString paramValue = objparamValue.substr(1, objparamValue.length() - 2);
		Value = new maUSString(*myDelegateObject, false, name, paramValue);
	}
	else if ((strvalue == "true") || (strvalue == "false"))
	{
		// boolean
		Value = new maBool(*myDelegateObject, false, name, false);
		Value->setValue(strvalue);
	}
	else
	{
		if (strvalue.find('.') != std::string::npos)
		{
			// float
			Value = new maFloat(*myDelegateObject, false, name, KFLOAT_ZERO);
		}
		else
		{
			// int
			Value = new maInt(*myDelegateObject, false, name, 0);
		}
		Value->setValue(strvalue);
	}

	myParamList.push_back(Value);
}



template <typename stringType, typename parserType>
bool JSonFileParserBase<stringType, parserType>::Export(CoreMap<stringType>* a_value, const kstl::string& a_fileName)
{
	stringType L_Buffer = "";
	RecursiveParseElement(a_value,L_Buffer);

	SmartPointer<::FileHandle> L_File = Platform_fopen(a_fileName.c_str(), "wb");
	if(L_File->myFile)
	{
		Platform_fwrite(GetStringByteBuffer(L_Buffer), GetStringCharSize(), GetStringByteSize(L_Buffer)-GetStringCharSize(), L_File.get());
		Platform_fclose(L_File.get());
		return true;
	}
	return false;
}

template <typename stringType, typename parserType>
bool JSonFileParserBase<stringType, parserType>::ExportToString(CoreMap<stringType>* a_value, stringType& output)
{
	output="";
	RecursiveParseElement(a_value,output);
	return true;
}

template <typename stringType, typename parserType>
void JSonFileParserBase<stringType, parserType>::RecursiveParseElement(CoreMap<stringType>* a_value, stringType& a_buffer)
{
	a_buffer += "{\n";
	CoreItemIterator It = a_value->begin();
	CoreItemIterator ItEnd = a_value->end();
	while(It != ItEnd)
	{
		if(It != a_value->begin())
		{
			a_buffer += ",";
		}
		a_buffer += "\"";
		stringType	key;
		It.getKey(key);
		a_buffer += key;
		a_buffer += "\":";
		
		switch((*It).GetType())
		{
			case CoreItem::COREMAP:
			{
				RecursiveParseElement((CoreMap<stringType>*)&(*It), a_buffer);
				break;
			}
			case CoreItem::COREVECTOR:
			{
				RecursiveParseElement((CoreVector*)&(*It), a_buffer);
				break;
			}
			case CoreItem::COREVALUE:
			{
				//a_buffer += "\"";
				AddValueToBuffer((CoreItem*)&(*It), a_buffer);
				a_buffer += "\n";
				break;
			}
			default:
			{
				break;
			}
		}
		It++;
	}
	a_buffer += "}\n";
}

template <typename stringType, typename parserType>
void JSonFileParserBase<stringType, parserType>::RecursiveParseElement(CoreVector* a_value, stringType& a_buffer)
{
	a_buffer += "[";

	for(unsigned int i = 0; i < a_value->size(); i++)
	{
		if(((*a_value)[i]).GetType() & CoreItem::COREMAP)
		{
			RecursiveParseElement(((CoreMap<stringType>*)&(*a_value)[i]), a_buffer);
		}
		else if(((*a_value)[i]).GetType() & CoreItem::COREVECTOR)
		{
			RecursiveParseElement(((CoreVector*)&(*a_value)[i]),a_buffer);
		}
		else if(((*a_value)[i]).GetType() & CoreItem::COREVALUE)
		{
			AddValueToBuffer(((CoreItem*)&(*a_value)[i]),a_buffer);
		}
		if(i < a_value->size()-1)
			a_buffer += ",";
	}
	a_buffer += "]\n";
}

template <typename stringType, typename parserType>
void JSonFileParserBase<stringType, parserType>::AddValueToBuffer(CoreItem* a_value, stringType& a_Destbuffer)
{
	stringType L_Value = "";
	a_value->getValue(L_Value);
	if(a_value->isString())
	{
		a_Destbuffer += "\"";
		a_Destbuffer += L_Value;
		a_Destbuffer += "\"";
	}
	else
		a_Destbuffer+=L_Value;
}

template <typename stringType, typename parserType>
CoreItem*	JSonFileParserBase<stringType, parserType>::Get_JsonDictionary(const kstl::string& filename)
{
	//Create instance of DictionaryFromJson
	myDictionaryFromJson = (DictionaryFromJson*)CreateDictionnaryFromJSONInstance();
	myDelegateObject = myDictionaryFromJson;

	InitParser(filename);

	CoreItem* L_TempDictionary = getDictionnary();

	// get a ref before destruction
	if(L_TempDictionary)
		L_TempDictionary->GetRef();

	myDictionaryFromJson->Destroy();
	myDictionaryFromJson = NULL;

	if(L_TempDictionary && !L_TempDictionary->empty())
		return L_TempDictionary;
	else
	{
		if(L_TempDictionary)
		{
			L_TempDictionary->Destroy();
		}
	}
	return NULL;
}

// specialized

template <>
CoreModifiable*			JSonFileParserBase<kstl::string, AsciiParserUtils>::CreateDictionnaryFromJSONInstance()
{
	return (CoreModifiable*)KigsCore::GetInstanceOf("L_DictionaryFromJson", "DictionaryFromJson");
}

template <>
CoreItem*	JSonFileParserBase< kstl::string, AsciiParserUtils>::getDictionnary()
{
	return ((DictionaryFromJson*)myDictionaryFromJson)->Get_Dictionary();
}


template <>
int	JSonFileParserBase<kstl::string, AsciiParserUtils>::GetStringByteSize(const kstl::string& tocheck)
{
	// add trailing 0
	return tocheck.size()+1;
}

template <>
int	JSonFileParserBase<kstl::string, AsciiParserUtils>::GetStringCharSize()
{
	return 1;
}

template <>
const unsigned char*	JSonFileParserBase<kstl::string, AsciiParserUtils>::GetStringByteBuffer(const kstl::string& tocheck)
{
	return (const unsigned char*)tocheck.c_str();
}

template <>
CoreModifiable*			JSonFileParserBase< usString, US16ParserUtils>::CreateDictionnaryFromJSONInstance()
{
	return (CoreModifiable*)KigsCore::GetInstanceOf("L_DictionaryFromJson", "DictionaryFromJsonUTF16");
}

template <>
CoreItem*	JSonFileParserBase< usString, US16ParserUtils>::getDictionnary()
{
	return ((DictionaryFromJsonUTF16*)myDictionaryFromJson)->Get_Dictionary();
}


template <>
int	JSonFileParserBase< usString, US16ParserUtils>::GetStringByteSize(const usString& tocheck)
{
	return (tocheck.strlen()+1)*2;
}

template <>
int	JSonFileParserBase< usString, US16ParserUtils>::GetStringCharSize()
{
	return 2;
}

template <>
const unsigned char*	JSonFileParserBase< usString, US16ParserUtils>::GetStringByteBuffer(const usString& tocheck)
{
	return (const unsigned char*)tocheck.us_str();
}

template <typename stringType, typename parserType>
CoreItem*	JSonFileParserBase<stringType, parserType>::Get_JsonDictionaryFromString(const stringType& jsonString)
{
	//Create instance of DictionaryFromJson
	myDictionaryFromJson = (DictionaryFromJson*)CreateDictionnaryFromJSONInstance();
	myDelegateObject = myDictionaryFromJson;

	char * copybuffer = new char[GetStringByteSize(jsonString)];
	memcpy(copybuffer, GetStringByteBuffer(jsonString), GetStringByteSize(jsonString));

	CoreRawBuffer*	Buff = new CoreRawBuffer((void*)copybuffer, GetStringByteSize(jsonString));

	InitParserFromString(Buff);

	Buff->Destroy();

	CoreItem* L_TempDictionary = getDictionnary();

	// get a ref before destruction
	if(L_TempDictionary)
		L_TempDictionary->GetRef();

	myDictionaryFromJson->Destroy();
	myDictionaryFromJson = NULL;

	if(L_TempDictionary && !L_TempDictionary->empty())
		return L_TempDictionary;
	else
	{
		if(L_TempDictionary)
		{
			L_TempDictionary->Destroy();
		}
	}
	return NULL;

}

// specialized
template <>
bool JSonFileParserBase<kstl::string, AsciiParserUtils>::ParseBlock(AsciiParserUtils& Block)
{
	// retreive obj name first (quoted word followed by ':')
	AsciiParserUtils objName(Block);
	while(Block.GetWord(objName,':',true))
	{
		// check that it's a valid name
		if((objName[0]=='"')&&(objName[objName.length()-1]=='"'))
		{
			// remove quotes
			kstl::string strObjName = objName.subString(1, objName.length() - 2);

			AsciiParserUtils objValue(Block);
			Block.GetTrailingPart(objValue,true);
			// check value type
			if(objValue[0] == '[')
			{
				if(myParamList.size())
				{
					NotifyDelegateWithParamList();
				}
				// array
				AsciiParserUtils newarray(Block);
				if (Block.GetBlockExcludeString(newarray, '[', ']'))
				{
					kstl::vector<CoreModifiableAttribute*>	params;
					maString* arrayname=new maString(*myDelegateObject,false,LABEL_AND_ID(ArrayName),strObjName);
					params.push_back(arrayname);

					myDelegateObject->CallMethod(myJSonArrayStartID,params);

					if(!ParseArray(newarray))
					{
						return false;
					}

					myDelegateObject->CallMethod(myJSonArrayEndID,params);
					
					delete arrayname;

					Block.GoAfterNextSeparator(',');
				}
				else
				{
					return false;
				}
			}
			else if(objValue[0] == '{')
			{
				// object
				if(myParamList.size())
				{
					NotifyDelegateWithParamList();
				}
				AsciiParserUtils newblock(Block);
				if (Block.GetBlockExcludeString(newblock, '{', '}'))
				{
					kstl::vector<CoreModifiableAttribute*>	params;
					maString* neobjname=new maString(*myDelegateObject,false,LABEL_AND_ID(ObjectName),strObjName);
					params.push_back(neobjname);

					myDelegateObject->CallMethod(myJSonObjectStartID,params);

					if(!ParseBlock(newblock))
					{
						return false;
					}

					myDelegateObject->CallMethod(myJSonObjectEndID,params);
					
					delete neobjname;
					
					Block.GoAfterNextSeparator(',');
				}
				else
				{
					return false;
				}
			}
			else if (objValue[0] == '"')
			{
				// string 
				kstl::string objparamValue;
				{
					AsciiParserUtils objparamValuebkl(Block);
					Block.GetQuotationWord(objparamValuebkl);
					objparamValue = (kstl::string) objparamValuebkl;
				}
				Block.GoAfterNextSeparator(',');
				Block.GoToNextNonWhitespace();
				AddValueToParamList(strObjName, objparamValue);
			}
			else // numeric or bool
			{
				AsciiParserUtils objparamValue(Block);
				Block.GetWord(objparamValue, ',', true);
				AddValueToParamList(strObjName, objparamValue);
			}

/*			else
			{
				// string numeric or bool
				AsciiParserUtils objparamValue(Block);
				Block.GetWord(objparamValue,',',true);
				AddValueToParamList(strObjName,objparamValue);
			}*/
		}
	}

	if(myParamList.size())
	{
		NotifyDelegateWithParamList();
	}
	
	return true;
}

template <>
bool JSonFileParserBase<usString, US16ParserUtils>::ParseBlock(US16ParserUtils& Block)
{
	// retreive obj name first (quoted word followed by ':')
	US16ParserUtils objName(Block);
	while (Block.GetWord(objName, ':', true))
	{
		// check that it's a valid name
		if ((objName[0] == '"') && (objName[objName.length() - 1] == '"'))
		{
			// remove quotes
			usString strObjName(objName.subString(1, objName.length() - 2));
			US16ParserUtils objValue(Block);
			Block.GetTrailingPart(objValue, true);
			// check value type
			if (objValue[0] == '[')
			{
				if (myParamList.size())
				{
					NotifyDelegateWithParamList();
				}
				// array
				US16ParserUtils newarray(Block);
				if (Block.GetBlockExcludeString(newarray, '[', ']'))
				{
					kstl::vector<CoreModifiableAttribute*>	params;
					maUSString* arrayname = new maUSString(*myDelegateObject, false, LABEL_AND_ID(ArrayName), strObjName);
					params.push_back(arrayname);

					myDelegateObject->CallMethod(myJSonArrayStartID, params);

					if (!ParseArray(newarray))
					{
						return false;
					}

					myDelegateObject->CallMethod(myJSonArrayEndID, params);

					delete arrayname;

					Block.GoAfterNextSeparator(',');
				}
				else
				{
					return false;
				}
			}
			else if (objValue[0] == '{')
			{
				// object
				if (myParamList.size())
				{
					NotifyDelegateWithParamList();
				}
				US16ParserUtils newblock(Block);
				if (Block.GetBlockExcludeString(newblock, '{', '}'))
				{
					kstl::vector<CoreModifiableAttribute*>	params;
					maUSString* neobjname = new maUSString(*myDelegateObject, false, LABEL_AND_ID(ObjectName), strObjName);
					params.push_back(neobjname);

					myDelegateObject->CallMethod(myJSonObjectStartID, params);

					if (!ParseBlock(newblock))
					{
						return false;
					}

					myDelegateObject->CallMethod(myJSonObjectEndID, params);

					delete neobjname;

					Block.GoAfterNextSeparator(',');
				}
				else
				{
					return false;
				}
			}
			else if (objValue[0] == '"')
			{
				// string 
				usString objparamValue("");
				{
					US16ParserUtils objparamValueblk(Block);
					Block.GetQuotationWord(objparamValueblk);
					objparamValue = (const usString&)objparamValueblk;
				}
				Block.GoAfterNextSeparator(',');
				Block.GoToNextNonWhitespace();
				AddValueToParamList(strObjName, objparamValue);
			}
			else // numeric or bool
			{
				US16ParserUtils objparamValue(Block);
				Block.GetWord(objparamValue, ',', true);
				AddValueToParamList(strObjName, objparamValue);
			}
		}
	}
	
	if (myParamList.size())
	{
		NotifyDelegateWithParamList();
	}
	
	return true;
}


template <typename stringType, typename parserType>
bool JSonFileParserBase<stringType,parserType>::ParseArray(parserType& Array)
{
	// retreive array values
	while(Array.GoToNextNonWhitespace())
	{
		if(*Array == '[')
		{
			if(myParamList.size())
			{
				NotifyDelegateWithParamList();
			}
			// array
			parserType newarray(Array);
			if (Array.GetBlockExcludeString(newarray, '[', ']'))
			{
				kstl::vector<CoreModifiableAttribute*>	params;
				myDelegateObject->CallMethod(myJSonArrayStartID,params);

				if(!ParseArray(newarray))
				{
					return false;
				}
				myDelegateObject->CallMethod(myJSonArrayEndID,params);

				Array.GoAfterNextSeparator(',');
			}
			else
			{
				return false;
			}
		}
		else if(*Array == '{')
		{
			if(myParamList.size())
			{
				NotifyDelegateWithParamList();
			}
			// object
			parserType newblock(Array);
			if (Array.GetBlockExcludeString(newblock, '{', '}'))
			{
				kstl::vector<CoreModifiableAttribute*>	params;
				myDelegateObject->CallMethod(myJSonObjectStartID,params);

				if(!ParseBlock(newblock))
				{
					return false;
				}
				myDelegateObject->CallMethod(myJSonObjectEndID,params);
				Array.GoAfterNextSeparator(',');
			}
			else
			{
				return false;
			}
		}
		else if (*Array == '"')
		{
			// string 
			stringType	objparamValue("");
			{
				parserType objparamValueblk(Array);
				Array.GetQuotationWord(objparamValueblk);
				objparamValue=(const stringType&)objparamValueblk;
			}
			Array.GoAfterNextSeparator(',');
			Array.GoToNextNonWhitespace();
			AddValueToParamList("", objparamValue);
		}
		else // numeric or bool
		{
			parserType objparamValue(Array);
			Array.GetWord(objparamValue, ',', true);
			AddValueToParamList("", objparamValue);
		}

	/*	else
		{
			// string numeric or bool
			parserType objparamValue(Array);
			Array.GetWord(objparamValue,',',true);
			AddValueToParamList("",objparamValue);
		}*/
	}
	
	if(myParamList.size())
	{
		NotifyDelegateWithParamList();
	}
	
	return true;
}

template <typename stringType, typename parserType>
void	JSonFileParserBase<stringType,parserType>::NotifyDelegateWithParamList()
{
	myDelegateObject->CallMethod(myJSonParamListID,myParamList);

	kstl::vector<CoreModifiableAttribute*>::iterator	todelete;
	for(todelete=myParamList.begin();todelete!=myParamList.end();++todelete)
	{
		delete (*todelete);
	}

	myParamList.clear();
}

																					/*DictionaryFromJson class*/
//-----------------------------------------------------------------------------------------------------
//Constructor

IMPLEMENT_CLASS_INFO(DictionaryFromJson);

DictionaryFromJson::DictionaryFromJson(const kstl::string& name,CLASS_NAME_TREE_ARG):
CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG)
,m_pCurrentObject(NULL)
{
	CONSTRUCT_METHOD(DictionaryFromJson, JSonObjectStart)
	CONSTRUCT_METHOD(DictionaryFromJson, JSonObjectEnd)
	CONSTRUCT_METHOD(DictionaryFromJson, JSonArrayStart)
	CONSTRUCT_METHOD(DictionaryFromJson, JSonArrayEnd)
	CONSTRUCT_METHOD(DictionaryFromJson, JSonParamList)

	//m_pCurrentObject = new CoreMap<kstl::string>();
	//m_vObjectStack.push_back(m_pCurrentObject);  // push null

}

//-----------------------------------------------------------------------------------------------------
//Destructor

DictionaryFromJson::~DictionaryFromJson()
{

	if(m_pCurrentObject)
	{
		m_pCurrentObject->Destroy();
	}

	m_vObjectStack.clear();
	m_pCurrentObject = NULL;
}

//-----------------------------------------------------------------------------------------------------
//Call Back

DEFINE_METHOD(DictionaryFromJson,JSonObjectStart)
{
	CoreMap<kstl::string>* toAdd = new CoreMap<kstl::string>();

	if (m_pCurrentObject)
	{
		if (m_pCurrentObject->GetType() == CoreItem::COREMAP)
		{
			((CoreMap<kstl::string>*)m_pCurrentObject)->set(((maString*)params[0])->c_str(), toAdd);
			toAdd->Destroy();
		}
		else if (m_pCurrentObject->GetType() == CoreItem::COREVECTOR)
		{
			((CoreVector*)m_pCurrentObject)->push_back(toAdd);
			toAdd->Destroy();
		}
	}
	else // first object => add it twice
	{
		m_pCurrentObject = toAdd;
		m_vObjectStack.push_back(m_pCurrentObject);
	}

	m_pCurrentObject=toAdd;
	m_vObjectStack.push_back(m_pCurrentObject);
	
	return false;
}

DEFINE_METHOD(DictionaryFromJson,JSonObjectEnd)
{
	if(m_vObjectStack.size()>0)
	{
		m_vObjectStack.pop_back();
		m_pCurrentObject = m_vObjectStack[m_vObjectStack.size()-1];
	}
	else
	{
		// ERROR stack can not be empty
		KIGS_ERROR("JSon parse error\n",1);
	}

	return false;
}

DEFINE_METHOD(DictionaryFromJson,JSonArrayStart)
{
	CoreVector* toAdd = new CoreVector();

	if (m_pCurrentObject)
	{
		if (m_pCurrentObject->GetType() == CoreItem::COREMAP)
		{
			((CoreMap<kstl::string>*)m_pCurrentObject)->set(((maString*)params[0])->c_str(), toAdd);
			toAdd->Destroy();
		}
		else if (m_pCurrentObject->GetType() == CoreItem::COREVECTOR)
		{
			((CoreVector*)m_pCurrentObject)->push_back(toAdd);
			toAdd->Destroy();
		}
	}
	else // first object => add it twice
	{
		m_pCurrentObject = toAdd;
		m_vObjectStack.push_back(m_pCurrentObject);
	}

	m_pCurrentObject=toAdd;
	m_vObjectStack.push_back(m_pCurrentObject);

	return false;
}

DEFINE_METHOD(DictionaryFromJson,JSonArrayEnd)
{
	
	if(m_vObjectStack.size() > 0)
	{
		m_vObjectStack.pop_back();
		m_pCurrentObject = m_vObjectStack[m_vObjectStack.size()-1];
	}
	else
	{
		// ERROR stack can not be empty
		KIGS_ERROR("JSon parse error\n",1);
	}
		
	return false;
}

DEFINE_METHOD(DictionaryFromJson,JSonParamList)
{
	
	if(!params.empty())
	{
		bool L_IsVector = false;
		if(m_pCurrentObject)
		{
			L_IsVector = (m_pCurrentObject->GetType() == CoreItem::COREVECTOR);
		}

		int idx = 0;
		for(unsigned int i = 0; i < params.size(); i+=2)
		{
			kstl::string L_Key = (*(maString*)params[i]).const_ref();
			idx = i+1;
			if(params[idx]->getType() == STRING)
			{
				CoreValue<kstl::string>* L_Value = new CoreValue<kstl::string>((*(maString*)params[idx]));

				if(L_IsVector)
					((CoreVector*)m_pCurrentObject)->push_back(L_Value);
				else
					((CoreMap<kstl::string>*)m_pCurrentObject)->set(L_Key,L_Value);

				L_Value->Destroy();
			}
			else if(params[idx]->getType() == FLOAT)
			{
				CoreValue<kfloat>* L_Value = new CoreValue<kfloat>((*(maFloat*)params[idx]));
				
				if(L_IsVector)
					((CoreVector*)m_pCurrentObject)->push_back(L_Value);
				else
					((CoreMap<kstl::string>*)m_pCurrentObject)->set(L_Key,L_Value);
				
				L_Value->Destroy();
			}
			else if(params[idx]->getType() == INT)
			{
				CoreValue<int>* L_Value = new CoreValue<int>((*(maInt*)params[idx]));

				
				if(L_IsVector)
					((CoreVector*)m_pCurrentObject)->push_back(L_Value);
				else
					((CoreMap<kstl::string>*)m_pCurrentObject)->set(L_Key,L_Value);
			
				
				L_Value->Destroy();
			}
			else if(params[idx]->getType() == BOOL)
			{
				CoreValue<bool>* L_Value = new CoreValue<bool>((*(maBool*)params[idx]));

				
				if(L_IsVector)
					((CoreVector*)m_pCurrentObject)->push_back(L_Value);
				else
					((CoreMap<kstl::string>*)m_pCurrentObject)->set(L_Key,L_Value);
				
				
				L_Value->Destroy();
			}
			else if(params[idx]->getType() == UINT)
			{
				CoreValue<unsigned int>* L_Value = new CoreValue<unsigned int>((*(maUInt*)params[idx]));

				if(L_IsVector)
					((CoreVector*)m_pCurrentObject)->push_back(L_Value);
				else
					((CoreMap<kstl::string>*)m_pCurrentObject)->set(L_Key,L_Value);
				
				L_Value->Destroy();
			}
			else if(params[idx]->getType() == DOUBLE)
			{
				CoreValue<double>* L_Value = new CoreValue<double>((*(maDouble*)params[idx]));

				if(L_IsVector)
					((CoreVector*)m_pCurrentObject)->push_back(L_Value);
				else
					((CoreMap<kstl::string>*)m_pCurrentObject)->set(L_Key,L_Value);
				
				L_Value->Destroy();
			}
		}
	}
		
	return false;
}

IMPLEMENT_CLASS_INFO(DictionaryFromJsonUTF16);

DictionaryFromJsonUTF16::DictionaryFromJsonUTF16(const kstl::string& name, CLASS_NAME_TREE_ARG) :
CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG)
, m_pCurrentObject(NULL)
{
	CONSTRUCT_METHOD(DictionaryFromJsonUTF16, JSonObjectStart)
	CONSTRUCT_METHOD(DictionaryFromJsonUTF16, JSonObjectEnd)
	CONSTRUCT_METHOD(DictionaryFromJsonUTF16, JSonArrayStart)
	CONSTRUCT_METHOD(DictionaryFromJsonUTF16, JSonArrayEnd)
	CONSTRUCT_METHOD(DictionaryFromJsonUTF16, JSonParamList)
	//m_pCurrentObject = new CoreMap<usString>();
	//m_vObjectStack.push_back(m_pCurrentObject); 
	
}

//-----------------------------------------------------------------------------------------------------
//Destructor

DictionaryFromJsonUTF16::~DictionaryFromJsonUTF16()
{

	if (m_pCurrentObject)
	{
		m_pCurrentObject->Destroy();
	}

	m_vObjectStack.clear();
	m_pCurrentObject = NULL;
}

//-----------------------------------------------------------------------------------------------------
//Call Back

DEFINE_METHOD(DictionaryFromJsonUTF16, JSonObjectStart)
{
	CoreMap<usString>* toAdd = new CoreMap<usString>();

	if (m_pCurrentObject)
	{
		if (m_pCurrentObject->GetType() == CoreItem::COREMAP)
		{
			((CoreMap<usString>*)m_pCurrentObject)->set(*((maUSString*)params[0]), toAdd);
			toAdd->Destroy();
		}
		else if (m_pCurrentObject->GetType() == CoreItem::COREVECTOR)
		{
			((CoreVector*)m_pCurrentObject)->push_back(toAdd);
			toAdd->Destroy();
		}
	}
	else // first object => add it twice
	{
		m_pCurrentObject = toAdd;
		m_vObjectStack.push_back(m_pCurrentObject);
	}

	m_pCurrentObject = toAdd;
	m_vObjectStack.push_back(m_pCurrentObject);

	return false;
}

DEFINE_METHOD(DictionaryFromJsonUTF16, JSonObjectEnd)
{
	if (m_vObjectStack.size()>0)
	{
		m_vObjectStack.pop_back();
		m_pCurrentObject = m_vObjectStack[m_vObjectStack.size() - 1];
	}
	else
	{
		// ERROR stack can not be empty
		KIGS_ERROR("JSon parse error\n", 1);
	}

	return false;
}

DEFINE_METHOD(DictionaryFromJsonUTF16, JSonArrayStart)
{
	CoreVector* toAdd = new CoreVector();

	if (m_pCurrentObject)
	{
		if (m_pCurrentObject->GetType() == CoreItem::COREMAP)
		{
			((CoreMap<usString>*)m_pCurrentObject)->set(*((maUSString*)params[0]), toAdd);
			toAdd->Destroy();
		}
		else if (m_pCurrentObject->GetType() == CoreItem::COREVECTOR)
		{
			((CoreVector*)m_pCurrentObject)->push_back(toAdd);
			toAdd->Destroy();
		}
	}
	else // first object => add it twice
	{
		m_pCurrentObject = toAdd;
		m_vObjectStack.push_back(m_pCurrentObject);
	}

	m_pCurrentObject = toAdd;
	m_vObjectStack.push_back(m_pCurrentObject);

	return false;
}

DEFINE_METHOD(DictionaryFromJsonUTF16, JSonArrayEnd)
{

	if (m_vObjectStack.size() > 0)
	{
		m_vObjectStack.pop_back();
		m_pCurrentObject = m_vObjectStack[m_vObjectStack.size() - 1];
	}
	else
	{
		// ERROR stack can not be empty
		KIGS_ERROR("JSon parse error\n", 1);
	}

	return false;
}

DEFINE_METHOD(DictionaryFromJsonUTF16, JSonParamList)
{

	if (!params.empty())
	{
		bool L_IsVector = false;
		if (m_pCurrentObject)
		{
			L_IsVector = (m_pCurrentObject->GetType() == CoreItem::COREVECTOR);
		}

		int idx = 0;
		for (unsigned int i = 0; i < params.size(); i += 2)
		{
			usString L_Key = (const usString&)(*(maUSString*)params[i]);
			idx = i + 1;
			if (params[idx]->getType() == STRING)
			{
				CoreValue<kstl::string>* L_Value = new CoreValue<kstl::string>((*(maString*)params[idx]).const_ref());

				if (L_IsVector)
					((CoreVector*)m_pCurrentObject)->push_back(L_Value);
				else
					((CoreMap<kstl::string>*)m_pCurrentObject)->set(L_Key.ToString(), L_Value);

				L_Value->Destroy();
			}
			else if (params[idx]->getType() == USSTRING)
			{
				CoreValue<usString>* L_Value = new CoreValue<usString>((*(maUSString*)params[idx]));

				if (L_IsVector)
					((CoreVector*)m_pCurrentObject)->push_back(L_Value);
				else
					((CoreMap<usString>*)m_pCurrentObject)->set(L_Key, L_Value);

				L_Value->Destroy();
			}
			else if (params[idx]->getType() == FLOAT)
			{
				CoreValue<kfloat>* L_Value = new CoreValue<kfloat>((*(maFloat*)params[idx]));

				if (L_IsVector)
					((CoreVector*)m_pCurrentObject)->push_back(L_Value);
				else
					((CoreMap<usString>*)m_pCurrentObject)->set(L_Key, L_Value);

				L_Value->Destroy();
			}
			else if (params[idx]->getType() == INT)
			{
				CoreValue<int>* L_Value = new CoreValue<int>((*(maInt*)params[idx]));


				if (L_IsVector)
					((CoreVector*)m_pCurrentObject)->push_back(L_Value);
				else
					((CoreMap<usString>*)m_pCurrentObject)->set(L_Key, L_Value);


				L_Value->Destroy();
			}
			else if (params[idx]->getType() == BOOL)
			{
				CoreValue<bool>* L_Value = new CoreValue<bool>((*(maBool*)params[idx]));


				if (L_IsVector)
					((CoreVector*)m_pCurrentObject)->push_back(L_Value);
				else
					((CoreMap<usString>*)m_pCurrentObject)->set(L_Key, L_Value);


				L_Value->Destroy();
			}
			else if (params[idx]->getType() == UINT)
			{
				CoreValue<unsigned int>* L_Value = new CoreValue<unsigned int>((*(maUInt*)params[idx]));

				if (L_IsVector)
					((CoreVector*)m_pCurrentObject)->push_back(L_Value);
				else
					((CoreMap<usString>*)m_pCurrentObject)->set(L_Key, L_Value);

				L_Value->Destroy();
			}
			else if (params[idx]->getType() == DOUBLE)
			{
				CoreValue<double>* L_Value = new CoreValue<double>((*(maDouble*)params[idx]));

				if (L_IsVector)
					((CoreVector*)m_pCurrentObject)->push_back(L_Value);
				else
					((CoreMap<usString>*)m_pCurrentObject)->set(L_Key, L_Value);

				L_Value->Destroy();
			}
		}
	}

	return false;
}

// force method creation
template class JSonFileParserBase< kstl::string, AsciiParserUtils >;
template class JSonFileParserBase< usString , US16ParserUtils>;