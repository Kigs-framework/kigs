#include "PrecompiledHeaders.h"
#include "LocalizationManager.h"
#include "FilePathManager.h"
#include "ModuleFileManager.h"
#include "JSonFileParser.h"



// ************************************************************
//
// LocalizationManager
//
// ************************************************************

//IMPLEMENT_AND_REGISTER_CLASS_INFO(LocalizationManager, LocalizationManager, StandAlone);
IMPLEMENT_CLASS_INFO(LocalizationManager)

LocalizationManager::LocalizationManager(const kstl::string& name,CLASS_NAME_TREE_ARG) : 
CoreModifiable(name, PASS_CLASS_NAME_TREE_ARG)
{
}

LocalizationManager::~LocalizationManager()
{
	EraseMap();

	mWbuffer[0]=0;
}

void LocalizationManager::setLocalizationFilePath(const char* path,bool a_erasePreviousLoc)
{
	if(a_erasePreviousLoc)
	{
		EraseMap();
	}
	mLocalizationFilePath=path;
	if(mLocalizationFilePath != "")
	{
		ParseStringsFile(mLocalizationFilePath.c_str());
	}
}

void LocalizationManager::InitWithConfigFile(const kstl::string& filename)
{
	JSonFileParser L_JsonParser;
	CoreItemSP L_Dictionary = L_JsonParser.Get_JsonDictionary(filename);

	if (!L_Dictionary.isNil())
	{
		// init map
		kstl::map<kstl::string, kstl::string>	langageMap;

		CoreItemSP pathList = L_Dictionary;
		int nbpath = pathList->size();
		int i;
		for (i = 0; i < nbpath; i += 2)
		{
			langageMap[(kstl::string)(CoreItem&)pathList[i]] = (kstl::string)(CoreItem&)pathList[i + 1];;
		}

		kstl::string	L_Lang = LocalizationManager::getCurrentUserLanguage();
		kstl::map<kstl::string, kstl::string>::const_iterator	langFound = langageMap.find(L_Lang);
		if (langFound != langageMap.end())
		{
			setLocalizationFilePath((*langFound).second.c_str());
			return;
		}
		
		// is there a default ?
		langFound = langageMap.find("default");
		if (langFound != langageMap.end())
		{
			// so set default
			langFound = langageMap.find((*langFound).second);
			if (langFound != langageMap.end())
			{
				setLocalizationFilePath((*langFound).second.c_str());
				return;
			}
		}
	}
	KIGS_WARNING("Can not found valid langage file for localization manager", 2);

}

void LocalizationManager::addLocalizationFromFileAtPath(const char* path)
{
	if(((kstl::string)path) != "")
	{
		ParseStringsFile(path);
	}
}

void LocalizationManager::addLocalizationFromBuffer(char* Buffer, unsigned int bufferSize)
{
	if(Buffer)
	{
		ParseBuffer(Buffer,bufferSize);
	}
}

const PLATFORM_WCHAR*	LocalizationManager::getLocalizedString(const kstl::string& key) const
{
	kstl::map<const kstl::string,PLATFORM_WCHAR* >::const_iterator itfound=mLocalizedString.find(key);
	if(itfound != mLocalizedString.end())
	{
		return (*itfound).second;
	}
#ifdef _DEBUG
	printf("Localization not found for key : %s\n",key.c_str());
#endif
	return 0;
}

// Strings Parser

bool	LocalizationManager::ParseStringsFile(const char* pszFile)
{
	auto&	pathManager=KigsCore::Singleton<FilePathManager>();

	kstl::string fullfilename=pszFile;
	SmartPointer<FileHandle> fullfilenamehandle;

	if (pathManager)
	{
		fullfilenamehandle = pathManager->FindFullName(pszFile);
	}
	u64 size;
	CoreRawBuffer* pBuffer=ModuleFileManager::LoadFileAsCharString(fullfilenamehandle.get(),size,2);

	if(!pBuffer)
	{
		return false;
	}

	ParseBuffer(pBuffer->buffer(),size);
	
	pBuffer->Destroy();
	return true;
}

void LocalizationManager::ParseBuffer(char* _pBuffer, unsigned long size)
{
	bool	finished=false;
	unsigned long	currentpos=0;

	while(!finished)
	{
		kstl::string	key="";
		
		PLATFORM_WCHAR*	localized=GetNextLocalizedString(_pBuffer,currentpos,key,size);

		if(localized)
		{
			// check if not already allocated
			kstl::map<const kstl::string,PLATFORM_WCHAR* >::const_iterator itfound=mLocalizedString.find(key);
			if(itfound != mLocalizedString.end())
			{
				delete[] (*itfound).second;
			}

			mLocalizedString[key]=localized;
		}
		else
		{
			
			finished=true;
		}
	}
}

PLATFORM_WCHAR*	LocalizationManager::GetNextLocalizedString(char* pBuffer,unsigned long& currentpos,kstl::string& key,unsigned long filelen)
{
	PLATFORM_WCHAR* startchar=(PLATFORM_WCHAR*)&pBuffer[currentpos];

	bool keyStartFound=false;

	while(!keyStartFound)
	{
		if((*startchar) == (PLATFORM_WCHAR)'"')
		{
			keyStartFound=ExtractQuoted(startchar,key,currentpos,filelen,true);
		}
		else if((*startchar) == (PLATFORM_WCHAR)'/')
		{
			ExtractComment(startchar,currentpos,filelen);
		}
		else
		{
			startchar++;
			currentpos+=sizeof(PLATFORM_WCHAR);

			// end of file
			if(currentpos>=filelen)
			{
				return 0;
			}
		}
	}

	// search next quoted 
	keyStartFound=false;
	kstl::string localizedstring="";

	while(!keyStartFound)
	{
		if((*startchar) == (PLATFORM_WCHAR)'"')
		{
			keyStartFound=ExtractQuoted(startchar,&mWbuffer[0],currentpos,filelen,false);
		}
		else if((*startchar) == (PLATFORM_WCHAR)'/')
		{
			ExtractComment(startchar,currentpos,filelen);
		}
		else
		{
			startchar++;
			currentpos+=sizeof(PLATFORM_WCHAR);

			// end of file
			if(currentpos>=filelen)
			{
				return 0;
			}
		}
	}

	// compute mWbuffer len
	int i=0;
	while(mWbuffer[i] != 0)
	{
		i++;
	}

	PLATFORM_WCHAR* tempBuffer=new PLATFORM_WCHAR[i+1];

	// copy buffer
	i=0;
	while(mWbuffer[i] != 0)
	{
		tempBuffer[i]=mWbuffer[i];
		i++;
	}
	tempBuffer[i]=0;

	return tempBuffer;

}

