#ifndef _JSONFILEPARSER_H_
#define _JSONFILEPARSER_H_

#include "CoreMap.h"

class CoreItem;

/**
		Parse a JSon File and call delegate object at each new level and data found
*/

#define Declare_JSonDelegate 	DECLARE_METHOD(JSonObjectStart); \
								DECLARE_METHOD(JSonObjectEnd); \
								DECLARE_METHOD(JSonArrayStart); \
								DECLARE_METHOD(JSonArrayEnd); \
								DECLARE_METHOD(JSonParamList); 


class DictionaryFromJson : public CoreModifiable
{
public:
	DECLARE_CLASS_INFO(DictionaryFromJson,CoreModifiable,Renderer);
	
	//! builds an object dictionary by calling the callback of JSonFileParser
	DictionaryFromJson(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	CoreItem* Get_Dictionary() const { if (m_vObjectStack.size()) return m_vObjectStack[0]; return 0; }

protected:
	virtual ~DictionaryFromJson();

	kstl::vector<CoreItem*>					m_vObjectStack;
	CoreItem*								m_pCurrentObject;

	Declare_JSonDelegate;
};

class DictionaryFromJsonUTF16 : public CoreModifiable
{
public:
	DECLARE_CLASS_INFO(DictionaryFromJsonUTF16, CoreModifiable, Renderer);

	//! builds an object dictionary by calling the callback of JSonFileParser
	DictionaryFromJsonUTF16(const kstl::string& name, DECLARE_CLASS_NAME_TREE_ARG);

	CoreItem* Get_Dictionary() const { if (m_vObjectStack.size()) return m_vObjectStack[0]; return 0; }

protected:
	virtual ~DictionaryFromJsonUTF16();

	kstl::vector<CoreItem*>					m_vObjectStack;
	CoreItem*								m_pCurrentObject;

	Declare_JSonDelegate;
};




template <typename stringType, typename parserType>
class	JSonFileParserBase
{
public:
	
	//! parse given JSON file calling given delegate object
	JSonFileParserBase(const kstl::string& filename, CoreModifiable*	delegateObject);

	//! parse given JSON file and retreive an object dictionary
	JSonFileParserBase();
	CoreItem*	Get_JsonDictionary(const kstl::string& filename);
	CoreItem*	Get_JsonDictionaryFromString(const stringType& jsonString);
	bool Export(CoreMap<stringType>* a_value, const kstl::string& a_fileName);
	bool ExportToString(CoreMap<stringType>* a_value, stringType& output);
protected:
	void InitParser(const kstl::string& filename);
	void InitParserFromString(CoreRawBuffer* Buff);
	bool ParseBlock(parserType& Block);
	bool ParseArray(parserType& Array);
	void NotifyDelegateWithParamList();
	void AddValueToParamList(stringType strObjName, stringType objparamValue);
	void RecursiveParseElement(CoreMap<stringType>* a_value, stringType& a_buffer);
	void RecursiveParseElement(CoreVector* a_value, stringType& a_buffer);
	void AddValueToBuffer(CoreItem* a_value, stringType& a_Destbuffer);

	int	GetStringByteSize(const stringType& tocheck);
	int	GetStringCharSize();
	const unsigned char*	GetStringByteBuffer(const stringType& tocheck);

	CoreModifiable*			CreateDictionnaryFromJSONInstance();

	CoreItem*	getDictionnary();

	unsigned int				myJSonObjectStartID;
	unsigned int				myJSonObjectEndID;
	unsigned int				myJSonArrayStartID;
	unsigned int				myJSonArrayEndID;
	unsigned int				myJSonParamListID;
	CoreModifiable*				myDelegateObject;
	CoreModifiable*				myDictionaryFromJson;

	
	kstl::vector<CoreModifiableAttribute*>	myParamList;

};


typedef JSonFileParserBase< kstl::string, AsciiParserUtils>	JSonFileParser;
typedef JSonFileParserBase< usString, US16ParserUtils >		JSonFileParserUTF16;

#endif //_JSONFILEPARSER_H_