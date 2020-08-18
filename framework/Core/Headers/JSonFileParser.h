#ifndef _JSONFILEPARSER_H_
#define _JSONFILEPARSER_H_

#include "CoreMap.h"
#include "AsciiParserUtils.h"

class CoreItem;

/**
		Parse a JSon File and call delegate object at each new level and data found
*/

#define Declare_JSonDelegate 	DECLARE_METHOD(JSonObjectStart)\
								DECLARE_METHOD(JSonObjectEnd)\
								DECLARE_METHOD(JSonArrayStart)\
								DECLARE_METHOD(JSonArrayEnd)\
								DECLARE_METHOD(JSonParamList) 

#define JSonDelegateMethods		JSonObjectStart,JSonObjectEnd,JSonArrayStart,JSonArrayEnd,JSonParamList

// ****************************************
// * DictionaryFromJson class
// * --------------------------------------
/**
 * \file	JSonFileParser.h
 * \class	DictionaryFromJson
 * \ingroup Core
 * \brief	Construct CoreItem tree from a JSon buffer
 */
 // ****************************************
class DictionaryFromJson : public CoreModifiable
{
public:
	DECLARE_CLASS_INFO(DictionaryFromJson,CoreModifiable,Renderer);
	
	//! builds an object dictionary by calling the callback of JSonFileParser
	DictionaryFromJson(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	CoreItemSP Get_Dictionary() const { if (mObjectStack.size()) return mObjectStack[0]; return CoreItemSP(nullptr); }

protected:
	virtual ~DictionaryFromJson();

	kstl::vector<CoreItemSP>					mObjectStack;
	CoreItemSP								mCurrentObject;

	Declare_JSonDelegate;

	COREMODIFIABLE_METHODS(JSonDelegateMethods);
};


// ****************************************
// * DictionaryFromJsonUTF16 class
// * --------------------------------------
/**
 * \file	JSonFileParser.h
 * \class	DictionaryFromJsonUTF16
 * \ingroup Core
 * \brief	Construct CoreItem tree from a JSon buffer in UTF16
 */
 // ****************************************
class DictionaryFromJsonUTF16 : public CoreModifiable
{
public:
	DECLARE_CLASS_INFO(DictionaryFromJsonUTF16, CoreModifiable, Renderer);

	//! builds an object dictionary by calling the callback of JSonFileParser
	DictionaryFromJsonUTF16(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	CoreItemSP Get_Dictionary() const { if (mObjectStack.size()) return mObjectStack[0]; return CoreItemSP(nullptr); }

protected:
	virtual ~DictionaryFromJsonUTF16();

	kstl::vector<CoreItemSP>					mObjectStack;
	CoreItemSP								mCurrentObject;

	Declare_JSonDelegate;

	COREMODIFIABLE_METHODS(JSonDelegateMethods);
};


// ****************************************
// * JSonFileParserBase class
// * --------------------------------------
/**
 * \file	JSonFileParser.h
 * \class	JSonFileParserBase
 * \ingroup Core
 * \brief	Base class for JSon parser
 */
 // ****************************************

template <typename stringType, typename parserType>
class	JSonFileParserBase
{
public:
	
	//! parse given JSON file calling given delegate object
	JSonFileParserBase(const kstl::string& filename, CoreModifiable*	delegateObject);

	//! parse given JSON file and retreive an object dictionary
	JSonFileParserBase();
	CoreItemSP	Get_JsonDictionary(const kstl::string& filename);
	CoreItemSP	Get_JsonDictionaryFromString(const stringType& jsonString);
	bool Export(CoreMap<stringType>* a_value, const kstl::string& a_fileName);
	bool ExportToString(CoreMap<stringType>* a_value, stringType& output);
protected:
	void InitParser(const kstl::string& filename);
	void InitParserFromString(CoreRawBuffer* Buff);
	bool ParseBlock(parserType& Block);
	bool ParseArray(parserType& Array);
	void NotifyDelegateWithParamList();
	void AddValueToParamList(const stringType& strObjName,const stringType& objparamValue);

	CoreModifiableAttribute* getNewStringAttribute(const stringType& attributeName,const stringType& strObjName);

	void RecursiveParseElement(CoreItem& a_value, stringType& a_buffer);
	void AddValueToBuffer(CoreItem& a_value, stringType& a_Destbuffer);

	int	GetStringByteSize(const stringType& tocheck);
	int	GetStringCharSize();
	const unsigned char*	GetStringByteBuffer(const stringType& tocheck);

	CMSP			CreateDictionnaryFromJSONInstance();

	CoreItemSP	getDictionnary();

	unsigned int				mJSonObjectStartID=0;
	unsigned int				mJSonObjectEndID=0;
	unsigned int				mJSonArrayStartID=0;
	unsigned int				mJSonArrayEndID=0;
	unsigned int				mJSonParamListID=0;
	CMSP						mDelegateObject=nullptr;
	CMSP						mDictionaryFromJson=nullptr;

	
	kstl::vector<CoreModifiableAttribute*>	mParamList;

};


typedef JSonFileParserBase< kstl::string, AsciiParserUtils>	JSonFileParser;
typedef JSonFileParserBase< usString, US16ParserUtils >		JSonFileParserUTF16;

#endif //_JSONFILEPARSER_H_