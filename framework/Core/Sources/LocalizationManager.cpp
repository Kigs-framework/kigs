#include "PrecompiledHeaders.h"
#include "LocalizationManager.h"
#include "FilePathManager.h"
#include "ModuleFileManager.h"
#include "JSonFileParser.h"
#include "utf8.h"
#include "utf8_decode.h"


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

	if (L_Dictionary)
	{
		// init map
		kstl::map<kstl::string, kstl::string>	langageMap;

		CoreItemSP pathList = L_Dictionary;
		int nbpath = (int)pathList->size();
		int i;
		for (i = 0; i < nbpath; i += 2)
		{
			langageMap[(kstl::string)(CoreItem&)(*pathList)[i]] = (kstl::string)(CoreItem&)(*pathList)[i + 1];;
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

template<typename charType>
void LocalizationManager::addLocalizationFromBuffer(char* Buffer, unsigned int bufferSize)
{
	if(Buffer)
	{
		ParseBuffer<charType>(Buffer,bufferSize);
	}
}

const std::string& LocalizationManager::getLocalizedStringUTF8(const kstl::string& key)
{
	kstl::map<const kstl::string, DoubleLocalizedUTF8UTF16 >::iterator itfound = mLocalizedString.find(key);

	if (itfound != mLocalizedString.end())
	{
		// if no utf8, then create it
		auto wchar_str = (wchar_t*)(*itfound).second.mUTF16;
		if ((*itfound).second.mUTF8.empty() && wchar_str)
		{
			(*itfound).second.mUTF8 = to_utf8(wchar_str, wcslen(wchar_str));
			/*
			usString				tmpOne((*itfound).second.mUTF16);
			std::vector<UTF8Char>	tmpUTF8=tmpOne.toUTF8();
			int l = (int)tmpUTF8.size();
			UTF8Char* tempBuffer = nullptr;
			if (l)
			{
				tempBuffer = new UTF8Char[l + 1];
				memcpy(tempBuffer, tmpUTF8.data(), l * sizeof(UTF8Char));
				tempBuffer[l] = 0;
				(*itfound).second.mUTF8 = tempBuffer;
			}*/
		}
		return (*itfound).second.mUTF8;
	}
#ifdef _DEBUG
	printf("Localization not found for key : %s\n", key.c_str());
#endif
	return "";
}


const PLATFORM_WCHAR*	LocalizationManager::getLocalizedString(const kstl::string& key)
{
	kstl::map<const kstl::string, DoubleLocalizedUTF8UTF16 >::iterator itfound=mLocalizedString.find(key);

	if(itfound != mLocalizedString.end())
	{
		// if no utf16, then create it
		if (!(*itfound).second.mUTF16)
		{
			auto wstr = to_wchar((*itfound).second.mUTF8);
			if (wstr.size())
			{
				PLATFORM_WCHAR* tempBuffer = new PLATFORM_WCHAR[wstr.size()+1];
				memcpy(tempBuffer, wstr.c_str(), wstr.size() * sizeof(PLATFORM_WCHAR));
				tempBuffer[wstr.size()] = 0;
				(*itfound).second.mUTF16 = tempBuffer;
			}
			/*usString			tmpOne((*itfound).second.mUTF8);
			PLATFORM_WCHAR*		tempBuffer = nullptr;
			int l = tmpOne.length();
			if (l)
			{
				tempBuffer = new PLATFORM_WCHAR[l + 1];
				memcpy(tempBuffer, tmpOne.us_str(), l * sizeof(PLATFORM_WCHAR));
				
				tempBuffer[l] = 0;
				(*itfound).second.mUTF16 = tempBuffer;
			}*/
		}
		return (*itfound).second.mUTF16;
	}
#ifdef _DEBUG
	printf("Localization not found for key : %s\n",key.c_str());
#endif
	return 0;
}

// Strings Parser

bool	LocalizationManager::ParseStringsFile(const char* pszFile)
{
	auto pathManager=KigsCore::Singleton<FilePathManager>();

	kstl::string fullfilename=pszFile;
	SmartPointer<FileHandle> fullfilenamehandle;

	if (pathManager)
	{
		fullfilenamehandle = pathManager->FindFullName(pszFile);
	}

	u64 size;
	auto pBuffer = ModuleFileManager::LoadFileAsCharString(fullfilenamehandle.get(), size, 0);
	if(!pBuffer)
	{
		return false;
	}

	// check file format (UTF16 or UTF8)
	unsigned char* bufferStart = (unsigned char*)pBuffer->buffer();
	if ((bufferStart[0] == 0xff) && (bufferStart[1] == 0xfe)) // utf16 bom
	{
		ParseBuffer<unsigned short>(pBuffer->buffer()+2, size);
	}
	else
	{
		// utf8
		ParseBuffer<UTF8Char>(pBuffer->buffer(), size);
	}
	return true;
}

template<typename charType>
void LocalizationManager::ParseBuffer(char* _pBuffer, unsigned long size)
{
	bool	finished=false;
	unsigned long	currentpos=0;

	while(!finished)
	{
		kstl::string	key="";
		
		charType*	localized=GetNextLocalizedString<charType>(_pBuffer,currentpos,key,size);

		if(localized)
		{
			// check if not already allocated
			kstl::map<const kstl::string, DoubleLocalizedUTF8UTF16 >::const_iterator itfound=mLocalizedString.find(key);
			
			if(itfound != mLocalizedString.end())
			{
				if((*itfound).second.mUTF16)
					delete[] (*itfound).second.mUTF16;
				/*if ((*itfound).second.mUTF8)
					delete[](*itfound).second.mUTF8;*/
			}
			if (sizeof(charType) == 2)
			{
				mLocalizedString[key].mUTF16 = (unsigned short*)localized;
				mLocalizedString[key].mUTF8.clear();
			}
			else
			{
				mLocalizedString[key].mUTF16 = nullptr;
				mLocalizedString[key].mUTF8 = (char*)localized;
			}
		}
		else
		{
			
			finished=true;
		}
	}
}

template<typename charType>
charType*	LocalizationManager::GetNextLocalizedString(char* pBuffer,unsigned long& currentpos,kstl::string& key,unsigned long filelen)
{

	std::vector<charType>	retreiveQuoted;
	charType* startchar=(charType*)&pBuffer[currentpos];

	bool keyStartFound=false;

	while(!keyStartFound)
	{
		if((*startchar) == (charType)'"')
		{
			keyStartFound=ExtractQuoted(startchar, retreiveQuoted,currentpos,filelen);
		}
		else if((*startchar) == (charType)'/')
		{
			ExtractComment(startchar,currentpos,filelen);
		}
		else
		{
			startchar++;
			currentpos+=sizeof(charType);

			// end of file
			if(currentpos>=filelen)
			{
				return 0;
			}
		}
	}

	if (keyStartFound)
	{
		key = "";
		for (auto c : retreiveQuoted)
		{
			key += c;
		}
	}
	retreiveQuoted.clear();
	// search next quoted 
	keyStartFound=false;
	kstl::string localizedstring="";

	while(!keyStartFound)
	{
		if((*startchar) == (charType)'"')
		{
			keyStartFound=ExtractQuoted(startchar, retreiveQuoted,currentpos,filelen);
		}
		else if((*startchar) == (charType)'/')
		{
			ExtractComment(startchar,currentpos,filelen);
		}
		else
		{
			startchar++;
			currentpos+=sizeof(charType);

			// end of file
			if(currentpos>=filelen)
			{
				return 0;
			}
		}
	}
	charType* tempBuffer = nullptr;
	if (keyStartFound)
	{

		tempBuffer = new charType[retreiveQuoted.size()+1];
		int i = 0;
		for (auto c : retreiveQuoted)
		{
			tempBuffer[i] = c;
			i++;
		}
		tempBuffer[i] = 0;
		
	}
	return tempBuffer;

}

template<typename charType>
bool	LocalizationManager::ExtractQuoted(charType*& startchar,std::vector<charType>& buffer,unsigned long& currentpos,unsigned long filelen)
{
	bool keyEndFound=false;
	startchar++;
	currentpos+=sizeof(charType);

	if(currentpos<filelen)
	{
		while(!keyEndFound)
		{
			if((*startchar) == (charType)'"')
			{
				keyEndFound = true;
				startchar++;
				currentpos += sizeof(charType);

				// end of localized string
				if ((startchar[0]) == ';')
				{
					startchar++;
					currentpos += sizeof(charType);
				}
			}

			if(!keyEndFound)
			{
				// remove escape characters
				if(startchar[0] != (charType)'\\')
				{
					buffer.push_back(startchar[0]);
				}
				startchar++;
				currentpos+=sizeof(charType);

				// end of file
				if(currentpos>=filelen)
				{
					buffer.clear();
					return false;
				}
			}
		}

		if(!keyEndFound)
		{
			buffer.clear();
		}
	}

	return keyEndFound;
}

template<typename charType>
void	LocalizationManager::ExtractComment(charType*& startchar,unsigned long& currentpos,unsigned long filelen)
{
	startchar++;
	currentpos+=sizeof(charType);
	if(currentpos<filelen)
	{
		if((*startchar) == (charType)'/')	// single line comment 
		{
			GotoNextLine(startchar,currentpos,filelen);
		}
		else if((*startchar) == (charType)'*') // multi line comment
		{
			GotoCommentEnd(startchar,currentpos,filelen);
		}
		else
		{
			return;	// syntax error
		}
	}
}
template<typename charType>
void	LocalizationManager::GotoNextLine(charType*& startchar,unsigned long& currentpos,unsigned long filelen)
{
	startchar++;
	currentpos+=sizeof(charType);
	while(currentpos<filelen)
	{
		if( ((*startchar) == (charType)'\n' ) ||  ((*startchar) == (charType)'\r' ) )
		{
			return;
		}
		startchar++;
		currentpos+=sizeof(charType);
	}
}

template<typename charType>
void	LocalizationManager::GotoCommentEnd(charType*& startchar,unsigned long& currentpos,unsigned long filelen)
{
	startchar++;
	currentpos+=sizeof(charType);
	while(currentpos<(filelen-1))
	{
		if( ((startchar[0]) == (charType)'*' ) &&  ((startchar[1]) == (charType)'/' ) )
		{
			startchar++;
			currentpos+=sizeof(charType);
			return;
		}
		startchar++;
		currentpos+=sizeof(charType);
	}
}

void	LocalizationManager::EraseMap()
{
	kstl::map<const kstl::string, DoubleLocalizedUTF8UTF16 >::const_iterator it=mLocalizedString.begin();
	while(it!=mLocalizedString.end())
	{
		PLATFORM_WCHAR* currentString=(*it).second.mUTF16;
		if(currentString)
		{
			delete[] currentString;
		}
		/*UTF8Char* currentUTF8String = (*it).second.mUTF8;
		if (currentUTF8String)
		{
			delete[] currentUTF8String;
		}*/
		++it;
	}
	mLocalizedString.clear();
}

#include "Platform/LocalizationManager/LocalizationManager.inl.h"