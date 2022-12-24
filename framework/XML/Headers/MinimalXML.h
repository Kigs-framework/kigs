#pragma once

#include "CoreTypes.h"
#include <string>

namespace Kigs
{
	namespace Xml
	{
		class	StringRef
		{
		public:

			StringRef() : mStringStart(0), mStringEnd(0), mBackupVal(0)
			{

			}

			void	set(unsigned char* end)
			{
				mStringEnd = end;
				mBackupVal = *end;
				*mStringEnd = 0;
			}

			void	clear()
			{
				if (mStringEnd)
				{
					// restore backuped
					*mStringEnd = mBackupVal;
					mStringEnd = 0;
				}
				mStringStart = 0;
			}

			~StringRef()
			{
				clear();
			}

			unsigned char* mStringStart;
			unsigned char* mStringEnd;
			unsigned char	mBackupVal;
		};

		class stringFromRef : public std::string
		{
		public:
			stringFromRef(const StringRef& str) : std::string((const char*)str.mStringStart, (unsigned int)(str.mStringEnd - str.mStringStart)) {}
		};


		/**
				Parse an ASCII xml file from one memory block and call the given callback
				does not construct any xml structure in memory
				very permissive (no strong validity check)
				the given memory buffer can be modified by the parser during parse, but restored at the end
				last buffer character must be 0
		*/

		typedef void (*MinimalXML_DeclHandler) (void* userData,
			const char* version,
			const char* encoding,
			int             standalone);

		typedef void (*MinimalXML_StartElementHandler) (void* userData,
			const char* name,
			const char** atts);

		typedef void (*MinimalXML_EndElementHandler) (void* userData,
			const char* name);

		/* s is not 0 terminated. */
		typedef void (*MinimalXML_CharacterDataHandler) (void* userData,
			const char* s,
			int len);

		typedef void (*MinimalXML_StartCdataSectionHandler) (void* userData);
		/* s is not 0 terminated. */
		typedef void (*MinimalXML_EndCdataSectionHandler) (void* userData,
			const char* s,
			int len);

		/* 0 terminated */
		typedef void (*MinimalXML_DefaultHandler) (void* userData,
			const char* s);

		// StringRef handler

		typedef void (*MinimalXML_StartElementHandlerStringRef) (void* userData,
			StringRef* name,
			StringRef** atts);

		typedef void (*MinimalXML_EndElementHandlerStringRef) (void* userData,
			StringRef* name);


		// ****************************************
		// * MinimalXML class
		// * --------------------------------------
		/**
		 * \file	MinimalXML.h
		 * \class	MinimalXML
		 * \ingroup ModuleXML
		 * \brief	Fast minimal XML parser.
		 */
		 // ****************************************
		class	MinimalXML
		{
		public:

			//! structors
			MinimalXML();
			virtual ~MinimalXML();

			void	setUserData(void* data)
			{
				mUserData = data;
			}
#ifdef _DEBUG
			static FILE* myOutDebug;
			void	setOutputExport();
#endif

			void	setElementHandler(MinimalXML_StartElementHandler starthandler, MinimalXML_EndElementHandler endhandler)
			{
				mStartElementHandler = starthandler;
				mEndElementHandler = endhandler;
			}

			void	setElementHandlerStringRef(MinimalXML_StartElementHandlerStringRef starthandler, MinimalXML_EndElementHandlerStringRef endhandler)
			{
				mStartElementHandlerStringRef = starthandler;
				mEndElementHandlerStringRef = endhandler;
				mUseStringRef = true;
			}

			void	setCDataHandler(MinimalXML_StartCdataSectionHandler starthandler, MinimalXML_EndCdataSectionHandler endhandler)
			{
				mStartCdataSectionHandler = starthandler;
				mEndCdataSectionHandler = endhandler;
			}
			void	setXmlDeclHandler(MinimalXML_DeclHandler handler)
			{
				mDeclHandler = handler;
			}

			void	setCharacterDataHandler(MinimalXML_CharacterDataHandler handler)
			{
				mCharacterDataHandler = handler;
			}

			void	setDefaultHandler(MinimalXML_DefaultHandler handler)
			{
				mDefaultHandler = handler;
			}

			bool	Parse(u8* buffer, u64 len);


			static std::string decodeEscaped(const std::string& toDecode);

			static std::string encodeEscaped(const std::string& toEncode);

		protected:

			void	startElement();
			void	endElement();

			bool	mUseStringRef = false;

			enum	XMLState
			{
				NO_STATE = 0,
				OPENING = 1,	//	<
				CLOSING = 2, //	>
				EXECUTE_OR_PROLOG = 3,	//  ?
				COMMENT_OR_DATA_OR_DOCTYPE = 4,	//  !

				// ELEMENTS
				IN_ELEMENT = 10,
				ELEMENT_START_END = 11,
				ELEMENT_START_SELF_CLOSED = 12,
				CLOSING_ELEMENT = 13,
				ELEMENT_CLOSING_END = 14,

				// COMMENT
				IN_COMMENT = 20,

				// PROLOG
				IN_PROLOG = 30,

				// DOCTYPE
				IN_DOCTYPE = 40,

				// DATA
				IN_DATA = 50,

				// COMMAND
				IN_EXECUTE = 60,

				XML_ERROR_STATE = 100

			};

			enum XMLSubState
			{
				NO_SUBSTATE = 0,
				ELEMENT_NAME = 1,
				ELEMENT_CLOSE_NAME = 2,
				ATTRIBUTE_NAME = 3,
				ATTRIBUTE_EQUAL = 4,
				ATTRIBUTE_SEARCH_EQUAL = 5,
				ATTRIBUTE_VALUE = 6,
			};

