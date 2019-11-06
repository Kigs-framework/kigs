#include "PrecompiledHeaders.h"
#include "MinimalXML.h"


// mask : 1 => space or tab
// 2 => strict alpha num
// 4 => autorised characters

unsigned char	MinimalXML::isAlphaNum[256]={

	0,	0,	0,	0,	0,	0,	0,	0,	0,	1,	1,	1,	1,	1,	0,	0,
	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,	0,
	1,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,
	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	4,	4,	4,	4,	4,	4,
	4,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,
	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	4,	4,	4,	4,	4,
	4,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,
	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	6,	4,	4,	4,	4,	0,
	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,
	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,
	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,
	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,
	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,
	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,
	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,
	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	4,	0
};

#ifdef _KIGS_TOOLS
FILE*	MinimalXML::myOutDebug=0;

void	MinimalXML::setOutputExport()
{
	myOutDebug=fopen("MinimalXmlOutput.xml","wt");
}

#endif



// if not set, use empty handlers
void MinimalXML::Empty_DefaultHandler(void *userData,  const char *s)
{
#ifdef _KIGS_TOOLS
	if(myOutDebug)
	{
		fprintf(myOutDebug,"Empty_DefaultHandler %s\n",s);
	}
	else
	{
		printf("Empty_DefaultHandler %s\n",s);
	}
#endif
}
void MinimalXML::Empty_DeclHandler(void *userData, const char *version, const char *encoding,int standalone)
{
#ifdef _KIGS_TOOLS
	if(myOutDebug)
	{
		if(version && encoding)
			fprintf(myOutDebug,"<?xml version=\"%s\" encoding=\"%s\" standalone=\"%i\" ?>",version,encoding,standalone);
		else
			fprintf(myOutDebug,"<?xml version=\"1.0\"?>\n");
	}
	else
	{
		if(version && encoding)
			printf("Empty_DeclHandler version=%s encoding=%s standalone=%i \n",version,encoding,standalone);
		else
			printf("Empty_DeclHandler \n");
	}
#endif
}
void MinimalXML::Empty_StartElementHandler(void *userData,const char *name,const char **atts)
{
#ifdef _KIGS_TOOLS
	if(myOutDebug)
	{
		fprintf(myOutDebug,"<%s ",name);
		int i=0;
		while(atts[i])
		{
			fprintf(myOutDebug,"%s=\"%s\" ",atts[i],atts[i+1]);
			i+=2;
		}
		fprintf(myOutDebug,">\n");
	}
	else
	{
		printf("Empty_StartElementHandler start element %s \n",name);
		int i=0;
		while(atts[i])
		{
			printf("Empty_StartElementHandler  attribute %s = %s \n",atts[i],atts[i+1]);
			i+=2;
		}
	}

#endif
}

void MinimalXML::Empty_EndElementHandler(void *userData,const char *name)
{
#ifdef _KIGS_TOOLS
	if(myOutDebug)
	{
		fprintf(myOutDebug,"</%s>\n",name);
	}
	else
	{
		printf("Empty_EndElementHandler %s \n",name);
	}
#endif
}

void MinimalXML::Empty_CharacterDataHandler(void *userData,const char *s,int len)
{
#ifdef _KIGS_TOOLS
	if(myOutDebug)
	{
		fprintf(myOutDebug,"Empty_CharacterDataHandler \n");
	}
	else
	{
		printf("Empty_CharacterDataHandler \n");
	}
#endif
}

void MinimalXML::Empty_StartCdataSectionHandler(void *userData)
{
#ifdef _KIGS_TOOLS
	if(myOutDebug)
	{
		fprintf(myOutDebug,"Empty_StartCdataSectionHandler \n");
	}
	else
	{
		printf("Empty_StartCdataSectionHandler \n");
	}
#endif
}
void MinimalXML::Empty_EndCdataSectionHandler(void *userData,const char *s,int len)
{
#ifdef _KIGS_TOOLS
	if(myOutDebug)
	{
		fprintf(myOutDebug,"Empty_EndCdataSectionHandler \n");
	}
	else
	{
		printf("Empty_EndCdataSectionHandler \n");
	}
#endif
}

MinimalXML::MinimalXML() :
	myUserData(0)
	, myPrologDone(false)
	, myStartElementHandler(&MinimalXML::Empty_StartElementHandler)
	, myEndElementHandler(&MinimalXML::Empty_EndElementHandler)
	, myStartCdataSectionHandler(&MinimalXML::Empty_StartCdataSectionHandler)
	, myEndCdataSectionHandler(&MinimalXML::Empty_EndCdataSectionHandler)
	, myDeclHandler(&MinimalXML::Empty_DeclHandler)
	, myCharacterDataHandler(&MinimalXML::Empty_CharacterDataHandler)
	, myDefaultHandler(&MinimalXML::Empty_DefaultHandler)
{
	InitPreallocated();
}