bool	LocalizationManager::ExtractQuoted(PLATFORM_WCHAR*& startchar,kstl::string& quoted,unsigned long& currentpos,unsigned long filelen,bool isKey)
{
	bool keyEndFound=false;
	startchar++;
	currentpos+=sizeof(PLATFORM_WCHAR);
	if(currentpos<filelen)
	{
		while(!keyEndFound)
		{
			if((*startchar) == '"')
			{
				if(isKey)
				{
					keyEndFound=true;
					startchar++;
					currentpos+=sizeof(PLATFORM_WCHAR);
				}
				else // not a key, check if next char is a ';'
				{
					if((startchar[1]) == ';')
					{
						startchar++;
						currentpos+=sizeof(PLATFORM_WCHAR);
						keyEndFound=true;
						startchar++;
						currentpos+=sizeof(PLATFORM_WCHAR);
					}
				}
			}

			if(!keyEndFound)
			{
				quoted+=(char)startchar[0];
				startchar++;
				currentpos+=sizeof(PLATFORM_WCHAR);

				// end of file
				if(currentpos>=filelen)
				{
					quoted="";
					return false;
				}
			}
		}

		if(!keyEndFound)
		{
			quoted="";
		}
	}

	return keyEndFound;
}

bool	LocalizationManager::ExtractQuoted(PLATFORM_WCHAR*& startchar,unsigned short* buffer,unsigned long& currentpos,unsigned long filelen,bool isKey)
{
	bool keyEndFound=false;
	startchar++;
	currentpos+=sizeof(PLATFORM_WCHAR);
	int currentindex=0;
	if(currentpos<filelen)
	{
		while(!keyEndFound)
		{
			if((*startchar) == '"')
			{
				if(isKey)
				{
					keyEndFound=true;
					startchar++;
					currentpos+=sizeof(PLATFORM_WCHAR);
				}
				else // not a key, check if next char is a ';'
				{
					if((startchar[1]) == ';')
					{
						startchar++;
						currentpos+=sizeof(PLATFORM_WCHAR);
						keyEndFound=true;
						startchar++;
						currentpos+=sizeof(PLATFORM_WCHAR);
					}
				}
			}

			if(!keyEndFound)
			{
				// remove escape characters
				if(startchar[0] != '\\')
				{
					buffer[currentindex++]=startchar[0];
				}
				startchar++;
				currentpos+=sizeof(PLATFORM_WCHAR);

				// end of file
				if(currentpos>=filelen)
				{
					buffer[0]=0;
					return false;
				}
			}
		}

		if(!keyEndFound)
		{
			buffer[0]=0;
		}
		else
		{
			buffer[currentindex++]=0;
		}
	}

	return keyEndFound;
}

void	LocalizationManager::ExtractComment(PLATFORM_WCHAR*& startchar,unsigned long& currentpos,unsigned long filelen)
{
	startchar++;
	currentpos+=sizeof(PLATFORM_WCHAR);
	if(currentpos<filelen)
	{
		if((*startchar) == '/')	// single line comment 
		{
			GotoNextLine(startchar,currentpos,filelen);
		}
		else if((*startchar) == '*') // multi line comment
		{
			GotoCommentEnd(startchar,currentpos,filelen);
		}
		else
		{
			return;	// syntax error
		}
	}
}

void	LocalizationManager::GotoNextLine(PLATFORM_WCHAR*& startchar,unsigned long& currentpos,unsigned long filelen)
{
	startchar++;
	currentpos+=sizeof(PLATFORM_WCHAR);
	while(currentpos<filelen)
	{
		if( ((*startchar) == '\n' ) ||  ((*startchar) == '\r' ) )
		{
			return;
		}
		startchar++;
		currentpos+=sizeof(PLATFORM_WCHAR);
	}
}
void	LocalizationManager::GotoCommentEnd(PLATFORM_WCHAR*& startchar,unsigned long& currentpos,unsigned long filelen)
{
	startchar++;
	currentpos+=sizeof(PLATFORM_WCHAR);
	while(currentpos<(filelen-1))
	{
		if( ((startchar[0]) == '*' ) &&  ((startchar[1]) == '/' ) )
		{
			startchar++;
			currentpos+=sizeof(PLATFORM_WCHAR);
			return;
		}
		startchar++;
		currentpos+=sizeof(PLATFORM_WCHAR);
	}
}

void	LocalizationManager::EraseMap()
{
	kstl::map<const kstl::string,PLATFORM_WCHAR* >::const_iterator it=mLocalizedString.begin();
	while(it!=mLocalizedString.end())
	{
		PLATFORM_WCHAR* currentString=(*it).second;
		if(currentString)
		{
			delete[] currentString;
		}
		++it;
	}
	mLocalizedString.clear();
}

#include "Platform/LocalizationManager/LocalizationManager.inl.h"