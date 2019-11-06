#ifndef _MINIMALXML_H_
#define _MINIMALXML_H_

#include "CoreTypes.h"
#include <string>

class	StringRef
{
public:

	StringRef() : myStringStart(0), myStringEnd(0)
	{

	}

	void	set(unsigned char* end)
	{
		myStringEnd = end;
		myBackupVal = *end;
		*myStringEnd = 0;
	}

	void	clear()
	{
		if (myStringEnd)
		{
			// restore backuped
			*myStringEnd = myBackupVal;
			myStringEnd = 0;
		}
		myStringStart = 0;
	}

	~StringRef()
	{
		clear();
	}

	unsigned char* myStringStart;
	unsigned char* myStringEnd;
	unsigned char	myBackupVal;
};

class stringFromRef : public std::string
{
public:
	stringFromRef(const StringRef& str) : std::string((const char*)str.myStringStart, (unsigned int)(str.myStringEnd - str.myStringStart)){}
};


/**
		Parse an ASCII xml file from one memory block and call the given callback
		does not construct any xml structure in memory
		very permissive (no strong validity check)
		the given memory buffer can be modified by the parser during parse, but restored at the end
		last buffer character must be 0
*/

typedef void (*MinimalXML_DeclHandler) (void           *userData,
                                            const char *version,
                                            const char *encoding,
                                            int             standalone);

typedef void (*MinimalXML_StartElementHandler) (void *userData,
                                                 const char *name,
                                                 const char **atts);

typedef void (*MinimalXML_EndElementHandler) (void *userData,
                                               const char *name);

/* s is not 0 terminated. */
typedef void (*MinimalXML_CharacterDataHandler) (void *userData,
                                                  const char *s,
                                                  int len);

typedef void (*MinimalXML_StartCdataSectionHandler) (void *userData);
/* s is not 0 terminated. */
typedef void (*MinimalXML_EndCdataSectionHandler) (void *userData,
													const char*s,
													int len);

/* 0 terminated */
typedef void (*MinimalXML_DefaultHandler) (void *userData,
                                            const char *s);

// StringRef handler

typedef void (*MinimalXML_StartElementHandlerStringRef) (void* userData,
	StringRef* name,
	StringRef** atts);

typedef void (*MinimalXML_EndElementHandlerStringRef) (void* userData,
	StringRef* name);

class	MinimalXML
{
public:
	
	//! structors
	MinimalXML();
	virtual ~MinimalXML();

	void	setUserData(void* data)
	{
		myUserData=data;
	}
#ifdef _DEBUG
	static FILE*	myOutDebug;
	void	setOutputExport();
#endif

	void	setElementHandler(MinimalXML_StartElementHandler starthandler, MinimalXML_EndElementHandler endhandler)
	{
		myStartElementHandler=starthandler;
		myEndElementHandler=endhandler;
	}

	void	setElementHandlerStringRef(MinimalXML_StartElementHandlerStringRef starthandler, MinimalXML_EndElementHandlerStringRef endhandler)
	{
		myStartElementHandlerStringRef = starthandler;
		myEndElementHandlerStringRef = endhandler;
		myUseStringRef = true;
	}

	void	setCDataHandler(MinimalXML_StartCdataSectionHandler starthandler, MinimalXML_EndCdataSectionHandler endhandler)
	{
		myStartCdataSectionHandler=starthandler;
		myEndCdataSectionHandler=endhandler;
	}
	void	setXmlDeclHandler(MinimalXML_DeclHandler handler)
	{
		myDeclHandler=handler;
	}

	void	setCharacterDataHandler(MinimalXML_CharacterDataHandler handler)
	{
		myCharacterDataHandler=handler;
	}

	void	setDefaultHandler(MinimalXML_DefaultHandler handler)
	{
		myDefaultHandler=handler;
	}

	bool	Parse(u8* buffer, u64 len);


protected:

	void	startElement();
	void	endElement();

	bool	myUseStringRef=false;

	enum	XMLState
	{
		NO_STATE						=0,
		OPENING							=1,	//	<
		CLOSING							=2, //	>
		EXECUTE_OR_PROLOG				=3,	//  ?
		COMMENT_OR_DATA_OR_DOCTYPE		=4,	//  !

		// ELEMENTS
		IN_ELEMENT						=10,
		ELEMENT_START_END				=11,
		ELEMENT_START_SELF_CLOSED		=12,
		CLOSING_ELEMENT					=13,
		ELEMENT_CLOSING_END				=14,
		
		// COMMENT
		IN_COMMENT						=20,
			
		// PROLOG
		IN_PROLOG						=30,

		// DOCTYPE
		IN_DOCTYPE						=40,

		// DATA
		IN_DATA							=50,

		// COMMAND
		IN_EXECUTE						=60,

		XML_ERROR_STATE					=100

	};

	enum XMLSubState
	{
		NO_SUBSTATE						=0,
		ELEMENT_NAME					=1,
		ELEMENT_CLOSE_NAME				=2,
		ATTRIBUTE_NAME					=3,
		ATTRIBUTE_EQUAL					=4,
		ATTRIBUTE_SEARCH_EQUAL			=5,
		ATTRIBUTE_VALUE					=6,
	};

