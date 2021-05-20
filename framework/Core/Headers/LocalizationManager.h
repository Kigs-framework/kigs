#ifndef __LOCALIZATIONMANAGER_H__
#define __LOCALIZATIONMANAGER_H__

#include "Core.h"
#include "CoreModifiable.h"

#define  PLATFORM_WCHAR unsigned short

enum GAMELANGUAGE
{
	ENGLISH = 0,
	FRENCH,
	DEUTCH,
	CATALAN,
	CROATE,
};

// ****************************************
// * LocalizationManager class
// * --------------------------------------
/**
 * \class	LocalizationManager
 * \file	LocalizationManager.h
 * \ingroup Core
 * \brief	Manage languages and localization files
 */
 // ****************************************


class LocalizationManager : public CoreModifiable
{
public:
	struct DoubleLocalizedUTF8UTF16
	{
		PLATFORM_WCHAR* mUTF16;
		UTF8Char* mUTF8;
	};

	DECLARE_CLASS_INFO(LocalizationManager,CoreModifiable,LocalizationManager);
	
	LocalizationManager(const kstl::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	virtual void	setLocalizationFilePath(const char* path,bool a_erasePreviousLoc=false);

	virtual const PLATFORM_WCHAR*	getLocalizedString(const kstl::string& key);
	virtual const UTF8Char* getLocalizedStringUTF8(const kstl::string& key);

	void			addLocalizationFromFileAtPath(const char* path);

	template<typename charType>
	void			addLocalizationFromBuffer(char* _buffer, unsigned int bufferSize);
	static kstl::string		getCurrentUserLanguage();
	inline kstl::string		getLocalizationFilePath(){ return mLocalizationFilePath; }

	void			InitWithConfigFile(const kstl::string& filename);

	kstl::map<const kstl::string, DoubleLocalizedUTF8UTF16 > getDebugFullMap()
	{
		return mLocalizedString;
	}
	virtual				~LocalizationManager();

protected:
	
	void				EraseMap();

	bool				ParseStringsFile(const char*);
	template<typename charType>
	charType*			GetNextLocalizedString(char* pBuffer,unsigned long& currentpos,kstl::string& key,unsigned long filelen);
	template<typename charType>
	bool				ExtractQuoted(charType*& startchar, std::vector<charType>& buffer, unsigned long& currentpos, unsigned long filelen);
	template<typename charType>
	void				ExtractComment(charType*& startchar,unsigned long& currentpos,unsigned long filelen);
	template<typename charType>
	void				GotoNextLine(charType*& startchar,unsigned long& currentpos,unsigned long filelen);
	template<typename charType>
	void				GotoCommentEnd(charType*& startchar,unsigned long& currentpos,unsigned long filelen);
	template<typename charType>
	void				ParseBuffer(char* _pBuffer, unsigned long size);
	kstl::string										mLocalizationFilePath;


	kstl::map<const kstl::string, DoubleLocalizedUTF8UTF16 >		mLocalizedString;

	maFloat mLanguageScale = BASE_ATTRIBUTE(LanguageScale, 1.0f);
};


#endif //__LOCALIZATIONMANAGER_H__