MinimalXML::~MinimalXML()
{

}

bool	MinimalXML::Parse(unsigned char*	buffer, u64 len)
{
	unsigned char* currentChar=buffer;
	unsigned char* endChar=currentChar+len;
	XMLState	state=NO_STATE;
	XMLSubState	substate=NO_SUBSTATE;

	Attribut_Parser*	currentAttribute=0;

	while(currentChar<endChar)
	{
		unsigned char&	charVal=*currentChar;

		if(substate != NO_SUBSTATE)
		{
			// in string retreiving
			switch(substate)
			{
				case ELEMENT_NAME:
				{
					if(isAlphaNum[charVal]&1) // space => element name is finished
					{
						substate=NO_SUBSTATE;
						myElementStart.myName.set(currentChar);
					}
					else if(charVal=='>')
					{
						substate=NO_SUBSTATE;
						myElementStart.myName.set(currentChar);
						state=ELEMENT_START_END;
					}
					else if(charVal=='/')
					{
						substate=NO_SUBSTATE;
						myElementStart.myName.set(currentChar);
						state=ELEMENT_START_SELF_CLOSED;
					}

					break;
				}
				case ELEMENT_CLOSE_NAME:
				{
					if(isAlphaNum[charVal]&1) // space => element close name is finished
					{
						substate=NO_SUBSTATE;
						myElementStart.myName.set(currentChar);
					}
					else if(charVal=='>')
					{
						substate=NO_SUBSTATE;
						myElementStart.myName.set(currentChar);
						state=ELEMENT_CLOSING_END;
					}
					
					break;
				}
				case ATTRIBUTE_NAME:
				{
					if(isAlphaNum[charVal]&1) // space => attribute name is finished
					{
						substate=ATTRIBUTE_SEARCH_EQUAL;	// search EQUAL
						currentAttribute->myName.set(currentChar);
					}
					else if(charVal=='=')
					{
						substate=ATTRIBUTE_EQUAL;
						currentAttribute->myName.set(currentChar);
					}
					else if(isAlphaNum[charVal]&4) // valid string
					{
						// continue
						
					}
					else 
					{
						substate=NO_SUBSTATE;
						state=XML_ERROR_STATE;
					}
					break;
				}
				case ATTRIBUTE_EQUAL:
				{
					if(isAlphaNum[charVal]&1) // space do nothing
					{
						
					}
					else if(charVal=='"')
					{
						substate=ATTRIBUTE_VALUE;
					}
				}
				break;
				case ATTRIBUTE_SEARCH_EQUAL:
				{
					if(isAlphaNum[charVal]&1) // space do nothing
					{
						
					}
					else if(charVal=='=')
					{
						substate=ATTRIBUTE_EQUAL;
					}
					else
					{
						substate=NO_SUBSTATE;
						state=XML_ERROR_STATE;
					}
				}
				break;
				case ATTRIBUTE_VALUE:
				{
					if(charVal=='"') // value end
					{
						// check empty attributes
						if(currentAttribute->myValue.myStringStart == 0)
						{
							currentAttribute->myValue.myStringStart=currentChar;
						}
						substate=NO_SUBSTATE;
						currentAttribute->myValue.set(currentChar);
					}
					else if(isAlphaNum[charVal]&4)
					{
						// check if first character
						if(currentAttribute->myValue.myStringStart == 0)
						{
							currentAttribute->myValue.myStringStart=currentChar;
						}
					}
				}
				break;
				default:
				break;
			}
		}
		
		// not only else because substate should have changed in previous if
		if(substate == NO_SUBSTATE)
		{
			switch(state)
			{
			case NO_STATE:
				if('<' == charVal)
				{
					state=OPENING;
					// if some characters are here, call handler
					if(myOutOfElementCharacters.myStringStart!=0)
					{
						(*myCharacterDataHandler)(myUserData,(const char*)myOutOfElementCharacters.myStringStart,(int)(currentChar-myOutOfElementCharacters.myStringStart));
						myOutOfElementCharacters.clear();

					}

				}
				else if(isAlphaNum[charVal]&1) // space continue
				{
					
				}
				else if(isAlphaNum[charVal]&4)
				{
					if(myOutOfElementCharacters.myStringStart==0)
					{
						myOutOfElementCharacters.myStringStart=currentChar;
					}
				}
				break;
			case OPENING:
				switch(charVal)
				{
				case '?':
					state=EXECUTE_OR_PROLOG;
					break;
				case '!':
					state=COMMENT_OR_DATA_OR_DOCTYPE;
					mySequenceDetect.init(currentChar);
					break;
				case '/':
					state=CLOSING_ELEMENT;
					break;
				default:
					// check alpha numeric
					if(isAlphaNum[charVal]&1)
					{
						// Space continue	
					}
					else if(isAlphaNum[charVal]&4)
					{
						state=IN_ELEMENT;
						substate=ELEMENT_NAME;
						myElementStart.myName.myStringStart=currentChar;
					}
					else
					{
						state=XML_ERROR_STATE;
					}
				}
				break;
			case CLOSING:
				break;
			case EXECUTE_OR_PROLOG:
				if(charVal == 'x') // start by x
				{
					if(myPrologDone)
					{
						state=IN_EXECUTE;
					}
					else
					{
						state=IN_PROLOG;
						mySequenceDetect.init(currentChar);
						myProlog.myCurrentAttribute=0;
					}
				}
				else if(isAlphaNum[charVal]&1)
				{
					// space continue
				}
				else
				{
					state=XML_ERROR_STATE;
				}
				break;
			case IN_PROLOG:
				if(('m' == charVal)&&('x' == mySequenceDetect.val(-1)))
				{
					mySequenceDetect.add(currentChar);
				}
				else if(('l' == charVal)&&('m' == mySequenceDetect.val(-1)))
				{
					mySequenceDetect.add(currentChar);
				}
				else if((isAlphaNum[charVal]&1)&&('l' == mySequenceDetect.val(-1) ))
				{
					// continue
				}
				else if((charVal=='?')&&('l' == mySequenceDetect.val(-1)))
				{	
					mySequenceDetect.add(currentChar);
				}
				else if((charVal=='>')&&('?' == mySequenceDetect.val(-1) ))
				{
					// call Prolog end
					const char* version=0;
					const char* encoding=0;
					int			standalone=0;

					if(myProlog.myAttributes[0].myName.myStringStart)
					{
						version=(const char*)myProlog.myAttributes[0].myValue.myStringStart;
					}

					if(myProlog.myAttributes[1].myName.myStringStart)
					{
						encoding=(const char*)myProlog.myAttributes[1].myValue.myStringStart;
					}

					if(myProlog.myAttributes[2].myName.myStringStart)
					{
						if(strcmp((const char*)myProlog.myAttributes[2].myValue.myStringStart,"yes") == 0)
						{
							standalone=1;
						}
					}

					(*myDeclHandler)(myUserData,version,encoding,standalone);
					myPrologDone=true;
					myProlog.clear();
					state=NO_STATE;
				}
				else if(isAlphaNum[charVal]&4)
				{
					// found attribute
					if(myProlog.myCurrentAttribute<=2)
					{
						substate=ATTRIBUTE_NAME;
						currentAttribute=&myProlog.myAttributes[myProlog.myCurrentAttribute++];
						currentAttribute->myName.myStringStart=currentChar;
					}
					else
					{
						state=XML_ERROR_STATE;
					}
				}

				break;
			case COMMENT_OR_DATA_OR_DOCTYPE:
				{
					// only detect comment for now
					if(('-'==charVal)&&('!' == mySequenceDetect.val(-1)))
					{
						mySequenceDetect.add(currentChar);
					}
					else if(('-' == charVal)&&('-' == mySequenceDetect.val(-1)))
					{
						state=IN_COMMENT;
						mySequenceDetect.reset();
					}
					// detect CDATA
					else if(('[' == charVal)&&('!' == mySequenceDetect.val(-1)))
					{
						mySequenceDetect.add(currentChar);
					}
					else if(('C' == charVal)&&('[' == mySequenceDetect.val(-1)))
					{
						mySequenceDetect.add(currentChar);
					}
					else if(('D' == charVal)&&('C' == mySequenceDetect.val(-1)))
					{
						mySequenceDetect.add(currentChar);
					}
					else if(('A' == charVal)&&('D' == mySequenceDetect.val(-1)))
					{
						mySequenceDetect.add(currentChar);
					}
					else if(('T' == charVal)&&('A' == mySequenceDetect.val(-1)))
					{
						mySequenceDetect.add(currentChar);
					}
					else if(('A' == charVal)&&('T' == mySequenceDetect.val(-1)))
					{
						mySequenceDetect.add(currentChar);
					}
					else if(('[' == charVal)&&('A' == mySequenceDetect.val(-1)))
					{
						state=IN_DATA;
						mySequenceDetect.reset();
						(*myStartCdataSectionHandler)(myUserData);
					}
					// DOCTYPE
					else if(('D' == charVal)&&('!' == mySequenceDetect.val(-1)))
					{
						// TODO
						state=IN_DOCTYPE;
						mySequenceDetect.reset();
					}
				}
				break;

			case IN_COMMENT:
				{
					// wait for comment end
					if(('-'==charVal) && (mySequenceDetect.size()==0))
					{
						mySequenceDetect.init(currentChar);
					}
					else if(('-' == charVal)&&('-' == mySequenceDetect.val(-1)))
					{
						mySequenceDetect.add(currentChar);
					}
					else if('>' == charVal)
					{
						if(mySequenceDetect.size()==2)
						{
							if('-' == mySequenceDetect.val(-1))
							{
								// comment end
								state=NO_STATE;
								mySequenceDetect.reset();
							}
						}
					}
					else if(mySequenceDetect.size())
					{
						mySequenceDetect.reset();
					}
				}
				break;
			case IN_DATA:
				{
					if(myOutOfElementCharacters.myStringStart==0)
					{
						myOutOfElementCharacters.myStringStart=currentChar;
					}
					// wait for data end
					if((']'==charVal) && (mySequenceDetect.size()==0))
					{
						mySequenceDetect.init(currentChar);
					}
					else if((']' == charVal)&&(']' == mySequenceDetect.val(-1)))
					{
						mySequenceDetect.add(currentChar);
					}
					else if(('>' == charVal)&& (mySequenceDetect.size()>0) && (']' == mySequenceDetect.val(-1)))
					{
						// data end
						if(myOutOfElementCharacters.myStringStart!=0)
						{
							(*myEndCdataSectionHandler)(myUserData,(const char*)myOutOfElementCharacters.myStringStart,(int)(currentChar-myOutOfElementCharacters.myStringStart-2));
							myOutOfElementCharacters.clear();
						}
						else
						{
							(*myEndCdataSectionHandler)(myUserData,0,0);
						}
						mySequenceDetect.reset();
						state=NO_STATE;
					}
					else if(mySequenceDetect.size())
					{
						mySequenceDetect.reset();
					}
				}
				break;
			case IN_DOCTYPE:
				{
					// wait for doctype end
					if('>'==charVal)
					{
						state=NO_STATE;
					}
				}
				break;

			case IN_ELEMENT:
				{
					if('/'==charVal)
					{
						if(myElementStart.myName.myStringStart)
						{
							state=ELEMENT_START_SELF_CLOSED;
						}
						else
						{
							state=CLOSING_ELEMENT;
						}
					}
					else if('>'==charVal)
					{
						//const char** attribs=myElementStart.getAttribArray();
						//(*myStartElementHandler)(myUserData,(const char*)myElementStart.myName.myStringStart,attribs);
						startElement();
						myElementStart.clear(this);
						state=NO_STATE;
					}
					else if(isAlphaNum[charVal]&1) // space, continue
					{
					}
					else if(isAlphaNum[charVal]&4)
					{
						// start element attribute
						substate=ATTRIBUTE_NAME;
						currentAttribute=myElementStart.getAttrib(this);
						currentAttribute->myName.myStringStart=currentChar;
					}
				}
				break;
			case ELEMENT_START_END:
				{
					//const char** attribs=myElementStart.getAttribArray();
					//(*myStartElementHandler)(myUserData,(const char*)myElementStart.myName.myStringStart,attribs);
					startElement();
					myElementStart.clear(this);
					state=NO_STATE;
				}
				break;
			case ELEMENT_START_SELF_CLOSED:
				{
					startElement();
					//const char** attribs=myElementStart.getAttribArray();
					//(*myStartElementHandler)(myUserData,(const char*)myElementStart.myName.myStringStart,attribs);
					endElement();
					//(*myEndElementHandler)(myUserData,(const char*)myElementStart.myName.myStringStart);
					//myElementStart.clear(this);
					state=NO_STATE;
				}
				break;
			case CLOSING_ELEMENT:
				{
					if(isAlphaNum[charVal]&1) // space, continue
					{

					}
					else if('>'==charVal)
					{
						endElement();
						//(*myEndElementHandler)(myUserData,(const char*)myElementStart.myName.myStringStart);
						//myElementStart.clear(this);
						state=NO_STATE;
					}
					else if(isAlphaNum[charVal]&4)
					{
						myElementStart.myName.myStringStart=currentChar;
						substate=ELEMENT_CLOSE_NAME;
					}

				}
				break;
			case ELEMENT_CLOSING_END:
				{
					endElement();
					//(*myEndElementHandler)(myUserData,(const char*)myElementStart.myName.myStringStart);
					//myElementStart.clear(this);
					state=NO_STATE;
				}
				break;
			case XML_ERROR_STATE:
			default:
				reset();
				return false;
			}
		}
		++currentChar;
	}

	reset();
	return true;

}