	// if not set, use empty handlers
	static void Empty_DefaultHandler(void *userData,  const char *s);
	static void Empty_DeclHandler(void *userData, const char *version, const char *encoding, int standalone);
	static void Empty_StartElementHandler(void *userData,const char *name,const char **atts);
	static void Empty_EndElementHandler(void *userData,const char *name);
	static void Empty_CharacterDataHandler(void *userData,const char *s, int len);
	static void Empty_StartCdataSectionHandler(void *userData);
	static void Empty_EndCdataSectionHandler(void *userData,const char *s, int len);

	static unsigned char	isAlphaNum[256];

	void	reset();

	void*	myUserData;

	bool	myPrologDone;

	MinimalXML_StartElementHandler				myStartElementHandler;
	MinimalXML_EndElementHandler				myEndElementHandler;
	MinimalXML_StartCdataSectionHandler			myStartCdataSectionHandler;
	MinimalXML_EndCdataSectionHandler			myEndCdataSectionHandler;
	MinimalXML_DeclHandler						myDeclHandler;
	MinimalXML_CharacterDataHandler				myCharacterDataHandler;
	MinimalXML_DefaultHandler					myDefaultHandler;
	MinimalXML_StartElementHandlerStringRef		myStartElementHandlerStringRef;
	MinimalXML_EndElementHandlerStringRef		myEndElementHandlerStringRef;

	class	Attribut_Parser
	{
	public:

		Attribut_Parser()
		{}

		void clear()
		{
			myName.clear();
			myValue.clear();
		}


		StringRef	myName;
		StringRef	myValue;
	};

	class	Attribut_Parser_LinkedNode
	{
	public:
		Attribut_Parser_LinkedNode() 
			:myNext(0)
		{

		}

	public:
		Attribut_Parser				myAttribute;
		Attribut_Parser_LinkedNode*	myNext;
	};

	class	SequenceDetect
	{
	public:

		void	init(unsigned	char* first)
		{
			myCurrentIndex=0;
			add(first);
		}

		void	add(unsigned	char* next)
		{
			mySequence[myCurrentIndex]=next;
			++myCurrentIndex;
		}

		const unsigned char&	val(int relativeIndex)
		{
			return *mySequence[myCurrentIndex+relativeIndex];
		}

		void	reset()
		{
			myCurrentIndex=0;
		}

		unsigned int size()
		{
			return myCurrentIndex;
		}

		unsigned	char*	mySequence[16];
		unsigned	int		myCurrentIndex;
	};

	class	Prolog_Parser
	{
	public:
		Prolog_Parser() :myCurrentAttribute(0)
		{
		
		}
		void	clear();
		Attribut_Parser		myAttributes[3]; // version, encoding, standalone
		int					myCurrentAttribute;
	};

	class	ElementStart_Parser
	{
	public:

		ElementStart_Parser() 
			:myFirstAttrib(0)
			,myCurrentAttrib(0)
			,myAttribCount(0)
			,myAttribArray(0)
			,myStringRefAttribArray(0)
		{

		}

		void				clear(MinimalXML* parentinstance);
		StringRef			myName;
		Attribut_Parser*	getAttrib(MinimalXML* parentinstance);
		const char**		getAttribArray();
		StringRef**			getStringRefAttribArray();

		Attribut_Parser_LinkedNode*	myFirstAttrib;
		Attribut_Parser_LinkedNode*	myCurrentAttrib;
		unsigned int				myAttribCount;
	
	protected:
		const char**		myAttribArray;
		StringRef**			myStringRefAttribArray;
		const char*			myPreallocLittleArray[17];
		StringRef*			myPreallocStringRefLittleArray[17];
	
	};

	Prolog_Parser					myProlog;
	SequenceDetect					mySequenceDetect;
	ElementStart_Parser				myElementStart;
	StringRef						myOutOfElementCharacters;

// manage prealloc linked nodes

	void	InitPreallocated()
	{
		myPreallocatedStart=(uptr)(&myPreallocatedLinkedNodes[0]);
		myPreallocatedEnd=(uptr)(&myPreallocatedLinkedNodes[31]);
		myPreallocatedLinkedNodesFlag=0;
	}

	friend class ElementStart_Parser;

	Attribut_Parser_LinkedNode*		getFreeLinkedNode()
	{
		if(myPreallocatedLinkedNodesFlag == 0xFFFFFFFF)
		{
			return new Attribut_Parser_LinkedNode();
		}

		unsigned int index=0;
		while(index<32)
		{
			if(!(myPreallocatedLinkedNodesFlag & (1<<index)))
			{
				myPreallocatedLinkedNodes[index].myNext=0;
				myPreallocatedLinkedNodesFlag|=(1<<index);
				return &myPreallocatedLinkedNodes[index];
			}
			index++;
		}

		// should never be there !
		return 0;
	}

	void	releaseLinkedNode(Attribut_Parser_LinkedNode* node)
	{
		// check if preallocated
		uptr nodeAddr=(uptr)node;
		if((nodeAddr<myPreallocatedStart) || (nodeAddr>myPreallocatedEnd))
		{
			delete node;
			return;
		}
		
		uptr index=nodeAddr-myPreallocatedStart;
		index/=sizeof(Attribut_Parser_LinkedNode);
		myPreallocatedLinkedNodes[index].myAttribute.clear();
		myPreallocatedLinkedNodesFlag^=(1<<index);
	}

	unsigned int					myPreallocatedLinkedNodesFlag;
	Attribut_Parser_LinkedNode		myPreallocatedLinkedNodes[32];
	uptr							myPreallocatedStart;
	uptr							myPreallocatedEnd;

};

#endif //_MINIMALXML_H_