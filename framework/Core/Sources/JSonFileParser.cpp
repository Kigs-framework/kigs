#include "PrecompiledHeaders.h"
#include "JSonFileParser.h"
#include "FilePathManager.h"
#include "ModuleFileManager.h"
#include "CoreModifiableAttribute.h"
#include "maUSString.h"
#include "CoreMap.h"

template <typename stringType, typename parserType>
JSonFileParserBase<stringType,parserType>::JSonFileParserBase(const kstl::string& filename, CoreModifiable*	delegateObject) :
	myDelegateObject(delegateObject, GetRefTag{})
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
	auto& pathManager = KigsCore::Singleton<FilePathManager>();

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
CoreModifiableAttribute* JSonFileParserBase<kstl::string, AsciiParserUtils>::getNewStringAttribute(const kstl::string& attrName,const kstl::string& strObjName)
{
	return new maString(*myDelegateObject.get(), false, attrName, strObjName);
}

template <>
CoreModifiableAttribute* JSonFileParserBase<usString, US16ParserUtils>::getNewStringAttribute(const usString& attrName,const usString& strObjName)
{
	kstl::string name = attrName.ToString();
	return new maUSString(*myDelegateObject.get(), false, name, strObjName);
}

template <>
void 	JSonFileParserBase<kstl::string, AsciiParserUtils>::AddValueToParamList(kstl::string strObjName, kstl::string objparamValue)
{
	CoreModifiableAttribute* Value;
	kstl::string	strvalue = objparamValue;

	myParamList.push_back(new maString(*myDelegateObject.get(), false, strObjName, strObjName));

	// check if string, numeric or boolean
	if (strvalue[0] == '"')
	{
		// string
		// remove quotes
		kstl::string paramValue = objparamValue.substr(1, objparamValue.length() - 2);
		Value = new maString(*myDelegateObject.get(), false, strObjName, paramValue);
	}
	else if ((strvalue == "true") || (strvalue == "false"))
	{
		// boolean
		Value = new maBool(*myDelegateObject.get(), false, strObjName, false);
		Value->setValue(strvalue);
	}
	else
	{
		if (strvalue.find('.') != std::string::npos)
		{
			// float
			Value = new maFloat(*myDelegateObject.get(), false, strObjName, KFLOAT_ZERO);
		}
		else
		{
			// int
			// check negative
			if (strvalue.find('-') != std::string::npos)
			{
				Value = new maInt(*myDelegateObject.get(), false, strObjName, 0);
			}
			else
			{
				Value = new maUInt(*myDelegateObject.get(), false, strObjName, 0);
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

	myParamList.push_back(new maUSString(*myDelegateObject.get(), false, name, strObjName));
	// check if string, numeric or boolean
	if (strvalue[0] == '"')
	{
		// string
		// remove quotes
		usString paramValue = objparamValue.substr(1, objparamValue.length() - 2);
		Value = new maUSString(*myDelegateObject.get(), false, name, paramValue);
	}
	else if ((strvalue == "true") || (strvalue == "false"))
	{
		// boolean
		Value = new maBool(*myDelegateObject.get(), false, name, false);
		Value->setValue(strvalue);
	}
	else
	{
		if (strvalue.find('.') != std::string::npos)
		{
			// float
			Value = new maFloat(*myDelegateObject.get(), false, name, KFLOAT_ZERO);
		}
		else
		{
			// int
			Value = new maInt(*myDelegateObject.get(), false, name, 0);
		}
		Value->setValue(strvalue);
	}

	myParamList.push_back(Value);
}



template <typename stringType, typename parserType>
bool JSonFileParserBase<stringType, parserType>::Export(CoreMap<stringType>* a_value, const kstl::string& a_fileName)
{
	stringType L_Buffer("");
	RecursiveParseElement(*a_value,L_Buffer);

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
	output= stringType("");
	RecursiveParseElement(*a_value,output);
	return true;
}


template <typename stringType, typename parserType>
void JSonFileParserBase<stringType, parserType>::RecursiveParseElement(CoreItem& a_value, stringType& a_buffer)
{
	CoreItem::COREITEM_TYPE type = a_value.GetType();
	// if value, just add value and return
	if (type & CoreItem::COREVALUE)
	{
		AddValueToBuffer(a_value, a_buffer);
		return;
	}

	// write header
	if (type & CoreItem::COREMAP)
	{
		a_buffer += stringType("{");
	}
	else if (type & CoreItem::COREVECTOR)
	{
		a_buffer += stringType("[");
	}

	bool firstElem = true;
	// iterate
	CoreItemIterator It = a_value.begin();
	CoreItemIterator ItEnd = a_value.end();
	while (It != ItEnd)
	{
		// add ',' between elements
		if (!firstElem)
		{
			a_buffer += stringType(",");
			if (type & CoreItem::COREMAP)
			{
				a_buffer += stringType("\n");
			}
		}
		firstElem = false;

		// if in a map, then set name
		if (type & CoreItem::COREMAP)
		{
			a_buffer += stringType("\"");
			stringType	key;
			It.getKey(key);
			a_buffer += key;
			a_buffer += stringType("\":");
		}

		RecursiveParseElement((CoreItem&)(*It), a_buffer);

		It++;
	}

	// write footer
	if (type & CoreItem::COREMAP)
	{
		a_buffer += stringType("}");
	}
	else if (type & CoreItem::COREVECTOR)
	{
		a_buffer += stringType("]");
	}
}


template <typename stringType, typename parserType>
void JSonFileParserBase<stringType, parserType>::AddValueToBuffer(CoreItem& a_value, stringType& a_Destbuffer)
{
	stringType L_Value("");
	a_value.getValue(L_Value);
	if (a_value.isString())
	{
		a_Destbuffer += stringType("\"");
		a_Destbuffer += L_Value;
		a_Destbuffer += stringType("\"");
	}
	else
		a_Destbuffer += L_Value;
}

template <typename stringType, typename parserType>
CoreItemSP	JSonFileParserBase<stringType, parserType>::Get_JsonDictionary(const kstl::string& filename)
{
	//Create instance of DictionaryFromJson
	myDictionaryFromJson = CreateDictionnaryFromJSONInstance();
	myDelegateObject = myDictionaryFromJson;

	InitParser(filename);

	CoreItemSP L_TempDictionary = getDictionnary();

	myDictionaryFromJson = nullptr;

	return L_TempDictionary;
}

// specialized

template <>
CMSP			JSonFileParserBase<kstl::string, AsciiParserUtils>::CreateDictionnaryFromJSONInstance()
{
	return  KigsCore::GetInstanceOf("L_DictionaryFromJson", "DictionaryFromJson");
}

template <>
CoreItemSP	JSonFileParserBase< kstl::string, AsciiParserUtils>::getDictionnary()
{
	return ((DictionaryFromJson*)myDictionaryFromJson.get())->Get_Dictionary();
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
CMSP			JSonFileParserBase< usString, US16ParserUtils>::CreateDictionnaryFromJSONInstance()
{
	return KigsCore::GetInstanceOf("L_DictionaryFromJson", "DictionaryFromJsonUTF16");
}

template <>
CoreItemSP	JSonFileParserBase< usString, US16ParserUtils>::getDictionnary()
{
	return ((DictionaryFromJsonUTF16*)myDictionaryFromJson.get())->Get_Dictionary();
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
CoreItemSP	JSonFileParserBase<stringType, parserType>::Get_JsonDictionaryFromString(const stringType& jsonString)
{
	//Create instance of DictionaryFromJson
	myDictionaryFromJson = CreateDictionnaryFromJSONInstance();
	myDelegateObject = myDictionaryFromJson;

	char * copybuffer = new char[GetStringByteSize(jsonString)];
	memcpy(copybuffer, GetStringByteBuffer(jsonString), GetStringByteSize(jsonString));

	CoreRawBuffer*	Buff = new CoreRawBuffer((void*)copybuffer, GetStringByteSize(jsonString));

	InitParserFromString(Buff);

	Buff->Destroy();

	CoreItemSP L_TempDictionary = getDictionnary();

	myDictionaryFromJson = nullptr;
	
	return L_TempDictionary;

}

template <typename stringType, typename parserType>
bool JSonFileParserBase<stringType, parserType>::ParseBlock(parserType& Block)
{
	// retreive obj name first (quoted word followed by ':')
	parserType objName(Block);
	while(Block.GetWord(objName,':',true))
	{
		// check that it's a valid name
		if((objName[0]=='"')&&(objName[objName.length()-1]=='"'))
		{
			// remove quotes
			stringType strObjName = objName.subString(1, objName.length() - 2);

			parserType objValue(Block);
			Block.GetTrailingPart(objValue,true);
			// check value type
			if(objValue[0] == '[')
			{
				if(myParamList.size())
				{
					NotifyDelegateWithParamList();
				}
				// array
				parserType newarray(Block);
				if (Block.GetBlockExcludeString(newarray, '[', ']'))
				{
					kstl::vector<CoreModifiableAttribute*>	params;
					CoreModifiableAttribute* arrayname= getNewStringAttribute(stringType("ArrayName"),strObjName);
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
				parserType newblock(Block);
				if (Block.GetBlockExcludeString(newblock, '{', '}'))
				{
					kstl::vector<CoreModifiableAttribute*>	params;
					CoreModifiableAttribute* neobjname = getNewStringAttribute(stringType("ObjectName"), strObjName);
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
				stringType objparamValue;
				{
					parserType objparamValuebkl(Block);
					Block.GetQuotationWord(objparamValuebkl);
					objparamValue = (const stringType&)objparamValuebkl;
				}
				Block.GoAfterNextSeparator(',');
				Block.GoToNextNonWhitespace();
				AddValueToParamList(strObjName, objparamValue);
			}
			else // numeric or bool
			{
				parserType objparamValue(Block);
				Block.GetWord(objparamValue, ',', true);
				AddValueToParamList(strObjName, objparamValue);
			}
		}
	}

	if(myParamList.size())
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
			AddValueToParamList(stringType(""), objparamValue);
		}
		else // numeric or bool
		{
			parserType objparamValue(Array);
			Array.GetWord(objparamValue, ',', true);
			AddValueToParamList(stringType(""), objparamValue);
		}
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
,m_pCurrentObject(nullptr)
{
	
}

//-----------------------------------------------------------------------------------------------------
//Destructor

DictionaryFromJson::~DictionaryFromJson()
{
	m_vObjectStack.clear();
}

//-----------------------------------------------------------------------------------------------------
//Call Back

DEFINE_METHOD(DictionaryFromJson,JSonObjectStart)
{
	CoreItemSP toAdd = CoreItemSP((CoreItem*)new CoreMap<kstl::string>(), StealRefTag{});

	if (!m_pCurrentObject.isNil())
	{
		if (m_pCurrentObject->GetType() == CoreItem::COREMAP)
		{
			((CoreMap<kstl::string>*)m_pCurrentObject.get())->set(((maString*)params[0])->const_ref(), toAdd);
		}
		else if (m_pCurrentObject->GetType() == CoreItem::COREVECTOR)
		{
			((CoreVector*)m_pCurrentObject.get())->push_back(toAdd);
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
	CoreItemSP toAdd = CoreItemSP((CoreItem*)new CoreVector(), StealRefTag{});

	if (!m_pCurrentObject.isNil())
	{
		if (m_pCurrentObject->GetType() == CoreItem::COREMAP)
		{
			((CoreMap<kstl::string>*)m_pCurrentObject.get())->set(((maString*)params[0])->const_ref(), toAdd);
		}
		else if (m_pCurrentObject->GetType() == CoreItem::COREVECTOR)
		{
			((CoreVector*)m_pCurrentObject.get())->push_back(toAdd);
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
		if(!m_pCurrentObject.isNil())
		{
			L_IsVector = (m_pCurrentObject->GetType() == CoreItem::COREVECTOR);
		}

		int idx = 0;
		for(unsigned int i = 0; i < params.size(); i+=2)
		{
			kstl::string L_Key = (*(maString*)params[i]).const_ref();
			idx = i+1;
			if(params[idx]->getType() == ATTRIBUTE_TYPE::STRING)
			{
				CoreItemSP L_Value = CoreItemSP((CoreItem*)new CoreValue<kstl::string>((*(maString*)params[idx])), StealRefTag{});

				if(L_IsVector)
					((CoreVector*)m_pCurrentObject.get())->push_back(L_Value);
				else
					((CoreMap<kstl::string>*)m_pCurrentObject.get())->set(L_Key,L_Value);

			}
			else if(params[idx]->getType() == ATTRIBUTE_TYPE::FLOAT)
			{
				CoreItemSP L_Value = CoreItemSP((CoreItem*)new CoreValue<kfloat>((*(maFloat*)params[idx])), StealRefTag{});
				
				if(L_IsVector)
					((CoreVector*)m_pCurrentObject.get())->push_back(L_Value);
				else
					((CoreMap<kstl::string>*)m_pCurrentObject.get())->set(L_Key,L_Value);
				
			}
			else if(params[idx]->getType() == ATTRIBUTE_TYPE::INT)
			{
				CoreItemSP  L_Value = CoreItemSP((CoreItem*)new CoreValue<int>((*(maInt*)params[idx])), StealRefTag{});
				
				if(L_IsVector)
					((CoreVector*)m_pCurrentObject.get())->push_back(L_Value);
				else
					((CoreMap<kstl::string>*)m_pCurrentObject.get())->set(L_Key,L_Value);
			
			}
			else if(params[idx]->getType() == ATTRIBUTE_TYPE::BOOL)
			{
				CoreItemSP L_Value = CoreItemSP((CoreItem*)new CoreValue<bool>((*(maBool*)params[idx])), StealRefTag{});

				if(L_IsVector)
					((CoreVector*)m_pCurrentObject.get())->push_back(L_Value);
				else
					((CoreMap<kstl::string>*)m_pCurrentObject.get())->set(L_Key,L_Value);

			}
			else if(params[idx]->getType() == ATTRIBUTE_TYPE::UINT)
			{
				CoreItemSP L_Value = CoreItemSP((CoreItem*)new CoreValue<unsigned int>((*(maUInt*)params[idx])), StealRefTag{});

				if(L_IsVector)
					((CoreVector*)m_pCurrentObject.get())->push_back(L_Value);
				else
					((CoreMap<kstl::string>*)m_pCurrentObject.get())->set(L_Key,L_Value);
				
			}
			else if(params[idx]->getType() == ATTRIBUTE_TYPE::DOUBLE)
			{
				CoreItemSP L_Value = CoreItemSP((CoreItem*)new CoreValue<double>((*(maDouble*)params[idx])), StealRefTag{});

				if(L_IsVector)
					((CoreVector*)m_pCurrentObject.get())->push_back(L_Value);
				else
					((CoreMap<kstl::string>*)m_pCurrentObject.get())->set(L_Key,L_Value);
				
			}
		}
	}
		
	return false;
}

IMPLEMENT_CLASS_INFO(DictionaryFromJsonUTF16);

DictionaryFromJsonUTF16::DictionaryFromJsonUTF16(const kstl::string& name, CLASS_NAME_TREE_ARG) :
CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG)
, m_pCurrentObject(nullptr)
{
	
}

//-----------------------------------------------------------------------------------------------------
//Destructor

DictionaryFromJsonUTF16::~DictionaryFromJsonUTF16()
{
	m_vObjectStack.clear();
}

//-----------------------------------------------------------------------------------------------------
//Call Back

DEFINE_METHOD(DictionaryFromJsonUTF16, JSonObjectStart)
{
	CoreItemSP toAdd = CoreItemSP((CoreItem*)new CoreMap<usString>(), StealRefTag{});

	if (m_pCurrentObject)
	{
		if (m_pCurrentObject->GetType() == CoreItem::COREMAP)
		{
			((CoreMap<usString>*)m_pCurrentObject.get())->set(*((maUSString*)params[0]), toAdd);
		}
		else if (m_pCurrentObject->GetType() == CoreItem::COREVECTOR)
		{
			((CoreVector*)m_pCurrentObject.get())->push_back(toAdd);
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
	CoreItemSP toAdd = CoreItemSP((CoreItem*)new CoreVector(), StealRefTag{});

	if (m_pCurrentObject)
	{
		if (m_pCurrentObject->GetType() == CoreItem::COREMAP)
		{
			((CoreMap<usString>*)m_pCurrentObject.get())->set(*((maUSString*)params[0]), toAdd);
			toAdd->Destroy();
		}
		else if (m_pCurrentObject->GetType() == CoreItem::COREVECTOR)
		{
			((CoreVector*)m_pCurrentObject.get())->push_back(toAdd);
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

			CoreItemSP L_Value(nullptr);

			switch (params[idx]->getType())
			{
			case ATTRIBUTE_TYPE::STRING:
				L_Value = CoreItemSP((CoreItem*)new CoreValue<kstl::string>((*(maString*)params[idx]).const_ref()), StealRefTag{});
				break;
			case ATTRIBUTE_TYPE::USSTRING:
				L_Value = CoreItemSP((CoreItem*)new CoreValue<usString>((*(maUSString*)params[idx])), StealRefTag{});
				break;
			case ATTRIBUTE_TYPE::FLOAT:
				L_Value = CoreItemSP((CoreItem*)new CoreValue<kfloat>((*(maFloat*)params[idx])), StealRefTag{});
				break;
			case ATTRIBUTE_TYPE::INT:
				L_Value = CoreItemSP((CoreItem*)new CoreValue<int>((*(maInt*)params[idx])), StealRefTag{});
				break;
			case ATTRIBUTE_TYPE::BOOL:
				L_Value = CoreItemSP((CoreItem*)new CoreValue<bool>((*(maBool*)params[idx])), StealRefTag{});
				break;
			case ATTRIBUTE_TYPE::UINT:
				L_Value = CoreItemSP((CoreItem*)new CoreValue<unsigned int>((*(maUInt*)params[idx])), StealRefTag{});
				break;
			case ATTRIBUTE_TYPE::DOUBLE:
				L_Value = CoreItemSP((CoreItem*)new CoreValue<double>((*(maDouble*)params[idx])), StealRefTag{});
				break;
			}

			if (!L_Value.isNil())
			{
				if (L_IsVector)
					((CoreVector*)m_pCurrentObject.get())->push_back(L_Value);
				else
					((CoreMap<usString>*)m_pCurrentObject.get())->set(L_Key, L_Value);
			}
		}
	}

	return false;
}

// force method creation
template class JSonFileParserBase< kstl::string, AsciiParserUtils >;
template class JSonFileParserBase< usString , US16ParserUtils>;