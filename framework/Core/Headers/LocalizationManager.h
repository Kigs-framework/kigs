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
		PLATFORM_WCHAR* mUTF16 = nullptr;
		std::string mUTF8;
	};

	DECLARE_CLASS_INFO(LocalizationManager,CoreModifiable,LocalizationManager);
	
	LocalizationManager(const std::string& name,DECLARE_CLASS_NAME_TREE_ARG);

	virtual void	setLocalizationFilePath(const char* path,bool a_erasePreviousLoc=false);

	virtual const PLATFORM_WCHAR* getLocalizedString(const std::string& key);
	virtual const std::string& getLocalizedStringUTF8(const std::string& key);

	void			addLocalizationFromFileAtPath(const char* path);

	template<typename charType>
	void			addLocalizationFromBuffer(char* _buffer, unsigned int bufferSize);
	static std::string		getCurrentUserLanguage();
	inline std::string		getLocalizationFilePath(){ return mLocalizationFilePath; }

	void			InitWithConfigFile(const std::string& filename);

	void SetLocalizationMap(std::map<const std::string, DoubleLocalizedUTF8UTF16> new_map)
	{
		EraseMap();
		mLocalizedString = std::move(new_map);
	}

	std::map<const std::string, DoubleLocalizedUTF8UTF16 > getDebugFullMap()
	{
		return mLocalizedString;
	}
	virtual				~LocalizationManager();

protected:
	
	void				EraseMap();

	bool				ParseStringsFile(const char*);
	template<typename charType>
	charType*			GetNextLocalizedString(char* pBuffer,unsigned long& currentpos,std::string& key,unsigned long filelen);
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
	std::string										mLocalizationFilePath;


	std::map<const std::string, DoubleLocalizedUTF8UTF16 >		mLocalizedString;

	maFloat mLanguageScale = BASE_ATTRIBUTE(LanguageScale, 1.0f);
};


#endif //__LOCALIZATIONMANAGER_H__
