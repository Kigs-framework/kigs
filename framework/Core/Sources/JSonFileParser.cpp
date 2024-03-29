#include "PrecompiledHeaders.h"
#include "JSonFileParser.h"
#include "FilePathManager.h"
#include "ModuleFileManager.h"
#include "CoreModifiableAttribute.h"
#include "maUSString.h"
#include "CoreMap.h"
#include "CoreVector.h"

using namespace Kigs::Core;
using namespace Kigs::File;

template <typename stringType, typename parserType>
JSonFileParserBase<stringType,parserType>::JSonFileParserBase(const std::string& filename, CoreModifiable*	delegateObject) :
	mDelegateObject(delegateObject->SharedFromThis())
{
	InitParser(filename);
}

template <typename stringType, typename parserType>
JSonFileParserBase<stringType,parserType>::JSonFileParserBase()
{
}

template <typename stringType, typename parserType>
void JSonFileParserBase<stringType,parserType>::InitParserFromString(const SP<CoreRawBuffer>& Buff)
{
	// init often used IDs
	mJSonObjectStartID=CharToID::GetID("JSonObjectStart");
	mJSonObjectEndID=CharToID::GetID("JSonObjectEnd");
	mJSonArrayStartID=CharToID::GetID("JSonArrayStart");
	mJSonArrayEndID=CharToID::GetID("JSonArrayEnd");
	mJSonParamListID=CharToID::GetID("JSonParamList");

	parserType	asciiParser(Buff);
	parserType	mainBlock(Buff);

	// check if first character is a { or a [

	asciiParser.GoToNextNonWhitespace();
	if (*asciiParser == '{')
	{
		if (asciiParser.GetBlockExcludeString(mainBlock, '{', '}'))
		{
			mParamList.clear();

			mDelegateObject->CallMethod(mJSonObjectStartID, mParamList);

			if (!ParseBlock(mainBlock))
			{
				KIGS_ERROR("JSon Parser : malformed json block ", 1);
			}

			mDelegateObject->CallMethod(mJSonArrayEndID, mParamList);
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
				mParamList.clear();
				mDelegateObject->CallMethod(mJSonArrayStartID, mParamList);

				if (!ParseArray(mainBlock))
				{
					KIGS_ERROR("JSon Parser : malformed json block ", 1);
				}

				mDelegateObject->CallMethod(mJSonArrayEndID, mParamList);
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
void JSonFileParserBase<stringType,parserType>::InitParser(const std::string& filename)
{
	auto pathManager = KigsCore::Singleton<FilePathManager>();

	SmartPointer<::FileHandle> fullfilenamehandle=pathManager->FindFullName(filename);

	u64 size;
	auto Buff = ModuleFileManager::LoadFileAsCharString(fullfilenamehandle.get(),size, GetStringCharSize());
	if(Buff)
	{
		InitParserFromString(Buff);
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
CoreModifiableAttribute* JSonFileParserBase<std::string, AsciiParserUtils>::getNewStringAttribute(const std::string& attrName,const std::string& strObjName)
{
	return new maString(*mDelegateObject.get(), attrName, strObjName);
}

template <>
CoreModifiableAttribute* JSonFileParserBase<usString, US16ParserUtils>::getNewStringAttribute(const usString& attrName,const usString& strObjName)
{
	std::string name = attrName.ToString();
	return new maUSString(*mDelegateObject.get(), name, strObjName);
}

template <>
void 	JSonFileParserBase<std::string, AsciiParserUtils>::AddValueToParamList(const std::string& strObjName,const std::string& objparamValue)
{
	CoreModifiableAttribute* Value;
	std::string	strvalue = objparamValue;

	mParamList.push_back(new maString(*mDelegateObject.get(), strObjName, strObjName));

	// check if string, numeric or boolean
	if (strvalue[0] == '"')
	{
		// string
		// remove quotes
		std::string paramValue = objparamValue.substr(1, objparamValue.length() - 2);
		Value = new maString(*mDelegateObject.get(), strObjName, paramValue);
	}
	else if ((strvalue == "true") || (strvalue == "false"))
	{
		// boolean
		Value = new maBool(*mDelegateObject.get(), strObjName, false);
		Value->setValue(strvalue, mDelegateObject.get());
	}
	else
	{
		if (strvalue.find('.') != std::string::npos)
		{
			// float
			Value = new maFloat(*mDelegateObject.get(), strObjName, 0.0f);
		}
		else
		{
			// fast check u32 / u64
			if (strvalue.length() <= 9)
			{
				// int
				// check negative
				if (strvalue.find('-') != std::string::npos)
				{
					Value = new maInt(*mDelegateObject.get(), strObjName, 0);
				}
				else
				{
					Value = new maUInt(*mDelegateObject.get(), strObjName, 0);
				}
			}
			else
			{
				// int64
				// check negative
				if (strvalue.find('-') != std::string::npos)
				{
					Value = new maLong(*mDelegateObject.get(), strObjName, 0);
				}
				else
				{
					Value = new maULong(*mDelegateObject.get(), strObjName, 0);
				}
			}
		}

		Value->setValue(strvalue, mDelegateObject.get());
	}

	mParamList.push_back(Value);
}

template <>
void 	JSonFileParserBase<usString, US16ParserUtils>::AddValueToParamList(const usString& strObjName, const usString& objparamValue)
{
	CoreModifiableAttribute* Value;
	usString	strvalue = objparamValue;

	std::string name = strObjName.ToString();

	mParamList.push_back(new maUSString(*mDelegateObject.get(), name, strObjName));
	// check if string, numeric or boolean
	if (strvalue[0] == '"')
	{
		// string
		// remove quotes
		usString paramValue = objparamValue.substr(1, objparamValue.length() - 2);
		Value = new maUSString(*mDelegateObject.get(), name, paramValue);
	}
	else if ((strvalue == "true") || (strvalue == "false"))
	{
		// boolean
		Value = new maBool(*mDelegateObject.get(), name, false);
		Value->setValue(strvalue, mDelegateObject.get());
	}
	else
	{
		if (strvalue.find('.') != std::string::npos)
		{
			// float
			Value = new maFloat(*mDelegateObject.get(), name, 0.0f);
		}
		else
		{
			// fast check u32 / u64
			if (strvalue.length() <= 9)
			{
				// int
				// check negative
				if (strvalue.find('-') != std::string::npos)
				{
					Value = new maInt(*mDelegateObject.get(), name, 0);
				}
				else
				{
					Value = new maUInt(*mDelegateObject.get(), name, 0);
				}
			}
			else
			{
				// int64
				// check negative
				if (strvalue.find('-') != std::string::npos)
				{
					Value = new maLong(*mDelegateObject.get(), name, 0);
				}
				else
				{
					Value = new maULong(*mDelegateObject.get(), name, 0);
				}
			}
		}
		Value->setValue(strvalue, mDelegateObject.get());
	}

	mParamList.push_back(Value);
}



template <typename stringType, typename parserType>
bool JSonFileParserBase<stringType, parserType>::Export(CoreMap<stringType>* a_value, const std::string& a_fileName)
{
	stringType L_Buffer("");
	RecursiveParseElement(*a_value,L_Buffer);

	SmartPointer<::FileHandle> L_File = Platform_fopen(a_fileName.c_str(), "wb");
	if(L_File->mFile)
	{
		// export buffer 
		Platform_fwrite(GetStringByteBuffer(L_Buffer), 1, GetStringByteSize(L_Buffer)-GetStringCharSize(), L_File.get());
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
CoreItemSP	JSonFileParserBase<stringType, parserType>::Get_JsonDictionary(const std::string& filename)
{
	//Create instance of DictionaryFromJson
	mDictionaryFromJson = CreateDictionnaryFromJSONInstance();
	mDelegateObject = mDictionaryFromJson;

	InitParser(filename);

	CoreItemSP L_TempDictionary = getDictionnary();

	mDictionaryFromJson = nullptr;

	return L_TempDictionary;
}

template <typename stringType, typename parserType>
CoreItemSP	JSonFileParserBase<stringType, parserType>::Get_JsonDictionary(SmartPointer<::FileHandle> filehandle)
{
	//Create instance of DictionaryFromJson
	mDictionaryFromJson = CreateDictionnaryFromJSONInstance();
	mDelegateObject = mDictionaryFromJson;

	u64 size;
	auto Buff = ModuleFileManager::LoadFileAsCharString(filehandle.get(), size, GetStringCharSize());
	if (Buff)
	{
		InitParserFromString(Buff);
	}
#ifdef _DEBUG
	else
	{
		printf("FILE NOT FOUND : %s\n", filehandle->mFileName.c_str());
	}
#endif

	CoreItemSP L_TempDictionary = getDictionnary();

	mDictionaryFromJson = nullptr;

	return L_TempDictionary;
}

// specialized

template <>
CMSP			JSonFileParserBase<std::string, AsciiParserUtils>::CreateDictionnaryFromJSONInstance()
{
	return  KigsCore::GetInstanceOf("L_DictionaryFromJson", "DictionaryFromJson");
}

template <>
CoreItemSP	JSonFileParserBase< std::string, AsciiParserUtils>::getDictionnary()
{
	return ((DictionaryFromJson*)mDictionaryFromJson.get())->Get_Dictionary();
}


template <>
int	JSonFileParserBase<std::string, AsciiParserUtils>::GetStringByteSize(const std::string& tocheck)
{
	// add trailing 0
	return (int)(tocheck.size()+1);
}

template <>
int	JSonFileParserBase<std::string, AsciiParserUtils>::GetStringCharSize()
{
	return 1;
}

template <>
const unsigned char*	JSonFileParserBase<std::string, AsciiParserUtils>::GetStringByteBuffer(const std::string& tocheck)
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
	return ((DictionaryFromJsonUTF16*)mDictionaryFromJson.get())->Get_Dictionary();
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
	mDictionaryFromJson = CreateDictionnaryFromJSONInstance();
	mDelegateObject = mDictionaryFromJson;

	unsigned int byteSize = GetStringByteSize(jsonString);

	char * copybuffer = new char[byteSize];
	memcpy(copybuffer, GetStringByteBuffer(jsonString), byteSize);
	// zero ended
	for (int i = 1; i <= GetStringCharSize(); i++)
	{
		copybuffer[byteSize - i] = 0;
	}
	auto Buff = MakeRefCounted<CoreRawBuffer>((void*)copybuffer, byteSize);
	InitParserFromString(Buff);
	CoreItemSP L_TempDictionary = getDictionnary();
	mDictionaryFromJson = nullptr;
	
	return L_TempDictionary;

}

template <typename stringType, typename parserType>
bool JSonFileParserBase<stringType, parserType>::ParseBlock(parserType& Block)
{
	// retreive mObj name first (quoted word followed by ':')
	parserType objName(Block);
	while(Block.GetQuotationWord(objName))
	{
		// check that it's a valid name
		if((objName[0]=='"')&&(objName[objName.length()-1]=='"'))
		{
			// remove quotes
			stringType strObjName = objName.subString(1, (int)objName.length() - 2);
			// remove object name so it doesn't interact with Block anymore
			objName.Clear();
			Block.GoToNextNonWhitespace();
			stringType isDoubleDot(" ");
			Block.ReadChar(isDoubleDot[0]);
			if (isDoubleDot[0] != ':')
				return false;

			parserType objValue(Block);
			Block.GetTrailingPart(objValue,true);
			// check value type
			if(objValue[0] == '[')
			{
				if(mParamList.size())
				{
					NotifyDelegateWithParamList();
				}
				// array
				parserType newarray(Block);
				if (Block.GetBlockExcludeString(newarray, '[', ']'))
				{
					std::vector<CoreModifiableAttribute*>	params;
					CoreModifiableAttribute* arrayname= getNewStringAttribute(stringType("ArrayName"),strObjName);
					params.push_back(arrayname);

					mDelegateObject->CallMethod(mJSonArrayStartID,params);

					if(!ParseArray(newarray))
					{
						return false;
					}

					mDelegateObject->CallMethod(mJSonArrayEndID,params);
					
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
				if(mParamList.size())
				{
					NotifyDelegateWithParamList();
				}
				parserType newblock(Block);
				if (Block.GetBlockExcludeString(newblock, '{', '}'))
				{
					std::vector<CoreModifiableAttribute*>	params;
					CoreModifiableAttribute* neobjname = getNewStringAttribute(stringType("ObjectName"), strObjName);
					params.push_back(neobjname);

					mDelegateObject->CallMethod(mJSonObjectStartID,params);

					if(!ParseBlock(newblock))
					{
						return false;
					}

					mDelegateObject->CallMethod(mJSonObjectEndID,params);
					
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

	if(mParamList.size())
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
			if(mParamList.size())
			{
				NotifyDelegateWithParamList();
			}
			// array
			parserType newarray(Array);
			if (Array.GetBlockExcludeString(newarray, '[', ']'))
			{
				std::vector<CoreModifiableAttribute*>	params;
				mDelegateObject->CallMethod(mJSonArrayStartID,params);

				if(!ParseArray(newarray))
				{
					return false;
				}
				mDelegateObject->CallMethod(mJSonArrayEndID,params);

				Array.GoAfterNextSeparator(',');
			}
			else
			{
				return false;
			}
		}
		else if(*Array == '{')
		{
			if(mParamList.size())
			{
				NotifyDelegateWithParamList();
			}
			// object
			parserType newblock(Array);
			if (Array.GetBlockExcludeString(newblock, '{', '}'))
			{
				std::vector<CoreModifiableAttribute*>	params;
				mDelegateObject->CallMethod(mJSonObjectStartID,params);

				if(!ParseBlock(newblock))
				{
					return false;
				}
				mDelegateObject->CallMethod(mJSonObjectEndID,params);
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
	
	if(mParamList.size())
	{
		NotifyDelegateWithParamList();
	}
	
	return true;
}

template <typename stringType, typename parserType>
void	JSonFileParserBase<stringType,parserType>::NotifyDelegateWithParamList()
{
	mDelegateObject->CallMethod(mJSonParamListID,mParamList);

	std::vector<CoreModifiableAttribute*>::iterator	todelete;
	for(todelete=mParamList.begin();todelete!=mParamList.end();++todelete)
	{
		delete (*todelete);
	}

	mParamList.clear();
}

																					/*DictionaryFromJson class*/
//-----------------------------------------------------------------------------------------------------
//Constructor

IMPLEMENT_CLASS_INFO(DictionaryFromJson);

DictionaryFromJson::DictionaryFromJson(const std::string& name,CLASS_NAME_TREE_ARG):
CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG)
,mCurrentObject(nullptr)
{
	
}

//-----------------------------------------------------------------------------------------------------
//Destructor

DictionaryFromJson::~DictionaryFromJson()
{
	mObjectStack.clear();
}

//-----------------------------------------------------------------------------------------------------
//Call Back

DEFINE_METHOD(DictionaryFromJson,JSonObjectStart)
{
	CoreItemSP toAdd = MakeCoreMap();

	if (mCurrentObject)
	{
		if (mCurrentObject->GetType() == CoreItem::COREMAP)
		{
			((CoreMap<std::string>*)mCurrentObject.get())->set(*((maString*)params[0]), toAdd);
		}
		else if (mCurrentObject->GetType() == CoreItem::COREVECTOR)
		{
			((CoreVector*)mCurrentObject.get())->push_back(toAdd);
		}
	}
	else // first object => add it twice
	{
		mCurrentObject = toAdd;
		mObjectStack.push_back(mCurrentObject);
	}

	mCurrentObject=toAdd;
	mObjectStack.push_back(mCurrentObject);
	
	return false;
}

DEFINE_METHOD(DictionaryFromJson,JSonObjectEnd)
{
	if(mObjectStack.size()>0)
	{
		mObjectStack.pop_back();
		mCurrentObject = mObjectStack[mObjectStack.size()-1];
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
	CoreItemSP toAdd = MakeCoreVector();

	if (mCurrentObject)
	{
		if (mCurrentObject->GetType() == CoreItem::COREMAP)
		{
			((CoreMap<std::string>*)mCurrentObject.get())->set(*((maString*)params[0]), toAdd);
		}
		else if (mCurrentObject->GetType() == CoreItem::COREVECTOR)
		{
			((CoreVector*)mCurrentObject.get())->push_back(toAdd);
		}
	}
	else // first object => add it twice
	{
		mCurrentObject = toAdd;
		mObjectStack.push_back(mCurrentObject);
	}

	mCurrentObject=toAdd;
	mObjectStack.push_back(mCurrentObject);

	return false;
}

DEFINE_METHOD(DictionaryFromJson,JSonArrayEnd)
{
	
	if(mObjectStack.size() > 0)
	{
		mObjectStack.pop_back();
		mCurrentObject = mObjectStack[mObjectStack.size()-1];
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
		if(mCurrentObject)
		{
			L_IsVector = (mCurrentObject->GetType() == CoreItem::COREVECTOR);
		}

		int idx = 0;
		for(unsigned int i = 0; i < params.size(); i+=2)
		{
			std::string L_Key = (*(maString*)params[i]);
			idx = i+1;

			auto paramType = params[idx]->getType();

			if ((paramType == ATTRIBUTE_TYPE::STRING) || (paramType == ATTRIBUTE_TYPE::USSTRING))
			{
				CoreItemSP L_Value = MakeCoreValue((*(maString*)params[idx]));

				if(L_IsVector)
					((CoreVector*)mCurrentObject.get())->push_back(L_Value);
				else
					((CoreMap<std::string>*)mCurrentObject.get())->set(L_Key,L_Value);

			}
			else if(paramType == ATTRIBUTE_TYPE::FLOAT)
			{
				CoreItemSP L_Value = MakeCoreValue((*(maFloat*)params[idx]));
				
				if(L_IsVector)
					((CoreVector*)mCurrentObject.get())->push_back(L_Value);
				else
					((CoreMap<std::string>*)mCurrentObject.get())->set(L_Key,L_Value);
				
			}
			else if(paramType == ATTRIBUTE_TYPE::INT)
			{
				CoreItemSP  L_Value = MakeCoreValue((*(maInt*)params[idx]));
				
				if(L_IsVector)
					((CoreVector*)mCurrentObject.get())->push_back(L_Value);
				else
					((CoreMap<std::string>*)mCurrentObject.get())->set(L_Key,L_Value);
			
			}
			else if (paramType == ATTRIBUTE_TYPE::LONG)
			{
				CoreItemSP  L_Value = MakeCoreValue((*(maLong*)params[idx]));

				if (L_IsVector)
					((CoreVector*)mCurrentObject.get())->push_back(L_Value);
				else
					((CoreMap<std::string>*)mCurrentObject.get())->set(L_Key, L_Value);

			}
			else if(paramType == ATTRIBUTE_TYPE::BOOL)
			{
				CoreItemSP L_Value = MakeCoreValue((*(maBool*)params[idx]));

				if(L_IsVector)
					((CoreVector*)mCurrentObject.get())->push_back(L_Value);
				else
					((CoreMap<std::string>*)mCurrentObject.get())->set(L_Key,L_Value);

			}
			else if(paramType == ATTRIBUTE_TYPE::UINT)
			{
				CoreItemSP L_Value = MakeCoreValue((*(maUInt*)params[idx]));

				if(L_IsVector)
					((CoreVector*)mCurrentObject.get())->push_back(L_Value);
				else
					((CoreMap<std::string>*)mCurrentObject.get())->set(L_Key,L_Value);
				
			}
			else if (paramType == ATTRIBUTE_TYPE::ULONG)
			{
				CoreItemSP L_Value = MakeCoreValue((*(maULong*)params[idx]));

				if (L_IsVector)
					((CoreVector*)mCurrentObject.get())->push_back(L_Value);
				else
					((CoreMap<std::string>*)mCurrentObject.get())->set(L_Key, L_Value);

			}
			else if(paramType == ATTRIBUTE_TYPE::DOUBLE)
			{
				CoreItemSP L_Value = MakeCoreValue((*(maDouble*)params[idx]));

				if(L_IsVector)
					((CoreVector*)mCurrentObject.get())->push_back(L_Value);
				else
					((CoreMap<std::string>*)mCurrentObject.get())->set(L_Key,L_Value);
				
			}
		}
	}
		
	return false;
}

IMPLEMENT_CLASS_INFO(DictionaryFromJsonUTF16);

DictionaryFromJsonUTF16::DictionaryFromJsonUTF16(const std::string& name, CLASS_NAME_TREE_ARG) :
CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG)
, mCurrentObject(nullptr)
{
	
}

//-----------------------------------------------------------------------------------------------------
//Destructor

DictionaryFromJsonUTF16::~DictionaryFromJsonUTF16()
{
	mObjectStack.clear();
}

//-----------------------------------------------------------------------------------------------------
//Call Back

DEFINE_METHOD(DictionaryFromJsonUTF16, JSonObjectStart)
{
	CoreItemSP toAdd = MakeCoreMapUS();

	if (mCurrentObject)
	{
		if (mCurrentObject->GetType() == CoreItem::COREMAP)
		{
			((CoreMap<usString>*)mCurrentObject.get())->set(*((maUSString*)params[0]), toAdd);
		}
		else if (mCurrentObject->GetType() == CoreItem::COREVECTOR)
		{
			((CoreVector*)mCurrentObject.get())->push_back(toAdd);
		}
	}
	else // first object => add it twice
	{
		mCurrentObject = toAdd;
		mObjectStack.push_back(mCurrentObject);
	}

	mCurrentObject = toAdd;
	mObjectStack.push_back(mCurrentObject);

	return false;
}

DEFINE_METHOD(DictionaryFromJsonUTF16, JSonObjectEnd)
{
	if (mObjectStack.size()>0)
	{
		mObjectStack.pop_back();
		mCurrentObject = mObjectStack[mObjectStack.size() - 1];
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
	CoreItemSP toAdd = MakeCoreVector();

	if (mCurrentObject)
	{
		if (mCurrentObject->GetType() == CoreItem::COREMAP)
		{
			((CoreMap<usString>*)mCurrentObject.get())->set(*((maUSString*)params[0]), toAdd);
		}
		else if (mCurrentObject->GetType() == CoreItem::COREVECTOR)
		{
			((CoreVector*)mCurrentObject.get())->push_back(toAdd);
		}
	}
	else // first object => add it twice
	{
		mCurrentObject = toAdd;
		mObjectStack.push_back(mCurrentObject);
	}

	mCurrentObject = toAdd;
	mObjectStack.push_back(mCurrentObject);

	return false;
}

DEFINE_METHOD(DictionaryFromJsonUTF16, JSonArrayEnd)
{
	if (mObjectStack.size() > 0)
	{
		mObjectStack.pop_back();
		mCurrentObject = mObjectStack[mObjectStack.size() - 1];
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
		if (mCurrentObject)
		{
			L_IsVector = (mCurrentObject->GetType() == CoreItem::COREVECTOR);
		}

		int idx = 0;
		for (unsigned int i = 0; i < params.size(); i += 2)
		{
			usString L_Key = *((maUSString*)params[i]);
			idx = i + 1;

			auto paramType = params[idx]->getType();

			if ((paramType == ATTRIBUTE_TYPE::STRING) || (paramType == ATTRIBUTE_TYPE::USSTRING))
			{
				CoreItemSP L_Value = MakeCoreValue(*((maUSString*)params[idx]));

				if (L_IsVector)
					((CoreVector*)mCurrentObject.get())->push_back(L_Value);
				else
					((CoreMap<usString>*)mCurrentObject.get())->set(L_Key, L_Value);

			}
			else if (paramType == ATTRIBUTE_TYPE::FLOAT)
			{
				CoreItemSP L_Value = MakeCoreValue((*(maFloat*)params[idx]));

				if (L_IsVector)
					((CoreVector*)mCurrentObject.get())->push_back(L_Value);
				else
					((CoreMap<usString>*)mCurrentObject.get())->set(L_Key, L_Value);

			}
			else if (paramType == ATTRIBUTE_TYPE::INT)
			{
				CoreItemSP  L_Value = MakeCoreValue((*(maInt*)params[idx]));

				if (L_IsVector)
					((CoreVector*)mCurrentObject.get())->push_back(L_Value);
				else
					((CoreMap<usString>*)mCurrentObject.get())->set(L_Key, L_Value);

			}
			else if (paramType == ATTRIBUTE_TYPE::LONG)
			{
				CoreItemSP  L_Value = MakeCoreValue((*(maLong*)params[idx]));

				if (L_IsVector)
					((CoreVector*)mCurrentObject.get())->push_back(L_Value);
				else
					((CoreMap<usString>*)mCurrentObject.get())->set(L_Key, L_Value);

			}
			else if (paramType == ATTRIBUTE_TYPE::BOOL)
			{
				CoreItemSP L_Value = MakeCoreValue((*(maBool*)params[idx]));

				if (L_IsVector)
					((CoreVector*)mCurrentObject.get())->push_back(L_Value);
				else
					((CoreMap<usString>*)mCurrentObject.get())->set(L_Key, L_Value);

			}
			else if (paramType == ATTRIBUTE_TYPE::UINT)
			{
				CoreItemSP L_Value = MakeCoreValue((*(maUInt*)params[idx]));

				if (L_IsVector)
					((CoreVector*)mCurrentObject.get())->push_back(L_Value);
				else
					((CoreMap<usString>*)mCurrentObject.get())->set(L_Key, L_Value);

			}
			else if (paramType == ATTRIBUTE_TYPE::ULONG)
			{
				CoreItemSP L_Value = MakeCoreValue((*(maULong*)params[idx]));

				if (L_IsVector)
					((CoreVector*)mCurrentObject.get())->push_back(L_Value);
				else
					((CoreMap<usString>*)mCurrentObject.get())->set(L_Key, L_Value);

			}
			else if (paramType == ATTRIBUTE_TYPE::DOUBLE)
			{
				CoreItemSP L_Value = MakeCoreValue((*(maDouble*)params[idx]));

				if (L_IsVector)
					((CoreVector*)mCurrentObject.get())->push_back(L_Value);
				else
					((CoreMap<usString>*)mCurrentObject.get())->set(L_Key, L_Value);

			}
		}
	}

	return false;

}

namespace Kigs
{
	namespace Core
	{
		// force method creation
		template class JSonFileParserBase< std::string, AsciiParserUtils >;
		template class JSonFileParserBase< usString, US16ParserUtils>;

		std::string CoreItemToJSon(CoreItemSP map)
		{
			KIGS_ASSERT(map->GetType() == CoreItem::COREMAP);
			std::string result;
			JSonFileParser parser;
			parser.ExportToString((CoreMap<std::string>*)map.get(), result);
			return result;
		}

		CoreItemSP JSonToCoreItem(const std::string& str)
		{
			JSonFileParser parser;
			return parser.Get_JsonDictionaryFromString(str);
		}
	}
}