			// if not set, use empty handlers
			static void Empty_DefaultHandler(void* userData, const char* s);
			static void Empty_DeclHandler(void* userData, const char* version, const char* encoding, int standalone);
			static void Empty_StartElementHandler(void* userData, const char* name, const char** atts);
			static void Empty_EndElementHandler(void* userData, const char* name);
			static void Empty_CharacterDataHandler(void* userData, const char* s, int len);
			static void Empty_StartCdataSectionHandler(void* userData);
			static void Empty_EndCdataSectionHandler(void* userData, const char* s, int len);

			static unsigned char	mIsAlphaNum[256];

			void	reset();

			void* mUserData;

			bool	mPrologDone;

			MinimalXML_StartElementHandler				mStartElementHandler;
			MinimalXML_EndElementHandler				mEndElementHandler;
			MinimalXML_StartCdataSectionHandler			mStartCdataSectionHandler;
			MinimalXML_EndCdataSectionHandler			mEndCdataSectionHandler;
			MinimalXML_DeclHandler						mDeclHandler;
			MinimalXML_CharacterDataHandler				mCharacterDataHandler;
			MinimalXML_DefaultHandler					mDefaultHandler;
			MinimalXML_StartElementHandlerStringRef		mStartElementHandlerStringRef;
			MinimalXML_EndElementHandlerStringRef		mEndElementHandlerStringRef;

			class	Attribut_Parser
			{
			public:

				Attribut_Parser()
				{}

				void clear()
				{
					mName.clear();
					mValue.clear();
				}


				StringRef	mName;
				StringRef	mValue;
			};

			class	Attribut_Parser_LinkedNode
			{
			public:
				Attribut_Parser_LinkedNode()
					:mNext(0)
				{

				}

			public:
				Attribut_Parser				mAttribute;
				Attribut_Parser_LinkedNode* mNext;
			};

			class	SequenceDetect
			{
			public:

				void	init(unsigned	char* first)
				{
					mCurrentIndex = 0;
					add(first);
				}

				void	add(unsigned	char* next)
				{
					mSequence[mCurrentIndex] = next;
					++mCurrentIndex;
				}

				const unsigned char& val(int relativeIndex)
				{
					return *mSequence[mCurrentIndex + relativeIndex];
				}

				void	reset()
				{
					mCurrentIndex = 0;
				}

				unsigned int size()
				{
					return mCurrentIndex;
				}

				unsigned	char* mSequence[16];
				unsigned	int		mCurrentIndex;
			};

			class	Prolog_Parser
			{
			public:
				Prolog_Parser() :mCurrentAttribute(0)
				{

				}
				void	clear();
				Attribut_Parser		mAttributes[3]; // version, encoding, standalone
				int					mCurrentAttribute;
			};

			class	ElementStart_Parser
			{
			public:

				ElementStart_Parser()
					:mFirstAttrib(0)
					, mCurrentAttrib(0)
					, mAttribCount(0)
					, mAttribArray(0)
					, mStringRefAttribArray(0)
				{

				}

				void				clear(MinimalXML* parentinstance);
				StringRef			mName;
				Attribut_Parser* getAttrib(MinimalXML* parentinstance);
				const char** getAttribArray();
				StringRef** getStringRefAttribArray();

				Attribut_Parser_LinkedNode* mFirstAttrib;
				Attribut_Parser_LinkedNode* mCurrentAttrib;
				unsigned int				mAttribCount;

			protected:
				const char** mAttribArray;
				StringRef** mStringRefAttribArray;
				const char* mPreallocLittleArray[17];
				StringRef* mPreallocStringRefLittleArray[17];

			};

			Prolog_Parser					mProlog;
			SequenceDetect					mSequenceDetect;
			ElementStart_Parser				mElementStart;
			StringRef						mOutOfElementCharacters;

			// manage prealloc linked nodes

			void	InitPreallocated()
			{
				mPreallocatedStart = (uptr)(&mPreallocatedLinkedNodes[0]);
				mPreallocatedEnd = (uptr)(&mPreallocatedLinkedNodes[31]);
				mPreallocatedLinkedNodesFlag = 0;
			}

			friend class ElementStart_Parser;

			Attribut_Parser_LinkedNode* getFreeLinkedNode()
			{
				if (mPreallocatedLinkedNodesFlag == 0xFFFFFFFF)
				{
					return new Attribut_Parser_LinkedNode();
				}

				unsigned int index = 0;
				while (index < 32)
				{
					if (!(mPreallocatedLinkedNodesFlag & (1 << index)))
					{
						mPreallocatedLinkedNodes[index].mNext = 0;
						mPreallocatedLinkedNodesFlag |= (1 << index);
						return &mPreallocatedLinkedNodes[index];
					}
					index++;
				}

				// should never be there !
				return 0;
			}

			void	releaseLinkedNode(Attribut_Parser_LinkedNode* node)
			{
				// check if preallocated
				uptr nodeAddr = (uptr)node;
				if ((nodeAddr < mPreallocatedStart) || (nodeAddr > mPreallocatedEnd))
				{
					delete node;
					return;
				}

				uptr index = nodeAddr - mPreallocatedStart;
				index /= sizeof(Attribut_Parser_LinkedNode);
				mPreallocatedLinkedNodes[index].mAttribute.clear();
				mPreallocatedLinkedNodesFlag ^= (1 << index);
			}

			unsigned int					mPreallocatedLinkedNodesFlag;
			Attribut_Parser_LinkedNode		mPreallocatedLinkedNodes[32];
			uptr							mPreallocatedStart;
			uptr							mPreallocatedEnd;

		};
	}

}