void	MinimalXML::startElement()
{
	if (myUseStringRef)
	{
		StringRef** attribs = myElementStart.getStringRefAttribArray();
		(*myStartElementHandlerStringRef)(myUserData,&myElementStart.myName, attribs);

	}
	else
	{
		const char** attribs = myElementStart.getAttribArray();
		(*myStartElementHandler)(myUserData, (const char*)myElementStart.myName.myStringStart, attribs);
	}
}
void	MinimalXML::endElement()
{
	if (myUseStringRef)
	{
		(*myEndElementHandlerStringRef)(myUserData, &myElementStart.myName);
		myElementStart.clear(this);
	}
	else
	{
		(*myEndElementHandler)(myUserData, (const char*)myElementStart.myName.myStringStart);
		myElementStart.clear(this);
	}
}

void	MinimalXML::reset()
{

#ifdef  _KIGS_TOOLS
	if(myOutDebug)
		fclose(myOutDebug);
#endif
	myProlog.clear();
	myElementStart.clear(this);
}


void	MinimalXML::Prolog_Parser::clear()
{
	myAttributes[0].clear();
	myAttributes[1].clear();
	myAttributes[2].clear();
}

void	MinimalXML::ElementStart_Parser::clear(MinimalXML* parentinstance)
{
	myName.clear();
	// clear attribute
	while(myFirstAttrib)
	{
		Attribut_Parser_LinkedNode* current=myFirstAttrib;
		current->myAttribute.clear();
		myFirstAttrib=current->myNext;
		parentinstance->releaseLinkedNode(current);
	}
	myAttribCount=0;
	myCurrentAttrib=0;
	if(myAttribArray)
	{
		delete[] myAttribArray;
		myAttribArray=0;
	}
	if (myStringRefAttribArray)
	{
		delete[] myStringRefAttribArray;
		myStringRefAttribArray = 0;
	}
}

