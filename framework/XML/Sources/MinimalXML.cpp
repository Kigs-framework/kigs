#include "PrecompiledHeaders.h"
#include "MinimalXML.h"


// mask : 1 => space or tab
// 2 => strict alpha num
// 4 => autorised characters

unsigned char	MinimalXML::mIsAlphaNum[256]={

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
	mUserData(0)
	, mPrologDone(false)
	, mStartElementHandler(&MinimalXML::Empty_StartElementHandler)
	, mEndElementHandler(&MinimalXML::Empty_EndElementHandler)
	, mStartCdataSectionHandler(&MinimalXML::Empty_StartCdataSectionHandler)
	, mEndCdataSectionHandler(&MinimalXML::Empty_EndCdataSectionHandler)
	, mDeclHandler(&MinimalXML::Empty_DeclHandler)
	, mCharacterDataHandler(&MinimalXML::Empty_CharacterDataHandler)
	, mDefaultHandler(&MinimalXML::Empty_DefaultHandler)
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
					if(mIsAlphaNum[charVal]&1) // space => element name is finished
					{
						substate=NO_SUBSTATE;
						mElementStart.mName.set(currentChar);
					}
					else if(charVal=='>')
					{
						substate=NO_SUBSTATE;
						mElementStart.mName.set(currentChar);
						state=ELEMENT_START_END;
					}
					else if(charVal=='/')
					{
						substate=NO_SUBSTATE;
						mElementStart.mName.set(currentChar);
						state=ELEMENT_START_SELF_CLOSED;
					}

					break;
				}
				case ELEMENT_CLOSE_NAME:
				{
					if(mIsAlphaNum[charVal]&1) // space => element close name is finished
					{
						substate=NO_SUBSTATE;
						mElementStart.mName.set(currentChar);
					}
					else if(charVal=='>')
					{
						substate=NO_SUBSTATE;
						mElementStart.mName.set(currentChar);
						state=ELEMENT_CLOSING_END;
					}
					
					break;
				}
				case ATTRIBUTE_NAME:
				{
					if(mIsAlphaNum[charVal]&1) // space => attribute name is finished
					{
						substate=ATTRIBUTE_SEARCH_EQUAL;	// search EQUAL
						currentAttribute->mName.set(currentChar);
					}
					else if(charVal=='=')
					{
						substate=ATTRIBUTE_EQUAL;
						currentAttribute->mName.set(currentChar);
					}
					else if(mIsAlphaNum[charVal]&4) // valid string
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
					if(mIsAlphaNum[charVal]&1) // space do nothing
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
					if(mIsAlphaNum[charVal]&1) // space do nothing
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
						if(currentAttribute->mValue.myStringStart == 0)
						{
							currentAttribute->mValue.myStringStart=currentChar;
						}
						substate=NO_SUBSTATE;
						currentAttribute->mValue.set(currentChar);
					}
					else if(mIsAlphaNum[charVal]&4)
					{
						// check if first character
						if(currentAttribute->mValue.myStringStart == 0)
						{
							currentAttribute->mValue.myStringStart=currentChar;
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
					if(mOutOfElementCharacters.myStringStart!=0)
					{
						(*mCharacterDataHandler)(mUserData,(const char*)mOutOfElementCharacters.myStringStart,(int)(currentChar-mOutOfElementCharacters.myStringStart));
						mOutOfElementCharacters.clear();

					}

				}
				else if(mIsAlphaNum[charVal]&1) // space continue
				{
					
				}
				else if(mIsAlphaNum[charVal]&4)
				{
					if(mOutOfElementCharacters.myStringStart==0)
					{
						mOutOfElementCharacters.myStringStart=currentChar;
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
					mSequenceDetect.init(currentChar);
					break;
				case '/':
					state=CLOSING_ELEMENT;
					break;
				default:
					// check alpha numeric
					if(mIsAlphaNum[charVal]&1)
					{
						// Space continue	
					}
					else if(mIsAlphaNum[charVal]&4)
					{
						state=IN_ELEMENT;
						substate=ELEMENT_NAME;
						mElementStart.mName.myStringStart=currentChar;
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
					if(mPrologDone)
					{
						state=IN_EXECUTE;
					}
					else
					{
						state=IN_PROLOG;
						mSequenceDetect.init(currentChar);
						mProlog.mCurrentAttribute=0;
					}
				}
				else if(mIsAlphaNum[charVal]&1)
				{
					// space continue
				}
				else
				{
					state=XML_ERROR_STATE;
				}
				break;
			case IN_PROLOG:
				if(('m' == charVal)&&('x' == mSequenceDetect.val(-1)))
				{
					mSequenceDetect.add(currentChar);
				}
				else if(('l' == charVal)&&('m' == mSequenceDetect.val(-1)))
				{
					mSequenceDetect.add(currentChar);
				}
				else if((mIsAlphaNum[charVal]&1)&&('l' == mSequenceDetect.val(-1) ))
				{
					// continue
				}
				else if((charVal=='?')&&('l' == mSequenceDetect.val(-1)))
				{	
					mSequenceDetect.add(currentChar);
				}
				else if((charVal=='>')&&('?' == mSequenceDetect.val(-1) ))
				{
					// call Prolog end
					const char* version=0;
					const char* encoding=0;
					int			standalone=0;

					if(mProlog.mAttributes[0].mName.myStringStart)
					{
						version=(const char*)mProlog.mAttributes[0].mValue.myStringStart;
					}

					if(mProlog.mAttributes[1].mName.myStringStart)
					{
						encoding=(const char*)mProlog.mAttributes[1].mValue.myStringStart;
					}

					if(mProlog.mAttributes[2].mName.myStringStart)
					{
						if(strcmp((const char*)mProlog.mAttributes[2].mValue.myStringStart,"yes") == 0)
						{
							standalone=1;
						}
					}

					(*mDeclHandler)(mUserData,version,encoding,standalone);
					mPrologDone=true;
					mProlog.clear();
					state=NO_STATE;
				}
				else if(mIsAlphaNum[charVal]&4)
				{
					// found attribute
					if(mProlog.mCurrentAttribute<=2)
					{
						substate=ATTRIBUTE_NAME;
						currentAttribute=&mProlog.mAttributes[mProlog.mCurrentAttribute++];
						currentAttribute->mName.myStringStart=currentChar;
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
					if(('-'==charVal)&&('!' == mSequenceDetect.val(-1)))
					{
						mSequenceDetect.add(currentChar);
					}
					else if(('-' == charVal)&&('-' == mSequenceDetect.val(-1)))
					{
						state=IN_COMMENT;
						mSequenceDetect.reset();
					}
					// detect CDATA
					else if(('[' == charVal)&&('!' == mSequenceDetect.val(-1)))
					{
						mSequenceDetect.add(currentChar);
					}
					else if(('C' == charVal)&&('[' == mSequenceDetect.val(-1)))
					{
						mSequenceDetect.add(currentChar);
					}
					else if(('D' == charVal)&&('C' == mSequenceDetect.val(-1)))
					{
						mSequenceDetect.add(currentChar);
					}
					else if(('A' == charVal)&&('D' == mSequenceDetect.val(-1)))
					{
						mSequenceDetect.add(currentChar);
					}
					else if(('T' == charVal)&&('A' == mSequenceDetect.val(-1)))
					{
						mSequenceDetect.add(currentChar);
					}
					else if(('A' == charVal)&&('T' == mSequenceDetect.val(-1)))
					{
						mSequenceDetect.add(currentChar);
					}
					else if(('[' == charVal)&&('A' == mSequenceDetect.val(-1)))
					{
						state=IN_DATA;
						mSequenceDetect.reset();
						(*mStartCdataSectionHandler)(mUserData);
					}
					// DOCTYPE
					else if(('D' == charVal)&&('!' == mSequenceDetect.val(-1)))
					{
						// TODO
						state=IN_DOCTYPE;
						mSequenceDetect.reset();
					}
				}
				break;

			case IN_COMMENT:
				{
					// wait for comment end
					if(('-'==charVal) && (mSequenceDetect.size()==0))
					{
						mSequenceDetect.init(currentChar);
					}
					else if(('-' == charVal)&&('-' == mSequenceDetect.val(-1)))
					{
						mSequenceDetect.add(currentChar);
					}
					else if('>' == charVal)
					{
						if(mSequenceDetect.size()==2)
						{
							if('-' == mSequenceDetect.val(-1))
							{
								// comment end
								state=NO_STATE;
								mSequenceDetect.reset();
							}
						}
					}
					else if(mSequenceDetect.size())
					{
						mSequenceDetect.reset();
					}
				}
				break;
			case IN_DATA:
				{
					if(mOutOfElementCharacters.myStringStart==0)
					{
						mOutOfElementCharacters.myStringStart=currentChar;
					}
					// wait for data end
					if((']'==charVal) && (mSequenceDetect.size()==0))
					{
						mSequenceDetect.init(currentChar);
					}
					else if((']' == charVal)&&(']' == mSequenceDetect.val(-1)))
					{
						mSequenceDetect.add(currentChar);
					}
					else if(('>' == charVal)&& (mSequenceDetect.size()>0) && (']' == mSequenceDetect.val(-1)))
					{
						// data end
						if(mOutOfElementCharacters.myStringStart!=0)
						{
							(*mEndCdataSectionHandler)(mUserData,(const char*)mOutOfElementCharacters.myStringStart,(int)(currentChar-mOutOfElementCharacters.myStringStart-2));
							mOutOfElementCharacters.clear();
						}
						else
						{
							(*mEndCdataSectionHandler)(mUserData,0,0);
						}
						mSequenceDetect.reset();
						state=NO_STATE;
					}
					else if(mSequenceDetect.size())
					{
						mSequenceDetect.reset();
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
						if(mElementStart.mName.myStringStart)
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
						//(*myStartElementHandler)(myUserData,(const char*)myElementStart.mName.myStringStart,attribs);
						startElement();
						mElementStart.clear(this);
						state=NO_STATE;
					}
					else if(mIsAlphaNum[charVal]&1) // space, continue
					{
					}
					else if(mIsAlphaNum[charVal]&4)
					{
						// start element attribute
						substate=ATTRIBUTE_NAME;
						currentAttribute=mElementStart.getAttrib(this);
						currentAttribute->mName.myStringStart=currentChar;
					}
				}
				break;
			case ELEMENT_START_END:
				{
					//const char** attribs=myElementStart.getAttribArray();
					//(*myStartElementHandler)(myUserData,(const char*)myElementStart.mName.myStringStart,attribs);
					startElement();
					mElementStart.clear(this);
					state=NO_STATE;
				}
				break;
			case ELEMENT_START_SELF_CLOSED:
				{
					startElement();
					//const char** attribs=myElementStart.getAttribArray();
					//(*myStartElementHandler)(myUserData,(const char*)myElementStart.mName.myStringStart,attribs);
					endElement();
					//(*myEndElementHandler)(myUserData,(const char*)myElementStart.mName.myStringStart);
					//myElementStart.clear(this);
					state=NO_STATE;
				}
				break;
			case CLOSING_ELEMENT:
				{
					if(mIsAlphaNum[charVal]&1) // space, continue
					{

					}
					else if('>'==charVal)
					{
						endElement();
						//(*myEndElementHandler)(myUserData,(const char*)myElementStart.mName.myStringStart);
						//myElementStart.clear(this);
						state=NO_STATE;
					}
					else if(mIsAlphaNum[charVal]&4)
					{
						mElementStart.mName.myStringStart=currentChar;
						substate=ELEMENT_CLOSE_NAME;
					}

				}
				break;
			case ELEMENT_CLOSING_END:
				{
					endElement();
					//(*myEndElementHandler)(myUserData,(const char*)myElementStart.mName.myStringStart);
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
	if (mUseStringRef)
	{
		StringRef** attribs = mElementStart.getStringRefAttribArray();
		(*mStartElementHandlerStringRef)(mUserData,&mElementStart.mName, attribs);

	}
	else
	{
		const char** attribs = mElementStart.getAttribArray();
		(*mStartElementHandler)(mUserData, (const char*)mElementStart.mName.myStringStart, attribs);
	}
}
void	MinimalXML::endElement()
{
	if (mUseStringRef)
	{
		(*mEndElementHandlerStringRef)(mUserData, &mElementStart.mName);
		mElementStart.clear(this);
	}
	else
	{
		(*mEndElementHandler)(mUserData, (const char*)mElementStart.mName.myStringStart);
		mElementStart.clear(this);
	}
}

void	MinimalXML::reset()
{

#ifdef  _KIGS_TOOLS
	if(myOutDebug)
		fclose(myOutDebug);
#endif
	mProlog.clear();
	mElementStart.clear(this);
}


void	MinimalXML::Prolog_Parser::clear()
{
	mAttributes[0].clear();
	mAttributes[1].clear();
	mAttributes[2].clear();
}

void	MinimalXML::ElementStart_Parser::clear(MinimalXML* parentinstance)
{
	mName.clear();
	// clear attribute
	while(mFirstAttrib)
	{
		Attribut_Parser_LinkedNode* current=mFirstAttrib;
		current->mAttribute.clear();
		mFirstAttrib=current->mNext;
		parentinstance->releaseLinkedNode(current);
	}
	mAttribCount=0;
	mCurrentAttrib=0;
	if(mAttribArray)
	{
		delete[] mAttribArray;
		mAttribArray=0;
	}
	if (mStringRefAttribArray)
	{
		delete[] mStringRefAttribArray;
		mStringRefAttribArray = 0;
	}
}

MinimalXML::Attribut_Parser*	MinimalXML::ElementStart_Parser::getAttrib(MinimalXML* parentinstance)
{
	Attribut_Parser_LinkedNode* result=parentinstance->getFreeLinkedNode();
	if(mCurrentAttrib)
	{
		mCurrentAttrib->mNext=result;
	}
	else
	{
		mFirstAttrib=result;
	}
	mCurrentAttrib=result;
	mAttribCount++;
	return &(mCurrentAttrib->mAttribute);
}

const char**		MinimalXML::ElementStart_Parser::getAttribArray()
{
	// return a pointer even if attribCount is 0
	const char**	result=0;
	
	if(mAttribCount<=8)
	{
		result=mPreallocLittleArray;
	}
	else
	{
		result=new const char*[mAttribCount*2+1];
		mAttribArray=result;
	}
	unsigned int i;
	Attribut_Parser_LinkedNode* current=mFirstAttrib;
	for(i=0;i<(mAttribCount*2);i+=2)
	{
		result[i]=(const char*)current->mAttribute.mName.myStringStart;
		result[i+1]=(const char*)current->mAttribute.mValue.myStringStart;
		current=current->mNext;
	}

	result[mAttribCount*2]=0;
	
	return result;

}


StringRef** MinimalXML::ElementStart_Parser::getStringRefAttribArray()
{
	// return a pointer even if attribCount is 0
	StringRef** result = 0;

	if (mAttribCount <= 8)
	{
		result = mPreallocStringRefLittleArray;
	}
	else
	{
		result = new StringRef * [mAttribCount * 2 + 1];
		mStringRefAttribArray = result;
	}
	unsigned int i;
	Attribut_Parser_LinkedNode* current = mFirstAttrib;
	for (i = 0; i < (mAttribCount * 2); i += 2)
	{
		result[i] = &current->mAttribute.mName;
		result[i + 1] = &current->mAttribute.mValue;
		current = current->mNext;
	}

	result[mAttribCount * 2] = 0;

	return result;

}