MinimalXML::Attribut_Parser*	MinimalXML::ElementStart_Parser::getAttrib(MinimalXML* parentinstance)
{
	Attribut_Parser_LinkedNode* result=parentinstance->getFreeLinkedNode();
	if(myCurrentAttrib)
	{
		myCurrentAttrib->myNext=result;
	}
	else
	{
		myFirstAttrib=result;
	}
	myCurrentAttrib=result;
	myAttribCount++;
	return &(myCurrentAttrib->myAttribute);
}

const char**		MinimalXML::ElementStart_Parser::getAttribArray()
{
	// return a pointer even if attribCount is 0
	const char**	result=0;
	
	if(myAttribCount<=8)
	{
		result=myPreallocLittleArray;
	}
	else
	{
		result=new const char*[myAttribCount*2+1];
		myAttribArray=result;
	}
	unsigned int i;
	Attribut_Parser_LinkedNode* current=myFirstAttrib;
	for(i=0;i<(myAttribCount*2);i+=2)
	{
		result[i]=(const char*)current->myAttribute.myName.myStringStart;
		result[i+1]=(const char*)current->myAttribute.myValue.myStringStart;
		current=current->myNext;
	}

	result[myAttribCount*2]=0;
	
	return result;

}


StringRef** MinimalXML::ElementStart_Parser::getStringRefAttribArray()
{
	// return a pointer even if attribCount is 0
	StringRef** result = 0;

	if (myAttribCount <= 8)
	{
		result = myPreallocStringRefLittleArray;
	}
	else
	{
		result = new StringRef * [myAttribCount * 2 + 1];
		myStringRefAttribArray = result;
	}
	unsigned int i;
	Attribut_Parser_LinkedNode* current = myFirstAttrib;
	for (i = 0; i < (myAttribCount * 2); i += 2)
	{
		result[i] = &current->myAttribute.myName;
		result[i + 1] = &current->myAttribute.myValue;
		current = current->myNext;
	}

	result[myAttribCount * 2] = 0;

	return result;

}
